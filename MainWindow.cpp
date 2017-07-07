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
    ui->graphicsViewCurrentRoom->setScene(mScene);

    SubscribeToFormEvents();
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

void MainWindow::SubscribeToFormEvents()
{
    connect(ui->btnAddLamp, SIGNAL(clicked()), this, SLOT(createCamera()));
}
