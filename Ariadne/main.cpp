#include <QtGui/QApplication>
#include <QtSvg>
#include <QIcon>
#include <QTextCodec>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Support of translation
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QString locale = QLocale::system().name();
    QTranslator* translator = new QTranslator;
    translator->load(QString("qt_") + locale);
    a.installTranslator(translator);

    a.setWindowIcon(QIcon(":/Ariadne.svg"));

    MainWindow w;
    w.show();

    return a.exec();
}
