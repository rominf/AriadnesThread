#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QPointF>

#include "grapharc.h"
#include "mapdoor.h"

class MapFloor;
class MapDoor;
class GraphArc;
class GraphNode : public QGraphicsEllipseItem
{
    friend QDataStream& operator<<(QDataStream &output, const GraphNode &node);
    friend QDataStream& operator>>(QDataStream &input, GraphNode &node);
public:
    enum {Type = QGraphicsItem::UserType + 4};
    static const QPen cPenNormal;
    static const QPen cPenSelected;
    static const QPen cPenWay;
    static const QBrush cBrushNormal;
    static const QBrush cBrushSelected;
    static const QBrush cBrushWay;

    GraphNode();
    GraphNode(const QPointF &point, quint32 floor);
    ~GraphNode();

    GraphArc* arc(const int i) const;
    GraphArc* arc(const GraphNode *adjacent, const bool oneWay = false) const;
    void addArc(GraphArc *arc);
    void deleteArc(GraphArc *arc);
    int arcsNumber() const;
    GraphNode* adjacentNode(const GraphArc *arc) const;
    MapDoor* door() const;
    void setDoor(MapDoor *door);
    quint32 floorUin() const;
    quint32 uin() const;
    int type() const;
private:
    static const qreal cCircleR;

    static quint32 m_count; // Counter for making uins

    QVector<GraphArc*> m_arcs;
    MapDoor *m_door;
    quint32 m_floor;
    quint32 m_uin;


};

#endif // GRAPHNODE_H
