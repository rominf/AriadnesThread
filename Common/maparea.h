#ifndef MAPAREA_H
#define MAPAREA_H

#include <QGraphicsPolygonItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

class MapArea : public QGraphicsPolygonItem
{
    friend QDataStream &operator<<(QDataStream &output, const MapArea &area);
    friend QDataStream &operator>>(QDataStream &input, MapArea &area);

public:
    enum { Type = UserType + 1 };
    explicit MapArea(int areaNumber, const QPolygonF &polygon);
    int getNumber();
    void setNumber(int i);
    void addDoor(QGraphicsLineItem *door);
    int type() const;
/*protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);*/
private:
    int number;
    QVector<QGraphicsLineItem*> doors;
};

#endif // MAPAREA_H
