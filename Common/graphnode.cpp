#include "graphnode.h"

const qreal GraphNode::cCircleR = 4.0;
quint32 GraphNode::m_count = 0;
const QPen GraphNode::cPenNormal = QPen(Qt::black);
//const QPen GraphNode::cPenSelected = QPen(QBrush(Qt::black), 2);
const QPen GraphNode::cPenWay = QPen(Qt::black);
const QBrush GraphNode::cBrushNormal = QBrush(Qt::blue);
//const QBrush GraphNode::cBrushSelected = QBrush(Global::colorSelected);
const QBrush GraphNode::cBrushWay = QBrush(Qt::darkGreen);


GraphNode::GraphNode():
        QGraphicsEllipseItem(-cCircleR, -cCircleR, 2*cCircleR, 2*cCircleR)
{
    setBrush(cBrushNormal);
    setPen(cPenNormal);
    setZValue(100500 - 1);
    m_door = 0;
    m_uin = ++m_count;
}

GraphNode::GraphNode(const QPointF &point, quint32 floor):
        QGraphicsEllipseItem(-cCircleR, -cCircleR, 2*cCircleR, 2*cCircleR),
        m_floor(floor)
{
    setPos(point);
    setBrush(cBrushNormal);
    setPen(cPenNormal);
    setZValue(100500 - 1);
    m_door = 0;
    m_uin = ++m_count;
}

GraphNode::~GraphNode()
{
    while (m_arcs.size() != 0)
    {
        GraphArc *arc = m_arcs.at(0);
        m_arcs.remove(0);
        delete arc;
    }
}

QDataStream & operator<<(QDataStream &output, const GraphNode &node)
{
    output << node.m_uin << node.x() << node.y() << node.m_floor;
    return output;
}

QDataStream & operator>>(QDataStream &input, GraphNode &node)
{
    qreal x, y;
    input >> node.m_uin >> x >> y >> node.m_floor;
    node.setPos(x, y);
    node.m_count = qMax(node.m_count, node.m_uin);
    return input;
}

MapDoor* GraphNode::door() const
{
    return m_door;
}

void GraphNode::setDoor(MapDoor *door)
{
    m_door = door;
    if (door->node() != this)
        door->setNode(this);
}

quint32 GraphNode::floorUin() const
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

GraphArc* GraphNode::arc(const GraphNode *adjacent, const bool oneWay) const
{
    for (int i = 0; i != m_arcs.size(); i++)
        if ((adjacentNode(m_arcs.at(i)) == adjacent) &
            !(oneWay & !m_arcs.at(i)->isRight(this)))
            return m_arcs.at(i);
    return 0;
}

void GraphNode::deleteArc(GraphArc *arc)
{
    int i = m_arcs.indexOf(arc);
    if (i > -1)
        m_arcs.remove(i);
}

int GraphNode::arcsNumber() const
{
    return m_arcs.size();
}

GraphNode* GraphNode::adjacentNode(const GraphArc *arc) const
{
    if ((arc->node1() != this) & (arc->node2() == this))
        return arc->node1();
    if ((arc->node2() != this) & (arc->node1() == this))
        return arc->node2();
    return 0; // It's impossible, but who know?.. Сompiller say me,
              // it isn't good to keep code without this stupid line :)
              // Bye the way: this is XPEHb! :D
}

int GraphNode::type() const
{
    return Type;
}

quint32 GraphNode::uin() const
{
    return m_uin;
}
