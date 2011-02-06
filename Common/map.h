#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QDataStream>
#include "global.h"
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
    struct WayInfo
    {
        int floorsNumber;
        qreal length;
        int stairsNumber;
        int stairsFloorsDownNumber;
        int stairsFloorsUpNumber;
        int liftsNumber;
        int liftsFloorsDownNumber;
        int liftsFloorsUpNumber;
    };

    void setPixPerDisplayM(qreal r);
    int floorsNumber();
    void insertFloor(int i);
    void swapFloors(int x, int y);
    void removeFloor(int i);
    MapFloor* floor(const int i) const;
    MapFloor* floorByUin(const quint32 uin) const;
    int floorNumber(MapFloor* floor) const;
    void addVertical();
    void deleteVertical(const int vertNum);
    void swapVerticals(int x, int y);
    int verticalsNumber();
    MapVertical* vertical(const int vertNum) const;
    MapVertical* vertical(const MapArea *area) const;
    void selectVertical(const int vertNum);
    void deselectVertical();
    bool areaCopy(MapArea *area, int floorFromIndex, int floorToIndex);
    bool areasAutoRenaming() const;
    void setAreasAutoRenaming(bool enabled);
    QList<MapArea*> findAreas(const QRegExp str) const;
    Graph* graph() const;
    QGraphicsItem* start() const;
    void setStart(QGraphicsItem *item);
    QGraphicsItem* finish() const;
    void setFinish(QGraphicsItem *item);
    bool isStartAndFinishNodesValid() const;
    void way(const GraphArc::WayPermissions permissions);
    void paintWay();
    void clearWay();
    bool isWayExist() const;
    WayInfo* wayInfo() const;
//    QVector<QPointF*> graphNodesCoordinates();

//    void graphStartAnew();
//    const Graph* graph() const;

//signals:

public slots:
    void graphStartAnew();
    void updateVertical(MapVertical *vertical = 0);

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
    static qreal m_lengthStairsDown;
    static qreal m_lengthStairsUp;

    Graph *m_graph;
    QVector<MapFloor*> m_floors;
    QVector<MapVertical*> m_verticals;
    MapSelection *m_start;
    MapSelection *m_finish;
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
