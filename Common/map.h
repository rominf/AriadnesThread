#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QDataStream>
#include "graph.h"
#include "graphnode.h"
#include "mapfloor.h"

class Map : public QObject
{
    Q_OBJECT

    friend QDataStream &operator<<(QDataStream &output, const Map &map);
    friend QDataStream &operator>>(QDataStream &input, Map &map);
public:
    explicit Map(qreal mapPixPerRealM, qreal mapPixPerDisplayM,
                 qreal mapPixSizeX, qreal mapPixSizeY, QObject *parent);
    void setPixPerDisplayM(qreal r);
    int floorsNumber();
    void insertFloor(int i);
    void swapFloors(int x, int y);
    void removeFloor(int i);
    MapFloor* floor(int i) const;
    Graph* graph() const;
//    QVector<QPointF*> graphNodesCoordinates();

//    void graphStartAnew();
//    const Graph* graph() const;

//signals:

public slots:
    void graphStartAnew();

private slots:
    void addNode(QPointF point, MapFloor &floor);
    void deleteNode(GraphNode *node);
    void setLastNode(GraphNode *node);
    void addGraphItem(QGraphicsItem *item);
//    void deleteGraphItem(QGraphicsItem *item);
//    void graphDeleteNode(GraphNode *node);
//    void graphNodesChanged(QVector<QPointF*> &nodes, int floor);

private:
    Graph *m_graph;

    QVector<MapFloor*> m_floors;
    qreal m_pixPerRealM;
    qreal m_pixPerDisplayM;
    qreal m_realMPerDisplayM;
    qreal m_pixSizeX;
    qreal m_pixSizeY;

    qreal convertPixToDisplayM(qreal r) const;
    qreal convertPixToRealM(qreal r) const;
    qreal convertDisplayMToPix(qreal r) const;
    qreal convertRealMToPix(qreal r) const;
};

#endif // MAP_H
