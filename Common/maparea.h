#ifndef MAPAREA_H
#define MAPAREA_H

#include <QBrush>
#include <QGraphicsPolygonItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

class MapArea : public QGraphicsPolygonItem
{
    friend QDataStream &operator<<(QDataStream &output, const MapArea &area);
    friend QDataStream &operator>>(QDataStream &input, MapArea &area);

public:
    enum {Type = QGraphicsItem::UserType + 1};
    explicit MapArea(const QPolygonF &polygon);
    MapArea* parent();
//    void setParent(MapArea *parent);
//    int number();
//    void setNumber(int i);
    void addArea(MapArea *area);
    MapArea* area(int i) const;
    void deleteArea(MapArea* area);
    int areasNumber();
    void addDoor(QGraphicsLineItem *door);
    int type() const;
/*protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);*/
private:
//    int m_number;
    MapArea *m_parent;
    QVector<MapArea*> m_areas;
    QVector<QGraphicsLineItem*> m_doors;
};

#endif // MAPAREA_H
