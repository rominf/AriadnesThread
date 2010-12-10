#include "mapfloor.h"

MapFloor::MapFloor(const QRectF &sceneRect, QString floorName,QObject *parent) :
    QGraphicsScene(sceneRect, parent),
    cCursorCircleR(15.0), cMagnetDestToLine(50.0), cMagnetDestToTop(20.0)
{
    setName(floorName);
    m_mode = Idle;
    m_border = new QGraphicsRectItem(sceneRect, 0, this);
    m_border->setBrush(QBrush(Qt::white));
    m_border->setZValue(-2.0);
    m_outline = 0;
    m_selectedArea = 0;
    m_base = 0;
    m_tempLine = 0;
    m_cursorCircle = new QGraphicsEllipseItem(
            -cCursorCircleR/2, -cCursorCircleR/2,
            cCursorCircleR, cCursorCircleR, 0, this);
    m_cursorCircle->setZValue(INFINITY - 1);
    m_cursorCircle->setPen(QPen(Qt::NoPen));
    m_cursorCircle->setBrush(QBrush(Qt::red));
    m_cursorCircle->hide();
    m_crossLineHorizontal = new QGraphicsLineItem(0, this);
    m_crossLineHorizontal->setZValue(INFINITY - 2);
    m_crossLineHorizontal->setPen(QPen(Qt::DotLine));
    m_crossLineHorizontal->hide();
    m_crossLineVertical = new QGraphicsLineItem(0, this);
    m_crossLineVertical->setZValue(INFINITY - 2);
    m_crossLineVertical->setPen(QPen(Qt::DotLine));
    m_crossLineVertical->hide();
}

QDataStream &operator<<(QDataStream &output, const MapFloor &floor)
{
    int last = floor.m_walls.size();
    output << floor.m_name << *floor.m_outline << last;
    for (int i = 0; i != last; i++)
    {
        QPointF p1 = floor.m_walls.at(i)->line().p1();
        QPointF p2 = floor.m_walls.at(i)->line().p2();
        output << p1.x() << p1.y() << p2.x() << p2.y();
    }
    return output;
}

QDataStream &operator>>(QDataStream &input, MapFloor &floor)
{
    floor.m_outline = new MapArea(0);
    floor.m_outline->setZValue(-1.0);
    input >> floor.m_name >> *floor.m_outline;
    floor.addItem(floor.m_outline);

    int last;
    input >> last;
    for (int i = 0; i != last; i++)
    {
        QGraphicsLineItem *line =  new QGraphicsLineItem();
        qreal x1, y1, x2, y2;
        input >> x1 >> y1 >> x2 >> y2;
        line->setLine(x1, y1, x2, y2);
        floor.m_walls.append(line);
        floor.addItem(floor.m_walls.at(i));
    }

    return input;
}

QString MapFloor::name() const
{
    return m_name;
}

void MapFloor::setName(const QString &floorName)
{
    m_name = floorName;
}

void MapFloor::setMode(Mode m)
{
    if (m != m_mode)
    {
        if (m != Planning)
            selectClear();
        if (m_outline != 0)
            if ((m == AreaAdd) | (m == WallAdd))
                areasToLineVec(m_outline);

        m_mode = m;
        emit modeChanged(m);
    }
}

