#ifndef MAPBASE_H
#define MAPBASE_H

#include <qmath.h>
#include <QBrush>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>

class MapBase : public QGraphicsPixmapItem
{
public:
    MapBase(QPixmap &pixmap, QRectF sceneRect);
    void setPixmap(const QPixmap &pixmap, QRectF sceneRect);

//protected:
//    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
//    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

//private:
//    const qreal cResizingHandleWidth;
//    QRectF *resizingHandle;
//    bool resizingHandlePressed;
//    qreal mousePressOffset;
};

#endif // MAPBASE_H
