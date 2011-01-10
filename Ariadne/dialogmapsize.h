#ifndef DIALOGMAPSIZE_H
#define DIALOGMAPSIZE_H

#include <QDialog>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

class DialogMapSize : public QDialog
{
    Q_OBJECT
public:
    explicit DialogMapSize(QWidget *parent = 0);
    qreal MSizeX();
    qreal MSizeY();

signals:

public slots:

private:
    QDoubleSpinBox *m_sbxMSizeX;
    QDoubleSpinBox *m_sbxMSizeY;
    QLabel *m_lblMSizeX;
    QLabel *m_lblMSizeY;
    QPushButton *m_btnOk;
    QPushButton *m_btnCancel;

    static const qreal cMSizeX = 247;
    static const qreal cMSizeY = 139.6;
    static const qreal cMinMSizeX = 10;
    static const qreal cMaxMSizeX = 10000;
    static const qreal cMinMSizeY = 10;
    static const qreal cMaxMSizeY = 10000;
};

#endif // DIALOGMAPSIZE_H
