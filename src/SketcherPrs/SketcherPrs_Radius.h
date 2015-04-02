// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Radius.h
// Created:     26 March 2015
// Author:      Vitaly SMETANNIKOV


#ifndef SketcherPrs_Radius_H
#define SketcherPrs_Radius_H

#include <GeomAPI_Ax3.h>
#include <ModelAPI_Feature.h>

#include <AIS_RadiusDimension.hxx>
#include <Standard_DefineHandle.hxx>

DEFINE_STANDARD_HANDLE(SketcherPrs_Radius, AIS_RadiusDimension)

/**
* \ingroup GUI
* A class for representation of radius constraint
*/
class SketcherPrs_Radius : public AIS_RadiusDimension
{
public:
  /// Constructor
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  Standard_EXPORT SketcherPrs_Radius(ModelAPI_Feature* theConstraint, 
                        const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  DEFINE_STANDARD_RTTI(SketcherPrs_Radius)
protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

private:
  /// Constraint feature
  ModelAPI_Feature* myConstraint;

  /// Plane of the current sketcher
  std::shared_ptr<GeomAPI_Ax3> myPlane;

  Handle(Prs3d_DimensionAspect) myAspect;
};

#endif