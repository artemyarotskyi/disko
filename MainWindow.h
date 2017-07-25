#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CommonUiControllers/LampLight.h"
#include "CommonUiControllers/Lamp.h"
#include "Repositories/OperationRepository.h"
#include <QGraphicsScene>
#include <QColorDialog>
#include <QColor>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QBrush>
#include <QList>
#include <QJsonObject>

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
    void setColorForCurrentLampLight(int id);
    void setCurrentCameraId(int id);

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

private:    
    void SubscribeToFormEvents();

    Ui::MainWindow *ui;    
    QGraphicsScene *mScene;

    int mCameraId;
    int mCurrentCameraId;

    QList<Lamp*> mLampList;

    int mWidth = 601;
    int mHeight = 480;

    const QString mPath = "C:/sqlite/disko.db";

    OperationRepository* repository;
};

#endif // MAINWINDOW_H
