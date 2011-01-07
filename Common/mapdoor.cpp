#include "mapdoor.h"

const qreal MapDoor::cCircleR = 7.0;
quint32 MapDoor::m_count = 0;
QSet<quint32> MapDoor::m_finishedDoors;

MapDoor::MapDoor(const MapDoor &door):
        QGraphicsEllipseItem(/*point.x() */- cCircleR, /*point.y() */- cCircleR,
                             2*cCircleR, 2*cCircleR)
{
    QPointF point = door.pos();
    setPos(point.x(), point.y());
    m_uin = ++m_count;
    setBrush(QBrush(Qt::white));
}

MapDoor::MapDoor(const QPointF &point):
        QGraphicsEllipseItem(/*point.x() */- cCircleR, /*point.y() */- cCircleR,
                             2*cCircleR, 2*cCircleR)
{
    setPos(point.x(), point.y());
    m_uin = ++m_count;
    setBrush(QBrush(Qt::white));
}

QDataStream &operator<<(QDataStream &output, const MapDoor &door)
{
    output << door.m_uin << door.pos().x() << door.pos().y()
            << door.m_parentAreas.size();
    for (int i = 0; i != door.m_parentAreas.size(); i++)
        output << door.m_parentAreas.at(i)->uin();
    door.m_finishedDoors.insert(door.m_uin);
    return output;
}

QDataStream &operator>>(QDataStream &input, MapDoor &door)
{
    qreal x, y;
    int size;
    quint32 c, uin;
    input >> uin >> x >> y >> size;
    door.m_count = qMax(door.m_count, uin);
    door.m_uin = uin;
    door.setPos(x/* - MapDoor::cCircleR*/, y/* - MapDoor::cCircleR*/);
    for (int i = 0; i != size; i++)
    {
        input >> c;
        door.m_parentAreasUins.insert(c);
    }
//        door.setZValue(100.0);
    return input;
}

void MapDoor::addParentArea(MapArea *area)
{
    m_parentAreas.append(area);
}

MapArea* MapDoor::parentArea(int i)
{
    return m_parentAreas[i];
}

int MapDoor::parentAreasNumber()
{
    return m_parentAreas.size();
}

QSet<quint32> MapDoor::parentAreasUins()
{
//    QSet<quint32> uins;
//    for (int i = 0; i != m_parentAreas.size(); i++)
//        uins.insert(m_parentAreas.at(i)->uin());
    return m_parentAreasUins;
}

bool MapDoor::isFinished() const
{
    return m_finishedDoors.contains(this->m_uin);
}

void MapDoor::clearFinishedDoors()
{
    m_finishedDoors.clear();
}

int MapDoor::type() const
{
    return Type;
}
