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

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                       QDialogButtonBox::Cancel);
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    vblMain->addWidget(m_buttonBox);
}

int DialogFloorChoice::floor() const
{
    return m_cbxFloorChoice->currentIndex();
}
