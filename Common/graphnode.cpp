#include "graphnode.h"

const qreal GraphNode::cCircleR = 4.0;

GraphNode::GraphNode(const QPointF &point, int floor):
        QGraphicsEllipseItem(-cCircleR, -cCircleR, 2*cCircleR, 2*cCircleR),
        m_floor(floor)
{
    setPos(point);
    setBrush(QBrush(Qt::white));
}

int GraphNode::floor()
{
    return m_floor;
}

void GraphNode::addArc(GraphArc *arc)
{
    m_arcs.append(arc);
}

GraphArc* GraphNode::arc(int i) const
{
    return m_arcs.at(i);
}

void GraphNode::deleteArc(GraphArc *arc)
{
    m_arcs.remove(m_arcs.indexOf(arc));
}

int GraphNode::arcsNumber()
{
    return m_arcs.size();
}

GraphNode* GraphNode::adjacentNode(GraphArc *arc)
{
    if ((arc->node1() != this) & (arc->node2() == this))
        return arc->node1();
    else if ((arc->node2() != this) & (arc->node1() == this))
        return arc->node1();
    else
        return 0;
}

int GraphNode::type() const
{
    return Type;
}
