#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mCameraId(1),
    mScene(new QGraphicsScene())
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
            if(mLampList.at(i)->getLampId() == id)
            {
                lamp = mLampList.at(i);
                QColor color = QColorDialog::getColor(Qt::white, this, "Choose color");
                if(color.isValid())
                {
                    lamp->getLampLight()->setLampLightColor(color);
                    update();//lamp->getLampLight()->update();
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

void MainWindow::SubscribeToFormEvents()
{
    connect(ui->btnAddLamp, SIGNAL(clicked()), this, SLOT(createCamera()));    
}
