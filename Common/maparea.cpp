#include "maparea.h"

const int MapArea::cFontSize = 16;
const QString MapArea::cFontFamily = "Arial";
quint32 MapArea::m_count = 0;

MapArea::MapArea(const QPolygonF &polygon, const quint32 floorUin):
        QGraphicsPolygonItem(polygon), m_floorUin(floorUin)
{
    m_uin = ++m_count;
    m_inscription = new QGraphicsTextItem(this);
    m_number = "";
    m_name = "";
    m_description = "";
//    updateTitle();
    setBrush(QBrush(Qt::NoBrush));
}

MapArea::MapArea(const MapArea &area, const quint32 floorUin,
                 const QString &before, const QString &after):
        QGraphicsPolygonItem(area.polygon()), m_floorUin(floorUin)
{
    setBrush(QBrush(Qt::NoBrush));
    m_uin = ++m_count;
    m_inscription = new QGraphicsTextItem(this);
    QString oldAreasNumber = area.m_number;
    QString newAreasNumber;
    if (oldAreasNumber.indexOf(before) == 0)
        newAreasNumber = oldAreasNumber.replace(before, after);
    else
        newAreasNumber = oldAreasNumber;
    m_number = newAreasNumber;
    m_name = area.m_name;
    m_description = area.m_description;
    setInscription(area.m_inscription->toPlainText());
//    updateTitle();
    for (int i = 0; i != area.m_areas.size(); i++)
        addArea(new MapArea(*area.m_areas.at(i), floorUin, before, after));
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
    output << area.m_uin << area.m_number << area.m_name << area.m_description
            << area.m_inscription->toPlainText() << last;
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
        MapArea *tempArea = new MapArea(0, area.m_floorUin);
        area.addArea(tempArea);
        input >> *tempArea;
    }
    QList<QPointF> list;
    input >> list;
    QPolygonF polygon(list.toVector());
    area.setPolygon(polygon);
    QString inscription;
    input >> area.m_uin >> area.m_number >> area.m_name >> area.m_description
            >> inscription >> last;
    area.setInscription(inscription);
//    area.updateTitle();
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

QString MapArea::number()
{
    return m_number;
}

void MapArea::setNumber(const QString &number)
{
    m_number = number;
}

//bool MapArea::isNumberVisible()
//{
//    return m_isNumberVisible;
//}

//void MapArea::setNumberVisible(bool show)
//{
//    m_isNumberVisible = show;
//}

QString MapArea::name()
{
    return m_name;
}

void MapArea::setName(const QString &name)
{
    m_name = name;
}

//bool MapArea::isNameVisible()
//{
//    return m_isNameVisible;
//}

//void MapArea::setNameVisible(bool show)
//{
//    m_isNameVisible = show;
//}

QString MapArea::description()
{
    return m_description;
}

void MapArea::setDescription(const QString &description)
{
    m_description = description;
}

QString MapArea::inscription()
{
    return m_inscription->toPlainText();
}

void MapArea::setInscription(const QString &inscription)
{
    if (!boundingRect().isValid())    // If area is invalid
        return;

    QTextDocument *doc = new QTextDocument(inscription);
    doc->setDefaultFont(QFont(cFontFamily, cFontSize));
    int fontSize = cFontSize;
    qreal width = polygon().boundingRect().width();
    qreal height = polygon().boundingRect().height();
    while (((doc->size().width() > width) |
            (doc->size().height() > height)) & (fontSize > 1))
        doc->setDefaultFont(QFont(cFontFamily, --fontSize));
//    doc->setDefaultFont(QFont(cFontFamily, ++fontSize));
    doc->setTextWidth(doc->size().width());
    QTextOption alignment(Qt::AlignHCenter);
    doc->setDefaultTextOption(alignment);
    m_inscription->setDocument(doc);
    m_inscription->setPos(boundingRect().center().x() - doc->size().width()/2,
                    boundingRect().center().y() - doc->size().height()/2);
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
//    if (isVisible() & m_areas.isEmpty())
//        setNameVisible(false);
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
//    if (m_areas.size() == 0)
//        setNameVisible(true);
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

quint32 MapArea::floorUin()
{
    return m_floorUin;
}

//void MapArea::updateTitle()
//{

////    m_name->setVisible(m_areas.isEmpty());
////        QFontMetricsF fm(m_name->font());
////        m_name->setPos(boundingRect().center().x() - fm.width(name)/2,
////                       boundingRect().center().y() - fm.height()/2);
//}
