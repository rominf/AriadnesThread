#include "map.h"
#include <QDataStream>

Map::Map(qreal mapPixPerRealM, qreal mapPixPerDisplayM,
         qreal mapPixSizeX, qreal mapPixSizeY, QObject *parent) : QObject(parent),
m_pixPerRealM(mapPixPerRealM), m_pixPerDisplayM(mapPixPerDisplayM),
m_pixSizeX(mapPixSizeX), m_pixSizeY(mapPixSizeY)
{
    // Count map geometry
    m_realMPerDisplayM = m_pixPerDisplayM / m_pixPerRealM;
}

QDataStream & operator<<(QDataStream &output, const Map &map)
{
    int last = map.m_floors.size();
    output << map.m_pixSizeX << map.m_pixSizeY << map.m_pixPerRealM << last;
    for (int i = 0; i != last; i++)
    {
        output << *map.m_floors.at(i);
    }
    return output;
}

QDataStream & operator>>(QDataStream &input, Map &map)
{
    int last;
    input >> map.m_pixSizeX >> map.m_pixSizeY >> map.m_pixPerRealM >> last;
    for (int i = 0; i != last; i++)
    {
        map.insertFloor(i);
        input >> *map.m_floors[i];
    }
    return input;
}

void Map::setPixPerDisplayM(qreal r)
{
    m_pixPerDisplayM = r;
}

qreal Map::convertPixToDisplayM(qreal r) const
{
    return r/m_pixPerDisplayM;
}

qreal Map::convertPixToRealM(qreal r) const
{
    return r/m_pixPerRealM;
}

qreal Map::convertDisplayMToPix(qreal r) const
{
    return r*m_pixPerDisplayM;
}

qreal Map::convertRealMToPix(qreal r) const
{
    return r*m_pixPerRealM;
}

int Map::floorsNumber()
{
    if (!m_floors.isEmpty())
        return m_floors.size();
    else
        return 0;
}

void Map::insertFloor(int i)
{
    m_floors.insert(i, new
                  MapFloor(QRectF(0, 0, m_pixSizeX, m_pixSizeY), this));
}

void Map::removeFloor(int i)
{
    m_floors.remove(i);
}

MapFloor* Map::floor(int i) const
{
    return m_floors.at(i);
}
