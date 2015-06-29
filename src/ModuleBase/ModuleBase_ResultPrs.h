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

  /// Returns result object
  Standard_EXPORT ResultPtr getResult() const { return myResult; }

  /// Returns True if the presentation is in sketch mode
  Standard_EXPORT bool isSketchMode() const { return myIsSketchMode; }

  /// Returns a list of faces
  Standard_EXPORT const std::list<std::shared_ptr<GeomAPI_Shape> >& facesList() { return myFacesList; }

  DEFINE_STANDARD_RTTI(ModuleBase_ResultPrs)
protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer aMode) ;

private:
  /// Reference to result object
  ResultPtr myResult;

  /// A sketch mode flag
  bool myIsSketchMode;

  /// Original shape of the result object
  TopoDS_Shape myOriginalShape;

  /// List of faces
  std::list<std::shared_ptr<GeomAPI_Shape> > myFacesList;
};


#endif