#include <QtGui/QApplication>
#include <QtSvg>
#include <QTextCodec>
#include <QTranslator>
//#include <QGtkStyle>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
//    QApplication::setStyle(new QGtkStyle);
    QApplication a(argc, argv);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QString locale = QLocale::system().name();
    QTranslator* translator = new QTranslator;
    translator->load(QString("qt_") + locale);
    a.installTranslator(translator);
    a.setWindowIcon(QIcon(":/Ariadne"));

    MainWindow w(MainWindow::pViewer);
    w.show();

    return a.exec();
}
