#include "mainwindow.h"
#include <QDebug>
#include "../Common/mapfloor.h"
#include "../Common/map.h"
#include "../Common/maparea.h"

MainWindow::MainWindow(MainWindow::Program program, QWidget *parent)
    : QMainWindow(parent), m_program(program)
{
    qApp->setWindowIcon(QIcon(":/Ariadne"));
    if (m_program == pFull)
    {
        qApp->setApplicationName(tr("Ариадна"));
    }
    else
    {
//        qApp->setWindowIcon(QIcon(":/AppIcon"));
        qApp->setApplicationName(tr("Тесей"));
    }
    qApp->setApplicationVersion("1.0");
    setWindowTitle(qApp->applicationName());

    map = 0;
    view = 0;
    curFloor = 0;
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
    if (m_program == pFull)
    {
        createPanelFloorsManagement();
        createPanelVerticals();
    }
    createPanelAreasMarking();
    createPanelWay();

    setState(eFile | eHelp, stTrue, stTrue);
    setState(eEdit | eView | eGo | eAdd | ePanels | eFloorsManagement |
             eVerticals | eAreasMarking | eWay, stFalse, stFalse);
    actMapSave->setEnabled(false);
    actMapSaveAs->setEnabled(false);
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


    actZoomOut= new QAction(QIcon(":/ZoomOut"), tr("У&меньшить"), this);
    connect(actZoomOut, SIGNAL(triggered()), SLOT(zoomOut()));
    actZoomOut->setShortcut(/*Qt::CTRL + */tr("-"));

    actZoomIn = new QAction(QIcon(":/ZoomIn"), tr("&Увеличить"), this);
    connect(actZoomIn, SIGNAL(triggered()), SLOT(zoomIn()));
    actZoomIn->setShortcut(/*Qt::CTRL + */tr("+"));

    actZoomFit = new QAction(QIcon(":/ZoomFit"), tr("&Вместить"), this);
    connect(actZoomFit, SIGNAL(triggered()), SLOT(zoomFit()));
    actZoomFit->setShortcut(/*Qt::CTRL + */tr("*"));

    actHandDrag = new QAction(QIcon(":/HandDrag"),
                              tr("&Панорамирование"), this);
    connect(actHandDrag, SIGNAL(triggered(bool)), SLOT(handDrag(bool)));
    actHandDrag->setShortcuts(shortcuts(tr("Ctrl+H"), tr("H")));
    actHandDrag->setCheckable(true);

    actAddBase = new QAction(QIcon(":/AddBase"),
                             tr("Добавить &подложку"), this);
    connect(actAddBase, SIGNAL(triggered()), SLOT(addBase()));
    actAddBase->setShortcuts(shortcuts(tr("Ctrl+B"), tr("B")));

    actLayerBase = new QAction(QIcon(":/LayerBase"), tr("&Подложка"), this);
    connect(actLayerBase, SIGNAL(triggered(bool)),
            SLOT(layerBaseSetVisible(bool)));
    actLayerBase->setCheckable(true);
    actLayerBase->setChecked(true);

    actLayerCrossBase = new QAction(QIcon(":/LayerCrossBase"),
                                    tr("&Клетки"), this);
    connect(actLayerCrossBase, SIGNAL(triggered(bool)),
            SLOT(layerCrossBaseSetVisible(bool)));
    actLayerCrossBase->setCheckable(true);
//    actLayerCrossBase->setChecked(true);

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
    actMagnetToExtensions->setShortcut(tr("F5"));
    actMagnetToExtensions->setCheckable(true);


//    actAddWall = new QAction(QIcon(":/AddWall"), tr("Добавить &стену"), this);
//    connect(actAddWall, SIGNAL(triggered()), SLOT(addWall()));
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


    actPanelAreasMarking = new QAction(QIcon(":/AreasMarking"),
                                         tr("&Разметка помещений"), this);
    actPanelAreasMarking->setData(eAreasMarking);
    if (m_program == pFull)
        actPanelAreasMarking->setShortcuts(shortcuts(tr("Ctrl+3"), tr("3")));
    else
        actPanelAreasMarking->setShortcuts(shortcuts(tr("Ctrl+1"), tr("1")));
    actPanelAreasMarking->setCheckable(true);


    actPanelWay = new QAction(QIcon(":/Way"), tr("&Путь"), this);
    actPanelWay->setData(eWay);
    if (m_program == pFull)
        actPanelWay->setShortcuts(shortcuts(tr("Ctrl+4"), tr("4")));
    else
        actPanelWay->setShortcuts(shortcuts(tr("Ctrl+2"), tr("2")));
    actPanelWay->setCheckable(true);

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
    actgrpPanels->addAction(actPanelFloorsManagement);
    actgrpPanels->addAction(actPanelVerticals);
    actgrpPanels->addAction(actPanelAreasMarking);
    actgrpPanels->addAction(actPanelWay);
    connect(actgrpPanels, SIGNAL(triggered(QAction *)),
            SLOT(actgrpPanelsTriggered(QAction *)));
}

