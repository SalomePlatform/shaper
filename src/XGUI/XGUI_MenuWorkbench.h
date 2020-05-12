// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef XGUI_MENUWORKBENCH_H_
#define XGUI_MENUWORKBENCH_H_

#include "XGUI.h"

#include <string>
#include <list>
#include <memory>

#ifdef WIN32
#pragma warning(disable : 4251) // std::string needs dll-interface
#endif

class XGUI_MenuGroup;

/**
* \ingroup GUI
* A class for management of menu actions (features). The actions should be arranged like they are
* in XML file. It listens the read feature of XML and fills internal structure of menu workbenches
* and groups of feature. After, it creates menues and tools in the module.
*/
class XGUI_EXPORT XGUI_MenuWorkbench
{
 public:
  /// Constructor
  XGUI_MenuWorkbench(const std::string& theName);
  /// Destructor
  virtual ~XGUI_MenuWorkbench() {}

  /// Returns a name of the workbench
  /// \return workbench name
  std::string getName() const { return myName; }

  /// Finds or creates a group for the given name
  /// \param theGroupName a name defined in XML
  /// \return an instance of group
  std::shared_ptr<XGUI_MenuGroup> findGroup(const std::string& theGroupName);

  /// Returns container of existing groups
  /// \returns list
  const std::list<std::shared_ptr<XGUI_MenuGroup> >& groups() const;

private:
  std::string myName; /// a name of the workbench
  std::list<std::shared_ptr<XGUI_MenuGroup> > myGroups; /// container of existing groups
};

#endif /* XGUI_MENUWORKBENCH_H_ */

