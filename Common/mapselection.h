#ifndef MAPSELECTION_H
#define MAPSELECTION_H

#include <QObject>
#include <QBrush>
#include <QPen>
#include <QVector>

#include "graphnode.h"
#include "maparea.h"
#include "mapdoor.h"

class MapSelection: public QObject
{
    Q_OBJECT
public:
    MapSelection(QColor color, bool multiselection = false);
    void addItem(QGraphicsItem *item);
    void removeItem(QGraphicsItem *item);
    QGraphicsItem* item();
    bool isEmpty();
    void clear();
signals:
    void areaActivated(MapArea *area, bool activated);
private:
    QVector<MapArea*> m_areas;
    QVector<MapDoor*> m_doors;
    QVector<GraphNode*> m_nodes;
    QVector<GraphArc*> m_arcs;
    QColor m_color;
    QBrush m_brush;
    QPen m_pen;
    bool m_multiselection;
};

#endif // MAPSELECTION_H
