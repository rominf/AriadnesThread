#ifndef MAPVERTICAL_H
#define MAPVERTICAL_H

#include <QMap>
#include "maparea.h"

class MapVertical
{
    friend QDataStream &operator<<(QDataStream &output,
                                   const MapVertical &vertical);
    friend QDataStream &operator>>(QDataStream &input, MapVertical &vertical);
public:
    MapVertical();
    void addArea(quint32 floorUin, quint32 areaUin);
    void deleteArea(quint32 floorUin);
    quint32 area(const quint32 floorUin) const;
    void setName(const QString &name);
    QString name() const;
    void setPassage(const bool b);
    bool passage() const;
private:
    QMap<quint32, quint32> m_areas;
    QString m_name;
    bool m_passage;
};

#endif // MAPVERTICAL_H
