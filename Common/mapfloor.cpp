#include "mapfloor.h"

const qreal MapFloor::OutlineZValue = 0.0;
quint32 MapFloor::m_count = 0;

MapFloor::MapFloor(const QRectF &sceneRect, QObject *parent) :
    QGraphicsScene(sceneRect, parent),
    cCursorCircleR(7.0)
{
    m_mode = Idle;
    m_magnetToExtensions = false;
    m_border = new QGraphicsRectItem(sceneRect, 0, this);
    m_border->setBrush(QBrush(Qt::white));
    m_border->setZValue(-2.0);
    m_base = 0;
    addBase("");
    m_tempLine = 0;
    m_cursorCircle = new QGraphicsEllipseItem(
            -cCursorCircleR, -cCursorCircleR,
            2*cCursorCircleR, 2*cCursorCircleR, 0, this);
    m_cursorCircle->setZValue(100500);  // ###increase 100500
    m_cursorCircle->setPen(QPen(Qt::red));
    m_cursorCircle->setBrush(QBrush(Qt::NoBrush));
    m_cursorCircle->hide();
    m_crossLineHorizontal = new QGraphicsLineItem(0, this);
    m_crossLineHorizontal->setZValue(100500 - 4);
    m_crossLineHorizontal->setPen(QPen(Qt::DotLine));
    m_crossLineHorizontal->hide();
    m_crossLineVertical = new QGraphicsLineItem(0, this);
    m_crossLineVertical->setZValue(100500 - 4);
    m_crossLineVertical->setPen(QPen(Qt::DotLine));
    m_crossLineVertical->hide();
    m_startPoint = new QGraphicsEllipseItem(
            -cCursorCircleR, -cCursorCircleR,
            2*cCursorCircleR, 2*cCursorCircleR, 0, this);
    m_startPoint->setZValue(100500 - 3);
//    m_startPoint->setPen(QPen(Qt::red));
    m_startPoint->setBrush(QBrush(Qt::red));
    m_startPoint->hide();
    m_selection = new MapSelection(false);
    m_uin = ++m_count;
}

