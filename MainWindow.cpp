#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QTableWidgetItem>
#include <QHBoxLayout>
#include <QTimer>
#include <QScrollBar>
#include "CommonUiControllers/TableButton.h"
#include <algorithm>
#include <iterator>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mCameraId(1),
    mRepository(new OperationRepository(mPath ,this))
{
    ui->setupUi(this);
    ui->graphicsViewCurrentRoom->setScene(mScene);
    ui->messageWidget->setVisible(false);  

    setRoomsListTableWidgetOptions();
    subscribeToFormEvents();
    setUiElementsState(false,false,false,
                       false,false,false,
                       false,false,false,false);

    try
    {
        OutputRoomList(mRepository->GetAllRooms());
    }
    catch(const std::exception& e)
    {
        qDebug() <<e.what();
    }
}

MainWindow::~MainWindow()
{
    mLampList.clear();
    ui->tblViewRooms->clear();    
    delete mScene;
    delete ui;
}

void MainWindow::createRoom()
{
    deleteRoom();    

    if(isSceneExist())
    {
        createScene();
        setRoomParameters();

        outputMessage(mCreateRoomMessage);

        setUiElementsState(true, false, true,
                           true, false, false,
                           true, true, false, false);
    }    
}

void MainWindow::deleteRoom()
{
    if(!isSceneExist())
    {
        deleteScene();
        clearContainers();

        mCurrentCameraId = 0;        
    }
}

void MainWindow::clearRoom()
{    
    mScene->clear();
    clearContainers();

    mCurrentCameraId = 0;
}

void MainWindow::createLamp()
{
    Lamp *lamp = createNewLamp();
    mCameraId++;

    mLampList.append(lamp);
    addLampToUndoStackAndClearRedoStack(*lamp->createMemento());
    mScene->addItem(lamp);

    setUndoRedoButtonsState(true, true);
}

void MainWindow::deleteLamp(int id)
{
    if(isLampIdValid(id))
    {
        for(int i = 0; i < mLampList.size(); ++i)
        {
            if(isFindLampExist(mLampList.at(i)->lampId(), id))
            {
                mLampList.at(i)->setLampIsDeleted(true);

                addLampToUndoStackAndClearRedoStack(*mLampList.at(i)->createMemento());

                mScene->removeItem(mLampList.at(i));
                mLampList.removeAt(i);
                mCurrentCameraId = 0;
                break;
            }
        }
    }

    setColorAndDeleteLampButtonsState(false, false);
}

void MainWindow::setColorForCurrentLampLight(int id)
{
    if(isLampIdValid(id))
    {
        Lamp *lamp;
        for(int i = 0; i < mLampList.size(); ++i)
        {
            if(isFindLampExist(mLampList.at(i)->lampId(), id))
            {
                lamp = mLampList.at(i);
                QColor color = QColorDialog::getColor(Qt::white, this, "Choose color");
                if(color.isValid())
                {
                    lamp->lampLight()->setLampLightColor(color);
                    update();

                    addLampToUndoStackAndClearRedoStack(*lamp->createMemento());
                }
                break;
            }
        }
    }
}

void MainWindow::setCurrentCameraId(int id)
{
    mCurrentCameraId = id;
    setColorAndDeleteLampButtonsState(true, true);
}

void MainWindow::saveRoom()
{
    ui->btnSaveRoom->setEnabled(false);
    QJsonObject roomObject;
    write(roomObject);
    try
    {
        mRepository->SaveRoom(roomObject);
    }
    catch(const std::exception& e)
    {
        qDebug() <<e.what();
    }

    OutputRoomList(mRepository->GetAllRooms());
    //ui->tblViewRooms->

    setUpdateAndSaveRoomButtonsState(true, true);
    outputMessage(mSaveRoomMesssage);
}

void MainWindow::loadRoom(int row, int)
{
    deleteRoom();
    createRoom();

    mCurrentRoomId = ui->tblViewRooms->item(row,1)->text().toInt();
    try
    {
        restorRoom(read(mRepository->GetCurrentRoom(mCurrentRoomId)));
    }
    catch(const std::exception& e)
    {
        qDebug() <<e.what();
    }

    setUpdateRoomAndUndoRedoButtonsState(true, true, true);
    outputMessage(mLoadRoomMessage);
}

void MainWindow::deleteRoomFromeDb(int id)
{    
    if(isRoomIdValid(id))
    {
        deleteRoom();
        setUiElementsState(false,false,false,
                           false,false,false,
                           false,false,false,false);

    }

    try
    {
        mRepository->DeleteRoom(id);
    }
    catch(const std::exception& e)
    {
        qDebug() <<e.what();
    }

    OutputRoomList(mRepository->GetAllRooms());

    outputMessage(mDeleteRoomMessage);
}

