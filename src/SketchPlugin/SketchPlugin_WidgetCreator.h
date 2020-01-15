// Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_WidgetCreator_H
#define SketchPlugin_WidgetCreator_H


#include <SketchPlugin.h>
#include <ModuleBase_IWidgetCreator.h>

#include <string>
#include <set>

class QWidget;

/**
 *  \ingroup GUI
 *  Interface to WidgetCreator which can create specific widgets by type
 */
class SketchPlugin_WidgetCreator : public ModuleBase_IWidgetCreator
{
public:
  /// Default constructor
  SketchPlugin_WidgetCreator();

  /// Virtual destructor
  virtual ~SketchPlugin_WidgetCreator() {}

  /// Returns a container of possible page types, which this creator can process
  /// \param theTypes a list of type names
  virtual void panelTypes(std::set<std::string>& theTypes);

  /// Create panel control by its type.
  /// The default implementation is empty
  /// \param theType a panel type
  /// \param theParent a parent widget
  /// \param theFeature a feature modified in the panel
  /// \return created widget or null
  virtual QWidget* createPanelByType(const std::string& theType,
                                     QWidget* theParent,
                                     const FeaturePtr& theFeature,
                                     Config_WidgetAPI* theWidgetApi);
private:
  std::set<std::string> myPanelTypes; ///< types of panels
};

#endif