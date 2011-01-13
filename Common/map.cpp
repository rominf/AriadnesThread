#include "map.h"
#include <QDataStream>

Map::Map(qreal mapPixPerRealM, qreal mapPixPerDisplayM,
         qreal mapPixSizeX, qreal mapPixSizeY, QObject *parent) : QObject(parent),
m_pixPerRealM(mapPixPerRealM), m_pixPerDisplayM(mapPixPerDisplayM),
m_pixSizeX(mapPixSizeX), m_pixSizeY(mapPixSizeY)
{
    // Count map geometry
    m_realMPerDisplayM = m_pixPerDisplayM / m_pixPerRealM;

    m_graph = new Graph();
    connect(m_graph, SIGNAL(graphItemAdded(QGraphicsItem*)),
            SLOT(addGraphItem(QGraphicsItem*)));
    connect(m_graph, SIGNAL(lastNodeChanged(GraphNode*)),
            SLOT(setLastNode(GraphNode*)));
//    connect(m_graph, SIGNAL(graphItemDeleted(QGraphicsItem*)),
//            SLOT(addGraphItem(QGraphicsItem*)));
//    connect(m_graph, SIGNAL(graphNodesChanged(QVector<QPointF*>,int)),
//            SLOT(graphNodesChanged(QVector<QPointF*>*)));
}

QDataStream & operator<<(QDataStream &output, const Map &map)
{
    int last = map.m_floors.size();
    output << map.m_pixSizeX << map.m_pixSizeY << map.m_pixPerRealM << last;
    for (int i = 0; i != last; i++)
        output << *map.m_floors.at(i);
    output << *map.m_graph;
    return output;
}

QDataStream & operator>>(QDataStream &input, Map &map)
{
    int last;
    input >> map.m_pixSizeX >> map.m_pixSizeY >> map.m_pixPerRealM >> last;
    for (int i = 0; i != last; i++)
    {
        map.insertFloor(i);
        input >> *map.m_floors[i];
    }
    input >> *map.m_graph;
    return input;
}

void Map::addNode(QPointF point, quint32 floorUin)
{
    m_graph->addNode(point, floorUin);
}

void Map::deleteNode(GraphNode *node)
{
    m_graph->deleteNode(node);
}

void Map::setLastNode(GraphNode *node)
{
    for (int i = 0; i != m_floors.size(); i++)
        m_floors.at(i)->setLastNode(0);
    if (node)
        floorByUin(node->floorUin())->setLastNode(node);
//        for (int i = 0; i != m_floors.size(); i++)
//            if (m_floors.at(i)->uin() == node->floorUin())
//                m_floors.at(i)->setLastNode(node);
}

void Map::addGraphItem(QGraphicsItem *item)
{
    if (item->type() == GraphArc::Type)
    {
        GraphArc *arc = qgraphicsitem_cast<GraphArc*>(item);
        if (arc->node1()->floorUin() == arc->node2()->floorUin())
//        {
            floorByUin(arc->node1()->floorUin())->addItem(arc);
//            for (int i = 0; i != m_floors.size(); i++)
//                if (m_floors.at(i)->uin() == )
//                    m_floors.at(i);
//        }
    }
    if (item->type() == GraphNode::Type)
    {
        GraphNode *node = qgraphicsitem_cast<GraphNode*>(item);
        MapFloor *floor = floorByUin(node->floorUin());
        floor->addItem(node);
        floor->addPointNodesMagnetTo(node->pos());
//        for (int i = 0; i != m_floors.size(); i++)
//            if (m_floors.at(i)->uin() == node->floorUin())
//            {
//                m_floors.at(i)->addItem(node);
//                m_floors.at(i)->addPointNodesMagnetTo(node->pos());
//            }
//        m_floors[node->floor()]->setLastNode(m_graph->lastNode());
    }
}

//void Map::deleteGraphItem(QGraphicsItem *item)
//{
//    if (item->type() == GraphArc::Type)
//    {
//        GraphArc *arc = qgraphicsitem_cast<GraphArc*>(item);
//        if (arc->node1()->floor() == arc->node2()->floor())
//            m_floors[arc->node1()->floor()]->removeItem(arc);
//    }
//    if (item->type() == GraphNode::Type)
//    {
//        GraphNode *node = qgraphicsitem_cast<GraphNode*>(item);
//        for (int i = 0; i != node->arcsNumber(); i++)
//        {
//            GraphArc *arc = node->arc(i);
//            if (arc->node1()->floor() == arc->node2()->floor())
//                m_floors[node->floor()]->removeItem(arc);
//        }
//        m_floors[node->floor()]->removeItem(node);
//        m_floors[node->floor()]->setLastNode(m_graph->lastNode());
//    }
//}

