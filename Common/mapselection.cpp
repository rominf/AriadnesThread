#include "mapselection.h"

MapSelection::MapSelection(bool multiselection):
        m_multiselection(multiselection)
{
    brushNormalArea = new QBrush(Qt::NoBrush);
    brushNormalDoor = new QBrush(Qt::white);
//    brushNormalNode = new QBrush(Qt::white);
    brushSelectedArea = new QBrush(QColor::fromRgb(255, 250, 205, 127));
    brushSelectedDoor = new QBrush(QColor::fromRgb(255, 250, 205, 255));
//    brushSelectedNode = new QBrush(QColor::fromRgb(255, 250, 205, 255));
    penNormal = new QPen(Qt::SolidLine);
    penSelectedArea = new QPen(Qt::SolidLine);
    penSelectedArea->setWidthF(4);
    penSelectedDoor = new QPen(Qt::SolidLine);
    penSelectedDoor->setWidthF(3);
//    penSelectedNode = new QPen(Qt::SolidLine);
//    penSelectedNode->setWidthF(2);
//    penSelected->setCapStyle(Qt::RoundCap);
}

void MapSelection::addItem(QGraphicsItem *item)
{
    bool b = item != this->item();
    if (!m_multiselection)
        clear();
    if (b)
    {
        QAbstractGraphicsShapeItem *shapeItem = 0;
        int type = item->type();
        if ((type == MapArea::Type) | (type == MapDoor::Type) |
            (type == GraphNode::Type))
            shapeItem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(item);
//        else
//            if (item->type() == QGraphicsTextItem::Type)
//                shapeItem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(
//                        item->parentItem());

        switch (type)
        {
        case MapArea::Type:
            m_areas.append(qgraphicsitem_cast<MapArea*>(shapeItem));
            shapeItem->setBrush(*brushSelectedArea);
            shapeItem->setPen(*penSelectedArea);
            emit areaActivated(m_areas.at(0), true);
            break;
        case MapDoor::Type:
            m_doors.append(qgraphicsitem_cast<MapDoor*>(shapeItem));
            shapeItem->setBrush(*brushSelectedDoor);
            shapeItem->setPen(*penSelectedDoor);
            break;
        case GraphNode::Type:
            m_nodes.append(qgraphicsitem_cast<GraphNode*>(shapeItem));
            brush = shapeItem->brush();
            pen = shapeItem->pen();
            shapeItem->setBrush(GraphNode::cBrushSelected);
            shapeItem->setPen(GraphNode::cPenSelected);
            break;
        case GraphArc::Type:
        {
            GraphArc *arc = qgraphicsitem_cast<GraphArc*>(item);
            m_arcs.append(arc);
            pen = arc->pen();
            arc->setPen(GraphArc::cPenSelected);
            break;
        }
        default:
            return;
        }
    }
}

void MapSelection::removeItem(QGraphicsItem *item)
{
    switch (item->type())
    {
    case MapArea::Type:
    {
        MapArea *area = qgraphicsitem_cast<MapArea*>(item);
        emit areaActivated(m_areas.at(0), false);
        area->setBrush(*brushNormalArea);
        area->setPen(*penNormal);
        int i = m_areas.indexOf(area);
        if (i > -1)
            m_areas.remove(i);
        break;
    }
    case MapDoor::Type:
    {
        MapDoor *door = qgraphicsitem_cast<MapDoor*>(item);
        door->setBrush(*brushNormalDoor);
        door->setPen(*penNormal);
        int i = m_doors.indexOf(door);
        if (i > -1)
            m_doors.remove(i);
        break;
    }
    case GraphNode::Type:
    {
        GraphNode *node = qgraphicsitem_cast<GraphNode*>(item);
        node->setBrush(brush/*GraphNode::cBrushNormal*/);
        node->setPen(pen/*GraphNode::cPenNormal*/);
        int i = m_nodes.indexOf(node);
        if (i > -1)
            m_nodes.remove(i);
        break;
    }
    case GraphArc::Type:
        {
        GraphArc *arc = qgraphicsitem_cast<GraphArc*>(item);
        arc->setPen(pen);
        int i = m_arcs.indexOf(arc);
        if (i > -1)
            m_arcs.remove(i);
        break;
    }
    default:
        return;
    }
}

QGraphicsItem* MapSelection::item()
{
    if (!m_multiselection)
    {
        if (m_areas.size() == 1)
            return m_areas[0];
        if (m_doors.size() == 1)
            return m_doors[0];
        if (m_nodes.size() == 1)
            return m_nodes[0];
        if (m_arcs.size() == 1)
            return m_arcs[0];
    }
    return 0;
}

bool MapSelection::isEmpty()
{
    return m_areas.isEmpty() & m_doors.isEmpty() &
            m_nodes.isEmpty() & m_arcs.isEmpty();
}

void MapSelection::clear()
{
    while (m_areas.size() != 0)
        removeItem(m_areas[0]);
    while (m_doors.size() != 0)
        removeItem(m_doors[0]);
    while (m_nodes.size() != 0)
        removeItem(m_nodes[0]);
    while (m_arcs.size() != 0)
        removeItem(m_arcs[0]);
}
