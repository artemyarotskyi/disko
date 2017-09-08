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
#include <QStack>
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

    void createLamp();
    void deleteLamp(int id);
    void setColorForCurrentLampLight(int id);
    void setCurrentCameraId(int id);

    void saveRoom();
    void loadRoom(int row, int);
    void deleteRoomFromeDb(int id);
    void updateRoom();

    void zoomIn();
    void zoomOut();

    void undo();
    void redo();

    void moveLampChanges(Lamp *lamp);
    void rotateLampChanges(Lamp *lamp);
    void changeLampLightSize(Lamp *lamp);

    void setMessageVisibleToFalse();

 private:
    Lamp* createAndRestoreLamp(Lamp &lmp);
    Lamp* createNewLamp();
    void updateFindLamp(QList<Lamp*>::iterator findLamp, Lamp* lamp);
    void addFindLamp(Lamp* lamp);
    void addNewOrUpdateLamp(QList<Lamp*>::iterator findLamp, Lamp *lamp, bool removeLamp);
    void setLampToPreviousState(QStack<Memento>::reverse_iterator lampToUpdate, bool removeLamp);
    void setLampToPreviousState(QStack<Memento>::iterator loadLamp, bool removeLamp);
    void crteateLampWithPreviousState(Memento memento, bool removeLamp);
    void setLampToPreviousState(QList<Lamp*>::iterator findLamp, Memento lastOperation, bool removeLamp);
    void OutputRoomList(const QJsonObject &json);
    void lampRotation(Lamp *lamp, qreal angle);

    void restorRoom(QJsonArray lampArray);
    QJsonArray read(const QJsonObject &json);
    void write(QJsonObject &json);

    bool isSceneExist();
    void createScene();
    void deleteScene();
    void setRoomParameters();
    void clearContainers();
    void subscribeToLampEvents(Lamp *lamp);
    bool isLampIdValid(int id);
    bool isRoomIdValid(int id);    
    bool isFindLampNotExist(QStack<Memento>::reverse_iterator findLamp);
    bool isFindLampExist(QList<Lamp*>::iterator findLamp);
    bool isFindLampExist(QList<Lamp*>::reverse_iterator lampToRemove);
    bool isFindLampExist(int findLampId, int currentLampId);
    Memento getLastOperation(QStack<Memento> &fromStack, QStack<Memento> &toStack);
    void addLampToLoadStack(Memento memento);
    void addLampToUndoStackAndClearRedoStack(Memento memento);
    void removeLampFromScene(QList<Lamp*>::reverse_iterator lampToRemove); // reverse iterator
    void removeLampFromScene(QList<Lamp*>::iterator lampToRemove); // iterator

    QWidget* addDeleteRoomButtonToRoomList();
    void subscribeToFormEvents();
    void setRoomsListTableWidgetOptions();
    void outputMessage(QString message);
    void setColorAndDeleteLampButtonsState(bool color, bool deleteLamp);
    void setUndoRedoButtonsState(bool undo, bool redo);
    void setUpdateAndSaveRoomButtonsState(bool saveRoom, bool updateRoom);
    void setUpdateRoomAndUndoRedoButtonsState(bool updateRoom, bool undo, bool redo);
    void setUiElementsState(bool saveRoom, bool updateRoom, bool clearRoom,
                            bool addLamp, bool deleteLamp, bool color,
                            bool zoomP, bool zoomM, bool undo, bool redo);

    Ui::MainWindow *ui;    
    QGraphicsScene *mScene;
    QString mSceneName;

    QStack<Memento> mUndoStack;
    QStack<Memento> mRedoStack;
    QStack<Memento> mLoadStack;

    int mCameraId;
    int mCurrentCameraId;

    int mCurrentRoomId;

    QList<Lamp*> mLampList;

    int mWidth = 600;
    int mHeight = 520;

    const QString mPath = "C:/sqlite/disko.db";

    const QString mCreateRoomMessage = "Room has been created";
    const QString mDeleteRoomMessage = "Room has been deleted";
    const QString mSaveRoomMesssage = "Room has been saved";
    const QString mUpdateRoomMessage = "Room has been updated";
    const QString mLoadRoomMessage = "Room has been loaded";

    OperationRepository* mRepository;
};

#endif // MAINWINDOW_H
