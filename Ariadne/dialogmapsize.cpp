#include "dialogmapsize.h"

DialogMapSize::DialogMapSize(QWidget *parent) :
    QDialog(parent)
{
    m_sbxMSizeX = new QDoubleSpinBox(this);
    m_sbxMSizeX->setRange(cMinMSizeX, cMaxMSizeX);
    m_sbxMSizeX->setValue(cMSizeX);
    m_sbxMSizeX->setSuffix(tr(" м"));
    m_sbxMSizeX->setWrapping(false);
    m_sbxMSizeY = new QDoubleSpinBox(this);
    m_sbxMSizeY->setRange(cMinMSizeY, cMaxMSizeY);
    m_sbxMSizeY->setValue(cMSizeY);
    m_sbxMSizeY->setSuffix(tr(" м"));
    m_sbxMSizeY->setWrapping(false);

    m_lblMSizeX = new QLabel(tr("&Длина"));
    m_lblMSizeY = new QLabel(tr("&Ширина"));

    m_lblMSizeX->setBuddy(m_sbxMSizeX);
    m_lblMSizeY->setBuddy(m_sbxMSizeY);

    m_btnOk = new QPushButton(tr("&OK"));
    m_btnCancel = new QPushButton(tr("&Отмена"));

    connect(m_btnOk, SIGNAL(clicked()), SLOT(accept()));
    connect(m_btnCancel, SIGNAL(clicked()), SLOT(reject()));

    //Layout setup
    QGridLayout* ptopLayout = new QGridLayout;
    ptopLayout->addWidget(m_lblMSizeX, 0, 0);
    ptopLayout->addWidget(m_lblMSizeY, 1, 0);
    ptopLayout->addWidget(m_sbxMSizeX, 0, 1);
    ptopLayout->addWidget(m_sbxMSizeY, 1, 1);
    ptopLayout->addWidget(m_btnOk, 2,0);
    ptopLayout->addWidget(m_btnCancel, 2, 1);
    setLayout(ptopLayout);
}

qreal DialogMapSize::MSizeX()
{
    return m_sbxMSizeX->value();
}

qreal DialogMapSize::MSizeY()
{
    return m_sbxMSizeY->value();
}
