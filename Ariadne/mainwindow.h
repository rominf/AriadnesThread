#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QActionGroup>
#include <QActionGroup>
#include <QApplication>
#include <QComboBox>
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
#include <QMainWindow>
#include <QMenu>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QPushButton>
#include <QRectF>
#include <QShortcut>
#include <QStackedWidget>
#include <QStatusBar>
#include <QToolBar>
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
        eMode = 2,
        eView = 4,
        eFloorNameChange = 8,
        eAdd  = 16,
        eDock = 32,
        eHelp = 64
    };
    enum State {stEnable_Visible, stDisable_Visible, stDisable_Unvisible};
    Q_DECLARE_FLAGS(Elements, Element)

/*protected:
    virtual void keyPressEvent(QKeyEvent *event);*/

private slots:
    void mapNew();                      // Creating map
    void mapOpen();                     // Opening map
    void mapSave();                     // Saving map
    void previousFloor();               // Set visible previous floor
    void nextFloor();                   // Set visible next floor
    void zoomOut();
    void zoomIn();
    void zoomFit();
    void addBase();
    void floorNameChange(const QString &); // Set current floor name
    void addFloor();                    // Adding floor, drawing outline for it
    void addWall();                     // Adding wall
    void addArea();                     // Adding area
    void addDoor();                     // Adding door
    void actgrpTriggered(QAction *);    // Link between actgrpMode & switchMode
    void switchMode(MapFloor::Mode m);  // Apply app to proper mode
    void setActiveFloor(int i);         // Change visible floor
    void about();                       // Show info about my fantastic program

private:
    ///////////////////////////////Constants////////////////////////////////////
    static const qint32 cMagicNumber = 0xA246B56D;
    static const qreal cPixPerRealM = 10;
    static const qreal cZoom = 1.1;

    ///////////////////////////////Variables////////////////////////////////////
    QString openedFile;
    MapFloor::Mode mode;
    int curFloor;

    ///////////////////////////////Enums////////////////////////////////////////


    ///////////////////////////////Objects//////////////////////////////////////
    // OUR ALL!!! Please, do not touch!
    Map *map;
    // our all ended. :)

    // Actions
    QAction *actMapNew;
    QAction *actMapOpen;
    QAction *actMapSave;
    QAction *actSwitchToPlanningMode;
    QAction *actSwitchToMarkingMode;
    QAction *actPreviousFloor;
    QAction *actNextFloor;
    QAction *actZoomOut;
    QAction *actZoomIn;
    QAction *actZoomFit;
    QAction *actAddBase;
    QAction *actAddFloor;
    QAction *actAddWall;
    QAction *actAddArea;
    QAction *actAddDoor;
    QAction *actAbout;
    // QAction *actAboutQT;
    QActionGroup *actgrpMode;

    // Menus
    QMenu *menuFile;
    QMenu *menuMode;
    QMenu *menuAdd;
    QMenu *menuHelp;

    // Toolbars
    QToolBar *tbrFile;
    QToolBar *tbrMode;
    QToolBar *tbrView;
    QComboBox *cbxFloorSelect;
    QToolBar *tbrAdd;
    /*QToolBar *tbrFloorNameChange;
    QLabel *lblFloorNameChange;
    QLineEdit *ldtFloorNameChange;*/

    // Central
    QWidget *wgtCentral;
    QVBoxLayout *vblwgtCentral;

    // Dock Info
    QDockWidget *dckwgtInfoEdit;
    QWidget *wgtInfoEdit;
    QVBoxLayout *vbldckwgtInfoEdit;
    QLabel *lblRoomName;
    QLineEdit *ldtRoomName;
    QLabel *lblRoomDescription;
    QPlainTextEdit *ptdtRoomDescription;

    // Graphics
    QGraphicsView *view;
    QGraphicsPixmapItem *pImage;

    ///////////////////////////////Functions////////////////////////////////////
    void createActions();
    void createMenus();
    void createToolBars();
    void createDock();
    void createGraphics();
    void setState(Elements elem, State st);
    qreal displayPixPerM(qreal pix, qreal mm) const;
    MapFloor::Mode getMode();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MainWindow::Elements)

#endif // MAINWINDOW_H
