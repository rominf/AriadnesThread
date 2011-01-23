#include "map.h"
#include <QDataStream>

qreal Map::m_lengthStairsDown = cSpeedMPerMin*cTimeStairsDown;
qreal Map::m_lengthStairsUp = cSpeedMPerMin*cTimeStairsUp;

Map::Map(qreal mapPixPerRealM, qreal mapPixPerDisplayM,
         qreal mapPixSizeX, qreal mapPixSizeY, QObject *parent): QObject(parent),
m_pixPerRealM(mapPixPerRealM), m_pixPerDisplayM(mapPixPerDisplayM),
m_pixSizeX(mapPixSizeX), m_pixSizeY(mapPixSizeY)
{
    // Count map geometry
//    m_realMPerDisplayM = m_pixPerDisplayM / m_pixPerRealM;
    // ### Make autoAreasRenaming editable by user
    m_areasAutoRenaming = true;
    m_selectedVertical = -1;
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
    last = map.m_verticals.size();
    output << last;
    for (int i = 0; i != last; i++)
        output << *map.m_verticals.at(i);
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
    input >> last;
    for (int i = 0; i != last; i++)
    {
        MapVertical *vertical = new MapVertical();
        input >> *vertical;
        map.m_verticals.append(vertical);
    }
    input >> *map.m_graph;
    for (int i = 0; i != map.m_verticals.size(); i++)
        map.setTypeVertical(i, map.m_verticals.at(i)->type());
    return input;
}

void Map::areaActivated(MapArea *area, bool activated)
{
    if (m_selectedVertical > -1)
    {
        if (activated)
            m_verticals.at(m_selectedVertical)->addArea(
                    area->floorUin(), area->uin());
        else
            m_verticals.at(m_selectedVertical)->deleteArea(area->floorUin());
        setTypeVertical(m_selectedVertical,
                        m_verticals.at(m_selectedVertical)->type());
    }
}

void Map::addNode(QPointF point, quint32 floorUin)
{
    m_graph->addNode(point, floorUin);
}

void Map::deleteNode(GraphNode *node)
{
    if (node->door())
        node->door()->setNode(0);;
    m_graph->deleteNode(node);
}

