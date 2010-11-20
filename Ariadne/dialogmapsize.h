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
    qreal getMSizeX();
    qreal getMSizeY();

signals:

public slots:

private:
    QDoubleSpinBox *sbxMSizeX;
    QDoubleSpinBox *sbxMSizeY;
    QLabel *lblMSizeX;
    QLabel *lblMSizeY;
    QPushButton *btnOk;
    QPushButton *btnCancel;

    static const qreal cMSizeX = 300;
    static const qreal cMSizeY = 300;
    static const qreal cMinMSizeX = 10;
    static const qreal cMaxMSizeX = 10000;
    static const qreal cMinMSizeY = 10;
    static const qreal cMaxMSizeY = 10000;
};

#endif // DIALOGMAPSIZE_H
