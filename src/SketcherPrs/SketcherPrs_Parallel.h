// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Parallel.h
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_Parallel_H
#define SketcherPrs_Parallel_H

#include <GeomAPI_Ax3.h>
#include <GeomAPI_Edge.h>

#include <AIS_InteractiveObject.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Standard_DefineHandle.hxx>

class SketchPlugin_Constraint;
class SketchPlugin_Sketch;


DEFINE_STANDARD_HANDLE(SketcherPrs_Parallel, AIS_InteractiveObject)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide  
* presentation of coincident constraint
*/
class SketcherPrs_Parallel: public AIS_InteractiveObject
{
public:
  /// Constructor
  /// \param theResult a result object
  Standard_EXPORT SketcherPrs_Parallel(SketchPlugin_Constraint* theConstraint, 
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane);


  DEFINE_STANDARD_RTTI(SketcherPrs_Parallel)
protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer aMode) ;

private:

  SketchPlugin_Constraint* myConstraint;
  std::shared_ptr<GeomAPI_Ax3> myPlane;
  Handle(Graphic3d_AspectMarker3d) myAspect;
  Handle(Graphic3d_ArrayOfPoints) myPntArray;
};

#endif