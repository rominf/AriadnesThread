#ifndef MAPGRAPH_H
#define MAPGRAPH_H

#include <QMessageBox>
#include <QStack>
#include "grapharc.h"
#include "graphnode.h"

class Graph: public QObject
{
    Q_OBJECT

    friend QDataStream &operator<<(QDataStream &output, const Graph &graph);
    friend QDataStream &operator>>(QDataStream &input, Graph &graph);
public:
    Graph();
//    void addFreeNode(QPointF *point);
    void deleteArc(GraphArc *arc);
    void addNode(QPointF point, quint32 floorUin/*, GraphNode *linkedNode*/);
    GraphNode* lastNode();
    void deleteNode(GraphNode *node);
    void copyFloor(quint32 fromUin, quint32 toUin);
    void setVisible(bool visible);
    void startAnew();
    void way(QVector<GraphNode*> start, QVector<GraphNode*> finish);
    void clearWay();
    qreal djkstra(GraphNode *start, QVector<GraphNode*> *finish,
                  qreal minWayLength);
signals:
    void graphItemAdded(QGraphicsItem *item);
    void graphItemDeleted(QGraphicsItem *item);
    void lastNodeChanged(GraphNode *node);
//    void graphNodesChanged(QVector<QPointF*> &nodes, int floor);
private:
    QVector<GraphNode*> m_startNodes;
    QVector<GraphNode*> m_way;
    GraphNode *m_lastNode;

    void setLastNode(GraphNode *node);
    void paintWay(bool isActive = true);
//    bool m_isFirstNode;
//    QVector<QPointF*> m_nodesCoordinates;
//    void nodesCoordinates();
};

#endif // MAPGRAPH_H
