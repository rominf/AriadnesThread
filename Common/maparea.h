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
//    friend bool operator<(const MapArea *a, const MapArea *b);

public:
    explicit MapArea(const QPolygonF &polygon, const quint32 floorUin);
    explicit MapArea(const MapArea &area, const quint32 floorUin,
                     const QString &before, const QString &after);

    enum {Type = QGraphicsItem::UserType + 1};
    enum CreationError
    {
        ceNone,
        ceIntersection,
        ceAreaExist
    };
    static const QPen cPenNormal;
    static const QPen cPenSelected;
    static const QBrush cBrushNormal;
    static const QBrush cBrushSelected;
//    bool operator<(const MapArea *b) const;
    QString number();
    void setNumber(const QString &number);
//    bool isNumberVisible();
//    void setNumberVisible(bool show = true);
    QString name();
    void setName(const QString &name);
//    bool isNameVisible();
//    void setNameVisible(bool show = true);
    QString description();
    void setDescription(const QString &description);
    QString inscription();
    void setInscription(const QString &inscription);
    void updateToolTip();
//    QString info(const QString pattern) const;
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
    quint32 floorUin();
private:
    static const int cFontSize;
    static const QString cFontFamily;

    QGraphicsTextItem *m_inscription;
    QString m_number;
    QString m_name;
    QString m_description;
    MapArea *m_parent;
    QVector<MapArea*> m_areas;
    QVector<MapDoor*> m_doors;
    quint32 m_floorUin;
    quint32 m_uin;

    static quint32 m_count; // Counter for making uins

//    void updateTitle();
};

#endif // MAPAREA_H
