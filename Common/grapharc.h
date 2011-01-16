#ifndef GRAPHARC_H
#define GRAPHARC_H

#include <qmath.h>
#include <QGraphicsLineItem>
#include "graphnode.h"
#include "geometry.h"

class GraphNode;
class GraphArc : public QGraphicsLineItem
{
    friend QDataStream& operator<<(QDataStream &output, const GraphArc &arc);
    friend QDataStream& operator>>(QDataStream &input, GraphArc &arc);
public:
    enum VerticalType
    {
        None = 0,
        Stairs = 1,
        Lift = 2,
        Room = 4
    };
    Q_DECLARE_FLAGS(WayPermissions, VerticalType)
    GraphArc(GraphNode *node1, GraphNode *node2,
             const VerticalType type = None, const bool oneWay = false);
    ~GraphArc();

    enum {Type = QGraphicsItem::UserType + 3};


    static const QPen cPenNormal;
    static const QPen cPenSelected;
    static const QPen cPenWay;

    GraphNode *node1() const;
    GraphNode *node2() const;
    bool isRight(const GraphNode *node) const;
    bool contain(GraphNode *node) const;
    void setLenght(const qreal lenght);
    qreal lenght() const;
    GraphArc::VerticalType verticalType() const;
    int type() const;
    quint32 floorUin() const;
    quint32 uin() const;
private:
    GraphNode *m_node1;
    GraphNode *m_node2;
    VerticalType m_verticalType;
    bool m_oneWay;
    int m_lenght;
    quint32 m_uin;

    static quint32 m_count; // Counter for making uins
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GraphArc::WayPermissions)

#endif // GRAPHARC_H
