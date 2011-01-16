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
//    static const qreal cLengthStairs = ;

    Graph();
//    enum WayPermission
//    {
//        GraphArc::None,

//    };

//    void addFreeNode(QPointF *point);
    void deleteArc(GraphArc *arc);
    void addNode(QPointF point, quint32 floorUin/*, GraphNode *linkedNode*/);
    GraphNode* lastNode();
    void deleteNode(GraphNode *node);
    void copyFloor(quint32 fromUin, quint32 toUin);
    void setVisible(bool visible);
    void startAnew();
    void makeWay(const QVector<GraphNode*> start,
                 const QVector<GraphNode*> finish,
                 const GraphArc::WayPermissions permissions);
    void paintWay(bool isActive = true);
    void clearWay();
    QVector<GraphNode*> way() const;
//    bool wayInfo(qreal &lengthPix, int &stairsDownNumber, int &stairsUpNumber,
//                 int &liftsNumber, qreal &time) const;
    qreal djkstra(GraphNode *start, const QVector<GraphNode*> *finish,
                  const GraphArc::WayPermissions permissions,
                  const qreal minWayLength);
signals:
    void graphItemAdded(QGraphicsItem *item);
    void graphItemDeleted(QGraphicsItem *item);
    void lastNodeChanged(GraphNode *node);
//    void graphNodesChanged(QVector<QPointF*> &nodes, int floor);
private:
    QVector<GraphNode*> m_startNodes;
    QVector<GraphNode*> m_way;
    GraphNode *m_lastNode;
    bool m_visible;

    void setLastNode(GraphNode *node);
//    bool m_isFirstNode;
//    QVector<QPointF*> m_nodesCoordinates;
//    void nodesCoordinates();
};

#endif // MAPGRAPH_H
