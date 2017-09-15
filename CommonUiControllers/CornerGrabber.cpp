#include "CornerGrabber.h"

CornerGrabber::CornerGrabber(QGraphicsItem *parent, int corner):
    QGraphicsItem(parent),
    mouseDownX(0),
    mouseDownY(0),
    mOutterborderColor(Qt::white),
    mOutterborderPen(),
    mWidth(3),
    mHeight(3),
    mCorner(corner),
    mMouseButtonState(kMouseReleased)
{
    setParentItem(parent);

    mOutterborderPen.setWidth(2);
    mOutterborderPen.setColor(mOutterborderColor);

    this->setAcceptHoverEvents(true);
}

int CornerGrabber::getCorner()
{
    return mCorner;
}

void CornerGrabber::setMouseState(int s)
{
    mMouseButtonState = s;
}

int CornerGrabber::getMouseState()
{
    return mMouseButtonState;
}

QRectF CornerGrabber::boundingRect() const
{
    return QRectF(0, 0, mWidth, mHeight);
}

void CornerGrabber::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    mOutterborderPen.setCapStyle(Qt::SquareCap);
    mOutterborderPen.setStyle(Qt::SolidLine);
    painter->setPen(mOutterborderPen);

    QPointF topLeft(0, 0);
    QPointF bottomRight(mWidth, mHeight);
    QRectF rect(topLeft, bottomRight);

    QBrush brush(Qt::SolidPattern);
    brush.setColor(mOutterborderColor);

    painter->fillRect(rect, brush);
}

void CornerGrabber::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
    mOutterborderColor = QColor(247, 50, 32); //145, 6, 99;
    this->update(0, 0, mWidth, mHeight);
}

void CornerGrabber::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    mOutterborderColor = Qt::white;
    this->update(0, 0, mWidth, mHeight);
}

void CornerGrabber::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted(false);
}

void CornerGrabber::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void CornerGrabber::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted(false);
}

void CornerGrabber::mousePressEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void CornerGrabber::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted(true);
}
