#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <qmath.h>
#include <QLineF>
#include <QPointF>

class Geometry
{
public:
    Geometry();

    enum Straight {None, SaveX, SaveY};
    static const qreal SnapingDistToLine = 25.0;
    static const qreal SnapingDistToNode = 15.0;
    static const qreal SnapingDistToNodeLine = 10.0;
    static const qreal SnapingDistToTop = 10.0;
    static const qreal Error = 0.001;

    static Straight straighten(QLineF &line);
    static qreal dist(QPointF m, QPointF n);
//    static qreal dest(QPointF m, QLineF l);
    static bool inBoundingRectOfSegment(const QLineF &l, const QPointF &p);
    static bool contain(const QPointF &p, const QLineF &l);
    static QPointF getPerpendicularBase(QPointF m, QLineF l);
    static bool getPointFromLine(QPointF m, QPointF &newPoint, const QLineF &l,
                                 Straight straight, bool extension, qreal &min);
    static bool getPointFromLineTops(QPointF m, QPointF &newPoint,
                                     const QLineF *line);
};

#endif // GEOMETRY_H