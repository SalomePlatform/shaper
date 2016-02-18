// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_LengthDimension.h
// Created:     27 March 2015
// Author:      Vitaly SMETANNIKOV


#ifndef SketcherPrs_LinearDimension_H
#define SketcherPrs_LinearDimension_H

#include <GeomAPI_Ax3.h>
#include <ModelAPI_Feature.h>
#include <AIS_LengthDimension.hxx>
#include <Standard_DefineHandle.hxx>


DEFINE_STANDARD_HANDLE(SketcherPrs_LengthDimension, AIS_LengthDimension)

/**
* \ingroup GUI
* A class for representation of linear dimension constraint.
* It supports SketchPlugin_ConstraintLength and SketchPlugin_ConstraintDistance features.
*/
class SketcherPrs_LengthDimension : public AIS_LengthDimension
{
public:
  /// Constructor
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  Standard_EXPORT SketcherPrs_LengthDimension(ModelAPI_Feature* theConstraint, 
                        const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  DEFINE_STANDARD_RTTI(SketcherPrs_LengthDimension)

  /// Returns true if the constraint feature arguments are correcly filled to build AIS presentation
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  /// \return boolean result value
  static bool IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                               const std::shared_ptr<GeomAPI_Ax3>& thePlane);
protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer aMode);

private:
  static bool getPoints(ModelAPI_Feature* theConstraint,
                 const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                 gp_Pnt& thePnt1, gp_Pnt& thePnt2);

  /// Constraint feature
  ModelAPI_Feature* myConstraint;

  /// Plane of the current sketcher
  std::shared_ptr<GeomAPI_Ax3> myPlane;

  Handle(Prs3d_DimensionAspect) myAspect;
};

#endif