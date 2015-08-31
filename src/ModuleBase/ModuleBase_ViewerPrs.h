// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ViewerPrs.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_ViewerPrs_H
#define ModuleBase_ViewerPrs_H

#include "ModuleBase.h"

#include <memory>
#include <TopoDS_Shape.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <AIS_InteractiveObject.hxx>

#include <ModelAPI_Result.h>

/**\class ModuleBase_ViewerPrs
 * \ingroup GUI
 * \brief Presentation. Provides container to have feature, shape and/or selection owner.
 */
class ModuleBase_ViewerPrs
{
 public:
  /// Constructor
  MODULEBASE_EXPORT ModuleBase_ViewerPrs();

  /// Constructor
  /// \param theResult an object
  /// \param theShape a viewer shape
  /// \param theOwner a selection owner
  MODULEBASE_EXPORT ModuleBase_ViewerPrs(ObjectPtr theResult, const TopoDS_Shape& theShape,
                       Handle_SelectMgr_EntityOwner theOwner);

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
  MODULEBASE_EXPORT void setShape(const TopoDS_Shape& theShape)
  {
    myShape = theShape;
  }

  /// Returns the shape
  /// \return a shape instance
  MODULEBASE_EXPORT const TopoDS_Shape& shape() const
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
    return myShape.IsNull() &&
           myOwner.IsNull() && !myResult.get();
  }

  /// Returns True if the current object is equal to the given one
  /// \param thePrs an object to compare
  MODULEBASE_EXPORT bool operator==(const ModuleBase_ViewerPrs& thePrs)
  {
    bool aResult = (myResult.get() == thePrs.object().get());
    bool aOwner = (myOwner.Access() == thePrs.owner().Access());
    bool aShape = myShape.IsEqual(thePrs.shape()) == Standard_True;
    bool aIO = (myInteractive == thePrs.interactive()) == Standard_True;
    return aResult && aOwner && aShape && aIO;
  }

 private:
  ObjectPtr myResult;  /// the feature
  Handle(SelectMgr_EntityOwner) myOwner;  /// the selection owner
  TopoDS_Shape myShape;  /// the shape
  Handle(AIS_InteractiveObject) myInteractive;  /// interactive object
};

#endif
