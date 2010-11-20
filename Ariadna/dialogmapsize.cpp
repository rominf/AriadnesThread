#include "dialogmapsize.h"

DialogMapSize::DialogMapSize(QWidget *parent) :
    QDialog(parent)
{
    sbxMSizeX = new QDoubleSpinBox(this);
    sbxMSizeX->setRange(cMinMSizeX, cMaxMSizeX);
    sbxMSizeX->setValue(cMSizeX);
    sbxMSizeX->setSuffix(tr(" м"));
    sbxMSizeX->setWrapping(false);
    sbxMSizeY = new QDoubleSpinBox(this);
    sbxMSizeY->setRange(cMinMSizeY, cMaxMSizeY);
    sbxMSizeY->setValue(cMSizeY);
    sbxMSizeY->setSuffix(tr(" м"));
    sbxMSizeY->setWrapping(false);

    lblMSizeX = new QLabel(tr("&Длина"));
    lblMSizeY = new QLabel(tr("&Ширина"));

    lblMSizeX->setBuddy(sbxMSizeX);
    lblMSizeY->setBuddy(sbxMSizeY);

    btnOk = new QPushButton(tr("&OK"));
    btnCancel = new QPushButton(tr("&Отмена"));

    connect(btnOk, SIGNAL(clicked()), SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), SLOT(reject()));

    //Layout setup
    QGridLayout* ptopLayout = new QGridLayout;
    ptopLayout->addWidget(lblMSizeX, 0, 0);
    ptopLayout->addWidget(lblMSizeY, 1, 0);
    ptopLayout->addWidget(sbxMSizeX, 0, 1);
    ptopLayout->addWidget(sbxMSizeY, 1, 1);
    ptopLayout->addWidget(btnOk, 2,0);
    ptopLayout->addWidget(btnCancel, 2, 1);
    setLayout(ptopLayout);
}

qreal DialogMapSize::getMSizeX()
{
    return sbxMSizeX->value();
}

qreal DialogMapSize::getMSizeY()
{
    return sbxMSizeY->value();
}
