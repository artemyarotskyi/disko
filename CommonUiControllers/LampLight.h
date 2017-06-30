#ifndef LAMPLIGHT_H
#define LAMPLIGHT_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QPolygon>
#include "CommonUiControllers/CornerGrabber.h"

class LampLight : public QGraphicsItem
{
public:
    LampLight();

private:
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent*);

    virtual void mouseMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent(QGraphicsSceneDragDropEvent *event);
    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event);

    void setCornerPosition();
    void adjustSize(int x, int y);

    QColor mOutterBorderColor;
    QPen mOutterBorderPen;
    QPointF mLocation;
    QPointF mDragStart;
    qreal mWidth;
    qreal mHeight;

    QPointF mCornerDragStart;

    int mXCornerGrabBuffer;
    int mYCornerGrabBuffer;
    qreal mDrawingWidth;
    qreal mDrawingHeight;
    qreal mDrawingOriginX;
    qreal mDrawingOriginY;

    CornerGrabber *mCorners[2];

};

#endif // LAMPLIGHT_H
