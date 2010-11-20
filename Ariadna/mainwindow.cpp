#include "mainwindow.h"
#include <QDebug>
#include "../Common/mapfloor.h"
#include "../Common/map.h"
#include "../Common/maparea.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qApp->setWindowIcon(QIcon(":/AppIcon"));
    qApp->setApplicationName("Проектировщик зданий");
    qApp->setApplicationVersion("1.0");

    // Create central widget
    {
    wgtCentral = new QWidget;
    setCentralWidget(wgtCentral);
    vblwgtCentral = new QVBoxLayout(wgtCentral);
    }
    createActions();
    createMenus();
    createToolBars();
    createDock();

    setState(eFile | eHelp, stEnable_Visible);
    setState(eMode | eView | eFloorNameChange | eAdd | eDock,
             stDisable_Unvisible);
    actMapSave->setEnabled(false);

    showMaximized();

    openedFile = "map.bld";
}

//QDataStream & operator<<(QDataStream & output, const Map &map)
//{
//    output << map.getFloor(0)->font();
//    //floor.setMode(MapFloor::Marking);
//    return output;
//}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
    actMapNew = new QAction(QIcon(":/New"), tr("Создать"), this);
    connect(actMapNew, SIGNAL(triggered()), SLOT(mapNew()));
    actMapNew->setShortcut(Qt::CTRL + Qt::Key_N);
    actMapOpen = new QAction(QIcon(":/Open"), tr("Открыть"), this);
    connect(actMapOpen, SIGNAL(triggered()), SLOT(mapOpen()));
    actMapOpen->setShortcut(Qt::CTRL + Qt::Key_O);
    actMapSave = new QAction(QIcon(":/Save"), tr("Сохранить"), this);
    connect(actMapSave, SIGNAL(triggered()), SLOT(mapSave()));
    actMapSave->setShortcut(Qt::CTRL + Qt::Key_S);


    actSwitchToPlanningMode = new QAction(QIcon(":/PlanningMode"),
                                          tr("Планировка здания"), this);
    actSwitchToPlanningMode->setData(MapFloor::Planning);
    actSwitchToPlanningMode->setShortcut(Qt::CTRL + Qt::Key_1);
    actSwitchToPlanningMode->setCheckable(true);
    actSwitchToPlanningMode->setChecked(true);

    actSwitchToMarkingMode = new QAction(QIcon(":/MarkingMode"),
                                         tr("Разметка здания"), this);
    actSwitchToMarkingMode->setData(MapFloor::Marking);
    actSwitchToMarkingMode->setShortcut(Qt::CTRL + Qt::Key_2);
    actSwitchToMarkingMode->setCheckable(true);

    actgrpMode = new QActionGroup(this);
    actgrpMode->addAction(actSwitchToPlanningMode);
    actgrpMode->addAction(actSwitchToMarkingMode);
    connect(actgrpMode, SIGNAL(triggered(QAction *)),
            SLOT(actgrpTriggered(QAction *)));


    actPreviousFloor = new QAction(QIcon(":/Previous"), tr("Предыдущий этаж"),
                                   this);
    connect(actPreviousFloor, SIGNAL(triggered()), SLOT(previousFloor()));
    actPreviousFloor->setShortcut(Qt::CTRL + Qt::Key_Left);
    actNextFloor = new QAction(QIcon(":/Next"), tr("Следующий этаж"), this);
    connect(actNextFloor, SIGNAL(triggered()), SLOT(nextFloor()));
    actNextFloor->setShortcut(Qt::CTRL + Qt::Key_Right);


    actAddFloor = new QAction(QIcon(":/AddFloor"), tr("Добавить этаж"), this);
    connect(actAddFloor, SIGNAL(triggered()), SLOT(addFloor()));
    actAddFloor->setShortcut(Qt::CTRL + Qt::Key_F);
    actAddWall = new QAction(QIcon(":/AddWall"), tr("Добавить стену"), this);
    connect(actAddWall, SIGNAL(triggered()), SLOT(addWall()));
    actAddWall->setShortcut(Qt::CTRL + Qt::Key_W);
    actAddArea = new QAction(QIcon(":/AddArea"), tr("Добавить помещение"),this);
    connect(actAddArea, SIGNAL(triggered()), SLOT(addArea()));
    actAddArea->setShortcut(Qt::CTRL + Qt::Key_A);
    actAddDoor = new QAction(QIcon(":/AddDoor"), tr("Добавить дверь"), this);
    connect(actAddDoor, SIGNAL(triggered()), SLOT(addDoor()));
    actAddDoor->setShortcut(Qt::CTRL + Qt::Key_D);


    actAbout = new QAction(tr("О программе"), this);
    connect(actAbout, SIGNAL(triggered()), SLOT(about()));
    /*actAboutQT = new QAction(tr("О QT"), this);
    actAboutQT->setStatusTip(tr("Об инструментарий Qt, при помощи которого "
                                "была создана эта программа"));
    connect(actAboutQT, SIGNAL(triggered()), qApp, SLOT(aboutQt()));*/
}

