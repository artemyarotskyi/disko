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
    mXCornerGrabBuffer(3),
    mYCornerGrabBuffer(3),
    mDrawingWidth(mWidth - (mXCornerGrabBuffer*2)),
    mDrawingHeight(mHeight - mYCornerGrabBuffer),
    mDrawingOriginX(mXCornerGrabBuffer),
    mDrawingOriginY(mYCornerGrabBuffer)
{
    mOutterBorderPen.setWidth(1);
    mOutterBorderPen.setColor(mOutterBorderColor);

    this->setAcceptHoverEvents(true);
}

QRectF LampLight::boundingRect() const
{
    return QRectF(0, 0, mWidth, mHeight);
}

void LampLight::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPolygon polygon;    

    polygon << QPoint(mDrawingOriginX, mDrawingOriginY) << QPoint (mDrawingWidth, mDrawingOriginY) << QPoint(/*10, mDrawingHeight*/mDrawingWidth / 2, mDrawingHeight);
    painter->setBrush(Qt::red);
    painter->drawPolygon(polygon);
}

void LampLight::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    mOutterBorderColor = Qt::red;

    mCorners[0] = new CornerGrabber(this, 0);
    mCorners[1] = new CornerGrabber(this, 1);

    mCorners[0]->installSceneEventFilter(this);
    mCorners[1]->installSceneEventFilter(this);

    setCornerPosition();
}

void LampLight::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    mOutterBorderColor = Qt::black;

    mCorners[0]->setParentItem(nullptr);
    mCorners[1]->setParentItem(nullptr);

    delete mCorners[0];
    delete mCorners[1];
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
        switch(corner->getCorner())
        {
            case 0:
            {
                XaxisSign = +1;
                YaxisSign = +1;
            }
            break;

            case 1:
            {
                XaxisSign = -1;
                YaxisSign = +1;
            }
            break;            
        }

        int xMoved = corner->mouseDownX - x;
        int yMoved = corner->mouseDownY - y;

        int newWidth = mWidth + (XaxisSign * xMoved);
        if(newWidth < 40)
        {
            newWidth = 40;
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

        if(corner->getCorner() == 0)
        {
            int newXpos = this->pos().x() + deltaWidth;
            int newYpos = this->pos().y() + deltaHeight;
            this->setPos(newXpos, newYpos);
        }
        else if(corner->getCorner() == 1)
        {
            int newYpos = this->pos().y() + deltaHeight;
            this->setPos(this->pos().x(), newYpos);
        }

        setCornerPosition();

        this->update();

    }

    return true;

}

void LampLight::setCornerPosition()
{
    mCorners[0]->setPos(0, 0);
    mCorners[1]->setPos(mDrawingWidth, 0);
}

void LampLight::adjustSize(int x, int y)
{
    mWidth += x;
    mHeight += y;

    mDrawingWidth = mWidth - mXCornerGrabBuffer;
    mDrawingHeight = mHeight - mYCornerGrabBuffer;
}
