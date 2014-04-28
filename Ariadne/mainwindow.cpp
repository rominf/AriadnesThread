#include "mainwindow.h"

const QString MainWindow::cNewFileName = QDir::currentPath() + tr("/New.bld");
QMap<int, GraphArc::VerticalType> MainWindow::verticalType;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    verticalType.insert(0, GraphArc::Auditorium);
    verticalType.insert(1, GraphArc::Stairs);
    verticalType.insert(2, GraphArc::Lift);

#ifdef EDITOR
    qApp->setApplicationName(tr("Ariadne"));
#else
    qApp->setApplicationName(tr("Theseus"));
#endif
    qApp->setApplicationVersion("2.0");
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
    createPanelAreasProperties();
    createPanelSearch();
    createPanelWays();

    setState(eFile | eHelp, stTrue, stTrue);
    setState(eEdit | eView | eGo | eAdd | eQuickSearch | ePanels | eFloors |
             eVerticals | eAreasProperties | eSearch | eWays, stFalse, stFalse);
#ifdef EDITOR
    actMapSave->setEnabled(false);
#endif
    actMapSaveAs->setEnabled(false);
    actMapExport->setEnabled(false);
    actMapProperties->setEnabled(false);
//    resize(792, 573);
//    resize(782, 498);
//    showMaximized();
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
    actMapNew = new QAction(QIcon(":/New"), tr("&Create..."), this);
    connect(actMapNew, SIGNAL(triggered()), SLOT(mapNew()));
    actMapNew->setShortcuts(shortcuts(QKeySequence::New, tr("N")));
#endif

    actMapOpen = new QAction(QIcon(":/Open"), tr("&Open..."), this);
    connect(actMapOpen, SIGNAL(triggered()), SLOT(mapOpen()));
    actMapOpen->setShortcuts(shortcuts(QKeySequence::Open, tr("O")));

#ifdef EDITOR
    actMapSave = new QAction(QIcon(":/Save"), tr("&Save"), this);
    connect(actMapSave, SIGNAL(triggered()), SLOT(mapSave()));
    actMapSave->setShortcuts(shortcuts(QKeySequence::Save, tr("S")));
#endif

    actMapSaveAs = new QAction(QIcon(":/SaveAs"), tr("Save &as..."), this);
    connect(actMapSaveAs, SIGNAL(triggered()), SLOT(mapSaveAs()));
    actMapSaveAs->setShortcuts(shortcuts(QKeySequence::SaveAs, tr("Shift+S")));

    actMapExport = new QAction(QIcon(":/Export"), tr("&Export..."), this);
    connect(actMapExport, SIGNAL(triggered()), SLOT(mapExport()));
    actMapExport->setShortcuts(shortcuts(tr("Ctrl+E"), tr("E")));

    actMapProperties = new QAction(QIcon(":/Properties"), tr("Properties"), this);
    connect(actMapProperties, SIGNAL(triggered()), SLOT(mapProperties()));

    actQuit = new QAction(QIcon(":/Quit"), tr("&Quit"), this);
    connect(actQuit, SIGNAL(triggered()), SLOT(exit()));
    actQuit->setShortcuts(shortcuts(QKeySequence::Quit, tr("Q")));


#ifdef EDITOR
    actAreaCopy = new QAction(QIcon(":/AreaCopy"),
                              tr("Copy &area..."), this);
    connect(actAreaCopy, SIGNAL(triggered()), SLOT(areaCopy()));
    actAreaCopy->setShortcuts(shortcuts(QKeySequence::Copy, tr("C")));

    actGraphCopy = new QAction(QIcon(":/GraphCopy"),
                              tr("Copy &graph..."), this);
    connect(actGraphCopy, SIGNAL(triggered()), SLOT(graphCopy()));
    actGraphCopy->setShortcuts(shortcuts(tr("Ctrl+Shift+C"), tr("Shift+C")));
#endif


#ifdef EDITOR
    actAddBase = new QAction(QIcon(":/AddBase"),
                             tr("Add &base..."), this);
    connect(actAddBase, SIGNAL(triggered()), SLOT(addBase()));
	actAddBase->setShortcuts(shortcuts(tr("Ctrl+B"), tr("B")));


    actLayerBase = new QAction(QIcon(":/LayerBase"), tr("&Base"), this);
    connect(actLayerBase, SIGNAL(triggered(bool)),
            SLOT(layerBaseSetVisible(bool)));
    actLayerBase->setShortcut(QKeySequence(tr("Ctrl+Shift+B")));
    actLayerBase->setCheckable(true);
    actLayerBase->setChecked(true);

    actLayerGrid = new QAction(QIcon(":/LayerGrid"), tr("&Grid"), this);
    connect(actLayerGrid, SIGNAL(triggered(bool)),
            SLOT(layerGridSetVisible(bool)));
    QList<QKeySequence> list;
    list << tr("Ctrl+#") << tr("Ctrl+№");
    if (isExtraShortcutsEnabled)
        list << tr("#") << tr("№");
    actLayerGrid->setShortcuts(list);
    actLayerGrid->setCheckable(true);

    actLayerGraph = new QAction(QIcon(":/LayerGraph"), tr("&Graph"), this);
    connect(actLayerGraph, SIGNAL(triggered(bool)),
            SLOT(layerGraphSetVisible(bool)));
    actLayerGraph->setShortcut(QKeySequence(tr("Ctrl+Shift+G")));
    actLayerGraph->setCheckable(true);
    actLayerGraph->setChecked(true);
#endif
    actZoomOut= new QAction(QIcon(":/ZoomOut"), tr("Zoom &out"), this);
    connect(actZoomOut, SIGNAL(triggered()), SLOT(zoomOut()));
    actZoomOut->setShortcuts(shortcuts(QKeySequence::ZoomOut, tr("-")));

    actZoomIn = new QAction(QIcon(":/ZoomIn"), tr("Zoom &in"), this);
    connect(actZoomIn, SIGNAL(triggered()), SLOT(zoomIn()));
    actZoomIn->setShortcuts(shortcuts(QKeySequence::ZoomIn, tr("+")));

    actZoomFit = new QAction(QIcon(":/ZoomFit"), tr("Zoom &fit"), this);
    connect(actZoomFit, SIGNAL(triggered()), SLOT(zoomFit()));
    actZoomFit->setShortcuts(shortcuts(tr("Ctrl+*"), tr("*")));

    actHandDrag = new QAction(QIcon(":/HandDrag"),
                              tr("&Hand drag"), this);
    connect(actHandDrag, SIGNAL(triggered(bool)), SLOT(handDrag(bool)));
    actHandDrag->setShortcuts(shortcuts(tr("Ctrl+H"), tr("H")));
    actHandDrag->setCheckable(true);
