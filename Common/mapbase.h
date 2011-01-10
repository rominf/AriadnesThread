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
    MapBase(QString &fileName, QRectF sceneRect);

    static const QString cFakeFileName;

    void setPixmap(QString &fileName, QRectF sceneRect);
    QString fileName() const;
//    void setVisible(bool Visible);

//protected:
//    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
//    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    QString m_fileName;
//    const qreal cResizingHandleWidth;
//    QRectF *resizingHandle;
//    bool resizingHandlePressed;
//    qreal mousePressOffset;
};

#endif // MAPBASE_H
