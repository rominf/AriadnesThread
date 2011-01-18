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
#include <QListWidget>
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
#include "dialogfloorchoice.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
//friend QDataStream & operator<<(QDataStream & output, const Map &map);
public:
    enum Program
    {
        pFull, pViewer, pViewerTerminal
    };
    MainWindow(MainWindow::Program program = MainWindow::pFull,
               QWidget *parent = 0);
    ~MainWindow();
    enum Element
    {
        eFile = 1,
        eEdit = 2,
        eView = 4,
        eLayers = 8,
        eAdd  = 16,
        eHelp = 32,
        eFloorsSwitching = 64,
        ePanels = 128,
        eFloorsManagement = 256,
        eVerticals = 512,
        eAreasMarking =1024,
        eWay = 2048
    };
    Q_DECLARE_FLAGS(Elements, Element)

//    const QStringListModel* modelFloorsList() const;

protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void mapNew();                      // Creating map
    void mapOpen();                     // Opening map
    void mapSave();                     // Saving map to open file
    void mapSaveAs();                   // Saving map to another file
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
    void handDrag(bool checked);
    void addBase();
    void layerBaseSetVisible(bool visible);
    void layerCrossBaseSetVisible(bool visible);
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
    void floorSetDefault();
    void viewFloorsListItemActivated(QModelIndex index);
    void viewFloorsListItemChanged(QModelIndex index);
    void mouseDoubleClicked();
    void mouseMiddleButtonClicked(QGraphicsItem *item);

    void panelVerticalsVisibilityChanged(bool visible);
    void verticalAdd();
    void verticalDelete();
    void verticalSetType(int type);
    void lstwgtVerticalsItemActivated(QListWidgetItem* item);
//    void wgtVerticalsListCurrentRowChanged(int row);
    void lstwgtVerticalsItemChanged(QListWidgetItem* item);

    void panelAreasMarkingVisibilityChanged(bool visible);
    void setAreaNumber();
    void setAreaName();
    void setAreaDescription();
    void setAreaInscription();
    void updateAreaInscription();

    void panelWayVisibilityChanged(bool visible);
    void setStart();
    void setFinish();
    void lstwgtWaysItemActivated(QListWidgetItem* item);
    void way();
//    void chkAreaNumberVisibleStateChanged(int state);
//    void chkAreaNameVisibleStateChanged(int state);
    void about();                       // Show info about my fantastic program

private:
    ///////////////////////////////Constants////////////////////////////////////
    static const qint32 cMagicNumber = 0xA246B56D;
    static const qreal cPixPerRealM = 10;
    static const qreal cZoom = 1.1;
    static const int cDockWidth = 150;
    static const bool isExtraShortcutsEnabled = false;
    enum State
    {
        stSave = -1,
        stFalse = false,
        stTrue = true
    };

    ///////////////////////////////Variables////////////////////////////////////
    MainWindow::Program m_program;
    QString openFileName;
    MapFloor::Mode mode;
    int curFloor;
    quint32 defaultFloor;
    bool autoAreasRenaming;
//    bool isFirstAreasMarking;

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
    QAction *actMapSaveAs;
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
    QAction *actLayerCrossBase;
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
    QAction *actFloorSetDefault;

    QAction *actPanelVerticals;
    QAction *actVerticalAdd;
    QAction *actVerticalDelete;
//    QAction *actVerticalSetPassage;

    QAction *actPanelAreasMarking;

    QAction *actPanelWay;
    QAction *actSetStart;
    QAction *actSetFinish;
//    QAction *actAllowStairs;
//    QAction *actAllowLifts;
//    QAction *actWay;

    QActionGroup *actgrpPanels;
    // QAction *actAboutQT;


    // Menus
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuPanels;
    QMenu *menuAdd;
    QMenu *menuHelp;

    // Toolbars
    QToolBar *tbrFile;
    QToolBar *tbrEdit;
    QToolBar *tbrFloorsSwitching;
    QComboBox *cbxFloorSelect;
    QToolBar *tbrView;
    QToolBar *tbrLayers;
    QToolBar *tbrAdd;
    QToolBar *tbrPanels;
    /*QToolBar *tbrFloorNameChange;
    QLabel *lblFloorNameChange;
    QLineEdit *ldtFloorNameChange;*/

    // Central
    QWidget *wgtCentral;
    QVBoxLayout *vblwgtCentral;

    // Dock FloorsManagement
    QDockWidget *dckwgtFloorsManagement;
    QWidget *wgtFloorsManagement;
    QVBoxLayout *vblFloorsManagement;
    QHBoxLayout *hblFloorsManagementButtons;
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
//    QToolButton *btnVerticalSetPassage;
    QComboBox *cbxVerticalType;
    QListWidget *lstwgtVerticals;
//    QListView *viewFloorsList;
//    QToolButton *btnFloorMoveDown;
//    QToolButton *btnFloorMoveUp;
//    QToolButton *btnFloorSetDefault;


    // Dock AreasMarking
    QDockWidget *dckwgtAreasMarking;
    QWidget *wgtAreasMarking;
    QVBoxLayout *vblAreasMarking;
    QLabel *lblAreaNumber;
    QLineEdit *ldtAreaNumber;
    QLabel *lblAreaName;
    QLineEdit *ldtAreaName;
    QLabel *lblAreaDescription;
    QPlainTextEdit *ptdtAreaDescription;
    QPushButton *btnUpdateAreaInscription;
    QLabel *lblAreaInscription;
    QPlainTextEdit *ptdtAreaInscription;
//    QGroupBox *grpbxFieldVisible;
//    QVBoxLayout *vblFieldVisible;
//    QCheckBox *chkAreaNumberVisible;
//    QCheckBox *chkAreaNameVisible;

    // Dock Way
    QDockWidget *dckwgtWay;
    QWidget *wgtWay;
    QVBoxLayout *vblWay;
    QHBoxLayout *hblWayButtons;
    QToolButton *btnSetStart;
    QToolButton *btnSetFinish;
    QListWidget *lstwgtWays;
//    QToolButton *btnAllowStairs;
//    QToolButton *btnAllowLifts;
//    QToolButton *btnWay;
    QLabel *lblWayInfo;

    // Graphics
    QGraphicsView *view;
    QGraphicsPixmapItem *pImage;

    ///////////////////////////////Functions////////////////////////////////////
    QList<QKeySequence> shortcuts(QKeySequence main, QKeySequence extra);
    void createActions();
    void createMenus();
    void createToolBars();
    void createPanelFloorsManagement();
    void createPanelVerticals();
    void createPanelAreasMarking();
    void createPanelWay();
    void blockAndFreePanelAreasMarking();
    void createGraphics();
    void setState(Elements elem, State visible, State enable);
    qreal displayPixPerM(qreal pix, qreal mm) const;
    void swapFloors(int x, int y);
    MapFloor::Mode getMode();
    void mapSave(QString &fileName);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MainWindow::Elements)

#endif // MAINWINDOW_H
