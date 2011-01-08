#ifndef MAPAREA_H
#define MAPAREA_H

#include <QBrush>
#include <QFont>
#include <QFontMetricsF>
#include <QGraphicsPolygonItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>
#include <QTextOption>
#include "mapdoor.h"

class MapDoor;
class MapArea : public QGraphicsPolygonItem
{
    friend QDataStream &operator<<(QDataStream &output, const MapArea &area);
    friend QDataStream &operator>>(QDataStream &input, MapArea &area);

public:
    explicit MapArea(const QPolygonF &polygon);
    explicit MapArea(const MapArea &area, const QString &before,
                     const QString &after);

    enum {Type = QGraphicsItem::UserType + 1};
    enum CreateError
    {
        ceNone,
        ceIntersection,
        ceAreaExist
    };
    QString name();
    void setName(const QString &name);
    void setNameVisible(bool show = true);
    bool isNameVisible();
    QString description();
    void setDescription(const QString &description);
    MapArea* parent();
//    void setParent(MapArea *parent);
    void addArea(MapArea *area);
    MapArea* area(int i) const;
    void deleteArea(MapArea* area);
    int areasNumber();
    void addDoor(MapDoor *door);
    MapDoor* door(int i) const;
    void deleteDoor(MapDoor* door);
    int doorsNumber();
    int type() const;
    quint32 uin();
private:
    static const int cFontSize;
    QGraphicsTextItem *m_name;
    QString m_description;
    MapArea *m_parent;
    QVector<MapArea*> m_areas;
    QVector<MapDoor*> m_doors;
    quint32 m_uin;

    static quint32 m_count; // Counter for making uins
};

#endif // MAPAREA_H
