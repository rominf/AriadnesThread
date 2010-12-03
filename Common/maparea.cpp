#include "maparea.h"

MapArea::MapArea(int areaNumber, const QPolygonF &polygon):
        QGraphicsPolygonItem(polygon), number(areaNumber)
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

int MapArea::getNumber()
{
    return number;
}

void MapArea::setNumber(int i)
{
    number = i;
}

void MapArea::addDoor(QGraphicsLineItem *door)
{
    doors.append(door);
}

int MapArea::type() const
{
    return Type;
}
