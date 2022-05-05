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

#ifndef ModuleBase_WidgetSelectorStore_H
#define ModuleBase_WidgetSelectorStore_H

#include "ModuleBase.h"

#include <ModelAPI_Object.h>
#include <ModelAPI_Attribute.h>
#include <GeomAPI_Shape.h>

class ModuleBase_IWorkshop;

/**
* \ingroup GUI
  Provides for an attribute backup of values. 
  It is possible to store/ restore the attribute values.
*/
class ModuleBase_WidgetSelectorStore
{
 public:

  /// Constructor
  MODULEBASE_EXPORT ModuleBase_WidgetSelectorStore();
  /// Destructor
  MODULEBASE_EXPORT virtual ~ModuleBase_WidgetSelectorStore() {}

  /// Creates a backup of the current values of the attribute
  /// \param theAttribute a model attribute which parameters are to be stored
  /// \param theWorkshop a current workshop
  MODULEBASE_EXPORT void storeAttributeValue(const AttributePtr& theAttribute,
                                             ModuleBase_IWorkshop* theWorkshop);

  /// Creates a backup of the current values of the attribute
  /// \param theAttribute a model attribute which parameters are to be restored
  /// \param theWorkshop a current workshop
  MODULEBASE_EXPORT void restoreAttributeValue(const AttributePtr& theAttribute,
                                               ModuleBase_IWorkshop* theWorkshop);

private:
  /// backup parameters of the model attribute. The class processes three types of attribute:
  /// Reference, RefAttr and Selection.
  /// Depending on the attribute type, only the attribute parameter
  /// values are reserved in the backup
  /// An attribute object
  ObjectPtr myObject;
  /// An attribute shape
  GeomShapePtr myShape;
  /// A reference of the attribute
  AttributePtr myRefAttribute;
  /// A boolean value whether refAttr uses reference of object
  bool myIsObject;

  /// Variable of selection type
  std::string mySelectionType;
  /// Variable of GeomSelection
  int mySelectionCount; // number of elements in the attribute selection list when store

};

#endif
