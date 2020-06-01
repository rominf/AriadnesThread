#ifndef GLOBAL_H
#define GLOBAL_H

#include <QPen>
#include <QBrush>
#include <QObject>

#define EDITOR

class Global : public QObject
{
    Q_OBJECT
public:
    explicit Global(QObject *parent = 0);
    static constexpr qreal cSpeedMPerMin = 85;
    static constexpr qreal cTimeStairsDown = 0.25;
    static constexpr qreal cTimeStairsUp = 0.33;
    static constexpr qreal cTimeLift = 0.08;
    static constexpr qreal cTimeLiftWaiting = 1;
    static const QColor colorSelected;
    static const QColor colorStart;
    static const QColor colorFinish;
    static const QPen pen;
signals:

public slots:

};

#endif // GLOBAL_H
