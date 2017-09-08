#ifndef LAMP_H
#define LAMP_H

#include <cmath>
#include <QApplication>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QBrush>
#include <QPen>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QJsonObject>
#include <QString>
#include <CommonUiControllers/LampLight.h>
#include <CommonUiControllers/Memento.h>

class Lamp : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    Lamp(qreal x, qreal y, qreal width, qreal height, int id, qreal lightWidth = 42, qreal lightHeight = 40);
    Lamp();

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    Memento* createMemento();
    void reinstateMemento(Memento memento);

    qreal lampXCoordinate() const;
    void setLampXCoordinate(qreal x);

    qreal lampYCoordinate() const;
    void setLampYCoordinate(qreal y);

    qreal lampWidth() const;
    qreal lampHeight() const;

    qreal lampAngle() const;
    void SetLampAngle(qreal angle);

    LampLight* lampLight();
    int lampId() const;

    void setLampLightWidth(qreal width);
    qreal lampLightWidth() const;

    void setLampLightHeight(qreal height);
    qreal lampLightHeight() const;

    QColor lampLightColor() const;

    void setLampIsDeleted(bool lampIsDeleted);
    bool lampIsDeleted() const;

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

signals:
    void clickCamera(int);
    void lampMove(Lamp*);
    void lampRotate(Lamp*);
    void lampLightSizeChange(Lamp*);

protected slots:
    void emitLampLightSizeChanges();


private:
    bool isLeftButtonPressed(QGraphicsSceneMouseEvent *event);
    bool isShiftPressed(QGraphicsSceneMouseEvent *event);
    bool isLampMove();
    bool isLampRotate();
    bool isLampLocatedInBorder();
    bool isLampLeftBorderCorrect();
    bool isLampRightBorderCorrect();
    bool isLampBottomBorderCorrect();
    bool isLampTopBorderCorrect();

    QPointF mInitialPos;
    QPointF mInitialCenter;

    int mLampId;

    qreal mX;
    qreal mY;
    qreal mWidth;
    qreal mHeight;
    qreal mAngle;

    qreal mOldX;
    qreal mOldY;
    qreal mOldAngle;

    LampLight *mLampLight;

    bool mIsDeleted;

    qreal mSceneWidth = 600;
    qreal mSceneHeight = 520;
    int mZindex;
};

#endif // LAMP_H
