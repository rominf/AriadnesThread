#include "dialogmapsize.h"

DialogMapSize::DialogMapSize(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Overall dimensions"));
    m_sbxMSizeX = new QDoubleSpinBox(this);
    m_sbxMSizeX->setRange(cMinMSizeX, cMaxMSizeX);
    m_sbxMSizeX->setValue(cMSizeX);
    m_sbxMSizeX->setSuffix(tr(" m"));
    m_sbxMSizeX->setWrapping(false);
    m_sbxMSizeY = new QDoubleSpinBox(this);
    m_sbxMSizeY->setRange(cMinMSizeY, cMaxMSizeY);
    m_sbxMSizeY->setValue(cMSizeY);
    m_sbxMSizeY->setSuffix(tr(" m"));
    m_sbxMSizeY->setWrapping(false);

    m_lblMSizeX = new QLabel(tr("&Length"));
    m_lblMSizeY = new QLabel(tr("&Width"));

    m_lblMSizeX->setBuddy(m_sbxMSizeX);
    m_lblMSizeY->setBuddy(m_sbxMSizeY);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                       QDialogButtonBox::Cancel);
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    //Layouts setup
    QVBoxLayout* vbl = new QVBoxLayout;

    QGridLayout* grdl = new QGridLayout;
    grdl->addWidget(m_lblMSizeX, 0, 0);
    grdl->addWidget(m_lblMSizeY, 1, 0);
    grdl->addWidget(m_sbxMSizeX, 0, 1);
    grdl->addWidget(m_sbxMSizeY, 1, 1);
    vbl->addLayout(grdl);

    vbl->addWidget(m_buttonBox);
    setLayout(vbl);
}

qreal DialogMapSize::MSizeX()
{
    return m_sbxMSizeX->value();
}

qreal DialogMapSize::MSizeY()
{
    return m_sbxMSizeY->value();
}
