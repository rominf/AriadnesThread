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
public:
    GraphNode(const QPointF &point, int floor);

    enum {Type = QGraphicsItem::UserType + 4};
    int floor();
    void addArc(GraphArc *arc);
    GraphArc* arc(int i) const;
    void deleteArc(GraphArc *arc);
    int arcsNumber();
    GraphNode* adjacentNode(GraphArc *arc);
    int type() const;
private:
    static const qreal cCircleR;
    int m_floor;
    QVector<GraphArc*> m_arcs;
};

#endif // GRAPHNODE_H
