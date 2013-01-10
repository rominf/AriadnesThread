#include "maparea.h"

const QPen MapArea::cPenNormal = QPen(Qt::black);
//const QPen MapArea::cPenSelected = QPen(QBrush(Qt::black), 3);
const QBrush MapArea::cBrushNormal = QBrush(Qt::NoBrush);
const QBrush MapArea::cBrushHole = QBrush(QColor(220, 220, 220, 255));
const QBrush MapArea::cBrushFull = QBrush(Qt::darkGray);
//const QBrush MapArea::cBrushSelected = QBrush(Global::colorSelected);
const int MapArea::cFontSize = 16;
const QString MapArea::cFontFamily = "Lucida Console";
quint32 MapArea::m_count = 0;

MapArea::MapArea(const QPolygonF &polygon, const quint32 floorUin):
        QGraphicsPolygonItem(polygon), m_floorUin(floorUin)
{
    m_uin = ++m_count;
    m_areaType = Room;
    m_number = "";
    m_name = "";
    m_description = "";
    m_inscription = new QGraphicsTextItem(this);
    m_inscription->setDefaultTextColor(Qt::black);
//    updateTitle();
    setBrush(cBrushNormal);
}

MapArea::MapArea(const MapArea &area, const quint32 floorUin,
                 const QString &floorFromName, const QString &floorToName):
        QGraphicsPolygonItem(area.polygon()), m_floorUin(floorUin)
{
    setBrush(cBrushNormal);
    m_uin = ++m_count;
    QString oldAreasNumber = area.m_number;
    QString newAreasNumber = oldAreasNumber;
    if (oldAreasNumber.indexOf(floorFromName) == 0)
        newAreasNumber.replace(0, floorFromName.length(), floorToName);
    m_number = newAreasNumber;
    m_name = area.m_name;
    m_description = area.m_description;
    m_inscription = new QGraphicsTextItem(this);
    setInscription(area.m_inscription->toPlainText().replace(
            oldAreasNumber, newAreasNumber));
//    updateTitle();
    for (int i = 0; i != area.m_areas.size(); i++)
        addArea(new MapArea(*area.m_areas.at(i), floorUin, floorFromName, floorToName));
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
    output << area.m_uin << area.m_areaType << area.m_number << area.m_name
            << area.m_description << area.m_inscription->toPlainText() << last;
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
    int type;
    QString inscription;
    input >> area.m_uin >> type >> area.m_number >> area.m_name >>
            area.m_description >> inscription >> last;
    area.setAreaType(MapArea::AreaType(type));
    area.setInscription(inscription);
    area.updateToolTip();
//    QString tooltip = area.info("<b>%1</b><br>%2<br><i>%3</i>");

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

//bool MapArea::operator<(const MapArea *b) const
//bool operator<(const MapArea *a, const MapArea *b) const
//{
//    QString numA = a->m_number;
//    QString numB = b->m_number;
//    QString nameA = a->m_name;
//    QString nameB = b->m_name;
//    if (!numA.isEmpty())
//    {
//        if (!numB.isEmpty())
//        {
//            if (numA < numB)
//                return true;
//            else
//                if (numA > numB)
//                    return false;
//        }
//        else
//            return true;

//    }
//    else
//        if (!numB.isEmpty())
//            return false;
//    return true;
//    if (!nameA.isEmpty())
//    {
//        if (!nameB.isEmpty())
//        {
//            if (nameA < nameB)
//                return true;
//            else
//                if (nameA > nameB)
//                    return false;
//        }
//        else
//            return true;

//    }
//    else
//        if (!nameB.isEmpty())
//            return false;
//    return false;
//}

MapArea::AreaType MapArea::areaType()
{
    return m_areaType;
}

void MapArea::setAreaType(MapArea::AreaType type, bool update)
{
    m_areaType = type;
    if (update)
        repaint();
}

QString MapArea::number()
{
    return m_number;
}

void MapArea::setNumber(const QString &number)
{
    m_number = number;
    updateToolTip();
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
    updateToolTip();
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
    updateToolTip();
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

void MapArea::updateToolTip()
{
    QString tooltip = "";
    if (!number().isEmpty())
        tooltip += "<b>" + number() + "</b>";

    if (!name().isEmpty() & !tooltip.isEmpty())
        tooltip += "<br>";
    tooltip += name();

    if (!description().isEmpty() & !tooltip.isEmpty())
        tooltip += "<br>";
    tooltip += "<i>" + description() + "</i>";

    tooltip.remove("<i></i>");
    tooltip.replace("\n", "<br>");

    if (!tooltip.isEmpty())
        setToolTip(tooltip);
}

void MapArea::repaint()
{
    setPen(cPenNormal);
    switch (m_areaType)
    {
    case Room:
        setBrush(cBrushNormal);
        break;
    case Aperture:
        setBrush(cBrushHole);
        break;
    case Column:
        setBrush(cBrushFull);
        break;
    }
}

//QString MapArea::info(const QString pattern) const
//{
//    QString str;
//    str = pattern;
//    str = pattern.arg(m_number).arg(m_name).arg(m_description);
//    if ((bool)str.contains("<br>") |
//        (bool)str.contains("<b>") | (bool)str.contains("<i>"))
//        str.replace("/n", "<br>");
//    str = str.trimmed();
//    str.remove("<b></b>");
//    str.remove("<i></i>");
//    str.replace("<br><br>", "<br>");
//    str.replace("/n/n", "/n");
//    while (str.indexOf("<br>") == 0)
//        str.remove(0, 4);
//    while (str.lastIndexOf("<br>") == str.length() - 4)
//        str.remove(str.length() - 4, 4);


//    return str;
//}

MapArea* MapArea::parent() const
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

int MapArea::doorsNumber() const
{
    return m_doors.size();
}

int MapArea::type() const
{
    return Type;
}

quint32 MapArea::uin() const
{
    return m_uin;
}

quint32 MapArea::floorUin() const
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
