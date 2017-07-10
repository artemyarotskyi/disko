#include "Lamp.h"
#include <QAbstractGraphicsShapeItem>


void rotateItem(QAbstractGraphicsShapeItem *shape, QPointF center1, QPointF moved, QPointF initial_pos)
{
    QRectF bbox = shape->boundingRect().normalized();
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

    QTransform xForm;
    xForm.translate(center.x(), center.y());
    xForm.rotate(angle);
    xForm.translate(-center.x(), -center.y());

    shape->setTransform(xForm, false);

}

Lamp::Lamp(qreal x, qreal y, qreal width, qreal height):
    QGraphicsRectItem(x, y, width, height),
    mLampLight(new LampLight(this)),
    mZindex(0)
{
    mInitialPos.setX(0.0);
    mInitialPos.setY(0.0);

    mInitialCenter.setX(0.0);
    mInitialCenter.setY(0.0);

    this->setFlags(QGraphicsItem::ItemIsSelectable);

    mLampLight->setPos(0,53);
}

void Lamp::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
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
            QAbstractGraphicsShapeItem::mouseMoveEvent(event);
        }
    }
}
