#ifndef CORNERGRABBER_H
#define CORNERGRABBER_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>

class CornerGrabber : public QGraphicsItem
{
public:
    CornerGrabber(QGraphicsItem *parent = 0, int corner = 0);

    int getCorner();
    void setMouseState(int s);
    int getMouseState();

    qreal mouseDownX;
    qreal mouseDownY;

    enum {kMouseReleased = 0, kMouseDown, kMouseMoving};

private:
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    QColor mOutterborderColor;
    QPen mOutterborderPen;

    qreal mWidth;
    qreal mHeight;

    int mCorner;

    int mMouseButtonState;
};

#endif // CORNERGRABBER_H
