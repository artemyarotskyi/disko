#include "LampLight.h"

#include "math.h"

LampLight::LampLight():
    mOutterBorderColor(Qt::black),
    mOutterBorderPen(),
    mLocation(0, 0),
    mDragStart(0, 0),
    mWidth(26),
    mHeight(40),
    mCornerDragStart(0, 0),
    mXCornerGrabBuffer(4),
    mYCornerGrabBuffer(4),
    mDrawingWidth(mWidth - (mXCornerGrabBuffer*2)),
    mDrawingHeight(mHeight - mYCornerGrabBuffer),
    mDrawingOriginX(0),
    mDrawingOriginY(0)
{
    mOutterBorderPen.setWidth(1);
    mOutterBorderPen.setColor(mOutterBorderColor);

    this->setAcceptHoverEvents(true);
}

QRectF LampLight::boundingRect() const
{
    return QRectF(0, 0, mWidth, mHeight);
}

void LampLight::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QPolygon polygon;    

    polygon << QPoint(mDrawingWidth / 2, mDrawingOriginY) << QPoint (mDrawingOriginX, mDrawingHeight) << QPoint(mDrawingWidth, mDrawingHeight);
    painter->setBrush(Qt::yellow);
    painter->drawPolygon(polygon);
}

void LampLight::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
    mOutterBorderColor = Qt::red;

    mCorners[0] = new CornerGrabber(this, 0);
    mCorners[0]->installSceneEventFilter(this);    

    setCornerPosition();
}

void LampLight::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    mOutterBorderColor = Qt::black;

    mCorners[0]->setParentItem(nullptr);
    delete mCorners[0];   
}

void LampLight::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void LampLight::mousePressEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

bool LampLight::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    CornerGrabber *corner = dynamic_cast<CornerGrabber*>(watched);
    if(corner == nullptr)
    {
        return false;
    }

    QGraphicsSceneMouseEvent *mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if(mevent == nullptr)
    {
        return false;
    }

    switch(event->type())
    {
        case QEvent::GraphicsSceneMousePress:
        {
            corner->setMouseState(CornerGrabber::kMouseDown);
            corner->mouseDownX = mevent->pos().x();
            corner->mouseDownY = mevent->pos().y();
        }
        break;

        case QEvent::GraphicsSceneMouseRelease:
        {
            corner->setMouseState(CornerGrabber::kMouseReleased);
        }
        break;

        case QEvent::GraphicsSceneMouseMove:
        {
            corner->setMouseState(CornerGrabber::kMouseMoving);
        }
        break;

        default:
            return false;
        break;
    }

    if(corner->getMouseState() == CornerGrabber::kMouseMoving)
    {
        qreal x = mevent->pos().x();
        qreal y = mevent->pos().y();

        int XaxisSign = 0;
        int YaxisSign = 0;

        XaxisSign = +1;
        YaxisSign = -1;

        int xMoved = corner->mouseDownX - x;
        int yMoved = corner->mouseDownY - y;

        int newWidth = mWidth + (XaxisSign * xMoved);
        if(newWidth < 27)
        {
            newWidth = 27;
        }
        if(newWidth > 200)
        {
            newWidth = 200;
        }

        int newHeight = mHeight + (YaxisSign * yMoved);
        if(newHeight < 40)
        {
            newHeight = 40;
        }
        if(newHeight > 200)
        {
            newHeight = 200;
        }

        int deltaWidth = newWidth - mWidth;
        int deltaHeight = newHeight - mHeight;

        adjustSize(deltaWidth, deltaHeight);

        deltaWidth *= (-1);
        deltaHeight *= (-1);

        int newXpos = this->pos().x() + deltaWidth;
        this->setPos(newXpos,this->pos().y());

        setCornerPosition();

        this->update();
    }

    return true;

}

void LampLight::setCornerPosition()
{
    mCorners[0]->setPos(mDrawingOriginX - mXCornerGrabBuffer, mDrawingHeight);
}

void LampLight::adjustSize(int x, int y)
{
    mWidth += x;
    mHeight += y;

    mDrawingWidth = mWidth - mXCornerGrabBuffer;
    mDrawingHeight = mHeight - mYCornerGrabBuffer;
}
