#include "LampLight.h"

#include "math.h"

LampLight::LampLight(QGraphicsRectItem *parent, qreal width, qreal height):
    mOutterBorderColor(Qt::black),
    mOutterBorderPen(),
    mLocation(0, 0),
    mDragStart(0, 0),
    mColor(QColor(39, 38, 44)),
    mWidth(width),
    mHeight(height),
    mCornerDragStart(0, 0),
    mXCornerGrabBuffer(4),
    mYCornerGrabBuffer(4),
    mDrawingWidth(mWidth - mXCornerGrabBuffer),
    mDrawingHeight(mHeight - mYCornerGrabBuffer),
    mDrawingOriginX(0),
    mDrawingOriginY(0)
{
    mOutterBorderPen.setWidth(1);
    mOutterBorderPen.setColor(mOutterBorderColor);

    this->setParentItem(parent);

    this->setAcceptHoverEvents(true);
}

QRectF LampLight::boundingRect() const
{
    return QRectF(0, 0, mWidth, mHeight);
}

void LampLight::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QPolygon polygon;

    polygon << QPoint(mWidth / 2, mDrawingOriginY)
            << QPoint (mDrawingOriginX + mXCornerGrabBuffer, mDrawingHeight)
            << QPoint(mDrawingWidth, mDrawingHeight);
    painter->setBrush(mColor);
    setLightCenterPosition();
    painter->drawPolygon(polygon);
}

qreal LampLight::lightWidth() const
{
    return mWidth;
}

void LampLight::setLightWidth(qreal width)
{
    mWidth = width;
}

qreal LampLight::lightHeight() const
{
    return mHeight;
}

void LampLight::setLightHeight(qreal height)
{
    mHeight = height;
}

QColor LampLight::lightColor() const
{
    return mColor;
}

void LampLight::setLampLightColor(QColor color)
{
    mColor = color;
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
    if(isCornerGrabberNotExist(corner))
    {
        return false;
    }

    QGraphicsSceneMouseEvent *mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if(isGraphicsSceneMouseEventNotExist(mevent))
    {
        return false;
    }

    switch(event->type())
    {
        case QEvent::GraphicsSceneMousePress:
        {
            mOldWidth = mWidth;
            mOldHeight = mHeight;

            corner->setMouseState(CornerGrabber::kMouseDown);
            corner->mouseDownX = mevent->pos().x();
            corner->mouseDownY = mevent->pos().y();
        }
        break;

        case QEvent::GraphicsSceneMouseRelease:
        {
            corner->setMouseState(CornerGrabber::kMouseReleased);

            if(isHeightChange()||isWidthChange())
                emit lightSizeChanged();
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
        calculateLampLightNewPosition(mevent, corner);

        setCornerPosition();
        setLightCenterPosition();

        this->update();        
    }

    return true;
}

void LampLight::calculateLampLightNewPosition(QGraphicsSceneMouseEvent *mevent, CornerGrabber *corner)
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
    if(isMinimumWidth(newWidth))
    {
        newWidth = mMinimumWidth;
    }

    int newHeight = mHeight + (YaxisSign * yMoved);
    if(isMinimumHeight(newHeight))
    {
        newHeight = mMinimumHeight;
    }

    int deltaWidth = newWidth - mWidth;
    int deltaHeight = newHeight - mHeight;

    adjustSize(deltaWidth, deltaHeight);

    deltaWidth *= (-1);
    deltaHeight *= (-1);

    int newXpos = this->pos().x() + deltaWidth;
    this->setPos(newXpos,this->pos().y());
}

void LampLight::setCornerPosition()
{
    mCorners[0]->setPos(mDrawingOriginX + 1, mDrawingHeight);
}

void LampLight::setLightCenterPosition()
{
    this->setPos((53 - mWidth) / 2, 53);
}

void LampLight::adjustSize(int x, int y)
{
    mWidth += x;
    mHeight += y;

    mDrawingWidth = mWidth - mXCornerGrabBuffer;
    mDrawingHeight = mHeight - mYCornerGrabBuffer;
}

bool LampLight::isHeightChange()
{
    return (mHeight != mOldHeight);
}

bool LampLight::isWidthChange()
{
    return (mWidth != mOldWidth);
}

bool LampLight::isCornerGrabberNotExist(CornerGrabber *corner)
{
    return (corner == nullptr);
}

bool LampLight::isGraphicsSceneMouseEventNotExist(QGraphicsSceneMouseEvent *mevent)
{
    return (mevent == nullptr);
}

bool LampLight::isMinimumWidth(int newWidth)
{
    return (newWidth < mMinimumWidth);
}

bool LampLight::isMinimumHeight(int newHeight)
{
    return (newHeight < mMinimumHeight);
}
