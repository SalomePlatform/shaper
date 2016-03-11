// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Angle.h
// Created:     20 August 2015
// Author:      Vitaly SMETANNIKOV


#ifndef SketcherPrs_Angle_H
#define SketcherPrs_Angle_H

#include <GeomAPI_Ax3.h>
#include <ModelAPI_Feature.h>

#include <AIS_AngleDimension.hxx>
#include <Standard_DefineHandle.hxx>

DEFINE_STANDARD_HANDLE(SketcherPrs_Angle, AIS_AngleDimension)

/**
* \ingroup GUI
* A class for representation of angle constraint
*/
class SketcherPrs_Angle : public AIS_AngleDimension
{
public:
  /// Type of angle
  enum AngleType{
    ANGLE_DIRECT,   ///< Angle from the first line to the second line
    ANGLE_SUPPLEMENTARY,  ///< Additional angle to the angle from first to second line
    ANGLE_BACKWARD ///< Angle from the second line to the first line
  };

public:
  /// Constructor
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  Standard_EXPORT SketcherPrs_Angle(ModelAPI_Feature* theConstraint, 
                        const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  DEFINE_STANDARD_RTTI(SketcherPrs_Angle)

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
  /// Constraint feature
  ModelAPI_Feature* myConstraint;

  /// Plane of the current sketcher
  std::shared_ptr<GeomAPI_Ax3> myPlane;

  Handle(Prs3d_DimensionAspect) myAspect;
};


#endif
