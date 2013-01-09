#ifndef DIALOGMAPSIZE_H
#define DIALOGMAPSIZE_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>

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
    QDialogButtonBox *m_buttonBox;
//    QPushButton *m_btnOk;
//    QPushButton *m_btnCancel;

    static const qreal cMSizeX = 500;
    static const qreal cMSizeY = 300;
    static const qreal cMinMSizeX = 1;
    static const qreal cMaxMSizeX = 10000;
    static const qreal cMinMSizeY = 1;
    static const qreal cMaxMSizeY = 10000;
};

#endif // DIALOGMAPSIZE_H
