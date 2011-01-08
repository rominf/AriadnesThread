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
    curFloor = 0;
    // ### Make autoAreasRenaming editable by user
    autoAreasRenaming = true;
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
    setState(eEdit | eFloorsSwitching | eView | eLayers | eAdd |ePanels |
             eFloorsManagement | eAreasMarking, stFalse, stFalse);
    actMapSave->setEnabled(false);

    showMaximized();

    openFileName = "map.bld";
}

MainWindow::~MainWindow()
{
}

QList<QKeySequence> MainWindow::shortcuts(QKeySequence main, QKeySequence extra)
{
    QList<QKeySequence> list;
    list.append(main);
    if (isExtraShortcutsEnabled)
        list.append(extra);
    return list;
}

void MainWindow::createActions()
{
    actMapNew = new QAction(QIcon(":/New"), tr("&Создать"), this);
    connect(actMapNew, SIGNAL(triggered()), SLOT(mapNew()));
    actMapNew->setShortcuts(shortcuts(QKeySequence::New, tr("N")));

    actMapOpen = new QAction(QIcon(":/Open"), tr("&Открыть..."), this);
    connect(actMapOpen, SIGNAL(triggered()), SLOT(mapOpen()));
    actMapOpen->setShortcuts(shortcuts(QKeySequence::Open, tr("O")));

    actMapSave = new QAction(QIcon(":/Save"), tr("Со&хранить"), this);
    connect(actMapSave, SIGNAL(triggered()), SLOT(mapSave()));
    actMapSave->setShortcuts(shortcuts(QKeySequence::Save, tr("S")));

    actMapSaveAs = new QAction(QIcon(":/SaveAs"),
                               tr("Сохранить &как..."), this);
    connect(actMapSaveAs, SIGNAL(triggered()), SLOT(mapSaveAs()));
    actMapSaveAs->setShortcuts(shortcuts(QKeySequence::SaveAs, tr("Shift+S")));

    actQuit = new QAction(QIcon(":/Quit"), tr("В&ыход"), this);
    connect(actQuit, SIGNAL(triggered()), SLOT(exit()));
    actQuit->setShortcuts(shortcuts(QKeySequence::Quit, tr("Q")));


    actAreaCopy = new QAction(QIcon(":/AreaCopy"),
                              tr("Копировать &область..."), this);
    connect(actAreaCopy, SIGNAL(triggered()), SLOT(areaCopy()));
    actAreaCopy->setShortcuts(shortcuts(QKeySequence::Copy, tr("C")));

    actGraphCopy = new QAction(QIcon(":/GraphCopy"),
                              tr("Копировать &граф..."), this);
    connect(actGraphCopy, SIGNAL(triggered()), SLOT(graphCopy()));
    actGraphCopy->setShortcuts(shortcuts(tr("Ctrl+Shift+C"), tr("Shift+C")));


    actFloorDown = new QAction(QIcon(":/FloorDown"), tr("Этаж &ниже"), this);
    connect(actFloorDown, SIGNAL(triggered()), SLOT(floorDown()));
    actFloorDown->setShortcuts(shortcuts(tr("Ctrl+Down"), Qt::Key_PageDown));

    actFloorUp = new QAction(QIcon(":/FloorUp"), tr("Этаж &выше"),
                                   this);
    connect(actFloorUp, SIGNAL(triggered()), SLOT(floorUp()));
    actFloorUp->setShortcuts(shortcuts(tr("Ctrl+Up"), Qt::Key_PageUp));


    actZoomOut= new QAction(QIcon(":/ZoomOut"), tr("У&меньшить"), this);
    connect(actZoomOut, SIGNAL(triggered()), SLOT(zoomOut()));
    actZoomOut->setShortcut(/*Qt::CTRL + */tr("-"));

    actZoomIn = new QAction(QIcon(":/ZoomIn"), tr("&Увеличить"), this);
    connect(actZoomIn, SIGNAL(triggered()), SLOT(zoomIn()));
    actZoomIn->setShortcut(/*Qt::CTRL + */tr("+"));

    actZoomFit = new QAction(QIcon(":/ZoomFit"), tr("&Вместить"), this);
    connect(actZoomFit, SIGNAL(triggered()), SLOT(zoomFit()));
    actZoomFit->setShortcut(/*Qt::CTRL + */tr("*"));
//    actZoomFit->setVisible(false);

    actAddBase = new QAction(QIcon(":/AddBase"),
                             tr("Добавить &подложку"), this);
    connect(actAddBase, SIGNAL(triggered()), SLOT(addBase()));
    actAddBase->setShortcuts(shortcuts(tr("Ctrl+B"), tr("B")));

    actLayerBase = new QAction(QIcon(":/LayerBase"), tr("&Подложка"), this);
    connect(actLayerBase, SIGNAL(triggered(bool)),
            SLOT(layerBaseSetVisible(bool)));
    actLayerBase->setCheckable(true);
    actLayerBase->setChecked(true);

    actLayerGraph = new QAction(QIcon(":/LayerGraph"), tr("&Граф"), this);
    connect(actLayerGraph, SIGNAL(triggered(bool)),
            SLOT(layerGraphSetVisible(bool)));
    actLayerGraph->setCheckable(true);
    actLayerGraph->setChecked(true);

    actMagnetToExtensions = new QAction(
            QIcon(":/MagnetToExtensions"),
            tr("&Притягиваться к продолжениям линий"), this);
    connect(actMagnetToExtensions, SIGNAL(triggered(bool)),
            SLOT(magnetToExtensions(bool)));
    actMagnetToExtensions->setCheckable(true);


    actAddWall = new QAction(QIcon(":/AddWall"), tr("Добавить &стену"), this);
    connect(actAddWall, SIGNAL(triggered()), SLOT(addWall()));
//    actAddWall->setShortcuts(tr("Ctrl+W"), tr("W"));

    actAddArea = new QAction(QIcon(":/AddArea"),
                             tr("Добавить &помещение"),this);
    connect(actAddArea, SIGNAL(triggered()), SLOT(addArea()));
    actAddArea->setShortcuts(shortcuts(tr("Ctrl+A"), tr("A")));

    actAddDoor = new QAction(QIcon(":/AddDoor"), tr("Добавить &дверь"), this);
    connect(actAddDoor, SIGNAL(triggered()), SLOT(addDoor()));
    actAddDoor->setShortcuts(shortcuts(tr("Ctrl+D"), tr("D")));

    actAddNode = new QAction(QIcon(":/AddNode"), tr("Добавить вершину &графа"),
                             this);
    connect(actAddNode, SIGNAL(triggered()), SLOT(addNode()));
    actAddNode->setShortcuts(shortcuts(tr("Ctrl+G"), tr("G")));


    actAbout = new QAction(tr("&О программе"), this);
    connect(actAbout, SIGNAL(triggered()), SLOT(about()));
    /*actAboutQT = new QAction(tr("О QT"), this);
    actAboutQT->setStatusTip(tr("Об инструментарий Qt, при помощи которого "
                                "была создана эта программа"));
    connect(actAboutQT, SIGNAL(triggered()), qApp, SLOT(aboutQt()));*/


    actPanelFloorsManagement = new QAction(QIcon(":/FloorsManagement"),
                                         tr("&Управление этажами"), this);
    actPanelFloorsManagement->setData(eFloorsManagement);
    actPanelFloorsManagement->setShortcuts(shortcuts(tr("Ctrl+1"), tr("1")));
    actPanelFloorsManagement->setCheckable(true);


    actFloorAdd = new QAction(QIcon(":/FloorAdd"), tr("&Добавить этаж"), this);
    connect(actFloorAdd, SIGNAL(triggered()), SLOT(floorAdd()));
    actFloorAdd->setShortcuts(shortcuts(tr("Ctrl+F"), tr("F")));

    actFloorDelete = new QAction(QIcon(":/FloorDelete"),
                                 tr("&Удалить этаж"), this);
    connect(actFloorDelete, SIGNAL(triggered()), SLOT(floorDelete()));
    actFloorDelete->setShortcuts(shortcuts(tr("Ctrl+Alt+F"), tr("Alt+F")));

    actFloorMoveDown = new QAction(QIcon(":/FloorDown"),
                                 tr("Переместить в&низ"), this);
    connect(actFloorMoveDown, SIGNAL(triggered()), SLOT(floorMoveDown()));

    actFloorMoveUp = new QAction(QIcon(":/FloorUp"),
                                 tr("Переместить &вверх"), this);
    connect(actFloorMoveUp, SIGNAL(triggered()), SLOT(floorMoveUp()));

    actFloorSetDefault = new QAction(QIcon(":/FloorDefault"),
                                 tr("Сделать этажом по умол&чанию"), this);
    connect(actFloorSetDefault, SIGNAL(triggered()), SLOT(floorSetDefault()));
    actPanelFloorsManagement->setCheckable(true);


    actPanelAreasMarking = new QAction(QIcon(":/AreasMarking"),
                                         tr("&Разметка помещений"), this);
    actPanelAreasMarking->setData(eAreasMarking);
    actPanelAreasMarking->setShortcuts(shortcuts(tr("Ctrl+2"), tr("2")));
    actPanelAreasMarking->setCheckable(true);


    actgrpPanels = new QActionGroup(this);
    actgrpPanels->setExclusive(false);
    actgrpPanels->addAction(actPanelAreasMarking);
    actgrpPanels->addAction(actPanelFloorsManagement);
    connect(actgrpPanels, SIGNAL(triggered(QAction *)),
            SLOT(actgrpPanelsTriggered(QAction *)));
}

