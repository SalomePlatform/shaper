
#ifndef XGUI_WORKSHOP_H
#define XGUI_WORKSHOP_H

#include "XGUI_Interfaces.h"

#include <QObject>
#include <QMap>
#include <QIcon>
#include <QKeySequence>

class XGUI_MainWindow;
class XGUI_Command;
class XGUI_Module;

class XGUI_Workshop: public QObject, public IWorkshop
{
	Q_OBJECT
public:

	XGUI_Workshop();
	virtual ~XGUI_Workshop();

    void startApplication();

    XGUI_MainWindow* mainWindow() const { return myMainWindow; }

    virtual IWorkbench* addWorkbench(const QString& theName);

public slots:
    void onNew();
    void onOpen();
    void onSave();
    void onSaveAs();
    void onExit();

private:
    void initMenu();

    IModule* loadModule(const QString& theModule);
    bool activateModule();

    XGUI_MainWindow* myMainWindow;
};

#endif