#include "mapselection.h"

MapSelection::MapSelection(bool multiselection):
        m_multiselection(multiselection)
{
    brushNormalArea = new QBrush(Qt::NoBrush);
    brushNormalDoor = new QBrush(Qt::white);
    brushSelectedArea = new QBrush(QColor::fromRgb(255, 250, 205, 127));
    brushSelectedDoor = new QBrush(QColor::fromRgb(255, 250, 205, 255));
    penNormal = new QPen(Qt::SolidLine);
    penSelected = new QPen(Qt::SolidLine);
    penSelected->setWidthF(4);
    penSelected->setCapStyle(Qt::RoundCap);
}

void MapSelection::addItem(QGraphicsItem *item)
{
        if (!m_multiselection)
            clear();
        QAbstractGraphicsShapeItem *shapeItem;
        if (item->type() != QGraphicsTextItem::Type)
            shapeItem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(item);
        else
            shapeItem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(
                    item->parentItem());

        switch (shapeItem->type())
        {
        case MapArea::Type:
            m_areas.append(qgraphicsitem_cast<MapArea*>(shapeItem));
            shapeItem->setBrush(*brushSelectedArea);
            break;
        case MapDoor::Type:
            m_doors.append(qgraphicsitem_cast<MapDoor*>(shapeItem));
            shapeItem->setBrush(*brushSelectedDoor);
            break;
        default:
            return;
        }
        shapeItem->setPen(*penSelected);
}

void MapSelection::removeItem(QGraphicsItem *item)
{
    switch (item->type())
    {
    case MapArea::Type:
    {
        MapArea *area = qgraphicsitem_cast<MapArea*>(item);
        area->setBrush(*brushNormalArea);
        area->setPen(*penNormal);
        m_areas.remove(m_areas.indexOf(area));
        break;
    }
    case MapDoor::Type:
    {
        MapDoor *door = qgraphicsitem_cast<MapDoor*>(item);
        door->setBrush(*brushNormalDoor);
        door->setPen(*penNormal);
        m_doors.remove(m_doors.indexOf(door));
        break;
    }
    default:
        return;
    }
}

QAbstractGraphicsShapeItem* MapSelection::item()
{
    if (m_areas.size() == 1)
        return m_areas[0];
    else if (m_doors.size() == 1)
        return m_doors[0];
    else
        return 0;
}

bool MapSelection::isEmpty()
{
    return m_areas.isEmpty() & m_doors.isEmpty();
}

void MapSelection::clear()
{
    while (m_areas.size() != 0)
        removeItem(m_areas[0]);
    while (m_doors.size() != 0)
        removeItem(m_doors[0]);
}
