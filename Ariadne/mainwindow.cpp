#include "mainwindow.h"

const QString MainWindow::cNewFileName = QDir::currentPath() + tr("/New.bld");

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
//    qApp->setWindowIcon(QIcon(":/Ariadne"));
#ifdef EDITOR
    qApp->setApplicationName(tr("Ариадна"));
#else
    qApp->setApplicationName(tr("Тесей"));
#endif
    qApp->setApplicationVersion("1.0");
    setWindowTitle(qApp->applicationName());

    map = 0;
    view = 0;
    curFloor = -1;
    modelFloorsList = new QStringListModel(this);
    selectionFloorsList = new QItemSelectionModel(modelFloorsList, this);
    connect(selectionFloorsList, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(viewFloorsListCurrentItemChanged(QModelIndex,QModelIndex)));

    // Create central widget
    {
        wgtCentral = new QWidget;
        setCentralWidget(wgtCentral);
        vblwgtCentral = new QVBoxLayout(wgtCentral);
    }
    createActions();
    createMenus();
    createToolBars();
#ifdef EDITOR
    createPanelFloors();
    createPanelVerticals();
#endif
    createPanelSearch();
    createPanelAreasProperties();
    createPanelWays();

    setState(eFile | eHelp, stTrue, stTrue);
    setState(eEdit | eView | eGo | eAdd | ePanels | eFloors |
             eVerticals | eAreasProperties | eSearch | eWays, stFalse, stFalse);
#ifdef EDITOR
    actMapSave->setEnabled(false);
#endif
    actMapSaveAs->setEnabled(false);
//    resize(792, 573);
    resize(782, 498);
//    showMaximized();

    openFileName = tr("УЛК МГТУ им. Н.Э. Баумана.bld");
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
#ifdef EDITOR
    actMapNew = new QAction(QIcon(":/New"), tr("&Создать…"), this);
    connect(actMapNew, SIGNAL(triggered()), SLOT(mapNew()));
    actMapNew->setShortcuts(shortcuts(QKeySequence::New, tr("N")));
#endif

    actMapOpen = new QAction(QIcon(":/Open"), tr("&Открыть…"), this);
    connect(actMapOpen, SIGNAL(triggered()), SLOT(mapOpen()));
    actMapOpen->setShortcuts(shortcuts(QKeySequence::Open, tr("O")));

#ifdef EDITOR
    actMapSave = new QAction(QIcon(":/Save"), tr("Со&хранить"), this);
    connect(actMapSave, SIGNAL(triggered()), SLOT(mapSave()));
    actMapSave->setShortcuts(shortcuts(QKeySequence::Save, tr("S")));
#endif

    actMapSaveAs = new QAction(QIcon(":/SaveAs"),
                               tr("Сохранить &как…"), this);
    connect(actMapSaveAs, SIGNAL(triggered()), SLOT(mapSaveAs()));
    actMapSaveAs->setShortcuts(shortcuts(QKeySequence::SaveAs, tr("Shift+S")));

    actQuit = new QAction(QIcon(":/Quit"), tr("В&ыход"), this);
    connect(actQuit, SIGNAL(triggered()), SLOT(exit()));
    actQuit->setShortcuts(shortcuts(QKeySequence::Quit, tr("Q")));


#ifdef EDITOR
    actAreaCopy = new QAction(QIcon(":/AreaCopy"),
                              tr("Копировать &область…"), this);
    connect(actAreaCopy, SIGNAL(triggered()), SLOT(areaCopy()));
    actAreaCopy->setShortcuts(shortcuts(QKeySequence::Copy, tr("C")));

    actGraphCopy = new QAction(QIcon(":/GraphCopy"),
                              tr("Копировать &граф…"), this);
    connect(actGraphCopy, SIGNAL(triggered()), SLOT(graphCopy()));
    actGraphCopy->setShortcuts(shortcuts(tr("Ctrl+Shift+C"), tr("Shift+C")));
#endif


#ifdef EDITOR
    actAddBase = new QAction(QIcon(":/AddBase"),
                             tr("Добавить &подложку"), this);
    connect(actAddBase, SIGNAL(triggered()), SLOT(addBase()));

    actLayerBase = new QAction(QIcon(":/LayerBase"), tr("&Подложка"), this);
    connect(actLayerBase, SIGNAL(triggered(bool)),
            SLOT(layerBaseSetVisible(bool)));
    actLayerBase->setShortcuts(shortcuts(tr("Ctrl+B"), tr("B")));
    actLayerBase->setCheckable(true);
    actLayerBase->setChecked(true);

    actLayerGrid = new QAction(QIcon(":/LayerGrid"), tr("&Сетка"), this);
    connect(actLayerGrid, SIGNAL(triggered(bool)),
            SLOT(layerGridSetVisible(bool)));
    QList<QKeySequence> list;
    list << tr("Ctrl+#") << tr("Ctrl+№");
    if (isExtraShortcutsEnabled)
        list << tr("#") << tr("№");
    actLayerGrid->setShortcuts(list);
    actLayerGrid->setCheckable(true);
//    actLayerCrossBase->setChecked(true);

    actLayerGraph = new QAction(QIcon(":/LayerGraph"), tr("&Граф"), this);
    connect(actLayerGraph, SIGNAL(triggered(bool)),
            SLOT(layerGraphSetVisible(bool)));
//    actLayerGraph->setShortcuts(shortcuts(tr("Ctrl+G"), tr("G")));
    actLayerGraph->setCheckable(true);
    actLayerGraph->setChecked(true);
#endif
    actZoomOut= new QAction(QIcon(":/ZoomOut"), tr("У&меньшить"), this);
    connect(actZoomOut, SIGNAL(triggered()), SLOT(zoomOut()));
    actZoomOut->setShortcuts(shortcuts(QKeySequence::ZoomOut/*tr("Ctrl+-")*/, tr("-")));
//    actZoomOut->setShortcut(/*Qt::CTRL + */tr("-"));

    actZoomIn = new QAction(QIcon(":/ZoomIn"), tr("&Увеличить"), this);
    connect(actZoomIn, SIGNAL(triggered()), SLOT(zoomIn()));
    actZoomIn->setShortcuts(shortcuts(QKeySequence::ZoomIn/*tr("Ctrl++")*/, tr("+")));
//    actZoomIn->setShortcut(/*Qt::CTRL + */tr("+"));

    actZoomFit = new QAction(QIcon(":/ZoomFit"), tr("&Вместить"), this);
    connect(actZoomFit, SIGNAL(triggered()), SLOT(zoomFit()));
    actZoomFit->setShortcuts(shortcuts(tr("Ctrl+*"), tr("*")));
//    actZoomFit->setShortcut(/*Qt::CTRL + */tr("*"));

    actHandDrag = new QAction(QIcon(":/HandDrag"),
                              tr("&Панорамирование"), this);
    connect(actHandDrag, SIGNAL(triggered(bool)), SLOT(handDrag(bool)));
    actHandDrag->setShortcuts(shortcuts(tr("Ctrl+H"), tr("H")));
    actHandDrag->setCheckable(true);
#ifdef EDITOR
    actMagnetToExtensions = new QAction(
            QIcon(":/MagnetToExtensions"),
            tr("&Прилипать к продолжениям отрезков"), this);
    connect(actMagnetToExtensions, SIGNAL(triggered(bool)),
            SLOT(magnetToExtensions(bool)));
    actMagnetToExtensions->setShortcut(tr("F5"));
    actMagnetToExtensions->setCheckable(true);
#endif


    actFloorLower = new QAction(QIcon(":/FloorLower"), tr("Cамый низ"), this);
    connect(actFloorLower, SIGNAL(triggered()), SLOT(floorLower()));
//    actFloorLower->setShortcuts(shortcuts(tr("Ctrl+Shift+Down"), Qt::Key_End));
    actFloorLower->setShortcut(Qt::Key_End);

    actFloorDown = new QAction(QIcon(":/Down"), tr("Этаж &ниже"), this);
    connect(actFloorDown, SIGNAL(triggered()), SLOT(floorDown()));
//    actFloorDown->setShortcuts(shortcuts(tr("Ctrl+Down"), Qt::Key_PageDown));
    actFloorDown->setShortcut(Qt::Key_PageDown);

    actFloorUp = new QAction(QIcon(":/Up"), tr("Этаж &выше"),
                                   this);
    connect(actFloorUp, SIGNAL(triggered()), SLOT(floorUp()));
    actFloorUp->setShortcut(Qt::Key_PageUp);

    actFloorUpper = new QAction(QIcon(":/FloorUpper"), tr("Cамый верх"), this);
    connect(actFloorUpper, SIGNAL(triggered()), SLOT(floorUpper()));
    actFloorUpper->setShortcut(Qt::Key_Home);


#ifdef EDITOR
//    actAddWall = new QAction(QIcon(":/AddWall"), tr("Добавить &стену"), this);
//    connect(actAddWall, SIGNAL(triggered()), SLOT(addWall()));
//    actAddWall->setShortcuts(tr("Ctrl+W"), tr("W"));

    actAddArea = new QAction(QIcon(":/AddArea"),
                             tr("Создание &областей"),this);
    connect(actAddArea, SIGNAL(triggered()), SLOT(addArea()));
    actAddArea->setShortcuts(shortcuts(tr("Ctrl+A"), tr("A")));

    actAddDoor = new QAction(QIcon(":/AddDoor"), tr("Создание &дверей"), this);
    connect(actAddDoor, SIGNAL(triggered()), SLOT(addDoor()));
    actAddDoor->setShortcuts(shortcuts(tr("Ctrl+D"), tr("D")));

    actAddNode = new QAction(QIcon(":/AddNode"), tr("Создание &графа"),
                             this);
    connect(actAddNode, SIGNAL(triggered()), SLOT(addNode()));
    actAddNode->setShortcuts(shortcuts(tr("Ctrl+G"), tr("G")));
