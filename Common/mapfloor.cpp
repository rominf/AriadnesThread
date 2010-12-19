#include "mapfloor.h"

const qreal MapFloor::OutlineZValue = -1.0;

MapFloor::MapFloor(const QRectF &sceneRect, QObject *parent) :
    QGraphicsScene(sceneRect, parent),
    cCursorCircleR(15.0)
{
    m_mode = Planning;
    m_border = new QGraphicsRectItem(sceneRect, 0, this);
    m_border->setBrush(QBrush(Qt::white));
    m_border->setZValue(-2.0);
    m_outline = 0;
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
    m_selection = new MapSelection(false);
}

QDataStream &operator<<(QDataStream &output, const MapFloor &floor)
{
    MapDoor::clearFinishedDoors();
    int last = floor.m_walls.size();
    output << *floor.m_outline << last;
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
    input >> *floor.m_outline;
    floor.addItem(floor.m_outline);

    MapArea *a1;
    QStack<MapArea*> stk1;
    stk1.push(floor.m_outline);
    while (!stk1.isEmpty())
    {
        a1 = stk1.pop();

        for (int i = 0; i != a1->doorsNumber(); i++)
        {
            MapDoor *door = a1->door(i);
            MapArea *a2;
            QStack<MapArea*> stk2;
            stk2.push(floor.m_outline);
            while (!stk2.isEmpty())
            {
                a2 = stk2.pop();
                if ((a1 != a2) & (door->parentAreasUins().contains(a2->uin())))
                    a2->addDoor(door);
//                if (door->parentAreasUins().contains(a2->uin())) // If door is
//                {                                                // in this area
//                    if (a2->doorsNumber() == 0)                  // If this area
//                        a2->addDoor(door);                       // don't
//                    else if (door != a2->door(0))                // contain this
//                        a2->addDoor(door);                       // door already
//                }

                int size = a2->areasNumber();
                for (int i = 0; i !=size; i++)
                    stk2.push(a2->area(i));
            }
            floor.addItem(door);
        }

        int size = a1->areasNumber();
        for (int i = 0; i !=size; i++)
            stk1.push(a1->area(i));
    }

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
    return m_outline->name();
}

void MapFloor::setName(const QString &floorName)
{
    m_outline->setName(floorName);
}

MapFloor::Mode MapFloor::mode()
{
    return m_mode;
}

