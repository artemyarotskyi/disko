#include "Memento.h"

Memento::Memento(int id, qreal x, qreal y, qreal width, qreal height, qreal angle,
                 QColor lampLightColor, qreal lampLightWidth, qreal lampLightHeight, bool lampIsDeleted) :
    mLampId(id),
    mX(x),
    mY(y),
    mWidth(width),
    mHeight(height),
    mAngle(angle),
    mLampLightColor(lampLightColor),
    mLampLightWidth(lampLightWidth),
    mLampLightHeight(lampLightHeight),
    mLampIsDeleted(lampIsDeleted)
{
}

Memento::Memento()
{

}

int Memento::id()
{
    return mLampId;
}

bool Memento::isDeleted()
{
    return mLampIsDeleted;
}
