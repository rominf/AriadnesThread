#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QDataStream>
#include "graph.h"
#include "grapharc.h"
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
    void swapVerticals(int x, int y);
    int verticalsNumber();
    MapVertical* vertical(const int vertNum) const;
    void setNameVertical(const int vertNum, const QString &name);
    void setTypeVertical(const int vertNum, GraphArc::VerticalType);
    void updateVertical(const int vertNum);
    GraphArc::VerticalType typeVertical(const int vertNum) const;
    void selectVertical(const int vertNum);
    void deselectVertical();
    bool areaCopy(MapArea *area, int floorFromIndex, int floorToIndex);
    bool areasAutoRenaming() const;
    void setAreasAutoRenaming(bool enabled);
    Graph* graph() const;
    void setStart(QGraphicsItem *item);
    void setFinish(QGraphicsItem *item);
    bool isStartAndFinishNodesValid() const;
    void way(const GraphArc::WayPermissions permissions);
    void paintWay();
    void clearWay();
    bool wayInfo(qreal &length, int &stairsDownNumber, int &stairsUpNumber,
                 int &liftsNumber, qreal &time) const;
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
    void deleteArc(GraphArc *arc);
    void setLastNode(GraphNode *node);
    void addGraphItem(QGraphicsItem *item);
//    void deleteGraphItem(QGraphicsItem *item);
//    void graphDeleteNode(GraphNode *node);
//    void graphNodesChanged(QVector<QPointF*> &nodes, int floor);

private:
    static const qreal cSpeedMPerMin = 85;
    static const qreal cTimeStairsDown = 0.5;
    static const qreal cTimeStairsUp = 1;
    static const qreal cTimeLift = 0.5;
    static qreal m_lengthStairsDown;
    static qreal m_lengthStairsUp;

    Graph *m_graph;
    QVector<MapFloor*> m_floors;
    QVector<MapVertical*> m_verticals;
    bool m_areasAutoRenaming;
    int m_selectedVertical;
//    MapSelection *m_copy;
//    QVector<GraphNode*> m_startNodes;
//    QVector<GraphNode*> m_finishNodes;

    qreal m_pixPerRealM;
    qreal m_pixPerDisplayM;
//    qreal m_realMPerDisplayM;
    qreal m_pixSizeX;
    qreal m_pixSizeY;

    qreal convertPixToDisplayM(qreal r) const;
    qreal convertPixToRealM(qreal r) const;
    qreal convertDisplayMToPix(qreal r) const;
    qreal convertRealMToPix(qreal r) const;
    QVector<GraphNode*> getNodesFromItem(QGraphicsItem *item);
};

#endif // MAP_H