void MainWindow::createMenus()
{
    menuFile = menuBar()->addMenu(tr("&Файл"));
    if (m_program == pFull)
        menuFile->addAction(actMapNew);
    menuFile->addAction(actMapOpen);
    if (m_program == pFull)
        menuFile->addAction(actMapSave);
    menuFile->addAction(actMapSaveAs);
    menuFile->addAction(actQuit);

    if (m_program == pFull)
    {
        menuEdit = menuBar()->addMenu(tr("&Правка"));
        menuEdit->addAction(actAreaCopy);
        menuEdit->addAction(actGraphCopy);
    }

    menuView = menuBar()->addMenu(tr("&Вид"));
    if (m_program == pFull)
    {
        menuView->addAction(actAddBase);
        menuView->addSeparator();
        menuView->addAction(actLayerBase);
        menuView->addAction(actLayerCrossBase);
        menuView->addAction(actLayerGraph);
        menuView->addSeparator();
    }
    menuView->addAction(actZoomOut);
    menuView->addAction(actZoomIn);
    menuView->addAction(actZoomFit);

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

//    menuAdd = menuBar()->addMenu(tr("&Добавить"));
////    menuAdd->addAction(actFloorAdd);
////    menuAdd->addAction(actAddWall);
//    menuAdd->addAction(actAddArea);
//    menuAdd->addAction(actAddDoor);
//    menuAdd->addAction(actAddNode);

    menuHelp = menuBar()->addMenu(tr("&Справка"));
    menuHelp->addAction(actAbout);
    //menuHelp->addAction(actAboutQT);
}

