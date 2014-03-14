#ifndef XGUI_MAINWINDOW_H
#define XGUI_MAINWINDOW_H

#include <QMainWindow>

class XGUI_MainMenu;
class QTreeWidget;

class XGUI_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    XGUI_MainWindow(QWidget* parent = 0);
    virtual ~XGUI_MainWindow();

    XGUI_MainMenu* menuObject() const { return myMenuBar; }
   
    QTreeWidget* objectBrowser() const { return myObjectBrowser; }
    void showObjectBrowser();
    void hideObjectBrowser();

private:
    //!! For test purposes only
    //QWidget* getSubWindow();

    void fillObjectBrowser();
    void addPropertyPanel();


    XGUI_MainMenu* myMenuBar;
    QTreeWidget* myObjectBrowser;
};

#endif