void MainWindow::createMenus()
{
    menuFile = menuBar()->addMenu(tr("Файл"));
    menuFile->addAction(actMapNew);
    menuFile->addAction(actMapOpen);
    menuFile->addAction(actMapSave);

    menuMode = menuBar()->addMenu(tr("Режим"));
    menuMode->addAction(actSwitchToPlanningMode);
    menuMode->addAction(actSwitchToMarkingMode);

    menuAdd = menuBar()->addMenu(tr("Добавить"));
    menuAdd->addAction(actAddFloor);
    menuAdd->addAction(actAddWall);
    menuAdd->addAction(actAddArea);
    menuAdd->addAction(actAddDoor);

    menuHelp = menuBar()->addMenu(tr("Помощь"));
    menuHelp->addAction(actAbout);
    //menuHelp->addAction(actAboutQT);
}

void MainWindow::createToolBars()
{
    tbrFile = addToolBar(tr("Файл"));
    tbrFile->setFloatable(false);
    tbrFile->addAction(actMapNew);
    tbrFile->addAction(actMapOpen);
    tbrFile->addAction(actMapSave);

    tbrMode = addToolBar(tr("Режим"));
    tbrMode->setFloatable(false);
    tbrMode->addAction(actSwitchToPlanningMode);
    tbrMode->addAction(actSwitchToMarkingMode);

    tbrView = addToolBar(tr("Вид"));
    tbrView->setFloatable(false);
    tbrView->addAction(actPreviousFloor);
    cbxFloorSelect = new QComboBox(this);
    connect(cbxFloorSelect,
            SIGNAL(currentIndexChanged(int)), SLOT(setActiveFloor(int)));
    cbxFloorSelect->setEditable(true);
    cbxFloorSelect->setInsertPolicy(QComboBox::NoInsert);
    tbrView->addWidget(cbxFloorSelect);
    tbrView->addAction(actNextFloor);

    /*tbrFloorNameChange = new QToolBar(tr("Изменить название этажа"));
    addToolBar(Qt::BottomToolBarArea, tbrFloorNameChange);
    tbrFloorNameChange->setFloatable(false);
    lblFloorNameChange = new QLabel(tr("Название этажа: "), this);
    tbrFloorNameChange->addWidget(lblFloorNameChange);
    ldtFloorNameChange = new QLineEdit(this);
    connect(ldtFloorNameChange,
            SIGNAL(textChanged(QString)), SLOT(FloorNameChange(QString)));
    tbrFloorNameChange->addWidget(ldtFloorNameChange);*/


    tbrAdd = addToolBar(tr("Добавить"));
    tbrAdd->setFloatable(false);
    tbrAdd->addAction(actAddFloor);
    tbrAdd->addAction(actAddWall);
    tbrAdd->addAction(actAddArea);
    tbrAdd->addAction(actAddDoor);
}

void MainWindow::createDock()
{
    dckwgtInfoEdit = new QDockWidget(tr("Редактирование информации"));
    addDockWidget(Qt::LeftDockWidgetArea, dckwgtInfoEdit);
    dckwgtInfoEdit->setFeatures(QDockWidget::DockWidgetClosable |
                                QDockWidget::DockWidgetMovable);
    wgtInfoEdit = new QWidget;
    dckwgtInfoEdit->setWidget(wgtInfoEdit);
    vbldckwgtInfoEdit = new QVBoxLayout(wgtInfoEdit);
    lblRoomName = new QLabel(tr("Название: "));
    ldtRoomName = new QLineEdit;
    lblRoomDescription = new QLabel(tr("Описание: "));
    ptdtRoomDescription = new QPlainTextEdit;
    vbldckwgtInfoEdit->addWidget(lblRoomName);
    vbldckwgtInfoEdit->addWidget(ldtRoomName);
    vbldckwgtInfoEdit->addWidget(lblRoomDescription);
    vbldckwgtInfoEdit->addWidget(ptdtRoomDescription);
}

void MainWindow::createGraphics()
{
    view = new QGraphicsView();
    view->setMouseTracking(true);
    view->setBackgroundBrush(QBrush(Qt::gray));
    vblwgtCentral->addWidget(view);
    view->show();
    setState(eFile | eMode | eView | eFloorNameChange | eAdd | eHelp,
             stEnable_Visible);
    setState(eDock, stDisable_Unvisible);
    switchMode(MapFloor::Planning);
}

