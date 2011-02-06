#include "map.h"
#include <QDataStream>

qreal Map::m_lengthStairsDown = Global::cSpeedMPerMin*Global::cTimeStairsDown;
qreal Map::m_lengthStairsUp = Global::cSpeedMPerMin*Global::cTimeStairsUp;

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
    m_start = new MapSelection(Global::colorStart);
    m_finish = new MapSelection(Global::colorFinish);
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
        map.addVertical();
        input >> *map.m_verticals[i];
    }
    input >> *map.m_graph;
//    for (int i = 0; i != map.m_verticals.size(); i++)
//        map.setTypeVertical(i, map.m_verticals.at(i)->type());
    return input;
}

void Map::areaActivated(MapArea *area, bool activated)
{
    if (m_selectedVertical > -1)
    {
        MapVertical *vertical = m_verticals.at(m_selectedVertical);
        if (activated)
        {
            if (Map::vertical(area) == 0)
                vertical->addArea(area->floorUin(), area->uin());
        }
        else
            vertical->deleteArea(area->floorUin());
        updateVertical(vertical);
//        setTypeVertical(m_selectedVertical,
//                        m_verticals.at(m_selectedVertical)->type());
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
    QVector<MapArea*> areas;
    QVector<MapDoor*> doors;
    QVector<GraphNode*> nodes;
    switch (item->type())
    {
    case MapArea::Type:
        areas.append(qgraphicsitem_cast<MapArea*>(item));
        break;
    case MapDoor::Type:
        doors.append(qgraphicsitem_cast<MapDoor*>(item));
        break;
    case GraphNode::Type:
        nodes.append(qgraphicsitem_cast<GraphNode*>(item));
        break;
    default:
        break;
    }
    if (!areas.isEmpty())
    {
        MapVertical *vertical = Map::vertical(areas.at(0));
        if (vertical)
            if (vertical->type() == GraphArc::Room)
                for (int i = 0; i != m_floors.size(); i++)
                {
                    quint32 floorUin = m_floors.at(i)->uin();
                    quint32 areaUin = vertical->area(floorUin);
                    MapArea *area = m_floors.at(i)->area(areaUin);
                    if (area)
                        areas.append(area);
                }
    }
    for (int i = 0; i != areas.size(); i++)
        for (int j = 0; j != areas.at(i)->doorsNumber(); j++)
            doors.append(areas.at(i)->door(j));
    for (int i = 0; i != doors.size(); i++)
        if (doors.at(i)->node() != 0)
            nodes.append(doors.at(i)->node());
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
    connect(floor->selection(), SIGNAL(areaActivated(MapArea*,bool)),
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
    QVector<MapVertical*>::const_iterator it = m_verticals.begin();
    for (; it != m_verticals.end(); ++it)
        updateVertical(*it);
//    for (int i = 0; i != m_verticals.size(); i++)
//        updateVertical(i);
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

int Map::floorNumber(MapFloor* floor) const
{
    return m_floors.indexOf(floor);
}

void Map::addVertical()
{
    MapVertical *vertical = new MapVertical();
    connect(vertical, SIGNAL(updated(MapVertical*)),
            SLOT(updateVertical(MapVertical*)));
    m_verticals.append(vertical);
//    deselectVertical();
//    selectVertical(m_verticals.size() - 1);
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

MapVertical* Map::vertical(const MapArea *area) const
{
    for (int i = 0; i != m_verticals.size(); i++)
        if (m_verticals.at(i)->contains(area->uin()))
            return m_verticals.at(i);
    return 0;
}

void Map::updateVertical(MapVertical *vertical)
{
    if (vertical == 0)
        vertical = qobject_cast<MapVertical*>(QObject::sender());
    if (vertical)
    {
        QVector<MapDoor*> doors;
        MapArea *area;
        for (int i = 0; i != m_floors.size(); i++)
        {
            area = m_floors.at(i)->area(vertical->area(m_floors.at(i)->uin()));
            if (area != 0)
            {
                if (area->doorsNumber() > 0)
                {
                    doors.append(area->door(0));
//                    if (area->doorsNumber() > 1)
//                        area->setBrush(Qt::red);
                }
            }
        }
        if (!doors.isEmpty())
            for (int i = 0; i != doors.size(); i++)
            {
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
            }
        GraphArc::VerticalType type = vertical->type();
        if ((type == GraphArc::Stairs) | (type == GraphArc::Lift))
        {
            QString symbol = "";
            bool isFirst = true;
            for (int i = 0; i != m_floors.size(); i++)
            {
                area = m_floors.at(i)->area(
                        vertical->area(m_floors.at(i)->uin()));
                if (area != 0)
                {
                    if (area->doorsNumber() > 0)
                    {
                        if (!vertical->name().isEmpty())
                            area->setName(vertical->name());
                        symbol = "" ;
                        switch (vertical->type())
                        {
                        case GraphArc::Stairs:
                            if (isFirst)
                            {
                                symbol = tr("￬");
                                isFirst = false;
                            }
                            else
                                symbol = tr("⇅");
                            break;
                        case GraphArc::Lift:
                            if (isFirst)
                            {
                                symbol = tr("⇓");
                                isFirst = false;
                            }
                            else
                                symbol = tr("⇕");
                            break;
                        default:
                            break;
                        }
                        if (!symbol.isEmpty())
                            area->setInscription(symbol);
                    }
                }
            }
            symbol = "" ;
            switch (vertical->type())
            {
            case GraphArc::Stairs:
                if (!doors.isEmpty())
                    symbol = tr("￪");
                break;
            case GraphArc::Lift:
                if (!doors.isEmpty())
                    symbol = tr("⇑");
                break;
            default:
                break;
            }
            quint32 floorUin = doors.last()->floorUin();
            floorByUin(floorUin)->area(
                    vertical->area(floorUin))->setInscription(symbol);

            if (!doors.isEmpty())
            {
                qreal lengthStairsDown = convertRealMToPix(m_lengthStairsDown);
                qreal lengthStairsUp = convertRealMToPix(m_lengthStairsUp);
                qreal lengthLift = convertRealMToPix(
                        Global::cSpeedMPerMin*Global::cTimeLift);
                for (int i = 0; i != doors.size() - 1; i++)
                    if ((doors.at(i)->node() != 0) &
                        (doors.at(i+1)->node() != 0))
                    {
                        GraphNode *node1 = doors.at(i)->node();
                        GraphNode *node2 = doors.at(i+1)->node();
                        if ((type == GraphArc::Stairs) |
                            (type == GraphArc::Lift))
                        {
                            GraphArc *arcDown = new GraphArc(
                                    node1, node2, type, GraphArc::Down);
                            GraphArc *arcUp = new GraphArc(
                                    node2, node1, type, GraphArc::Up);
                            if (type == GraphArc::Stairs)
                            {
                                arcDown->setLenght(lengthStairsDown);
                                arcUp->setLenght(lengthStairsUp);
                            }
                            if (type == GraphArc::Lift)
                            {
                                arcDown->setLenght(lengthLift);
                                arcUp->setLenght(lengthLift);
                            }
                        }
                    }
            }
        }
    }
}

void Map::selectVertical(const int vertNum)
{
    for (int i = 0; i != m_floors.size(); i++)
        m_floors.at(i)->selection()->blockSignals(true);
    deselectVertical();
    m_selectedVertical = vertNum;
    for (int i = 0; i != m_floors.size(); i++)
    {
        quint32 floorUin = m_floors.at(i)->uin();
        quint32 areaUin = m_verticals.at(vertNum)->area(floorUin);
        if (areaUin)
        {
            MapArea *area = m_floors.at(i)->area(areaUin);
            if (area)
                m_floors.at(i)->selection()->addItem(area);
            else
                m_verticals.at(vertNum)->deleteArea(floorUin);
        }
    }
    for (int i = 0; i != m_floors.size(); i++)
        m_floors.at(i)->selection()->blockSignals(false);
}

void Map::deselectVertical()
{
    m_selectedVertical = -1;
    for (int i = 0; i != m_floors.size(); i++)
        m_floors.at(i)->selection()->clear();
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

QList<MapArea*> Map::findAreas(const QRegExp str) const
{
    QList<MapArea*> result;
    for (int i = m_floors.size() - 1; i != -1; i--)
        result << m_floors.at(i)->findAreas(str);
    return result;
}

Graph* Map::graph() const
{
    return m_graph;
}

QGraphicsItem* Map::start() const
{
    return m_start->item();
}

void Map::setStart(QGraphicsItem *item)
{
    if (((item->type() == MapArea::Type) | (item->type() == MapDoor::Type) |
        (item->type() == GraphNode::Type)) & (m_start->item() != item))
    {
        m_start->addItem(item);
        QVector<GraphNode*> nodes = getNodesFromItem(item);
        if (!nodes.isEmpty())
            m_graph->setStartNodes(nodes);
    }
}

QGraphicsItem* Map::finish() const
{
    return m_finish->item();
}

void Map::setFinish(QGraphicsItem *item)
{
    if (((item->type() == MapArea::Type) | (item->type() == MapDoor::Type) |
        (item->type() == GraphNode::Type)) & (m_finish->item() != item))
    {
        m_finish->addItem(item);
        QVector<GraphNode*> nodes = getNodesFromItem(item);
        if (!nodes.isEmpty())
            m_graph->setFinishNodes(nodes);
    }
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

bool Map::isWayExist() const
{
    return !graph()->way().isEmpty();
}

Map::WayInfo* Map::wayInfo() const
{
    WayInfo *info = new WayInfo;
    info->length = 0;
    info->floorsNumber = 0;
    info->stairsNumber = 0;
    info->stairsFloorsDownNumber = 0;
    info->stairsFloorsUpNumber = 0;
    info->liftsNumber = 0;
    info->liftsFloorsDownNumber = 0;
    info->liftsFloorsUpNumber = 0;
    qreal lengthPix = 0;
    GraphArc::VerticalType lastArcType = GraphArc::None;
    QVector<GraphNode*> way = graph()->way();
    QSet<quint32> floors;
    if (isWayExist())
    {
        for (int i = 0; i != way.size() - 1; i++)
        {
            GraphArc *arc = way.at(i)->arc(way.at(i+1), true);
            switch (arc->verticalType())
            {
            case GraphArc::None:
                lengthPix += arc->lenght();
                floors.insert(way.at(i+1)->floorUin());
                break;
            case GraphArc::Stairs:
                if (lastArcType != GraphArc::Stairs)
                    info->stairsNumber++;
                if (arc->verticalDirection() == GraphArc::Down)
                    info->stairsFloorsDownNumber++;
                if (arc->verticalDirection() == GraphArc::Up)
                   info-> stairsFloorsUpNumber++;
                break;
            case GraphArc::Lift:
                if (lastArcType != GraphArc::Lift)
                    info->liftsNumber++;
                if (arc->verticalDirection() == GraphArc::Down)
                    info->liftsFloorsDownNumber++;
                if (arc->verticalDirection() == GraphArc::Up)
                    info->liftsFloorsUpNumber++;
                break;
            case GraphArc::Room:
                break;
            }
            lastArcType = arc->verticalType();
        }
        info->floorsNumber = floors.size();
        info->length = convertPixToRealM(lengthPix);
//        time = length/cSpeedMPerMin + stairsFloorsDownNumber*cTimeStairsDown +
//               stairsFloorsUpNumber*cTimeStairsUp + liftsNumber*cTimeLift;
    }
    return info;
}

//QVector<QPointF*> Map::graphNodesCoordinates()
//{
//    return m_graph->nodesCoordinates();
//}

//const Graph* Map::graph() const
//{
//    return m_graph;
//}
