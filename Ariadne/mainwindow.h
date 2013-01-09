#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QActionGroup>
#include <QActionGroup>
#include <QApplication>
#include <QComboBox>
#include <QCheckBox>
#include <QCursor>
#include <QDesktopServices>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QMainWindow>
#include <QMatrix>
#include <QMenu>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMotifStyle>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QPushButton>
#include <QRectF>
#include <QItemSelectionModel>
#include <QScrollBar>
#include <QShortcut>
#include <QStatusBar>
#include <QStringListModel>
#include <QTime>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

#include "../Common/global.h"
#include "../Common/map.h"
#include "../Common/mapfloor.h"
#include "dialogmapproperties.h"
#include "dialogmapsize.h"
#include "dialogfloorchoice.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum Program
    {
        pFull, pViewer, pViewerTerminal
    };
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    enum Element
    {
        eFile = 1,
        eEdit = 2,
        eView = 4,
        eGo = 8,
        eAdd  = 16,
        eQuickSearch = 32,
        eHelp = 64,
        ePanels = 128,
        eFloors = 256,
        eVerticals = 512,
        eAreasProperties = 1024,
        eSearch = 2048,
        eWays = 4096
    };
    Q_DECLARE_FLAGS(Elements, Element)

protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void mapNew();                      // Creating map
	void mapOpenShowError();            // Show error on opening
    void mapOpen();                     // Opening map
    void mapSave();                     // Saving map to open file
    void mapSaveAs();                   // Saving map to another file
    void mapExport();                   // Export map to bitmap file
    void mapProperties();               // Map properties
    void exit();                        // Exit program
    void areaCopy();                    // Copy area to another floor
    void graphCopy();                   // Copy graph to another floor
    void floorLower();
    void floorDown();                   // Set visible lower floor
    void floorUp();                     // Set visible higher floor
    void floorUpper();
    void zoomOut();
    void zoomIn();
    void zoomFit();
    void zoomOn(QGraphicsItem *item);
    void handDrag(bool checked);
    void addBase();
    void layerBaseSetVisible(bool visible);
    void layerGridSetVisible(bool visible);
    void layerGraphSetVisible(bool visible);
    void snapToExtensions(bool b);
    void addArea();                     // Adding area
    void addDoor();                     // Adding door
    void addNode();                     // Adding graph node
    void ldtQuickSearchTextChanged(const QString &text);
    void actgrpPanelsTriggered(QAction *);    // Link between actgrpMode & switchMode
    void switchMode(MapFloor::Mode m);  // Apply app to proper mode
    void setActiveFloor(int i);         // Change visible floor

    void panelFloorsVisibilityChanged(bool visible);
    void floorAdd();                    // Adding floor
    void floorDelete();
    void floorMoveDown();
    void floorMoveUp();
    void floorSetDefault();
    void viewFloorsListCurrentItemChanged(const QModelIndex &current,
                                          const QModelIndex &previous);
    void viewFloorsListItemChanged(QModelIndex index);
    void mouseDoubleClicked();
//    void mouseMiddleButtonClicked(QGraphicsItem *item);

    void panelVerticalsVisibilityChanged(bool visible);
    void verticalAdd();
    void verticalDelete();
    void verticalMoveDown();
    void verticalMoveUp();
    void verticalSetTypeCurrentIndexChanged(int index);
    void lstwgtVerticalsCurrentItemChanged(QListWidgetItem *current,
                                           QListWidgetItem *previous);
    void lstwgtVerticalsItemChanged(QListWidgetItem* item);

    void panelAreasPropertiesVisibilityChanged(bool visible);
    void cbxAreaTypeCurrentIndexChanged(int index);
    void setAreaNumber();
    void setAreaName();
    void setAreaDescription();
    void setAreaInscription();
    void updateAreaInscription();

    void panelSearchVisibilityChanged(bool visible);
    void ldtSearchTextChanged(const QString &text);
    void lstwgtSearchCurrentItemChanged(QListWidgetItem *current,
                                      QListWidgetItem *previous);

    void panelWaysVisibilityChanged(bool visible);
    void setStart();
    void setFinish();
    void lstwgtWaysCurrentItemChanged(QListWidgetItem *current,
                                      QListWidgetItem *previous);
    void way();
    void help();
    void about();                       // Show info about my fantastic program

    void updateStatusBar();

