#ifndef GRAPHARC_H
#define GRAPHARC_H

#include <qmath.h>
#include <QGraphicsLineItem>
#include "graphnode.h"

class GraphNode;
class GraphArc : public QGraphicsLineItem
{
public:
    GraphArc(GraphNode *node1, GraphNode *node2);
    ~GraphArc();

    enum {Type = QGraphicsItem::UserType + 3};
    GraphNode *node1() const;
    GraphNode *node2() const;
    int type() const;
    quint32 uin();
private:
    GraphNode *m_node1;
    GraphNode *m_node2;
    quint32 m_uin;

    static quint32 m_count; // Counter for making uins
};

#endif // GRAPHARC_H
