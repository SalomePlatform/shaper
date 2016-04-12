// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef XGUI_SALOMECONNECTOR_H
#define XGUI_SALOMECONNECTOR_H

#include "XGUI.h"

#include <QString>
#include <QStringList>

#include <ModuleBase_ActionInfo.h>

class QMainWindow;
class Config_FeatureMessage;
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
  //! \param theNestedActions a list of nested actions
  virtual QAction* addFeatureOfNested(const QString& theWBName,
                                    const ActionInfo& theInfo,
                                    const QList<QAction*>& theNestedActions) = 0;

  //! Returns true if the feature action is a nested action, in other words,
  //! it is created by addFeatureOfNested().
  //! \param theAction - an action of a feature
  //! returns boolean result
  virtual bool isFeatureOfNested(const QAction* theAction) = 0;

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

  /// Add an action into tool bar
  /// \param theAction the Action object
  /// \param theToolBarTitle the toolbar name
  virtual bool addActionInToolbar( QAction* theAction, const QString& theToolBarTitle ) = 0;

  //! Returns desktop window of SALOME
  virtual QMainWindow* desktop() const = 0;

  //! Returns command string Id by QAction instance
  virtual QString commandId(const QAction* theCmd) const = 0;

  //! Returns QAction instance by command string Id
  virtual QAction* command(const QString& theId) const = 0;

  //! Returns list of defined actions (just by SHAPER module)
  virtual QList<QAction*> commandList() const = 0;

  //! Returns list of Ids of defined actions (just by NewGeom module)
  virtual QStringList commandIdList() const = 0;

  //! Stores XML information for the feature kind
  //! \param theFeatureId a feature kind
  //! \param theMessage a container of the feature XML properties
  virtual void setFeatureInfo(const QString& theFeatureId,
                              const std::shared_ptr<Config_FeatureMessage>& theMessage) = 0;

  //! Returns XML information for the feature kind
  //! \param theFeatureId a feature kind
  //! \return theMessage a container of the feature XML properties
  virtual const std::shared_ptr<Config_FeatureMessage>& featureInfo(const QString& theFeatureId) = 0;

  //! Returns interface to Salome viewer
  virtual ModuleBase_IViewer* viewer() const = 0;

  //! Create preferences
  virtual void createPreferences() = 0;
};

#endif
