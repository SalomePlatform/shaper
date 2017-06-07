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

#ifndef PartSet_ExternalObjectsMgr_H
#define PartSet_ExternalObjectsMgr_H

#include "PartSet.h"

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Object.h>
#include <GeomAPI_Shape.h>

#include <ModuleBase_Definitions.h>

#include <string>

class ModuleBase_IWorkshop;
class ModuleBase_ViewerPrs;
class XGUI_Workshop;

/**
* \ingroup Modules
* Customosation of ModuleBase_WidgetShapeSelector in order to provide 
* working with sketch specific objects.
*/
class PARTSET_EXPORT PartSet_ExternalObjectsMgr
{
 public:
  /// Constructor
  /// \param theExternal the external state
  /// \param theCanCreateExternal the state if it can and should create external features
  /// \param theDefaultValue the default value for the external object using
  PartSet_ExternalObjectsMgr(const std::string& theExternal,
                             const std::string& theCanCreateExternal,
                             const bool theDefaultValue);

  virtual ~PartSet_ExternalObjectsMgr() {}

  /// Returns the state whether the external object is used
  bool useExternal() const { return myUseExternal; }

  /// Returns if new external objects can be created
  /// \return boolean value
  bool canCreateExternal() { return myCanCreateExternal;}

  /// Checks validity of the given object
  /// \param theObject an object to check
  /// \return valid or not valid
  bool isValidObject(const ObjectPtr& theObject);

  /// Finds or create and external object
  /// \param theSelectedObject an object
  /// \param theShape a selected shape, which is used in the selection attribute
  /// \param theSketch a current sketch
  /// \param theTemporary the created external object is temporary, execute is not performed for it
  /// \return the object
  ObjectPtr externalObject(const ObjectPtr& theSelectedObject, const GeomShapePtr& theShape,
                           const CompositeFeaturePtr& theSketch, const bool theTemporary = false);

  // Removes the external presentation from the model
  /// \param theSketch a current sketch
  /// \param theFeature a current feature
  /// \param theWorkshop a current workshop
  /// \param theTemporary if true, a temporary external object is removed overwise all ext objects
  void removeExternal(const CompositeFeaturePtr& theSketch,
                      const FeaturePtr& theFeature,
                      ModuleBase_IWorkshop* theWorkshop,
                      const bool theTemporary);

  /// Return an object and geom shape by the viewer presentation
  /// \param thePrs a selection
  /// \param theObject an output object
  /// \param theShape a shape of the selection
  virtual void getGeomSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs,
                                ObjectPtr& theObject,
                                GeomShapePtr& theShape,
                                ModuleBase_IWorkshop* theWorkshop,
                                const CompositeFeaturePtr& theSketch,
                                const bool isInValidate);

protected:
  /// Delete from the document the feature of the object. It deletes all objects, which refers to
  /// the deleted one. The parameter feature is ignored even it refer to the deleted object.
  /// \param theObject a removed object
  /// \param theSketch a current sketch
  /// \param theFeature a current feature
  /// \param theWorkshop a current workshop
  void removeExternalObject(const ObjectPtr& theObject,
                            const CompositeFeaturePtr& theSketch,
                            const FeaturePtr& theFeature,
                            ModuleBase_IWorkshop* theWorkshop);

  /// Returns the workshop
  static XGUI_Workshop* workshop(ModuleBase_IWorkshop* theWorkshop);

protected:
  /// An external object
  ObjectPtr myExternalObjectValidated;

  /// Boolean value about the neccessity of the external object use
  bool myUseExternal;
  /// Boolean value about the necessity of a new external object creation
  bool myCanCreateExternal;
};

#endif