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
#include <CommonUiControllers/LampLight.h>

class Lamp : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    Lamp(qreal x, qreal y, qreal width, qreal height, int id);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    LampLight *getLampLight();
    int getLampId() const;

signals:
    void clickCamera(int);

private:
    QPointF mInitialPos;
    QPointF mInitialCenter;

    int mLampId;

    LampLight *mLampLight;

    int mZindex;


};

#endif // LAMP_H
