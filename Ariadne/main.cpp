#include <QtGui/QApplication>
#include <QtSvg>
#include <QIcon>
#include <QTextCodec>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QString locale = "en";//QLocale::system().name();

	// Support of Qt dialogs translation
    QTranslator* translator_qt = new QTranslator;
	translator_qt->load(":/tr/qt_" + locale);
    a.installTranslator(translator_qt);

	QTranslator* translator = new QTranslator;
	translator->load(":/tr/" + locale);
    a.installTranslator(translator);

    a.setWindowIcon(QIcon(":/Ariadne.svg"));

    MainWindow w;
    w.show();

    return a.exec();
}
