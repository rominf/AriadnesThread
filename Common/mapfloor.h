#ifndef MAPFLOOR_H
#define MAPFLOOR_H

#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QLineF>
#include <QObject>
#include <QRectF>
#include "maparea.h"

#include <QtCore>
#include <QtGui>

class MapFloor: public QGraphicsScene
{
    Q_OBJECT

    friend QDataStream &operator<<(QDataStream &output, const MapFloor &floor);
    friend QDataStream &operator>>(QDataStream &input, MapFloor &floor);
public:
    enum Mode {Idle, Planning, FloorAdd, WallAdd, AreaAdd, DoorAdd, Marking};
    enum Straight {None, SaveX, SaveY};

    explicit MapFloor(const QRectF &sceneRect, QString floorName = "",
                      QObject *parent = 0);
    QString getName() const;
    void setName(const QString &floorName);
    void setMode(Mode m);
    void addBase(QPixmap &pixmap);

signals:
    void modeChanged(MapFloor::Mode);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private:
    Mode mode;

    QString name;
    QGraphicsRectItem *border;
    MapArea *outline;
    QVector<MapArea*> areas;
    QVector<QGraphicsLineItem*> walls;
    QGraphicsPixmapItem *base;

    QGraphicsLineItem *tempLine;
    QVector<QGraphicsLineItem*> tempPolyline;
    QGraphicsEllipseItem *tempCircle;

    bool validatePos(QPointF pos, QPointF &rightPos);
    Straight straighten(QLineF &line);
    qreal dest(QPointF m, QPointF n);
    QPointF getPerpendicularBase(QPointF m, QLineF l);
    bool getPointFromLine(QPointF m, QPointF &newPoint,
                               const QLineF &l, Straight straight, qreal &min);
    bool getPointFromAreasTops(QPointF m, QPointF &newPoint,
                               const MapArea &area);
    bool getPointFromAreasLines(QPointF m, QPointF &newPoint,
                                const MapArea &area, Straight straight,
                                qreal &min);
    QPointF getPoint(QPointF m, Straight straight);
//    void getLine(QPointF m, QLineF &l, bool isControlPressed);
//    void getSides(QPointF m, QLineF l, qreal &a, qreal &b, qreal &c);
//    qreal perpendicularLength(qreal p, qreal a, qreal b, qreal c);
//    qreal dest(QPointF m, QLineF l);


    void finalizeFloor();
    void finalizeWall();
    void finalizeArea();
    QPolygonF convertLineVecToPolygon(const
                                      QVector<QGraphicsLineItem*> &vec) const;
};

#endif // MAPFLOOR_H
