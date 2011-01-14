#ifndef MAPDOOR_H
#define MAPDOOR_H

#include <QBrush>
#include <QGraphicsEllipseItem>

#include "maparea.h"

class MapArea;
class MapDoor : public QGraphicsEllipseItem
{
    friend QDataStream &operator<<(QDataStream &output, const MapDoor &door);
    friend QDataStream &operator>>(QDataStream &input, MapDoor &door);

public:
    MapDoor(const MapDoor &door);
    MapDoor(const QPointF &point);

    enum {Type = QGraphicsItem::UserType + 2};
    void addParentArea(MapArea *area);
    MapArea* parentArea(int i);
    int parentAreasNumber();
    quint32 floorUin();
    QSet<quint32> parentAreasUins();
    bool isFinished() const;
    static void clearFinishedDoors();
    int type() const;
private:
    static const qreal cCircleR;

    QVector<MapArea*> m_parentAreas;
    QSet<quint32> m_parentAreasUins;
    quint32 m_uin;
    static quint32 m_count;

//    static QSet<MapDoor*> m_finishedDoors;
    static QSet<quint32> m_finishedDoors;
//    static quint32 releaseDoor(const MapDoor *door, int i);
};

#endif // MAPDOOR_H
