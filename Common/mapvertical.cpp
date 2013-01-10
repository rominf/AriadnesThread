#include "mapvertical.h"

MapVertical::MapVertical()
{
    m_type = GraphArc::None;
}

QDataStream &operator<<(QDataStream &output, const MapVertical &vertical)
{
    output << vertical.m_name << vertical.m_type;
    QMap<quint32, quint32>::const_iterator it = vertical.m_areas.begin();
    for (; it != vertical.m_areas.end(); ++it)
        output << it.key() << it.value();
    output << 0 << 0; // Marking end of areas
    return output;
}

QDataStream &operator>>(QDataStream &input, MapVertical &vertical)
{
    quint32 floorUin, uin;
    int type;
//    bool l;
    input >> vertical.m_name >> type >> floorUin >> uin;
    vertical.m_type = GraphArc::VerticalType(type);
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
    emit updated(this);
}

void MapVertical::deleteArea(quint32 floorUin)
{
    m_areas.remove(floorUin);
    emit updated(this);
}

quint32 MapVertical::area(const quint32 floorUin) const
{
    if (m_areas.contains(floorUin))
        return m_areas.value(floorUin);
    else
        return 0;
}

bool MapVertical::contains(const quint32 areaUin) const
{
    return m_areas.key(areaUin) != 0;
}

void MapVertical::setName(const QString &name)
{
    m_name = name;
}

void MapVertical::setType(const GraphArc::VerticalType type)
{
    m_type = type;
    emit updated(this);
}

GraphArc::VerticalType MapVertical::type() const
{
    return m_type;
}

QString MapVertical::name() const
{
    return m_name;
}