#ifdef EDITOR
    actSnapToExtensions = new QAction(
            QIcon(":/SnapToExtensions"),
            tr("&Snap to segment's extensions"), this);
    connect(actSnapToExtensions, SIGNAL(triggered(bool)),
            SLOT(snapToExtensions(bool)));
    actSnapToExtensions->setShortcut(tr("F5"));
    actSnapToExtensions->setCheckable(true);
#endif


    actFloorLower = new QAction(QIcon(":/FloorLower"), tr("&Lower floor"), this);
    connect(actFloorLower, SIGNAL(triggered()), SLOT(floorLower()));
    actFloorLower->setShortcut(Qt::Key_End);

    actFloorDown = new QAction(QIcon(":/Down"), tr("Floor &below"), this);
    connect(actFloorDown, SIGNAL(triggered()), SLOT(floorDown()));
    actFloorDown->setShortcut(Qt::Key_PageDown);

    actFloorUp = new QAction(QIcon(":/Up"), tr("Floor &above"), this);
    connect(actFloorUp, SIGNAL(triggered()), SLOT(floorUp()));
    actFloorUp->setShortcut(Qt::Key_PageUp);

    actFloorUpper = new QAction(QIcon(":/FloorUpper"), tr("&Upper floor"), this);
    connect(actFloorUpper, SIGNAL(triggered()), SLOT(floorUpper()));
    actFloorUpper->setShortcut(Qt::Key_Home);


#ifdef EDITOR
    actAddArea = new QAction(QIcon(":/AddArea"),
                             tr("Add &area"),this);
    connect(actAddArea, SIGNAL(triggered()), SLOT(addArea()));
    actAddArea->setShortcuts(shortcuts(tr("Ctrl+A"), tr("A")));

    actAddDoor = new QAction(QIcon(":/AddDoor"), tr("Add &door"), this);
    connect(actAddDoor, SIGNAL(triggered()), SLOT(addDoor()));
    actAddDoor->setShortcuts(shortcuts(tr("Ctrl+D"), tr("D")));

    actAddNode = new QAction(QIcon(":/AddNode"), tr("Add &graph"),
                             this);
    connect(actAddNode, SIGNAL(triggered()), SLOT(addNode()));
    actAddNode->setShortcuts(shortcuts(tr("Ctrl+G"), tr("G")));
#endif


    actHelp = new QAction(QIcon(":/Help"), tr("&Help"), this);
    connect(actHelp, SIGNAL(triggered()), SLOT(help()));
    actHelp->setShortcut(QKeySequence::HelpContents);

    actAbout = new QAction(QIcon(":/Ariadne.svg"), tr("&About"), this);
    connect(actAbout, SIGNAL(triggered()), SLOT(about()));

#ifdef EDITOR
    actPanelFloors = new QAction(QIcon(":/Floors"),
                                         tr("&Floors"), this);
    actPanelFloors->setData(eFloors);
    actPanelFloors->setShortcuts(shortcuts(tr("Ctrl+1"), tr("1")));
    actPanelFloors->setCheckable(true);

    actFloorAdd = new QAction(QIcon(":/Add"), tr("&Add floor"), this);
    connect(actFloorAdd, SIGNAL(triggered()), SLOT(floorAdd()));
    actFloorAdd->setShortcuts(shortcuts(tr("Ctrl+F"), tr("F")));

    actFloorDelete = new QAction(QIcon(":/Delete"), tr("&Delete floor"), this);
    connect(actFloorDelete, SIGNAL(triggered()), SLOT(floorDelete()));
    actFloorDelete->setShortcuts(shortcuts(tr("Ctrl+Alt+F"), tr("Alt+F")));

    actFloorMoveDown = new QAction(QIcon(":/Down"),
                                   tr("Move &down"), this);
    connect(actFloorMoveDown, SIGNAL(triggered()), SLOT(floorMoveDown()));

    actFloorMoveUp = new QAction(QIcon(":/Up"),
                                 tr("Move &up"), this);
    connect(actFloorMoveUp, SIGNAL(triggered()), SLOT(floorMoveUp()));

    actFloorSetDefault = new QAction(QIcon(":/FloorDefault"),
                                     tr("Set as default floor"), this);
    connect(actFloorSetDefault, SIGNAL(triggered()), SLOT(floorSetDefault()));


    actPanelVerticals = new QAction(QIcon(":/Verticals"),
                                    tr("&Verticals"), this);
    actPanelVerticals->setData(eVerticals);
    actPanelVerticals->setShortcuts(shortcuts(tr("Ctrl+2"), tr("2")));
    actPanelVerticals->setCheckable(true);

    actVerticalAdd = new QAction(QIcon(":/Add"),
                                 tr("&Add vertical"), this);
    connect(actVerticalAdd, SIGNAL(triggered()), SLOT(verticalAdd()));
    actVerticalAdd->setShortcuts(shortcuts(tr("Ctrl+V"), tr("V")));

    actVerticalDelete = new QAction(QIcon(":/Delete"),
                                    tr("&Delete vertical"), this);
    connect(actVerticalDelete, SIGNAL(triggered()), SLOT(verticalDelete()));
    actVerticalDelete->setShortcuts(shortcuts(tr("Ctrl+Alt+V"), tr("Alt+V")));

    actVerticalMoveDown = new QAction(QIcon(":/Down"),
                                      tr("Move &down"), this);
    connect(actVerticalMoveDown, SIGNAL(triggered()), SLOT(verticalMoveDown()));

    actVerticalMoveUp = new QAction(QIcon(":/Up"),
                                    tr("Move &up"), this);
    connect(actVerticalMoveUp, SIGNAL(triggered()), SLOT(verticalMoveUp()));
