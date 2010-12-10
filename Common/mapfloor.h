#ifndef MAPFLOOR_H
#define MAPFLOOR_H

#include <QtCore/qmath.h>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QLineF>
#include <QMessageBox>
#include <QObject>
#include <QRectF>
#include <QStack>
#include "maparea.h"
#include "mapbase.h"
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
    enum Mode {Idle, Planning, WallAdd, AreaAdd, DoorAdd, Marking};
    enum Straight {None, SaveX, SaveY};

    explicit MapFloor(const QRectF &sceneRect, QString floorName = "",
                      QObject *parent = 0);
    QString name() const;
    void setName(const QString &floorName);
    void setMode(Mode m);
    void addBase(QPixmap &pixmap);
    MapArea* area(int i);

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
    const qreal cMagnetDestToLine;
    const qreal cMagnetDestToTop;

    Mode m_mode;
    QString m_name;
    QGraphicsRectItem *m_border;
    QVector<QLineF*> m_lines;
    MapArea *m_outline;
    MapArea *m_selectedArea;
//    QVector<MapArea*> areas;
    QVector<QGraphicsLineItem*> m_walls;
    QVector<QGraphicsLineItem*> m_doors;
    MapBase *m_base;
    bool m_isCrossLinesActive;

    QGraphicsLineItem *m_tempLine;
    QVector<QGraphicsLineItem*> m_tempPolyline;
    QGraphicsLineItem *m_crossLineHorizontal;
    QGraphicsLineItem *m_crossLineVertical;
    QGraphicsEllipseItem *m_cursorCircle;

    QSet<MapArea*> parentAreas(MapArea *area, const QPointF *pos);
    void areasToLineVec(MapArea *area);
    bool validatePos(QPointF pos, QPointF &rightPos);
    Straight straighten(QLineF &line);
    qreal dest(QPointF m, QPointF n);
    bool isInSegment(const QLineF &l, const QPointF &p);
    QPointF getPerpendicularBase(QPointF m, QLineF l);
    bool getPointFromLine(QPointF m, QPointF &newPoint,
                               const QLineF &l, Straight straight, qreal &min);
    bool getPointFromLineTops(QPointF m, QPointF &newPoint, const QLineF *line);
    QPointF getPoint(QPointF m, Straight straight, bool magnetToTops = true);

    void removeLastFromTempPolyline();

    QPolygonF convertLineVecToPolygon(const
                                      QVector<QGraphicsLineItem*> &vec) const;
    gpc_polygon convertQtToGpcPolygon(const QPolygonF *pgn);

    void finalizeWall();
    void finalizeArea();
    void deleteArea(MapArea *area);
    void finalizeDoor(QPointF pos/*MapArea *area, QLineF line*/);

    void selectArea(MapArea *area);
    void selectClear();

};

#endif // MAPFLOOR_H
