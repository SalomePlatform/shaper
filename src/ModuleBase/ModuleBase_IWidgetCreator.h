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

#ifndef ModuleBase_IWidgetCreator_H
#define ModuleBase_IWidgetCreator_H

#include "ModuleBase.h"

#include "ModelAPI_Feature.h"

#include <set>
#include <string>
#include <memory>

#ifdef _MSC_VER
#pragma warning(disable: 4100)
#endif

class ModuleBase_ModelWidget;
class ModuleBase_PageBase;
class ModuleBase_IWorkshop;

class Config_WidgetAPI;

class QWidget;

/** 
* \ingroup GUI
* Interface to WidgetCreator which can create specific widgets by type
*/
class MODULEBASE_EXPORT ModuleBase_IWidgetCreator
{
public:
  /// Default constructor
  ModuleBase_IWidgetCreator();

  /// Virtual destructor
  ~ModuleBase_IWidgetCreator();

  /// Returns a container of possible page types, which this creator can process
  /// The default implementation is empty
  /// \param theTypes a list of type names
  virtual void panelTypes(std::set<std::string>& /*theTypes*/) {}

  /// Returns a container of possible page types, which this creator can process
  /// The default implementation is empty
  /// \param theTypes a list of type names
  virtual void pageTypes(std::set<std::string>& theTypes) {}

  /// Returns a container of possible widget types, which this creator can process
  /// \param theTypes a list of type names
  virtual void widgetTypes(std::set<std::string>& theTypes) {}

  /// Create panel control by its type.
  /// The default implementation is empty
  /// \param theType a panel type
  /// \param theParent a parent widget
  /// \param theFeature a feature modified in the panel
  /// \param theWidgetApi a low-level API for reading xml definitions of widget
  /// \return created widget or null
  virtual QWidget* createPanelByType(const std::string& theType,
                                     QWidget* theParent,
                                     const FeaturePtr& theFeature,
                                     Config_WidgetAPI* theWidgetApi = 0);

  /// Create page by its type
  /// The default implementation is empty
  /// \param theType a type
  /// \param theParent a parent widget
  /// \param theWidgetApi a low-level API for reading xml definitions of widgets
  virtual ModuleBase_PageBase* createPageByType(const std::string& theType,
                                                QWidget* theParent,
                                                Config_WidgetAPI* theWidgetApi);

  /// Create widget by its type
  /// The default implementation is empty
  /// \param theType a type
  /// \param theParent a parent widget
  /// \param theWidgetApi a low-level API for reading xml definitions of widgets
  /// \param theWorkshop a workshop class instance
  virtual ModuleBase_ModelWidget* createWidgetByType(const std::string& theType,
                                                     QWidget* theParent,
                                                     Config_WidgetAPI* theWidgetApi,
                                                     ModuleBase_IWorkshop* theWorkshop);
};

typedef std::shared_ptr<ModuleBase_IWidgetCreator> WidgetCreatorPtr;


#endif