void MainWindow::updateRoom()
{
    ui->btnUpdateRoom->setEnabled(false);
    QJsonObject roomObject;
    write(roomObject);

    try
    {
        mRepository->UpdateRoom(roomObject, mCurrentRoomId);
    }
    catch(const std::exception& e)
    {
        qDebug() <<e.what();
    }

    OutputRoomList(mRepository->GetAllRooms());

    outputMessage(mUpdateRoomMessage);
    ui->btnUpdateRoom->setEnabled(true);
}

void MainWindow::zoomIn()
{
    ui->graphicsViewCurrentRoom->scale(2,2);
}

void MainWindow::zoomOut()
{
    ui->graphicsViewCurrentRoom->scale(.5,.5);
}

void MainWindow::undo()
{
    int idForLoadStack;
    if(!mUndoStack.isEmpty())
    {
        Memento lastOperation = getLastOperation(mUndoStack, mRedoStack);

        idForLoadStack = lastOperation.id();

        auto lampToRemove = std::find_if(mLampList.rbegin(), mLampList.rend(),
                                         [&lastOperation](Lamp *l){ return l->lampId() == lastOperation.id();});

        if(isFindLampExist(lampToRemove))
        {
            removeLampFromScene(lampToRemove);
        }

        QStack<Memento>::reverse_iterator lampToUpdate;
        for(lampToUpdate = mUndoStack.rbegin(); lampToUpdate!=mUndoStack.rend(); ++lampToUpdate)
        {
            if(isFindLampExist(lampToUpdate->id(), idForLoadStack))
            {
                setLampToPreviousState(lampToUpdate, false);
                break;
            }
        }

        if (isFindLampNotExist(lampToUpdate) || (!mLoadStack.isEmpty() && mUndoStack.isEmpty()))
        {
            for(auto loadlamp = mLoadStack.begin(); loadlamp!= mLoadStack.end(); ++loadlamp)
            {
                if(isFindLampExist(loadlamp->id(), idForLoadStack))
                {
                    setLampToPreviousState(loadlamp, true);
                    break;
                }
            }
        }
    }
}

void MainWindow::redo()
{
    if(!mRedoStack.isEmpty())
    {
        Memento lastOperation = getLastOperation(mRedoStack, mUndoStack);

        auto findLamp = std::find_if(mLampList.begin(), mLampList.end(),
                                     [&lastOperation](Lamp *l){ return l->lampId() == lastOperation.id();});

        removeLampFromScene(findLamp);

        if(!lastOperation.isDeleted())
        {
            setLampToPreviousState(findLamp, lastOperation, false);
        }
    }
}

void MainWindow::moveLampChanges(Lamp* lamp)
{
    addLampToUndoStackAndClearRedoStack(*lamp->createMemento());
}

void MainWindow::rotateLampChanges(Lamp *lamp)
{
    addLampToUndoStackAndClearRedoStack(*lamp->createMemento());
}

void MainWindow::changeLampLightSize(Lamp *lamp)
{
    addLampToUndoStackAndClearRedoStack(*lamp->createMemento());
}

void MainWindow::setMessageVisibleToFalse()
{
    ui->messageWidget->setVisible(false);
}

Lamp* MainWindow::createNewLamp()
{
    Lamp *lamp = new Lamp(0, 0, 53, 53, mCameraId);
    lamp->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
    lamp->setBrush(Qt::white);

    subscribeToLampEvents(lamp);

    return lamp;
}

void MainWindow::updateFindLamp(QList<Lamp*>::iterator findLamp, Lamp *lamp)
{
    *findLamp = lamp;
    mScene->addItem(lamp);
    update();
}

void MainWindow::addFindLamp(Lamp *lamp)
{
    mLampList.append(lamp);
    mScene->addItem(lamp);
    update();
}

void MainWindow::addNewOrUpdateLamp(QList<Lamp*>::iterator findLamp, Lamp *lamp, bool removeLamp)
{
    if(isFindLampExist(findLamp))
    {
        if(removeLamp)
            mScene->removeItem(*findLamp);
        updateFindLamp(findLamp,lamp);
    }
    else
    {
        addFindLamp(lamp);
    }
}

void MainWindow::setLampToPreviousState(QStack<Memento>::reverse_iterator lampToUpdate, bool removeLamp)
{
    Memento memento = *lampToUpdate;
    crteateLampWithPreviousState(memento, removeLamp);
}