void Map::deleteArc(GraphArc *arc)
{
    m_graph->deleteArc(arc);
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
        QGraphicsItem *item = floor->itemAt(node->pos());
        if (item)
            if (item->type() == MapDoor::Type)
            {
                MapDoor *door = qgraphicsitem_cast<MapDoor*>(item);
                door->setNode(node);
            }
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
//        quint32 floorUin = doors.at(i)->parentArea(0)->floorUin();
//        QGraphicsItem *itemAt =
//                itemAt(doors.at(i)->pos());
//        if (itemAt != 0)
//            if (itemAt->type() == GraphNode::Type)
//                nodes.append(qgraphicsitem_cast<GraphNode*>(itemAt));
        if (doors.at(i)->node() != 0)
            nodes.append(doors.at(i)->node());
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
    connect(floor, SIGNAL(areaActivated(MapArea*,bool)),
            SLOT(areaActivated(MapArea*,bool)));
    connect(floor, SIGNAL(addedNode(QPointF,quint32)),
            SLOT(addNode(QPointF,quint32)));
    connect(floor, SIGNAL(deletedNode(GraphNode*)),
            SLOT(deleteNode(GraphNode*)));
    connect(floor, SIGNAL(deletedArc(GraphArc*)), SLOT(deleteArc(GraphArc*)));
    connect(floor, SIGNAL(graphStartedAnew()), SLOT(graphStartAnew()));
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

void Map::addVertical()
{
    m_verticals.append(new MapVertical());
//    deselectVertical();
    selectVertical(m_verticals.size() - 1);
}

void Map::deleteVertical(const int vertNum)
{
//    deselectVertical();
    MapVertical *vertical = m_verticals.at(vertNum);
    delete vertical;
    m_verticals.remove(vertNum);
}

void Map::swapVerticals(int x, int y)
{
    MapVertical *vertical = m_verticals.at(x);
    m_verticals[x] = m_verticals[y];
    m_verticals[y] = vertical;
}

int Map::verticalsNumber()
{
    return m_verticals.size();
}

MapVertical* Map::vertical(const int vertNum) const
{
    if (vertNum > -1)
        return m_verticals.at(vertNum);
    else
        return 0;
}

void Map::setNameVertical(const int vertNum, const QString &name)
{
    if (vertNum > -1)
        m_verticals.at(vertNum)->setName(name);
}

void Map::setTypeVertical(const int vertNum, const GraphArc::VerticalType type)
{
    if (vertNum > -1)
    {
        MapVertical *vertical = m_verticals.at(vertNum);
        m_verticals.at(vertNum)->setType(type);
        if ((type == GraphArc::Stairs) | (type == GraphArc::Lift))
        {
            QVector<MapDoor*> doors;
            for (int i = 0; i != m_floors.size(); i++)
            {
                MapArea *area = m_floors.at(i)->areaByUin(
                        vertical->area(m_floors.at(i)->uin()));
                if (area != 0)
                {
                    if (area->doorsNumber() > 0)
                    {
                        for (int i = 0; i != area->doorsNumber() - 1; i++)
                        {
                            GraphNode *node1 = area->door(i)->node();
                            GraphNode *node2 = area->door(i+1)->node();
                            if ((node1 != 0) & (node2 != 0))
                            {
                                GraphArc *arc =
                                        new GraphArc(node1, node2, type);
                                arc->setLenght(0);
                            }
                        }
                        doors.append(area->door(0));
                        if (area->doorsNumber() > 1)
                            area->setBrush(Qt::red);
                    }
                }

            }
            if (!doors.isEmpty())
                for (int i = 0; i != doors.size(); i++)
                {
    //                MapDoor *door = doors.at(i);
                    GraphNode *n = doors.at(i)->node();
                    if (n)
                    {
                        int j = 0;
                        while (j != n->arcsNumber())
                            if (n->arc(j)->floorUin() == 0)
                                delete n->arc(j);
                            else
                                j++;
                    }
    //                QGraphicsItem *item =
    //                        floorByUin(door->floorUin())->itemAt(door->pos());
    //                if (item != 0)
    //                    if (item->type() == GraphNode::Type)
    //                    {
    //                        GraphNode *n = qgraphicsitem_cast<GraphNode*>(item);
    //                        int j = 0;
    //                        while (j != n->arcsNumber())
    //                            if (n->arc(j)->floorUin() == 0)
    //                                delete n->arc(j);
    //                            else
    //                                j++;
    //                    }
                }
            if (!doors.isEmpty())
                for (int i = 0; i != doors.size() - 1; i++)
                    if ((doors.at(i)->node() != 0) & (doors.at(i+1)->node() != 0))
                    {
                        GraphNode *node1 = doors.at(i)->node();
                        GraphNode *node2 = doors.at(i+1)->node();
                        if (type == GraphArc::Lift)
                        {
                            GraphArc *arc = new GraphArc(node1, node2, type);
                            arc->setLenght(
                                    convertRealMToPix(cSpeedMPerMin * cTimeLift));
                        }
                        if (type == GraphArc::Stairs)
                        {
                            GraphArc *arc1 = new GraphArc(node1, node2, type, true);
                            arc1->setLenght(convertRealMToPix(m_lengthStairsDown));
                            GraphArc *arc2 = new GraphArc(node2, node1, type, true);
                            arc2->setLenght(convertRealMToPix(m_lengthStairsUp));
                        }
                    }
        }
//                QGraphicsItem *item1 =
//                        floorByUin(door1->floorUin())->itemAt(door1->pos());
//                QGraphicsItem *item2 =
//                        floorByUin(door2->floorUin())->itemAt(door2->pos());
//                if ((item1 != 0) & (item2 != 0))
//                    if ((item1->type() == GraphNode::Type) &
//                        (item2->type() == GraphNode::Type))
//                    {
//                        GraphArc *arc = new GraphArc(
//                                qgraphicsitem_cast<GraphNode*>(item1),
//                                qgraphicsitem_cast<GraphNode*>(item2));
//                        arc->setLenght(convertRealMToPix(50));
//                    }
//            }
    }
//    }
//    MapVertical *vertical = m_verticals.at(vertNum);
//    QVector<MapDoor*> doors;
////    if (vertical->passage() != b)
////    {
//        vertical->setPassage(b);
//        for (int i = 0; i != m_floors.size(); i++)
//        {
//            MapArea *area = m_floors.at(i)->areaByUin(
//                    vertical->area(m_floors.at(i)->uin()));
//            if (area != 0)
//                doors.append(area->door(0));
//        }
//        if (!doors.isEmpty())
//            for (int i = 0; i != doors.size() - 1; i++)
//            {
//                MapDoor *door1 = doors.at(i);
//                MapDoor *door2 = doors.at(i+1);
//                QGraphicsItem *item1 =
//                        floorByUin(door1->floorUin())->itemAt(door1->pos());
//                QGraphicsItem *item2 =
//                        floorByUin(door2->floorUin())->itemAt(door2->pos());
//                if ((item1 != 0) & (item2 != 0))
//                    if ((item1->type() == GraphNode::Type) &
//                        (item2->type() == GraphNode::Type))
//                    {
//                        GraphNode *n1 = qgraphicsitem_cast<GraphNode*>(item1);
//                        GraphNode *n2 = qgraphicsitem_cast<GraphNode*>(item2);
//                        if (b)
//                            new GraphArc(n1, n2);
//                        else
//                            delete n1->arc(n2);
//                    }
//            }
//    }
}

GraphArc::VerticalType Map::typeVertical(const int vertNum) const
{
    return m_verticals.at(vertNum)->type();
}

void Map::selectVertical(const int vertNum)
{
    for (int i = 0; i != m_floors.size(); i++)
        m_floors.at(i)->blockSignals(true);
    deselectVertical();
    m_selectedVertical = vertNum;
    for (int i = 0; i != m_floors.size(); i++)
    {
        quint32 floorUin = m_floors.at(i)->uin();
        quint32 areaUin = m_verticals.at(vertNum)->area(floorUin);
        if (areaUin)
        {
            MapArea *area = m_floors.at(i)->areaByUin(areaUin);
            if (area)
                m_floors.at(i)->selectArea(area);
            else
                m_verticals.at(vertNum)->deleteArea(floorUin);
        }
    }
    for (int i = 0; i != m_floors.size(); i++)
        m_floors.at(i)->blockSignals(false);
}

//const QVector<MapVertical*> Map::verticals()
//{
//    return m_verticals;
//}

void Map::deselectVertical()
{
    m_selectedVertical = -1;
    for (int i = 0; i != m_floors.size(); i++)
        m_floors.at(i)->resetSelection();
}

bool Map::areaCopy(MapArea *area, int floorFromIndex, int floorToIndex)
{
    MapFloor *floorFrom = m_floors.at(floorFromIndex);
    MapFloor *floorTo = m_floors.at(floorToIndex);
    if (floorFromIndex != floorToIndex)
    {
        MapArea *newArea;
        if (m_areasAutoRenaming)
            newArea = new MapArea(*area, floorTo->uin(),
                                  floorFrom->name(), floorTo->name());
        else
            newArea = new MapArea(*area, floorTo->uin(), "", "");
        switch (floorTo->addArea(newArea))
        {
        case MapArea::ceNone:
        {
            MapArea *a;
            QStack<MapArea*> stk;
            stk.push(area);
            while (!stk.isEmpty())
            {
                a = stk.pop();
                for (int i = 0; i != a->doorsNumber(); i++)
                {
                    MapDoor *door = new MapDoor(*a->door(i));
                    floorTo->addDoor(door);
                }
                int size = a->areasNumber();
                for (int i = 0; i !=size; i++)
                    stk.push(a->area(i));
            }
            return true;
            break;
        }
        case MapArea::ceIntersection:
            QMessageBox::warning(
                    0, tr("Ошибка при копировании области"), tr("На ") +
                    floorTo->name() + tr(" этаже недостаточно места для "
                                         "вставки области."));
            break;
        case MapArea::ceAreaExist:
            QMessageBox::warning(
                    0, tr("Ошибка при копировании области"), tr("На ") +
                    floorTo->name() + tr(" этаже уже есть данная область."));
            break;
        }
    }
    else
        QMessageBox::warning(
                0, tr("Ошибка при копировании области"), tr("На ") +
                floorTo->name() + tr(" этаже уже есть данная область."));
    return false;
}

bool Map::areasAutoRenaming() const
{
    return m_areasAutoRenaming;
}

void Map::setAreasAutoRenaming(bool enabled)
{
    m_areasAutoRenaming = enabled;
}

Graph* Map::graph() const
{
    return m_graph;
}

void Map::setStart(QGraphicsItem *item)
{
    QVector<GraphNode*> nodes = getNodesFromItem(item);
    if (!nodes.isEmpty())
        m_graph->setStartNodes(nodes);
//        m_startNodes = nodes;
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
        m_graph->setFinishNodes(nodes);
//        m_finishNodes = nodes;
//    if (item->type() == GraphNode::Type)
//    {
//        m_finishNodes.clear();
//        m_finishNodes.append(qgraphicsitem_cast<GraphNode*>(item));
//    }
}

bool Map::isStartAndFinishNodesValid() const
{
    return m_graph->isStartAndFinishNodesValid();
}

void Map::way(const GraphArc::WayPermissions permissions)
{
    if (isStartAndFinishNodesValid())
        m_graph->makeWay(permissions);
}

void Map::paintWay()
{
    m_graph->paintWay(true);
}

void Map::clearWay()
{
    m_graph->clearWay();
}

bool Map::wayInfo(qreal &length, int &stairsDownNumber, int &stairsUpNumber,
                  int &liftsNumber, qreal &time) const
{
    qreal lengthPix = 0;
    stairsDownNumber = 0;
    stairsUpNumber = 0;
    liftsNumber = 0;
    time = 0;
//    qreal lengthStairsDown = cTimeStairsDown * cSpeedMPerMin;
//    qreal lengthStairsUp = cTimeStairsUp * cSpeedMPerMin;
    QVector<GraphNode*> way = graph()->way();
    if (!way.isEmpty())
    {
        for (int i = 0; i != way.size() - 1; i++)
        {
            GraphArc *arc = way.at(i)->arc(way.at(i+1), true);
            switch (arc->verticalType())
            {
            case GraphArc::None:
                lengthPix += arc->lenght();
                break;
            case GraphArc::Stairs:
                if (convertPixToRealM(arc->lenght()) == m_lengthStairsDown)
                    stairsDownNumber++;
                if (convertPixToRealM(arc->lenght()) == m_lengthStairsUp)
                    stairsUpNumber++;
                break;
            case GraphArc::Lift:
                liftsNumber++;
                break;
            case GraphArc::Room:
                break;
            }
        }
        length = convertPixToRealM(lengthPix);
        time = length/cSpeedMPerMin + stairsDownNumber*cTimeStairsDown +
               stairsUpNumber*cTimeStairsUp + liftsNumber*cTimeLift;
    }
    return !way.isEmpty();
}

//QVector<QPointF*> Map::graphNodesCoordinates()
//{
//    return m_graph->nodesCoordinates();
//}

//const Graph* Map::graph() const
//{
//    return m_graph;
//}
