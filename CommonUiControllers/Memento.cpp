#include "Memento.h"

Memento::Memento(int id, qreal x, qreal y, qreal width, qreal height, qreal angle,
                 QColor lampLightColor, qreal lampLightWidth, qreal lampLightHeight) :
    mLampId(id),
    mX(x),
    mY(y),
    mWidth(width),
    mHeight(height),
    mAngle(angle),
    mLampLightColor(lampLightColor),
    mLampLightWidth(lampLightWidth),
    mLampLightHeight(lampLightHeight)
{
}

Memento::Memento()
{

}

int Memento::id()
{
    return mLampId;
}
