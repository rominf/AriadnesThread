#include "mapfloor.h"

MapFloor::MapFloor(const QRectF &sceneRect, QString floorName,QObject *parent) :
    QGraphicsScene(sceneRect, parent)
{
    setName(floorName);
    mode = Planning;\
    border = new QGraphicsRectItem(sceneRect, 0, this);
    border->setBrush(QBrush(Qt::white));
    border->setZValue(-2.0);
    outline = 0;
    base = 0;
    tempLine = 0;
    cursorCircle = new QGraphicsEllipseItem(
            -cCursorCircleR/2, -cCursorCircleR/2,
            cCursorCircleR, cCursorCircleR, 0, this);
    cursorCircle->setPen(QPen(Qt::NoPen));
    cursorCircle->setBrush(QBrush(Qt::red));
    cursorCircle->hide();
    crossLineHorizontal = new QGraphicsLineItem(0, this);
    crossLineHorizontal->setPen(QPen(Qt::DotLine));
    crossLineHorizontal->hide();
    crossLineVertical = new QGraphicsLineItem(0, this);
    crossLineVertical->setPen(QPen(Qt::DotLine));
    crossLineVertical->hide();
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
        switch (mode)
        {
        case WallAdd:
            if (!(tempLine))
            {
                tempLine = new QGraphicsLineItem(
                        QLineF(cursorCircle->pos(),
                               cursorCircle->pos()));
                addItem(tempLine);
            }
            break;
        case FloorAdd:
        case AreaAdd:
        {
            QPointF p1;
            if (tempPolyline.isEmpty())
            {
                p1 = cursorCircle->pos();
                crossLineHorizontal->setLine(
                        0, p1.y(), sceneRect().right(), p1.y());
                crossLineVertical->setLine(
                        p1.x(), 0, p1.x(), sceneRect().bottom());
            }
            else
            {
                p1 = tempPolyline.at(
                        tempPolyline.size()-1)->line().p2();

                if ((cursorCircle->pos().x() ==
                    crossLineVertical->line().x1()) |
                    (cursorCircle->pos().y() ==
                    crossLineHorizontal->line().y1()))
                    isCrossLinesActive = false;
                else
                    isCrossLinesActive = true;
                crossLineHorizontal->hide();
                crossLineVertical->hide();
            }
            tempPolyline.push_back(new QGraphicsLineItem(
                    QLineF(p1, cursorCircle->pos())));
            addItem(tempPolyline.back());
            break;
        }
        default:
            break; // Ignore other :)
        }
        break;
    default:
        break;
    }
}

void MapFloor::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    /*if (event->scenePos().x() < 0)
        event->setScenePos(QPoint(0, event->scenePos().y()));*/
    switch (event->buttons())
    {
    case Qt::NoButton:
        QPointF pos = QPointF(0, 0);
        if (!validatePos(event->scenePos(), pos))
            event->setScenePos(pos);
        bool isCtrl = (event->modifiers() == Qt::ControlModifier);
        Straight straight;
        switch (mode)
        {
        case WallAdd:
            if (tempLine)
            {
                QLineF line(tempLine->line().p1(), pos);
                if (!isCtrl)
                    straight = straighten(line);
                else
                    straight = None;
                pos = getPoint(line.p2(), straight);
                line.setP2(pos);
                tempLine->setLine(line);
            }
            else
            {
                pos = getPoint(pos, None);
            }
            cursorCircle->show();
            cursorCircle->setPos(pos);
            break;
        case FloorAdd:
        case AreaAdd:
            if (!(tempPolyline.isEmpty()))
            {
                QLineF line(tempPolyline.back()->line().p1(), pos);
                if (!isCtrl)
                    straight = straighten(line);
                else
                    straight = None;
                pos = getPoint(line.p2(), straight);
                line.setP2(pos);
                tempPolyline.at(tempPolyline.size()-1)->setLine(line);
            }
            else
            {
                pos = getPoint(pos, None);
            }
            cursorCircle->show();
            cursorCircle->setPos(pos);
            break;
        case DoorAdd:
            break;
        default:
            break; // Ignore other :)
        }
        break;
    }
}

