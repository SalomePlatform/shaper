// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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
  /// \param theWorkshop the current workshop
  XGUI_EXPORT XGUI_MenuMgr(XGUI_Workshop* theWorkshop);
  XGUI_EXPORT virtual ~XGUI_MenuMgr() {}

  /// Creates feature actions
  XGUI_EXPORT void createFeatureActions();

  /// Redefinition of Events_Listener method
  /// \param theMessage a message
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
  XGUI_Workshop* myWorkshop; ///< the current workshop

  /// container of existing workbenchs
  std::list< std::shared_ptr<XGUI_MenuWorkbench> > myWorkbenches;
};

#endif /* XGUI_MENUMGR_H_ */
