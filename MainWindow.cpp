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
    mScene(new QGraphicsScene()),
    mRepository(new OperationRepository(mPath ,this))
{
    ui->setupUi(this);
    ui->graphicsViewCurrentRoom->setScene(mScene);    

    SetRoomsListTableWidgetOptions();
    SubscribeToFormEvents();

    loadRoomList(mRepository->GetAllRooms());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mScene;
}

void MainWindow::createCamera()
{
    Lamp *lamp = new Lamp(0, 0, 53, 53, mCameraId);
    lamp->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
    lamp->setBrush(Qt::black);
    mScene->addItem(lamp);

    mLampList.append(lamp);

    connect(lamp, SIGNAL(clickCamera(int)), this, SLOT(setCurrentCameraId(int)));

    mCameraId++;

    ui->graphicsViewCurrentRoom->setFixedSize(mWidth, mHeight);
    ui->graphicsViewCurrentRoom->setSceneRect(0, 0, mWidth, mHeight);
    ui->graphicsViewCurrentRoom->fitInView(0, 0,mWidth, mHeight, Qt::KeepAspectRatio);
    //mScene->setSceneRect(0,0,597,477);
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
}

void MainWindow::saveRoom()
{
    QJsonObject roomObject;
    write(roomObject);
    mRepository->SaveRoom(roomObject);
}

void MainWindow::loadRoom(int row, int)
{
    int id = ui->tblViewRooms->item(row,1)->text().toInt();
    read(mRepository->GetCurrentRoom(id));
    paintLamps();
}

void MainWindow::paintLamps()
{

}

void MainWindow::loadRoomList(const QJsonObject &json)
{
    QJsonArray roomArray = json["rooms"].toArray();
    for(int roomIndex = 0;  roomIndex < roomArray.size(); ++roomIndex)
    {
        QJsonObject subtree = roomArray.at(roomIndex).toObject();

        ui->tblViewRooms->insertRow(ui->tblViewRooms->rowCount());
        ui->tblViewRooms->setItem(ui->tblViewRooms->rowCount()- 1, 0, new QTableWidgetItem(subtree.value("roomName").toString()));
        ui->tblViewRooms->setItem(ui->tblViewRooms->rowCount()- 1, 1, new QTableWidgetItem(subtree.value("id").toString()));
    }
}

void MainWindow::read(const QJsonObject &json)
{
    mLampList.clear();

    mSceneName = json["roomName"].toString();
    mSceneId = json["roomId"].toInt();

    QJsonArray lampArray = json["lamps"].toArray();

    for(int lampIndex = 0; lampIndex < lampArray.size(); ++lampIndex)
    {
        QJsonObject lampObject = lampArray[lampIndex].toObject();
        Lamp lamp;        
        lamp.read(lampObject);
        mLampList.append(&lamp);
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
    connect(ui->btnAddLamp, SIGNAL(clicked()), this, SLOT(createCamera()));

    connect(ui->btnSaveRoom, SIGNAL(clicked()), this, SLOT(saveRoom()));
    connect(ui->tblViewRooms, SIGNAL(cellClicked(int,int)), this, SLOT(loadRoom(int,int)));

    connect(ui->btnColor, &QPushButton::clicked, this, [=]{setColorForCurrentLampLight(mCurrentCameraId);});
}

void MainWindow::SetRoomsListTableWidgetOptions()
{   
    ui->tblViewRooms->setColumnCount(2);    
    ui->tblViewRooms->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tblViewRooms->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->tblViewRooms->setColumnWidth(0,181);
    ui->tblViewRooms->setColumnHidden(1, true);
}
