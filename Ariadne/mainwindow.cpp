#include "mainwindow.h"
#include <QDebug>
#include "../Common/mapfloor.h"
#include "../Common/map.h"
#include "../Common/maparea.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qApp->setWindowIcon(QIcon(":/AppIcon"));
    qApp->setApplicationName(tr("Ариадна"));
    qApp->setApplicationVersion("1.0");
    setWindowTitle(qApp->applicationName());

    map = 0;
    view = 0;
    curFloor = -1;
    modelFloorsList = new QStringListModel(this);
    selectionFloorsList = new QItemSelectionModel(modelFloorsList, this);

    // Create central widget
    {
        wgtCentral = new QWidget;
        setCentralWidget(wgtCentral);
        vblwgtCentral = new QVBoxLayout(wgtCentral);
    }
    createActions();
    createMenus();
    createToolBars();
    createPanelFloorsManagement();
    createPanelAreasMarking();

    setState(eFile | eHelp, stTrue, stTrue);
    setState(eView | eAdd | ePanels | eFloorsManagement | eAreasMarking,
             stFalse, stFalse);
    actMapSave->setEnabled(false);

    showMaximized();

    openedFile = "map.bld";
}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
    actMapNew = new QAction(QIcon(":/New"), tr("Создать"), this);
    connect(actMapNew, SIGNAL(triggered()), SLOT(mapNew()));
    actMapNew->setShortcut(QKeySequence::New);
    actMapOpen = new QAction(QIcon(":/Open"), tr("Открыть"), this);
    connect(actMapOpen, SIGNAL(triggered()), SLOT(mapOpen()));
    actMapOpen->setShortcut(QKeySequence::Open);
    actMapSave = new QAction(QIcon(":/Save"), tr("Сохранить"), this);
    connect(actMapSave, SIGNAL(triggered()), SLOT(mapSave()));
    actMapSave->setShortcut(QKeySequence::Save);


//    actSwitchToPlanningMode = new QAction(QIcon(":/PlanningMode"),
//                                          tr("Планировка здания"), this);
//    actSwitchToPlanningMode->setData(MapFloor::Planning);
//    actSwitchToPlanningMode->setShortcut(Qt::CTRL + Qt::Key_1);
//    actSwitchToPlanningMode->setCheckable(true);
//    actSwitchToPlanningMode->setChecked(true);

    actPanelFloorsManagement = new QAction(QIcon(":/FloorsManagement"),
                                         tr("Управление этажами"), this);
    actPanelFloorsManagement->setData(eFloorsManagement);
    actPanelFloorsManagement->setShortcut(Qt::CTRL + Qt::Key_1);
    actPanelFloorsManagement->setCheckable(true);

    actPanelAreasMarking = new QAction(QIcon(":/AreasMarking"),
                                         tr("Разметка помещений"), this);
    actPanelAreasMarking->setData(eAreasMarking);
    actPanelAreasMarking->setShortcut(Qt::CTRL + Qt::Key_2);
    actPanelAreasMarking->setCheckable(true);

    actgrpPanels = new QActionGroup(this);
    actgrpPanels->setExclusive(false);
    actgrpPanels->addAction(actPanelAreasMarking);
    actgrpPanels->addAction(actPanelFloorsManagement);
    connect(actgrpPanels, SIGNAL(triggered(QAction *)),
            SLOT(actgrpPanelsTriggered(QAction *)));


    actFloorDown = new QAction(QIcon(":/FloorDown"), tr("Этаж ниже"), this);
    connect(actFloorDown, SIGNAL(triggered()), SLOT(floorDown()));
    actFloorDown->setShortcut(Qt::CTRL + Qt::Key_Right);

    actFloorUp = new QAction(QIcon(":/FloorUp"), tr("Этаж выше"),
                                   this);
    connect(actFloorUp, SIGNAL(triggered()), SLOT(floorUp()));
    actFloorUp->setShortcut(Qt::CTRL + Qt::Key_Left);


    actZoomOut= new QAction(QIcon(":/ZoomOut"), tr("Уменьшить"), this);
    connect(actZoomOut, SIGNAL(triggered()), SLOT(zoomOut()));
    actZoomOut->setShortcut(/*Qt::CTRL + */Qt::Key_Minus);

    actZoomIn = new QAction(QIcon(":/ZoomIn"), tr("Увеличить"), this);
    connect(actZoomIn, SIGNAL(triggered()), SLOT(zoomIn()));
    actZoomIn->setShortcut(/*Qt::CTRL + */Qt::Key_Plus);

    actZoomFit = new QAction(QIcon(":/ZoomFit"), tr("Вместить"), this);
    connect(actZoomFit, SIGNAL(triggered()), SLOT(zoomFit()));
    actZoomFit->setShortcut(/*Qt::CTRL + */Qt::Key_Asterisk);

    actAddBase = new QAction(QIcon(":/AddBase"), tr("Добавить подложку"), this);
    connect(actAddBase, SIGNAL(triggered()), SLOT(addBase()));
    actAddBase->setShortcut(Qt::CTRL + Qt::Key_B);

    actHideBase = new QAction(QIcon(":/HideBase"), tr("Скрыть подложку"), this);
    connect(actHideBase, SIGNAL(triggered(bool)), SLOT(hideBase(bool)));
    actHideBase->setCheckable(true);
