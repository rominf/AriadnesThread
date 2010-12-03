#include "mapbase.h"

MapBase::MapBase(QPixmap &pixmap, QRectF sceneRect):
        QGraphicsPixmapItem(pixmap)/*, cResizingHandleWidth(20.0),
        resizingHandlePressed(false)*/
{
    setZValue(-1.0);
    setOpacity(0.5);
    setPixmap(pixmap, sceneRect);

//    setFlags(QGraphicsItem::ItemIsMovable);
//    resizingHandle = new QGraphicsRectItem(
//                boundingRect().right() - 2*cResizingHandleWidth,
//                boundingRect().bottom() - 2*cResizingHandleWidth,
//                2*cResizingHandleWidth, 2*cResizingHandleWidth);
//    resizingHandle = new QGraphicsRectItem(
//            boundingRect().right() - 2*cResizingHandleWidth,
//            boundingRect().bottom() - 2*cResizingHandleWidth,
//            2*cResizingHandleWidth, 2*cResizingHandleWidth, this, 0);
//    resizingHandle->setBrush(QBrush(Qt::white));
//    resizingHandle->setFlag(QGraphicsItem::ItemIsMovable);
//    resizingHandle->setHandlesChildEvents(true);

//    setFiltersChildEvents(true);
}

//void MapBase::mousePressEvent(QGraphicsSceneMouseEvent *event)
//{
//    event->accept();
//    if (resizingHandle->contains(event->pos()))
//    {
//        resizingHandlePressed = true;
//        mousePressOffset = bottomRight() - event->pos();
//    }
//}

//void MapBase::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
//{
//    event->accept();
//    if (event->buttons() = Qt::LeftButton)
//    {
//        if (resizingHandlePressed)
//        {
//            transform.scale(
//                    ((boundingRect().width() + mousePressOffset)
//                     /boundingRect().width(),
//                     (boundingRect().height() + mousePressOffset)
//                                          /boundingRect().height()));
//        }
////        if (resizingHandle->contains(event->pos()))
////        {
////            resizingHandle->translate(event->scenePos().x() -
////                                      resizingHandle->x(),
////                                      event->scenePos().y() -
////                                      resizingHandle->y());
////            QTransform transform;
////            transform.scale(
////                    (event->scenePos().x() -
////                     boundingRect().left())/boundingRect().width(),
////                    (event->scenePos().y() -
////                     boundingRect().top())/boundingRect().height());
////            setTransform(transform);
////            //update();
////        }

//    }
//    QGraphicsPixmapItem::mouseMoveEvent(event);
//}

void MapBase::setPixmap(const QPixmap &pixmap, QRectF sceneRect)
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


