#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QActionGroup>
#include <QActionGroup>
#include <QApplication>
#include <QComboBox>
#include <QCheckBox>
#include <QCursor>
#include <QDockWidget>
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
#include <QMainWindow>
#include <QMenu>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QPushButton>
#include <QRectF>
#include <QItemSelectionModel>
#include <QScrollBar>
#include <QShortcut>
#include <QSignalMapper>
#include <QStackedWidget>
#include <QStatusBar>
#include <QStringListModel>
#include <QTime>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include "../Common/map.h"
#include "../Common/mapfloor.h"
#include "dialogmapsize.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
//friend QDataStream & operator<<(QDataStream & output, const Map &map);
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    enum Element
    {
        eFile = 1,
        eView = 2,
        eLayers = 4,
        eAdd  = 8,
        eHelp = 16,
        eFloorsSwitching = 32,
        ePanels = 64,
        eFloorsManagement = 128,
        eAreasMarking = 256
    };
    Q_DECLARE_FLAGS(Elements, Element)

private slots:
    void mapNew();                      // Creating map
    void mapOpen();                     // Opening map
    void mapSave();                     // Saving map
    void floorDown();                   // Set visible lower floor
    void floorUp();                     // Set visible higher floor

    void zoomOut();
    void zoomIn();
    void zoomFit();
    void addBase();
    void layerBaseSetVisible(bool visible);
    void layerGraphSetVisible(bool visible);
    void magnetToExtensions(bool b);
//    void floorNameChange(const QString &); // Set current floor name
    void addWall();                     // Adding wall
    void addArea();                     // Adding area
    void addDoor();                     // Adding door
    void addNode();                     // Adding graph node
    void actgrpPanelsTriggered(QAction *);    // Link between actgrpMode & switchMode
    void switchMode(MapFloor::Mode m);  // Apply app to proper mode
    void setActiveFloor(int i);         // Change visible floor
    void panelFloorsManagementVisibilityChanged(bool visible);
    void floorAdd();                    // Adding floor
    void floorDelete();
    void floorMoveDown();
    void floorMoveUp();
    void viewFloorsListItemActivated(QModelIndex index);
    void viewFloorsListItemChanged(QModelIndex index);
    void mouseDoubleClicked();
    void panelAreasMarkingVisibilityChanged(bool visible);
    void setAreaName();
    void chkAreaNameVisibleStateChanged(int state);
    void about();                       // Show info about my fantastic program

private:
    ///////////////////////////////Constants////////////////////////////////////
    static const qint32 cMagicNumber = 0xA246B56D;
    static const qreal cPixPerRealM = 10;
    static const qreal cZoom = 1.1;
    static const int cDockWidth = 250;
    enum State
    {
        stSave = -1,
        stFalse = false,
        stTrue = true
    };

    ///////////////////////////////Variables////////////////////////////////////
    QString openedFile;
    MapFloor::Mode mode;
    int curFloor;

    ///////////////////////////////Enums////////////////////////////////////////


    ///////////////////////////////Objects//////////////////////////////////////
    // OUR ALL!!! Please, do not touch!
    Map *map;
    QStringListModel *modelFloorsList;
    QItemSelectionModel *selectionFloorsList;
    // our all ended. :)

    // Actions
    QAction *actMapNew;
    QAction *actMapOpen;
    QAction *actMapSave;
    QAction *actFloorDown;
    QAction *actFloorUp;
    QAction *actZoomOut;
    QAction *actZoomIn;
    QAction *actZoomFit;
    QAction *actAddBase;
    QAction *actLayerBase;
    QAction *actLayerGraph;
    QAction *actMagnetToExtensions;
    QAction *actAddWall;
    QAction *actAddArea;
    QAction *actAddDoor;
    QAction *actAddNode;
    QAction *actAbout;

    QAction *actPanelFloorsManagement;
    QAction *actFloorAdd;
    QAction *actFloorDelete;
    QAction *actFloorMoveDown;
    QAction *actFloorMoveUp;
    QAction *actPanelAreasMarking;
    QActionGroup *actgrpPanels;
    // QAction *actAboutQT;


    // Menus
    QMenu *menuFile;
    QMenu *menuPanels;
    QMenu *menuAdd;
    QMenu *menuHelp;

    // Toolbars
    QToolBar *tbrFile;
    QToolBar *tbrPanels;
    QToolBar *tbrFloorsSwitching;
    QComboBox *cbxFloorSelect;
    QToolBar *tbrView;
    QToolBar *tbrLayers;
    QToolBar *tbrAdd;
    /*QToolBar *tbrFloorNameChange;
    QLabel *lblFloorNameChange;
    QLineEdit *ldtFloorNameChange;*/

    // Central
    QWidget *wgtCentral;
    QVBoxLayout *vblwgtCentral;

    QSignalMapper *panelsSignalMapper;

    // Dock FloorsManagement
    QDockWidget *dckwgtFloorsManagement;
    QWidget *wgtFloorsManagement;
    QVBoxLayout *vblFloorsManagement;
    QHBoxLayout *hblFloorsManagementButtons;
    QToolButton *btnFloorAdd;
    QToolButton *btnFloorDelete;
    QToolButton *btnFloorMoveDown;
    QToolButton *btnFloorMoveUp;
    QListView *viewFloorsList;

    // Dock AreasMarking
    QDockWidget *dckwgtAreasMarking;
    QWidget *wgtAreasMarking;
    QVBoxLayout *vblAreasMarking;
    QLabel *lblAreaName;
    QPlainTextEdit *ptdtAreaName;
    QCheckBox *chkAreaNameVisible;
    QLabel *lblAreaDescription;
    QPlainTextEdit *ptdtAreaDescription;

    // Graphics
    QGraphicsView *view;
    QGraphicsPixmapItem *pImage;

    ///////////////////////////////Functions////////////////////////////////////
    void createActions();
    void createMenus();
    void createToolBars();
    void createPanelFloorsManagement();
    void createPanelAreasMarking();
    void createGraphics();
    void setState(Elements elem, State visible, State enable);
    qreal displayPixPerM(qreal pix, qreal mm) const;
    void swapFloors(int x, int y);
    MapFloor::Mode getMode();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MainWindow::Elements)

#endif // MAINWINDOW_H