QDataStream &operator<<(QDataStream &output, const MapFloor &floor)
{
    MapDoor::clearFinishedDoors();
    int last = floor.m_outlines.size();
    output << floor.m_uin << floor.m_name << floor.m_base->fileName() << last;
    for (int i = 0; i != last; i++)
        output << *floor.m_outlines[i];
    last = floor.m_walls.size();
    output << last;
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
    QString baseFileName;
    int last;
    input >> floor.m_uin >> floor.m_name >> baseFileName >> last;
    floor.addBase(baseFileName);
    for (int i = 0; i != last; i++)
    {
        floor.m_outlines.append(new MapArea(0));
        input >> *floor.m_outlines[i];
        floor.m_outlines[i]->setZValue(MapFloor::OutlineZValue);
        floor.addItem(floor.m_outlines[i]);
    }

    MapArea *a1;
    QStack<MapArea*> stk1;
    for (int i = 0; i != last; i++)
        stk1.push(floor.m_outlines[i]);
    while (!stk1.isEmpty())
    {
        a1 = stk1.pop();

        for (int i = 0; i != a1->doorsNumber(); i++)
        {
            MapDoor *door = a1->door(i);
            MapArea *a2;
            QStack<MapArea*> stk2;
            for (int i = 0; i != last; i++)
                stk2.push(floor.m_outlines[i]);
            while (!stk2.isEmpty())
            {
                a2 = stk2.pop();
                if ((a1 != a2) & (door->parentAreasUins().contains(a2->uin())))
                    a2->addDoor(door);
                int size = a2->areasNumber();
                for (int i = 0; i !=size; i++)
                    stk2.push(a2->area(i));
            }
            floor.addItem(door);
            floor.addPointNodesMagnetTo(door->pos());
        }

        int size = a1->areasNumber();
        for (int i = 0; i !=size; i++)
            stk1.push(a1->area(i));
    }

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
    floor.m_count = qMax(floor.m_count, floor.m_uin);

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
        case Idle:
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
            takeAwayGraphicsLineItem(m_crossLineHorizontal);
            takeAwayGraphicsLineItem(m_crossLineVertical);
            m_startPoint->hide();
            m_cursorCircle->hide();
//            break;
        case AreaAdd:
        case DoorAdd:
        case WallAdd:
            m_selection->clear();
            areasToLineVec();
            break;
        case NodeAdd:
            m_selection->clear();
            m_tempLine = new QGraphicsLineItem();
            m_tempLine->setZValue(100500 - 2);
            addItem(m_tempLine);
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
                m_tempLine->setZValue(100500 - 5);
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
                m_startPoint->setPos(p1);
                m_startPoint->show();
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
            m_tempPolyline.back()->setZValue(100500 - 5);
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
        case NodeAdd:
            emit addedNode(m_cursorCircle->pos(), m_uin);
            break;
        default:
        {
            QGraphicsItem *item = itemAt(event->scenePos());
            bool selectionIsNormal = false;
            if (item)
                if (((item->type() == MapArea::Type) |
                    (item->type() == MapDoor::Type) |
                    (item->type() == GraphNode::Type)) &
                    ((m_mode == Idle) | (m_mode == Selection)))
                {
                    selectionIsNormal = true;
                    m_selection->addItem(item);
                    setMode(Selection);
                }
            if (!selectionIsNormal)
                m_selection->clear();
            break; // Ignore other :)
        }
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
        QLineF line;
        QPointF point = QPointF(0, 0);
        switch (m_mode)
        {
        case WallAdd:
            if (m_tempLine)
                point = m_tempLine->line().p1();
            line = getLine(QLineF(point, pos), (m_tempLine == 0),
                           miLines | miTops, event->modifiers());
            if (m_tempLine)
                m_tempLine->setLine(line);
            break;
        case AreaAdd:
            if (!m_tempPolyline.isEmpty())
                point = m_tempPolyline.back()->line().p1();
            line = getLine(QLineF(point, pos), m_tempPolyline.isEmpty(),
                           miLines | miTops, event->modifiers());
            if (!m_tempPolyline.isEmpty())
                m_tempPolyline.at(m_tempPolyline.size()-1)->setLine(line);
            break;
        case DoorAdd:
            line = getLine(QLineF(point, pos), true, miLines,
                           event->modifiers());
            break;
        case NodeAdd:
            if (m_lastNode)
                point = m_lastNode->pos();
            line = getLine(QLineF(point, pos), (m_lastNode == 0), miNodes,
                           event->modifiers());
            if (m_lastNode)
                m_tempLine->setLine(line);
            break;
        default:
            break; // Ignore other :)
        }
        if ((m_mode >= WallAdd) & (m_mode <= NodeAdd))
        {
            m_cursorCircle->show();
            m_cursorCircle->setPos(line.p2());
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
//    case Idle:
    case Selection:
        if (!m_selection->isEmpty())
            if (m_selection->item()->type() == MapArea::Type)
                emit mouseDoubleClicked();
        break;
//    case Planning:
//        setMode(Marking);
//        break;
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
            if (m_tempPolyline.isEmpty())
                m_startPoint->hide();
            break;
        case NodeAdd:
            if (m_lastNode)
            {
                int i = m_graphNodes.indexOf(m_lastNode->pos());
                if (i > -1)
                    m_graphNodes.remove(i);
                takeAwayGraphicsLineItem(m_tempLine);
                emit deletedNode(m_lastNode);
            }
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
                case GraphNode::Type:
                {
                    int i = m_graphNodes.indexOf(shapeItem->pos());
                    if (i > -1)
                        m_graphNodes.remove(i);
                    takeAwayGraphicsLineItem(m_tempLine);
                    emit deletedNode(qgraphicsitem_cast<GraphNode*>(shapeItem));
                    break;
                }
                }
                areasToLineVec();
            }
            break; // Ignore other :)
        }
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        switch (m_mode)
        {
        case AreaAdd:
            finalizeArea();
            break;
        case DoorAdd:
            finalizeDoor();
            break;
        case WallAdd:
            finalizeWall();
            break;
        case NodeAdd:
            setMode(Idle);
//            emit graphStartedAnew();
            break;
        default:
            break; // Ignore other :)
        }
        break;
    case Qt::Key_Escape:
        if ((m_mode == AreaAdd) | (m_mode == DoorAdd) |
            (m_mode == WallAdd) | (m_mode == NodeAdd))
            setMode(Idle);
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

//void MapFloor::setGraphNodes(QVector<QPointF*> &nodes)
//{
//    m_graphNodes = nodes;
//}