void MainWindow::createMenus()
{
    menuFile = menuBar()->addMenu(tr("&Файл"));
    menuFile->addAction(actMapNew);
    menuFile->addAction(actMapOpen);
    menuFile->addAction(actMapSave);
    menuFile->addAction(actMapSaveAs);
    menuFile->addAction(actQuit);

    menuEdit = menuBar()->addMenu(tr("&Правка"));
    menuEdit->addAction(actAreaCopy);
    menuEdit->addAction(actGraphCopy);

    menuPanels = menuBar()->addMenu(tr("Пане&ли"));
    menuPanels->addAction(actPanelFloorsManagement);
    menuPanels->addAction(actPanelAreasMarking);

    menuAdd = menuBar()->addMenu(tr("&Добавить"));
//    menuAdd->addAction(actFloorAdd);
//    menuAdd->addAction(actAddWall);
    menuAdd->addAction(actAddArea);
    menuAdd->addAction(actAddDoor);
    menuAdd->addAction(actAddNode);

    menuHelp = menuBar()->addMenu(tr("&Справка"));
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
//    tbrFile->addAction(actMapSaveAs);
//    tbrFile->addAction(actQuit);

    tbrEdit = addToolBar(tr("Правка"));
    tbrEdit->setFloatable(false);
    tbrEdit->addAction(actAreaCopy);
    tbrEdit->addAction(actGraphCopy);

    tbrFloorsSwitching = addToolBar(tr("Переключение этажей"));
//    addToolBar(Qt::BottomToolBarArea, tbrFloorsSwitching);
    tbrFloorsSwitching->setFloatable(false);
//    QWidget *spacerLeft = new QWidget(this);
//    spacerLeft->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
//    tbrFloorsSwitching->addWidget(spacerLeft);
    tbrFloorsSwitching->addAction(actFloorDown);
    cbxFloorSelect = new QComboBox(this);
    cbxFloorSelect->setModel(modelFloorsList);
    connect(cbxFloorSelect,
            SIGNAL(currentIndexChanged(int)), SLOT(setActiveFloor(int)));
    cbxFloorSelect->setEditable(true);
    cbxFloorSelect->setInsertPolicy(QComboBox::NoInsert);
    tbrFloorsSwitching->addWidget(cbxFloorSelect);
    tbrFloorsSwitching->addAction(actFloorUp);
//    QWidget *spacerRight = new QWidget(this);
//    spacerRight ->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
//    tbrFloorsSwitching->addWidget(spacerRight);

    tbrView = addToolBar(tr("Вид"));
    tbrView->setFloatable(false);
    tbrView->addAction(actZoomOut);
    tbrView->addAction(actZoomIn);
    tbrView->addAction(actZoomFit);
    tbrView->addAction(actAddBase);
    tbrView->addAction(actMagnetToExtensions);

    tbrLayers = addToolBar(tr("Слои"));
    tbrLayers->setFloatable(false);
    tbrLayers->addAction(actLayerBase);
    tbrLayers->addAction(actLayerGraph);

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
//    tbrAdd->addAction(actAddWall);
    tbrAdd->addAction(actAddArea);
    tbrAdd->addAction(actAddDoor);
    tbrAdd->addAction(actAddNode);

    tbrPanels = new QToolBar(tr("Панели"));
    addToolBar(Qt::LeftToolBarArea, tbrPanels);
    tbrPanels->setFloatable(false);
    tbrPanels->addAction(actPanelFloorsManagement);
    tbrPanels->addAction(actPanelAreasMarking);
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

    btnFloorAdd = new QToolButton();
    btnFloorAdd->setDefaultAction(actFloorAdd);
    hblFloorsManagementButtons->addWidget(btnFloorAdd);

    btnFloorDelete = new QToolButton();
    btnFloorDelete->setDefaultAction(actFloorDelete);
    hblFloorsManagementButtons->addWidget(btnFloorDelete);

    btnFloorMoveDown = new QToolButton();
    btnFloorMoveDown->setDefaultAction(actFloorMoveDown);
    hblFloorsManagementButtons->addWidget(btnFloorMoveDown);

    btnFloorMoveUp = new QToolButton();
    btnFloorMoveUp->setDefaultAction(actFloorMoveUp);
    hblFloorsManagementButtons->addWidget(btnFloorMoveUp);

    btnFloorSetDefault = new QToolButton();
    btnFloorSetDefault->setDefaultAction(actFloorSetDefault);
    hblFloorsManagementButtons->addWidget(btnFloorSetDefault);

    hblFloorsManagementButtons->addStretch();
    vblFloorsManagement->addLayout(hblFloorsManagementButtons);

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

    lblAreaName = new QLabel(tr("Название: "));
    vblAreasMarking->addWidget(lblAreaName);

    ptdtAreaName = new QPlainTextEdit();
    connect(ptdtAreaName, SIGNAL(textChanged()), SLOT(setAreaName()));
    ptdtAreaName->setMaximumHeight(
            (ptdtAreaName->fontMetrics().lineSpacing() +
             ptdtAreaName->font().pointSize())*2);
    ptdtAreaName->setMinimumWidth(cDockWidth);
    vblAreasMarking->addWidget(ptdtAreaName);

    chkAreaNameVisible = new QCheckBox(tr("Показывать название"));
    connect(chkAreaNameVisible, SIGNAL(stateChanged(int)),
            SLOT(chkAreaNameVisibleStateChanged(int)));
    vblAreasMarking->addWidget(chkAreaNameVisible);

    lblAreaDescription = new QLabel(tr("Описание: "));
    vblAreasMarking->addWidget(lblAreaDescription);

    ptdtAreaDescription = new QPlainTextEdit();
    connect(ptdtAreaDescription, SIGNAL(textChanged()),
            SLOT(setAreaDescription()));
    vblAreasMarking->addWidget(ptdtAreaDescription);
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
        setState(eFile | eEdit | eFloorsSwitching | eView | eLayers | eAdd |
                 eHelp | ePanels, stTrue, stTrue);
        setState(eFloorsManagement | eAreasMarking, stFalse, stFalse);
        switchMode(MapFloor::Idle);
    }
    else
    {
        modelFloorsList->setStringList(QStringList());
        curFloor = 0;
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
            actMapSaveAs->setVisible(visible);
            actQuit->setVisible(visible);

            menuFile->menuAction()->setVisible(visible);

            tbrFile->setVisible(visible);
        }
        if (elem & eEdit)
        {
            actAreaCopy->setVisible(visible);
            actGraphCopy->setVisible(visible);

            menuEdit->menuAction()->setVisible(visible);

            tbrEdit->setVisible(visible);
        }
        if (elem & eView)
        {
            actZoomOut->setVisible(visible);
            actZoomIn->setVisible(visible);
            actZoomFit->setVisible(visible);
            actAddBase->setVisible(visible);
            actMagnetToExtensions->setVisible(visible);
//            menuMode->menuAction()->setVisible(vs);

            tbrView->setVisible(visible);
        }
        if (elem & eLayers)
        {
            actLayerBase->setVisible(visible);
            actLayerGraph->setVisible(visible);

            tbrLayers->setVisible(visible);
        }
        if (elem & eAdd)
        {
            actAddWall->setVisible(visible);
            actAddArea->setVisible(visible);
            actAddDoor->setVisible(visible);
            actAddNode->setVisible(visible);

            menuAdd->menuAction()->setVisible(visible);

            tbrAdd->setVisible(visible);
        }
        if (elem & eHelp)
        {
            actAbout->setVisible(visible);

            menuHelp->menuAction()->setVisible(visible);

//            tbrAdd->setVisible(vs);
        }
        if (elem & eFloorsSwitching)
        {
            actFloorUp->setVisible(visible);
            cbxFloorSelect->setVisible(visible);
            actFloorDown->setVisible(visible);

            tbrFloorsSwitching->setVisible(visible);
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
            if (!visible)
                if (view)
                    setFocus();
        }
        if (elem & eAreasMarking)
        {
            dckwgtAreasMarking->setVisible(visible);
            if (!visible)
                if (view)
                    setFocus();
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
            actMapSaveAs->setEnabled(enable);
            actQuit->setEnabled(enable);

            menuFile->menuAction()->setEnabled(enable);

            tbrFile->setEnabled(enable);
        }
        if (elem & eEdit)
        {
            actAreaCopy->setEnabled(enable);
            actGraphCopy->setEnabled(enable);

            menuEdit->menuAction()->setEnabled(enable);

            tbrEdit->setEnabled(enable);
        }
        if (elem & eView)
        {
            actZoomOut->setEnabled(enable);
            actZoomIn->setEnabled(enable);
            actZoomFit->setEnabled(enable);
            actAddBase->setEnabled(enable);
            actMagnetToExtensions->setEnabled(enable);

//            menu->menuAction()->setEnabled(vs);

            tbrView->setEnabled(enable);
        }
        if (elem & eLayers)
        {
            actLayerBase->setEnabled(enable);
            actLayerGraph->setEnabled(enable);

            tbrLayers->setEnabled(enable);
        }
        if (elem & eAdd)
        {
            actAddWall->setEnabled(enable);
            actAddArea->setEnabled(enable);
            actAddDoor->setEnabled(enable);
            actAddNode->setEnabled(enable);

            menuAdd->menuAction()->setEnabled(enable);

            tbrAdd->setEnabled(enable);
        }
        if (elem & eHelp)
        {
            actAbout->setEnabled(enable);

            menuHelp->menuAction()->setEnabled(enable);

//            tbrAdd->setEnabled(b);
        }
        if (elem & eFloorsSwitching)
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

            tbrFloorsSwitching->setEnabled(enable);
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

