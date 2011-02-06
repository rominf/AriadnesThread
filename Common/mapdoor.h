#ifndef MAPDOOR_H
#define MAPDOOR_H

#include <QBrush>
#include <QGraphicsEllipseItem>
#include "global.h"
#include "maparea.h"
#include "graphnode.h"

class MapArea;
class GraphNode;
class MapDoor : public QGraphicsEllipseItem
{
    friend QDataStream &operator<<(QDataStream &output, const MapDoor &door);
    friend QDataStream &operator>>(QDataStream &input, MapDoor &door);

public:
    enum {Type = QGraphicsItem::UserType + 2};
    static const QPen cPenNormal;
    static const QPen cPenSelected;
    static const QBrush cBrushNormal;
    static const QBrush cBrushIsntConnectedWithNode;
    static const QBrush cBrushSelected;

    MapDoor(const MapDoor &door);
    MapDoor(const QPointF &point);

    MapArea* parentArea(int i);
    void addParentArea(MapArea *area);
    int parentAreasNumber();
    GraphNode* node();
    void setNode(GraphNode *node);
    quint32 floorUin();
    QSet<quint32> parentAreasUins();
    bool isFinished() const;
    static void clearFinishedDoors();
    int type() const;
private:
    static const qreal cCircleR;

    static QSet<quint32> m_finishedDoors;
    static quint32 m_count;
    QVector<MapArea*> m_parentAreas;
    QSet<quint32> m_parentAreasUins;
    GraphNode *m_node;
    quint32 m_uin;
};

#endif // MAPDOOR_H
