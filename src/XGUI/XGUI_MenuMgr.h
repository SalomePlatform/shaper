// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_MenuMgr.hxx
// Created:     13 Apr 2016
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_MENUMGR_H_
#define XGUI_MENUMGR_H_

#include "XGUI.h"

#include <Events_Listener.h>

#include <string>
#include <list>
#include <memory>

class XGUI_MenuWorkbench;
class XGUI_Workshop;
class Config_FeatureMessage;

class QAction;

/**
* \ingroup GUI
* A class for management of menu actions (features). The actions should be arranged like they are
* in XML file. It listens the read feature of XML and fills internal structure of menu workbenches
* and groups of feature. After, it creates menues and tools in the module.
*/
class XGUI_MenuMgr : public Events_Listener
{
 public:
  /// Constructor
  /// \param the current workshop
  XGUI_EXPORT XGUI_MenuMgr(XGUI_Workshop* theWorkshop);
  XGUI_EXPORT virtual ~XGUI_MenuMgr() {}

  XGUI_EXPORT void createFeatureActions();

  /// Redefinition of Events_Listener method
  XGUI_EXPORT virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

protected:
  /// Process event "Add a feature"
  void addFeature(const std::shared_ptr<Config_FeatureMessage>& theMessage);

  /// Finds or creates a workbench for the given name
  /// \param theWorkbenchName a name defined in XML
  /// \return an instance of workbench
  std::shared_ptr<XGUI_MenuWorkbench> findWorkbench(const std::string& theWorkbenchName);

  /// Creates a new action by the message
  /// \param theMessage an XML information of the feature
  /// \param theWchName a workbench name, will be used as menu/tool bar name
  /// \param aUseSeparator boolean value if a separator should be added after the action
  /// \return an action
  QAction* buildAction(const std::shared_ptr<Config_FeatureMessage>& theMessage,
                       const std::string& theWchName, const bool aUseSeparator) const;

private:
  XGUI_Workshop* myWorkshop; /// the current workshop
  std::list< std::shared_ptr<XGUI_MenuWorkbench> > myWorkbenches; /// container of existing workbenchs
};

#endif /* XGUI_MENUMGR_H_ */
