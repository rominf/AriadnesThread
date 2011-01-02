#ifndef MAPSELECTION_H
#define MAPSELECTION_H

#include <QBrush>
#include <QPen>
#include <QVector>
#include "maparea.h"
#include "mapdoor.h"

class MapSelection
{
public:
    MapSelection(bool multiselection = false);
    void addItem(QGraphicsItem *item);
    void removeItem(QGraphicsItem *item);
    QAbstractGraphicsShapeItem* item();
    bool isEmpty();
    void clear();

private:
    bool m_multiselection;
    QVector<MapArea*> m_areas;
    QVector<MapDoor*> m_doors;
    QBrush *brushNormalArea;
    QBrush *brushNormalDoor;
    QBrush *brushSelectedArea;
    QBrush *brushSelectedDoor;
    QPen *penNormal;
    QPen *penSelected;
};

#endif // MAPSELECTION_H