#endif


    actPanelAreasProperties = new QAction(
            QIcon(":/AreasProperties"), tr("Area &properties"), this);
    actPanelAreasProperties->setData(eAreasProperties);
#ifdef EDITOR
    actPanelAreasProperties->setShortcuts(shortcuts(tr("Ctrl+3"), tr("3")));
#else
    actPanelAreasProperties->setShortcuts(shortcuts(tr("Ctrl+1"), tr("1")));
#endif
    actPanelAreasProperties->setCheckable(true);


    actPanelSearch = new QAction(QIcon(":/Search"), tr("Search"), this);
    actPanelSearch->setData(eSearch);
#ifdef EDITOR
    actPanelSearch->setShortcuts(shortcuts(tr("Ctrl+4"), tr("4")));
#else
    actPanelSearch->setShortcuts(shortcuts(tr("Ctrl+2"), tr("2")));
#endif
    actPanelSearch->setCheckable(true);


    actPanelWays = new QAction(QIcon(":/Ways"), tr("&Ways"), this);
    actPanelWays->setData(eWays);
#ifdef EDITOR
    actPanelWays->setShortcuts(shortcuts(tr("Ctrl+5"), tr("5")));
#else
    actPanelWays->setShortcuts(shortcuts(tr("Ctrl+3"), tr("3")));
#endif
    actPanelWays->setCheckable(true);

    actSetStart = new QAction(QIcon(":/SetStart"), tr("Start"), this);
    connect(actSetStart, SIGNAL(triggered()), SLOT(setStart()));

    actSetFinish = new QAction(QIcon(":/SetFinish"), tr("Finish"), this);
    connect(actSetFinish, SIGNAL(triggered()), SLOT(setFinish()));


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
    menuFile = menuBar()->addMenu(tr("&File"));
#ifdef EDITOR
    menuFile->addAction(actMapNew);
#endif
    menuFile->addAction(actMapOpen);
#ifdef EDITOR
    menuFile->addAction(actMapSave);
#endif
    menuFile->addAction(actMapSaveAs);
    menuFile->addAction(actMapExport);
    menuFile->addAction(actMapProperties);
    menuFile->addAction(actQuit);

#ifdef EDITOR
    menuEdit = menuBar()->addMenu(tr("&Edit"));
    menuEdit->addAction(actAreaCopy);
    menuEdit->addAction(actGraphCopy);
#endif

    menuView = menuBar()->addMenu(tr("&View"));
#ifdef EDITOR
    menuView->addAction(actAddBase);
    menuLayers = menuView->addMenu(tr("&Layers"));
    menuLayers->addAction(actLayerBase);
    menuLayers->addAction(actLayerGrid);
    menuLayers->addAction(actLayerGraph);
    menuView->addSeparator();
#endif
    menuZoom = menuView->addMenu(QIcon(":/Zoom"), tr("&Zoom"));
    menuZoom->addAction(actZoomOut);
    menuZoom->addAction(actZoomIn);
    menuZoom->addAction(actZoomFit);
    menuView->addAction(actHandDrag);
#ifdef EDITOR
    menuView->addSeparator();
    menuView->addAction(actSnapToExtensions);
#endif

    menuGo = menuBar()->addMenu(tr("&Go"));
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
    menuAdd = menuBar()->addMenu(tr("&Add"));
    menuAdd->addAction(actAddArea);
    menuAdd->addAction(actAddDoor);
    menuAdd->addAction(actAddNode);
#endif

    menuHelp = menuBar()->addMenu(tr("&Help"));
    menuHelp->addAction(actHelp);
    menuHelp->addAction(actAbout);
    //menuHelp->addAction(actAboutQT);
}

void MainWindow::createToolBars()
{
    tbrFile = addToolBar(tr("File"));
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

    tbrView = addToolBar(tr("View"));
    tbrView->setFloatable(false);
    tbrView->addAction(actZoomOut);
    tbrView->addAction(actZoomIn);
    tbrView->addAction(actZoomFit);
    tbrView->addAction(actHandDrag);

    tbrGo = addToolBar(tr("Go"));
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
#ifdef EDITOR
    tbrAdd = addToolBar(tr("Add"));
    tbrAdd->setFloatable(false);
//    tbrAdd->addAction(actAddWall);
    tbrAdd->addAction(actAddArea);
    tbrAdd->addAction(actAddDoor);
    tbrAdd->addAction(actAddNode);
#endif

    tbrQuickSearch = addToolBar(tr("Instant search"));
    tbrQuickSearch->setFloatable(false);
    ldtQuichSearch = new QLineEdit();
    connect(ldtQuichSearch, SIGNAL(textChanged(QString)),
            SLOT(ldtQuickSearchTextChanged(QString)));
    ldtQuichSearch->setPlaceholderText(
            tr("Start typing number, name or description of area"));
    tbrQuickSearch->addWidget(ldtQuichSearch);
    tbrQuickSearch->addAction(actSetStart);
    tbrQuickSearch->addAction(actSetFinish);

    tbrPanels = new QToolBar(tr("Panels"));
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
    dckwgtFloors = new QDockWidget(tr("Floors"));
	dckwgtFloors->resize(cDockWidth, dckwgtFloors->height());
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
    connect(modelFloorsList, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            SLOT(viewFloorsListItemChanged(QModelIndex)));
    viewFloorsList->setModel(modelFloorsList);
    viewFloorsList->setSelectionModel(selectionFloorsList);
    vblFloors->addWidget(viewFloorsList);
}

