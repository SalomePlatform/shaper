// File:        ModuleBase_ViewerPrs.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_ViewerPrs_H
#define ModuleBase_ViewerPrs_H

#include "ModuleBase.h"

#include <boost/shared_ptr.hpp>
#include <TopoDS_Shape.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <AIS_InteractiveObject.hxx>

#include <ModelAPI_Result.h>

/**\class ModuleBase_ViewerPrs
 * \ingroup Module base
 * \brief Presentation. Provides container to have feature, shape and/or selection owner.
 */
class ModuleBase_ViewerPrs
{
 public:
  /// Constructor
  ModuleBase_ViewerPrs()
  {
  }

  /// Constructor
  /// \param theFeature a model feature
  /// \param theShape a viewer shape
  /// \param theOwner a selection owner
  ModuleBase_ViewerPrs(ObjectPtr theResult, const TopoDS_Shape& theShape,
                       Handle_SelectMgr_EntityOwner theOwner)
      : myResult(theResult),
        myShape(theShape),
        myOwner(theOwner)
  {
  }

  /// Destructor
  virtual ~ModuleBase_ViewerPrs()
  {
  }

  /// Sets the feature.
  /// \param theFeature a feature instance
  void setFeature(ObjectPtr theResult)
  {
    myResult = theResult;
  }

  /// Returns the feature.
  /// \return a feature instance
  ObjectPtr object() const
  {
    return myResult;
  }

  /// Returns the presentation owner
  /// \param the owner
  void setOwner(Handle_SelectMgr_EntityOwner theOwner)
  {
    myOwner = theOwner;
  }

  /// Returns the presentation owner
  /// \return an owner
  Handle_SelectMgr_EntityOwner owner() const
  {
    return myOwner;
  }

  /// Sets the shape
  /// \param theShape a shape instance
  void setShape(const TopoDS_Shape& theShape)
  {
    myShape = theShape;
  }

  /// Returns the shape
  /// \return a shape instance
  const TopoDS_Shape& shape() const
  {
    return myShape;
  }

  void setInteractive(const Handle(AIS_InteractiveObject)& theIO)
  {
    myInteractive = theIO;
  }

  Handle(AIS_InteractiveObject) interactive() const
  {
    return myInteractive;
  }

  bool operator==(const ModuleBase_ViewerPrs& thePrs)
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
  Handle(AIS_InteractiveObject) myInteractive;
};

#endif
