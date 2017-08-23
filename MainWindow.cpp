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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mCameraId(1),
    mRepository(new OperationRepository(mPath ,this))
{
    ui->setupUi(this);
    ui->graphicsViewCurrentRoom->setScene(mScene);
    ui->lblMessage->setVisible(false);

    SetRoomsListTableWidgetOptions();
    SubscribeToFormEvents();
    SetUiElementsState(false,false,false,
                       false,false,false,
                       false,false,false,false);

    loadRoomList(mRepository->GetAllRooms());    
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mScene;
}

void MainWindow::createRoom()
{
    deleteRoom();

    if(mScene == nullptr)
    {
        mScene = new QGraphicsScene(this);
        ui->graphicsViewCurrentRoom->setScene(mScene);
        ui->graphicsViewCurrentRoom->setSceneRect(0, 0, mWidth, mHeight);
        ui->graphicsViewCurrentRoom->fitInView(0, 0,mWidth, mHeight, Qt::KeepAspectRatio);

        OutputMessage(mCreateRoomMessage);
    }

    SetUiElementsState(true, false, true,
                       true, false, false,
                       true, true, false, false);
}

void MainWindow::deleteRoom()
{
    if(mScene != nullptr)
    {
        mLampList.clear();
        mScene->clear();
        mScene = nullptr;
        delete mScene;        
    }
}

void MainWindow::clearRoom()
{
    mScene->clear();
    mLampList.clear();   
}

void MainWindow::createCamera()
{
    Lamp *lamp = new Lamp(0, 0, 53, 53, mCameraId);
    lamp->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
    lamp->setBrush(Qt::black);
    mLampList.append(lamp);
    mUndoStack.push_back(*lamp->createMemento());

    mScene->addItem(lamp);

    connect(lamp, SIGNAL(clickCamera(int)), this, SLOT(setCurrentCameraId(int)));
    connect(lamp, SIGNAL(lampMoveOrRotate(Lamp*)), this, SLOT(moveLampChanges(Lamp*)));
    connect(lamp, SIGNAL(lampLightSizeChange(Lamp*)), this, SLOT(changeLampLightSize(Lamp*)));

    mCameraId++;

    ui->btnUndo->setEnabled(true);
    ui->btnRedo->setEnabled(true);
}

void MainWindow::deleteCamera(int id)
{
    if(id !=0)
    {
        for(int i = 0; i < mLampList.size(); ++i)
        {
            if(mLampList.at(i)->lampId() == id)
            {
                mUndoStack.push_back(*mLampList.at(i)->createMemento());

                mScene->removeItem(mLampList.at(i));
                mLampList.removeAt(i);
                mCurrentCameraId = 0;                
                break;
            }
        }
    }

    ui->btnColor->setEnabled(false);
    ui->btnDeleteLamp->setEnabled(false);
}

void MainWindow::setColorForCurrentLampLight(int id)
{
    if(id != 0)
    {
        Lamp *lamp;
        for(int i = 0; i < mLampList.size(); ++i)
        {
            if(mLampList.at(i)->lampId() == id)
            {
                lamp = mLampList.at(i);
                QColor color = QColorDialog::getColor(Qt::white, this, "Choose color");
                if(color.isValid())
                {
                    lamp->lampLight()->setLampLightColor(color);
                    update();

                    mUndoStack.push_back(*lamp->createMemento());
                }
                break;
            }
        }
    }
}

void MainWindow::setCurrentCameraId(int id)
{
    mCurrentCameraId = id;
    ui->btnColor->setEnabled(true);
    ui->btnDeleteLamp->setEnabled(true);
}

void MainWindow::saveRoom()
{
    ui->btnSaveRoom->setEnabled(false);
    QJsonObject roomObject;
    write(roomObject);
    mRepository->SaveRoom(roomObject);
    loadRoomList(mRepository->GetAllRooms());

    ui->btnUpdateRoom->setEnabled(true);

    OutputMessage(mSaveRoomMesssage);
    ui->btnSaveRoom->setEnabled(true);
}

void MainWindow::loadRoom(int row, int)
{
    deleteRoom();
    createRoom();

    mCurrentRoomId = ui->tblViewRooms->item(row,1)->text().toInt();
    read(mRepository->GetCurrentRoom(mCurrentRoomId));

    ui->btnUpdateRoom->setEnabled(true);

    OutputMessage(mLoadRoomMessage);
}

