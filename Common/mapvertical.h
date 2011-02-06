#ifndef MAPVERTICAL_H
#define MAPVERTICAL_H

#include <QMap>
#include "maparea.h"
#include "grapharc.h"

class MapVertical: public QObject
{
    Q_OBJECT
    friend QDataStream &operator<<(
            QDataStream &output, const MapVertical &vertical);
    friend QDataStream &operator>>(QDataStream &input, MapVertical &vertical);
public:
    MapVertical();
    void addArea(quint32 floorUin, quint32 areaUin);
    void deleteArea(quint32 floorUin);
    quint32 area(const quint32 floorUin) const;
    bool contains(const quint32 areaUin) const;
    void setName(const QString &name);
    QString name() const;
    void setType(const GraphArc::VerticalType type);
    GraphArc::VerticalType type() const;
signals:
    void updated(MapVertical *vertical);
private:
    QMap<quint32, quint32> m_areas;
    QString m_name;
    GraphArc::VerticalType m_type;
};

#endif // MAPVERTICAL_H
