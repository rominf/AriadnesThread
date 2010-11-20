#ifndef MAPAREA_H
#define MAPAREA_H

#include <QGraphicsPolygonItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

class MapArea : public QGraphicsPolygonItem
{
    friend QDataStream &operator<<(QDataStream &output, const MapArea &area);
    friend QDataStream &operator>>(QDataStream &input, MapArea &area);

public:
    explicit MapArea(const QPolygonF &polygon);
/*protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
private:
    QGraphicsEllipseItem *tempCircle;*/
};

#endif // MAPAREA_H
