#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CommonUiControllers/LampLight.h"
#include "CommonUiControllers/Lamp.h"
#include <QGraphicsScene>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void createCamera();

private:
    void SubscribeToFormEvents();

    Ui::MainWindow *ui;

    Lamp *mLamp;
    LampLight *mLight;
    QGraphicsScene *mScene;
};

#endif // MAINWINDOW_H
