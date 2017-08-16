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
    void createRoom();
    void deleteRoom();
    void clearRoom();

    void createCamera();
    void deleteCamera(int id);
    void setColorForCurrentLampLight(int id);
    void setCurrentCameraId(int id);

    void saveRoom();
    void loadRoom(int row, int);
    void deleteRoomFromeDb(int id);
    void updateRoom();

    void zoomIn();
    void zoomOut();

 private:
    void loadRoomList(const QJsonObject &json);
    void lampRotation(Lamp *lamp, qreal angle);

    void read(const QJsonObject &json);
    void write(QJsonObject &json);

    void SubscribeToFormEvents();
    void SetRoomsListTableWidgetOptions();
    void SetUiElementsState(bool saveRoom, bool updateRoom, bool clearRoom,
                            bool addLamp, bool deleteLamp, bool color,
                            bool zoomP, bool zoomM, bool undo, bool redo);

    Ui::MainWindow *ui;    
    QGraphicsScene *mScene;
    QString mSceneName;

    int mCameraId;
    int mCurrentCameraId;

    int mCurrentRoomId;

    QList<Lamp*> mLampList;

    int mWidth = 600;
    int mHeight = 520;

    const QString mPath = "C:/sqlite/disko.db";

    OperationRepository* mRepository;
};

#endif // MAINWINDOW_H