void MapFloor::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    switch (event->buttons())
    {
    case Qt::LeftButton:
        switch (m_mode)
        {
        case WallAdd:
            if (!(m_tempLine))
            {
                m_tempLine = new QGraphicsLineItem(
                        QLineF(m_cursorCircle->pos(),
                               m_cursorCircle->pos()));
                m_tempLine->setZValue(INFINITY - 2);
                addItem(m_tempLine);
            }
            break;
        case AreaAdd:
        {
            QPointF p1;
            if (m_tempPolyline.isEmpty())
            {
                p1 = m_cursorCircle->pos();
                m_crossLineHorizontal->setLine(
                        0, p1.y(), sceneRect().right(), p1.y());
                m_crossLineVertical->setLine(
                        p1.x(), 0, p1.x(), sceneRect().bottom());
            }
            else
            {
                p1 = m_tempPolyline.at(
                        m_tempPolyline.size()-1)->line().p2();

                if ((m_cursorCircle->pos().x() ==
                    m_crossLineVertical->line().x1()) |
                    (m_cursorCircle->pos().y() ==
                    m_crossLineHorizontal->line().y1()))
                    m_isCrossLinesActive = false;
                else
                    m_isCrossLinesActive = true;
                m_crossLineHorizontal->hide();
                m_crossLineVertical->hide();
            }
            m_tempPolyline.push_back(new QGraphicsLineItem(
                    QLineF(p1, m_cursorCircle->pos())));
            m_tempPolyline.back()->setZValue(INFINITY - 2);
            addItem(m_tempPolyline.back());
            break;
        }
        case DoorAdd:
            if (getPoint(event->scenePos(), None, false) ==
                m_cursorCircle->pos())
            {
                finalizeDoor(m_cursorCircle->pos());
//                cursorCircle->hide();
//                outline->hide();
//                if (itemAt(cursorCircle->pos())->type() == MapArea::Type)
//                {
//                    MapArea *area = qgraphicsitem_cast<MapArea *>(
//                            itemAt(cursorCircle->pos()));
//                    finalizeDoor(area,
//                                 QLineF(cursorCircle->pos().x(),
//                                        cursorCircle->pos().y() - 20,
//                                        cursorCircle->pos().x(),
//                                        cursorCircle->pos().y() + 20));
//                }
            }
        default:
            if (itemAt(event->scenePos())->type() == MapArea::Type)
                selectArea(qgraphicsitem_cast<MapArea*>(
                        itemAt(event->scenePos())));

            break; // Ignore other :)
        }
        break;
    default:
        break;
    }
    QGraphicsScene::mousePressEvent(event);
}

