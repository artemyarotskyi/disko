#include "Lamp.h"
#include <QAbstractGraphicsShapeItem>
#include <QDebug>
#include <QColor>

void rotateItem(Lamp *lamp, QPointF center1, QPointF moved, QPointF initial_pos)
{
    QRectF bbox = lamp->boundingRect().normalized();
    QPointF center = bbox.center();

    qreal init_x = initial_pos.x() - center1.x();
    qreal init_y = initial_pos.y() - center1.y();
    qreal initial_angle = std::atan2(init_y, init_x);
    qreal x = moved.x() - center1.x();
    qreal y = moved.y() - center1.y();

    qreal mv_angle = std::atan2(y, x);

    qreal angle = (mv_angle - initial_angle) * 180 / 3.14159265359;

    if(std::fabs(angle) > 360.0)
    {
        angle = 0.0;
    }    

    lamp->SetLampAngle(angle);

    QTransform xForm;
    xForm.translate(center.x(), center.y());
    xForm.rotate(angle);
    xForm.translate(-center.x(), -center.y());

    lamp->setTransform(xForm, false);
}

Lamp::Lamp(qreal x, qreal y, qreal width, qreal height, int id, qreal lightWidth, qreal lightHeight):
    QGraphicsRectItem(x, y, width, height),
    mLampId(id),
    mX(x),
    mY(y),
    mWidth(width),
    mHeight(height),
    mAngle(0),
    mLampLight(new LampLight(this, lightWidth, lightHeight)),
    mZindex(0)
{
    mInitialPos.setX(0.0);
    mInitialPos.setY(0.0);

    mInitialCenter.setX(0.0);
    mInitialCenter.setY(0.0);

    this->setFlags(QGraphicsItem::ItemIsSelectable);
}

Lamp::Lamp() : mLampLight(new LampLight(this)), mZindex(0)
{

}

void Lamp::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit clickCamera(mLampId);
        this->setZValue(++mZindex);        
        if(event->modifiers() == Qt::ShiftModifier)
        {
            mInitialPos = event->scenePos();
            mInitialCenter = this->sceneBoundingRect().normalized().center();
        }
        else
        {
            QAbstractGraphicsShapeItem::mousePressEvent(event);
        }
    }
}

void Lamp::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QAbstractGraphicsShapeItem::mouseReleaseEvent(event);
}

void Lamp::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        if(event->modifiers() == Qt::ShiftModifier)
        {
            rotateItem(this, mInitialCenter, event->scenePos(), mInitialPos);
            event->accept();
        }
        else
        {
            if((this->x() >= 0) && (this->y() >= 0) &&
               (this->x() + 53 <= 601) && (this->y() + 53 <= 480))
            {
                QAbstractGraphicsShapeItem::mouseMoveEvent(event);
                mX = this->x();
                mY = this->y();
            }
            if(this->x() < 0) this->setX(1);
            if(this->y() < 0) this->setY(1);
            if(this->x() + 53 > 601) this->setX(601 - 54);
            if(this->y() + 53 > 480) this->setY(480 - 54);
        }
    }
}

qreal Lamp::lampXCoordinate() const
{
    return mX;
}

void Lamp::setLampXCoordinate(qreal x)
{
    mX = x;
}

qreal Lamp::lampYCoordinate() const
{
    return mY;
}

void Lamp::setLampYCoordinate(qreal y)
{
    mY = y;
}

qreal Lamp::lampWidth() const
{
    return mWidth;
}

qreal Lamp::lampHeight() const
{
    return mHeight;
}

qreal Lamp::lampAngle() const
{
    return mAngle;
}

void Lamp::SetLampAngle(qreal angle)
{
    mAngle = angle;
}

LampLight* Lamp::lampLight()
{
    return mLampLight;
}

int Lamp::lampId() const
{
    return mLampId;
}

void Lamp::setLampLightWidth(qreal width)
{
    mLampLight->setLightWidth(width);
}

qreal Lamp::lampLightWidth() const
{
    return mLampLight->lightWidth();
}

void Lamp::setLampLightHeight(qreal height)
{
    mLampLight->setLightHeight(height);
}

qreal Lamp::lampLightHeight() const
{
    return mLampLight->lightHeight();
}

QColor Lamp::lampLightColor() const
{
    return mLampLight->lightColor();
}

void Lamp::read(const QJsonObject &json)
{
    mLampId =   json["lampId"].toInt();
    mX =        json["x"].toDouble();
    mY =        json["y"].toDouble();
    mWidth =    json["width"].toDouble();
    mHeight =   json["height"].toDouble();
    mAngle=     json["angle"].toDouble();
    const QColor color(json["color"].toString());
    mLampLight->setLampLightColor(color);
    mLampLight->setLightWidth(json["lightWidth"].toDouble());
    mLampLight->setLightHeight(json["lightHeight"].toDouble());
}

void Lamp::write(QJsonObject &json) const
{
    json["lampId"] =        mLampId;
    json["x"] =             mX;
    json["y"] =             mY;
    json["width"] =         mWidth;
    json["height"] =        mHeight;
    json["angle"] =         mAngle;
    json["color"] =         mLampLight->lightColor().name();
    json["lightWidth"] =    mLampLight->lightWidth();
    json["lightHeight"] =   mLampLight->lightHeight();
}
