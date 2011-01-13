#ifndef MAPFLOOR_H
#define MAPFLOOR_H

#include <qmath.h>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QLineF>
#include <QMessageBox>
#include <QObject>
#include <QPixmap>
#include <QRectF>
#include <QStack>
#include "geometry.h"
#include "graphnode.h"
#include "maparea.h"
#include "mapbase.h"
#include "mapselection.h"

//extern "C"
//{
//    #include "gpc.h"
//}

class MapFloor: public QGraphicsScene
{
    Q_OBJECT

    friend QDataStream &operator<<(QDataStream &output, const MapFloor &floor);
    friend QDataStream &operator>>(QDataStream &input, MapFloor &floor);
public:    
    explicit MapFloor(const QRectF &sceneRect, QObject *parent = 0);

    static const qreal OutlineZValue;
    enum Mode
    {
        Idle, WallAdd, AreaAdd, DoorAdd, NodeAdd, Selection
    };
    enum MagnetItem
    {
        miNone = 0,
        miLines = 1,
        miTops = 2,
        miNodes = 4
    };
    Q_DECLARE_FLAGS(MagnetItems, MagnetItem)

    QString name() const;
    void setName(const QString &floorName);
    MapFloor::Mode mode();
    void setMode(Mode m);
    bool isBaseExist();
    void addBase(QString fileName);
//    bool isBaseVisible();
    void baseSetVisible(bool visible);
    void crossBaseSetVisible(bool visible);
    MapArea* area(int i = 0);
    QAbstractGraphicsShapeItem* selectedItem();
    virtual void addItem (QGraphicsItem *item);
    MapArea::CreateError addArea(MapArea *area);
    bool addDoor(MapDoor *door);
    void addPointNodesMagnetTo(QPointF point);
    void addArc(GraphArc *arc);
//    void setGraphNodes(QVector<QPointF*> &nodes);
    void magnetToExtensions(bool b);
    void setLastNode(GraphNode *node);
    quint32 uin();
signals:
    void modeChanged(MapFloor::Mode);
    void mouseDoubleClicked();
    void mouseMiddleButtonClicked(QGraphicsItem *item);
    void addedNode(QPointF point, quint32 floor);
    void deletedNode(GraphNode *node);
    void graphStartedAnew();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

//    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change,
//                                 const QVariant &value);

private:
    const qreal cCursorCircleR;

    Mode m_mode;
    bool m_magnetToExtensions;
    QString m_name;
    QGraphicsRectItem *m_border;
    QVector<QLineF*> m_lines;
    QVector<QPointF> m_graphNodes;
    QVector<MapArea*> m_outlines;
//    QVector<MapArea*> areas;
    QVector<QGraphicsLineItem*> m_walls;
    QVector<QGraphicsLineItem*> m_doors;
    MapBase *m_base;
    QGraphicsRectItem *m_crossBase;
    bool m_isCrossLinesActive;
    MapSelection *m_selection;
    quint32 m_uin;

    static quint32 m_count; // Counter for making uins

    QGraphicsLineItem *m_tempLine;
    QVector<QGraphicsLineItem*> m_tempPolyline;
    QGraphicsLineItem *m_crossLineHorizontal;
    QGraphicsLineItem *m_crossLineVertical;
    QGraphicsEllipseItem *m_startPoint;
    QGraphicsEllipseItem *m_cursorCircle;
    GraphNode *m_lastNode;

    QSet<MapArea*> parentAreas(MapArea *area, const QPointF *pos);
    void areasToLineVec(/*MapArea *area*/);
    QVector<MapArea*> pointContainers(QPointF pos);
    bool validatePos(QPointF pos, QPointF &rightPos);
    QPointF getPoint(QPointF m, Geometry::Straight straight, MagnetItems items);
    QLineF getLine(QLineF line, bool first, MagnetItems items,
                   Qt::KeyboardModifiers modifiers);
    QPointF graphGetPoint(QPointF pos);

    void removeLastFromTempPolyline();

    QPolygonF convertLineVecToPolygon(const
                                      QVector<QGraphicsLineItem*> &vec) const;
//    gpc_polygon convertQtToGpcPolygon(const QPolygonF *pgn);

    void takeAwayGraphicsLineItem(QGraphicsLineItem *item);
    void finalizeWall();
    void finalizeArea();
    void deleteArea(MapArea *area);
    void finalizeDoor();
    void deleteDoor(MapDoor *door);
//    void finalizeNode();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MapFloor::MagnetItems)

#endif // MAPFLOOR_H
