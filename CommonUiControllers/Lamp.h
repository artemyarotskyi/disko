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

public:
    Lamp(qreal x, qreal y, qreal width, qreal height);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    QPointF mInitialPos;
    QPointF mInitialCenter;

    int mZindex;

    LampLight *mLampLight;
};

#endif // LAMP_H