void MapFloor::setMode(Mode m)
{
    if (m != m_mode)
    {
        switch (m)
        {
        case Planning:
            for (int i = 0; i != m_tempPolyline.size(); i++)
                removeItem(m_tempPolyline.at(i));
            qDeleteAll(m_tempPolyline);
            m_tempPolyline.clear();
            if (m_tempLine != 0)
            {
                removeItem(m_tempLine);
                delete m_tempLine;
                m_tempLine = 0;
            }
            m_crossLineHorizontal->hide();
            m_crossLineVertical->hide();
            m_crossLineHorizontal->setLine(-5, -5, -5, -5);   // Else there are false
            m_crossLineVertical->setLine(-5, -5, -5, -5);     // misoperation
            m_cursorCircle->hide();

        case AreaAdd:
        case DoorAdd:
        case WallAdd:
            m_selection->clear();
            if (m_outline != 0)
                areasToLineVec(m_outline);
            break;
        default:
            break;
        }

        m_mode = m;
//        emit modeChanged(m);
    }
//    else if (m == Selection)
        emit modeChanged(m);
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
//            if (getPoint(event->scenePos(), Geometry::None, false) ==
//                m_cursorCircle->pos())
//            {
                finalizeDoor();
                break;
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
//            }
        default:
            if (itemAt(event->scenePos()) != 0)
            {
                m_selection->addItem(itemAt(event->scenePos()));
                if ((m_mode == Marking) | (m_mode == Selection))
                    setMode(Selection);
            }
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
        Geometry::Straight straight;
        switch (m_mode)
        {
        case WallAdd:
            if (m_tempLine)
            {
                QLineF line(m_tempLine->line().p1(), pos);
                if (!(event->modifiers() & Qt::ControlModifier))
                    straight = Geometry::straighten(line);
                else
                    straight = Geometry::None;
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
                pos = getPoint(pos, Geometry::None);
            }
            m_cursorCircle->show();
            m_cursorCircle->setPos(pos);
            break;
        case AreaAdd:
            if (!(m_tempPolyline.isEmpty()))
            {
                QLineF line(m_tempPolyline.back()->line().p1(), pos);
                if (!(event->modifiers() & Qt::ControlModifier))
                    straight = Geometry::straighten(line);
                else
                    straight = Geometry::None;
                if (!(event->modifiers() & Qt::ShiftModifier))
                    pos = getPoint(line.p2(), straight);
                else
                    pos = line.p2();
                line.setP2(pos);
                m_tempPolyline.at(m_tempPolyline.size()-1)->setLine(line);
            }
            else
                pos = getPoint(pos, Geometry::None);

            if (!m_cursorCircle->isVisible())
                m_cursorCircle->show();
            m_cursorCircle->setPos(pos);
            break;
        case DoorAdd:
            if (!(event->modifiers() & Qt::ShiftModifier))
                pos = getPoint(pos, Geometry::None);

//            if (itemAt(pos)->type() == MapArea::Type)
//                selectArea(qgraphicsitem_cast<MapArea*>(itemAt(pos)));
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
    case Planning:
        setMode(Marking);
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
            if (!m_selection->isEmpty())
            {
                QAbstractGraphicsShapeItem *shapeItem = m_selection->item();
                m_selection->clear();
                switch (shapeItem->type())
                {
                case MapArea::Type:
                    deleteArea(qgraphicsitem_cast<MapArea*>(shapeItem));
                    break;
                case MapDoor::Type:
                    deleteDoor(qgraphicsitem_cast<MapDoor*>(shapeItem));
                    break;
                }
                areasToLineVec(m_outline);
            }
            break; // Ignore other :)
        }
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        switch (m_mode)
        {
        case AreaAdd: {finalizeArea(); break;}
        case DoorAdd: {finalizeDoor(); break;}
        case WallAdd: {finalizeWall(); break;}
        default: {break;} // Ignore other :)
        }
        break;
    case Qt::Key_Escape:
        if ((m_mode == AreaAdd) | (m_mode == DoorAdd) | (m_mode == WallAdd))
            setMode(Planning);
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

void MapFloor::addBase(QPixmap &pixmap)
{
    if (!m_base)
    {
//        m_base = new MapBase(pixmap, sceneRect());
//        QPixmap *p = new QPixmap();
//        p->load(":/Abc");
//        p->load("Фото0025.jpg");
//        QPixmap p = QPixmap::fromImage(QImage(":/Abc"), Qt::AutoColor);
//        if (!p->isNull())
//        {
            m_base = new MapBase(pixmap, sceneRect());
            addItem(m_base);
//        }
    }
    else
        m_base->setPixmap(pixmap, sceneRect());
}

MapArea* MapFloor::area(int i)
{
    return m_outline;
}

QAbstractGraphicsShapeItem* MapFloor::selectedItem()
{
    return m_selection->item();
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
    if (area == 0)
        return;
    int size;
    MapArea *a;
    QStack<MapArea*> stk;
    stk.push(area);
    while (!stk.isEmpty())
    {
        a = stk.pop();
        size = a->polygon().size();
        for (int i = 0; i != size; i++)
            m_lines.append(new QLineF(a->polygon().at(i),
                                      a->polygon().at((i+1)%size)));
        size = a->areasNumber();
        for (int i = 0; i !=size; i++)
            stk.push(a->area(i));
    }
}

QVector<MapArea*> MapFloor::pointContainers(QPointF pos)
{
    QVector<MapArea*> result;
    if (m_outline != 0)
    {
        int size;
        MapArea *a;
        QStack<MapArea*> stk;
        stk.push(m_outline);
        while (!stk.isEmpty())
        {
            a = stk.pop();
            size = a->polygon().size();
            for (int i = 0; i != size; i++)
                if (Geometry::contain(pos,
                                      QLineF(a->polygon().at(i),
                                             a->polygon().at((i+1)%size))))
                {
                    result.append(a);
                    break;
                }
            size = a->areasNumber();
            for (int i = 0; i !=size; i++)
                stk.push(a->area(i));
        }
    }
    return result;
}

QPointF MapFloor::getPoint(QPointF m, Geometry::Straight straight,
                           bool magnetToTops)
{
    qreal min = INFINITY;
    QPointF newPoint;

    if ((straight == Geometry::None) & (magnetToTops))
        for (int i = 0; i != m_lines.size(); i++)
            if (Geometry::getPointFromLineTops(m, newPoint, m_lines.at(i)))
                return newPoint;

    if (m_isCrossLinesActive & (!m_tempPolyline.isEmpty()))
    {
        if (Geometry::getPointFromLine(
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
        if (Geometry::getPointFromLine(
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
        Geometry::getPointFromLine(m, newPoint, *m_lines.at(i), straight, min);

    for (int i = 0; i != m_walls.size(); i++)
        Geometry::getPointFromLine(m, newPoint, m_walls.at(i)->line(), straight, min);

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
        delete m_tempPolyline.back();
        m_tempPolyline.pop_back();
    }
}

void MapFloor::finalizeWall()
{
    m_walls.append(m_tempLine);
    setMode(Planning);
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
            area->setZValue(MapFloor::OutlineZValue);
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
    while (area->doorsNumber() != 0)
        deleteDoor(area->door(0));
    if (area != m_outline)
        area->parent()->deleteArea(area);
    else
    {
        delete m_outline;
        m_outline = 0;
    }
}

void MapFloor::deleteDoor(MapDoor *door)
{
    for (int i = 0; i != door->parentAreasNumber(); i++)
        door->parentArea(i)->deleteDoor(door);
    removeItem(door);
    delete door;
}

void MapFloor::finalizeDoor()
{
    QVector<MapArea*> areas = pointContainers(m_cursorCircle->pos());
    if (!areas.isEmpty())
    {
        MapDoor *door = new MapDoor(m_cursorCircle->pos());
        qreal maxZValue = 0;
        for (int i = 0; i != areas.size(); i++)
        {
            maxZValue = qMax(maxZValue, areas[i]->zValue());
            areas[i]->addDoor(door);
        }
        door->setZValue(maxZValue + 1.0);
        addItem(door);
        setMode(Planning);
    }

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
