#include <QApplication>
#include <QtSvg>
#include <QIcon>
#include <QTextCodec>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
#endif
    QString locale = "en";//QLocale::system().name();

	// Support of Qt dialogs translation
    QTranslator* translator_qt = new QTranslator;
	translator_qt->load(":/tr/qt_" + locale);
    a.installTranslator(translator_qt);

	QTranslator* translator = new QTranslator;
	translator->load(":/tr/" + locale);
    a.installTranslator(translator);

//    a.setWindowIcon(QIcon(":/Ariadne.svg"));

    MainWindow w;
	w.show();
//	w.showMaximized();

    return a.exec();
}
