#ifndef GRAPHARC_H
#define GRAPHARC_H

#include <qmath.h>
#include <QGraphicsLineItem>
#include "graphnode.h"
#include "geometry.h"

class GraphNode;
class GraphArc : public QGraphicsLineItem
{
public:
    GraphArc(GraphNode *node1, GraphNode *node2);
    ~GraphArc();

    enum {Type = QGraphicsItem::UserType + 3};
    static const QPen cPenNormal;
    static const QPen cPenSelected;
    static const QPen cPenWay;

    GraphNode *node1() const;
    GraphNode *node2() const;
    bool contain(GraphNode *node) const;
    void setLenght(const qreal lenght);
    qreal lenght() const;
    int type() const;
    quint32 floorUin() const;
    quint32 uin() const;
private:
    GraphNode *m_node1;
    GraphNode *m_node2;
    int m_lenght;
    quint32 m_uin;

    static quint32 m_count; // Counter for making uins
};

#endif // GRAPHARC_H
