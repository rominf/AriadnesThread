#ifndef MAPGRAPH_H
#define MAPGRAPH_H

#include <qmath.h>
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
    void deleteArc(GraphArc *arc);
    void addNode(QPointF point, quint32 floorUin);
    GraphNode* lastNode();
    void deleteNode(GraphNode *node);
    void copyFloor(quint32 fromUin, quint32 toUin);
    void setVisible(bool visible);
    void startAnew();
    void setStartNodes(const QVector<GraphNode*> nodes);
    void setFinishNodes(const QVector<GraphNode*> nodes);
    bool isStartAndFinishNodesValid() const;
    void makeWay(const GraphArc::WayPermissions permissions);
    void paintWay(bool isActive = true);
    void clearWay();
    QVector<GraphNode*> way() const;
    qreal djkstra(GraphNode *start, const QVector<GraphNode*> *finish,
                  const GraphArc::WayPermissions permissions,
                  const qreal minWayLength);
signals:
    void graphItemAdded(QGraphicsItem *item);
    void graphItemDeleted(QGraphicsItem *item);
    void lastNodeChanged(GraphNode *node);
private:
    static qreal m_lengthLiftWaiting;
    QVector<GraphNode*> m_nodes;
    QVector<GraphNode*> m_startNodes;
    QVector<GraphNode*> m_finishNodes;
    QVector<GraphNode*> m_way;
    GraphNode *m_lastNode;
    bool m_visible;

    void setLastNode(GraphNode *node);
};

#endif // MAPGRAPH_H
