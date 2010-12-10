#include "maparea.h"

MapArea::MapArea(const QPolygonF &polygon):
        QGraphicsPolygonItem(polygon)
{
//    setFlags(QGraphicsItem::ItemIsSelectable);
    setBrush(QBrush(Qt::white));
}

QDataStream &operator<<(QDataStream &output, const MapArea &area)
{

    int last = area.m_areas.size();
    output << last;
    for (int i = 0; i != last; i++)
    {
        output << *area.m_areas.at(i);
    }

    QList<QPointF> list;
    list = area.polygon().toList();
    output << list;
    return output;
}

QDataStream &operator>>(QDataStream &input, MapArea &area)
{
    int last;
    input >> last;
    for (int i = 0; i != last; i++)
    {
        MapArea *tempArea =  new MapArea(0);
        input >> *tempArea;
        area.addArea(tempArea);
//        area.m_areas.append(tempArea);
    }

    QList<QPointF> list;
    input >> list;
    QPolygonF polygon(list.toVector());
    area.setPolygon(polygon);
    return input;
}

MapArea* MapArea::parent()
{
    return m_parent;
}

//void MapArea::setParent(MapArea *parent)
//{
//    m_parent = this;
//}

//int MapArea::number()
//{
//    if (m_parent != 0)
//        return m_parent->
////    return m_number;
//}

//void MapArea::setNumber(int i)
//{
//    m_number = i;
//}

void MapArea::addArea(MapArea *area)
{
    area->m_parent = this;
    m_areas.append(area);
    area->setZValue(zValue() + 1.0);
}

MapArea* MapArea::area(int i) const
{
    return m_areas.at(i);
}

void MapArea::deleteArea(MapArea* area)
{
    delete area;
    m_areas.remove(m_areas.indexOf(area));
}

int MapArea::areasNumber()
{
    return m_areas.size();
}

void MapArea::addDoor(QGraphicsLineItem *door)
{
    m_doors.append(door);
}

int MapArea::type() const
{
    return Type;
}