//    actBaseVisible->setShortcut(Qt::CTRL + Qt::Key_B);

    actMagnetToExtensions = new QAction(
            QIcon(":/MagnetToExtensions"),
            tr("Притягиваться к продолжениям линий"), this);
    connect(actMagnetToExtensions, SIGNAL(triggered(bool)),
            SLOT(magnetToExtensions(bool)));
    actMagnetToExtensions->setCheckable(true);


    actAddWall = new QAction(QIcon(":/AddWall"), tr("Добавить стену"), this);
    connect(actAddWall, SIGNAL(triggered()), SLOT(addWall()));
    actAddWall->setShortcut(Qt::CTRL + Qt::Key_W);

    actAddArea = new QAction(QIcon(":/AddArea"), tr("Добавить помещение"),this);
    connect(actAddArea, SIGNAL(triggered()), SLOT(addArea()));
    actAddArea->setShortcut(Qt::CTRL + Qt::Key_A);

    actAddDoor = new QAction(QIcon(":/AddDoor"), tr("Добавить дверь"), this);
    connect(actAddDoor, SIGNAL(triggered()), SLOT(addDoor()));
    actAddDoor->setShortcut(Qt::CTRL + Qt::Key_D);


    actFloorAdd = new QAction(QIcon(":/FloorAdd"), tr("Добавить этаж"), this);
    connect(actFloorAdd, SIGNAL(triggered()), SLOT(floorAdd()));
    actFloorAdd->setShortcut(Qt::CTRL + Qt::Key_F);

    actFloorDelete = new QAction(QIcon(":/FloorDelete"),
                                 tr("Удалить этаж"), this);
    connect(actFloorDelete, SIGNAL(triggered()), SLOT(floorDelete()));
    actFloorDelete->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_F);

    actFloorMoveDown = new QAction(QIcon(":/FloorDown"),
                                 tr("Переместить вниз"), this);
    connect(actFloorMoveDown, SIGNAL(triggered()), SLOT(floorMoveDown()));
//    actFloorMoveDown->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_F);

    actFloorMoveUp = new QAction(QIcon(":/FloorUp"),
                                 tr("Переместить вверх"), this);
    connect(actFloorMoveUp, SIGNAL(triggered()), SLOT(floorMoveUp()));
//    actFloorMoveUp->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_F);


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

    menuPanels = menuBar()->addMenu(tr("Панели"));
    menuPanels->addAction(actPanelFloorsManagement);
    menuPanels->addAction(actPanelAreasMarking);

    menuAdd = menuBar()->addMenu(tr("Добавить"));
