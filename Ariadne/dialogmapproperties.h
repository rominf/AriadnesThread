#ifndef DIALOGMAPPROPERTIES_H
#define DIALOGMAPPROPERTIES_H

#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTemporaryFile>
#include <QVBoxLayout>
#include <QWebView>

#include "../Common/global.h"
#include "../Common/map.h"

class DialogMapProperties : public QDialog
{
    Q_OBJECT
public:
    explicit DialogMapProperties(QWidget *parent, Map::Properties *properties);
    Map::Properties* properties() const;
signals:

public slots:

private slots:
    void focusChanged(QWidget *old, QWidget *now);
    void mapUpdating();

private:
    QString m_mapTemplate;
    QLineEdit *m_ldtName;
    QPlainTextEdit *m_ptdtDescription;
    QLineEdit *m_ldtCoordinates;
    QWebView *m_webMap;
    QDialogButtonBox *m_buttonBox;
};

#endif // DIALOGMAPPROPERTIES_H