void MainWindow::mapSave(QString &fileName)
{
    if (!fileName.isNull())
    {
        QFile f(fileName);
        if (f.open(QIODevice::WriteOnly))
        {
            QDataStream s(&f);
            s.setVersion(QDataStream::Qt_4_7);
            s << cMagicNumber << *map << defaultFloor << autoAreasRenaming;
        }
        // Copy
        QString copyFile = QTime::currentTime().toString("hh.mm.ss") + ".bld";
        QFile copy(copyFile);
        if (copy.open(QIODevice::WriteOnly))
        {
            QDataStream s(&copy);
            s.setVersion(QDataStream::Qt_4_7);
            s << cMagicNumber << *map << defaultFloor << autoAreasRenaming;
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!view)
        event->accept();
    else
    {
        QFileInfo openFile(openFileName);
        QMessageBox *msgbxClose = new QMessageBox(
                QMessageBox::Information, tr("Сохранение здания"),
                tr("Здание \"") + openFile.fileName() +
                tr("\" было изменено.\nСохранить изменения?"),
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        int answer = msgbxClose->exec();
        switch (answer)
        {
        case QMessageBox::Yes:
            mapSave();
            event->accept();
            return;
        case QMessageBox::No:
            event->accept();
            return;
        case QMessageBox::Cancel:
            event->ignore();
            return;
        }
    }
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
        floorSetDefault();
        view->fitInView(view->sceneRect(), Qt::KeepAspectRatio);
    }
}

void MainWindow::mapOpen()
{
    QString str = QFileDialog::getOpenFileName(
            this, tr("Открытие здания"),
            openFileName, tr("Файл здания(*.bld)"));
//    QString str = openFileName;
    if (!str.isNull())
    {
        QFile f(str);
        if (f.open(QIODevice::ReadOnly))
        {
            QDataStream s(&f);
            s.setVersion(QDataStream::Qt_4_7);
            qint32 magicNumber;
            s >> magicNumber;
            if (magicNumber != cMagicNumber)
            {
                QMessageBox::warning(this, tr("Ошибка при чтении файла"),
                                           tr("Выбранный файл повреждён или "
                                              "не является файлом здания."));
                return;
            }
            if (map)
                delete map;
            map = new Map(0, 0, 0, 0, this);
            s >> *map >> defaultFloor >> autoAreasRenaming;
            map->setPixPerDisplayM(displayPixPerM(width(), widthMM()));
            createGraphics();

            cbxFloorSelect->blockSignals(true);
            modelFloorsList->blockSignals(true);
            for (int i = 0; i != map->floorsNumber(); i++)
            {
                modelFloorsList->insertRow(i);
                QModelIndex index = modelFloorsList->index(i);
                modelFloorsList->setData(index, map->floor(i)->name());
                if (map->floor(i)->uin() == defaultFloor)
                    setActiveFloor(i);
//                viewFloorsList->repaint();
            }
            modelFloorsList->blockSignals(false);
            cbxFloorSelect->blockSignals(false);
            modelFloorsList->insertRow(0);
            modelFloorsList->removeRow(0);


            view->fitInView(view->sceneRect(), Qt::KeepAspectRatio);
            openFileName = f.fileName();
        }
        else
    {
            QMessageBox::warning(this, tr("Ошибка при чтении файла"),
                                       tr("Выбранный файл повреждён или "
                                          "не является файлом здания."));
    }
    }
}

void MainWindow::mapSave()
{
    mapSave(openFileName);
}

void MainWindow::mapSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(
            this, tr("Сохранение здания"), openFileName,
            tr("Файл здания(*.bld)"));
    mapSave(fileName);
}

