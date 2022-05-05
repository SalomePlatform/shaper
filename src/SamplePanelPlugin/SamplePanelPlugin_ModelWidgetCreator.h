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

#ifndef SamplePanelPlugin_ModelWidgetCreator_H
#define SamplePanelPlugin_ModelWidgetCreator_H

#include "SamplePanelPlugin.h"

#include <ModuleBase_IWidgetCreator.h>

#include <string>
#include <set>

class ModuleBase_IWorkshop;
class Config_WidgetAPI;
class QWidget;

/** 
* \ingroup GUI
* Interface to WidgetCreator which can create specific widgets by type
*/
class SamplePanelPlugin_ModelWidgetCreator : public ModuleBase_IWidgetCreator
{
public:
  /// Default constructor
  SamplePanelPlugin_ModelWidgetCreator();

  /// Virtual destructor
  ~SamplePanelPlugin_ModelWidgetCreator() {}

  /// Returns a container of possible widget types, which this creator can process
  /// \param a list of type names
  virtual void widgetTypes(std::set<std::string>& theTypes);

  /// Create widget by its type
  /// The default implementation is empty
  /// \param theType a type
  /// \param theParent a parent widget
  /// \param theData a low-level API for reading xml definitions of widgets
  /// \param theWorkshop a current workshop
  /// \return a created model widget or null
  virtual ModuleBase_ModelWidget* createWidgetByType(const std::string& theType,
                                                     QWidget* theParent,
                                                     Config_WidgetAPI* theWidgetApi,
                                                     ModuleBase_IWorkshop* /*theWorkshop*/);
private:
  std::set<std::string> myModelWidgetTypes; /// types of widgets
};

#endif