void MainWindow::deleteRoomFromeDb(int id)
{
    if(mCurrentRoomId == id)
    {
        deleteRoom();
        SetUiElementsState(false,false,false,
                           false,false,false,
                           false,false,false,false);

    }

    mRepository->DeleteRoom(id);
    loadRoomList(mRepository->GetAllRooms());

    OutputMessage(mDeleteRoomMessage);
}

void MainWindow::updateRoom()
{
    ui->btnUpdateRoom->setEnabled(false);
    QJsonObject roomObject;
    write(roomObject);
    mRepository->UpdateRoom(roomObject, mCurrentRoomId);
    loadRoomList(mRepository->GetAllRooms());

    OutputMessage(mUpdateRoomMessage);
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
    if(!mUndoStack.isEmpty())
    {
        Memento lastOperation = mUndoStack.pop();

        mRedoStack.push_back(lastOperation);

        Memento memento = mUndoStack.pop();
 //
        Lamp lmp;
        lmp.reinstateMemento(memento);

        Lamp *lamp = new Lamp(0,0,lmp.lampWidth(), lmp.lampHeight(), lmp.lampId(), lmp.lampLightWidth(), lmp.lampLightHeight());

        lamp->setLampXCoordinate(lmp.lampXCoordinate());
        lamp->setLampYCoordinate(lmp.lampYCoordinate());
        lamp->setX(lamp->lampXCoordinate());
        lamp->setY(lamp->lampYCoordinate());

        lamp->lampLight()->setLightWidth(lmp.lampLightWidth());
        lamp->lampLight()->setLightHeight(lmp.lampLightHeight());
        lamp->lampLight()->setLampLightColor(lmp.lampLightColor());

        lampRotation(lamp, lmp.lampAngle());

        lamp->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
        lamp->setBrush(Qt::black);

        connect(lamp, SIGNAL(clickCamera(int)), this, SLOT(setCurrentCameraId(int)));

        for(int i = 0; i < mLampList.size(); ++i)
        {
            if(mLampList.at(i)->lampId() == lamp->lampId())
            {
                mScene->removeItem(mLampList.at(i));
                mLampList.replace(i,lamp);

                mScene->addItem(lamp);
                update();
            }
        }
    }
}

void MainWindow::redo()
{
    if(!mRedoStack.isEmpty())
    {
        Memento currentLamp = mRedoStack.pop();
        mUndoStack.push_back(currentLamp);

        for(int i = 0; i < mLampList.size(); ++i)
        {
            if(mLampList.at(i)->lampId() == currentLamp.id())
            {
                mLampList.at(i)->reinstateMemento(currentLamp);
            }
        }
    }
}

void MainWindow::moveLampChanges(Lamp* lamp)
{
    mUndoStack.push_back(*lamp->createMemento());
}

void MainWindow::changeLampLightSize(Lamp *lamp)
{
    mUndoStack.push_back(*lamp->createMemento());
}

void MainWindow::setMessageVisibleToFalse()
{
    ui->lblMessage->setVisible(false);
}