void MainWindow::setLampToPreviousState(QStack<Memento>::iterator loadLamp, bool removeLamp)
{
    Memento memento = *loadLamp;
    crteateLampWithPreviousState(memento, removeLamp);
}

void MainWindow::crteateLampWithPreviousState(Memento memento, bool removeLamp)
{
    Lamp lmp;
    lmp.reinstateMemento(memento);

    Lamp *lamp = createAndRestoreLamp(lmp);

    auto findLamp = std::find_if(mLampList.begin(), mLampList.end(),
                                 [lamp](Lamp *l){return l->lampId() == lamp->lampId();});

    addNewOrUpdateLamp(findLamp, lamp, removeLamp);
}

void MainWindow::setLampToPreviousState(QList<Lamp*>::iterator findLamp, Memento lastOperation, bool removeLamp)
{
    Lamp lmp;
    lmp.reinstateMemento(lastOperation);

    Lamp *lamp = createAndRestoreLamp(lmp);
    lamp->setLampIsDeleted(false);

    addNewOrUpdateLamp(findLamp, lamp, removeLamp);
}

Lamp* MainWindow::createAndRestoreLamp(Lamp &lmp)
{
    Lamp *lamp = new Lamp(0, 0, lmp.lampWidth(), lmp.lampHeight(), lmp.lampId(),lmp.lampLightWidth(),lmp.lampLightHeight());

    lamp->setLampXCoordinate(lmp.lampXCoordinate());
    lamp->setLampYCoordinate(lmp.lampYCoordinate());
    lamp->setX(lamp->lampXCoordinate());
    lamp->setY(lamp->lampYCoordinate());

    lamp->setLampIsDeleted(lmp.lampIsDeleted());

    lamp->lampLight()->setLightWidth(lmp.lampLightWidth());
    lamp->lampLight()->setLightHeight(lmp.lampLightHeight());
    lamp->lampLight()->setLampLightColor(lmp.lampLightColor());

    lampRotation(lamp, lmp.lampAngle());

    lamp->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
    lamp->setBrush(Qt::white);

    subscribeToLampEvents(lamp);

    return lamp;
}

void MainWindow::OutputRoomList(const QJsonObject &json)
{
    ui->tblViewRooms->clearContents();
    ui->tblViewRooms->setRowCount(0);
    QJsonArray roomArray = json["rooms"].toArray();
    for(int roomIndex = 0;  roomIndex < roomArray.size(); ++roomIndex)
    {
        QJsonObject subtree = roomArray.at(roomIndex).toObject();

        mCurrentRoomId = subtree.value("id").toString().toInt(); // for update room after save

        ui->tblViewRooms->insertRow(ui->tblViewRooms->rowCount());
        ui->tblViewRooms->setItem(ui->tblViewRooms->rowCount()- 1, 0, new QTableWidgetItem("  "+subtree.value("roomName").toString()));
        ui->tblViewRooms->setItem(ui->tblViewRooms->rowCount()- 1, 1, new QTableWidgetItem(subtree.value("id").toString()));
        ui->tblViewRooms->setCellWidget(roomIndex, 2, addDeleteRoomButtonToRoomList());
    }
}

void MainWindow::lampRotation(Lamp *lamp, qreal angle)
{
    QRectF bbox = lamp->boundingRect().normalized();
    QPointF center = bbox.center();

    lamp->SetLampAngle(angle);

    QTransform xForm;
    xForm.translate(center.x(), center.y());
    xForm.rotate(angle);
    xForm.translate(-center.x(), -center.y());

    lamp->setTransform(xForm, false);
}

void MainWindow::restorRoom(QJsonArray lampArray)
{
    for(int lampIndex = 0; lampIndex < lampArray.size(); ++lampIndex)
    {
        QJsonObject lampObject = lampArray[lampIndex].toObject();
        Lamp lmp;
        lmp.read(lampObject);

        Lamp *lamp = createAndRestoreLamp(lmp);
        lamp->setLampIsDeleted(false);

        addLampToLoadStack(*lamp->createMemento());

        mCameraId =  lamp->lampId() + 1;

        mLampList.append(lamp);
        mScene->addItem(lamp);
    }
}

QJsonArray MainWindow::read(const QJsonObject &json)
{
    mLampList.clear();

    mSceneName = json["roomName"].toString();
    ui->txtRoomName->setText(mSceneName);

    QJsonArray lampArray = json["lamps"].toArray();

    return lampArray;
}

