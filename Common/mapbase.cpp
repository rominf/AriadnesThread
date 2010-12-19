#include "mapbase.h"

MapBase::MapBase(QPixmap &pixmap, QRectF sceneRect):
        QGraphicsPixmapItem(pixmap)
{
    setZValue(-1);
    setOpacity(0.5);
    setPixmap(pixmap, sceneRect);
}

void MapBase::setPixmap(const QPixmap &pixmap, QRectF sceneRect)
{
    if (!pixmap.isNull())
    {
        QGraphicsPixmapItem::setPixmap(pixmap);
        qreal k, ky, kx, x, y;
        kx = sceneRect.width()/boundingRect().width();
        ky = sceneRect.height()/boundingRect().height();
        if (kx < ky)
        {
            k = kx;
            x = 0;
            y = (sceneRect.height() - k*boundingRect().height())/2;
        }
        else
        {
            k = ky;
            x = (sceneRect.width() - k*boundingRect().width())/2;
            y = 0;
        }
        setScale(k);
        setPos(x, y);
    }
}


