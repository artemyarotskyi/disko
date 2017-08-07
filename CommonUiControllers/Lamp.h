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

class Lamp : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    Lamp(qreal x, qreal y, qreal width, qreal height, int id);
    Lamp();

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    qreal lampXCoordinate() const;
    qreal lampYCoordinate() const;
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

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

signals:
    void clickCamera(int);

private:
    QPointF mInitialPos;
    QPointF mInitialCenter;

    int mLampId;

    qreal mX;
    qreal mY;
    qreal mWidth;
    qreal mHeight;
    qreal mAngle;

    LampLight *mLampLight;

    int mZindex;
};

#endif // LAMP_H