#endif


    actAbout = new QAction(QIcon(":/Ariadne.svg"), tr("&О программе"), this);
    connect(actAbout, SIGNAL(triggered()), SLOT(about()));
    /*actAboutQT = new QAction(tr("О QT"), this);
    actAboutQT->setStatusTip(tr("Об инструментарий Qt, при помощи которого "
                                "была создана эта программа"));
    connect(actAboutQT, SIGNAL(triggered()), qApp, SLOT(aboutQt()));*/

#ifdef EDITOR
    actPanelFloors = new QAction(QIcon(":/Floors"),
                                         tr("&Этажи"), this);
    actPanelFloors->setData(eFloors);
    actPanelFloors->setShortcuts(shortcuts(tr("Ctrl+1"), tr("1")));
    actPanelFloors->setCheckable(true);

    actFloorAdd = new QAction(QIcon(":/Add"), tr("&Добавить этаж"), this);
    connect(actFloorAdd, SIGNAL(triggered()), SLOT(floorAdd()));
    actFloorAdd->setShortcuts(shortcuts(tr("Ctrl+F"), tr("F")));

    actFloorDelete = new QAction(QIcon(":/Delete"), tr("&Удалить этаж"), this);
    connect(actFloorDelete, SIGNAL(triggered()), SLOT(floorDelete()));
    actFloorDelete->setShortcuts(shortcuts(tr("Ctrl+Alt+F"), tr("Alt+F")));

    actFloorMoveDown = new QAction(QIcon(":/Down"),
                                   tr("Переместить в&низ"), this);
    connect(actFloorMoveDown, SIGNAL(triggered()), SLOT(floorMoveDown()));

    actFloorMoveUp = new QAction(QIcon(":/Up"),
                                 tr("Переместить &вверх"), this);
    connect(actFloorMoveUp, SIGNAL(triggered()), SLOT(floorMoveUp()));

    actFloorSetDefault = new QAction(QIcon(":/FloorDefault"),
                                     tr("Сделать этажом по умол&чанию"), this);
    connect(actFloorSetDefault, SIGNAL(triggered()), SLOT(floorSetDefault()));


    actPanelVerticals = new QAction(QIcon(":/Verticals"),
                                    tr("&Вертикали"), this);
    actPanelVerticals->setData(eVerticals);
    actPanelVerticals->setShortcuts(shortcuts(tr("Ctrl+2"), tr("2")));
    actPanelVerticals->setCheckable(true);

    actVerticalAdd = new QAction(QIcon(":/Add"),
                                 tr("&Добавить вертикаль"), this);
    connect(actVerticalAdd, SIGNAL(triggered()), SLOT(verticalAdd()));
    actVerticalAdd->setShortcuts(shortcuts(tr("Ctrl+V"), tr("V")));

    actVerticalDelete = new QAction(QIcon(":/Delete"),
                                    tr("&Удалить вертикаль"), this);
    connect(actVerticalDelete, SIGNAL(triggered()), SLOT(verticalDelete()));
    actVerticalDelete->setShortcuts(shortcuts(tr("Ctrl+Alt+V"), tr("Alt+V")));

    actVerticalMoveDown = new QAction(QIcon(":/Down"),
                                      tr("Переместить в&низ"), this);
    connect(actVerticalMoveDown, SIGNAL(triggered()), SLOT(verticalMoveDown()));

    actVerticalMoveUp = new QAction(QIcon(":/Up"),
                                    tr("Переместить &вверх"), this);
    connect(actVerticalMoveUp, SIGNAL(triggered()), SLOT(verticalMoveUp()));

//    actVerticalSetPassage = new QAction(QIcon(":/SetPassage"),
//                                        tr("Лифт или лестница"), this);
//    connect(actVerticalSetPassage, SIGNAL(triggered(bool)),
//            SLOT(verticalSetPassage(bool)));
//    actVerticalSetPassage->setShortcuts(shortcuts(tr("Ctrl+L"), tr("L")));
//    actVerticalSetPassage->setCheckable(true);
#endif


    actPanelAreasProperties = new QAction(QIcon(":/AreasProperties"),
                                         tr("&Свойства области"), this);
    actPanelAreasProperties->setData(eAreasProperties);
#ifdef EDITOR
    actPanelAreasProperties->setShortcuts(shortcuts(tr("Ctrl+3"), tr("3")));
#else
    actPanelAreasProperties->setShortcuts(shortcuts(tr("Ctrl+1"), tr("1")));
#endif
    actPanelAreasProperties->setCheckable(true);


    actPanelSearch = new QAction(QIcon(":/Search"), tr("Поиск"), this);
    actPanelSearch->setData(eSearch);
#ifdef EDITOR
    actPanelSearch->setShortcuts(shortcuts(tr("Ctrl+4"), tr("4")));
#else
    actPanelSearch->setShortcuts(shortcuts(tr("Ctrl+2"), tr("2")));
#endif
    actPanelSearch->setCheckable(true);


    actPanelWays = new QAction(QIcon(":/Ways"), tr("&Пути"), this);
    actPanelWays->setData(eWays);
#ifdef EDITOR
    actPanelWays->setShortcuts(shortcuts(tr("Ctrl+5"), tr("5")));
#else
    actPanelWays->setShortcuts(shortcuts(tr("Ctrl+3"), tr("3")));
#endif
    actPanelWays->setCheckable(true);

    actSetStart = new QAction(QIcon(":/SetStart"), tr("Cтарт"), this);
    connect(actSetStart, SIGNAL(triggered()), SLOT(setStart()));

    actSetFinish = new QAction(QIcon(":/SetFinish"), tr("Финиш"), this);
    connect(actSetFinish, SIGNAL(triggered()), SLOT(setFinish()));

//    actAllowStairs = new QAction(QIcon(":/AllowStairs"),
//                                 tr("Разрешить лестницы"), this);
//    actAllowStairs->setCheckable(true);
//    actAllowStairs->setChecked(true);

//    actAllowLifts = new QAction(QIcon(":/AllowLifts"),
//                                tr("Разрешить лифты"), this);
//    actAllowLifts->setCheckable(true);
//    actAllowLifts->setChecked(true);

//    actWay = new QAction(QIcon(":/Way"), tr("Путь"), this);
//    connect(actWay, SIGNAL(triggered()), SLOT(way()));


    actgrpPanels = new QActionGroup(this);
    actgrpPanels->setExclusive(false);
#ifdef EDITOR
    actgrpPanels->addAction(actPanelFloors);
    actgrpPanels->addAction(actPanelVerticals);
#endif
    actgrpPanels->addAction(actPanelAreasProperties);
    actgrpPanels->addAction(actPanelSearch);
    actgrpPanels->addAction(actPanelWays);
    connect(actgrpPanels, SIGNAL(triggered(QAction *)),
            SLOT(actgrpPanelsTriggered(QAction *)));
}

void MainWindow::createMenus()
{
    menuFile = menuBar()->addMenu(tr("&Файл"));
#ifdef EDITOR
    menuFile->addAction(actMapNew);
#endif
    menuFile->addAction(actMapOpen);
#ifdef EDITOR
    menuFile->addAction(actMapSave);
#endif
    menuFile->addAction(actMapSaveAs);
    menuFile->addAction(actQuit);

#ifdef EDITOR
    menuEdit = menuBar()->addMenu(tr("&Правка"));
    menuEdit->addAction(actAreaCopy);
    menuEdit->addAction(actGraphCopy);
#endif

    menuView = menuBar()->addMenu(tr("&Вид"));
#ifdef EDITOR
    menuView->addAction(actAddBase);
//    menuView->addSeparator();
    menuLayers = menuView->addMenu(tr("&Слои"));
    menuLayers->addAction(actLayerBase);
    menuLayers->addAction(actLayerGrid);
    menuLayers->addAction(actLayerGraph);
    menuView->addSeparator();
#endif
    menuZoom = menuView->addMenu(QIcon(":/Zoom"), tr("&Масштаб"));
    menuZoom->addAction(actZoomOut);
    menuZoom->addAction(actZoomIn);
    menuZoom->addAction(actZoomFit);
//    menuView->addSeparator();
    menuView->addAction(actHandDrag);
#ifdef EDITOR
    menuView->addSeparator();
    menuView->addAction(actMagnetToExtensions);
#endif

    menuGo = menuBar()->addMenu(tr("&Перейти"));
    menuGo->addAction(actFloorUp);
    menuGo->addAction(actFloorDown);
    menuGo->addSeparator();
    menuGo->addAction(actFloorUpper);
    menuGo->addAction(actFloorLower);

//    menuPanels = menuBar()->addMenu(tr("Пане&ли"));
//    menuPanels->addAction(actPanelFloorsManagement);
//    menuPanels->addAction(actPanelVerticals);
//    menuPanels->addAction(actPanelAreasMarking);
//    menuPanels->addAction(actPanelWay);

#ifdef EDITOR
    menuAdd = menuBar()->addMenu(tr("&Добавить"));
    menuAdd->addAction(actAddArea);
    menuAdd->addAction(actAddDoor);
    menuAdd->addAction(actAddNode);
#endif

    menuHelp = menuBar()->addMenu(tr("&Справка"));
    menuHelp->addAction(actAbout);
    //menuHelp->addAction(actAboutQT);
}

