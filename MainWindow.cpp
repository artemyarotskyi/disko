#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mCameraId(1),
    mScene(new QGraphicsScene()),
    mRepository(new OperationRepository(mPath ,this))
{
    ui->setupUi(this);
    ui->graphicsViewCurrentRoom->setScene(mScene);

    connect(ui->btnColor, &QPushButton::clicked, this, [=]{setColorForCurrentLampLight(mCurrentCameraId);});

    SubscribeToFormEvents();
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
    write(mRoomObject);
    mRepository->SaveRoom(mRoomObject);
}

void MainWindow::loadRoom(int id)
{

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
    json["roomName"] = mSceneName;
    QJsonArray lampArray;
    foreach (const Lamp *lamp, mLampList) {
        QJsonObject lampObject;
        lamp->write(lampObject);
        lampArray.append(lampObject);
    }    
    json["lamps"] = lampArray;
}

void MainWindow::SubscribeToFormEvents()
{
    connect(ui->btnAddLamp, SIGNAL(clicked()), this, SLOT(createCamera()));
    connect(ui->btnSaveRoom, SIGNAL(clicked()), this, SLOT(saveRoom()));
}
