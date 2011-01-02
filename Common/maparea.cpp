#include "maparea.h"

const int MapArea::cFontSize = 16;
quint32 MapArea::m_count = 0;

MapArea::MapArea(const QPolygonF &polygon):
        QGraphicsPolygonItem(polygon)
{
//    setFlags(QGraphicsItem::ItemIsSelectable);

    m_name = new QGraphicsTextItem(this);
    m_name->setFont(QFont("Arial", cFontSize));
//    m_name->
//    m_name->setZValue(1);
//    m_name->setPos(this->polygon().boundingRect().center());
    m_description = "";
    setBrush(QBrush(Qt::NoBrush));
    m_uin = ++m_count;
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

    QString s = area.m_name->toPlainText();
    output << s << area.m_uin << last;
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
        input >> *tempArea;
        area.addArea(tempArea);
//        area.m_areas.append(tempArea);
    }
    QList<QPointF> list;
    input >> list;
    QPolygonF polygon(list.toVector());
    area.setPolygon(polygon);
    QString s;
    input >> s >> area.m_uin >> last;
    area.setName(s);
//    area.m_name->setPlainText(s);
//    area.m_name->setPos(area.polygon().boundingRect().center());
    area.m_count = qMax(area.m_count, area.m_uin);
    for (int i = 0; i != last; i++)
    {
        QPointF p = QPointF(0, 0);
        MapDoor *door = new MapDoor(p);
        input >> *door;
        area.addDoor(door);
        door->setZValue(qMax(door->zValue(), area.zValue() + 1.0));
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
    if (m_areas.isEmpty())
    {
        m_name->setVisible(true);
//        QFontMetricsF fm(m_name->font());
//        m_name->setPos(boundingRect().center().x() - fm.width(name)/2,
//                       boundingRect().center().y() - fm.height()/2);
        m_name->setPos(boundingRect().center().x() - doc->size().width()/2,
                       boundingRect().center().y() - doc->size().height()/2);
    }
    else
        m_name->setVisible(false);
}

void MapArea::setNameVisible(bool show)
{
    m_name->setVisible(show);
}

QString MapArea::description()
{
    return m_description;
}

void MapArea::setDescription(QString &description)
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
    area->m_parent = this;
    m_areas.append(area);
    area->setZValue(zValue() + 1.0);
    setNameVisible(false);
}

MapArea* MapArea::area(int i) const
{
    return m_areas.at(i);
}

void MapArea::deleteArea(MapArea* area)
{
    m_areas.remove(m_areas.indexOf(area));
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
}

MapDoor* MapArea::door(int i) const
{
    return m_doors.at(i);
}

void MapArea::deleteDoor(MapDoor* door)
{
    m_doors.remove(m_doors.indexOf(door));
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
