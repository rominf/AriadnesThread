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
    enum AreaType
    {
        Normal,
        Hole,
        Full
    };
    static const QPen cPenNormal;
    static const QPen cPenSelected;
    static const QBrush cBrushNormal;
    static const QBrush cBrushHole;
    static const QBrush cBrushFull;
    static const QBrush cBrushSelected;
    MapArea::AreaType areaType();
    void setAreaType(MapArea::AreaType type, bool update = true);
    QString number();
    void setNumber(const QString &number);
    QString name();
    void setName(const QString &name);
    QString description();
    void setDescription(const QString &description);
    QString inscription();
    void setInscription(const QString &inscription);
    void updateToolTip();
    void repaint();
    MapArea* parent() const;
    void addArea(MapArea *area);
    MapArea* area(int i) const;
    void deleteArea(MapArea* area);
    int areasNumber();
    void addDoor(MapDoor *door);
    MapDoor* door(int i) const;
    void deleteDoor(MapDoor* door);
    int doorsNumber() const;
    int type() const;
    quint32 uin() const;
    quint32 floorUin() const;
private:
    static const int cFontSize;
    static const QString cFontFamily;

    MapArea::AreaType m_areaType;
    QString m_number;
    QString m_name;
    QString m_description;
    QGraphicsTextItem *m_inscription;
    MapArea *m_parent;
    QVector<MapArea*> m_areas;
    QVector<MapDoor*> m_doors;
    quint32 m_floorUin;
    quint32 m_uin;

    static quint32 m_count; // Counter for making uins
};

#endif // MAPAREA_H