void Map::graphStartAnew()
{
    m_graph->startAnew();
    for (int i = 0; i != m_floors.size(); i++)
        m_floors.at(i)->setLastNode(0);
}

//void Map::graphDeleteNode(GraphNode *node)
//{
//    m_graph->deleteNode(node);
//}

//void Map::graphNodesChanged(QVector<QPointF*> &nodes, int floor)
//{
//    m_floors[floor]->setGraphNodes(nodes);
//}

void Map::setPixPerDisplayM(qreal r)
{
    m_pixPerDisplayM = r;
}

qreal Map::convertPixToDisplayM(qreal r) const
{
    return r/m_pixPerDisplayM;
}

qreal Map::convertPixToRealM(qreal r) const
{
    return r/m_pixPerRealM;
}

qreal Map::convertDisplayMToPix(qreal r) const
{
    return r*m_pixPerDisplayM;
}

qreal Map::convertRealMToPix(qreal r) const
{
    return r*m_pixPerRealM;
}

QVector<GraphNode*> Map::getNodesFromItem(QGraphicsItem *item)
{
    MapArea *area = 0;
    QVector<MapDoor*> doors;
    QVector<GraphNode*> nodes;
    switch (item->type())
    {
    case MapArea::Type:
        area = qgraphicsitem_cast<MapArea*>(item);
        break;
    case MapDoor::Type:
        doors.append(qgraphicsitem_cast<MapDoor*>(item));
        break;
    case GraphNode::Type:
        nodes.append(qgraphicsitem_cast<GraphNode*>(item));
        break;
    }
    if (area)
        for (int i = 0; i != area->doorsNumber(); i++)
            doors.append(area->door(i));
    for (int i = 0; i != doors.size(); i++)
    {
        quint32 floorUin = doors.at(i)->parentArea(0)->floorUin();
        QGraphicsItem *itemAt =
                floorByUin(floorUin)->itemAt(doors.at(i)->pos());
        if (itemAt != 0)
            if (itemAt->type() == GraphNode::Type)
                nodes.append(qgraphicsitem_cast<GraphNode*>(itemAt));
    }
    return nodes;
}

int Map::floorsNumber()
{
//    if (!m_floors.isEmpty())
        return m_floors.size();/*
    else
        return 0;*/
}

void Map::insertFloor(int i)
{
    MapFloor *floor = new MapFloor(QRectF(0, 0, m_pixSizeX, m_pixSizeY), this);
    m_floors.insert(i, floor);
    connect(floor, SIGNAL(addedNode(QPointF,quint32)),
            SLOT(addNode(QPointF,quint32)));
    connect(floor, SIGNAL(deletedNode(GraphNode*)),
            SLOT(deleteNode(GraphNode*)));
    connect(floor, SIGNAL(graphStartedAnew()),
            SLOT(graphStartAnew()));
}

void Map::swapFloors(int x, int y)
{
    MapFloor *floor = m_floors[x];
    m_floors[x] = m_floors[y];
    m_floors[y] = floor;
}

void Map::removeFloor(int i)
{
    m_floors.remove(i);
}

MapFloor* Map::floor(const int i) const
{
    return m_floors.at(i);
}

MapFloor* Map::floorByUin(const quint32 uin) const
{
    for (int i = 0; i != m_floors.size(); i++)
        if (m_floors.at(i)->uin() == uin)
            return m_floors.at(i);
    return 0;
}

Graph* Map::graph() const
{
    return m_graph;
}

void Map::setStart(QGraphicsItem *item)
{
    QVector<GraphNode*> nodes = getNodesFromItem(item);
    if (!nodes.isEmpty())
        m_startNodes = nodes;
//    if (item->type() == GraphNode::Type)
//    {
//        m_startNodes.clear();
//        m_startNodes.append(qgraphicsitem_cast<GraphNode*>(item));
//    }
}

void Map::setFinish(QGraphicsItem *item)
{
    QVector<GraphNode*> nodes = getNodesFromItem(item);
    if (!nodes.isEmpty())
        m_finishNodes = nodes;
//    if (item->type() == GraphNode::Type)
//    {
//        m_finishNodes.clear();
//        m_finishNodes.append(qgraphicsitem_cast<GraphNode*>(item));
//    }
}

void Map::way()
{
    m_graph->way(m_startNodes, m_finishNodes);
}

void Map::clearWay()
{
    m_graph->clearWay();
}

//QVector<QPointF*> Map::graphNodesCoordinates()
//{
//    return m_graph->nodesCoordinates();
//}

//const Graph* Map::graph() const
//{
//    return m_graph;
//}
