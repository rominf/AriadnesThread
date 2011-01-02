#include "grapharc.h"

GraphArc::GraphArc(GraphNode *node1, GraphNode *node2):
        QGraphicsLineItem(QLineF(node1->pos(), node2->pos()))
{
    m_node1 = node1;
    m_node2 = node2;
//    setPen(QPen(Qt::black));
//    setZValue(INFINITY - 2);
}

GraphArc::~GraphArc()
{
    m_node1->deleteArc(this);
    m_node2->deleteArc(this);
}

GraphNode* GraphArc::node1() const
{
    return m_node1;
}

GraphNode* GraphArc::node2() const
{
    return m_node2;
}

int GraphArc::type() const
{
    return Type;
}
