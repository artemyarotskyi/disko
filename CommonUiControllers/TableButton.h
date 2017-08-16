#ifndef TABLEBUTTON_H
#define TABLEBUTTON_H

#include <QPushButton>
#include <QObject>
#include <QWidget>
#include <QMouseEvent>

class TableButton : public QPushButton
{
    Q_OBJECT

protected:
    void mousePressEvent(QMouseEvent *e)
    {
        if(e->button() == Qt::LeftButton)
        {
            emit clickTableButton(mId);
        }
    }

signals:
    void clickTableButton(int id);

public:
    TableButton(int id, QWidget *parent = 0) :
        mId(id),
        QPushButton(parent)
    {}

private:
    int mId;
};

#endif // TABLEBUTTON_H
