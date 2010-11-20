#include "mapfloor.h"

MapFloor::MapFloor(const QRectF &sceneRect, QString floorName,QObject *parent) :
    QGraphicsScene(sceneRect, parent)
{
    setName(floorName);
    mode = Planning;
    /*QPen *pen = new QPen(Qt::black);
    QBrush *brush = new QBrush(Qt::white);*/
    border = new QGraphicsRectItem(sceneRect, 0, this);
    border->setBrush(QBrush(Qt::white));
    outline = 0;
    tempLine = 0;
    tempCircle = new QGraphicsEllipseItem(0, 0, 0, 0);
    addItem(tempCircle);
}

QDataStream &operator<<(QDataStream &output, const MapFloor &floor)
{
    output << floor.name << *floor.outline;

    int last = floor.walls.size();
    output << last;
    for (int i = 0; i != last; ++i)
    {
        QPointF p1 = floor.walls.at(i)->line().p1();
        QPointF p2 = floor.walls.at(i)->line().p2();
        output << p1.x() << p1.y() << p2.x() << p2.y();
    }

    last = floor.areas.size();
    output << last;
    for (int i = 0; i != last; ++i)
    {
        output << *floor.areas.at(i);
    }

    return output;
}

QDataStream &operator>>(QDataStream &input, MapFloor &floor)
{
    floor.outline = new MapArea(0);
    input >> floor.name >> *floor.outline;
    floor.addItem(floor.outline);

    int last;
    input >> last;
    for (int i = 0; i != last; ++i)
    {
        QGraphicsLineItem *line =  new QGraphicsLineItem();
        qreal x1, y1, x2, y2;
        input >> x1 >> y1 >> x2 >> y2;
        line->setLine(x1, y1, x2, y2);
        floor.walls.append(line);
        floor.addItem(floor.walls.at(i));
    }

    input >> last;
    for (int i = 0; i != last; ++i)
    {
        MapArea *area =  new MapArea(0);
        input >> *area;
        floor.areas.append(area);
        floor.addItem(floor.areas.at(i));
    }

    return input;
}

QString MapFloor::getName() const
{
    return name;
}

void MapFloor::setName(const QString &floorName)
{
    name = floorName;
}

void MapFloor::setMode(Mode m)
{
    if (m != mode)
    {
        mode = m;
        emit modeChanged(m);
    }
}

void MapFloor::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    switch (event->buttons())
    {
    case Qt::LeftButton:
        {
            switch (mode)
            {
            case WallAdd:
                {
                    if (!(tempLine))
                    {
                        tempLine = new QGraphicsLineItem(
                                QLineF(event->scenePos(), event->scenePos()));
                        addItem(tempLine);
                    }
                    break;
                }
            case FloorAdd:
            case AreaAdd:
                {
                    QPointF p1;
                    if (tempPolyline.isEmpty())
                    {
                        p1 = event->scenePos();
                    }
                    else
                    {
                        p1 = tempPolyline.at(
                                tempPolyline.size()-1)->line().p2();
                    }
                    tempPolyline.push_back(new QGraphicsLineItem(
                            QLineF(p1, event->scenePos())));
                    addItem(tempPolyline.back());
                    break;
                }
            default:{break;} // Ignore other :)
            }
            break;
        }
    }
}