private:
    ///////////////////////////////Constants////////////////////////////////////
    static const qint32 cMagicNumber = 0xA246B56D;
    static const qreal cPixPerRealM = 10;
    static const qreal cZoom = 1.1;
    static const int cDockWidth = 160;
    static const bool isExtraShortcutsEnabled = false;
    static const QString cNewFileName;
    enum State
    {
        stSave = -1,
        stFalse = false,
        stTrue = true
    };
    static QMap<int, GraphArc::VerticalType> verticalType;

    QString openFileName;
    MapFloor::Mode mode;
    int curFloor;
    quint32 defaultFloor;

    // OUR ALL!!! Please, do not touch!
    Map *map;

    // our all ended. :)
    QStringListModel *modelFloorsList;
    QItemSelectionModel *selectionFloorsList;

    // Actions
    QAction *actMapNew;
    QAction *actMapOpen;
    QAction *actMapSave;
    QAction *actMapSaveAs;
    QAction *actMapExport;
    QAction *actMapProperties;
    QAction *actQuit;
    QAction *actAreaCopy;
    QAction *actGraphCopy;
    QAction *actFloorLower;
    QAction *actFloorDown;
    QAction *actFloorUp;
    QAction *actFloorUpper;
    QAction *actZoomOut;
    QAction *actZoomIn;
    QAction *actZoomFit;
    QAction *actHandDrag;
    QAction *actAddBase;
    QAction *actLayerBase;
    QAction *actLayerGrid;
    QAction *actLayerGraph;
    QAction *actSnapToExtensions;
    QAction *actAddArea;
    QAction *actAddDoor;
    QAction *actAddNode;
    QAction *actHelp;
    QAction *actAbout;

    QAction *actPanelFloors;
    QAction *actFloorAdd;
    QAction *actFloorDelete;
    QAction *actFloorMoveDown;
    QAction *actFloorMoveUp;
    QAction *actFloorSetDefault;

    QAction *actPanelVerticals;
    QAction *actVerticalAdd;
    QAction *actVerticalDelete;
    QAction *actVerticalMoveDown;
    QAction *actVerticalMoveUp;

    QAction *actPanelAreasProperties;

    QAction *actPanelSearch;

    QAction *actPanelWays;
    QAction *actSetStart;
    QAction *actSetFinish;

    QActionGroup *actgrpPanels;

    // Menus
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QMenu *menuLayers;
    QMenu *menuZoom;
    QMenu *menuGo;
    QMenu *menuAdd;
    QMenu *menuHelp;

    // Toolbars
    QToolBar *tbrFile;
//    QToolBar *tbrEdit;
    QToolBar *tbrView;
    QToolBar *tbrGo;
    QComboBox *cbxFloorSelect;
//    QToolBar *tbrLayers;
    QToolBar *tbrAdd;
    QToolBar *tbrQuickSearch;
    QLineEdit *ldtQuichSearch;
    QToolBar *tbrPanels;

    // Central
    QWidget *wgtCentral;
    QVBoxLayout *vblwgtCentral;

    // Dock FloorsManagement
    QDockWidget *dckwgtFloors;
    QWidget *wgtFloors;
    QVBoxLayout *vblFloors;
    QHBoxLayout *hblFloorsButtons;
    QToolButton *btnFloorAdd;
    QToolButton *btnFloorDelete;
    QToolButton *btnFloorMoveDown;
    QToolButton *btnFloorMoveUp;
    QToolButton *btnFloorSetDefault;
    QListView *viewFloorsList;

    // Dock Verticals
    QDockWidget *dckwgtVerticals;
    QWidget *wgtVerticals;
    QVBoxLayout *vblVerticals;
    QHBoxLayout *hblVerticalsButtons;
    QToolButton *btnVerticalAdd;
    QToolButton *btnVerticalDelete;
    QToolButton *btnVerticalMoveDown;
    QToolButton *btnVerticalMoveUp;
    QComboBox *cbxVerticalType;
    QListWidget *lstwgtVerticals;

    // Dock AreasProperties
    QDockWidget *dckwgtAreasProperties;
    QWidget *wgtAreasProperties;
    QVBoxLayout *vblAreasProperties;
    QLabel *lblAreaType;
    QComboBox *cbxAreaType;
    QLabel *lblAreaNumber;
    QLineEdit *ldtAreaNumber;
    QLabel *lblAreaName;
    QLineEdit *ldtAreaName;
    QLabel *lblAreaDescription;
    QPlainTextEdit *ptdtAreaDescription;
    QPushButton *btnUpdateAreaInscription;
    QLabel *lblAreaInscription;
    QPlainTextEdit *ptdtAreaInscription;

    // Dock Search
    QDockWidget *dckwgtSearch;
    QWidget *wgtSearch;
    QVBoxLayout *vblSearch;
    QLabel *lblAreasNumber;
    QLineEdit *ldtSearch;
    QListWidget *lstwgtAreas;

    // Dock Way
    QDockWidget *dckwgtWay;
    QWidget *wgtWay;
    QVBoxLayout *vblWay;
    QHBoxLayout *hblWayButtons;
    QToolButton *btnSetStart;
    QToolButton *btnSetFinish;
    QGroupBox *grpbxWay;
    QVBoxLayout *vblgrpbxWay;
    QLabel *lblWaysChoice;
    QListWidget *lstwgtWays;
    QLabel *lblWayInfo;

    // Graphics
    QGraphicsView *view;
    QGraphicsPixmapItem *pImage;

    QList<QKeySequence> shortcuts(QKeySequence main, QKeySequence extra);
    void createActions();
    void createMenus();
    void createToolBars();
    void createPanelFloors();
    void createPanelVerticals();
    void createPanelAreasProperties();
    void createPanelSearch();
    void createPanelWays();
    void blockAndFreePanelAreasProperties();
    void freePanelWays();
    void createGraphics();
    void setState(Elements elem, State visible, State enable);
    qreal displayPixPerM(qreal pix, qreal mm) const;
    void swapFloors(int x, int y);
    void swapVerticals(int x, int y);
    MapFloor::Mode getMode();
    void mapSave(QString &fileName);
    MapArea* selectedArea();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MainWindow::Elements)

#endif // MAINWINDOW_H
