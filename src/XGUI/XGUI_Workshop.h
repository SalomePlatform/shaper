
#ifndef XGUI_WORKSHOP_H
#define XGUI_WORKSHOP_H

#include <QObject>
#include <QMap>
#include <QIcon>
#include <QKeySequence>

class XGUI_MainWindow;
class XGUI_Command;
class XGUI_Module;

class XGUI_Workshop: public QObject
{
	Q_OBJECT
public:
    enum XCommandId {
        NEW_CMD,
        OPEN_CMD,
        SAVE_CMD,
        SAVEAS_CMD,
        EXIT_CMD,
        UNDO_CMD,
        REDO_CMD,
        COPY_CMD,
        CUT_CMD,
        PASTE_CMD,
        LAST_CMD
    };

	XGUI_Workshop();
	virtual ~XGUI_Workshop();

    void startApplication();

    XGUI_Command* command(XCommandId theId) const;

    XGUI_MainWindow* mainWindow() const { return myMainWindow; }

public slots:
    void onNew();
    void onOpen();
    void onSave();
    void onSaveAs();
    void onExit();

private:
    void initMenu();

    XGUI_Module* loadModule(const QString& theModule);
    void loadModules();

    void buildModuleMenu(const QString& theXML);

    int addWorkbench(const QString& theName);
    int addPageGroup(int thePageId);
    void addCommand(XCommandId theCommandId, int thePageId, int theGroupId, XGUI_Command* theCommand);
    XGUI_Command* createMenuCommand(int thePageId, int theGroupId, XCommandId theCmdId, 
                                    const QString& theTitle, const QString& theTip, 
                                    const QIcon& theIcon = QIcon(), const QKeySequence& theKeys = QKeySequence());

    QMap<int, XGUI_Command*> myCommands;

    XGUI_MainWindow* myMainWindow;
};

#endif