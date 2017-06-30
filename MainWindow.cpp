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

    light->setPos(17,52);
    light->setParentItem(lamp);
    scene->addItem(light);

    ui->graphicsViewCurrentRoom->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete light;
    delete scene;
}
