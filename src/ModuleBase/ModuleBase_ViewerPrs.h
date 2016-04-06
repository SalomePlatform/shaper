// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ViewerPrs.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_ViewerPrs_H
#define ModuleBase_ViewerPrs_H

#include "ModuleBase.h"

#include <memory>
#include <SelectMgr_EntityOwner.hxx>
#include <AIS_InteractiveObject.hxx>

#include <ModelAPI_Result.h>
#include <GeomAPI_Shape.h>

/**\class ModuleBase_ViewerPrs
 * \ingroup GUI
 * \brief Presentation. Provides container to have feature, shape and/or selection owner.
 */
class ModuleBase_ViewerPrs
{
 public:
  /// Constructor
  /// \param theResult an object
  /// \param theShape a viewer shape
  /// \param theOwner a selection owner
  MODULEBASE_EXPORT ModuleBase_ViewerPrs(ObjectPtr theResult = ObjectPtr(),
                                         const GeomShapePtr& theShape = GeomShapePtr(),
                                         Handle_SelectMgr_EntityOwner theOwner = NULL);

  /// Destructor
  MODULEBASE_EXPORT virtual ~ModuleBase_ViewerPrs();

  /// Sets the object.
  /// \param theResult an object instance
  MODULEBASE_EXPORT void setObject(ObjectPtr theResult)
  {
    myResult = theResult;
  }

  /// Returns the feature.
  /// \return a feature instance
  MODULEBASE_EXPORT ObjectPtr object() const
  {
    return myResult;
  }

  /// Set the presentation owner
  /// \param theOwner an owner to set
  MODULEBASE_EXPORT void setOwner(Handle_SelectMgr_EntityOwner theOwner)
  {
    myOwner = theOwner;
  }

  /// Returns the presentation owner
  /// \return an owner
  MODULEBASE_EXPORT Handle_SelectMgr_EntityOwner owner() const
  {
    return myOwner;
  }

  /// Sets the shape
  /// \param theShape a shape instance
  MODULEBASE_EXPORT void setShape(const GeomShapePtr& theShape)
  {
    myShape = theShape;
  }

  /// Returns the shape
  /// \return a shape instance
  MODULEBASE_EXPORT const GeomShapePtr& shape() const
  {
    return myShape;
  }

  /// Set interactive object
  /// \param theIO an interactive object
  MODULEBASE_EXPORT void setInteractive(const Handle(AIS_InteractiveObject)& theIO)
  {
    myInteractive = theIO;
  }

  /// Returns interactive object if it is installed
  MODULEBASE_EXPORT Handle(AIS_InteractiveObject) interactive() const
  {
    return myInteractive;
  }

  /// Returns true if all presentation fields are empty
  /// \return boolean value
  MODULEBASE_EXPORT bool isEmpty() const
  {
    return (!myShape.get() || myShape->isNull()) &&
           myOwner.IsNull() && !myResult.get();
  }

  /// Returns True if the current object is equal to the given one
  /// \param thePrs an object to compare
  MODULEBASE_EXPORT bool operator==(const ModuleBase_ViewerPrs& thePrs);

 private:
  ObjectPtr myResult;  /// the feature
  Handle(SelectMgr_EntityOwner) myOwner;  /// the selection owner
  GeomShapePtr myShape;  /// the shape
  Handle(AIS_InteractiveObject) myInteractive;  /// interactive object
};

typedef std::shared_ptr<ModuleBase_ViewerPrs> ModuleBase_ViewerPrsPtr;

#endif
