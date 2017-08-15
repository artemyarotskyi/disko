#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mCameraId(1),
    mRepository(new OperationRepository(mPath ,this))
{
    ui->setupUi(this);
    ui->graphicsViewCurrentRoom->setScene(mScene);    

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

    mScene->addItem(lamp);

    connect(lamp, SIGNAL(clickCamera(int)), this, SLOT(setCurrentCameraId(int)));

    mCameraId++;
}

void MainWindow::deleteCamera(int id)
{
    if(id !=0)
    {
        for(int i = 0; i < mLampList.size(); ++i)
        {
            if(mLampList.at(i)->lampId() == id)
            {
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
    QJsonObject roomObject;
    write(roomObject);
    mRepository->SaveRoom(roomObject);
    loadRoomList(mRepository->GetAllRooms());

    ui->btnUpdateRoom->setEnabled(true);
}

void MainWindow::loadRoom(int row, int)
{
    deleteRoom();
    createRoom();

    mCurrentRoomId = ui->tblViewRooms->item(row,1)->text().toInt();
    read(mRepository->GetCurrentRoom(mCurrentRoomId));

    ui->btnUpdateRoom->setEnabled(true);
}

void MainWindow::deleteRoomFromeDb(int id)
{

}

void MainWindow::updateRoom()
{
    QJsonObject roomObject;
    write(roomObject);
    mRepository->UpdateRoom(roomObject, mCurrentRoomId);
}

void MainWindow::zoomIn()
{
    ui->graphicsViewCurrentRoom->scale(2,2);
}

void MainWindow::zoomOut()
{
    ui->graphicsViewCurrentRoom->scale(.5,.5);
}

void MainWindow::loadRoomList(const QJsonObject &json)
{
    ui->tblViewRooms->clearContents();
    ui->tblViewRooms->setRowCount(0);
    QJsonArray roomArray = json["rooms"].toArray();
    for(int roomIndex = 0;  roomIndex < roomArray.size(); ++roomIndex)
    {
        QJsonObject subtree = roomArray.at(roomIndex).toObject();

        ui->tblViewRooms->insertRow(ui->tblViewRooms->rowCount());
        ui->tblViewRooms->setItem(ui->tblViewRooms->rowCount()- 1, 0, new QTableWidgetItem(subtree.value("roomName").toString()));
        ui->tblViewRooms->setItem(ui->tblViewRooms->rowCount()- 1, 1, new QTableWidgetItem(subtree.value("id").toString()));

        mCurrentRoomId = subtree.value("id").toString().toInt(); // for update room after save
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

        connect(lamp, SIGNAL(clickCamera(int)), this, SLOT(setCurrentCameraId(int)));
        mCameraId = mCurrentCameraId + 1;

        mLampList.append(lamp);
        mScene->addItem(lamp);
    }
}

void MainWindow::write(QJsonObject &json) const
{
    QJsonArray lampArray;

    foreach (const Lamp *lamp, mLampList)
    {
        QJsonObject lampObject;
        lamp->write(lampObject);
        lampArray.append(lampObject);
    }

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
}

void MainWindow::SetRoomsListTableWidgetOptions()
{   
    ui->tblViewRooms->setColumnCount(2);    
    ui->tblViewRooms->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tblViewRooms->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->tblViewRooms->setColumnWidth(0,181);
    ui->tblViewRooms->setColumnHidden(1, true);
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
