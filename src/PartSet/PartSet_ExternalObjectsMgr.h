// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_ExternalObjectsMgr.h
// Created:     15 Apr 2015
// Author:      Natalia Ermolaeva


#ifndef PartSet_ExternalObjectsMgr_H
#define PartSet_ExternalObjectsMgr_H

#include "PartSet.h"

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Object.h>
#include <GeomAPI_Shape.h>

#include <ModuleBase_Definitions.h>

#include <string>

class ModuleBase_IWorkshop;
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
  /// \param theDefaultValue the default value for the external object using
  PartSet_ExternalObjectsMgr(const std::string& theExternal, const bool theDefaultValue);

  virtual ~PartSet_ExternalObjectsMgr() {}

  /// Returns the state whether the external object is used
  bool useExternal() const { return myUseExternal; }

  bool isValidObject(const ObjectPtr& theObject);

  /// Finds or create and external object
  /// \param theSelectedObject an object
  /// \param theShape a selected shape, which is used in the selection attribute
  /// \param theSketch a current sketch
  /// \return the object
  ObjectPtr externalObject(const ObjectPtr& theSelectedObject, const GeomShapePtr& theShape,
                           const CompositeFeaturePtr& theSketch);

  ObjectPtr externalObjectValidated(const ObjectPtr& theSelectedObject, const GeomShapePtr& theShape,
                           const CompositeFeaturePtr& theSketch);

  // Removes the external presentation from the model
  /// \param theSketch a current sketch
  /// \param theFeature a current feature
  /// \param theFeature a current workshop
  void removeExternal(const CompositeFeaturePtr& theSketch,
                      const FeaturePtr& theFeature,
                      ModuleBase_IWorkshop* theWorkshop);

  void removeExternalValidated(const CompositeFeaturePtr& theSketch,
                               const FeaturePtr& theFeature,
                               ModuleBase_IWorkshop* theWorkshop);

  void removeUnusedExternalObjects(const QObjectPtrList& theIgnoreObjects,
                            const CompositeFeaturePtr& theSketch,
                            const FeaturePtr& theFeature);

protected:
  void removeExternalObject(const ObjectPtr& theObject,
                            const CompositeFeaturePtr& theSketch,
                            const FeaturePtr& theFeature,
                            ModuleBase_IWorkshop* theWorkshop);

  /// Returns the workshop
  static XGUI_Workshop* workshop(ModuleBase_IWorkshop* theWorkshop);

protected:
  /// An external object
  QObjectPtrList myExternalObjects;

  /// An external object
  ObjectPtr myExternalObjectValidated;

  /// Boolean value about the neccessity of the external object use
  bool myUseExternal;
};

#endif