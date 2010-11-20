#include "map.h"
#include <QDataStream>

Map::Map(qreal mapPixPerRealM, qreal mapPixPerDisplayM,
         qreal mapPixSizeX, qreal mapPixSizeY, QObject *parent) : QObject(parent),
pixPerRealM(mapPixPerRealM), pixPerDisplayM(mapPixPerDisplayM),
pixSizeX(mapPixSizeX), pixSizeY(mapPixSizeY)
{
    // Count map geometry
    realMPerDisplayM = pixPerDisplayM / pixPerRealM;
}

QDataStream & operator<<(QDataStream &output, const Map &map)
{
    int last = map.floors.size();
    output << map.pixSizeX << map.pixSizeY << map.pixPerRealM << last;
    for (int i = 0; i != last; ++i)
    {
        output << *map.floors.at(i);
    }
    return output;
}

QDataStream & operator>>(QDataStream &input, Map &map)
{
    int last;
    input >> map.pixSizeX >> map.pixSizeY >> map.pixPerRealM >> last;
    for (int i = 0; i != last; ++i)
    {
        map.insertFloor(i);
        input >> *map.floors[i];
    }
    return input;
}

void Map::setPixPerDisplayM(qreal r)
{
    pixPerDisplayM = r;
}

qreal Map::convertPixToDisplayM(qreal r) const
{
    return r/pixPerDisplayM;
}

qreal Map::convertPixToRealM(qreal r) const
{
    return r/pixPerRealM;
}

qreal Map::convertDisplayMToPix(qreal r) const
{
    return r*pixPerDisplayM;
}

qreal Map::convertRealMToPix(qreal r) const
{
    return r*pixPerRealM;
}

int Map::floorsNumber()
{
    return floors.size();
}

void Map::insertFloor(int i, QString floorName)
{
    floors.insert(i, new
                  MapFloor(QRectF(0, 0, pixSizeX, pixSizeY), floorName, this));
}

void Map::removeFloor(int i)
{
    floors.remove(i);
}

MapFloor* Map::floor(int i) const
{
    return floors.at(i);
}
