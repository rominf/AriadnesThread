#include "dialogfloorchoice.h"

DialogFloorChoice::DialogFloorChoice(QWidget *parent, QStringListModel *floors,
                                     int floorSelected, QString labelText):
    QDialog(parent), m_floors(floors)
{
    vblMain = new QVBoxLayout(this);
    m_label = new QLabel(labelText);
    vblMain->addWidget(m_label);
    m_cbxFloorChoice = new QComboBox();
    m_cbxFloorChoice->setEditable(true);
    m_cbxFloorChoice->setInsertPolicy(QComboBox::NoInsert);
    m_cbxFloorChoice->setModel(floors);
    m_cbxFloorChoice->setCurrentIndex(floorSelected);
    vblMain->addWidget(m_cbxFloorChoice);

    hblButtons = new QHBoxLayout();
    hblButtons->addStretch();
    m_btnOk = new QPushButton(tr("&OK"));
    connect(m_btnOk, SIGNAL(clicked()), SLOT(accept()));
    hblButtons->addWidget(m_btnOk);
    m_btnCancel = new QPushButton(tr("&Отмена"));
    connect(m_btnCancel, SIGNAL(clicked()), SLOT(reject()));
    hblButtons->addWidget(m_btnCancel);
    vblMain->addLayout(hblButtons);
}

int DialogFloorChoice::floor() const
{
    return m_cbxFloorChoice->currentIndex();
}
