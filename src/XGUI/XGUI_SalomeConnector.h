// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
  //! \param theTBName - a toolbar name
  //! \param theId - an id of the feature
  //! \param theTitle - a menu item string
  //! \param theTip - a tooltip string (help)
  //! \param theIcon - icon
  //! \param isCheckable - is checkable or not
  //! \param theKeys - hot keys
  //! \param isAddSeparator boolean flag about adding separator after the action
  //! returns created action
  virtual QAction* addFeature(const QString& theWBName, const QString& theTBName,
                              const QString& theId,
                              const QString& theTitle, const QString& theTip,
                              const QIcon& theIcon,
                              const QKeySequence& theKeys, bool isCheckable,
                              const bool isAddSeparator,
                              const QString& theStatusTip) = 0;

  //! Creates a feature (command) in SALOME desktop
  //! \param theWBName - name of toolbar (workbench)
  //! \param theInfo - information about action (icon, text, etc)
  //! \param isAddSeparator boolean flag about adding separator after the action
  virtual QAction* addFeature(const QString& theWBName,
                              const ActionInfo& theInfo, const bool isAddSeparator) = 0;

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
  //! \param theSubMenu - a sub-menu name
  //! \param theMenuPosition - a position of the command in the desktop menu
  //! \param theSuibMenuPosition - a position of the sub-menu in the desktop menu
  //! returns created action
  virtual QAction* addDesktopCommand(const QString& theId, const QString& theTitle,
                                     const QString& theTip, const QIcon& theIcon,
                                     const QKeySequence& theKeys, bool isCheckable,
                                     const char* theMenuSourceText,
                                     const QString& theSubMenu = QString(),
                                     const int theMenuPosition = 10,
                                     const int theSuibMenuPosition = -1) = 0;

  //! Insert separator into Edit menu of SALOME desktop
  virtual void addDesktopMenuSeparator(const char* theMenuSourceText,
                                       const int theMenuPosition = 10) = 0;

  /// Add an action into tool bar
  /// \param theAction the Action object
  /// \param theToolBarTitle the toolbar name
  virtual bool addActionInToolbar( QAction* theAction, const QString& theToolBarTitle ) = 0;

  /// Creates menu/tool bar actions for loaded features stored in the menu manager
  virtual void createFeatureActions() = 0;

  //! Returns desktop window of SALOME
  virtual QMainWindow* desktop() const = 0;

  //! Returns list of defined actions (just by SHAPER module)
  virtual QList<QAction*> commandList() const = 0;

  //! Stores XML information for the feature kind
  //! \param theFeatureId a feature kind
  //! \param theMessage a container of the feature XML properties
  virtual void setFeatureInfo(const QString& theFeatureId,
                              const std::shared_ptr<Config_FeatureMessage>& theMessage) = 0;

  //! Returns XML information for the feature kind
  //! \param theFeatureId a feature kind
  //! \return theMessage a container of the feature XML properties
  virtual std::shared_ptr<Config_FeatureMessage> featureInfo(const QString& theFeatureId) = 0;

  //! Returns interface to Salome viewer
  virtual ModuleBase_IViewer* viewer() const = 0;

  //! Create preferences
  virtual void createPreferences() = 0;

  //! Shows the given text in status bar as a permanent text
  //! \theInfo a string value
  //! \theMsecs interval of msec milliseconds when the message will be hidden, if -1, it stays.
  //            If 0, default value is used, it is 3000
  virtual void putInfo(const QString& theInfo, const int theMSecs = 0) = 0;
};

#endif
