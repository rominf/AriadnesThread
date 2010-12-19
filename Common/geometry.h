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

    static Straight straighten(QLineF &line);
    static qreal dest(QPointF m, QPointF n);
    static bool inBoundingRectOfSegment(const QLineF &l, const QPointF &p);
    static bool contain(const QPointF &p, const QLineF &l);
    static QPointF getPerpendicularBase(QPointF m, QLineF l);
    static bool getPointFromLine(QPointF m, QPointF &newPoint, const QLineF &l,
                                 Straight straight, qreal &min);
    static bool getPointFromLineTops(QPointF m, QPointF &newPoint,
                                     const QLineF *line);
private:
    static const qreal cMagnetDestToLine = 50.0;
    static const qreal cMagnetDestToTop = 20.0;
    static const qreal cError = 0.1;

};

#endif // GEOMETRY_H
