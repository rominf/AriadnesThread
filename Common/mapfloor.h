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
#include "maparea.h"
#include "mapbase.h"
#include "mapselection.h"

extern "C"
{
    #include "gpc.h"
}


class MapFloor: public QGraphicsScene
{
    Q_OBJECT

    friend QDataStream &operator<<(QDataStream &output, const MapFloor &floor);
    friend QDataStream &operator>>(QDataStream &input, MapFloor &floor);
public:    
    explicit MapFloor(const QRectF &sceneRect, QObject *parent = 0);

    static const qreal OutlineZValue;
    enum Mode {Idle, Planning, WallAdd, AreaAdd, DoorAdd, Marking, Selection};

    QString name() const;
    void setName(const QString &floorName);
    MapFloor::Mode mode();
    void setMode(Mode m);
    void addBase(QPixmap &pixmap);
    MapArea* area(int i = 0);
    QAbstractGraphicsShapeItem* selectedItem();

signals:
    void modeChanged(MapFloor::Mode);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void addItem (QGraphicsItem * item);
//    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change,
//                                 const QVariant &value);

private:
    const qreal cCursorCircleR;

    Mode m_mode;
    QString m_name;
    QGraphicsRectItem *m_border;
    QVector<QLineF*> m_lines;
    MapArea *m_outline;
//    QVector<MapArea*> areas;
    QVector<QGraphicsLineItem*> m_walls;
    QVector<QGraphicsLineItem*> m_doors;
    MapBase *m_base;
    bool m_isCrossLinesActive;
    MapSelection *m_selection;

    QGraphicsLineItem *m_tempLine;
    QVector<QGraphicsLineItem*> m_tempPolyline;
    QGraphicsLineItem *m_crossLineHorizontal;
    QGraphicsLineItem *m_crossLineVertical;
    QGraphicsEllipseItem *m_cursorCircle;

    QSet<MapArea*> parentAreas(MapArea *area, const QPointF *pos);
    void areasToLineVec(MapArea *area);
    QVector<MapArea*> pointContainers(QPointF pos);
    bool validatePos(QPointF pos, QPointF &rightPos);
    QPointF getPoint(QPointF m, Geometry::Straight straight,
                     bool magnetToTops = true);

    void removeLastFromTempPolyline();

    QPolygonF convertLineVecToPolygon(const
                                      QVector<QGraphicsLineItem*> &vec) const;
    gpc_polygon convertQtToGpcPolygon(const QPolygonF *pgn);

    void finalizeWall();
    void finalizeArea();
    void deleteArea(MapArea *area);
    void finalizeDoor();
    void deleteDoor(MapDoor *door);
};

#endif // MAPFLOOR_H
