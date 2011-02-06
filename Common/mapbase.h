#ifndef MAPBASE_H
#define MAPBASE_H

#include <qmath.h>
#include <QBrush>
#include <QFile>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>

class MapBase : public QGraphicsPixmapItem
{
public:
    static const QString cFakeFileName;

    MapBase(QString &fileName, QRectF sceneRect);

    void setPixmap(QString &fileName, QRectF sceneRect);
    QString fileName() const;

private:
    QString m_fileName;
};

#endif // MAPBASE_H
