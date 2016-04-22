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

class SketcherPrs_DimensionStyleListener;

DEFINE_STANDARD_HANDLE(SketcherPrs_Angle, AIS_AngleDimension)

/**
* \ingroup GUI
* A class for representation of angle constraint
*/
class SketcherPrs_Angle : public AIS_AngleDimension
{
public:
  /// Constructor
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  Standard_EXPORT SketcherPrs_Angle(ModelAPI_Feature* theConstraint, 
                        const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  /// Destructor
  Standard_EXPORT ~SketcherPrs_Angle();

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

  /// Checks is the angle plane has inverted direction of normal to the plane of current sketcher
  /// Returns true if crossed product is negative.
  /// \return bolean value
  bool isAnglePlaneReversedToSketchPlane();

  /// Calculates a distance between center of the angle presentation and the value of flyout point
  /// attribute. It should be used after setting measured geometry for the angle presentation as
  /// it uses calculated center/first/second point coordinates
  /// \return real value
  double calculateDistanceToFlyoutPoint();

  static bool readyToDisplay(ModelAPI_Feature* theConstraint,
                             const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                             gp_Pnt& theFirstPoint, gp_Pnt& theSecondPoint,
                             gp_Pnt& theCenterPoint);

private:
  /// Constraint feature
  ModelAPI_Feature* myConstraint;

  /// Plane of the current sketcher
  std::shared_ptr<GeomAPI_Ax3> mySketcherPlane;

  Handle(Prs3d_DimensionAspect) myAspect;

  /// Listener to update dimension visualization style
  SketcherPrs_DimensionStyleListener* myStyleListener;

  /// container of values obtained from the constraint, which are necessary to fill the presentation
  gp_Pnt myFirstPoint; ///< the dimension first point for measured geometry
  gp_Pnt mySecondPoint; ///< the dimension second point for measured geometry
  gp_Pnt myCenterPoint; ///< the dimension center point for measured geometry
  gp_Pnt myFlyOutPoint;  ///< the dimension fly out point for measured geometry
  double myAngle; ///< the angle value to be shown as custom value of presentation
  bool myHasParameters; ///< true if the atrribute value has used parameters
  std::string myValue; ///< the angle value depending on angle type
};


#endif
