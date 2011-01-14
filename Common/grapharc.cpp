#include "grapharc.h"

quint32 GraphArc::m_count = 0;
const QPen GraphArc::cPenNormal = QPen(Qt::blue);
const QPen GraphArc::cPenSelected = QPen(QBrush(Qt::black), 2);
const QPen GraphArc::cPenWay = QPen(QBrush(Qt::darkGreen), 2);

GraphArc::GraphArc(GraphNode *node1, GraphNode *node2):
        QGraphicsLineItem(QLineF(node1->pos(), node2->pos()))
{
    m_node1 = node1;
    m_node2 = node2;
    node1->addArc(this);
    node2->addArc(this);
    setPen(cPenNormal);
    setZValue(100500 - 2);
    if (floorUin() != 0)    // If arc isn't between floors
        m_lenght = Geometry::dest(m_node1->pos(), m_node2->pos());
    else
        m_lenght = 0;
    m_uin = ++m_count;
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

bool GraphArc::contain(GraphNode *node) const
{
    return (m_node1 == node) | (m_node2 == node);
}

void GraphArc::setLenght(const qreal lenght)
{
    m_lenght = lenght;
}

qreal GraphArc::lenght() const
{
    return m_lenght;
}

int GraphArc::type() const
{
    return Type;
}

quint32 GraphArc::floorUin() const
{
    if (m_node1->floorUin() == m_node2->floorUin())
        return m_node1->floorUin();
    else
        return 0;
}

quint32 GraphArc::uin() const
{
    return m_uin;
}
