#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QDataStream>
#include "mapfloor.h"

class Map : public QObject
{
    Q_OBJECT

    friend QDataStream & operator<<(QDataStream &output, const Map &map);
    friend QDataStream & operator>>(QDataStream &input, Map &map);
public:
    explicit Map(qreal mapPixPerRealM, qreal mapPixPerDisplayM,
                 qreal mapPixSizeX, qreal mapPixSizeY, QObject *parent);
    void setPixPerDisplayM(qreal r);
    int floorsNumber();
    void insertFloor(int i, QString floorName = "");
    void removeFloor(int i);
    MapFloor* getFloor(int i) const;

signals:

public slots:

private:
    QVector<MapFloor*> floors;
    qreal pixPerRealM;
    qreal pixPerDisplayM;
    qreal realMPerDisplayM;
    qreal pixSizeX;
    qreal pixSizeY;

    qreal convertPixToDisplayM(qreal r) const;
    qreal convertPixToRealM(qreal r) const;
    qreal convertDisplayMToPix(qreal r) const;
    qreal convertRealMToPix(qreal r) const;
};

#endif // MAP_H
