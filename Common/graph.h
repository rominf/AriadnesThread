#ifndef MAPGRAPH_H
#define MAPGRAPH_H

#include <QStack>
#include "grapharc.h"
#include "graphnode.h"

class Graph: public QObject
{
    Q_OBJECT

public:
    Graph();
//    void addFreeNode(QPointF *point);
    void deleteArc(GraphArc *arc);
    void addNode(QPointF &point, int floor/*, GraphNode *linkedNode*/);
    GraphNode* lastNode();
    void deleteNode(GraphNode *node);
    void setVisible(bool visible);
    void startAnew();
signals:
    void graphItemAdded(QGraphicsItem *item);
    void graphItemDeleted(QGraphicsItem *item);
//    void graphNodesChanged(QVector<QPointF*> &nodes, int floor);
private:
    QVector<GraphNode*> m_freeNodes;
    GraphNode *m_lastNode;
    bool m_isFirstNode;
//    QVector<QPointF*> m_nodesCoordinates;
//    void nodesCoordinates();
};

#endif // MAPGRAPH_H