void MapFloor::magnetToExtensions(bool b)
{
    m_magnetToExtensions = b;
}

void MapFloor::setLastNode(GraphNode *node)
{
    m_lastNode = node;
}

void MapFloor::addBase(QString fileName)
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
            m_base = new MapBase(fileName, sceneRect());
            addItem(m_base);
//        }
    }
    else
        m_base->setPixmap(fileName, sceneRect());
}

void MapFloor::baseSetVisible(bool visible)
{
    m_base->setVisible(visible);
}

MapArea* MapFloor::area(int i)
{
    return m_outlines[i];
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

void MapFloor::areasToLineVec(/*MapArea *area*/)
{
    if (!m_lines.isEmpty())
    {
        qDeleteAll(m_lines);
        m_lines.clear();
    }
    int size;
    MapArea *a;
    QStack<MapArea*> stk;
    for (int i = 0; i != m_outlines.size(); i++)
        stk.push(m_outlines[i]);
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
    int size;
    MapArea *a;
    QStack<MapArea*> stk;
    for (int i = 0; i != m_outlines.size(); i++)
        stk.push(m_outlines[i]);
    while (!stk.isEmpty())
    {
        a = stk.pop();
        size = a->polygon().size();
        for (int i = 0; i != size; i++)
            if (Geometry::contain(pos, QLineF(a->polygon().at(i),
                                              a->polygon().at((i+1)%size))))
            {
                result.append(a);
                break;
            }
        size = a->areasNumber();
        for (int i = 0; i !=size; i++)
            stk.push(a->area(i));
    }
    return result;
}

QPointF MapFloor::getPoint(QPointF m, Geometry::Straight straight,
                           MagnetItems items)
{
    qreal min = INFINITY;
    QPointF newPoint;
    bool b = m_magnetToExtensions;

    if ((straight == Geometry::None) & ((items & miTops) != 0))
        for (int i = 0; i != m_lines.size(); i++)
            if (Geometry::getPointFromLineTops(m, newPoint, m_lines.at(i)))
                return newPoint;

    if (items & miLines)
    {
        if (m_isCrossLinesActive & (!m_tempPolyline.isEmpty()))
        {
            if (Geometry::getPointFromLine(
                m, newPoint, m_crossLineHorizontal->line(), straight, b, min))
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
                            sceneRect().right(),
                            m_tempPolyline.at(0)->line().y1());
                    m_crossLineHorizontal->hide();
                }
            if (Geometry::getPointFromLine(
                m, newPoint, m_crossLineVertical->line(), straight, b, min))
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
                            m_tempPolyline.at(0)->line().x1(),
                            sceneRect().bottom());
                    m_crossLineVertical->hide();
                }
        }

        for (int i = 0; i != m_lines.size(); i++)
            Geometry::getPointFromLine(m, newPoint,
                                       *m_lines.at(i), straight, b, min);

        for (int i = 0; i != m_walls.size(); i++)
            Geometry::getPointFromLine(m, newPoint,
                                       m_walls.at(i)->line(), straight, b, min);
    }

    if (items & miNodes)
    {
        if ((straight == Geometry::None))
            for (int i = 0; i != m_graphNodes.size(); i++)
            {
                qreal d = Geometry::dest(m_graphNodes.at(i), m);
                // WARNING! cMagnet...
                if (d < 10.0)
                    return m_graphNodes.at(i);
            }
    }

    if (min == INFINITY)
        return m;
    else
        return newPoint;

}

QLineF MapFloor::getLine(QLineF line, bool first, MagnetItems items,
                         Qt::KeyboardModifiers modifiers)
{
    Geometry::Straight straight = Geometry::None;
    if (!(modifiers & Qt::CTRL) & !first)
        straight = Geometry::straighten(line);
    if (modifiers & Qt::ALT)
        items = items & !miTops;
    if (!(modifiers & Qt::SHIFT))
        line.setP2(getPoint(line.p2(), straight, items));
    return line;
}