void MainWindow::exit()
{
    close();
}

void MainWindow::areaCopy()
{
    if (map->floor(curFloor)->selectedItem() != 0)
        if (map->floor(curFloor)->selectedItem()->type() == MapArea::Type)
        {
            MapArea *oldArea = qgraphicsitem_cast<MapArea*>(
                    map->floor(curFloor)->selectedItem());
            DialogFloorChoice* dialog = new DialogFloorChoice(
                    this, modelFloorsList, tr("Выберите этаж, на который будет"
                                              "\nскопирована выделенная "
                                              "область:"));
            if (dialog->exec() == QDialog::Accepted)
            {
                int floor = dialog->floor();
                if (floor != curFloor)
                {
                    MapArea *newArea;
                    if (autoAreasRenaming)
                        newArea = new MapArea(
                                *oldArea, map->floor(curFloor)->name(),
                                map->floor(floor)->name());
                    else
                        newArea = new MapArea(*oldArea, "", "");
                    switch (map->floor(floor)->addArea(newArea))
                    {
                    case MapArea::ceNone:
                    {
                        MapArea *a;
                        QStack<MapArea*> stk;
                        stk.push(oldArea);
                        while (!stk.isEmpty())
                        {
                            a = stk.pop();
                            for (int i = 0; i != a->doorsNumber(); i++)
                            {
                                MapDoor *door = new MapDoor(*a->door(i));
                                map->floor(floor)->addDoor(door);
                            }
                            int size = a->areasNumber();
                            for (int i = 0; i !=size; i++)
                                stk.push(a->area(i));
                        }
                        setActiveFloor(floor);
                        break;
                    }
                    case MapArea::ceIntersection:
                        QMessageBox::warning(
                                0, tr("Ошибка при копировании области"),
                                tr("На ") + map->floor(floor)->name() +
                                tr(" этаже недостаточно места для вставки "
                                   "области."));
                        break;
                    case MapArea::ceAreaExist:
                        QMessageBox::warning(
                                0, tr("Ошибка при копировании области"),
                                tr("На ") + map->floor(floor)->name() +
                                tr(" этаже уже есть данная область."));
                        break;
                    }
                }
                else
                    QMessageBox::warning(
                            0, tr("Ошибка при копировании области"),
                            tr("На ") + map->floor(floor)->name() +
                            tr(" этаже уже есть данная область."));
            }
        }
}

