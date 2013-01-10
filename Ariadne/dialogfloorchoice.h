#ifndef DIALOGFLOORCHOICE_H
#define DIALOGFLOORCHOICE_H

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QStringListModel>
#include <QVBoxLayout>

class DialogFloorChoice : public QDialog
{
    Q_OBJECT
public:
    explicit DialogFloorChoice(QWidget *parent, QStringListModel *floors,
                               int floorSelected, QString labelText);
    int floor() const;

signals:

public slots:

private:
    QStringListModel *m_floors;

    QVBoxLayout *vblMain;
    QLabel *m_label;
    QComboBox *m_cbxFloorChoice;
    QDialogButtonBox *m_buttonBox;
};

#endif // DIALOGFLOORCHOICE_H
