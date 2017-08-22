#ifndef MEMENTO_H
#define MEMENTO_H

#include <QObject>
#include <QColor>

class Lamp;

class Memento
{
public:
    Memento(int id, qreal x, qreal y, qreal width, qreal height, qreal angle,
            QColor lampLightColor, qreal lampLightWidth, qreal lampLightHeight);
    Memento();
    int id();

private:
    friend class Lamp;

    int   mLampId;
    qreal mX;
    qreal mY;
    qreal mWidth;
    qreal mHeight;
    qreal mAngle;

    QColor mLampLightColor;
    qreal mLampLightWidth;
    qreal mLampLightHeight;


};

#endif // MEMENTO_H
