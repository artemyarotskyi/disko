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

    void saveRoom();
    void loadRoom(int row, int);

 private:
    void loadRoomList(const QJsonObject &json);

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    void SubscribeToFormEvents();
    void SetRoomsListTableWidgetOptions();

    Ui::MainWindow *ui;    
    QGraphicsScene *mScene;
    QString mSceneName = "Room 1";
    int mSceneId;

    int mCameraId;
    int mCurrentCameraId;

    QList<Lamp*> mLampList;

    int mWidth = 601;
    int mHeight = 480;

    const QString mPath = "C:/sqlite/disko.db";

    OperationRepository* mRepository;
};

#endif // MAINWINDOW_H