void MapFloor::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    switch (event->buttons())
    {
    case Qt::NoButton:
        QPointF pos = QPointF(0, 0);
        if (!validatePos(event->scenePos(), pos))
            event->setScenePos(pos);
        Straight straight;
        switch (m_mode)
        {
        case WallAdd:
            if (m_tempLine)
            {
                QLineF line(m_tempLine->line().p1(), pos);
                if (!(event->modifiers() & Qt::ControlModifier))
                    straight = straighten(line);
                else
                    straight = None;
                if (!(event->modifiers() & Qt::ShiftModifier))
                    pos = getPoint(line.p2(), straight);
                else
                    pos = line.p2();
                pos = getPoint(line.p2(), straight);
                line.setP2(pos);
                m_tempLine->setLine(line);
            }
            else
            {
                pos = getPoint(pos, None);
            }
            m_cursorCircle->show();
            m_cursorCircle->setPos(pos);
            break;
        case AreaAdd:
            if (!(m_tempPolyline.isEmpty()))
            {
                QLineF line(m_tempPolyline.back()->line().p1(), pos);
                if (!(event->modifiers() & Qt::ControlModifier))
                    straight = straighten(line);
                else
                    straight = None;
                if (!(event->modifiers() & Qt::ShiftModifier))
                    pos = getPoint(line.p2(), straight);
                else
                    pos = line.p2();
                line.setP2(pos);
                m_tempPolyline.at(m_tempPolyline.size()-1)->setLine(line);
            }
            else
                pos = getPoint(pos, None);

            if (!m_cursorCircle->isVisible())
                m_cursorCircle->show();
            m_cursorCircle->setPos(pos);
            break;
        case DoorAdd:
            pos = getPoint(pos, None);
            if (!m_cursorCircle->isVisible())
                m_cursorCircle->show();
            m_cursorCircle->setPos(pos);
            break;
        default:
            break; // Ignore other :)
        }
        break;
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void MapFloor::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    switch (m_mode)
    {
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
    event->accept();
    switch (event->key())
    {
    case Qt::Key_Delete:
        switch (m_mode)
        {
//        case FloorAdd:
        case AreaAdd:
            removeLastFromTempPolyline();
            break;
        default:
//            if (selectedItems()[0]->type() == MapArea::Type)
//                deleteArea(qgraphicsitem_cast<MapArea*>(selectedItems()[0]));
            if (m_selectedArea != 0)
            {
                deleteArea(m_selectedArea);
                m_selectedArea = 0;
                areasToLineVec(m_outline);
            }
            break; // Ignore other :)
        }
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        switch (m_mode)
        {
//        case FloorAdd: {finalizeFloor(); break;}
        case AreaAdd: {finalizeArea(); break;}
        case WallAdd: {finalizeWall(); break;}
        default:{break;} // Ignore other :)
        }
        break;
    default:
        break; // Ignore other :)
    }
}

void MapFloor::addItem (QGraphicsItem *item)
{
    QGraphicsScene::addItem(item);
    if (item->type() == MapArea::Type)
    {
        MapArea *area = qgraphicsitem_cast<MapArea*>(item);
        for (int i = 0; i != area->areasNumber(); i++)
            addItem(area->area(i));
    }
}

//QVariant MapFloor::itemChange(QGraphicsItem::GraphicsItemChange change,
//                              const QVariant &value)
//{
//    if (change == QGraphicsItem::ItemSelectedChange)
//        for (int i = 0; i != selectedItems().size(); i++)
//            selectedItems()[i]->setSelected(false);
//    return QGraphicsItem::itemChange(change, value);
//}

void MapFloor::addBase(QPixmap &pixmap)
{
    if (!m_base)
    {
        m_base = new MapBase(pixmap, sceneRect());
        addItem(m_base);
    }
    else
        m_base->setPixmap(pixmap, sceneRect());
}

MapArea* MapFloor::area(int i)
{
    return m_outline;
}

QSet<MapArea*> MapFloor::parentAreas(MapArea *area, const QPointF *pos)
{
    QSet<MapArea*> set;
    MapArea *a;
    QStack<MapArea*> stk;
    stk.push(area);
    while (!stk.isEmpty())
    {
        a = stk.pop();
        if (a->contains(*pos))
            set << a;
        int size = a->areasNumber();
        for (int i = 0; i !=size; i++)
            stk.push(a->area(i));
    }
    return set;
}

void MapFloor::areasToLineVec(MapArea *area)
{
    if (!m_lines.isEmpty())
    {
        qDeleteAll(m_lines);
        m_lines.clear();
    }
    int size;
    MapArea *a;
    QStack<MapArea*> stk;
    stk.push(area);
    while (!stk.isEmpty())
    {
        a = stk.pop();
        size = a->polygon().size();
        for (int i = 0; i != size; i++)
        {

            m_lines.append(new QLineF(a->polygon().at(i),
                                  a->polygon().at((i+1)%size)));
        }
        size = a->areasNumber();
        for (int i = 0; i !=size; i++)
            stk.push(a->area(i));
    }
    int i = m_lines.size();
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
    // Compute destination between 2 points with standard formula
    return qSqrt(qPow(m.x() - n.x(), 2) + qPow(m.y() - n.y(), 2));
}

bool MapFloor::isInSegment(const QLineF &l, const QPointF &p)
{
    return ((p.x() >= qMin(l.x1(), l.x2())) & (p.x() <= qMax(l.x1(), l.x2()))&
            (p.y() >= qMin(l.y1(), l.y2())) & (p.y() <= qMax(l.y1(), l.y2())));
}

QPointF MapFloor::getPerpendicularBase(QPointF m, QLineF l)
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

bool MapFloor::getPointFromLine(QPointF m, QPointF &newPoint, const QLineF &l,
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



    if ((d < cMagnetDestToLine) & (d < min) & (isInSegment(l, *p)))
    {
        min = d;
        newPoint = *p;
        return true;
    }
    else
        return false;
}

bool MapFloor::getPointFromLineTops(QPointF m, QPointF &newPoint,
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

QPointF MapFloor::getPoint(QPointF m, Straight straight, bool magnetToTops)
{
    qreal min = INFINITY;
    QPointF newPoint;

    if ((straight == None) & (magnetToTops))
        for (int i = 0; i != m_lines.size(); i++)
            if (getPointFromLineTops(m, newPoint, m_lines.at(i)))
                return newPoint;

    if (m_isCrossLinesActive & (!m_tempPolyline.isEmpty()))
    {
        if (getPointFromLine(
            m, newPoint, m_crossLineHorizontal->line(), straight, min))
        {
            m_crossLineHorizontal->setLine(
                    QLineF(m_tempPolyline.at(0)->line().p1(), newPoint));
            m_crossLineHorizontal->show();
            return newPoint;
        }
        else
            {
                m_crossLineHorizontal->setLine(
                        0, m_tempPolyline.at(0)->line().y1(),
                        sceneRect().right(), m_tempPolyline.at(0)->line().y1());
                m_crossLineHorizontal->hide();
            }
        if (getPointFromLine(
            m, newPoint, m_crossLineVertical->line(), straight, min))
        {
            m_crossLineVertical->setLine(
                    QLineF(m_tempPolyline.at(0)->line().p1(), newPoint));
            m_crossLineVertical->show();
            return newPoint;
        }
        else
            {
                m_crossLineVertical->setLine(
                        m_tempPolyline.at(0)->line().x1(), 0,
                        m_tempPolyline.at(0)->line().x1(), sceneRect().bottom());
                m_crossLineVertical->hide();
            }
    }

    for (int i = 0; i != m_lines.size(); i++)
        getPointFromLine(m, newPoint, *m_lines.at(i), straight, min);

    for (int i = 0; i != m_walls.size(); i++)
        getPointFromLine(m, newPoint, m_walls.at(i)->line(), straight, min);

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
    if (!(m_tempPolyline.isEmpty()))
    {
        removeItem(m_tempPolyline.at(m_tempPolyline.size()-1));
        m_tempPolyline.pop_back();
    }
}

void MapFloor::finalizeWall()
{
    setMode(Planning);
    m_walls.append(m_tempLine);
    m_tempLine = 0;
    m_cursorCircle->hide();
}

void MapFloor::finalizeArea()
{
    if (m_tempPolyline.size() > 2)
    {

        QPolygonF polygon(convertLineVecToPolygon(m_tempPolyline));

        MapArea *area;
        MapArea *parentArea = 0;
        if (!m_outline)
        {
            area = new MapArea(polygon);
            m_outline = area;
            area->setZValue(0.0);
        }
        else
        {
            gpc_polygon gpc_curpgn = convertQtToGpcPolygon(&polygon);
            gpc_polygon gpc_intersection;
            MapArea *a;
            QStack<MapArea*> stk;
            stk.push(m_outline);
            while (!stk.isEmpty())
            {
                a = stk.pop();

                QPolygonF pgn = a->polygon();
                gpc_polygon gpc_pgn = convertQtToGpcPolygon(&pgn);
                gpc_polygon_clip(GPC_INT, &gpc_curpgn, &gpc_pgn,
                                 &gpc_intersection);
                if (gpc_intersection.num_contours != 0)
                {
                    gpc_polygon gpc_newIntersection;
                    gpc_polygon_clip(GPC_DIFF, &gpc_curpgn, &gpc_intersection,
                                     &gpc_newIntersection);
                    if (gpc_newIntersection.num_contours == 0)
                    {
                        parentArea = a;
                        int size = a->areasNumber();
                        for (int i = 0; i !=size; i++)
                            stk.push(a->area(i));
                    }
                    else
                    {
                        QMessageBox::warning(0, tr("Ошибка при создании области"),
                                             tr("Пожалуйста, поместите "
                                                "область на этаж, чтобы не "
                                                "было пересечения с другими "
                                                "областями"));
                        return;
                    }
                }
            }
            if (parentArea != 0)
            {
                area = new MapArea(polygon);
                parentArea->addArea(area);
            }
            else
            {
                QMessageBox::warning(0, tr("Ошибка при создании области"),
                                     tr("Пожалуйста, поместите область "
                                        "внутри области этажа."));
                return;
            }
        }

        addItem(area);
        for (int i = 0; i != m_tempPolyline.size(); i++)
            removeItem(m_tempPolyline.at(i));
        qDeleteAll(m_tempPolyline);
        m_tempPolyline.clear();
        m_crossLineHorizontal->hide();
        m_crossLineVertical->hide();
        m_crossLineHorizontal->setLine(-5, -5, -5, -5);   // Else there are false
        m_crossLineVertical->setLine(-5, -5, -5, -5);     // misoperation
        m_cursorCircle->hide();
        setMode(Planning);
    }
    else
        QMessageBox::warning(0, tr("Ошибка при создании области"),
                             tr("Область должна иметь хотя бы две вершины. "
                                "Пожалуйста, завершите создание области."));
}

void MapFloor::deleteArea(MapArea *area)
{
    while (area->areasNumber() != 0)
        deleteArea(area->area(0));
    removeItem(area);
    if (area != m_outline)
        area->parent()->deleteArea(area);
    else
    {
        delete m_outline;
        m_outline = 0;
    }
}

void MapFloor::finalizeDoor(QPointF pos/*MapArea *area, QLineF line*/)
{
    /*setMode(Planning);
    cursorCircle->hide();
    for (int i = 0; i !=areas.size(); i++)
    {

        for (int j = 0; j != areas.at(i)->polygon().size(); j++)
        {
            QLineF l = QLineF(areas.at(i)->polygon().at(j),
                       areas.at(i)->polygon().at((j+1)%
                                                areas.at(i)->polygon().size()));
            bool b = false;
            if (l.dx() == 0)
            {
                if (pos.x() == l.x1())
                    b = true;
            }
            else
            {
                qreal k = (l.dy()/l.dx());
                if (abs((l.y1() - pos.y())/(l.x1() - pos.x()) - k) < 0.00001)
                    b = true;
            }
            if (b)
            {
                b = false;
                qreal x1, x2, y1, y2, r, sinA, cosA;
//                x2 = l.dy()/l.length() * 50 + pos.x();
//                x1 = 2*pos.x() - x2;
//                y2 = l.dx()*l.length() * 50 + pos.y();
//                y1 = 2*pos.y() - y2;
                r = l.angle()*M_PI/180;
                sinA = sin(r);
                cosA = cos(r);
                x2 = pos.x() + cosA*50;
                y2 = pos.y() + sinA*50;
                x1 = 2*pos.x() - x2;
                y1 = 2*pos.y() - y2;
                QGraphicsLineItem *door = new QGraphicsLineItem(
                        QLineF(x1, y1, x2, y2), areas.at(i), this);
                door->setPen(QPen(Qt::red));
                doors.append(door);
                areas.at(i)->addDoor(door);
                return;
            }
        }
    }*/

}

QPolygonF MapFloor::convertLineVecToPolygon(const
                                  QVector<QGraphicsLineItem*> &vec) const
{
    QPolygonF polygon;
    for (int i = 0; i != vec.size(); i++)
        polygon << vec.at(i)->line().p1();
    return polygon;
}

gpc_polygon MapFloor::convertQtToGpcPolygon(const QPolygonF *pgn)
{
    gpc_polygon gpc_pgn;
    int size = pgn->size();
    gpc_pgn.num_contours = 1;
    gpc_pgn.contour = new gpc_vertex_list[1];
    gpc_pgn.contour[0].num_vertices = size;
    gpc_pgn.contour[0].vertex = new gpc_vertex[size];
    for (int i = 0; i != size; i++)
    {
        gpc_pgn.contour[0].vertex[i].x = pgn->at(i).x();
        gpc_pgn.contour[0].vertex[i].y = pgn->at(i).y();
    }
    return gpc_pgn;
}

void MapFloor::selectArea(MapArea *area)
{
    selectClear();
    area->setBrush(QBrush(QColor::fromRgb(243, 243, 243)));
    m_selectedArea = area;
}

void MapFloor::selectClear()
{
    if (m_selectedArea != 0)
        m_selectedArea->setBrush(QBrush(Qt::white));
}
