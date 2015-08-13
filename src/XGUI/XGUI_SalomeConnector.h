// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef XGUI_SALOMECONNECTOR_H
#define XGUI_SALOMECONNECTOR_H

#include "XGUI.h"

#include <QString>
#include <QStringList>

#include <ModuleBase_ActionInfo.h>

class QMainWindow;
class ModuleBase_IViewer;

/**
 * \ingroup GUI
 * An interface which provides a connection of XGUI functionality 
 * with functionality of SALOME module interface.
 */
class XGUI_EXPORT XGUI_SalomeConnector
{
 public:
  /// Default constructor
  XGUI_SalomeConnector();
  /// Virtual destructor
  virtual ~XGUI_SalomeConnector();

  //! Creates a feature (command) in SALOME desktop
  //! \param theWBName - a workbench name
  //! \param theId - an id of the feature
  //! \param theTitle - a menu item string
  //! \param theTip - a tooltip string (help)
  //! \param theIcon - icon
  //! \param isCheckable - is checkable or not
  //! \param theKeys - hot keys
  //! returns created action
  virtual QAction* addFeature(const QString& theWBName, const QString& theId,
                              const QString& theTitle, const QString& theTip, const QIcon& theIcon,
                              const QKeySequence& theKeys, bool isCheckable) = 0;

  //! Creates a feature (command) in SALOME desktop
  //! \param theWBName - name of toolbar (workbench)
  //! \param theInfo - information about action (icon, text, etc)
  virtual QAction* addFeature(const QString& theWBName,
                              const ActionInfo& theInfo) = 0;

  //! Creates a feature (command) in SALOME desktop
  //! \param theWBName - name of toolbar (workbench)
  //! \param theInfo - information about action (icon, text, etc)
  virtual QAction* addNestedFeature(const QString& theWBName,
                                    const ActionInfo& theInfo,
                                    const QList<QAction*>& theNestedActions) = 0;

  //! Creates a command in Edit menu of SALOME desktop
  //! \param theId - an id of the feature
  //! \param theTitle - a menu item string
  //! \param theTip - a tooltip string (help)
  //! \param theIcon - icon
  //! \param isCheckable - is checkable or not
  //! \param theKeys - hot keys
  //! \param theMenuSourceText - a source text of the desktop menu
  //! \param theMenuPosition - a position of the command in the desktop menu
  //! returns created action
  virtual QAction* addDesktopCommand(const QString& theId, const QString& theTitle,
                                     const QString& theTip, const QIcon& theIcon,
                                     const QKeySequence& theKeys, bool isCheckable,
                                     const char* theMenuSourceText,
                                     const int theMenuPosition = 10) = 0;

  //! Insert separator into Edit menu of SALOME desktop
  virtual void addDesktopMenuSeparator(const char* theMenuSourceText,
                                       const int theMenuPosition = 10) = 0;

  virtual bool addActionInToolbar( QAction* theAction, const QString& theToolBarTitle ) = 0;

  //! Returns desktop window of SALOME
  virtual QMainWindow* desktop() const = 0;

  //! Returns command string Id by QAction instance
  virtual QString commandId(const QAction* theCmd) const = 0;

  //! Returns QAction instance by command string Id
  virtual QAction* command(const QString& theId) const = 0;

  //! Returns list of defined actions (just by NewGeom module)
  virtual QList<QAction*> commandList() const = 0;

  //! Returns list of Ids of defined actions (just by NewGeom module)
  virtual QStringList commandIdList() const = 0;

  //! Set nested actions dependent on command Id
  //! \param theId - the command ID
  //! \param theActions - the list of nested actions
  virtual void setNestedActions(const QString& theId, const QStringList& theActions) = 0;

  //! Returns list of nested actions according to the given command ID
  virtual QStringList nestedActions(const QString& theId) const = 0;

  //! Set the document kind of the action by the given command Id
  //! \param theId - the command ID
  //! \param theKind - the document kind
  virtual void setDocumentKind(const QString& theId, const QString& theKind) = 0;

  //! Returns the document kind of the action by the given command ID
  virtual QString documentKind(const QString& theId) const = 0;

  //! Returns interface to Salome viewer
  virtual ModuleBase_IViewer* viewer() const = 0;

  //! Create preferences
  virtual void createPreferences() = 0;
};

#endif
