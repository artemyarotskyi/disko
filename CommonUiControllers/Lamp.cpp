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
    mZindex(0),
    mIsDeleted(false)
{
    mInitialPos.setX(0.0);
    mInitialPos.setY(0.0);

    mInitialCenter.setX(0.0);
    mInitialCenter.setY(0.0);

    this->setFlags(QGraphicsItem::ItemIsSelectable);

    connect(mLampLight, SIGNAL(lightSizeChanged()), this, SLOT(emitLampLightSizeChanges()));
}

Lamp::Lamp() : mLampLight(new LampLight(this)), mZindex(0)
{

}

void Lamp::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    mOldX = mX;
    mOldY = mY;
    mOldAngle = mAngle;

    if(isLeftButtonPressed(event))
    {
        emit clickCamera(mLampId);
        this->setZValue(++mZindex);

        if(isShiftPressed(event))
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

    if(isLampMove())
        emit lampMove(this);

    if(isLampRotate())
        emit lampRotate(this);
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

            if(isLampLocatedInBorder())
            {
                QAbstractGraphicsShapeItem::mouseMoveEvent(event);
            }
            if(isLampLeftBorderCorrect()) this->setX(1);
            if(isLampTopBorderCorrect()) this->setY(1);
            if(isLampRightBorderCorrect()) this->setX(mSceneWidth - mWidth);
            if(isLampBottomBorderCorrect()) this->setY(mSceneHeight - mHeight);

            mX = this->x();
            mY = this->y();
        }        
    }    
}

Memento* Lamp::createMemento()
{    
    return new Memento(mLampId, mX, mY, mWidth, mHeight, mAngle,
                       mLampLight->lightColor(), mLampLight->lightWidth(), mLampLight->lightHeight(), mIsDeleted);
}

void Lamp::reinstateMemento(Memento memento)
{
     mLampId = memento.mLampId;
     mX = memento.mX;
     mY = memento.mY;
     mWidth = memento.mWidth;
     mHeight = memento.mHeight;
     mAngle = memento.mAngle;

     mLampLight->setLampLightColor(memento.mLampLightColor);
     mLampLight->setLightWidth(memento.mLampLightWidth);
     mLampLight->setLightHeight(memento.mLampLightHeight);

     mIsDeleted = memento.mLampIsDeleted;
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

void Lamp::setLampIsDeleted(bool isDeleted)
{
    mIsDeleted = isDeleted;
}

bool Lamp::lampIsDeleted() const
{
    return mIsDeleted;
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

void Lamp::emitLampLightSizeChanges()
{
    emit lampLightSizeChange(this);
}

bool Lamp::isLeftButtonPressed(QGraphicsSceneMouseEvent *event)
{
    return (event->button() == Qt::LeftButton);
}

bool Lamp::isShiftPressed(QGraphicsSceneMouseEvent *event)
{
    return (event->modifiers() == Qt::ShiftModifier);
}

bool Lamp::isLampMove()
{
    return ((mOldX != mX) || (mOldY != mY));
}

bool Lamp::isLampRotate()
{
    return (mOldAngle != mAngle);
}

bool Lamp::isLampLocatedInBorder()
{
    return ((this->x() >= 0) && (this->y() >= 0) &&
            (this->x() + mWidth <= mSceneWidth) && (this->y() + mHeight <= mSceneHeight));
}

bool Lamp::isLampLeftBorderCorrect()
{
    return(this->x() < 0);
}

bool Lamp::isLampRightBorderCorrect()
{
    return(this->x() + mWidth > mSceneWidth);
}

bool Lamp::isLampBottomBorderCorrect()
{
    return(this->y() + mHeight > mSceneHeight);
}

bool Lamp::isLampTopBorderCorrect()
{
    return(this->y() < 0);
}