void MapFloor::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    switch (mode)
    {
    case FloorAdd:
        removeLastFromTempPolyline();
        finalizeFloor();
        break;
    case AreaAdd:
        removeLastFromTempPolyline();
        finalizeArea();
        break;
    case WallAdd:
        finalizeWall();
        break;
    default:
        break; // Ignore other :)
    }
}

void MapFloor::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Delete:
        switch (mode)
        {
        case FloorAdd:
        case AreaAdd:
            removeLastFromTempPolyline();
            break;
        default:
            break; // Ignore other :)
        }
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        switch (mode)
        {
        case FloorAdd: {finalizeFloor(); break;}
        case AreaAdd: {finalizeArea(); break;}
        case WallAdd: {finalizeWall(); break;}
        default:{break;} // Ignore other :)
        }
        break;
    default:
        break; // Ignore other :)
    }
}

void MapFloor::addBase(QPixmap &pixmap)
{
    if (!base)
    {
        base = new QGraphicsPixmapItem(pixmap);
        base->setZValue(-1.0);
        base->setFlags(QGraphicsItem::ItemIsMovable);
        addItem(base);
    }
    else
        base->setPixmap(pixmap);
}

MapFloor::Straight MapFloor::straighten(QLineF &line)
{
    if (abs(line.dx()) >= abs(line.dy()))
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

qreal MapFloor::dest(QPointF m, QPointF n)
{
    return qSqrt(qPow(m.x() - n.x(), 2) + qPow(m.y() - n.y(), 2));
}

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
    if ((x < qMin(l.x1(), l.x2()) - cMagnetDestToTop) |
        (x > qMax(l.x1(), l.x2()) + cMagnetDestToTop) |
        (y < qMin(l.y1(), l.y2()) - cMagnetDestToTop) |
        (y > qMax(l.y1(), l.y2()) + cMagnetDestToTop))
        x = -INFINITY;
    return QPointF(x, y);

}

bool MapFloor::getPointFromLine(QPointF m, QPointF &newPoint,
                                const QLineF &l, Straight straight, qreal &min)
{
    QPointF *p = new QPointF();
    *p = getPerpendicularBase(m, l);
    qreal d = dest(*p, m);
    if ((d < cMagnetDestToLine) & (d < min))
    {
        min = d;
        switch (straight)
        {
        case None:
            newPoint = *p;
            break;
        case SaveX:
            newPoint.setX(m.x());
            if (l.dx() == 0)                    // If line is near and parallel
                newPoint.setY(p->y());
            else
                newPoint.setY((l.dy()/l.dx())*(m.x() - l.x1()) + l.y1());
            break;
        case SaveY:
            newPoint.setY(m.y());
            if (l.dy() == 0)                    // If line is near and parallel
                newPoint.setX(p->x());
            else
                newPoint.setX((l.dx()/l.dy())*(m.y() - l.y1()) + l.x1());
            break;
        }
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
        if (d < cMagnetDestToTop)
        {
            newPoint = area.polygon().at(i);
            return true;
        }
    }
    return false;
}

bool MapFloor::getPointFromAreasLines(QPointF m, QPointF &newPoint,
                      const MapArea &area, Straight straight, qreal &min)
{
    int size = area.polygon().size();
    QLineF *l = new QLineF();
    for (int i = 0; i != size; ++i)
    {
        l->setPoints(area.polygon().at(i),
                     area.polygon().at((i+1)%size));
        getPointFromLine(m, newPoint, *l, straight, min);
    }
    return true;
}

