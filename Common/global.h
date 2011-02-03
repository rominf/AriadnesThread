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
    static const QColor colorSelected;
    static const QColor colorStart;
    static const QColor colorFinish;
    static const QPen pen;
//    static const QPen penStart;
//    static const QBrush brushStart;
//    static const QPen penFinish;
//    static const QBrush brushFinish;
signals:

public slots:

};

#endif // GLOBAL_H