QPointF MapFloor::graphGetPoint(QPointF pos)
{
    QVector<QPointF> vec;
//    for (int i = 0; i != vec.size(); i++)
    /////////////////////!@!!/!/!/!/!/!/!/!/
    return QPointF();

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

void MapFloor::takeAwayGraphicsLineItem(QGraphicsLineItem *item)
{
    if (item)
        item->setLine(-INFINITY, -INFINITY, -INFINITY, -INFINITY);
}

MapArea::CreateError MapFloor::addArea(MapArea *area)
{
    MapArea *parentArea = 0;

    QPolygonF areaPgn = area->polygon();
    MapArea *a;
    QStack<MapArea*> stk;
    for (int i = 0; i != m_outlines.size(); i++)
        stk.push(m_outlines[i]);
    while (!stk.isEmpty())
    {
        a = stk.pop();
        QPolygonF pgn = a->polygon();
        QPolygonF intersection = pgn.intersected(areaPgn);
        if (!intersection.isEmpty())
        {
            QPolygonF subtraction = areaPgn.subtracted(intersection);
            if (subtraction.isEmpty())
            {
                if (!pgn.subtracted(areaPgn).isEmpty())
                {
                    parentArea = a;
                    int size = a->areasNumber();
                    for (int i = 0; i !=size; i++)
                        stk.push(a->area(i));
                }
                else
                {
                    delete area;
                    return MapArea::ceAreaExist;
                }
            }
            else
            {
                delete area;
                return MapArea::ceIntersection;
            }
        }
    }
    if (parentArea != 0)
        parentArea->addArea(area);
    else
        m_outlines.append(area);
    addItem(area);
    return MapArea::ceNone;
}

bool MapFloor::addDoor(MapDoor *door)
{
    QPointF point = QPointF(door->pos());
    QVector<MapArea*> areas = pointContainers(point);
    if (!areas.isEmpty())
    {
//        qreal maxZValue = 0;
        for (int i = 0; i != areas.size(); i++)
//        {
//            maxZValue = qMax(maxZValue, areas[i]->zValue());
            areas[i]->addDoor(door);
//        }
//        door->setZValue(maxZValue + 1.0);
        if (!m_graphNodes.contains(point))
            m_graphNodes.append(point);
        addItem(door);
        return true;
    }
    else
    {
        delete door;
        return false;
    }
}

void MapFloor::addPointNodesMagnetTo(QPointF point)
{
    if (!m_graphNodes.contains(point))
        m_graphNodes.append(point);
}

void MapFloor::finalizeWall()
{
    m_walls.append(m_tempLine);
    setMode(Idle);
}

void MapFloor::finalizeArea()
{
    if (m_tempPolyline.size() > 2)
    {
        QPolygonF polygon(convertLineVecToPolygon(m_tempPolyline));
        MapArea *area = new MapArea(polygon);
        switch (addArea(area))
        {
        case MapArea::ceNone:
            setMode(Idle);
            break;
        case MapArea::ceIntersection:
            QMessageBox::warning(0, tr("Ошибка при создании области"),
                                 tr("Создаваемая область пересекается с "
                                    "другими областями."));
            break;
        case MapArea::ceAreaExist:
            QMessageBox::warning(0, tr("Ошибка при создании области"),
                                 tr("На этаже уже есть данная область."));
            break;
        }
    }
    else
        QMessageBox::warning(0, tr("Ошибка при создании области"),
                             tr("Создаваемая область имеет менее 3 вершин."));
}

void MapFloor::deleteArea(MapArea *area)
{
    while (area->areasNumber() != 0)
        deleteArea(area->area(0));
    removeItem(area);
    while (area->doorsNumber() != 0)
        deleteDoor(area->door(0));
    int i = m_outlines.indexOf(area);
    if (i == -1)  // If there is not outline area
        area->parent()->deleteArea(area);
    else
    {
        m_outlines.remove(i);
        delete area;
    }
}

void MapFloor::finalizeDoor()
{
    MapDoor *door = new MapDoor(m_cursorCircle->pos());
    if (addDoor(door))
        setMode(Idle);
}

void MapFloor::deleteDoor(MapDoor *door)
{
    for (int i = 0; i != door->parentAreasNumber(); i++)
        door->parentArea(i)->deleteDoor(door);
    removeItem(door);
    int i = m_graphNodes.indexOf(door->pos());
    if (i > -1)
        m_graphNodes.remove(i);
    delete door;
}

QPolygonF MapFloor::convertLineVecToPolygon(const
                                  QVector<QGraphicsLineItem*> &vec) const
{
    QPolygonF polygon;
    for (int i = 0; i != vec.size(); i++)
        polygon << vec.at(i)->line().p1();
    return polygon;
}

quint32 MapFloor::uin()
{
    return m_uin;
}
