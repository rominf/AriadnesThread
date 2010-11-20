#include "maparea.h"

MapArea::MapArea(const QPolygonF &polygon):
        QGraphicsPolygonItem(polygon)
{
}

QDataStream &operator<<(QDataStream &output, const MapArea &area)
{
    QList<QPointF> list;
    list = area.polygon().toList();
    output << list;
    return output;
}

QDataStream &operator>>(QDataStream &input, MapArea &area)
{
    QList<QPointF> list;
    input >> list;
    QPolygonF polygon(list.toVector());
    area.setPolygon(polygon);
    return input;
}

/*void MapArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{


}*/
