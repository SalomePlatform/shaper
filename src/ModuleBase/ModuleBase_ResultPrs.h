// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ResultPrs.h
// Created:     21 October 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModuleBase_ResultPrs_H
#define ModuleBase_ResultPrs_H

#include "ModuleBase.h"

#include <ModelAPI_Result.h>

#include <AIS_Shape.hxx>
#include <Standard_DefineHandle.hxx>

DEFINE_STANDARD_HANDLE(ModuleBase_ResultPrs, AIS_Shape)

class ModuleBase_ResultPrs: public AIS_Shape
{
public:
  Standard_EXPORT ModuleBase_ResultPrs(ResultPtr theResult);

  Standard_EXPORT ResultPtr getResult() const { return myResult; }

  DEFINE_STANDARD_RTTI(ModuleBase_ResultPrs)
protected:
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer aMode) ;

private:
  ResultPtr myResult;

  bool myIsSketchMode;

  TopoDS_Shape myOriginalShape;
  std::list<std::shared_ptr<GeomAPI_Shape> > myFacesList;
};


#endif