void MainWindow::setState(Elements elem, State st)
{
    bool en = (st == stEnable_Visible);
    bool vs = (st != stDisable_Unvisible);
    if (elem & eFile)
    {
        actMapNew->setEnabled(en);
        actMapNew->setVisible(vs);
        actMapOpen->setEnabled(en);
        actMapOpen->setVisible(vs);
        actMapSave->setEnabled(en);
        actMapSave->setVisible(vs);

        menuFile->menuAction()->setEnabled(en);
        menuFile->menuAction()->setVisible(vs);

        tbrFile->setEnabled(en);
        tbrFile->setVisible(vs);
    }
    if (elem & eMode)
    {
        actSwitchToPlanningMode->setEnabled(en);
        actSwitchToPlanningMode->setVisible(vs);
        actSwitchToMarkingMode->setEnabled(en);
        actSwitchToMarkingMode->setVisible(vs);
        actgrpMode->setEnabled(en);
        actgrpMode->setVisible(vs);

        menuMode->menuAction()->setEnabled(en);
        menuMode->menuAction()->setVisible(vs);

        tbrMode->setEnabled(en);
        tbrMode->setVisible(vs);
    }
    if (elem & eView)
    {
        actPreviousFloor->setEnabled(en);
        actPreviousFloor->setVisible(vs);
        actNextFloor->setEnabled(en);
        actNextFloor->setVisible(vs);

        /*menu->menuAction()->setEnabled(vs);
        menuMode->menuAction()->setVisible(vs);*/

        tbrView->setEnabled(en);
        tbrView->setVisible(vs);
    }
    if (elem & eFloorNameChange)
    {
        /*tbrFloorNameChange->setEnabled(en);
        tbrFloorNameChange->setVisible(vs);*/
    }
    if (elem & eAdd)
    {
        actAddFloor->setEnabled(en);
        actAddFloor->setVisible(vs);
        actAddWall->setEnabled(en);
        actAddWall->setVisible(vs);
        actAddArea->setEnabled(en);
        actAddArea->setVisible(vs);
        actAddDoor->setEnabled(en);
        actAddDoor->setVisible(vs);

        menuAdd->menuAction()->setEnabled(en);
        menuAdd->menuAction()->setVisible(vs);

        tbrAdd->setEnabled(en);
        tbrAdd->setVisible(vs);
    }
    if (elem & eDock)
    {
        dckwgtInfoEdit->setEnabled(en);
        dckwgtInfoEdit->setVisible(vs);
    }
    if (elem & eHelp)
    {
        actAbout->setEnabled(en);
        actAbout->setVisible(vs);

        menuHelp->menuAction()->setEnabled(en);
        menuHelp->menuAction()->setVisible(vs);

        /*tbrAdd->setEnabled(b);
        tbrAdd->setVisible(vs);*/
    }
}

qreal MainWindow::displayPixPerM(qreal pix, qreal mm) const
{
    return (static_cast<qreal>(pix)/mm)*1000;
}

void MainWindow::mapNew()
{
    DialogMapSize* pDialog = new DialogMapSize(this);
    if (pDialog->exec() == QDialog::Accepted)
    {
        map = new Map(cPixPerRealM, displayPixPerM(width(), widthMM()),
                      pDialog->getMSizeX()*cPixPerRealM,
                      pDialog->getMSizeY()*cPixPerRealM, this);
        createGraphics();
        addFloor();
        view->fitInView(view->sceneRect(), Qt::KeepAspectRatio);
    }
}

void MainWindow::mapOpen()
{
    QString str = QFileDialog::getOpenFileName(this, tr("Выбирете карту"), "",
                                               tr("Файл карты здания(*.bld)"));
    if (!str.isNull())
    {
        QFile f(str);
        if (f.open(QIODevice::ReadOnly))
        {
            QDataStream s(&f);
            s.setVersion(QDataStream::Qt_4_7);
            qint32 i;
            s >> i;
            if (i != cMagicNumber)
            {
                QMessageBox::warning(this, tr("Ошибка при чтении файла"),
                                           tr("Файл, который Вы собираетесь "
                                              "открыть, не является файлом "
                                              "карты или поврежден."));
                return;
            }
            map = new Map(0, 0, 0, 0, this);
            s >> *map >> curFloor;
            map->setPixPerDisplayM(displayPixPerM(width(), widthMM()));
            createGraphics();
            setActiveFloor(curFloor);
            view->fitInView(view->sceneRect(), Qt::KeepAspectRatio);
            for (int j = 0; j != map->floorsNumber(); ++j)
                cbxFloorSelect->addItem(map->getFloor(j)->getName());
            openedFile = f.fileName();
        }
        else
    {
        QMessageBox::warning(this, tr("Ошибка при чтении файла"),
                                   tr("Файл, который Вы собираетесь "
                                      "открыть, не является файлом "
                                      "карты или поврежден."));
    }
    }
}