void MainWindow::createPanelVerticals()
{
    dckwgtVerticals = new QDockWidget(tr("Verticals"));
	dckwgtVerticals->resize(cDockWidth, dckwgtVerticals->height());
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
    cbxVerticalType->insertItem(0, tr("Class room"), (int)GraphArc::Auditorium);
    cbxVerticalType->insertItem(1, tr("Stairs"), (int)GraphArc::Stairs);
    cbxVerticalType->insertItem(2, tr("Lift"), (int)GraphArc::Lift);
    connect(cbxVerticalType, SIGNAL(currentIndexChanged(int)),
            SLOT(verticalSetTypeCurrentIndexChanged(int)));
    hblVerticalsButtons->addWidget(cbxVerticalType);

    hblVerticalsButtons->addStretch();
    vblVerticals->addLayout(hblVerticalsButtons);

    lstwgtVerticals = new QListWidget();
    connect(lstwgtVerticals,
            SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            SLOT(lstwgtVerticalsCurrentItemChanged(QListWidgetItem*,
                                                   QListWidgetItem*)));
    connect(lstwgtVerticals, SIGNAL(itemChanged(QListWidgetItem*)),
            SLOT(lstwgtVerticalsItemChanged(QListWidgetItem*)));
    vblVerticals->addWidget(lstwgtVerticals);
}

void MainWindow::createPanelAreasProperties()
{
    dckwgtAreasProperties = new QDockWidget(tr("Area properties"));
	dckwgtAreasProperties->resize(cDockWidth, dckwgtAreasProperties->height());
    connect(dckwgtAreasProperties, SIGNAL(visibilityChanged(bool)),
            SLOT(panelAreasPropertiesVisibilityChanged(bool)));
    addDockWidget(Qt::LeftDockWidgetArea, dckwgtAreasProperties);
    dckwgtAreasProperties->setFeatures(
            QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

    wgtAreasProperties = new QWidget;
    dckwgtAreasProperties->setWidget(wgtAreasProperties);

    vblAreasProperties = new QVBoxLayout(wgtAreasProperties);

    lblAreaType = new QLabel(tr("Type: "));
#ifndef EDITOR
    lblAreaType->setVisible(false);
#endif
    vblAreasProperties->addWidget(lblAreaType);

    cbxAreaType = new QComboBox();
    cbxAreaType->insertItem(0, tr("Room"), (int)MapArea::Room);
    cbxAreaType->insertItem(1, tr("Aperture"), (int)MapArea::Aperture);
    cbxAreaType->insertItem(2, tr("Column"), (int)MapArea::Column);
    connect(cbxAreaType, SIGNAL(currentIndexChanged(int)),
            SLOT(cbxAreaTypeCurrentIndexChanged(int)));
#ifndef EDITOR
    cbxAreaType->setEnabled(false);
    cbxAreaType->setVisible(false);
#endif
    vblAreasProperties->addWidget(cbxAreaType);

    lblAreaNumber = new QLabel(tr("Number: "));
    vblAreasProperties->addWidget(lblAreaNumber);

    ldtAreaNumber = new QLineEdit();
    connect(ldtAreaNumber, SIGNAL(textChanged(QString)), SLOT(setAreaNumber()));
#ifndef EDITOR
    ldtAreaNumber->setReadOnly(true);
#endif
    vblAreasProperties->addWidget(ldtAreaNumber);

    lblAreaName = new QLabel(tr("Name: "));
    vblAreasProperties->addWidget(lblAreaName);

    ldtAreaName = new QLineEdit();
    connect(ldtAreaName, SIGNAL(textChanged(QString)), SLOT(setAreaName()));
#ifndef EDITOR
    ldtAreaName->setReadOnly(true);
#endif
    vblAreasProperties->addWidget(ldtAreaName);

    lblAreaDescription = new QLabel(tr("Description: "));
    vblAreasProperties->addWidget(lblAreaDescription);

    ptdtAreaDescription = new QPlainTextEdit("");
    connect(ptdtAreaDescription, SIGNAL(textChanged()),
            SLOT(setAreaDescription()));
#ifndef EDITOR
    ptdtAreaDescription->setReadOnly(true);
#endif
    vblAreasProperties->addWidget(ptdtAreaDescription);

#ifdef EDITOR
    btnUpdateAreaInscription = new QPushButton(tr("Update inscription"));
    connect(btnUpdateAreaInscription, SIGNAL(clicked()),
            SLOT(updateAreaInscription()));
    vblAreasProperties->addWidget(btnUpdateAreaInscription);

    lblAreaInscription = new QLabel(tr("Inscription: "));
    vblAreasProperties->addWidget(lblAreaInscription);

    ptdtAreaInscription = new QPlainTextEdit("");
    connect(ptdtAreaInscription, SIGNAL(textChanged()),
            SLOT(setAreaInscription()));
    vblAreasProperties->addWidget(ptdtAreaInscription);
#endif
}

void MainWindow::createPanelSearch()
{
    dckwgtSearch = new QDockWidget(tr("Search"));
	dckwgtSearch->resize(cDockWidth, dckwgtSearch->height());
    connect(dckwgtSearch, SIGNAL(visibilityChanged(bool)),
            SLOT(panelSearchVisibilityChanged(bool)));
    addDockWidget(Qt::LeftDockWidgetArea, dckwgtSearch);
    dckwgtSearch->setFeatures(
            QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

    wgtSearch = new QWidget;
    dckwgtSearch->setWidget(wgtSearch);

    vblSearch = new QVBoxLayout(wgtSearch);

    ldtSearch = new QLineEdit();
    connect(ldtSearch, SIGNAL(textChanged(QString)),
            SLOT(ldtSearchTextChanged(QString)));
    ldtSearch->setPlaceholderText(
            tr("Start typing number, name or description of area"));
    vblSearch->addWidget(ldtSearch);

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
    dckwgtWay = new QDockWidget(tr("Ways"));
	dckwgtWay->resize(cDockWidth, dckwgtWay->height());
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

    vblWay->addLayout(hblWayButtons);

    grpbxWay = new QGroupBox();
    vblWay->addWidget(grpbxWay);

    vblgrpbxWay = new QVBoxLayout(grpbxWay);

    lblWaysChoice = new QLabel();
    lblWaysChoice->setWordWrap(true);
    vblgrpbxWay->addWidget(lblWaysChoice);

    lstwgtWays = new QListWidget();
    connect(lstwgtWays,
	    SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
	    SLOT(lstwgtWaysCurrentItemChanged(
		     QListWidgetItem*,QListWidgetItem*)));
    vblgrpbxWay->addWidget(lstwgtWays);

    lblWayInfo = new QLabel();
    lblWayInfo->setWordWrap(true);
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
}

void MainWindow::freePanelWays()
{
    lblWaysChoice->setText(tr("To find a way, select initial object and press "
	                          "\"Start\". Similarly mark the finish."));
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
        view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform |
                             QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing);
        view->setMouseTracking(true);
        view->setBackgroundBrush(QBrush(Qt::gray));
        vblwgtCentral->addWidget(view);
        view->show();
    }
    else
    {
        modelFloorsList->setStringList(QStringList());
        curFloor = -1;
#ifdef EDITOR
        lstwgtVerticals->clear();
#endif
        lstwgtAreas->clear();
    }
#ifndef EDITOR
    layerBaseSetVisible(false);
    layerGraphSetVisible(false);
#endif
    freePanelWays();
    switchMode(MapFloor::Idle);
}