void MapFloor::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    /*switch (event->buttons())
    {
    case Qt::NoButton:
        {*/
    /*if (event->scenePos().x() < 0)
        event->setScenePos(QPoint(0, event->scenePos().y()));*/
    QPointF pos = QPointF(0, 0);
    if (!validatePos(event->scenePos(), pos))
        event->setScenePos(pos);
    pos.setX(0);
    if (!getPoint(event->scenePos(), pos))
    {
        pos = event->scenePos();
    }
            switch (mode)
            {
            case WallAdd:
                {
                    if (tempLine)
                    {
                        QLineF line(tempLine->line().p1(), pos);
                        if (event->modifiers() != Qt::ControlModifier)
                        {
                            if (abs(line.dx()) >= abs(line.dy()))
                            {
                                line.setP2(QPointF(line.x2(), line.y1()));
                            }
                            else
                            {
                                line.setP2(QPointF(line.x1(), line.y2()));
                            }
                        }
                        tempLine->setLine(line);
                    }
                    /*else
                    {
                        QPointF p;
                        if (getPoint(event->scenePos(), p))
                            event->setScenePos(p);
                    }*/
                    break;
                }
            case FloorAdd:
            case AreaAdd:
                {
                    if (!(tempPolyline.isEmpty()))
                    {
                        QLineF line(tempPolyline.back()->line().p1(), pos);
                        if (event->modifiers() != Qt::ControlModifier)
                        {
                            // Straightening line, if Shift is pressed
                            if (abs(line.dx()) >= abs(line.dy()))
                            {
                                line.setP2(QPointF(line.x2(), line.y1()));
                            }
                            else
                            {
                                line.setP2(QPointF(line.x1(), line.y2()));
                            }
                        }
                        tempPolyline.at(tempPolyline.size()-1)->setLine(line);
                    }
                    break;
                }
            case DoorAdd:
                {
                    int x = event->scenePos().toPoint().x();
                    int y = event->scenePos().toPoint().y();
                    bool ex = false;
                    for (int i = 1; (i != 10)&(!ex); ++i)
                    {
                        for (int j = x-i; (j != x+i)&(!ex); ++j)
                        {
                            if (itemAt(j, y-i) != 0)
                            {
                                tempCircle->setRect(j, y-i, 5, 5);
                                ex = true;
                                tempCircle->show();
                            }
                            if (itemAt(j, y+i) != 0)
                            {
                                tempCircle->setRect(j, y+i, 5, 5);
                                ex = true;
                                tempCircle->show();
                            }
                        }
                        for (int j = y-i; (j != y+i)&(!ex); ++j)
                        {
                            if (itemAt(x-i, j) != 0)
                            {
                                tempCircle->setRect(x-i, j, 5, 5);
                                ex = true;
                                tempCircle->show();
                            }
                            if (itemAt(x+i, j) != 0)
                            {
                                tempCircle->setRect(x+i, j, 5, 5);
                                ex = true;
                                tempCircle->show();
                            }
                        }

                    }
                    if (!ex & tempCircle->isVisible())
                    {
                        tempCircle->hide();
                    }
                }
            default:{break;} // Ignore other :)
            }

      /*      break;
       }
    }*/
}

void MapFloor::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    switch (mode)
    {
    case FloorAdd: {finalizeFloor(); break;}
    case AreaAdd: {finalizeArea(); break;}
    case WallAdd: {finalizeWall(); break;}
    default:{break;} // Ignore other :)
    }
}

void MapFloor::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Delete:
    {
        switch (mode)
        {
        case FloorAdd:
        case AreaAdd:
            {
                if (!(tempPolyline.isEmpty()))
                {
                    removeItem(tempPolyline.at(tempPolyline.size()-1));
                    tempPolyline.pop_back();
                }
                break;
            }
        default:{break;} // Ignore other :)
        }
        break;
    }
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        switch (mode)
        {
        case FloorAdd: {finalizeFloor(); break;}
        case AreaAdd: {finalizeArea(); break;}
        case WallAdd: {finalizeWall(); break;}
        default:{break;} // Ignore other :)
        }
        break;
    }
    default:{break;} // Ignore other :)
    }
}

qreal MapFloor::dest(QPointF m, QPointF n)
{
    return qSqrt(qPow(m.x() - n.x(), 2) + qPow(m.y() - n.y(), 2));
}

/*void getSides(QPointF m, QLineF l, qreal &a, qreal &b, qreal &c)
{
    a = dest(m, l.p1());
    b = dest(m, l.p2());
    c = l.length();
}

qreal perpendicularLength(qreal p, qreal a, qreal b, qreal c)
{
    return (2*qSqrt(p*(p-a)*(p-b)*(p-c)))/c;
}

qreal dest(QPointF m, QLineF l)
{
    qreal a, b, c;
    getSides(m, l, a, b, c);
    qreal p = (a + b + c)/2;
    qreal t = perpendicularLength(p, a, b, c);
    return qMin(qMin(a, b), t);
}*/

QPointF MapFloor::getPerpendicularBase(QPointF m, QLineF l)
{
    qreal x, y;
    if (l.x1() != l.x2())
    {
        qreal k = (l.y2() - l.y1())/(l.x2() - l.x1());
        qreal b = l.y1() - k*l.x1();
        x = (k*(m.y() - b) + m.x())/(k*k + 1);
        y = k*x + b;
    }
    else
    {
        x = l.x1();
        y = m.y();
    }
    if ((x < qMin(l.x1(), l.x2())) | (x > qMax(l.x1(), l.x2())) |
        (y < qMin(l.y1(), l.y2())) | (y > qMax(l.y1(), l.y2())))
        x = -100500.0;
    return QPointF(x, y);

}

