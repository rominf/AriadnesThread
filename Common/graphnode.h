#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QPointF>
#include "grapharc.h"
#include "mapdoor.h"

class GraphArc;
class GraphNode : public QGraphicsEllipseItem
{

    friend QDataStream &operator<<(QDataStream &output, const GraphNode &node);
    friend QDataStream &operator>>(QDataStream &input, GraphNode &node);
public:
    GraphNode();
    GraphNode(const QPointF &point, int floor);
    ~GraphNode();

    enum {Type = QGraphicsItem::UserType + 4};
    int floor();
    void addArc(GraphArc *arc);
    GraphArc* arc(int i) const;
    void deleteArc(GraphArc *arc);
    int arcsNumber();
    GraphNode* adjacentNode(GraphArc *arc);
    int type() const;
    quint32 uin();
private:
    static const qreal cCircleR;
    int m_floor;
    QVector<GraphArc*> m_arcs;
    quint32 m_uin;

    static quint32 m_count; // Counter for making uins
};

#endif // GRAPHNODE_H
