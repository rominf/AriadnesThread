#include "maparea.h"

const int MapArea::cFontSize = 16;
quint32 MapArea::m_count = 0;

MapArea::MapArea(const QPolygonF &polygon):
        QGraphicsPolygonItem(polygon)
{
    m_name = new QGraphicsTextItem(this);
    m_description = "";
    setBrush(QBrush(Qt::NoBrush));
    m_uin = ++m_count;
}

MapArea::MapArea(const MapArea &area, const QString &before,
                 const QString &after):
        QGraphicsPolygonItem(area.polygon())
{
    setBrush(QBrush(Qt::NoBrush));
    m_uin = ++m_count;
    m_name = new QGraphicsTextItem(this);
    QString oldAreasName = area.m_name->toPlainText();
    QString newAreasName;
    if (oldAreasName.indexOf(before) == 0)
        newAreasName = oldAreasName.replace(before, after);
    else
        newAreasName = oldAreasName;
    setName(newAreasName);
    m_name->setVisible(area.m_name->isVisible());
    m_description = area.m_description;
    for (int i = 0; i != area.m_areas.size(); i++)
        addArea(new MapArea(*area.m_areas.at(i), before, after));
}

QDataStream &operator<<(QDataStream &output, const MapArea &area)
{

    int last = area.m_areas.size();
    output << last;
    for (int i = 0; i != last; i++)
        output << *area.m_areas.at(i);

    QList<QPointF> list;
    list = area.polygon().toList();
    output << list;
    last = 0;
    for (int i = 0; i != area.m_doors.size(); i++)
        last += !area.m_doors.at(i)->isFinished();
    output << area.m_uin << area.m_name->toPlainText()
            << area.m_name->isVisible() << area.m_description << last;
    for (int i = 0; i != area.m_doors.size(); i++)
        if (!area.m_doors.at(i)->isFinished())
            output << *area.m_doors.at(i);

    return output;
}

QDataStream &operator>>(QDataStream &input, MapArea &area)
{
    int last;
    input >> last;
    for (int i = 0; i != last; i++)
    {
        MapArea *tempArea = new MapArea(0);
        area.addArea(tempArea);
        input >> *tempArea;
    }
    QList<QPointF> list;
    input >> list;
    QPolygonF polygon(list.toVector());
    area.setPolygon(polygon);
    QString name;
    bool nameVisible;
    input >> area.m_uin >> name >> nameVisible >> area.m_description >> last;
    area.setName(name);
    area.m_name->setVisible(nameVisible);
    area.m_count = qMax(area.m_count, area.m_uin);
    for (int i = 0; i != last; i++)
    {
        QPointF p = QPointF(0, 0);
        MapDoor *door = new MapDoor(p);
        input >> *door;
        area.addDoor(door);
    }
    return input;
}

QString MapArea::name()
{
    return m_name->toPlainText();
}

void MapArea::setName(const QString &name)
{
    QTextDocument *doc = new QTextDocument(name);
    doc->setDefaultFont(QFont("Arial", cFontSize));
    int fontSize = cFontSize;
    while ((doc->size().width() > boundingRect().width()) |
           (doc->size().height() > boundingRect().height()))
    {
        fontSize--;
        doc->setDefaultFont(QFont("Arial", fontSize));
    }
    QTextOption alignment(Qt::AlignHCenter);
    doc->setDefaultTextOption(alignment);
    m_name->setDocument(doc);
    m_name->setPos(boundingRect().center().x() - doc->size().width()/2,
                   boundingRect().center().y() - doc->size().height()/2);
//    m_name->setVisible(m_areas.isEmpty());
//        QFontMetricsF fm(m_name->font());
//        m_name->setPos(boundingRect().center().x() - fm.width(name)/2,
//                       boundingRect().center().y() - fm.height()/2);
}

void MapArea::setNameVisible(bool show)
{
    m_name->setVisible(show);
}

bool MapArea::isNameVisible()
{
    return m_name->isVisible();
}

QString MapArea::description()
{
    return m_description;
}

void MapArea::setDescription(const QString &description)
{
    m_description = description;
}

MapArea* MapArea::parent()
{
    return m_parent;
}

//void MapArea::setParent(MapArea *parent)
//{
//    m_parent = this;
//}

//int MapArea::number()
//{
//    if (m_parent != 0)
//        return m_parent->
////    return m_number;
//}

//void MapArea::setNumber(int i)
//{
//    m_number = i;
//}

void MapArea::addArea(MapArea *area)
{
    if (isVisible() & m_areas.isEmpty())
        setNameVisible(false);
    area->m_parent = this;
    m_areas.append(area);
    area->setZValue(zValue() + 2.0);
}

MapArea* MapArea::area(int i) const
{
    return m_areas.at(i);
}

void MapArea::deleteArea(MapArea* area)
{
    int i = m_areas.indexOf(area);
    if (i > -1)
        m_areas.remove(i);
    delete area;
    if (m_areas.size() == 0)
        setNameVisible(true);
}

int MapArea::areasNumber()
{
    return m_areas.size();
}

void MapArea::addDoor(MapDoor *door)
{
    m_doors.append(door);
    door->addParentArea(this);
    door->setZValue(qMax(door->zValue(), zValue() + 1.0));
}

MapDoor* MapArea::door(int i) const
{
    return m_doors.at(i);
}

void MapArea::deleteDoor(MapDoor* door)
{
    int i = m_doors.indexOf(door);
    if (i > -1)
        m_doors.remove(i);
}

int MapArea::doorsNumber()
{
    return m_doors.size();
}

int MapArea::type() const
{
    return Type;
}

quint32 MapArea::uin()
{
    return m_uin;
}