void MainWindow::setState(Elements elem, State visible, State enable)
{
    if (visible != stSave)
    {
        if (elem & eFile)
        {
#ifdef EDITOR
            actMapNew->setVisible(visible);
            actMapSave->setVisible(visible);
#endif
            actMapOpen->setVisible(visible);
            actMapSaveAs->setVisible(visible);
            actMapExport->setVisible(visible);
            actMapProperties->setVisible(visible);
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
            actSnapToExtensions->setVisible(visible);
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
        if (elem & eQuickSearch)
        {
            tbrQuickSearch->setVisible(visible);
        }
        if (elem & eHelp)
        {
            actHelp->setVisible(visible);
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
            actMapExport->setEnabled(enable);
            actMapProperties->setEnabled(enable);
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
            actSnapToExtensions->setEnabled(enable);
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
        if ((elem & eQuickSearch) > 0)
        {
            tbrQuickSearch->setEnabled(enable);
        }
        if (elem & eHelp)
        {
            actHelp->setEnabled(enable);
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
            wgtFloors->setEnabled(enable);
        if (elem & eVerticals)
            wgtVerticals->setEnabled(enable);
#endif
        if (elem & eAreasProperties)
        {
            if (!enable)
                dckwgtAreasProperties->setVisible(false);
            wgtAreasProperties->setEnabled(enable);
        }
        if (elem & eSearch)
            wgtWay->setEnabled(enable);
        if (elem & eWays)
            wgtWay->setEnabled(enable);
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
            openFileName = fileName;
        }
        // Copy
        QString copyFile = QDateTime::currentDateTime().toString(
                "yy.MM.dd.hh.mm.ss") + ".bld";
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

MapArea* MainWindow::selectedArea()
{
    if ((map->floorsNumber() > 0) & (curFloor > -1))
    {
        MapSelection *selection = map->floor(curFloor)->selection();
        if (selection)
        {
            QGraphicsItem *item = selection->item();
            if (item)
                if (item->type() == MapArea::Type)
                {
                    MapArea *area = qgraphicsitem_cast<MapArea*>(item);
                    if (area)
                        return area;
                }
        }
    }
    return 0;
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
                QMessageBox::Information, tr("Building saving"),
                tr("Building \"") + openFile.fileName() +
                tr("\" has been changed.\nSave changes?"),
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
    DialogMapSize* dialog = new DialogMapSize(this);
    if (dialog->exec() == QDialog::Accepted)
    {
        openFileName = cNewFileName;
        if (map)
            delete map;
        map = new Map(cPixPerRealM, displayPixPerM(width(), widthMM()),
                      dialog->MSizeX()*cPixPerRealM,
                      dialog->MSizeY()*cPixPerRealM, this);
        createGraphics();
        floorAdd();
        floorSetDefault();
        view->fitInView(view->sceneRect(), Qt::KeepAspectRatio);
    }
}

void MainWindow::mapOpenShowError()
{
	QMessageBox::warning(
	            this, tr("Error on file reading"),
	            tr("The selected file is damaged or is not a building file."));
}

void MainWindow::mapOpen()
{
    // ### Ask about saving on opening
    QString str = QFileDialog::getOpenFileName(
            this, tr("Building opening"),
            openFileName, tr("Building file(*.bld)"));
	if (str.isNull())
	{
		mapOpenShowError();
		return;
	}
	QFile f(str);
	if (!f.open(QIODevice::ReadOnly))
	{
		mapOpenShowError();
		return;
	}

	QDataStream s(&f);
	s.setVersion(QDataStream::Qt_4_7);
	qint32 magicNumber;
	s >> magicNumber;
	if (magicNumber != cMagicNumber)
	{
		mapOpenShowError();
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
	actFloorDelete->setEnabled(map->floorsNumber() > 0);
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
            this, tr("Building saving"), openFileName,
            tr("Building file(*.bld)"));
    mapSave(fileName);
}

void MainWindow::mapExport()
{
    MapFloor *floor = map->floor(curFloor);
    QString fileName = QFileDialog::getSaveFileName(
            this, tr("Building export"),
            openFileName.replace(".bld", " - " + floor->name() + ".png"),
            tr("Images(*.png *.bmp *.jpg *.jpeg)"));
    QRectF r = floor->sceneRect();
    QImage img(r.width(), r.height(), QImage::Format_ARGB32_Premultiplied);
    QPainter p(&img);
    floor->render(&p, QRectF(), QRectF(1, 1, r.width() - 1, r.height() - 1));
    p.end();
    img.save(fileName);
}

void MainWindow::mapProperties()
{
    DialogMapProperties* dialog = new DialogMapProperties(
            this, map->properties());
    if (dialog->exec() == QDialog::Accepted)
        map->setProperties(dialog->properties());
}

void MainWindow::exit()
{
    close();
}

void MainWindow::areaCopy()
{
    MapArea *oldArea = selectedArea();
    if (oldArea)
    {
        int i;
        if (curFloor > 0)
            i = curFloor - 1;
        else
            if (curFloor < map->floorsNumber() - 1)
                i = curFloor + 1;
            else
            {
                QMessageBox::warning(
                        0, tr("Error while area copying"),
                        tr("There is only one floor in the building. "
				           "Copying area to the same floor is impossible."));
                return;
            }
        DialogFloorChoice* dialog = new DialogFloorChoice(
                this, modelFloorsList, i, tr("Choose a floor, on which the "
		                                     "selected area will be copied:"));
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
            QMessageBox::warning(0, tr("Error while graph copying"),
			                     tr("There is only one floor in the building. "
			                        "Copying graph to the same floor is "
			                        "impossible."));
            return;
        }
    DialogFloorChoice* dialog = new DialogFloorChoice(
	            this, modelFloorsList, i, tr("Choose a floor, on which part of "
	                                         "the graph on this floor will be "
	                                         "copied:"));
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
            QMessageBox::warning(0, tr("Error while graph copying"),
                                 tr("Copying graph to the same floor is "
			                        "impossible. Choose another floor."));
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
    updateStatusBar();
}

void MainWindow::zoomIn()
{
    view->scale(cZoom, cZoom);
    updateStatusBar();
}

void MainWindow::zoomFit()
{
    view->fitInView(view->sceneRect(), Qt::KeepAspectRatio);
    updateStatusBar();
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
    updateStatusBar();
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
            this, tr("Choose base"), "",
            tr("Images(*.png *.bmp *.jpg *.jpeg)"));
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

void MainWindow::snapToExtensions(bool b)
{
    map->floor(curFloor)->setSnapToExtensions(b);
}

void MainWindow::floorAdd()
{
    map->insertFloor(0);

    modelFloorsList->blockSignals(true);
    modelFloorsList->insertRow(0);
    modelFloorsList->blockSignals(false);
    modelFloorsList->setData(modelFloorsList->index(0), tr("New floor"));
//    QModelIndex index = modelFloorsList->index(map->floorsNumber()-1);
//    modelFloorsList->setData(index, floorName);
//    cbxFloorSelect->addItem(floorName);
//    cbxFloorSelect->setCurrentIndex(curFloor);
    setActiveFloor(0);
    actFloorDelete->setEnabled(map->floorsNumber() > 1);

//    QString floorName = QString::number(map->floorsNumber());
//    map->floor(curFloor)->area()->setName(floorName);

}

void MainWindow::floorDelete()
{
    map->removeFloor(curFloor);
//    modelFloorsList->blockSignals(true);
    modelFloorsList->removeRow(curFloor);
    actFloorDelete->setEnabled(map->floorsNumber() > 1);
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
    actFloorSetDefault->setChecked(true);
    actFloorSetDefault->setEnabled(false);
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

void MainWindow::ldtQuickSearchTextChanged(const QString &text)
{
    if (ldtSearch->text() != text)
        ldtSearch->setText(text);
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
        setState(eFile | eEdit | eView | eGo | eAdd | eQuickSearch | eHelp |
                 ePanels, stTrue, stTrue);
        setState(eFloors | eVerticals | eSearch | eWays, stSave, stTrue);
#else
        setState(eFile | eView | eGo | eQuickSearch | eHelp | ePanels,
                 stTrue, stTrue);
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
        setState(eEdit | eGo | eAdd | eQuickSearch | ePanels | eFloors |
                 eVerticals | eAreasProperties | eSearch  | eWays,
                 stSave, stFalse);
        setCursor(Qt::CrossCursor);
        if (m == MapFloor::NodeAdd)
            map->graphStartAnew();
        break;
    case MapFloor::Selection:
        {
            bool selectionIsValid = false;
            MapArea *area = selectedArea();
            if (area)
            {
                selectionIsValid = true;
                setState(eAreasProperties/* | eWays*/, stSave, stTrue);
                bool isModified = false;
                isModified = (!area->number().isEmpty() |
                              !area->name().isEmpty() |
                              !area->description().isEmpty() |
                              !area->inscription().isEmpty());
#ifdef EDITOR
                ptdtAreaInscription->document()->setModified(isModified);
#endif
                cbxAreaType->setCurrentIndex((int)area->areaType());
                ldtAreaNumber->setText(area->number());
                ldtAreaName->setText(area->name());
                ptdtAreaDescription->setPlainText(area->description());
#ifdef EDITOR
                ptdtAreaInscription->setPlainText(area->inscription());
                ptdtAreaInscription->document()->setModified(isModified);
#endif
                ldtAreaNumber->selectAll();
                if (dckwgtAreasProperties->isVisible())
                    ldtAreaNumber->setFocus();
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
    {
        if (map->floor(curFloor)->mode() != m)
            map->floor(curFloor)->setMode(m);
        updateStatusBar();
    }
}

void MainWindow::setActiveFloor(int i)
{
    if ((i >= 0)/* & (i != curFloor) */& (i < map->floorsNumber()))
    {
        for (int j = 0; j != map->floorsNumber(); j++)
        {
            MapFloor *oldFloor = map->floor(j);
            oldFloor->disconnect(SIGNAL(modeChanged(MapFloor::Mode)));
            oldFloor->disconnect(SIGNAL(mouseDoubleClicked()));
            oldFloor->disconnect(
                    SIGNAL(mouseMiddleButtonClicked(QGraphicsItem*)));
            oldFloor->disconnect(SIGNAL(mouseMoved()));
        }
//            map->floor(j)->disconnect();
//        if ((curFloor >= 0) & (curFloor != i) &
//            (curFloor < map->floorsNumber()))
//        {
//            MapFloor *oldFloor = map->floor(curFloor);
//            oldFloor->disconnect(SIGNAL(modeChanged(MapFloor::Mode)));
//            oldFloor->disconnect(SIGNAL(mouseDoubleClicked()));
//            oldFloor->disconnect(
//                    SIGNAL(mouseMiddleButtonClicked(QGraphicsItem*)));
//            oldFloor->disconnect(SIGNAL(mouseMoved()));
//        }
        curFloor = i;
        MapFloor *floor = map->floor(i);
//        selectionFloorsList->setCurrentIndex(
//                index, QItemSelectionModel::ClearAndSelect);
        QModelIndex index = modelFloorsList->index(i);
//        if (selectionFloorsList->currentIndex().row() != i)
        selectionFloorsList->setCurrentIndex(
                index, QItemSelectionModel::Clear |
                QItemSelectionModel::SelectCurrent);
        cbxFloorSelect->blockSignals(true);
        cbxFloorSelect->setCurrentIndex(i);
        cbxFloorSelect->blockSignals(false);
        view->setScene(floor);
        connect(floor, SIGNAL(modeChanged(MapFloor::Mode)),
                SLOT(switchMode(MapFloor::Mode)));
        connect(floor, SIGNAL(mouseDoubleClicked()),
                SLOT(mouseDoubleClicked()));
        connect(floor, SIGNAL(mouseMiddleButtonClicked(QGraphicsItem*)),
                SLOT(zoomOn(QGraphicsItem*)));
        connect(floor, SIGNAL(mouseMoved()), SLOT(updateStatusBar()));
        if ((focusWidget() != ldtSearch) & (focusWidget() != ldtQuichSearch))
            setFocus();
        bool switchToSelection = false;
        if (floor->selection()->item() != 0)
            if ((mode == MapFloor::Idle) | (mode == MapFloor::Selection))
            {
                switchMode(MapFloor::Selection);
                switchToSelection = true;
            }
        if (!switchToSelection & (floor->mode() != mode))
            switchMode(mode);
#ifdef EDITOR
        actFloorDelete->setEnabled(map->floorsNumber() > 1);
#endif
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
        bool isDefaultFloor = floor->uin() == defaultFloor;
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
	Q_UNUSED(previous)
    int floor = current.row();
    if (curFloor != floor)
        setActiveFloor(floor);
}

void MainWindow::viewFloorsListItemChanged(QModelIndex index)
{
    map->floor(index.row())->setName(modelFloorsList->data(
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
    int i = lstwgtVerticals->count();
    map->addVertical();
    QListWidgetItem *item = new QListWidgetItem(tr("New vertical"));
    item->setFlags(item->flags () | Qt::ItemIsEditable);
    map->vertical(i)->setName(item->text());
    lstwgtVerticals->insertItem(i, item);
    lstwgtVerticals->setCurrentItem(item);
    cbxVerticalType->setCurrentIndex(-1);
    cbxVerticalType->setCurrentIndex(0);
}

void MainWindow::verticalDelete()
{
    int i = lstwgtVerticals->currentRow();
    lstwgtVerticals->takeItem(i);
    map->deleteVertical(i);
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
        map->vertical(i)->setType(verticalType.value(index));
}

void MainWindow::lstwgtVerticalsCurrentItemChanged(QListWidgetItem *current,
                                                   QListWidgetItem *previous)
{
	Q_UNUSED(previous)
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
        cbxVerticalType->setCurrentIndex(
                verticalType.key(map->vertical(i)->type()));
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

void MainWindow::mouseDoubleClicked()
{
    actPanelAreasProperties->trigger();
}

void MainWindow::panelAreasPropertiesVisibilityChanged(bool visible)
{
    actPanelAreasProperties->setChecked(visible);
    if (visible)
        ldtAreaNumber->setFocus();
}

void MainWindow::cbxAreaTypeCurrentIndexChanged(int index)
{
    MapArea *area = selectedArea();
    MapArea::AreaType type = MapArea::AreaType(index);
    if (area)
        if (type != area->areaType())
            area->setAreaType(type, false);
}

void MainWindow::setAreaNumber()
{
    MapArea *area = selectedArea();
    if (area)
    {
        area->setNumber(ldtAreaNumber->text());
#ifdef EDITOR
        if (!ptdtAreaInscription->document()->isModified())
            updateAreaInscription();
#endif
    }
}

void MainWindow::setAreaName()
{
    MapArea *area = selectedArea();
    if (area)
    {
        area->setName(ldtAreaName->text());
#ifdef EDITOR
        if (!ptdtAreaInscription->document()->isModified())
            updateAreaInscription();
#endif
    }
}

void MainWindow::setAreaDescription()
{
    MapArea *area = selectedArea();
    if (area)
    {
        area->setDescription(ptdtAreaDescription->document()->toPlainText());
#ifdef EDITOR
        if (!ptdtAreaInscription->document()->isModified())
            updateAreaInscription();
#endif
    }
}

void MainWindow::setAreaInscription()
{
    MapArea *area = selectedArea();
    if (area)
        area->setInscription(ptdtAreaInscription->document()->toPlainText());
}

void MainWindow::panelSearchVisibilityChanged(bool visible)
{
    actPanelSearch->setChecked(visible);
}

void MainWindow::ldtSearchTextChanged(const QString &text)
{
    if (ldtQuichSearch->text() != text)
        ldtQuichSearch->setText(text);
    lstwgtAreas->clear();
    if (!text.isEmpty())
    {
        QList<MapArea*> areas = map->findAreas(
                QRegExp(text, Qt::CaseInsensitive));
        lblAreasNumber->show();
        lblAreasNumber->setText(tr("Number of found areas: ") +
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

            if (str.isEmpty())
                str = tr("Untitled");

            if (!str.isEmpty() & !floor->name().isEmpty())
                str += " (";
            str += floor->name() + tr(" floor)");

			QListWidgetItem *item = new QListWidgetItem(str, lstwgtAreas);
			item->setData(Qt::UserRole, (qulonglong)area);
        }
        lstwgtAreas->setCurrentRow(0);
    }
    else
        lblAreasNumber->hide();
}

void MainWindow::lstwgtSearchCurrentItemChanged(QListWidgetItem *current,
                                                QListWidgetItem *previous)
{
	Q_UNUSED(previous)
    if (current)
    {
		MapArea *area = (MapArea*)current->data(Qt::UserRole).toULongLong();
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
	Q_UNUSED(previous)
    if (current)
    {
        GraphArc::WayPermissions permissions =
                (GraphArc::WayPermissions)current->data(Qt::UserRole).toInt();
        map->way(permissions);
        map->paintWay();
		QGraphicsItem *item = map->start();
		if ((item->type() != MapDoor::Type) & (item->type() != GraphNode::Type))
			zoomOn(item);
        Map::WayInfo *info = map->wayInfo();
        QString floorsInfo, stairsInfo, liftsInfo;
        floorsInfo  =
                tr("<br><center><b>Way information</b></center><br><table>") +
                tr("<i>Floors: </i>") +
                "<tr><td>" + tr("Count:") + "</td>" +
                "<td>" + QString::number(info->floorsNumber, 'f', 0) + "</td></tr>" +

                "<tr><td>" + tr("Overall distance<br>(not including stairs and lifts):") + "</td>" +
                "<td>" + QString::number(info->length, 'f', 0) + tr(" m") + "</td></tr>";
        if (info->stairsNumber != 0)
            stairsInfo =
                    "<tr><td>" + tr("<br><i>Stairs: </i>") + "</td></tr>" +
                    "<tr><td>" + tr("Count:")  + "</td>" +
                    "<td>" + QString::number(info->stairsNumber, 'f', 0) + "</td></tr>" +

                    "<tr><td>" + tr("Floors down count:") + "</td>" +
                    "<td>" + QString::number(info->stairsFloorsDownNumber, 'f', 0) + "</td></tr>" +

                    "<tr><td>" + tr("Floors up count:") + "</td>" +
                    "<td>" + QString::number(info->stairsFloorsUpNumber, 'f', 0) + "</td></tr>";
        else
            stairsInfo =
                    "<tr><td>" + tr("<br><i>No stairs.</i>") + "</td></tr>";
        if (info->liftsNumber != 0)
            liftsInfo =
                    "<tr><td>" + tr("<br><i>Lifts: </i>") + "</td></tr>" +

                    "<tr><td>" + tr("Count:") + "</td>" +
                    "<td>" + QString::number(info->liftsNumber, 'f', 0) + "</td></tr>" +

                    "<tr><td>" + tr("Floors down count:") + "</td>" +
                    "<td>" + QString::number(info->liftsFloorsDownNumber, 'f', 0) + "</td></tr>" +

                    "<tr><td>" + tr("Floors up count:") + "</td>" +
                    "<td>" + QString::number(info->liftsFloorsUpNumber, 'f', 0) + "</td></tr></table>";
        else
            liftsInfo =
                    "<tr><td>" + tr("<br><i>No lifts.</i>") + "</td></tr></table>";

        lblWayInfo->setText(floorsInfo + stairsInfo + liftsInfo);
    }
}

void MainWindow::way()
{
    if (map->isStartAndFinishNodesValid())
    {
        GraphArc::WayPermissions permissions;
        Map::WayInfo *info;

        lblWaysChoice->setText(tr("<b>Way settings</b>"));

        lstwgtWays->clear();

        permissions = GraphArc::None;
        map->way(permissions);
        info = map->wayInfo();
        if (map->isWayExist())
        {
            QListWidgetItem *item = new QListWidgetItem(
			            QString::number(lstwgtWays->count() + 1) + ". " +
                    tr("On the floor"), lstwgtWays);
            item->setData(Qt::UserRole, (int)permissions);
        }

        permissions = GraphArc::Stairs;
        map->way(permissions);
        info = map->wayInfo();
        if (info->stairsNumber != 0)
        {
            QListWidgetItem *item = new QListWidgetItem(
			            QString::number(lstwgtWays->count() + 1) + ". " +
                    tr("With stairs"), lstwgtWays);
            item->setData(Qt::UserRole, (int)permissions);
        }

        permissions = GraphArc::Lift;
        map->way(permissions);
        info = map->wayInfo();
        if (info->liftsNumber != 0)
        {
            QListWidgetItem *item = new QListWidgetItem(
			            QString::number(lstwgtWays->count() + 1) + ". " +
                    tr("With lifts"), lstwgtWays);
            item->setData(Qt::UserRole, (int)permissions);
        }

        permissions = GraphArc::Stairs | GraphArc::Lift;
        map->way(permissions);
        info = map->wayInfo();
        if ((info->stairsNumber != 0) & (info->liftsNumber != 0))
        {
            QListWidgetItem *item = new QListWidgetItem(
			            QString::number(lstwgtWays->count() + 1) + ". " +
                    tr("With stairs and "), lstwgtWays);
            item->setData(Qt::UserRole, (int)permissions);
        }

        if (lstwgtWays->count())
        {
            lstwgtWays->setVisible(true);
            lstwgtWays->setCurrentItem(lstwgtWays->item(0));
        }
        else
            lblWayInfo->setText(
                    tr("Unable to find a way between the given objects."));
    }
}

void MainWindow::help()
{
    QDesktopServices::openUrl(QUrl(qApp->applicationDirPath() + "/Help.pdf"));
}

void MainWindow::about()
{
    QString s;
#ifdef EDITOR
    s = tr("This program is designed for creating and displaying floor "
	       "plans of buildings, searching required areas and finding "
	       "shortest ways between objects.");
#else
    s = tr("This program is designed for displaying floor "
	       "plans of buildings, searching required areas and finding "
	       "shortest ways between objects.");
#endif
    QMessageBox::about(this, tr("About"), "<b>" + qApp->applicationName()
                       + " " + qApp->applicationVersion() + "</b><br>" + s);
}

void MainWindow::updateStatusBar()
{
//    if ((map->floorsNumber() <= 0) | (curFloor = -1))
//        return;

    MapFloor *floor = map->floor(curFloor);
    QPointF pos = map->pos(floor);

    QSizeF mapSize = map->size();

    QSizeF size;
    QString s;
    MapArea *area = selectedArea();
    if (area)
    {
        size = map->areaSize(area);
        s = tr("Area: ");
    }
    else
    {
        QPointF tl(view->horizontalScrollBar()->value(),
                   view->verticalScrollBar()->value());
        QPointF br = tl + view->viewport()->rect().bottomRight();
        QMatrix mat = view->matrix().inverted();
        size = map->convertPixToRealM(mat.mapRect(QRectF(tl,br)).size());
        s = tr("Displayed: ");
    }
    QString message =
            tr("Cursor's coordinates: ") +
	        QString::number(pos.x(), 'f', 1) + ", " +
	        QString::number(pos.y(), 'f', 1) + tr(" m") + "; " +
            s +
            QString::number(
                    qMin(size.width(), mapSize.width()), 'f', 0) + tr("x") +
            QString::number(
                    qMin(size.height(), mapSize.height()), 'f', 0) + tr(" m");
}