void MainWindow::mapSave()
{
    QFile f(openedFile);
    if (f.open(QIODevice::WriteOnly))
    {
        QDataStream s(&f);
        s.setVersion(QDataStream::Qt_4_7);
        s << cMagicNumber << *map << curFloor;
    }
}

void MainWindow::previousFloor()
{
    setActiveFloor(curFloor - 1);
}

void MainWindow::nextFloor()
{
    setActiveFloor(curFloor + 1);
}

void MainWindow::zoomIn(qreal r)
{
    view->scale(r, r);
}

void MainWindow::zoomOut(qreal r)
{
    view->scale(r, r);
}

void MainWindow::FloorNameChange(const QString &floorName)
{
    map->getFloor(curFloor)->setName(floorName);
    cbxFloorSelect->setItemText(curFloor, floorName);
}

void MainWindow::addFloor()
{
    bool bOk;
    QString s = QInputDialog::getText(this, tr("Создание этажа"), tr("Введите,"
                                      " пожалуйста, название этажа:"),
                                      QLineEdit::Normal, tr("Новый"), &bOk);
    if (!bOk | s.isEmpty())
        s = tr("Без названия (" +
               QString::number(map->floorsNumber()).toUtf8() + ")");

    map->insertFloor(map->floorsNumber(), s);
    setActiveFloor(map->floorsNumber()-1);
    //QString name = map->getFloor(curFloor)->getName();
    cbxFloorSelect->addItem(s);
    cbxFloorSelect->setCurrentIndex(curFloor);
    //ldtFloorNameChange->setText(name);
    switchMode(MapFloor::FloorAdd);
}

void MainWindow::addWall()
{
    switchMode(MapFloor::WallAdd);
}

void MainWindow::addArea()
{
    switchMode(MapFloor::AreaAdd);
}

void MainWindow::addDoor()
{
    switchMode(MapFloor::DoorAdd);
}

void MainWindow::actgrpTriggered(QAction *qAct)
{
    emit switchMode(MapFloor::Mode(qAct->data().toInt()));
}

void MainWindow::switchMode(MapFloor::Mode m)
{
    switch (m)
    {
    case MapFloor::Idle:
        {
            break;
        }
    case MapFloor::Planning:
        {
            setState(eFile | eMode | eView | eFloorNameChange | eAdd | eHelp,
                     stEnable_Visible);
            setState(eDock, stDisable_Unvisible);
            setCursor(Qt::ArrowCursor);
            break;
        }
    case MapFloor::FloorAdd:
    case MapFloor::WallAdd:
    case MapFloor::AreaAdd:
    case MapFloor::DoorAdd:
        {
            setState(eFile | eMode | eView | eFloorNameChange | eAdd | eHelp,
                     stDisable_Visible);
            setState(eDock, stDisable_Unvisible);
            setCursor(Qt::CrossCursor);
            break;
        }
    case MapFloor::Marking:
        {
            setState(eFile | eMode | eView | eFloorNameChange | eDock | eHelp,
                     stEnable_Visible);
            setState(eAdd, stDisable_Unvisible);
            break;
        }
    }
    if (map->floorsNumber() != 0)
    {
        map->getFloor(curFloor)->setMode(mode = m);
    }
}

/*MapFloor::Mode MainWindow::getMode()
{
    return mode;
}*/

void MainWindow::setActiveFloor(int i)
{
    if ((i >= 0)&(i < map->floorsNumber()))
    {
        curFloor = i;
        cbxFloorSelect->setCurrentIndex(i);
        //ldtFloorNameChange->setText(map->getFloor(i)->getName());
        view->setScene(map->getFloor(i));
        connect(map->getFloor(i), SIGNAL(modeChanged(MapFloor::Mode)),
                SLOT(switchMode(MapFloor::Mode)));
    }
}

void MainWindow::about()
{
    /*QString *s = new QString("<b>"+qApp->applicationName()+" "
                            +qApp->applicationVersion()+" (альфа)</b><br>"+
                            "\""+qApp->applicationName()+
                            "\" является частью программного комлекса"
                            " \"Карты зданий\". Эта программа создает карты для "
                            "последующего их отображения в программе "
                            "\"Навигатор\".<br>&copy;Роман Инфлянскас. "
                            "2010 г.");*/

    QMessageBox::about(this, tr("О программе"),
                       tr("<b>Проектировщик зданий 1.0 (альфа)</b><br>"
                       "\"Проектировщик зданий\" является частью программного комлекса"
                       " \"Карты зданий\". Эта программа создает карты для "
                       "последующего их отображения в программе "
                       "\"Навигатор\".<br>&copy;Роман Инфлянскас. "
                       "2010 г."));

}