//    menuAdd->addAction(actFloorAdd);
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

    tbrPanels = new QToolBar(tr("Панели"));
    addToolBar(Qt::LeftToolBarArea, tbrPanels);
    tbrPanels->setFloatable(false);
    tbrPanels->addAction(actPanelFloorsManagement);
    tbrPanels->addAction(actPanelAreasMarking);

    tbrView = addToolBar(tr("Вид"));
    tbrView->setFloatable(false);
    tbrView->addAction(actFloorDown);
    cbxFloorSelect = new QComboBox(this);
    cbxFloorSelect->setModel(modelFloorsList);
    connect(cbxFloorSelect,
            SIGNAL(currentIndexChanged(int)), SLOT(setActiveFloor(int)));
    cbxFloorSelect->setEditable(true);
    cbxFloorSelect->setInsertPolicy(QComboBox::NoInsert);
    tbrView->addWidget(cbxFloorSelect);
    tbrView->addAction(actFloorUp);
    tbrView->addAction(actZoomOut);
    tbrView->addAction(actZoomIn);
    tbrView->addAction(actZoomFit);
    tbrView->addAction(actAddBase);
    tbrView->addAction(actHideBase);
    tbrView->addAction(actMagnetToExtensions);

    /*tbrALT = new QToolBar(tr("Изменить название этажа"));
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
//    tbrAdd->addAction(actFloorAdd);
    tbrAdd->addAction(actAddWall);
    tbrAdd->addAction(actAddArea);
    tbrAdd->addAction(actAddDoor);
}

void MainWindow::createPanelFloorsManagement()
{
    dckwgtFloorsManagement = new QDockWidget(tr("Управление этажами"));
    connect(dckwgtFloorsManagement, SIGNAL(visibilityChanged(bool)),
            SLOT(panelFloorsManagementVisibilityChanged(bool)));
    addDockWidget(Qt::LeftDockWidgetArea, dckwgtFloorsManagement);
    dckwgtFloorsManagement->setFeatures(
            QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    wgtFloorsManagement = new QWidget;
    dckwgtFloorsManagement->setWidget(wgtFloorsManagement);
    vblFloorsManagement = new QVBoxLayout(wgtFloorsManagement);
    hblFloorsManagementButtons = new QHBoxLayout();
    vblFloorsManagement->addLayout(hblFloorsManagementButtons);
    btnFloorAdd = new QToolButton();
    btnFloorAdd->setDefaultAction(actFloorAdd);
    btnFloorDelete = new QToolButton();
    btnFloorDelete->setDefaultAction(actFloorDelete);
    btnFloorMoveDown = new QToolButton();
    btnFloorMoveDown->setDefaultAction(actFloorMoveDown);
    btnFloorMoveUp = new QToolButton();
    btnFloorMoveUp->setDefaultAction(actFloorMoveUp);
    hblFloorsManagementButtons->addWidget(btnFloorAdd);
    hblFloorsManagementButtons->addWidget(btnFloorDelete);
    hblFloorsManagementButtons->addWidget(btnFloorMoveDown);
    hblFloorsManagementButtons->addWidget(btnFloorMoveUp);
    hblFloorsManagementButtons->addStretch();
    viewFloorsList = new QListView();
    connect(viewFloorsList, SIGNAL(activated(QModelIndex)),
            SLOT(viewFloorsListItemActivated(QModelIndex)));
    connect(modelFloorsList, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            SLOT(viewFloorsListItemChanged(QModelIndex)));
    viewFloorsList->setModel(modelFloorsList);
    viewFloorsList->setSelectionModel(selectionFloorsList);
    vblFloorsManagement->addWidget(viewFloorsList);
}

void MainWindow::createPanelAreasMarking()
{
    dckwgtAreasMarking = new QDockWidget(tr("Разметка помещений"));
    connect(dckwgtAreasMarking, SIGNAL(visibilityChanged(bool)),
            SLOT(panelAreasMarkingVisibilityChanged(bool)));
    addDockWidget(Qt::LeftDockWidgetArea, dckwgtAreasMarking);
    dckwgtAreasMarking->setFeatures(
            QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    wgtAreasMarking = new QWidget;
    dckwgtAreasMarking->setWidget(wgtAreasMarking);
    vblAreasMarking = new QVBoxLayout(wgtAreasMarking);
    lblRoomName = new QLabel(tr("Название: "));
    ptdtRoomName = new QPlainTextEdit();
    connect(ptdtRoomName, SIGNAL(textChanged()), SLOT(setAreaName()));
    ptdtRoomName->setMaximumHeight(
            (ptdtRoomName->fontMetrics().lineSpacing() +
             ptdtRoomName->font().pointSize())*2);
    ptdtRoomName->setMinimumWidth(cDockWidth);
    //    ptdtRoomName->setMaximumHeight(ptdtRoomName->setGeometry(.pointSize()*2);
    lblRoomDescription = new QLabel(tr("Описание: "));
    ptdtRoomDescription = new QPlainTextEdit();
    ptdtRoomDescription->setEnabled(false);
    vblAreasMarking->addWidget(lblRoomName);
    vblAreasMarking->addWidget(ptdtRoomName);
    vblAreasMarking->addWidget(lblRoomDescription);
    vblAreasMarking->addWidget(ptdtRoomDescription);

}

void MainWindow::createGraphics()
{
    if (!view)
    {
        view = new QGraphicsView();
//        view->setRenderHint(QPainter::Antialiasing);
        view->setMouseTracking(true);
        view->setBackgroundBrush(QBrush(Qt::gray));
        vblwgtCentral->addWidget(view);
        view->show();
        setState(eFile | eView | eAdd | eHelp | ePanels, stTrue, stTrue);
        setState(eFloorsManagement | eAreasMarking, stFalse, stFalse);
        switchMode(MapFloor::Idle);
    }
    else
    {
        modelFloorsList->setStringList(QStringList());
//        cbxFloorSelect->clear();
    }
}

void MainWindow::setState(Elements elem, State visible, State enable)
{
//    bool enable = (st == stEnable_Visible);
//    bool visible = (st != stDisable_Unvisible);
    if (visible != stSave)
    {
        if (elem & eFile)
        {
            actMapNew->setVisible(visible);
            actMapOpen->setVisible(visible);
            actMapSave->setVisible(visible);

            menuFile->menuAction()->setVisible(visible);

            tbrFile->setVisible(visible);
        }
        if (elem & eView)
        {
            actFloorUp->setVisible(visible);
            cbxFloorSelect->setVisible(visible);
            actFloorDown->setVisible(visible);
    //        actZoomOut->setVisible(vs);
    //        actZoomIn->setVisible(vs);
    //        actZoomFit->setVisible(vs);
            actAddBase->setVisible(visible);

//            menuMode->menuAction()->setVisible(vs);

            tbrView->setVisible(visible);
        }
        if (elem & eAdd)
        {
            actFloorAdd->setVisible(visible);
            actAddWall->setVisible(visible);
            actAddArea->setVisible(visible);
            actAddDoor->setVisible(visible);

            menuAdd->menuAction()->setVisible(visible);

            tbrAdd->setVisible(visible);
        }
        if (elem & eHelp)
        {
            actAbout->setVisible(visible);

            menuHelp->menuAction()->setVisible(visible);

//            tbrAdd->setVisible(vs);
        }
        if (elem & ePanels)
        {
    //        actSwitchToPlanningMode->setVisible(vs);
    //        actSwitchToMarkingMode->setVisible(vs);
    //        actSwitchToGraphMode->setVisible(vs);
            actgrpPanels->setVisible(visible);
            menuPanels->menuAction()->setVisible(visible);
            tbrPanels->setVisible(visible);

    //        menuPanels->menuAction()->setVisible(vs);

    //        tbrPanels->setVisible(vs);
        }
        if (elem & eFloorsManagement)
        {
            dckwgtFloorsManagement->setVisible(visible);
        }
        if (elem & eAreasMarking)
        {
            dckwgtAreasMarking->setVisible(visible);
    //        ptdtRoomName->setVisible(vs);
    //        ptdtRoomDescription->setVisible(vs);
        }
    }
    if (enable != stSave)
    {
        if (elem & eFile)
        {
            actMapNew->setEnabled(enable);
            actMapOpen->setEnabled(enable);
            actMapSave->setEnabled(enable);

            menuFile->menuAction()->setEnabled(enable);

            tbrFile->setEnabled(enable);
        }
        if (elem & eView)
        {
            if (curFloor == 0)
                actFloorUp->setEnabled(false);
            else
                actFloorUp->setEnabled(enable);
            cbxFloorSelect->setEnabled(enable);
            if (map)
            {
                if (curFloor == map->floorsNumber() - 1)
                    actFloorDown->setEnabled(false);
                else
                    actFloorDown->setEnabled(enable);
            }
            //        actZoomOut->setEnabled(en);
            //        actZoomIn->setEnabled(en);
            //        actZoomFit->setEnabled(en);
            actAddBase->setEnabled(enable);

//            menu->menuAction()->setEnabled(vs);

            //        tbrView->setEnabled(en);
        }
        if (elem & eAdd)
        {
            actFloorAdd->setEnabled(enable);
            actAddWall->setEnabled(enable);
            actAddArea->setEnabled(enable);
            actAddDoor->setEnabled(enable);

            menuAdd->menuAction()->setEnabled(enable);

            tbrAdd->setEnabled(enable);
        }
        if (elem & eHelp)
        {
            actAbout->setEnabled(enable);

            menuHelp->menuAction()->setEnabled(enable);

//            tbrAdd->setEnabled(b);
        }
        if (elem & ePanels)
        {
            //        actSwitchToPlanningMode->setEnabled(en);
            //        actSwitchToMarkingMode->setEnabled(en);
            //        actSwitchToGraphMode->setEnabled(en);
            actgrpPanels->setEnabled(enable);
            menuPanels->menuAction()->setEnabled(enable);
            tbrPanels->setEnabled(enable);

            //        menuPanels->menuAction()->setEnabled(en);

            //        tbrPanels->setEnabled(en);
        }
        if (elem & eFloorsManagement)
        {
            wgtFloorsManagement->setEnabled(enable);
            //        dckwgtFloorsManagement->setEnabled(en);
        }
        if (elem & eAreasMarking)
        {
            wgtAreasMarking->setEnabled(enable);
            //        dckwgtAreasMarking->setEnabled(en);
            //        ptdtRoomName->setEnabled(en);
            //        ptdtRoomDescription->setEnabled(en);
        }
    }

}

qreal MainWindow::displayPixPerM(qreal pix, qreal mm) const
{
    return (static_cast<qreal>(pix)/mm)*1000;
}

void MainWindow::swapFloors(int x, int y)
{
    QModelIndex indexX = modelFloorsList->index(x);
    QModelIndex indexY = modelFloorsList->index(y);
    modelFloorsList->blockSignals(true);
    QString str = modelFloorsList->data(indexX, Qt::DisplayRole).toString();
    modelFloorsList->setData(
            indexX, modelFloorsList->data(indexY, Qt::DisplayRole).toString());
    modelFloorsList->setData(indexY, str);
    modelFloorsList->blockSignals(false);
}

void MainWindow::mapNew()
{
    DialogMapSize* pDialog = new DialogMapSize(this);
    if (pDialog->exec() == QDialog::Accepted)
    {
        if (map)
            delete map;
        map = new Map(cPixPerRealM, displayPixPerM(width(), widthMM()),
                      pDialog->MSizeX()*cPixPerRealM,
                      pDialog->MSizeY()*cPixPerRealM, this);
        createGraphics();
        floorAdd();
        view->fitInView(view->sceneRect(), Qt::KeepAspectRatio);
    }
}

void MainWindow::mapOpen()
{
//    QString str = QFileDialog::getOpenFileName(this, tr("Выберите карту"), "",
//                                               tr("Файл карты здания(*.bld)"));
    QString str = openedFile;
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
                                           tr("Выбранный файл повреждён или "
                                              "не является файлом карты."));
                return;
            }
            if (map)
                delete map;
            map = new Map(0, 0, 0, 0, this);
            s >> *map >> curFloor;
            map->setPixPerDisplayM(displayPixPerM(width(), widthMM()));
            createGraphics();
            cbxFloorSelect->blockSignals(true);
            modelFloorsList->blockSignals(true);
            for (int j = 0; j != map->floorsNumber(); j++)
            {
                modelFloorsList->insertRow(j);
                QModelIndex index = modelFloorsList->index(j);
//                QString str = ;
                modelFloorsList->setData(index, map->floor(j)->name());
//                cbxFloorSelect->addItem(str);
            }
            modelFloorsList->blockSignals(false);
            cbxFloorSelect->blockSignals(false);
            setActiveFloor(curFloor);
            view->fitInView(view->sceneRect(), Qt::KeepAspectRatio);
            openedFile = f.fileName();
        }
        else
    {
            QMessageBox::warning(this, tr("Ошибка при чтении файла"),
                                       tr("Выбранный файл повреждён или "
                                          "не является файлом карты."));
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
    // Copy
    QString copyFile = QTime::currentTime().toString("hh.mm.ss.zzz") + ".bld";
    QFile copy(copyFile);
    if (copy.open(QIODevice::WriteOnly))
    {
        QDataStream s(&copy);
        s.setVersion(QDataStream::Qt_4_7);
        s << cMagicNumber << *map << curFloor;
    }
}

void MainWindow::floorUp()
{
    setActiveFloor(curFloor - 1);
}

void MainWindow::floorDown()
{
    setActiveFloor(curFloor + 1);
}

void MainWindow::zoomOut()
{
    view->scale(1/cZoom, 1/cZoom);
}

void MainWindow::zoomIn()
{
    view->scale(cZoom, cZoom);
}

void MainWindow::zoomFit()
{
    view->fitInView(view->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::addBase()
{
    QString str = QFileDialog::getOpenFileName(
            this, tr("Выберите подложку"), "",
            tr("Изображения(*.jpg *.jpeg *.bmp *.png)"));
    if (!str.isNull())
    {
        map->floor(curFloor)->addBase(str);
    }
}

void MainWindow::hideBase(bool b)
{
    map->floor(curFloor)->baseSetVisible(!b);
}

void MainWindow::magnetToExtensions(bool b)
{
    map->floor(curFloor)->magnetToExtensions(b);
}

//void MainWindow::floorNameChange(const QString &floorName)
//{
//    map->floor(curFloor)->setName(floorName);
//    modelFloorsList->setData(modelFloorsList->index(curFloor), floorName);
////    cbxFloorSelect->setItemText(curFloor, floorName);
//}

void MainWindow::floorAdd()
{
//    bool bOk;
//    QString s = QInputDialog::getText(this, tr("Создание этажа"), tr("Введите,"
//                                      " пожалуйста, название этажа:"),
//                                      QLineEdit::Normal, tr("Новый"), &bOk);
//    if (!bOk | s.isEmpty())
//        s = tr("Без названия (" +
//               QString::number(map->floorsNumber()).toUtf8() + ")");

    map->insertFloor(0);
    modelFloorsList->insertRow(0);
//    QModelIndex index = modelFloorsList->index(map->floorsNumber()-1);
//    modelFloorsList->setData(index, floorName);
//    cbxFloorSelect->addItem(floorName);
//    cbxFloorSelect->setCurrentIndex(curFloor);
    setActiveFloor(0);

//    QString floorName = QString::number(map->floorsNumber());
//    map->floor(curFloor)->area()->setName(floorName);

}

void MainWindow::floorDelete()
{
    map->removeFloor(curFloor);
    modelFloorsList->removeRow(curFloor);
    if (curFloor > 0)
        setActiveFloor(curFloor - 1);
    else
        setActiveFloor(curFloor + 1);
}

void MainWindow::floorMoveDown()
{
    map->swapFloors(curFloor, curFloor + 1);
    swapFloors(curFloor, curFloor + 1);
    setActiveFloor(curFloor + 1);
}

void MainWindow::floorMoveUp()
{
    map->swapFloors(curFloor, curFloor - 1);
    swapFloors(curFloor, curFloor - 1);
    setActiveFloor(curFloor - 1);
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

void MainWindow::actgrpPanelsTriggered(QAction *qAct)
{
    // Unchecking all previous checked actions
    for (int i = 0; i != actgrpPanels->actions().size(); i++)
    {
        QAction *act = actgrpPanels->actions().at(i);
        if ((act != qAct) & (act->isChecked()))
        {
            act->setChecked(false);
            setState(Elements(act->data().toInt()), stFalse, stSave);
        }
    }

    setState(Elements(qAct->data().toInt()), State(qAct->isChecked()), stSave);
}

void MainWindow::switchMode(MapFloor::Mode m)
{
    switch (m)
    {
    case MapFloor::Idle:
        setState(eFile | eView | eAdd | eHelp | ePanels, stTrue, stTrue);
        setState(eFloorsManagement | eAreasMarking, stSave, stTrue);
        setCursor(Qt::ArrowCursor);
        break;
    case MapFloor::WallAdd:
    case MapFloor::AreaAdd:
    case MapFloor::DoorAdd:
    case MapFloor::Graph:
        setState(eFile | eView | eHelp | ePanels, stTrue, stTrue);
        setState(eAdd | eFloorsManagement | eAreasMarking, stSave, stFalse);
        setCursor(Qt::CrossCursor);
        if (m == MapFloor::Graph)
            map->graphStartAnew();
        break;
    case MapFloor::Selection:
        {
            bool selectionIsValid = false;
            if (map->floor(curFloor)->selectedItem() != 0)
                if (map->floor(curFloor)->selectedItem()->type() == MapArea::Type)
                {
                    selectionIsValid = true;
                    setState(eAreasMarking, stSave, stTrue);
                    MapArea *area = qgraphicsitem_cast<MapArea*>(
                            map->floor(curFloor)->selectedItem());
                    // If selected area is outline without name
//                    if ((area->zValue() == MapFloor::OutlineZValue) &
//                        (area->name() == ""))
//                        ptdtRoomName->setPlainText(modelFloorsList->data(
//                                modelFloorsList->index(curFloor),
//                                Qt::DisplayRole).toString());
////                                cbxFloorSelect->itemText(curFloor));

//                    else
                    ptdtRoomName->setPlainText(area->name());
                    ptdtRoomName->setFocus();
                    ptdtRoomName->selectAll();
                }
            if (!selectionIsValid)
            {
                setState(eAreasMarking, stSave, stFalse);
                ptdtRoomName->setPlainText("");
            }
            break;
        }
    }
    mode = m;
    if (map->floorsNumber() != 0)
        if (map->floor(curFloor)->mode() != m)
            map->floor(curFloor)->setMode(m);
}

void MainWindow::setActiveFloor(int i)
{
    if ((i >= 0)&(i < map->floorsNumber()))
    {
        if ((curFloor >= 0) & (curFloor != i))
            map->floor(curFloor)->disconnect();
        curFloor = i;
        QModelIndex index = modelFloorsList->index(i);
        selectionFloorsList->setCurrentIndex(
                index, QItemSelectionModel::ClearAndSelect);
        cbxFloorSelect->blockSignals(true);
        cbxFloorSelect->setCurrentIndex(i);
        cbxFloorSelect->blockSignals(false);
        view->setScene(map->floor(i));
        connect(map->floor(i), SIGNAL(modeChanged(MapFloor::Mode)),
                SLOT(switchMode(MapFloor::Mode)));
        connect(map->floor(i), SIGNAL(mouseDoubleClicked()),
                SLOT(mouseDoubleClicked()));
        setFocus();
        bool switchToSelection = false;
        if (map->floor(i)->selectedItem() != 0)
            if ((mode == MapFloor::Idle) | (mode == MapFloor::Selection))
            {
                switchMode(MapFloor::Selection);
                switchToSelection = true;
            }
        if (!switchToSelection & (map->floor(i)->mode() != mode))
            switchMode(mode);

        if (i == 0)
        {
            actFloorMoveUp->setEnabled(false);
            actFloorUp->setEnabled(false);
        }
        else
        {
            actFloorMoveUp->setEnabled(true);
            actFloorUp->setEnabled(true);
        }
        if (i == map->floorsNumber() - 1)
        {
            actFloorMoveDown->setEnabled(false);
            actFloorDown->setEnabled(false);
        }
        else
        {
            actFloorMoveDown->setEnabled(true);
            actFloorDown->setEnabled(true);
        }
    }
}

void MainWindow::setAreaName()
{
    if (map->floor(curFloor)->selectedItem() != 0)
        if (map->floor(curFloor)->selectedItem()->type() == MapArea::Type)
        {

            MapArea *area = qgraphicsitem_cast<MapArea*>(
                    map->floor(curFloor)->selectedItem());
            QString areaName = ptdtRoomName->document()->toPlainText();
            area->setName(areaName);
        }
}

void MainWindow::panelFloorsManagementVisibilityChanged(bool visible)
{
    actPanelFloorsManagement->setChecked(visible);
}

void MainWindow::mouseDoubleClicked()
{
    actPanelAreasMarking->trigger();
}

void MainWindow::panelAreasMarkingVisibilityChanged(bool visible)
{
    actPanelAreasMarking->setChecked(visible);
}

void MainWindow::viewFloorsListItemActivated(QModelIndex index)
{
    int floor = index.row();
    if (curFloor != floor)
        setActiveFloor(floor);
}

void MainWindow::viewFloorsListItemChanged(QModelIndex index)
{
    map->floor(curFloor)->setName(modelFloorsList->data(
            index, Qt::DisplayRole).toString());
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("О программе"), "<b>" + qApp->applicationName()
                       + " " + qApp->applicationVersion() + "</b>" + "<br>" +
                       tr("Программа предназначена для перевода пользователем "
                          "поэтажного плана здания в собственный векторный "
                          "формат, добавления информации об объектах, "
                          "автоматизированного создания и редактирования "
                          "графа здания."));
}
