#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QDataStream>
#include "graph.h"
#include "graphnode.h"
#include "mapfloor.h"
#include "mapvertical.h"

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
    MapFloor* floor(const int i) const;
    MapFloor* floorByUin(const quint32 uin) const;
    void addVertical();
    void deleteVertical(const int vertNum);
    int verticalsNumber();
    MapVertical* vertical(const int vertNum) const;
    void setNameVertical(const int vertNum, const QString &name);
    void setPassageVertical(const int vertNum, const bool b);
    bool passageVertical(const int vertNum) const;
    void selectVertical(const int vertNum);
    void deselectVertical();
    Graph* graph() const;
    void setStart(QGraphicsItem *item);
    void setFinish(QGraphicsItem *item);
    void way();
    void clearWay();
//    QVector<QPointF*> graphNodesCoordinates();

//    void graphStartAnew();
//    const Graph* graph() const;

//signals:

public slots:
    void graphStartAnew();

private slots:
    void areaActivated(MapArea *area, bool activated);
    void addNode(QPointF point, quint32 floorUin);
    void deleteNode(GraphNode *node);
    void setLastNode(GraphNode *node);
    void addGraphItem(QGraphicsItem *item);
//    void deleteGraphItem(QGraphicsItem *item);
//    void graphDeleteNode(GraphNode *node);
//    void graphNodesChanged(QVector<QPointF*> &nodes, int floor);

private:
    Graph *m_graph;
    QVector<MapFloor*> m_floors;
    QVector<MapVertical*> m_verticals;
    int m_selectedVertical;
//    MapSelection *m_copy;
    QVector<GraphNode*> m_startNodes;
    QVector<GraphNode*> m_finishNodes;

    qreal m_pixPerRealM;
    qreal m_pixPerDisplayM;
    qreal m_realMPerDisplayM;
    qreal m_pixSizeX;
    qreal m_pixSizeY;

    qreal convertPixToDisplayM(qreal r) const;
    qreal convertPixToRealM(qreal r) const;
    qreal convertDisplayMToPix(qreal r) const;
    qreal convertRealMToPix(qreal r) const;
    QVector<GraphNode*> getNodesFromItem(QGraphicsItem *item);
};

#endif // MAP_H