void MainWindow::write(QJsonObject &json)
{
    QJsonArray lampArray;

    foreach (const Lamp *lamp, mLampList)
    {
        if(!lamp->lampIsDeleted())
        {
            QJsonObject lampObject;
            lamp->write(lampObject);
            lampArray.append(lampObject);
        }
    }

    mSceneName = ui->txtRoomName->text();
    json["roomName"] = mSceneName;
    json["lamps"] = lampArray;
}

bool MainWindow::isSceneExist()
{
    return (mScene == nullptr);
}

void MainWindow::createScene()
{
    mScene = new QGraphicsScene(this);
}

void MainWindow::deleteScene()
{
    mScene->clear();
    mScene = nullptr;
    delete mScene;
}

void MainWindow::setRoomParameters()
{
    ui->graphicsViewCurrentRoom->setScene(mScene);
    ui->graphicsViewCurrentRoom->setSceneRect(0, 0, mWidth, mHeight);
    ui->graphicsViewCurrentRoom->fitInView(0, 0,mWidth, mHeight, Qt::KeepAspectRatio);
}

void MainWindow::clearContainers()
{
    mLampList.clear();
    mUndoStack.clear();
    mRedoStack.clear();
    mLoadStack.clear();
}

void MainWindow::subscribeToLampEvents(Lamp *lamp)
{
    connect(lamp, SIGNAL(clickCamera(int)), this, SLOT(setCurrentCameraId(int)));
    connect(lamp, SIGNAL(lampMove(Lamp*)), this, SLOT(moveLampChanges(Lamp*)));
    connect(lamp, SIGNAL(lampLightSizeChange(Lamp*)), this, SLOT(changeLampLightSize(Lamp*)));
    connect(lamp, SIGNAL(lampRotate(Lamp*)), this, SLOT(rotateLampChanges(Lamp*)));
}

bool MainWindow::isLampIdValid(int id)
{
    return (id != 0);
}

bool MainWindow::isRoomIdValid(int id)
{
    return (mCurrentRoomId == id);
}

bool MainWindow::isFindLampNotExist(QVector<Memento>::reverse_iterator findLamp)
{
    return (findLamp == mUndoStack.rend());
}

bool MainWindow::isFindLampExist(QList<Lamp*>::iterator findLamp)
{
    return (findLamp != mLampList.end());
}

bool MainWindow::isFindLampExist(QList<Lamp*>::reverse_iterator lampToRemove)
{
    return (lampToRemove != mLampList.rend());
}

bool MainWindow::isFindLampExist(int findLampId, int currentLampId)
{
    return (findLampId == currentLampId);
}

Memento MainWindow::getLastOperation(QStack<Memento> &fromStack, QStack<Memento> &toStack)
{
    Memento lastOperation = fromStack.pop();
    toStack.push_back(lastOperation);
    return lastOperation;
}

void MainWindow::addLampToLoadStack(Memento memento)
{
    mLoadStack.push_back(memento);
}

void MainWindow::addLampToUndoStackAndClearRedoStack(Memento memento)
{
    mUndoStack.push_back(memento);
    mRedoStack.clear();
}

void MainWindow::removeLampFromScene(QList<Lamp*>::reverse_iterator lampToRemove)
{
    mScene->removeItem(*lampToRemove);
    (*lampToRemove)->setLampIsDeleted(true);
    update();
}

void MainWindow::removeLampFromScene(QList<Lamp*>::iterator lampToRemove)
{
    mScene->removeItem(*lampToRemove);
    (*lampToRemove)->setLampIsDeleted(true);
    update();
}

QWidget* MainWindow::addDeleteRoomButtonToRoomList()
{
    QWidget* pWidget = new QWidget();
    TableButton* btnDeleteRoom = new TableButton(mCurrentRoomId);
    btnDeleteRoom->setText("x");
    connect(btnDeleteRoom, SIGNAL(clickTableButton(int)), this, SLOT(deleteRoomFromeDb(int)));
    btnDeleteRoom->setMinimumSize(38,29);
    btnDeleteRoom->setObjectName("btnDeleteRoom");
    btnDeleteRoom->setStyleSheet("QPushButton#btnDeleteRoom{"
                                 "  border : none;"
                                 "  color : #FFFFFF;"
                                 "  background-color: #27262C;"
                                 "}"

                                 "QPushButton#btnDeleteRoom:hover{"
                                 "  color:#f73220;"
                                 "}"

                                 "QPushButton#btnDeleteRoom:pressed{"
                                 "  color:#02ed29;"
                                 "  border: 1px solid #f73220;"
                                 "}"
                                 );

    QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
    pLayout->addWidget(btnDeleteRoom);
    pLayout->setAlignment(Qt::AlignCenter);
    pLayout->setContentsMargins(0, 0, 2, 0);
    pWidget->setLayout(pLayout);

    return pWidget;
}