void MainWindow::createToolBars()
{
    tbrFile = addToolBar(tr("Файл"));
    tbrFile->setFloatable(false);
#ifdef EDITOR
    tbrFile->addAction(actMapNew);
#endif
    tbrFile->addAction(actMapOpen);
#ifdef EDITOR
    tbrFile->addAction(actMapSave);
#else
    tbrFile->addAction(actMapSaveAs);
#endif
//    tbrFile->addAction(actQuit);

//    tbrEdit = addToolBar(tr("Правка"));
//    tbrEdit->setFloatable(false);
//    tbrEdit->addAction(actAreaCopy);
//    tbrEdit->addAction(actGraphCopy);

    tbrView = addToolBar(tr("Вид"));
    tbrView->setFloatable(false);
    tbrView->addAction(actZoomOut);
    tbrView->addAction(actZoomIn);
    tbrView->addAction(actZoomFit);
//    tbrView->addAction(actAddBase);
    tbrView->addAction(actHandDrag);
//    actHandDrag->setVisible(false);
//#ifdef EDITOR
//    tbrView->addAction(actMagnetToExtensions);
//#endif
//    actMagnetToExtensions->setVisible(false);

    tbrGo = addToolBar(tr("Переключение этажей"));
//    addToolBar(Qt::BottomToolBarArea, tbrFloorsSwitching);
    tbrGo->setFloatable(false);
//    QWidget *spacerLeft = new QWidget(this);
//    spacerLeft->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
//    tbrFloorsSwitching->addWidget(spacerLeft);
//    tbrFloorsSwitching->addAction(actFloorLower);
    tbrGo->addAction(actFloorDown);
    cbxFloorSelect = new QComboBox(this);
    cbxFloorSelect->setModel(modelFloorsList);
    connect(cbxFloorSelect,
            SIGNAL(currentIndexChanged(int)), SLOT(setActiveFloor(int)));
    cbxFloorSelect->setEditable(true);
    cbxFloorSelect->setInsertPolicy(QComboBox::NoInsert);
    tbrGo->addWidget(cbxFloorSelect);
    tbrGo->addAction(actFloorUp);
//    tbrFloorsSwitching->addAction(actFloorUpper);
//    QWidget *spacerRight = new QWidget(this);
//    spacerRight ->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
//    tbrFloorsSwitching->addWidget(spacerRight);

//    tbrLayers = addToolBar(tr("Слои"));
//    tbrLayers->setFloatable(false);
//    tbrLayers->addAction(actLayerBase);
//    tbrLayers->addAction(actLayerCrossBase);
//    tbrLayers->addAction(actLayerGraph);

    /*tbrALT = new QToolBar(tr("Изменить название этажа"));
    addToolBar(Qt::BottomToolBarArea, tbrFloorNameChange);
    tbrFloorNameChange->setFloatable(false);
    lblFloorNameChange = new QLabel(tr("Название этажа: "), this);
    tbrFloorNameChange->addWidget(lblFloorNameChange);
    ldtFloorNameChange = new QLineEdit(this);
    connect(ldtFloorNameChange,
            SIGNAL(textChanged(QString)), SLOT(FloorNameChange(QString)));
    tbrFloorNameChange->addWidget(ldtFloorNameChange);*/
#ifdef EDITOR
    tbrAdd = addToolBar(tr("Добавить"));
    tbrAdd->setFloatable(false);
//    tbrAdd->addAction(actAddWall);
    tbrAdd->addAction(actAddArea);
    tbrAdd->addAction(actAddDoor);
    tbrAdd->addAction(actAddNode);
#endif

    tbrPanels = new QToolBar(tr("Панели"));
    addToolBar(Qt::LeftToolBarArea, tbrPanels);
    tbrPanels->setFloatable(false);
#ifdef EDITOR
    tbrPanels->addAction(actPanelFloors);
    tbrPanels->addAction(actPanelVerticals);
#endif
    tbrPanels->addAction(actPanelAreasProperties);
    tbrPanels->addAction(actPanelSearch);
    tbrPanels->addAction(actPanelWays);
}

