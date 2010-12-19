#include "geometry.h"

Geometry::Geometry()
{

}

Geometry::Straight Geometry::straighten(QLineF &line)
{
    if (qAbs(line.dx()) >= qAbs(line.dy()))
    {
        line.setP2(QPointF(line.x2(), line.y1()));
        return SaveY;
    }
    else
    {
        line.setP2(QPointF(line.x1(), line.y2()));
        return SaveX;
    }
}

qreal Geometry::dest(QPointF m, QPointF n)
{
    // Compute destination between 2 points with standard formula
    return qSqrt(qPow(m.x() - n.x(), 2) + qPow(m.y() - n.y(), 2));
}

bool Geometry::inBoundingRectOfSegment(const QLineF &l, const QPointF &p)
{
    return ((p.x() >= qMin(l.x1(), l.x2())) & (p.x() <= qMax(l.x1(), l.x2()))&
            (p.y() >= qMin(l.y1(), l.y2())) & (p.y() <= qMax(l.y1(), l.y2())));
}

bool Geometry::contain(const QPointF &p, const QLineF &l)
{
//    if (l.x1() != l.x2())
//        return (l.dy()/l.dx() == (p.y() - l.y1())/(p.x() - l.x1())) &
//               (inBoundingRectOfSegment(l, p));
//    else
//        return ((p.x() == l.x1()) & (inBoundingRectOfSegment(l, p)));
    qreal x = p.x();  qreal y = p.y();
    qreal x1 = l.x1(); qreal y1 = l.y1();
    qreal x2 = l.x2(); qreal y2 = l.y2();
    // From determinant condition of three points on the one line
    return (x*(y1 - y2) - y*(x1 - x2) + (x1*y2 - x2*y1) <= cError) &
            (inBoundingRectOfSegment(l, p));
}

QPointF Geometry::getPerpendicularBase(QPointF m, QLineF l)
{
    qreal x, y;
    if (l.x1() != l.x2())   // If line isn't vertical
    {
        /* For more information about this computing see book:
           ISBN 0486411478, 2.2 and 2.3-9                      */
        qreal k = l.dy()/l.dx();
        qreal b = l.y1() - k*l.x1();
        x = (k*(m.y() - b) + m.x())/(k*k + 1);
        y = k*x + b;
    }
    else
    {
        x = l.x1();
        y = m.y();
    }
    return QPointF(x, y);

}

bool Geometry::getPointFromLine(QPointF m, QPointF &newPoint, const QLineF &l,
                                Straight straight, qreal &min)
{
    QPointF *p = new QPointF();
    qreal d;
    if (l.x1() == l.x2())
        if (straight != SaveX)
        {
            p->setX(l.x1());
            p->setY(m.y());
            d = dest(*p, m);
        }
        else
            d = INFINITY;
//    else if (l.y1() == l.y2())
//    {
//        if (straight != SaveY)
//        {
//            p->setX(m.x());
//            p->setY(l.y1());
//            d = dest(*p, m);
//        }
//        else
//            d = INFINITY;
//    }
    else
    {
        switch (straight)
        {
        case None:
            *p = getPerpendicularBase(m, l);
            d = dest(*p, m);
            break;
        case SaveX:
            p->setX(m.x());
            p->setY((l.dy()/l.dx())*(m.x() - l.x1()) + l.y1());
            d = qAbs(p->y() - m.y());
            break;
        case SaveY:
            p->setY(m.y());
            p->setX((l.dx()/l.dy())*(m.y() - l.y1()) + l.x1());
            d = qAbs(p->x() - m.x());
            break;
        }
    }

    if ((d < cMagnetDestToLine) &
        (d < min) & (inBoundingRectOfSegment(l, *p)))
    {
        min = d;
        newPoint = *p;
        return true;
    }
    else
        return false;
}

bool Geometry::getPointFromLineTops(QPointF m, QPointF &newPoint,
                                    const QLineF *line)
{
    qreal d;
    d = dest(line->p1(), m);
    if (d < cMagnetDestToTop)
    {
        newPoint = line->p1();
        return true;
    }
    d = dest(line->p2(), m);
    if (d < cMagnetDestToTop)
    {
        newPoint = line->p2();
        return true;
    }

    return false;
}