void MainWindow::subscribeToFormEvents()
{
    connect(ui->btnCreateNewRoom, SIGNAL(clicked()), this, SLOT(createRoom()));
    connect(ui->btnAddLamp, SIGNAL(clicked()), this, SLOT(createLamp()));
    connect(ui->btnClearRoom, SIGNAL(clicked()), this, SLOT(clearRoom()));

    connect(ui->btnSaveRoom, SIGNAL(clicked()), this, SLOT(saveRoom()));
    connect(ui->tblViewRooms, SIGNAL(cellClicked(int,int)), this, SLOT(loadRoom(int,int)));
    connect(ui->btnUpdateRoom, SIGNAL(clicked()), this, SLOT(updateRoom()));

    connect(ui->btnColor, &QPushButton::clicked, this, [=]{setColorForCurrentLampLight(mCurrentCameraId);});
    connect(ui->btnDeleteLamp, &QPushButton::clicked, this, [=]{deleteLamp(mCurrentCameraId);});

    connect(ui->btnZoomPlus, SIGNAL(clicked(bool)), this, SLOT(zoomIn()));
    connect(ui->btnZoomMinus, SIGNAL(clicked(bool)), this, SLOT(zoomOut()));

    connect(ui->btnRedo, SIGNAL(clicked(bool)), this, SLOT(redo()));
    connect(ui->btnUndo, SIGNAL(clicked(bool)), this, SLOT(undo()));
}

void MainWindow::setRoomsListTableWidgetOptions()
{   
    ui->tblViewRooms->setColumnCount(3);
    ui->tblViewRooms->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tblViewRooms->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->tblViewRooms->setColumnWidth(0,140);
    ui->tblViewRooms->setColumnHidden(1, true);
    ui->tblViewRooms->setColumnWidth(2,39);

    ui->tblViewRooms->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {"
                                                         "    border: 1px solid #797979;"
                                                         "    background:#ffffff;;"
                                                         "    width:5px;    "
                                                         "    margin: 0px 0px 0px 0px;"
                                                         "}"
                                                         "QScrollBar::handle:vertical {"
                                                         "    background: #BEC947;"   // #18181a; #797979; #909090
                                                         "    min-height: 0px;"
                                                         "}"
                                                         "QScrollBar::add-line:vertical {"
                                                         "    height: 0px;"
                                                         "}"
                                                         "QScrollBar::sub-line:vertical {"
                                                         "    height: 0 px;"
                                                         "}");
}

void MainWindow::outputMessage(QString message)
{
    ui->lblMessage->setText(message);
    ui->messageWidget->setVisible(true);
    QTimer::singleShot(2000,this,SLOT(setMessageVisibleToFalse()));
}

void MainWindow::setColorAndDeleteLampButtonsState(bool color, bool deleteLamp)
{
    ui->btnColor->setEnabled(color);
    ui->btnDeleteLamp->setEnabled(deleteLamp);
}

void MainWindow::setUndoRedoButtonsState(bool undo, bool redo)
{
    ui->btnUndo->setEnabled(undo);
    ui->btnRedo->setEnabled(redo);
}

void MainWindow::setUpdateAndSaveRoomButtonsState(bool saveRoom, bool updateRoom)
{
    ui->btnSaveRoom->setEnabled(saveRoom);
    ui->btnUpdateRoom->setEnabled(updateRoom);
}

void MainWindow::setUpdateRoomAndUndoRedoButtonsState(bool updateRoom, bool undo, bool redo)
{
    ui->btnUpdateRoom->setEnabled(updateRoom);
    setUndoRedoButtonsState(undo, redo);
}

void MainWindow::setUiElementsState(bool saveRoom, bool updateRoom,bool clearRoom,
                                    bool addLamp, bool deleteLamp, bool color,
                                    bool zoomP, bool zoomM, bool undo, bool redo)
{
    ui->btnSaveRoom->setEnabled(saveRoom);
    ui->btnUpdateRoom->setEnabled(updateRoom);
    ui->btnClearRoom->setEnabled(clearRoom);

    ui->btnAddLamp->setEnabled(addLamp);
    ui->btnDeleteLamp->setEnabled(deleteLamp);
    ui->btnColor->setEnabled(color);

    ui->btnZoomPlus->setEnabled(zoomP);
    ui->btnZoomMinus->setEnabled(zoomM);
    ui->btnUndo->setEnabled(undo);
    ui->btnRedo->setEnabled(redo);
}
