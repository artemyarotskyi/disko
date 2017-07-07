#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mLamp(new Lamp(0, 0, 53, 53)),
    mLight(new LampLight()),
    mScene(new QGraphicsScene())
{
    ui->setupUi(this);

    connect(ui->btnAddLamp, SIGNAL(clicked()), this, SLOT(createCamera()));

//    lamp->setPos(0,0);
//    lamp->setFlags(QGraphicsItem::ItemIsMovable);
//    lamp->setBrush(Qt::black);
//    scene->addItem(lamp);

    ui->graphicsViewCurrentRoom->setScene(mScene);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mLight;
    delete mScene;
}

void MainWindow::createCamera()
{
    Lamp *lamp = new Lamp(0, 0, 53, 53);
    lamp->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
    lamp->setBrush(Qt::black);
    mScene->addItem(lamp);
}
