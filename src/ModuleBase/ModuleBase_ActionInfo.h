// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef MODULEBASE_ACTIONINFO_H_
#define MODULEBASE_ACTIONINFO_H_

#include <ModuleBase.h>
#include <Config_FeatureMessage.h>

#include <QAction>
#include <QIcon>
#include <QKeySequence>
#include <QFont>

#include <memory>

/*!
 * Structure to pass info about QActions, AppElements_Commands, etc.
 */
struct MODULEBASE_EXPORT ModuleBase_ActionInfo
{
  QString id; //!< action's (command) id
  bool checkable; //!< action's checkable state
  bool checked; //!< action's checked state
  bool enabled; //!< action's enabled state
  bool visible; //!< action's visibility state

  //! = true if the acton is a modal dialog box else property panel (==false by default)
  bool modal;
  QIcon icon; //!< action's icon
  QString text; //!< action's text
  QString iconText; //!< action's descriptive icon text
  QString iconFile; //!< path to icon's file. Can not be initialized from QAction
  QString toolTip; //!< action's tooltip
  QString toolBar;  //!< A name of toolbar. By default = group of features name
  // QString statusTip;
  // QString whatsThis;
  QKeySequence shortcut; //!< action's primary shortcut key
  QFont font; //!< action's text font

 public:
  //! Default constructor, \sa initDefault
  ModuleBase_ActionInfo();
  //! Initializes structure with default values, except text
  ModuleBase_ActionInfo(const QString &text);
  //! Initializes structure with default values, except icon and text
  ModuleBase_ActionInfo(const QIcon &icon, const QString &text);
  virtual ~ModuleBase_ActionInfo();

  //! Fills itself with info from given \param theAction
  void initFrom(QAction* theAction);
  //! Fills itself with info from given \param theFeatureMessage
  void initFrom(std::shared_ptr<Config_FeatureMessage> theFeatureMessage);

 protected:
  //! Initializes structure with default values, like QAction()
  void initDefault();
};

typedef ModuleBase_ActionInfo ActionInfo;

#endif /* XGUI_ACTIONINFO_H_ */
