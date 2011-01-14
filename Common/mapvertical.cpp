#include "mapvertical.h"

MapVertical::MapVertical()
{
}

QDataStream &operator<<(QDataStream &output, const MapVertical &vertical)
{
    output << vertical.m_passage;
    QMap<quint32, quint32>::const_iterator it = vertical.m_areas.begin();
    for (; it != vertical.m_areas.end(); ++it)
        output << it.key() << it.value();
    output << 0 << 0; // Marking end of areas
    return output;
}

QDataStream &operator>>(QDataStream &input, MapVertical &vertical)
{
    quint32 floorUin, uin;
    input >> vertical.m_passage >> floorUin >> uin;
    while (floorUin != 0)
    {
        vertical.m_areas[floorUin] = uin;
        input >> floorUin >> uin;
    }
    return input;
}


void MapVertical::addArea(quint32 floorUin, quint32 areaUin)
{
    m_areas[floorUin] = areaUin;
}

quint32 MapVertical::area(const quint32 floorUin) const
{
    if (m_areas.contains(floorUin))
        return m_areas.value(floorUin);
    else
        return 0;
}

void MapVertical::setName(const QString &name)
{
    m_name = name;
}

void MapVertical::setPassage(const bool b)
{
    m_passage = b;
}

bool MapVertical::passage() const
{
    return m_passage;
}

QString MapVertical::name() const
{
    return m_name;
}