void MainWindow::createToolBars()
{
    tbrFile = addToolBar(tr("Файл"));
    tbrFile->setFloatable(false);
    if (m_program == pFull)
        tbrFile->addAction(actMapNew);
    tbrFile->addAction(actMapOpen);
    if (m_program == pFull)
        tbrFile->addAction(actMapSave);
    else
        tbrFile->addAction(actMapSaveAs);
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
    tbrView->addAction(actMagnetToExtensions);
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

    if (m_program == pFull)
    {
        tbrAdd = addToolBar(tr("Добавить"));
        tbrAdd->setFloatable(false);
    //    tbrAdd->addAction(actAddWall);
        tbrAdd->addAction(actAddArea);
        tbrAdd->addAction(actAddDoor);
        tbrAdd->addAction(actAddNode);
    }

    tbrPanels = new QToolBar(tr("Панели"));
    addToolBar(Qt::LeftToolBarArea, tbrPanels);
    tbrPanels->setFloatable(false);
    if (m_program == pFull)
    {
        tbrPanels->addAction(actPanelFloorsManagement);
        tbrPanels->addAction(actPanelVerticals);
    }
    tbrPanels->addAction(actPanelAreasMarking);
    tbrPanels->addAction(actPanelWay);
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
//    connect(viewFloorsList, SIGNAL(activated(QModelIndex)),
//            SLOT(viewFloorsListItemActivated(QModelIndex)));
    connect(modelFloorsList, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            SLOT(viewFloorsListItemChanged(QModelIndex)));
    viewFloorsList->setModel(modelFloorsList);
    viewFloorsList->setSelectionModel(selectionFloorsList);
    vblFloorsManagement->addWidget(viewFloorsList);
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
    cbxVerticalType->insertItem(0, tr("Аудитория"));
    cbxVerticalType->insertItem(1, tr("Лестница"));
    cbxVerticalType->insertItem(2, tr("Лифт"));
//    new QListWidgetItem(tr("Аудитория"), cbxVerticalType);
//    new QListWidgetItem(tr("Лестница"), cbxVerticalType);
//    new QListWidgetItem(tr("Лифт"), cbxVerticalType);
    connect(cbxVerticalType, SIGNAL(currentIndexChanged(int)),
            SLOT(verticalSetType(int)));
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

void MainWindow::createPanelAreasMarking()
{
    dckwgtAreasMarking = new QDockWidget(tr("Свойства помещения"));
    connect(dckwgtAreasMarking, SIGNAL(visibilityChanged(bool)),
            SLOT(panelAreasMarkingVisibilityChanged(bool)));
    addDockWidget(Qt::LeftDockWidgetArea, dckwgtAreasMarking);
    dckwgtAreasMarking->setFeatures(
            QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

    wgtAreasMarking = new QWidget;
    dckwgtAreasMarking->setWidget(wgtAreasMarking);

    vblAreasMarking = new QVBoxLayout(wgtAreasMarking);

    lblAreaNumber = new QLabel(tr("Номер: "));
    vblAreasMarking->addWidget(lblAreaNumber);

    ldtAreaNumber = new QLineEdit();
    connect(ldtAreaNumber, SIGNAL(textChanged(QString)), SLOT(setAreaNumber()));
    ldtAreaNumber->setReadOnly(m_program != pFull);
    vblAreasMarking->addWidget(ldtAreaNumber);

    lblAreaName = new QLabel(tr("Название: "));
    vblAreasMarking->addWidget(lblAreaName);

    ldtAreaName = new QLineEdit();
    connect(ldtAreaName, SIGNAL(textChanged(QString)), SLOT(setAreaName()));
    ldtAreaName->setReadOnly(m_program != pFull);
//    ldtAreaName->setMaximumHeight(
//            (ptdtAreaName->fontMetrics().lineSpacing() +
//             ptdtAreaName->font().pointSize())*2);
//    // ### Must be rewrited
//    ptdtAreaName->setMinimumWidth(cDockWidth);
//    ptdtAreaName->setMinimumWidth(0);
    vblAreasMarking->addWidget(ldtAreaName);

    lblAreaDescription = new QLabel(tr("Описание: "));
    vblAreasMarking->addWidget(lblAreaDescription);

    ptdtAreaDescription = new QPlainTextEdit("");
    connect(ptdtAreaDescription, SIGNAL(textChanged()),
            SLOT(setAreaDescription()));
    ptdtAreaDescription->setReadOnly(m_program != pFull);
    vblAreasMarking->addWidget(ptdtAreaDescription);

    if (m_program == pFull)
    {
        btnUpdateAreaInscription = new QPushButton(tr("Обновить надпись"));
        connect(btnUpdateAreaInscription, SIGNAL(clicked()),
                SLOT(updateAreaInscription()));
        vblAreasMarking->addWidget(btnUpdateAreaInscription);

        lblAreaInscription = new QLabel(tr("Надпись: "));
        vblAreasMarking->addWidget(lblAreaInscription);

        ptdtAreaInscription = new QPlainTextEdit("");
        connect(ptdtAreaInscription, SIGNAL(textChanged()),
                SLOT(setAreaInscription()));
        vblAreasMarking->addWidget(ptdtAreaInscription);
    }

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

void MainWindow::createPanelWay()
{
    dckwgtWay = new QDockWidget(tr("Путь"));
    connect(dckwgtWay, SIGNAL(visibilityChanged(bool)),
            SLOT(panelWayVisibilityChanged(bool)));
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

    lstwgtWays = new QListWidget();
    connect(lstwgtWays, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            SLOT(lstwgtWaysCurrentItemChanged(QListWidgetItem*,QListWidgetItem*)));
//    connect(lstwgtWays, SIGNAL(itemActivated(QListWidgetItem*)),
//            SLOT(lstwgtWaysItemActivated(QListWidgetItem*)));
    lstwgtWays->setVisible(false);
    vblWay->addWidget(lstwgtWays);

    lblWayInfo = new QLabel(tr("Для того, чтобы найти путь выберите объект, "
                               "откуда вы хотите начать путь и нажмите кнопку "
                               "\"Старт\". Аналогично обозначьте финиш."));
    lblWayInfo->setWordWrap(true);
//            tr("Для того, чтобы найти путь\nвыберите стартовое помещение или\n"
//               "дверь и нажмите кнопку \"Старт\".\nАналогично обозначьте финиш.\n"
//               "Программа выведет список\nкратчайших маршрутов (с учётом средств"
//               "передвижения)."));
    vblWay->addWidget(lblWayInfo);

    vblWay->addStretch();
}

void MainWindow::blockAndFreePanelAreasMarking()
{
    setState(eAreasMarking, stSave, stFalse);
    ldtAreaNumber->setText("");
    ldtAreaName->setText("");
    ptdtAreaDescription->setPlainText("");
    if (m_program == pFull)
        ptdtAreaInscription->setPlainText("");
//    chkAreaNumberVisible->setChecked(false);
//    chkAreaNameVisible->setChecked(false);
}

void MainWindow::updateAreaInscription()
{
    QString number = ldtAreaNumber->text();
    QString name = ldtAreaName->text();
    QString description = ptdtAreaDescription->toPlainText();
    QString inscription = "";
    inscription += number;
    if (!name.isEmpty())
        if (!inscription.isEmpty())
            inscription += "\n";
    inscription += name;
    if (!description.isEmpty())
        if (!inscription.isEmpty())
            inscription += "\n";
    inscription += description;
    ptdtAreaInscription->setPlainText(inscription);
    ptdtAreaInscription->document()->setModified(false);
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
        if (m_program == pFull)
        {
            setState(eFile | eEdit | eGo | eView | eAdd |
                     eHelp | ePanels, stTrue, stTrue);
            setState(eFloorsManagement | eVerticals | eAreasMarking | eWay,
                     stFalse, stFalse);
        }
        else
        {
            layerBaseSetVisible(false);
            layerGraphSetVisible(false);
            setState(eFile | eGo | eView | eHelp | ePanels,
                     stTrue, stTrue);
            setState(eEdit | eAdd | eFloorsManagement | eAreasMarking,
                     stFalse, stFalse);
        }

        switchMode(MapFloor::Idle);
    }
    else
    {
        lstwgtVerticals->clear();
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
            if (m_program == pFull)
            {
                actMapNew->setVisible(visible);
                actMapSave->setVisible(visible);
            }
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
        if ((m_program == pFull) & ((elem & eEdit) > 0))
        {
            actAreaCopy->setVisible(visible);
            actGraphCopy->setVisible(visible);

            menuEdit->menuAction()->setVisible(visible);
//            tbrEdit->setVisible(visible);
        }
        if (elem & eView)
        {
            if (m_program == pFull)
            {
                actAddBase->setVisible(visible);
                actMagnetToExtensions->setVisible(visible);
            }
//            else
//            {
//                actAddBase->setVisible(false);
//                actMagnetToExtensions->setVisible(false);
//            }
            actHandDrag->setVisible(visible);

            if (m_program == pFull)
            {
                actLayerBase->setVisible(visible);
                actLayerCrossBase->setVisible(visible);
                actLayerGraph->setVisible(visible);
            }

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
        if ((m_program == pFull) & ((elem & eAdd) > 0))
        {
//            actAddWall->setVisible(visible);
            actAddArea->setVisible(visible);
            actAddDoor->setVisible(visible);
            actAddNode->setVisible(visible);

//            menuAdd->menuAction()->setVisible(visible);
            tbrAdd->setVisible(visible);
        }
        if (elem & eHelp)
        {
            actAbout->setVisible(visible);

            menuHelp->menuAction()->setVisible(visible);
        }
        if (elem & ePanels)
        {
            actgrpPanels->setVisible(visible);
            if (m_program == pFull)
            {
                actPanelFloorsManagement->setVisible(visible);
                actPanelVerticals->setVisible(visible);
            }
//            else
//            {
//                actPanelFloorsManagement->setVisible(false);
//                actPanelVerticals->setVisible(false);
//            }
            actPanelAreasMarking->setVisible(visible);
            actPanelWay->setVisible(visible);

//            menuPanels->menuAction()->setVisible(visible);
            tbrPanels->setVisible(visible);
        }
        if ((m_program == pFull) & ((elem & eFloorsManagement) > 0))
        {
            dckwgtFloorsManagement->setVisible(visible);
            if (!visible)
                if (view)
                    setFocus();
        }
        if ((m_program == pFull) & ((elem & eVerticals) > 0))
        {
            dckwgtVerticals->setVisible(visible);
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
//            if (m_program != pFull)
//            {
//                btnUpdateAreaInscription->setVisible(false);
//                lblAreaInscription->setVisible(false);
//                ptdtAreaInscription->setVisible(false);
//            }
        }
        if (elem & eWay)
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
            if (m_program == pFull)
            {
                actMapNew->setEnabled(enable);
                actMapSave->setEnabled(enable);
            }
//            else
//            {
//                actMapNew->setEnabled(false);
//                actMapSave->setEnabled(false);
//            }
            actMapOpen->setEnabled(enable);
            actMapSaveAs->setEnabled(enable);
            actQuit->setEnabled(enable);

            menuFile->menuAction()->setEnabled(enable);
            tbrFile->setEnabled(enable);
        }
        if ((m_program == pFull) & ((elem & eEdit) > 0))
        {
            actAreaCopy->setEnabled(enable);
            actGraphCopy->setEnabled(enable);

            menuEdit->menuAction()->setEnabled(enable);
//            tbrEdit->setEnabled(enable);
        }
        if (elem & eView)
        {
            if (m_program == pFull)
            {
                actAddBase->setEnabled(enable);
                actMagnetToExtensions->setEnabled(enable);
            }
//            else
//            {
//                actAddBase->setEnabled(false);
//                actMagnetToExtensions->setEnabled(false);
//            }
            actHandDrag->setEnabled(enable);

            if (m_program == pFull)
            {
                actLayerBase->setEnabled(enable);
                actLayerCrossBase->setEnabled(enable);
                actLayerGraph->setEnabled(enable);
            }

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
        if ((m_program == pFull) & ((elem & eAdd) > 0))
        {
//            actAddWall->setEnabled(enable);
            actAddArea->setEnabled(enable);
            actAddDoor->setEnabled(enable);
            actAddNode->setEnabled(enable);

//            menuAdd->menuAction()->setEnabled(enable);
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
            if (m_program == pFull)
            {
                actPanelFloorsManagement->setEnabled(enable);
                actPanelVerticals->setEnabled(enable);
            }
            else
            {
                actPanelFloorsManagement->setEnabled(false);
                actPanelVerticals->setEnabled(false);
            }
            actPanelAreasMarking->setEnabled(enable);
            actPanelWay->setEnabled(enable);

            actgrpPanels->setEnabled(enable);
//            menuPanels->menuAction()->setEnabled(enable);
            tbrPanels->setEnabled(enable);
        }
        if ((m_program == pFull) & ((elem & eFloorsManagement) > 0))
        {
            wgtFloorsManagement->setEnabled(enable);

//            actPanelFloorsManagement->setEnabled(enable);
            //        dckwgtFloorsManagement->setEnabled(en);
        }
        if ((m_program == pFull) & (elem & eVerticals))
        {
            wgtVerticals->setEnabled(enable);

//            actPanelVerticals->setEnabled(enable);
            //        dckwgtFloorsManagement->setEnabled(en);
        }
        if (elem & eAreasMarking)
        {
            if (!enable)
                dckwgtAreasMarking->setVisible(false);
            wgtAreasMarking->setEnabled(enable);
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
        if (elem & eWay)
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
    if (!view | (m_program != pFull))
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

            if (m_program == pFull)
            {
                lstwgtVerticals->blockSignals(true);
                for (int i = 0; i != map->verticalsNumber(); i++)
                {
                    QListWidgetItem *item =
                            new QListWidgetItem(map->vertical(i)->name());
                    item->setFlags (item->flags () | Qt::ItemIsEditable);
                    lstwgtVerticals->insertItem(lstwgtVerticals->count(), item);
                }
                lstwgtVerticals->blockSignals(false);
            }

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
            QMessageBox::warning(0, tr("Ошибка при копировании области"),
                                 tr("В здании только 1 этаж. Копирование "
                                    "области на этаж, на котором она "
                                    "находится невозможно."));
            return;
        }
    DialogFloorChoice* dialog = new DialogFloorChoice(
            this, modelFloorsList, i, tr("Выберите этаж, на который будет"
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

void MainWindow::layerCrossBaseSetVisible(bool visible)
{
    for (int i = 0; i != map->floorsNumber(); i++)
        map->floor(i)->crossBaseSetVisible(visible);
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
        blockAndFreePanelAreasMarking();
        if (m_program == pFull)
        {
            setState(eFile | eEdit | eView | eGo | eAdd | eHelp | ePanels,
                     stTrue, stTrue);
            setState(eFloorsManagement | eVerticals | eWay, stSave, stTrue);
        }
        else
        {
            setState(eFile | eView | eGo | eHelp | ePanels, stTrue, stTrue);
            setState(eEdit | eAdd | eFloorsManagement | eVerticals,
                     stFalse, stFalse);
            setState(eWay, stSave, stTrue);
        }
        setCursor(Qt::ArrowCursor);
        break;
//    case MapFloor::WallAdd:
    case MapFloor::AreaAdd:
    case MapFloor::DoorAdd:
    case MapFloor::NodeAdd:
        modeName = MapFloor::modeName(m);
        setState(eFile | eView | eHelp | ePanels, stTrue, stTrue);
        setState(eEdit | eGo | eAdd | eFloorsManagement | eVerticals |
                 eAreasMarking | eWay, stSave, stFalse);
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
                    setState(eAreasMarking | eWay, stSave, stTrue);
                    MapArea *area = qgraphicsitem_cast<MapArea*>(
                            map->floor(curFloor)->selection()->item());
                    bool isModified = false;
                    isModified = (!area->number().isEmpty() |
                                  !area->name().isEmpty() |
                                  !area->description().isEmpty() |
                                  !area->inscription().isEmpty());
                    if (m_program == pFull)
                        ptdtAreaInscription->document()->setModified(isModified);
                    ldtAreaNumber->setText(area->number());
                    ldtAreaName->setText(area->name());
                    ptdtAreaDescription->setPlainText(area->description());
                    if (m_program == pFull)
                    {
                        ptdtAreaInscription->setPlainText(area->inscription());
                        ptdtAreaInscription->document()->setModified(isModified);
                    }
                    ldtAreaNumber->selectAll();
                    if (dckwgtAreasMarking->isVisible())
                        ldtAreaNumber->setFocus();;
                }
            if (!selectionIsValid)
            {
                blockAndFreePanelAreasMarking();
                setState(eWay, stSave, stTrue);
            }
            break;
        }
    }
    windowTitle = fileName + " - " + qApp->applicationName();
    if (!modeName.isNull())
        windowTitle += " (" + modeName + ")";
    setWindowTitle(windowTitle);
    mode = m;
    if (map->floorsNumber() != 0)
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
            actFloorMoveUp->setEnabled(false);
            actFloorUp->setEnabled(false);
            actFloorUpper->setEnabled(false);
        }
        else
        {
            actFloorMoveUp->setEnabled(true);
            actFloorUp->setEnabled(true);
            actFloorUpper->setEnabled(true);
        }
        if (i == map->floorsNumber() - 1)
        {
            actFloorMoveDown->setEnabled(false);
            actFloorLower->setEnabled(false);
            actFloorDown->setEnabled(false);
        }
        else
        {
            actFloorMoveDown->setEnabled(true);
            actFloorLower->setEnabled(true);
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

void MainWindow::viewFloorsListCurrentItemChanged(const QModelIndex &current, const QModelIndex &previous)
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
    QListWidgetItem *item = new QListWidgetItem(tr("Новая вертикаль"), lstwgtVerticals);
    item->setFlags(item->flags () | Qt::ItemIsEditable);
    map->addVertical();
    map->setNameVertical(lstwgtVerticals->row(item), item->text());
//    lstwgtVerticals->insertItem(->count(), item);
    lstwgtVerticals->setCurrentItem(item);
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

void MainWindow::verticalSetType(int type)
{
    map->setTypeVertical(lstwgtVerticals->currentRow(),
                         GraphArc::VerticalType(type));
}

void MainWindow::lstwgtVerticalsCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
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
    }
}

void MainWindow::lstwgtVerticalsItemChanged(QListWidgetItem *item)
{
    map->setNameVertical(lstwgtVerticals->currentRow(), item->text());
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
        actPanelAreasMarking->trigger();
}

void MainWindow::mouseMiddleButtonClicked(QGraphicsItem *item)
{
    view->fitInView(item, Qt::KeepAspectRatio);
}

void MainWindow::panelAreasMarkingVisibilityChanged(bool visible)
{
    actPanelAreasMarking->setChecked(visible);
}

void MainWindow::setAreaNumber()
{
    if (map->floorsNumber() > 0)
        if (map->floor(curFloor)->selection()->item() != 0)
            if (map->floor(curFloor)->selection()->item()->type() == MapArea::Type)
            {
                MapArea *area = qgraphicsitem_cast<MapArea*>(
                        map->floor(curFloor)->selection()->item());
                area->setNumber(ldtAreaNumber->text());
                if (m_program == pFull)
                    if (!ptdtAreaInscription->document()->isModified())
                        updateAreaInscription();
            }
}

void MainWindow::setAreaName()
{
    if (map->floorsNumber() > 0)
        if (map->floor(curFloor)->selection()->item() != 0)
            if (map->floor(curFloor)->selection()->item()->type() == MapArea::Type)
            {
                MapArea *area = qgraphicsitem_cast<MapArea*>(
                        map->floor(curFloor)->selection()->item());
                area->setName(ldtAreaName->text());
                if (m_program == pFull)
                    if (!ptdtAreaInscription->document()->isModified())
                        updateAreaInscription();
            }
}

void MainWindow::setAreaDescription()
{
    if (map->floorsNumber() > 0)
        if (map->floor(curFloor)->selection()->item() != 0)
            if (map->floor(curFloor)->selection()->item()->type() == MapArea::Type)
            {
                MapArea *area = qgraphicsitem_cast<MapArea*>(
                        map->floor(curFloor)->selection()->item());
                area->setDescription(
                        ptdtAreaDescription->document()->toPlainText());
                if (m_program == pFull)
                    if (!ptdtAreaInscription->document()->isModified())
                        updateAreaInscription();
            }
}

void MainWindow::setAreaInscription()
{
    if (map->floorsNumber() > 0)
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

void MainWindow::panelWayVisibilityChanged(bool visible)
{
    actPanelWay->setChecked(visible);
}

void MainWindow::setStart()
{
    if (mode == MapFloor::Selection)
    {
        map->setStart(map->floor(curFloor)->selection()->item());
        way();
    }
}

void MainWindow::setFinish()
{
    if (mode == MapFloor::Selection)
    {
        map->setFinish(map->floor(curFloor)->selection()->item());
        way();
    }
}

void MainWindow::lstwgtWaysCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
//    if (item->data(Qt::UserRole).canConvert<Graph::WayPermissions>())
//    {
    if (current)
    {
        GraphArc::WayPermissions permissions =
                (GraphArc::WayPermissions)current->data(Qt::UserRole).toInt();
        map->way(permissions);
        map->paintWay();
        qreal length;
        int stairsDownNumer;
        int stairsUpNumer;
        int liftsNumber;
        qreal time;
        map->wayInfo(length, stairsDownNumer, stairsUpNumer, liftsNumber, time);
        lblWayInfo->setText(
                tr("<b>Информация о пути:</b><br>"
                   "Длина (по горизонтали): ") + QString::number(length, 'f', 0) + tr(" м") + "<br>" +
                tr("Число этажей вниз: ") + QString::number(stairsDownNumer, 'f', 0) + "<br>" +
                tr("Число этажей вверх: ") + QString::number(stairsUpNumer, 'f', 0) + "<br>" +
                tr("Число лифтов: ") + QString::number(liftsNumber, 'f', 0) + "<br>" +
                tr("Время: ") + QString::number(time, 'f', 0) + tr(" мин") + "<br>");
    }

//    }

}

void MainWindow::way()
{
    if (map->isStartAndFinishNodesValid())
    {
        qreal length;
        int stairsDownNumer;
        int stairsUpNumer;
        int liftsNumber;
        qreal time/*, timeStairs, timeLift*/;
        GraphArc::WayPermissions permissions;

        lstwgtWays->clear();

        permissions = GraphArc::None;
        map->way(permissions);
        if (map->wayInfo(length, stairsDownNumer, stairsUpNumer, liftsNumber, time))
        {
            QListWidgetItem *item = new QListWidgetItem(
                    tr("По горизонтали (") + QString::number(time, 'f', 0) + tr(" мин)"), lstwgtWays);
            item->setData(Qt::UserRole, (int)permissions);
    //        lstwgtWays->insertItem(lstwgtWays->count(), );
        }

        permissions = GraphArc::Stairs;
        map->way(permissions);
        if (map->wayInfo(length, stairsDownNumer, stairsUpNumer, liftsNumber, time) & (stairsDownNumer + stairsUpNumer > 0))
        {
            QListWidgetItem *item = new QListWidgetItem(
                    tr("По горизонтали + лестницы (") + QString::number(time, 'f', 0) + tr(" мин)"), lstwgtWays);
            item->setData(Qt::UserRole, (int)permissions);
        }
//        timeStairs = time;
    //    lstwgtWays->insertItem(lstwgtWays->count(), tr("Без катания на лифтах (" + time + ")");

        permissions = GraphArc::Lift;
        map->way(permissions);
        if (map->wayInfo(length, stairsDownNumer, stairsUpNumer, liftsNumber, time) & (liftsNumber > 0))
        {
            QListWidgetItem *item = new QListWidgetItem(
                    tr("По горизонтали + лифты (") + QString::number(time, 'f', 0) + tr(" мин)"), lstwgtWays);
            item->setData(Qt::UserRole, (int)permissions);
        }
//        timeLift = time;

        permissions = GraphArc::Stairs | GraphArc::Lift;
        map->way(permissions);
        if (map->wayInfo(length, stairsDownNumer, stairsUpNumer, liftsNumber, time) & ((stairsDownNumer + stairsUpNumer > 0) & (liftsNumber > 0)))
        {
//            if (!(((liftsNumber == 0) & (timeStairs == time)) |
//                ((stairsDownNumer + stairsUpNumer == 0) & (timeLift == time))))
//            if ((stairsDownNumer + stairsUpNumer > 0) & (liftsNumber > 0))
//            {
                QListWidgetItem *item = new QListWidgetItem(
                        tr("По горизонтали + лестницы + лифты (") + QString::number(time, 'f', 0) + tr(" мин)"), lstwgtWays);
                item->setData(Qt::UserRole, (int)permissions);
//            }
        }

        if (lstwgtWays->count())
        {
            lstwgtWays->setVisible(true);
            lstwgtWays->setCurrentItem(lstwgtWays->item(0));
//            lstwgtWaysItemActivated(, );
        }
        else
        {
            lblWayInfo->setText(tr("Облом!"));
        }
    }
}

void MainWindow::about()
{
    QString s;
    if (m_program == pFull)
        s = tr("Программа предназначена для перевода пользователем "
               "поэтажного плана здания в собственный векторный "
               "формат, добавления информации об объектах, "
               "автоматизированного создания и редактирования графа здания.");
    else
        s = tr("Программа предназначена для отображения поэтажного плана "
               "здания, поиска нужных объектов и нахождения между ними "
               "кратчайшего пути.");
    QMessageBox::about(this, tr("О программе"), "<b>" + qApp->applicationName()
                       + " " + qApp->applicationVersion() + "</b>"
                       + "<br>" + s);
}
