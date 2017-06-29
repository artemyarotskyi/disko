#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    light(new LampLight()),
    scene(new QGraphicsScene())
{
    ui->setupUi(this);

    light->setPos(100, 100);
    scene->addItem(light);

    ui->graphicsViewCurrentRoom->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete light;
    delete scene;
}