bool MapFloor::getPointFromLine(QPointF m, QPointF &newPoint,
                                const QLineF &l, qreal &min)
{
    QPointF *p = new QPointF();
    *p = getPerpendicularBase(m, l);
    qreal d = dest(*p, m);
    if ((d < 50) & (d < min))
    {
        min = d;
        newPoint = *p;
        return true;
    }
    else
        return false;

}

bool MapFloor::getPointFromAreasTops(QPointF m, QPointF &newPoint,
                                     const MapArea &area)
{
    int size = area.polygon().size();
    qreal d;
    for (int i = 0; i != size; ++i)
    {
        d = dest(area.polygon().at(i), m);
        if (d < 20)
        {
            newPoint = area.polygon().at(i);
            return true;
        }
    }
    return false;
}

bool MapFloor::getPointFromAreasLines(QPointF m, QPointF &newPoint,
                      const MapArea &area, qreal &min)
{
    int size = area.polygon().size();
    qreal d;
    QLineF *l = new QLineF();
    QPointF *p = new QPointF();
    for (int i = 0; i != size; ++i)
    {
        l->setPoints(area.polygon().at(i),
                     area.polygon().at((i+1)%size));

        /**p = getPerpendicularBase(m, *l);
        d = dest(*p, m);
        if ((d < 50) & (d < min))
        {
            min = d;
            newPoint = *p;
        }*/
        getPointFromLine(m, newPoint, *l, min);
    }
    return true;
}

bool MapFloor::getPoint(QPointF m, QPointF &newPoint)
{
    qreal min = 100500.0;   // There must be max real

    /*for (int i = 0; i != areas.size(); ++i)
        {
            size = areas.at(i)->polygon().size();
            for (int j = 0; j != size; ++j)
            {
                d = dest(areas.at(i)->polygon().at(j), m);
                if (d < 20)
                {
                    newPoint = areas.at(i)->polygon().at(j);
                    return true;
                }
            }
        }*/
    if (outline)
        if (getPointFromAreasTops(m, newPoint, *outline))
            return true;

    for (int i = 0; i != areas.size(); ++i)
    {
        if (getPointFromAreasTops(m, newPoint, *areas.at(i)))
            return true;
    }

    if (outline)
        getPointFromAreasLines(m, newPoint, *outline, min);

    for (int i = 0; i != areas.size(); ++i)
    {
        getPointFromAreasLines(m, newPoint, *areas.at(i), min);
    }

    for (int i = 0; i != walls.size(); ++i)
    {
        getPointFromLine(m, newPoint, walls.at(i)->line(), min);
    }

    if (min == 100500.0)
        return false;
    else
        return true;
}

bool MapFloor::validatePos(QPointF pos, QPointF &rightPos)
{
    //rightPos = pos;
    rightPos.setX(qMin(qMax(0.0, pos.x()), sceneRect().right()));
    rightPos.setY(qMin(qMax(0.0, pos.y()), sceneRect().bottom()));
    /*rightPos.setX(pos.x() <= 0? 0:pos.x());
    rightPos.setX(pos.x() > sceneRect().right()? sceneRect().right():pos.x());
    rightPos.setY(pos.y() < 0? 0:pos.y());
    rightPos.setY(pos.y() > sceneRect().bottom()? sceneRect().bottom():pos.y());*/
    return pos == rightPos;
}

void MapFloor::finalizeFloor()
{
    setMode(Planning);
    outline = new MapArea(convertLineVecToPolygon(tempPolyline));
    addItem(outline);
    for (int i = 0; i != tempPolyline.size(); ++i)
    {
        removeItem(tempPolyline.at(i));
    }
    tempPolyline.clear();
}

void MapFloor::finalizeWall()
{
    setMode(Planning);
    walls.append(tempLine);
    tempLine = 0;
}

void MapFloor::finalizeArea()
{
    setMode(Planning);
    MapArea *area = new MapArea(convertLineVecToPolygon(tempPolyline));
    areas.append(area);
    addItem(area);
    for (int i = 0; i != tempPolyline.size(); ++i)
    {
        removeItem(tempPolyline.at(i));
    }
    tempPolyline.clear();
}

QPolygonF MapFloor::convertLineVecToPolygon(const
                                  QVector<QGraphicsLineItem*> &vec) const
{
    QPolygonF polygon;
    for (int i = 0; i != vec.size(); ++i)
    {
        polygon << vec.at(i)->line().p1();
    }
    return polygon;
}

