// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef XGUI_MENUGROUP_H_
#define XGUI_MENUGROUP_H_

#include "XGUI.h"

#include <string>
#include <list>
#include <memory>

#ifdef WIN32
#pragma warning(disable : 4251) // std::string needs dll-interface
#endif

class Config_FeatureMessage;

/**
* \ingroup GUI
* A class for management of menu actions (features). The actions should be arranged like they are
* in XML file. It listens the read feature of XML and fills internal structure of menu workbenches
* and groups of feature. After, it creates menues and tools in the module.
*/
class XGUI_EXPORT XGUI_MenuGroup
{
 public:
  /// Constructor
  XGUI_MenuGroup(const std::string& theName);
  virtual ~XGUI_MenuGroup() {}

  /// Returns a name of the workbench
  /// \return workbench name
  std::string getName() const { return myName; }

  /// Stores XML information for the feature kind
  /// \param theMessage a container of the feature XML properties
  virtual void setFeatureInfo(const std::shared_ptr<Config_FeatureMessage>& theMessage);

  /// Returns container of existing features
  /// \returns list
  const std::list<std::shared_ptr<Config_FeatureMessage> >& featuresInfo() const;

private:
  std::string myName; /// a name of the workbench

  /// container of existing features
  std::list<std::shared_ptr<Config_FeatureMessage> > myFeatureInfo;
};

#endif /* XGUI_MENUGROUP_H_ */
