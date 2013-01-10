#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QDataStream>
#include <QTextStream>

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
    struct Properties
    {
        QString name;
        QString description;
        qreal longitude;
        qreal latitude;
    };

    void setPixPerDisplayM(qreal r);
    QSizeF convertPixToRealM(const QSizeF size) const;
    QSizeF size() const;
    QSizeF areaSize(const MapArea *area) const;
    QPointF pos(const MapFloor *floor) const;
    Properties* properties();
    void setProperties(Properties *properties);
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

public slots:
    void graphStartAnew();
    void updateVertical(MapVertical *vertical);

private slots:
    void areaActivated(MapArea *area, bool activated);
    void addNode(QPointF point, quint32 floorUin);
    void deleteNode(GraphNode *node);
    void deleteArc(GraphArc *arc);
    void setLastNode(GraphNode *node);
    void addGraphItem(QGraphicsItem *item);

private:
    static qreal m_lengthStairsDown;
    static qreal m_lengthStairsUp;

    Properties *m_properties;
    Graph *m_graph;
    QVector<MapFloor*> m_floors;
    QVector<MapVertical*> m_verticals;
    MapSelection *m_start;
    MapSelection *m_finish;
    bool m_areasAutoRenaming;
    int m_selectedVertical;

    qreal m_pixPerRealM;
    qreal m_pixPerDisplayM;
    qreal m_pixSizeX;
    qreal m_pixSizeY;

    qreal convertPixToDisplayM(const qreal r) const;
    QPointF convertPixToDisplayM(const QPointF point) const;
    qreal convertPixToRealM(const qreal r) const;
    QPointF convertPixToRealM(const QPointF point) const;
    QRectF convertPixToRealM(const QRectF rect) const;
    qreal convertDisplayMToPix(const qreal r) const;
    qreal convertRealMToPix(const qreal r) const;
    QVector<GraphNode*> getNodesFromItem(QGraphicsItem *item);
};

#endif // MAP_H
