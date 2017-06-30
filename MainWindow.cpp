#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    lamp(new Lamp(0, 0, 53, 53)),
    light(new LampLight()),
    scene(new QGraphicsScene())
{
    ui->setupUi(this);

    lamp->setPos(0,0);
    lamp->setFlags(QGraphicsItem::ItemIsMovable);
    lamp->setBrush(Qt::black);
    scene->addItem(lamp);

    light->setPos(0,55);
    light->setParentItem(lamp);
    scene->addItem(light);

//    Lamp *lmp = new Lamp(0,0, 53, 53);
//    lmp->setPos(0,0);
//    lmp->setFlags(QGraphicsItem::ItemIsMovable);
//    lmp->setBrush(Qt::black);
//    scene->addItem(lmp);

//    LampLight *l = new LampLight();
//    l->setPos(0,55);
//    l->setParentItem(lmp);
//    scene->addItem(l);

    ui->graphicsViewCurrentRoom->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete light;
    delete scene;
}