QPointF MapFloor::getPoint(QPointF m, Straight straight)
{
    qreal min = INFINITY;   // There must be max real
    QPointF newPoint;

    if (straight == None)
    {
        if (outline)
            if (getPointFromAreasTops(m, newPoint, *outline))
                return newPoint;

        for (int i = 0; i != areas.size(); ++i)
        {
            if (getPointFromAreasTops(m, newPoint, *areas.at(i)))
                return newPoint;
        }

    }

    if (isCrossLinesActive & (!tempPolyline.isEmpty()))
    {
        if (getPointFromLine(
            m, newPoint, crossLineHorizontal->line(), straight, min))
        {
            crossLineHorizontal->setLine(
                    QLineF(tempPolyline.at(0)->line().p1(), newPoint));
            crossLineHorizontal->show();
            return newPoint;
        }
        else
            {
                crossLineHorizontal->setLine(
                        0, tempPolyline.at(0)->line().y1(),
                        sceneRect().right(), tempPolyline.at(0)->line().y1());
                crossLineHorizontal->hide();
            }
        if (getPointFromLine(
            m, newPoint, crossLineVertical->line(), straight, min))
        {
            crossLineVertical->setLine(
                    QLineF(tempPolyline.at(0)->line().p1(), newPoint));
            crossLineVertical->show();
            return newPoint;
        }
        else
            {
                crossLineVertical->setLine(
                        tempPolyline.at(0)->line().x1(), 0,
                        tempPolyline.at(0)->line().x1(), sceneRect().bottom());
                crossLineVertical->hide();
            }
    }


    if (outline)
        getPointFromAreasLines(m, newPoint, *outline, straight, min);

    for (int i = 0; i != areas.size(); ++i)
        getPointFromAreasLines(m, newPoint, *areas.at(i), straight, min);

    for (int i = 0; i != walls.size(); ++i)
        getPointFromLine(m, newPoint, walls.at(i)->line(), straight, min);

    if (min == INFINITY)
        return m;
    else
        return newPoint;

}

bool MapFloor::validatePos(QPointF pos, QPointF &rightPos)
{
    rightPos.setX(qMin(qMax(0.0, pos.x()), sceneRect().right()));
    rightPos.setY(qMin(qMax(0.0, pos.y()), sceneRect().bottom()));
    return pos == rightPos;
}

void MapFloor::removeLastFromTempPolyline()
{
    if (!(tempPolyline.isEmpty()))
    {
        removeItem(tempPolyline.at(tempPolyline.size()-1));
        tempPolyline.pop_back();
    }
}

void MapFloor::finalizeFloor()
{
    setMode(Planning);
    outline = new MapArea(convertLineVecToPolygon(tempPolyline));
    addItem(outline);
    for (int i = 0; i != tempPolyline.size(); ++i)
        removeItem(tempPolyline.at(i));
    tempPolyline.clear();
    crossLineHorizontal->hide();
    crossLineVertical->hide();
    crossLineHorizontal->setLine(-5, -5, -5, -5);   // Else there are false
    crossLineVertical->setLine(-5, -5, -5, -5);     // misoperation
    cursorCircle->hide();
}

void MapFloor::finalizeWall()
{
    setMode(Planning);
    walls.append(tempLine);
    tempLine = 0;
    cursorCircle->hide();
}

void MapFloor::finalizeArea()
{
    setMode(Planning);
    MapArea *area = new MapArea(convertLineVecToPolygon(tempPolyline));
    areas.append(area);
    addItem(area);
    for (int i = 0; i != tempPolyline.size(); ++i)
        removeItem(tempPolyline.at(i));
    tempPolyline.clear();
    crossLineHorizontal->hide();
    crossLineVertical->hide();
    crossLineHorizontal->setLine(-5, -5, -5, -5);   // Else there are false
    crossLineVertical->setLine(-5, -5, -5, -5);     // misoperation
    cursorCircle->hide();
}

QPolygonF MapFloor::convertLineVecToPolygon(const
                                  QVector<QGraphicsLineItem*> &vec) const
{
    QPolygonF polygon;
    for (int i = 0; i != vec.size(); ++i)
        polygon << vec.at(i)->line().p1();
    return polygon;
}

