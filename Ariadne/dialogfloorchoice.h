#ifndef DIALOGFLOORCHOICE_H
#define DIALOGFLOORCHOICE_H

#include <QComboBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStringListModel>
#include <QVBoxLayout>

class DialogFloorChoice : public QDialog
{
    Q_OBJECT
public:
    explicit DialogFloorChoice(QWidget *parent, QStringListModel *floors,
                               QString labelText);
    int floor() const;

signals:

public slots:

private:
    QStringListModel *m_floors;

    QVBoxLayout *vblMain;
    QLabel *m_label;
    QComboBox *m_cbxFloorChoice;
    QHBoxLayout *hblButtons;
    QPushButton *m_btnOk;
    QPushButton *m_btnCancel;
};

#endif // DIALOGFLOORCHOICE_H
