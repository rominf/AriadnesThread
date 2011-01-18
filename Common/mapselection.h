#ifndef MAPSELECTION_H
#define MAPSELECTION_H

#include <QBrush>
#include <QPen>
#include <QVector>
#include "graphnode.h"
#include "maparea.h"
#include "mapdoor.h"

class MapSelection
{
public:
    MapSelection(bool multiselection = false);
    void addItem(QGraphicsItem *item);
    void removeItem(QGraphicsItem *item);
    QGraphicsItem* item();
    bool isEmpty();
    void clear();

private:
    bool m_multiselection;
    QVector<MapArea*> m_areas;
    QVector<MapDoor*> m_doors;
    QVector<GraphNode*> m_nodes;
    QVector<GraphArc*> m_arcs;
    QBrush *brushNormalArea;
    QBrush *brushNormalDoor;
    QBrush *brushSelectedArea;
    QBrush *brushSelectedDoor;
    QPen *penNormal;
    QPen *penSelectedArea;
    QPen *penSelectedDoor;

    QBrush brush;
    QPen pen;
};

#endif // MAPSELECTION_H
