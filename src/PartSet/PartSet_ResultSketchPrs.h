// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_ResultSketchPrs.h
// Created:     25 March 2015
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_ResultSketchPrs_H
#define PartSet_ResultSketchPrs_H

#include "ModuleBase.h"

#include <ModelAPI_Result.h>

#include <ViewerData_AISShape.hxx>

#include <TopoDS_Compound.hxx>

DEFINE_STANDARD_HANDLE(PartSet_ResultSketchPrs, ViewerData_AISShape)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide specific behaviour 
* for wire presentations based in a one plane
*/
class PartSet_ResultSketchPrs: public ViewerData_AISShape
{
public:
  /// Constructor
  /// \param theResult a result object
  Standard_EXPORT PartSet_ResultSketchPrs(ResultPtr theResult);

  /// Returns true if the checked type can be accepted for the base selection type
  /// It returns true if they are coicide or if the base type is face and the checked type is wire
  /// @return boolean result
  static Standard_EXPORT bool isValidShapeType(const TopAbs_ShapeEnum& theBaseType,
                                               const TopAbs_ShapeEnum& theCheckedType);

  DEFINE_STANDARD_RTTI(PartSet_ResultSketchPrs)
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
  /// \param theTypeOfSelection type of selection: VERTEX, EDGE, WIRE
  void appendShapeSelection(const Handle(SelectMgr_Selection)& theSelection,
                            const TopoDS_Shape& theShape, const TopAbs_ShapeEnum& theTypeOfSelection);

  /// Sets color/line style/width of the presentation depending on the parameter state
  /// \param isAuxiliary a boolean value if the properties are for auxiliary objects
  void setAuxiliaryPresentationStyle(const bool isAuxiliary);

  /// Fills the containers by the current result
  /// \param theResultShape contains a shape of the result, it will be set as a Shape of AIS_Shape
  /// \param theAuxiliaryCompound a compound of auxiliary shapes
  /// \param theFaceList a list of face shapes
  void PartSet_ResultSketchPrs::fillShapes(TopoDS_Shape& aResultShape,
                                         TopoDS_Compound& theAuxiliaryCompound,
                                         NCollection_List<TopoDS_Shape>& theFaceList);

  /// Reference to result object
  ResultPtr myResult;

  /// Original shape of the result object
  TopoDS_Shape myOriginalShape;

  /// Compound of auxiliary shapes
  TopoDS_Compound myAuxiliaryCompound;

  /// List of faces
  //std::list<std::shared_ptr<GeomAPI_Shape> > myFacesList;
  NCollection_List<TopoDS_Shape> mySketchFaceList;
};


#endif