void MainWindow::createPanelFloors()
{
    dckwgtFloors = new QDockWidget(tr("Этажи"));
    connect(dckwgtFloors, SIGNAL(visibilityChanged(bool)),
            SLOT(panelFloorsVisibilityChanged(bool)));
    addDockWidget(Qt::LeftDockWidgetArea, dckwgtFloors);
    dckwgtFloors->setFeatures(
            QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

    wgtFloors = new QWidget;
    dckwgtFloors->setWidget(wgtFloors);

    vblFloors = new QVBoxLayout(wgtFloors);

    hblFloorsButtons = new QHBoxLayout();

    btnFloorAdd = new QToolButton();
    btnFloorAdd->setDefaultAction(actFloorAdd);
    hblFloorsButtons->addWidget(btnFloorAdd);

    btnFloorDelete = new QToolButton();
    btnFloorDelete->setDefaultAction(actFloorDelete);
    hblFloorsButtons->addWidget(btnFloorDelete);

    btnFloorMoveDown = new QToolButton();
    btnFloorMoveDown->setDefaultAction(actFloorMoveDown);
    hblFloorsButtons->addWidget(btnFloorMoveDown);

    btnFloorMoveUp = new QToolButton();
    btnFloorMoveUp->setDefaultAction(actFloorMoveUp);
    hblFloorsButtons->addWidget(btnFloorMoveUp);

    btnFloorSetDefault = new QToolButton();
    btnFloorSetDefault->setDefaultAction(actFloorSetDefault);
    hblFloorsButtons->addWidget(btnFloorSetDefault);

    hblFloorsButtons->addStretch();
    vblFloors->addLayout(hblFloorsButtons);

    viewFloorsList = new QListView();
//    connect(viewFloorsList, SIGNAL(activated(QModelIndex)),
//            SLOT(viewFloorsListItemActivated(QModelIndex)));
    connect(modelFloorsList, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            SLOT(viewFloorsListItemChanged(QModelIndex)));
    viewFloorsList->setModel(modelFloorsList);
    viewFloorsList->setSelectionModel(selectionFloorsList);
    vblFloors->addWidget(viewFloorsList);
}

void MainWindow::createPanelVerticals()
{
    dckwgtVerticals = new QDockWidget(tr("Вертикали"));
    connect(dckwgtVerticals, SIGNAL(visibilityChanged(bool)),
            SLOT(panelVerticalsVisibilityChanged(bool)));
    addDockWidget(Qt::LeftDockWidgetArea, dckwgtVerticals);
    dckwgtVerticals->setFeatures(
            QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

    wgtVerticals = new QWidget;
    dckwgtVerticals->setWidget(wgtVerticals);

    vblVerticals = new QVBoxLayout(wgtVerticals);

    hblVerticalsButtons = new QHBoxLayout();

    btnVerticalAdd = new QToolButton();
    btnVerticalAdd->setDefaultAction(actVerticalAdd);
    hblVerticalsButtons->addWidget(btnVerticalAdd);

    btnVerticalDelete = new QToolButton();
    btnVerticalDelete->setDefaultAction(actVerticalDelete);
    hblVerticalsButtons->addWidget(btnVerticalDelete);

    btnVerticalMoveDown = new QToolButton();
    btnVerticalMoveDown->setDefaultAction(actVerticalMoveDown);
    hblVerticalsButtons->addWidget(btnVerticalMoveDown);

    btnVerticalMoveUp = new QToolButton();
    btnVerticalMoveUp->setDefaultAction(actVerticalMoveUp);
    hblVerticalsButtons->addWidget(btnVerticalMoveUp);

    cbxVerticalType = new QComboBox();
    cbxVerticalType->insertItem(0, tr("Аудитория"), (int)GraphArc::Room);
    cbxVerticalType->insertItem(1, tr("Лестница"), (int)GraphArc::Stairs);
    cbxVerticalType->insertItem(2, tr("Лифт"), (int)GraphArc::Lift);
//    new QListWidgetItem(tr("Аудитория"), cbxVerticalType);
//    new QListWidgetItem(tr("Лестница"), cbxVerticalType);
//    new QListWidgetItem(tr("Лифт"), cbxVerticalType);
    connect(cbxVerticalType, SIGNAL(currentIndexChanged(int)),
            SLOT(verticalSetTypeCurrentIndexChanged(int)));
    hblVerticalsButtons->addWidget(cbxVerticalType);
//    btnVerticalSetPassage = new QToolButton();
//    btnVerticalSetPassage->setDefaultAction(actVerticalSetPassage);
//    hblVerticalsButtons->addWidget(btnVerticalSetPassage);

    hblVerticalsButtons->addStretch();
    vblVerticals->addLayout(hblVerticalsButtons);

    lstwgtVerticals = new QListWidget();
    connect(lstwgtVerticals, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            SLOT(lstwgtVerticalsCurrentItemChanged(QListWidgetItem*,QListWidgetItem*)));
    connect(lstwgtVerticals, SIGNAL(itemChanged(QListWidgetItem*)),
            SLOT(lstwgtVerticalsItemChanged(QListWidgetItem*)));
    vblVerticals->addWidget(lstwgtVerticals);
//    viewVerticalsList = new QListView();
//    connect(viewVerticalsList, SIGNAL(activated(QModelIndex)),
//            SLOT(viewVerticalsListItemActivated(QModelIndex)));
//    connect(modelVerticalsList, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
//            SLOT(viewVerticalsListItemChanged(QModelIndex)));
//    viewVerticalsList->setModel(modelVerticalsList);
//    viewVerticalsList->setSelectionModel(selectionVerticalsList);
//    vblVerticals->addWidget(viewVerticalsList);
}

void MainWindow::createPanelAreasProperties()
{
    dckwgtAreasProperties = new QDockWidget(tr("Свойства области"));
    connect(dckwgtAreasProperties, SIGNAL(visibilityChanged(bool)),
            SLOT(panelAreasPropertiesVisibilityChanged(bool)));
    addDockWidget(Qt::LeftDockWidgetArea, dckwgtAreasProperties);
    dckwgtAreasProperties->setFeatures(
            QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

    wgtAreasProperties = new QWidget;
    dckwgtAreasProperties->setWidget(wgtAreasProperties);

    vblAreasProperties = new QVBoxLayout(wgtAreasProperties);

    lblAreaNumber = new QLabel(tr("Номер: "));
    vblAreasProperties->addWidget(lblAreaNumber);

    ldtAreaNumber = new QLineEdit();
    connect(ldtAreaNumber, SIGNAL(textChanged(QString)), SLOT(setAreaNumber()));
#ifndef EDITOR
    ldtAreaNumber->setReadOnly(true);
#endif
    vblAreasProperties->addWidget(ldtAreaNumber);

    lblAreaName = new QLabel(tr("Название: "));
    vblAreasProperties->addWidget(lblAreaName);

    ldtAreaName = new QLineEdit();
    connect(ldtAreaName, SIGNAL(textChanged(QString)), SLOT(setAreaName()));
#ifndef EDITOR
    ldtAreaName->setReadOnly(true);
#endif
//    ldtAreaName->setMaximumHeight(
//            (ptdtAreaName->fontMetrics().lineSpacing() +
//             ptdtAreaName->font().pointSize())*2);
//    // ### Must be rewrited
//    ptdtAreaName->setMinimumWidth(cDockWidth);
//    ptdtAreaName->setMinimumWidth(0);
    vblAreasProperties->addWidget(ldtAreaName);

    lblAreaDescription = new QLabel(tr("Описание: "));
    vblAreasProperties->addWidget(lblAreaDescription);

    ptdtAreaDescription = new QPlainTextEdit("");
    connect(ptdtAreaDescription, SIGNAL(textChanged()),
            SLOT(setAreaDescription()));
#ifndef EDITOR
    ptdtAreaDescription->setReadOnly(true);
#endif
    vblAreasProperties->addWidget(ptdtAreaDescription);

#ifdef EDITOR
    btnUpdateAreaInscription = new QPushButton(tr("Обновить надпись"));
    connect(btnUpdateAreaInscription, SIGNAL(clicked()),
            SLOT(updateAreaInscription()));
    vblAreasProperties->addWidget(btnUpdateAreaInscription);

    lblAreaInscription = new QLabel(tr("Надпись: "));
    vblAreasProperties->addWidget(lblAreaInscription);

    ptdtAreaInscription = new QPlainTextEdit("");
    connect(ptdtAreaInscription, SIGNAL(textChanged()),
            SLOT(setAreaInscription()));
    vblAreasProperties->addWidget(ptdtAreaInscription);
#endif

//    grpbxFieldVisible = new QGroupBox(tr("Показывать на плане: "));
//    vblAreasMarking->addWidget(grpbxFieldVisible);

//    vblFieldVisible = new QVBoxLayout(grpbxFieldVisible);

//    chkAreaNumberVisible = new QCheckBox(tr("Номер"));
//    connect(chkAreaNumberVisible, SIGNAL(stateChanged(int)),
//            SLOT(chkAreaNumberVisibleStateChanged(int)));
//    vblFieldVisible->addWidget(chkAreaNumberVisible);

//    chkAreaNameVisible = new QCheckBox(tr("Название"));
//    connect(chkAreaNameVisible, SIGNAL(stateChanged(int)),
//            SLOT(chkAreaNameVisibleStateChanged(int)));
//    vblFieldVisible->addWidget(chkAreaNameVisible);

//    grpbxFieldVisible->setLayout(vblFieldVisible);
}

void MainWindow::createPanelSearch()
{
    dckwgtSearch = new QDockWidget(tr("Поиск"));
    connect(dckwgtSearch, SIGNAL(visibilityChanged(bool)),
            SLOT(panelSearchVisibilityChanged(bool)));
    addDockWidget(Qt::LeftDockWidgetArea, dckwgtSearch);
    dckwgtSearch->setFeatures(
            QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

    wgtSearch = new QWidget;
    dckwgtSearch->setWidget(wgtSearch);

    vblSearch = new QVBoxLayout(wgtSearch);

    ldtInput = new QLineEdit();
    connect(ldtInput, SIGNAL(textEdited(QString)),
            SLOT(ldtInputTextEdited(QString)));
    ldtInput->setPlaceholderText(
            tr("Начните вводить номер, название или описание области"));
    vblSearch->addWidget(ldtInput);

    lblAreasNumber = new QLabel();
    lblAreasNumber->hide();
    vblSearch->addWidget(lblAreasNumber);

    lstwgtAreas = new QListWidget();
    connect(lstwgtAreas,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            SLOT(lstwgtSearchCurrentItemChanged(
                    QListWidgetItem*,QListWidgetItem*)));
    vblSearch->addWidget(lstwgtAreas);
}

void MainWindow::createPanelWays()
{
    dckwgtWay = new QDockWidget(tr("Пути"));
    connect(dckwgtWay, SIGNAL(visibilityChanged(bool)),
            SLOT(panelWaysVisibilityChanged(bool)));
    addDockWidget(Qt::LeftDockWidgetArea, dckwgtWay);
    dckwgtWay->setFeatures(
            QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

    wgtWay = new QWidget;
    dckwgtWay->setWidget(wgtWay);

    vblWay = new QVBoxLayout(wgtWay);

    hblWayButtons = new QHBoxLayout();

    btnSetStart = new QToolButton();
    btnSetStart->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnSetStart->setDefaultAction(actSetStart);
    hblWayButtons->addWidget(btnSetStart);

    btnSetFinish = new QToolButton();
    btnSetFinish->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnSetFinish->setDefaultAction(actSetFinish);
    hblWayButtons->addWidget(btnSetFinish);

//    btnAllowStairs = new QToolButton();
//    btnAllowStairs->setDefaultAction(actAllowStairs);
//    hblWayButtons->addWidget(btnAllowStairs);

//    btnAllowLifts = new QToolButton();
//    btnAllowLifts->setDefaultAction(actAllowLifts);
//    hblWayButtons->addWidget(btnAllowLifts);

//    btnWay = new QToolButton();
//    btnWay->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
//    btnWay->setDefaultAction(actWay);
//    hblWayButtons->addWidget(btnWay);

    vblWay->addLayout(hblWayButtons);
//    vblWay->addSpacing(5);

    grpbxWay = new QGroupBox();
    vblWay->addWidget(grpbxWay);

    vblgrpbxWay = new QVBoxLayout(grpbxWay);

    lblWaysChoice = new QLabel();
    lblWaysChoice->setWordWrap(true);
    vblgrpbxWay->addWidget(lblWaysChoice);
//    vblWay->addWidget(lblWaysChoice);

    lstwgtWays = new QListWidget();
    connect(lstwgtWays, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            SLOT(lstwgtWaysCurrentItemChanged(QListWidgetItem*,QListWidgetItem*)));
//    connect(lstwgtWays, SIGNAL(itemActivated(QListWidgetItem*)),
//            SLOT(lstwgtWaysItemActivated(QListWidgetItem*)));
//    lstwgtWays->setPalette(QPalette(Qt::transparent));
//    lstwgtWays->setStyle(new QMotifStyle());
//    QString style;
//    style += "QListWidget {background-color: transparent;}";
//    style += "QListWidget::item {background-color: transparent;}";
//    style += "QListWidget::item {selection-color: white;}";
//    style += "QListWidget::item {color: black;}";
//    lstwgtWays->setStyleSheet(style);
    vblgrpbxWay->addWidget(lstwgtWays);

    lblWayInfo = new QLabel();
    lblWayInfo->setWordWrap(true);
//            tr("Для того, чтобы найти путь\nвыберите стартовое помещение или\n"
//               "дверь и нажмите кнопку \"Старт\".\nАналогично обозначьте финиш.\n"
//               "Программа выведет список\nкратчайших маршрутов (с учётом средств"
//               "передвижения)."));
    vblgrpbxWay->addWidget(lblWayInfo);

    vblWay->addStretch();

    freePanelWays();
}

void MainWindow::blockAndFreePanelAreasProperties()
{
    setState(eAreasProperties, stSave, stFalse);
    ldtAreaNumber->setText("");
    ldtAreaName->setText("");
    ptdtAreaDescription->setPlainText("");
#ifdef EDITOR
    ptdtAreaInscription->setPlainText("");
#endif
//    chkAreaNumberVisible->setChecked(false);
//    chkAreaNameVisible->setChecked(false);
}

void MainWindow::freePanelWays()
{
    lblWaysChoice->setText(tr("Для того, чтобы найти путь выберите объект, "
                              "откуда вы хотите начать путь и нажмите кнопку "
                              "\"Старт\". Аналогично обозначьте финиш."));
    lstwgtWays->setVisible(false);
    lblWayInfo->setText("");
}

void MainWindow::updateAreaInscription()
{
    QString number = ldtAreaNumber->text();
    QString name = ldtAreaName->text();
    QString description = ptdtAreaDescription->toPlainText();
    QString inscription = "";
    inscription += number;
    if (!name.isEmpty() & !inscription.isEmpty())
        inscription += "\n";
    inscription += name;
    if (!description.isEmpty() & !inscription.isEmpty())
        inscription += "\n";
    inscription += description;
    ptdtAreaInscription->setPlainText(inscription);
//    MapSelection *selection = map->floor(curFloor)->selection();
//    if (!selection->isEmpty())
//        if (selection->item()->type() == MapArea::Type)
//        {
//            ptdtAreaInscription->setPlainText(
//                    qgraphicsitem_cast<MapArea*>(
//                            selection->item())->info("%1/n%2/n%3"));
            ptdtAreaInscription->document()->setModified(false);
//        }
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
    }
    else
    {
#ifdef EDITOR
        lstwgtVerticals->clear();
#endif
        modelFloorsList->setStringList(QStringList());
        lstwgtAreas->clear();
        curFloor = -1;
//        cbxFloorSelect->clear();
    }
#ifndef EDITOR
    layerBaseSetVisible(false);
    layerGraphSetVisible(false);
#endif
//    blockAndFreePanelAreasProperties();
    freePanelWays();
    switchMode(MapFloor::Idle);
}

void MainWindow::setState(Elements elem, State visible, State enable)
{
//    bool enable = (st == stEnable_Visible);
//    bool visible = (st != stDisable_Unvisible);
    if (visible != stSave)
    {
        if (elem & eFile)
        {
#ifdef EDITOR
            actMapNew->setVisible(visible);
            actMapSave->setVisible(visible);
#endif
//            else
//            {
//                actMapNew->setVisible(false);
//                actMapSave->setVisible(false);
//            }
            actMapOpen->setVisible(visible);
            actMapSaveAs->setVisible(visible);
            actQuit->setVisible(visible);

            menuFile->menuAction()->setVisible(visible);
            tbrFile->setVisible(visible);
        }
#ifdef EDITOR
        if ((elem & eEdit) > 0)
        {
            actAreaCopy->setVisible(visible);
            actGraphCopy->setVisible(visible);

            menuEdit->menuAction()->setVisible(visible);
//            tbrEdit->setVisible(visible);
        }
#endif
        if (elem & eView)
        {
#ifdef EDITOR
            actAddBase->setVisible(visible);
            actMagnetToExtensions->setVisible(visible);
#endif
            actHandDrag->setVisible(visible);
#ifdef EDITOR
            actLayerBase->setVisible(visible);
            actLayerGrid->setVisible(visible);
            actLayerGraph->setVisible(visible);
#endif
            actZoomOut->setVisible(visible);
            actZoomIn->setVisible(visible);
            actZoomFit->setVisible(visible);

            menuView->menuAction()->setVisible(visible);
            tbrView->setVisible(visible);
        }
        if (elem & eGo)
        {
            actFloorLower->setVisible(visible);
            actFloorDown->setVisible(visible);
            cbxFloorSelect->setVisible(visible);
            actFloorUp->setVisible(visible);
            actFloorUpper->setVisible(visible);

            menuGo->menuAction()->setVisible(visible);
            tbrGo->setVisible(visible);
        }
//        if (elem & eLayers)
//        {

////            tbrLayers->setVisible(visible);
//        }
#ifdef EDITOR
        if ((elem & eAdd) > 0)
        {
//            actAddWall->setVisible(visible);
            actAddArea->setVisible(visible);
            actAddDoor->setVisible(visible);
            actAddNode->setVisible(visible);

            menuAdd->menuAction()->setVisible(visible);
            tbrAdd->setVisible(visible);
        }
#endif
        if (elem & eHelp)
        {
            actAbout->setVisible(visible);

            menuHelp->menuAction()->setVisible(visible);
        }
        if (elem & ePanels)
        {
            actgrpPanels->setVisible(visible);
#ifdef EDITOR
            actPanelFloors->setVisible(visible);
            actPanelVerticals->setVisible(visible);
#endif
            actPanelAreasProperties->setVisible(visible);
            actPanelSearch->setVisible(visible);
            actPanelWays->setVisible(visible);

//            menuPanels->menuAction()->setVisible(visible);
            tbrPanels->setVisible(visible);
        }
#ifdef EDITOR
        if ((elem & eFloors) > 0)
        {
            dckwgtFloors->setVisible(visible);
            if (!visible)
                if (view)
                    setFocus();
        }
        if ((elem & eVerticals) > 0)
        {
            dckwgtVerticals->setVisible(visible);
            if (!visible)
                if (view)
                    setFocus();
        }
#endif
        if (elem & eAreasProperties)
        {
            dckwgtAreasProperties->setVisible(visible);
            if (!visible)
                if (view)
                    setFocus();
//            if (m_program != pFull)
//            {
//                btnUpdateAreaInscription->setVisible(false);
//                lblAreaInscription->setVisible(false);
//                ptdtAreaInscription->setVisible(false);
//            }
        }
        if (elem & eSearch)
        {
            dckwgtSearch->setVisible(visible);
            if (!visible)
                if (view)
                    setFocus();
        }
        if (elem & eWays)
        {
            dckwgtWay->setVisible(visible);
            if (!visible)
                if (view)
                    setFocus();
        }
    }
    if (enable != stSave)
    {
        if (elem & eFile)
        {
#ifdef EDITOR
            actMapNew->setEnabled(enable);
            actMapSave->setEnabled(enable);
#endif
            actMapOpen->setEnabled(enable);
            actMapSaveAs->setEnabled(enable);
            actQuit->setEnabled(enable);

            menuFile->menuAction()->setEnabled(enable);
            tbrFile->setEnabled(enable);
        }
#ifdef EDITOR
        if ((elem & eEdit) > 0)
        {
            actAreaCopy->setEnabled(enable);
            actGraphCopy->setEnabled(enable);

            menuEdit->menuAction()->setEnabled(enable);
//            tbrEdit->setEnabled(enable);
        }
#endif
        if (elem & eView)
        {
#ifdef EDITOR
            actAddBase->setEnabled(enable);
            actMagnetToExtensions->setEnabled(enable);
#endif
            actHandDrag->setEnabled(enable);
#ifdef EDITOR
            actLayerBase->setEnabled(enable);
            actLayerGrid->setEnabled(enable);
            actLayerGraph->setEnabled(enable);
#endif
            actZoomOut->setEnabled(enable);
            actZoomIn->setEnabled(enable);
            actZoomFit->setEnabled(enable);

            menuView->menuAction()->setEnabled(enable);
            tbrView->setEnabled(enable);
        }
        if (elem & eGo)
        {
            if (map)
            {
                if (curFloor == map->floorsNumber() - 1)
                {
                    actFloorLower->setEnabled(false);
                    actFloorDown->setEnabled(false);
                }
                else
                {
                    actFloorLower->setEnabled(enable);
                    actFloorDown->setEnabled(enable);
                }
            }
            cbxFloorSelect->setEnabled(enable);
            if (curFloor == 0)
            {
                actFloorUp->setEnabled(false);
                actFloorUpper->setEnabled(false);
            }
            else
            {
                actFloorUp->setEnabled(enable);
                actFloorUpper->setEnabled(enable);
            }

            menuGo->setEnabled(enable);
            tbrGo->setEnabled(enable);
        }

//        if (elem & eLayers)
//        {

////            tbrLayers->setEnabled(enable);
//        }
#ifdef EDITOR
        if ((elem & eAdd) > 0)
        {
//            actAddWall->setEnabled(enable);
            actAddArea->setEnabled(enable);
            actAddDoor->setEnabled(enable);
            actAddNode->setEnabled(enable);

            menuAdd->menuAction()->setEnabled(enable);
            tbrAdd->setEnabled(enable);
        }
#endif

        if (elem & eHelp)
        {
            actAbout->setEnabled(enable);

            menuHelp->menuAction()->setEnabled(enable);
//            tbrAdd->setEnabled(b);
        }
        if (elem & ePanels)
        {
#ifdef EDITOR
            actPanelFloors->setEnabled(enable);
            actPanelVerticals->setEnabled(enable);
#endif
            actPanelAreasProperties->setEnabled(enable);
            actPanelSearch->setEnabled(enable);
            actPanelWays->setEnabled(enable);

            actgrpPanels->setEnabled(enable);
//            menuPanels->menuAction()->setEnabled(enable);
            tbrPanels->setEnabled(enable);
        }
#ifdef EDITOR
        if (elem & eFloors)
        {
            wgtFloors->setEnabled(enable);

//            actPanelFloorsManagement->setEnabled(enable);
            //        dckwgtFloorsManagement->setEnabled(en);
        }
        if (elem & eVerticals)
        {
            wgtVerticals->setEnabled(enable);

//            actPanelVerticals->setEnabled(enable);
            //        dckwgtFloorsManagement->setEnabled(en);
        }
#endif

        if (elem & eAreasProperties)
        {
            if (!enable)
                dckwgtAreasProperties->setVisible(false);
            wgtAreasProperties->setEnabled(enable);
//            if (m_program != pFull)
//            {
//                ldtAreaNumber->setReadOnly(true);
//                ldtAreaName->setReadOnly(true);
//                ptdtAreaDescription->setReadOnly(true);
//                btnUpdateAreaInscription->setEnabled(false);
//                lblAreaInscription->setEnabled(false);
//                ptdtAreaInscription->setEnabled(false);
//            }

//            actPanelAreasMarking->setEnabled(enable);
            //        dckwgtAreasMarking->setEnabled(en);
            //        ptdtRoomName->setEnabled(en);
            //        ptdtRoomDescription->setEnabled(en);
        }
        if (elem & eSearch)
        {
            wgtWay->setEnabled(enable);
        }
        if (elem & eWays)
        {
            wgtWay->setEnabled(enable);
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

void MainWindow::swapVerticals(int x, int y)
{
    QListWidgetItem *itemX = lstwgtVerticals->item(x);
    QListWidgetItem *itemY = lstwgtVerticals->item(y);
    lstwgtVerticals->blockSignals(true);
    QString str = itemX->text();
    itemX->setText(itemY->text());
    itemY->setText(str);
    lstwgtVerticals->blockSignals(false);
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
            s << cMagicNumber << *map << defaultFloor
                    << map->areasAutoRenaming();
        }
        // Copy
        QString copyFile = QTime::currentTime().toString("hh.mm.ss") + ".bld";
        QFile copy(copyFile);
        if (copy.open(QIODevice::WriteOnly))
        {
            QDataStream s(&copy);
            s.setVersion(QDataStream::Qt_4_7);
            s << cMagicNumber << *map << defaultFloor
                    << map->areasAutoRenaming();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef EDITOR
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
#else
    event->accept();
#endif
}

void MainWindow::mapNew()
{
    DialogMapSize* pDialog = new DialogMapSize(this);
    if (pDialog->exec() == QDialog::Accepted)
    {
        openFileName = cNewFileName;
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
    // ### Переспросить при открытии о сохранении
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
            openFileName = f.fileName();
            if (map)
                delete map;
            map = new Map(0, 0, 0, 0, this);
            bool b;
            s >> *map >> defaultFloor >> b;
            map->setPixPerDisplayM(displayPixPerM(width(), widthMM()));
            map->setAreasAutoRenaming(b);
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
#ifdef EDITOR
            lstwgtVerticals->blockSignals(true);
            for (int i = 0; i != map->verticalsNumber(); i++)
            {
                QListWidgetItem *item =
                        new QListWidgetItem(map->vertical(i)->name());
                item->setFlags (item->flags () | Qt::ItemIsEditable);
                lstwgtVerticals->insertItem(lstwgtVerticals->count(), item);
            }
            lstwgtVerticals->blockSignals(false);
#endif
            view->fitInView(view->sceneRect(), Qt::KeepAspectRatio);
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
    if (openFileName != cNewFileName)
        mapSave(openFileName);
    else
        mapSaveAs();
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
    if (map->floor(curFloor)->selection()->item() != 0)
        if (map->floor(curFloor)->selection()->item()->type() == MapArea::Type)
        {
            MapArea *oldArea = qgraphicsitem_cast<MapArea*>(
                    map->floor(curFloor)->selection()->item());
            int i;
            if (curFloor > 0)
                i = curFloor - 1;
            else
                if (curFloor < map->floorsNumber() - 1)
                    i = curFloor + 1;
                else
                {
                    QMessageBox::warning(
                            0, tr("Ошибка при копировании области"),
                            tr("В здании только 1 этаж. Копирование области на "
                               "этаж, на котором она находится невозможно."));
                    return;
                }
            DialogFloorChoice* dialog = new DialogFloorChoice(
                    this, modelFloorsList, i, tr("Выберите этаж, на который "
                                                 "будет\nскопирована "
                                                 "выделенная область:"));
            if (dialog->exec() == QDialog::Accepted)
            {
                int floorTo = dialog->floor();
                if (map->areaCopy(oldArea, curFloor, floorTo))
                    setActiveFloor(floorTo);
            }
        }
}

void MainWindow::graphCopy()
{
    int i;
    if (curFloor > 0)
        i = curFloor - 1;
    else
        if (curFloor < map->floorsNumber() - 1)
            i = curFloor + 1;
        else
        {
            QMessageBox::warning(0, tr("Ошибка при копировании графа"),
                                 tr("В здании только 1 этаж. Копирование "
                                    "графа на этаж, на котором он "
                                    "находится невозможно."));
            return;
        }
    DialogFloorChoice* dialog = new DialogFloorChoice(
            this, modelFloorsList, i, tr("Выберите этаж, на который будет"
                                         "\nскопирована часть графа,"
                                         "\nнаходящаяся на данном этаже:"));
    if (dialog->exec() == QDialog::Accepted)
    {
        int floor = dialog->floor();
        if (floor != curFloor)
        {
            map->graph()->copyFloor(map->floor(curFloor)->uin(),
                                    map->floor(floor)->uin());
            setActiveFloor(floor);
        }
        else
        {
            QMessageBox::warning(0, tr("Ошибка при копировании графа"),
                                 tr("Выбран этаж, совпадающий с текущим. "
                                    "Выберите другой этаж."));
            return;
        }
    }
}

void MainWindow::floorLower()
{
    setActiveFloor(map->floorsNumber() - 1);
}

void MainWindow::floorDown()
{
    setActiveFloor(curFloor + 1);
}

void MainWindow::floorUp()
{
    setActiveFloor(curFloor - 1);
}

void MainWindow::floorUpper()
{
    setActiveFloor(0);
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

void MainWindow::zoomOn(QGraphicsItem *item)
{
    QRectF rect = item->mapRectToScene(item->boundingRect());
    qreal x, y, width, height;
    qreal k = 0.33;
    x = qMax(rect.x() - rect.width()/k, view->sceneRect().x());
    y = qMax(rect.y() - rect.width()/k, view->sceneRect().y());
    width = qMin(rect.width() + 2*(rect.x() - x),
                 view->sceneRect().right() - x);
    height = qMin(rect.height() + 2*(rect.y() - y),
                  view->sceneRect().bottom() - y);
    rect.setRect(x, y, width, height);

    quint32 floorUin;
    switch (item->type())
    {
    case MapArea::Type:
        floorUin = qgraphicsitem_cast<MapArea*>(item)->floorUin();
        break;
    case MapDoor::Type:
        floorUin = qgraphicsitem_cast<MapDoor*>(item)->floorUin();
        break;
    case GraphNode::Type:
        floorUin = qgraphicsitem_cast<GraphNode*>(item)->floorUin();
        break;
    default:
        floorUin = 0;
    }
    if (floorUin > 0)
    {
        MapFloor *floor = map->floorByUin((floorUin));
        setActiveFloor(map->floorNumber(floor));
        floor->selection()->clear();
        floor->selection()->addItem(item);
        switchMode(MapFloor::Selection);
    }
    view->fitInView(rect, Qt::KeepAspectRatio);
}

void MainWindow::handDrag(bool checked)
{
    if (checked)
        view->setDragMode(QGraphicsView::ScrollHandDrag);
    else
        view->setDragMode(QGraphicsView::NoDrag);
}

void MainWindow::addBase()
{
    QString str = QFileDialog::getOpenFileName(
            this, tr("Выберите подложку"), "",
            tr("Изображения(*.jpg *.jpeg *.bmp *.png)"));
    if (!str.isNull())
    {
        map->floor(curFloor)->addBase(str);
        if (map->floor(curFloor)->isBaseExist())
        {
            actLayerBase->setChecked(true);
            actLayerBase->setEnabled(true);
        }
    }
}

void MainWindow::layerBaseSetVisible(bool visible)
{
    for (int i = 0; i != map->floorsNumber(); i++)
        map->floor(i)->baseSetVisible(visible);
}

void MainWindow::layerGridSetVisible(bool visible)
{
    for (int i = 0; i != map->floorsNumber(); i++)
        map->floor(i)->gridSetVisible(visible);
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
    modelFloorsList->setData(modelFloorsList->index(0), tr("Новый этаж"));
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
//    modelFloorsList->blockSignals(true);
    modelFloorsList->removeRow(curFloor);
//    modelFloorsList->blockSignals(false);
//    setActiveFloor(curFloor);
//    if (curFloor > 0)
//        setActiveFloor(curFloor - 1);
//    else
//        setActiveFloor(curFloor + 1);
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

//void MainWindow::addWall()
//{
//    switchMode(MapFloor::WallAdd);
//}

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
    QString fileName = QFileInfo(openFileName).fileName();
    QString modeName, windowTitle;
    switch (m)
    {
    case MapFloor::Idle:
        blockAndFreePanelAreasProperties();
#ifdef EDITOR
        setState(eFile | eEdit | eView | eGo | eAdd | eHelp | ePanels,
                 stTrue, stTrue);
        setState(eFloors | eVerticals | eSearch  | eWays, stSave, stTrue);
#else
        setState(eFile | eView | eGo | eHelp | ePanels, stTrue, stTrue);
        setState(eEdit | eAdd | eFloors | eVerticals,
                 stFalse, stFalse);
        setState(eSearch | eWays, stSave, stTrue);
#endif
        setCursor(Qt::ArrowCursor);
        break;
//    case MapFloor::WallAdd:
    case MapFloor::AreaAdd:
    case MapFloor::DoorAdd:
    case MapFloor::NodeAdd:
        modeName = MapFloor::modeName(m);
        setState(eFile | eView | eHelp, stTrue, stTrue);
        setState(eEdit | eGo | eAdd | ePanels | eFloors | eVerticals |
                 eAreasProperties | eSearch  | eWays, stSave, stFalse);
        setCursor(Qt::CrossCursor);
        if (m == MapFloor::NodeAdd)
            map->graphStartAnew();
        break;
    case MapFloor::Selection:
        {
            bool selectionIsValid = false;
            if (map->floor(curFloor)->selection()->item() != 0)
                if (map->floor(curFloor)->selection()->item()->type() ==
                                                                MapArea::Type)
                {
                    selectionIsValid = true;
                    setState(eAreasProperties/* | eWays*/, stSave, stTrue);
                    MapArea *area = qgraphicsitem_cast<MapArea*>(
                            map->floor(curFloor)->selection()->item());
                    bool isModified = false;
                    isModified = (!area->number().isEmpty() |
                                  !area->name().isEmpty() |
                                  !area->description().isEmpty() |
                                  !area->inscription().isEmpty());
#ifdef EDITOR
                    ptdtAreaInscription->document()->setModified(isModified);
#endif
                    ldtAreaNumber->setText(area->number());
                    ldtAreaName->setText(area->name());
                    ptdtAreaDescription->setPlainText(area->description());
#ifdef EDITOR
                    ptdtAreaInscription->setPlainText(area->inscription());
                    ptdtAreaInscription->document()->setModified(isModified);
#endif
                    ldtAreaNumber->selectAll();
                    if (dckwgtAreasProperties->isVisible())
                        ldtAreaNumber->setFocus();;
                }
            if (!selectionIsValid)
            {
                blockAndFreePanelAreasProperties();
//                setState(eWays, stSave, stTrue);
            }
            break;
        }
    }
    windowTitle = fileName + " - " + qApp->applicationName();
    if (!modeName.isNull())
        windowTitle += " (" + modeName + ")";
    setWindowTitle(windowTitle);
    mode = m;
    if ((map->floorsNumber() > 0) & (curFloor > -1))
        if (map->floor(curFloor)->mode() != m)
            map->floor(curFloor)->setMode(m);
}

void MainWindow::setActiveFloor(int i)
{
    if ((i >= 0) & (i != curFloor) & (i < map->floorsNumber()))
    {
        if ((curFloor >= 0) & (curFloor != i) &
            (curFloor < map->floorsNumber()))
        {
            map->floor(curFloor)->disconnect(
                    SIGNAL(modeChanged(MapFloor::Mode)));
            map->floor(curFloor)->disconnect(
                    SIGNAL(mouseDoubleClicked()));
            map->floor(curFloor)->disconnect(
                    SIGNAL(mouseMiddleButtonClicked(QGraphicsItem*)));
        }
        curFloor = i;
//        QModelIndex index = modelFloorsList->index(i);
//        selectionFloorsList->setCurrentIndex(
//                index, QItemSelectionModel::ClearAndSelect);
        cbxFloorSelect->blockSignals(true);
        cbxFloorSelect->setCurrentIndex(i);
        cbxFloorSelect->blockSignals(false);
        view->setScene(map->floor(i));
        connect(map->floor(i), SIGNAL(modeChanged(MapFloor::Mode)),
                SLOT(switchMode(MapFloor::Mode)));
        connect(map->floor(i), SIGNAL(mouseDoubleClicked()),
                SLOT(mouseDoubleClicked()));
        connect(map->floor(i), SIGNAL(mouseMiddleButtonClicked(QGraphicsItem*)),
                SLOT(mouseMiddleButtonClicked(QGraphicsItem*)));
        setFocus();
        bool switchToSelection = false;
        if (map->floor(i)->selection()->item() != 0)
            if ((mode == MapFloor::Idle) | (mode == MapFloor::Selection))
            {
                switchMode(MapFloor::Selection);
                switchToSelection = true;
            }
        if (!switchToSelection & (map->floor(i)->mode() != mode))
            switchMode(mode);

        if (i == 0)
        {
#ifdef EDITOR
            actFloorMoveUp->setEnabled(false);
#endif
            actFloorUp->setEnabled(false);
            actFloorUpper->setEnabled(false);
        }
        else
        {
#ifdef EDITOR
            actFloorMoveUp->setEnabled(true);
#endif
            actFloorUp->setEnabled(true);
            actFloorUpper->setEnabled(true);
        }
        if (i == map->floorsNumber() - 1)
        {
#ifdef EDITOR
            actFloorMoveDown->setEnabled(false);
#endif
            actFloorLower->setEnabled(false);
            actFloorDown->setEnabled(false);
        }
        else
        {
#ifdef EDITOR
            actFloorMoveDown->setEnabled(true);
#endif
            actFloorLower->setEnabled(true);
            actFloorDown->setEnabled(true);
        }
#ifdef EDITOR
        bool isDefaultFloor = map->floor(i)->uin() == defaultFloor;
        actFloorSetDefault->setChecked(isDefaultFloor);
        actFloorSetDefault->setEnabled(!isDefaultFloor);
#endif
    }
}

void MainWindow::panelFloorsVisibilityChanged(bool visible)
{
    actPanelFloors->setChecked(visible);
}

void MainWindow::viewFloorsListCurrentItemChanged(const QModelIndex &current,
                                                  const QModelIndex &previous)
{
    int floor = current.row();
    if (curFloor != floor)
        setActiveFloor(floor);
}

void MainWindow::viewFloorsListItemChanged(QModelIndex index)
{
    map->floor(curFloor)->setName(modelFloorsList->data(
            index, Qt::DisplayRole).toString());
}

void MainWindow::panelVerticalsVisibilityChanged(bool visible)
{
    actPanelVerticals->setChecked(visible);
    if (!visible)
        map->deselectVertical();
}

void MainWindow::verticalAdd()
{
    map->addVertical();
    QListWidgetItem *item =
            new QListWidgetItem(tr("Новая вертикаль"), lstwgtVerticals);
    item->setFlags(item->flags () | Qt::ItemIsEditable);
//    map->vertical(lstwgtVerticals->row(item))->setName(item->text());
//    lstwgtVerticals->insertItem(->count(), item);
    lstwgtVerticals->setCurrentItem(item);
    cbxVerticalType->setCurrentIndex(-1);
    cbxVerticalType->setCurrentIndex(0);
}

void MainWindow::verticalDelete()
{
    int i = lstwgtVerticals->currentRow();
    map->deleteVertical(i);
    lstwgtVerticals->takeItem(i);
//    lstwgtVerticals->setCurrentItem(lstwgtVerticals->item(i));
//    lstwgtVerticalsCurrentItemChanged();
}

void MainWindow::verticalMoveDown()
{
    int i = lstwgtVerticals->currentRow();
    map->swapVerticals(i, i + 1);
    swapVerticals(i, i + 1);
    lstwgtVerticals->setCurrentRow(i + 1);
}

void MainWindow::verticalMoveUp()
{
    int i = lstwgtVerticals->currentRow();
    map->swapVerticals(i, i - 1);
    swapVerticals(i, i - 1);
    lstwgtVerticals->setCurrentRow(i - 1);
}

void MainWindow::verticalSetTypeCurrentIndexChanged(int index)
{
    int i = lstwgtVerticals->currentRow();
    if ((i > -1) & (index > -1))
    {
        int type = cbxVerticalType->itemData(index).toInt();
        map->vertical(i)->setType((GraphArc::VerticalType)type);
    }
}

void MainWindow::lstwgtVerticalsCurrentItemChanged(QListWidgetItem *current,
                                                   QListWidgetItem *previous)
{
    int i = lstwgtVerticals->row(current);
//    actVerticalSetPassage->setChecked(map->typeVertical(i));
    if (i > -1)
    {
        if (i == 0)
            actVerticalMoveUp->setEnabled(false);
        else
            actVerticalMoveUp->setEnabled(true);
        if (i == lstwgtVerticals->count() - 1)
            actVerticalMoveDown->setEnabled(false);
        else
            actVerticalMoveDown->setEnabled(true);
        cbxVerticalType->setCurrentIndex(map->vertical(i)->type());
        map->selectVertical(i);
//        map->updateVertical(i);
    }
}

void MainWindow::lstwgtVerticalsItemChanged(QListWidgetItem *item)
{
    int i = lstwgtVerticals->currentRow();
    if (i > -1)
        map->vertical(i)->setName(item->text());
}

//void MainWindow::view
//{
//    int floor = index.row();
//    if (curFloor != floor)
//        setActiveFloor(floor);
//}

//void MainWindow::viewFloorsListItemChanged(QModelIndex index)
//{
//    map->floor(curFloor)->setName(modelFloorsList->data(
//            index, Qt::DisplayRole).toString());
//}

void MainWindow::mouseDoubleClicked()
{
//    if (!m_isAriadne)
        actPanelAreasProperties->trigger();
}

void MainWindow::mouseMiddleButtonClicked(QGraphicsItem *item)
{
    zoomOn(item);
//    view->fitInView(item, Qt::KeepAspectRatio);
}

void MainWindow::panelAreasPropertiesVisibilityChanged(bool visible)
{
    actPanelAreasProperties->setChecked(visible);
    if (visible)
        ldtAreaNumber->setFocus();
}

void MainWindow::setAreaNumber()
{
    if ((map->floorsNumber() > 0) & (curFloor > -1))
        if (map->floor(curFloor)->selection()->item() != 0)
            if (map->floor(curFloor)->selection()->item()->type() == MapArea::Type)
            {
                MapArea *area = qgraphicsitem_cast<MapArea*>(
                        map->floor(curFloor)->selection()->item());
                area->setNumber(ldtAreaNumber->text());
#ifdef EDITOR
                if (!ptdtAreaInscription->document()->isModified())
                    updateAreaInscription();
#endif
            }
}

void MainWindow::setAreaName()
{
    if ((map->floorsNumber() > 0) & (curFloor > -1))
        if (map->floor(curFloor)->selection()->item() != 0)
            if (map->floor(curFloor)->selection()->item()->type() == MapArea::Type)
            {
                MapArea *area = qgraphicsitem_cast<MapArea*>(
                        map->floor(curFloor)->selection()->item());
                area->setName(ldtAreaName->text());
#ifdef EDITOR
                if (!ptdtAreaInscription->document()->isModified())
                    updateAreaInscription();
#endif
            }
}

void MainWindow::setAreaDescription()
{
    if ((map->floorsNumber() > 0) & (curFloor > -1))
        if (map->floor(curFloor)->selection()->item() != 0)
            if (map->floor(curFloor)->selection()->item()->type() == MapArea::Type)
            {
                MapArea *area = qgraphicsitem_cast<MapArea*>(
                        map->floor(curFloor)->selection()->item());
                area->setDescription(
                        ptdtAreaDescription->document()->toPlainText());
#ifdef EDITOR
                if (!ptdtAreaInscription->document()->isModified())
                    updateAreaInscription();
#endif
            }
}

void MainWindow::setAreaInscription()
{
    if ((map->floorsNumber() > 0) & (curFloor > -1))
        if (map->floor(curFloor)->selection()->item() != 0)
            if (map->floor(curFloor)->selection()->item()->type() == MapArea::Type)
            {
                MapArea *area = qgraphicsitem_cast<MapArea*>(
                        map->floor(curFloor)->selection()->item());
                area->setInscription(
                        ptdtAreaInscription->document()->toPlainText());
            }
}

//void MainWindow::chkAreaNumberVisibleStateChanged(int state)
//{
//    if (map->floor(curFloor)->selectedItem() != 0)
//        if (map->floor(curFloor)->selectedItem()->type() == MapArea::Type)
//        {

//        MapArea *area = qgraphicsitem_cast<MapArea*>(
//                map->floor(curFloor)->selectedItem());
//        area->setNumberVisible(state == Qt::Checked);
//    }
//}

//void MainWindow::chkAreaNameVisibleStateChanged(int state)
//{
//    if (map->floor(curFloor)->selectedItem() != 0)
//        if (map->floor(curFloor)->selectedItem()->type() == MapArea::Type)
//        {

//            MapArea *area = qgraphicsitem_cast<MapArea*>(
//                    map->floor(curFloor)->selectedItem());
//            area->setNameVisible(state == Qt::Checked);
//        }
//}

void MainWindow::panelSearchVisibilityChanged(bool visible)
{
    actPanelSearch->setChecked(visible);
}

void MainWindow::ldtInputTextEdited(const QString &text)
{
    lstwgtAreas->clear();
    if (!text.isEmpty())
    {
        QList<MapArea*> areas = map->findAreas(
                QRegExp(text, Qt::CaseInsensitive));
        lblAreasNumber->show();
        lblAreasNumber->setText(tr("Количество найденных областей: ") +
                                QString::number(areas.size()));
        for (int i = 0; i != areas.size(); i++)
        {
            MapArea *area = areas.at(i);

//            area->setName(area->name().simplified());

            MapFloor *floor = map->floorByUin(area->floorUin());

            QString str;

            str += area->number();

            if (!str.isEmpty() & !area->name().isEmpty())
                str += ", ";
            str += area->name();

            if (!str.isEmpty() & !floor->name().isEmpty())
                str += " (";
            str += floor->name() + tr(" этаж)");

            QListWidgetItem *item = new QListWidgetItem(str, lstwgtAreas);
            item->setData(Qt::UserRole, (uint)area);
        }
    }
    else
        lblAreasNumber->hide();
}

void MainWindow::lstwgtSearchCurrentItemChanged(QListWidgetItem *current,
                                                QListWidgetItem *previous)
{
    if (current)
    {
        MapArea *area = (MapArea*)current->data(Qt::UserRole).toUInt();
        if (area)
            zoomOn(area);
    }
}

void MainWindow::panelWaysVisibilityChanged(bool visible)
{
    actPanelWays->setChecked(visible);
}

void MainWindow::setStart()
{
    QGraphicsItem *item = map->floor(curFloor)->selection()->item();
    if ((mode == MapFloor::Selection) & (item != 0))
    {
        map->floor(curFloor)->selection()->clear();
        map->setStart(item);
        way();
    }
}

void MainWindow::setFinish()
{
    QGraphicsItem *item = map->floor(curFloor)->selection()->item();
    if ((mode == MapFloor::Selection) & (item != 0))
    {
        map->floor(curFloor)->selection()->clear();
        map->setFinish(item);
        way();
    }
}

void MainWindow::lstwgtWaysCurrentItemChanged(QListWidgetItem *current,
                                              QListWidgetItem *previous)
{
    if (current)
    {
        GraphArc::WayPermissions permissions =
                (GraphArc::WayPermissions)current->data(Qt::UserRole).toInt();
        map->way(permissions);
        map->paintWay();
        QGraphicsItem *item = map->start();
        if (item->type() != MapDoor::Type)
            zoomOn(item);
        Map::WayInfo *info = map->wayInfo();
        QString floorsInfo, stairsInfo, liftsInfo;
        floorsInfo  =
                tr("<br><center><b>Информация о пути</b></center><br><table>") +
                tr("<i>Этажи: </i>") +
                "<tr><td>" + tr("Количество:") + "</td>" +
                "<td>" + QString::number(info->floorsNumber, 'f', 0) + "</td></tr>" +

                "<tr><td>" + tr("Общее расстояние<br>(без лестниц и лифтов):") + "</td>" +
                "<td>" + QString::number(info->length, 'f', 0) + tr(" м") + "</td></tr>";
        if (info->stairsNumber != 0)
            stairsInfo =
                    "<tr><td>" + tr("<br><i>Лестницы: </i>") + "</td></tr>" +
                    "<tr><td>" + tr("Количество:")  + "</td>" +
                    "<td>" + QString::number(info->stairsNumber, 'f', 0) + "</td></tr>" +

                    "<tr><td>" + tr("Число пролётов вниз:") + "</td>" +
                    "<td>" + QString::number(info->stairsFloorsDownNumber, 'f', 0) + "</td></tr>" +

                    "<tr><td>" + tr("Число пролётов вверх:") + "</td>" +
                    "<td>" + QString::number(info->stairsFloorsUpNumber, 'f', 0) + "</td></tr>";
        else
            stairsInfo =
                    "<tr><td>" + tr("<br><i>Лестниц нет.</i>") + "</td></tr>";
        if (info->liftsNumber != 0)
            liftsInfo =
                    "<tr><td>" + tr("<br><i>Лифты: </i>") + "</td></tr>" +

                    "<tr><td>" + tr("Количество:") + "</td>" +
                    "<td>" + QString::number(info->liftsNumber, 'f', 0) + "</td></tr>" +

                    "<tr><td>" + tr("Число этажей вниз:") + "</td>" +
                    "<td>" + QString::number(info->liftsFloorsDownNumber, 'f', 0) + "</td></tr>" +

                    "<tr><td>" + tr("Число этажей вверх:") + "</td>" +
                    "<td>" + QString::number(info->liftsFloorsUpNumber, 'f', 0) + "</td></tr></table>";
        else
            liftsInfo =
                    "<tr><td>" + tr("<br><i>Лифтов нет.</i>") + "</td></tr></table>";

        lblWayInfo->setText(floorsInfo + stairsInfo + liftsInfo);
//                tr("Время: ") + QString::number(time, 'f', 0) + tr(" мин") + "<br>");
    }

//    }

}

void MainWindow::way()
{
    if (map->isStartAndFinishNodesValid())
    {
        GraphArc::WayPermissions permissions;
        Map::WayInfo *info;

        lblWaysChoice->setText(tr("<b>Выбор варианта пути</b>"));

        lstwgtWays->clear();

        permissions = GraphArc::None;
        map->way(permissions);
        info = map->wayInfo();
        QListWidgetItem *item = new QListWidgetItem(
                QString::number(lstwgtWays->count() + 1) +
                tr(". По этажу"), lstwgtWays);
        item->setData(Qt::UserRole, (int)permissions);

        permissions = GraphArc::Stairs;
        map->way(permissions);
        info = map->wayInfo();
        if (info->stairsNumber != 0)
        {
            QListWidgetItem *item = new QListWidgetItem(
                    QString::number(lstwgtWays->count() + 1) +
                    tr(". C лестницами"), lstwgtWays);
            item->setData(Qt::UserRole, (int)permissions);
        }

        permissions = GraphArc::Lift;
        map->way(permissions);
        info = map->wayInfo();
        if (info->liftsNumber != 0)
        {
            QListWidgetItem *item = new QListWidgetItem(
                    QString::number(lstwgtWays->count() + 1) +
                    tr(". С лифтами"), lstwgtWays);
            item->setData(Qt::UserRole, (int)permissions);
        }

        permissions = GraphArc::Stairs | GraphArc::Lift;
        map->way(permissions);
        info = map->wayInfo();
        if ((info->stairsNumber != 0) & (info->liftsNumber != 0))
        {
            QListWidgetItem *item = new QListWidgetItem(
                    QString::number(lstwgtWays->count() + 1) +
                    tr(". С лестницами и лифтами"), lstwgtWays);
            item->setData(Qt::UserRole, (int)permissions);
        }

        if (lstwgtWays->count())
        {
            lstwgtWays->setVisible(true);
            lstwgtWays->setCurrentItem(lstwgtWays->item(0));
        }
        else
            lblWayInfo->setText(
                    tr("Не удалось найти путь между заданными объектами."));
    }
}

void MainWindow::about()
{
    QString s;
#ifdef EDITOR
    s = tr("Программа предназначена для перевода пользователем "
           "поэтажного плана здания в собственный векторный "
           "формат, добавления информации об объектах, "
           "автоматизированного создания и редактирования графа здания.");
#else
    s = tr("Программа предназначена для отображения поэтажного плана "
           "здания, поиска нужных объектов и нахождения между ними "
           "кратчайшего пути.");
#endif
    QMessageBox::about(this, tr("О программе"), "<b>" + qApp->applicationName()
                       + " " + qApp->applicationVersion() + "</b>"
                       + "<br>" + s);
}
