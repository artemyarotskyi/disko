#ifndef LAMPLIGHT_H
#define LAMPLIGHT_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QPolygon>
#include "CommonUiControllers/CornerGrabber.h"

class LampLight :public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    LampLight(QGraphicsRectItem *parent = 0, qreal width = 42, qreal height = 40);   
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override;    

    qreal lightWidth() const;
    void setLightWidth(qreal width);

    qreal lightHeight() const;
    void setLightHeight(qreal height);

    QColor lightColor() const;
    void setLampLightColor(QColor color);

signals:
    void lightSizeChanged();

private:
    QRectF boundingRect() const override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent*) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;

    virtual void mouseMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent(QGraphicsSceneDragDropEvent *event);
    bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;

    void calculateLampLightNewPosition(QGraphicsSceneMouseEvent *mevent, CornerGrabber *corner);
    void setCornerPosition();
    void setLightCenterPosition();
    void adjustSize(int x, int y);
    bool isHeightChange();
    bool isWidthChange();
    bool isCornerGrabberNotExist(CornerGrabber *corner);
    bool isGraphicsSceneMouseEventNotExist(QGraphicsSceneMouseEvent *mevent);
    bool isMinimumWidth(int newWidth);
    bool isMinimumHeight(int newHeight);

    QColor mOutterBorderColor;
    QPen mOutterBorderPen;
    QPointF mLocation;
    QPointF mDragStart;

    QColor mColor;

    qreal mWidth;
    qreal mHeight;

    qreal mOldWidth;
    qreal mOldHeight;

    QPointF mCornerDragStart;

    int mXCornerGrabBuffer;
    int mYCornerGrabBuffer;
    qreal mDrawingWidth;
    qreal mDrawingHeight;
    qreal mDrawingOriginX;
    qreal mDrawingOriginY;

    CornerGrabber *mCorners[2];

    const qreal mMinimumWidth = 28;
    const qreal mMinimumHeight = 40;
};

#endif // LAMPLIGHT_H
