/*
 * ActionInfo.h
 *
 *  Created on: Feb 4, 2015
 *      Author: sbh
 */

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
  QIcon icon; //!< action's icon
  QString text; //!< action's text
  QString iconText; //!< action's descriptive icon text
  QString iconFile; //!< path to icon's file. Can not be initialized from QAction
  QString toolTip; //!< action's tooltip
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
  //! Creates new QAction with given parent and data initialized from this structure
  //! \param theParent - parent of created action
  QAction* makeAction(QObject* theParent = 0);

 protected:
  //! Initializes structure with default values, like QAction()
  void initDefault();
};

typedef ModuleBase_ActionInfo ActionInfo;

#endif /* XGUI_ACTIONINFO_H_ */
