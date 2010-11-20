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

    explicit MapFloor(const QRectF &sceneRect, QString floorName = "",
                      QObject *parent = 0);
    QString getName() const;
    void setName(const QString &floorName);
    void setMode(Mode m);

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

    QGraphicsLineItem *tempLine;
    QVector<QGraphicsLineItem*> tempPolyline;
    QGraphicsEllipseItem *tempCircle;

    qreal dest(QPointF m, QPointF n);
    /*void getSides(QPointF m, QLineF l, qreal &a, qreal &b, qreal &c);
    qreal perpendicularLength(qreal p, qreal a, qreal b, qreal c);
    qreal dest(QPointF m, QLineF l);*/
    QPointF getPerpendicularBase(QPointF m, QLineF l);
    bool getPointFromLine(QPointF m, QPointF &newPoint,
                               const QLineF &l, qreal &min);
    bool getPointFromAreasTops(QPointF m, QPointF &newPoint,
                               const MapArea &area);
    bool getPointFromAreasLines(QPointF m, QPointF &newPoint,
                                const MapArea &area, qreal &min);
    bool getPoint(QPointF m, QPointF &newPoint);
    //void getLine(QPointF m, QLineF &l, bool isControlPressed);

    bool validatePos(QPointF pos, QPointF &rightPos);
    void finalizeFloor();
    void finalizeWall();
    void finalizeArea();
    QPolygonF convertLineVecToPolygon(const
                                      QVector<QGraphicsLineItem*> &vec) const;
};

#endif // MAPFLOOR_H
