#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),    
    mScene(new QGraphicsScene())
{
    ui->setupUi(this);
    ui->graphicsViewCurrentRoom->setScene(mScene);

    SubscribeToFormEvents();
}

MainWindow::~MainWindow()
{
    delete ui;    
    delete mScene;
}

void MainWindow::createCamera()
{
    Lamp *lamp = new Lamp(0, 0, 53, 53);
    lamp->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
    lamp->setBrush(Qt::black);
    mScene->addItem(lamp);

    ui->graphicsViewCurrentRoom->setFixedSize(mWidth, mHeight);
    ui->graphicsViewCurrentRoom->setSceneRect(0, 0, mWidth, mHeight);
    ui->graphicsViewCurrentRoom->fitInView(0,0,mWidth, mHeight, Qt::KeepAspectRatio);
    //mScene->setSceneRect(0,0,597,477);
}

void MainWindow::SubscribeToFormEvents()
{
    connect(ui->btnAddLamp, SIGNAL(clicked()), this, SLOT(createCamera()));
}