void MainWindow::loadRoomList(const QJsonObject &json)
{
    ui->tblViewRooms->clearContents();
    ui->tblViewRooms->setRowCount(0);
    QJsonArray roomArray = json["rooms"].toArray();
    for(int roomIndex = 0;  roomIndex < roomArray.size(); ++roomIndex)
    {
        QJsonObject subtree = roomArray.at(roomIndex).toObject();

        mCurrentRoomId = subtree.value("id").toString().toInt(); // for update room after save

        // add delete room button to room list
        QWidget* pWidget = new QWidget();
        TableButton* btnDeleteRoom = new TableButton(mCurrentRoomId);
        btnDeleteRoom->setText("x");
        connect(btnDeleteRoom, SIGNAL(clickTableButton(int)), this, SLOT(deleteRoomFromeDb(int)));

        QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
        pLayout->addWidget(btnDeleteRoom);
        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0, 0, 2, 0);
        pWidget->setLayout(pLayout);
        //

        ui->tblViewRooms->insertRow(ui->tblViewRooms->rowCount());
        ui->tblViewRooms->setItem(ui->tblViewRooms->rowCount()- 1, 0, new QTableWidgetItem(subtree.value("roomName").toString()));
        ui->tblViewRooms->setItem(ui->tblViewRooms->rowCount()- 1, 1, new QTableWidgetItem(subtree.value("id").toString()));
        ui->tblViewRooms->setCellWidget(roomIndex, 2,pWidget );
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

void MainWindow::read(const QJsonObject &json)
{
    mLampList.clear();

    mSceneName = json["roomName"].toString();
    ui->txtRoomName->setText(mSceneName);

    QJsonArray lampArray = json["lamps"].toArray();

    for(int lampIndex = 0; lampIndex < lampArray.size(); ++lampIndex)
    {
        QJsonObject lampObject = lampArray[lampIndex].toObject();
        Lamp lmp;
        lmp.read(lampObject);

        Lamp *lamp = new Lamp(0, 0, lmp.lampWidth(), lmp.lampHeight(), lmp.lampId(),lmp.lampLightWidth(),lmp.lampLightHeight());

        lamp->setLampXCoordinate(lmp.lampXCoordinate());
        lamp->setLampYCoordinate(lmp.lampYCoordinate());
        lamp->setX(lamp->lampXCoordinate());
        lamp->setY(lamp->lampYCoordinate());

        lamp->lampLight()->setLightWidth(lmp.lampLightWidth());
        lamp->lampLight()->setLightHeight(lmp.lampLightHeight());
        lamp->lampLight()->setLampLightColor(lmp.lampLightColor());

        lampRotation(lamp, lmp.lampAngle());

        lamp->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
        lamp->setBrush(Qt::black);

        mUndoStack.push_back(*lamp->createMemento());

        connect(lamp, SIGNAL(clickCamera(int)), this, SLOT(setCurrentCameraId(int)));
        mCameraId = mCurrentCameraId + 1;

        mLampList.append(lamp);
        mScene->addItem(lamp);
    }
}

void MainWindow::write(QJsonObject &json)
{
    QJsonArray lampArray;

    foreach (const Lamp *lamp, mLampList)
    {
        QJsonObject lampObject;
        lamp->write(lampObject);
        lampArray.append(lampObject);
    }

    mSceneName = ui->txtRoomName->text();
    json["roomName"] = mSceneName;
    json["lamps"] = lampArray;
}

void MainWindow::SubscribeToFormEvents()
{
    connect(ui->btnCreateNewRoom, SIGNAL(clicked()), this, SLOT(createRoom()));
    connect(ui->btnAddLamp, SIGNAL(clicked()), this, SLOT(createCamera()));
    connect(ui->btnClearRoom, SIGNAL(clicked()), this, SLOT(clearRoom()));

    connect(ui->btnSaveRoom, SIGNAL(clicked()), this, SLOT(saveRoom()));
    connect(ui->tblViewRooms, SIGNAL(cellClicked(int,int)), this, SLOT(loadRoom(int,int)));
    connect(ui->btnUpdateRoom, SIGNAL(clicked()), this, SLOT(updateRoom()));

    connect(ui->btnColor, &QPushButton::clicked, this, [=]{setColorForCurrentLampLight(mCurrentCameraId);});
    connect(ui->btnDeleteLamp, &QPushButton::clicked, this, [=]{deleteCamera(mCurrentCameraId);});

    connect(ui->btnZoomPlus, SIGNAL(clicked(bool)), this, SLOT(zoomIn()));
    connect(ui->btnZoomMinus, SIGNAL(clicked(bool)), this, SLOT(zoomOut()));

    connect(ui->btnRedo, SIGNAL(clicked(bool)), this, SLOT(redo()));
    connect(ui->btnUndo, SIGNAL(clicked(bool)), this, SLOT(undo()));
}

void MainWindow::SetRoomsListTableWidgetOptions()
{   
    ui->tblViewRooms->setColumnCount(3);
    ui->tblViewRooms->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tblViewRooms->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->tblViewRooms->setColumnWidth(0,140);
    ui->tblViewRooms->setColumnHidden(1, true);
    ui->tblViewRooms->setColumnWidth(2,41);

    ui->tblViewRooms->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {"
                                                         "    border: 1px solid #797979;"
                                                         "    background:#ffffff;;"
                                                         "    width:5px;    "
                                                         "    margin: 0px 0px 0px 0px;"
                                                         "}"
                                                         "QScrollBar::handle:vertical {"
                                                         "    background: #cccccc;"   // #18181a; #797979; #909090
                                                         "    min-height: 0px;"
                                                         "}"
                                                         "QScrollBar::add-line:vertical {"
                                                         "    height: 0px;"
                                                         "}"
                                                         "QScrollBar::sub-line:vertical {"
                                                         "    height: 0 px;"
                                                         "}");
}

void MainWindow::OutputMessage(QString message)
{
    ui->lblMessage->setText(message);
    ui->lblMessage->setVisible(true);
    QTimer::singleShot(1500,this,SLOT(setMessageVisibleToFalse()));
}

void MainWindow::SetUiElementsState(bool saveRoom, bool updateRoom,bool clearRoom,
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
