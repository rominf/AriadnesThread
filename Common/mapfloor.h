#ifndef MAPFLOOR_H
#define MAPFLOOR_H

#include <qmath.h>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QLineF>
#include <QMap>
#include <QMessageBox>
#include <QObject>
#include <QPixmap>
#include <QRectF>
#include <QStack>

#include "global.h"
#include "geometry.h"
#include "graphnode.h"
#include "maparea.h"
#include "mapbase.h"
#include "mapselection.h"

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
        Idle, /*WallAdd, */AreaAdd, DoorAdd, NodeAdd, Selection
    };
    enum SnapItem
    {
        miNone = 0,
        miLines = 1,
        miTops = 2,
        miNodes = 4
    };
    Q_DECLARE_FLAGS(SnapItems, SnapItem)

    static const QString modeName(Mode mode);
    QString name() const;
    void setName(const QString &floorName);
    MapFloor::Mode mode();
    void setMode(Mode m);
    bool isBaseExist();
    void addBase(QString fileName);
    void baseSetVisible(bool visible);
    void gridSetVisible(bool visible);
    MapArea* area(int i);
    MapArea* area(quint32 uin);
    MapSelection* selection();
    virtual void addItem (QGraphicsItem *item);
    MapArea::CreationError addArea(MapArea *area);
    bool addDoor(MapDoor *door);
    bool addDoor(const QPointF &point);
    void addPointNodesSnapTo(const QPointF point);
    void addArc(GraphArc *arc);
    void setSnapToExtensions(const bool b);
    void setLastNode(GraphNode *node);
    QList<MapArea*> findAreas(const QRegExp &str) const;
    QPointF pos() const;
    quint32 uin();
signals:
    void modeChanged(MapFloor::Mode);
    void mouseMoved(/*const QPointF &pos*/);
    void mouseDoubleClicked();
    void mouseMiddleButtonClicked(QGraphicsItem *item);
    void addedNode(QPointF point, quint32 floor);
    void deletedNode(GraphNode *node);
    void deletedArc(GraphArc *arc);
    void graphStartedAnew();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private:
    const qreal cCursorCircleR;
    static QMap<int, QString> cModesNames;

    Mode m_mode;
    QPointF m_pos;
    bool m_snapToExtensions;
    QString m_name;
    QGraphicsRectItem *m_border;
    QVector<QLineF*> m_lines;
    QVector<QPointF> m_graphNodes;
    QVector<MapArea*> m_outlines;
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
    void areasToLineVec();
    QVector<MapArea*> pointContainers(QPointF pos);
    bool validatePos(QPointF pos, QPointF &rightPos);
    QPointF getPoint(QPointF m, Geometry::Straight straight,
                     SnapItems items, bool SnapToExtensions, bool first);
    QLineF getLine(QLineF line, bool first, SnapItems items,
                   Qt::KeyboardModifiers modifiers);
//    QPointF graphGetPoint(QPointF pos);
    void removeLastFromTempPolyline();
    QPolygonF convertLineVecToPolygon(
            const QVector<QGraphicsLineItem*> &vec) const;
    void takeAwayGraphicsLineItem(QGraphicsLineItem *item);
    void finalizeWall();
    void finalizeArea();
    void deleteArea(MapArea *area);
    void finalizeDoor();
    void deleteDoor(MapDoor *door);
    void finalizeNode();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MapFloor::SnapItems)

#endif // MAPFLOOR_H
