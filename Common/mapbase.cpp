#include "mapbase.h"

const QString MapBase::cFakeFileName = "/this/file/mustn't/exist";

MapBase::MapBase(QString &fileName, QRectF sceneRect):
        QGraphicsPixmapItem()
{
//    m_fileName = "";
    m_fileName = cFakeFileName;
    setZValue(-1);
    setOpacity(0.5);
    setPixmap(fileName, sceneRect);
}

void MapBase::setPixmap(QString &fileName, QRectF sceneRect)
{
    QPixmap pixmap;
    if (QFile::exists(fileName))
        pixmap = QPixmap(fileName);
    if (!pixmap.isNull())
    {
        m_fileName = fileName;
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
    else
        m_fileName = cFakeFileName;
}

QString MapBase::fileName() const
{
    return m_fileName;
}


