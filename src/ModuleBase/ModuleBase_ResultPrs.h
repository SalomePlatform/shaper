// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ResultPrs.h
// Created:     21 October 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModuleBase_ResultPrs_H
#define ModuleBase_ResultPrs_H

#include "ModuleBase.h"

#include <ModelAPI_Result.h>

#include <ViewerData_AISShape.hxx>
#include <Standard_DefineHandle.hxx>
#include <StdSelect_BRepOwner.hxx>

DEFINE_STANDARD_HANDLE(ModuleBase_BRepOwner, StdSelect_BRepOwner)

/**
* \ingroup GUI
* A redefinition of standard BRep Owner in order to provide specific selection
* of CompSolid objects. This owner is created only for selection mode TopAbs_COMPSOLID
*/
class ModuleBase_BRepOwner: public StdSelect_BRepOwner
{
public:
  ModuleBase_BRepOwner(const TopoDS_Shape& aShape, 
    const Standard_Integer aPriority = 0, 
    const Standard_Boolean ComesFromDecomposition = Standard_False)
    : StdSelect_BRepOwner(aShape, aPriority, ComesFromDecomposition) {}

  virtual void HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& aPM, 
    const Quantity_NameOfColor aCol, const Standard_Integer aMode = 0)
  {  Selectable()->HilightOwnerWithColor(aPM, aCol, this); }

  DEFINE_STANDARD_RTTI(ModuleBase_BRepOwner)
};



DEFINE_STANDARD_HANDLE(ModuleBase_ResultPrs, ViewerData_AISShape)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide specific behaviour 
* for wire presentations based in a one plane
*/
class ModuleBase_ResultPrs: public ViewerData_AISShape
{
public:
  /// Constructor
  /// \param theResult a result object
  Standard_EXPORT ModuleBase_ResultPrs(ResultPtr theResult);

  /// Returns true if the checked type can be accepted for the base selection type
  /// It returns true if they are coicide or if the base type is face and the checked type is wire
  /// @return boolean result
  static Standard_EXPORT bool isValidShapeType(const TopAbs_ShapeEnum& theBaseType,
                                               const TopAbs_ShapeEnum& theCheckedType);


  //! Method which draws selected owners ( for fast presentation draw )
  Standard_EXPORT virtual void HilightSelected(const Handle(PrsMgr_PresentationManager3d)& thePM, 
                                               const SelectMgr_SequenceOfOwner& theOwners);
  
  //! Method which hilight an owner belonging to
  //! this selectable object  ( for fast presentation draw )
  Standard_EXPORT virtual void HilightOwnerWithColor(const Handle(PrsMgr_PresentationManager3d)& thePM, 
                                                     const Quantity_NameOfColor theColor, const Handle(SelectMgr_EntityOwner)& theOwner);

  /// Returns result object
  Standard_EXPORT ResultPtr getResult() const { return myResult; }

  DEFINE_STANDARD_RTTI(ModuleBase_ResultPrs)
protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer aMode) ;

private:
  /// Appens sensitive and owners for wires of the given shape into selection
  /// \param theSelection a current filled selection
  /// \param theShape a shape
  void appendWiresSelection(const Handle(SelectMgr_Selection)& theSelection,
                            const TopoDS_Shape& theShape);

  /// Reference to result object
  ResultPtr myResult;

  /// Original shape of the result object
  TopoDS_Shape myOriginalShape;
};


#endif