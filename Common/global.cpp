#include "global.h"

const QColor Global::colorSelected = QColor(255, 250, 205, 255);
const QColor Global::colorStart = QColor(224, 255, 224, 255);
//const QColor Global::colorStart = QColor(196, 253, 196, 255);
//const QColor Global::colorStart = QColor(240, 255, 240, 255);
const QColor Global::colorFinish = QColor(255, 228, 225, 255);
const QPen Global::pen = QPen(Qt::black);
//const QColor Global::colorStart = QColor(0, 128, 0, 128);
//const QColor Global::colorFinish = QColor(255, 0, 0, 128);

Global::Global(QObject *parent) :
    QObject(parent)
{
}