void MainWindow::graphCopy()
{
    DialogFloorChoice* dialog = new DialogFloorChoice(
            this, modelFloorsList, tr("Выберите этаж, на который будет"
                                      "\nскопирована часть графа,"
                                      "\n находящегося на данном этаже:"));
    if (dialog->exec() == QDialog::Accepted)
    {
        int floor = dialog->floor();
        if (floor != curFloor)
        {
            map->graph()->copyFloor(map->floor(curFloor)->uin(),
                                    map->floor(floor)->uin());
            setActiveFloor(floor);
        }
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
        actLayerBase->setChecked(true);
    }
}

void MainWindow::layerBaseSetVisible(bool visible)
{
    map->floor(curFloor)->baseSetVisible(visible);
}

void MainWindow::layerGraphSetVisible(bool visible)
{
    map->graph()->setVisible(visible);
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

void MainWindow::floorSetDefault()
{
    defaultFloor = map->floor(curFloor)->uin();
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

void MainWindow::addNode()
{
    switchMode(MapFloor::NodeAdd);
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
        setState(eFile | eEdit | eFloorsSwitching | eView | eLayers | eAdd |
                 eHelp | ePanels, stTrue, stTrue);
        setState(eFloorsManagement | eAreasMarking, stSave, stTrue);
        setCursor(Qt::ArrowCursor);
        break;
    case MapFloor::WallAdd:
    case MapFloor::AreaAdd:
    case MapFloor::DoorAdd:
    case MapFloor::NodeAdd:
        setState(eFile | eView | eLayers | eHelp | ePanels, stTrue, stTrue);
        setState(eEdit | eFloorsSwitching | eAdd |
                 eFloorsManagement | eAreasMarking, stSave, stFalse);
        setCursor(Qt::CrossCursor);
        if (m == MapFloor::NodeAdd)
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
                    ptdtAreaName->setPlainText(area->name());
                    ptdtAreaName->selectAll();
                    chkAreaNameVisible->setChecked(area->isNameVisible());
                    ptdtAreaDescription->setPlainText(area->description());
                }
            if (!selectionIsValid)
            {
                setState(eAreasMarking, stSave, stFalse);
                ptdtAreaName->setPlainText("");
                ptdtAreaDescription->setPlainText("");
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
        {
            map->floor(curFloor)->disconnect(
                    SIGNAL(modeChanged(MapFloor::Mode)));
            map->floor(curFloor)->disconnect(SIGNAL(mouseDoubleClicked()));
        }
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
        bool isDefaultFloor = map->floor(i)->uin() == defaultFloor;
        actFloorSetDefault->setChecked(isDefaultFloor);
        actFloorSetDefault->setEnabled(!isDefaultFloor);
    }
}

void MainWindow::panelFloorsManagementVisibilityChanged(bool visible)
{
    actPanelFloorsManagement->setChecked(visible);
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

void MainWindow::mouseDoubleClicked()
{
    actPanelAreasMarking->trigger();
}

void MainWindow::panelAreasMarkingVisibilityChanged(bool visible)
{
    actPanelAreasMarking->setChecked(visible);
}

void MainWindow::setAreaName()
{
    if (map->floor(curFloor)->selectedItem() != 0)
        if (map->floor(curFloor)->selectedItem()->type() == MapArea::Type)
        {

            MapArea *area = qgraphicsitem_cast<MapArea*>(
                    map->floor(curFloor)->selectedItem());
            area->setName(ptdtAreaName->document()->toPlainText());
        }
}

void MainWindow::setAreaDescription()
{
    if (map->floor(curFloor)->selectedItem() != 0)
        if (map->floor(curFloor)->selectedItem()->type() == MapArea::Type)
        {
            MapArea *area = qgraphicsitem_cast<MapArea*>(
                    map->floor(curFloor)->selectedItem());
            area->setDescription(
                    ptdtAreaDescription->document()->toPlainText());
        }
}

void MainWindow::chkAreaNameVisibleStateChanged(int state)
{
    if (map->floor(curFloor)->selectedItem() != 0)
        if (map->floor(curFloor)->selectedItem()->type() == MapArea::Type)
        {

            MapArea *area = qgraphicsitem_cast<MapArea*>(
                    map->floor(curFloor)->selectedItem());
            area->setNameVisible(state == Qt::Checked);
        }
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
