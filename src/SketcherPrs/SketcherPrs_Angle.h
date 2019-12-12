// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef SketcherPrs_Angle_H
#define SketcherPrs_Angle_H

#include <GeomAPI_Ax3.h>
#include <ModelAPI_Feature.h>
#include <SketchPlugin_Sketch.h>

#include <AIS_AngleDimension.hxx>
#include <Standard_DefineHandle.hxx>

#include <SketcherPrs_DimensionStyle.h>

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
    SketchPlugin_Sketch* theSketcher);

  /// Destructor
  Standard_EXPORT ~SketcherPrs_Angle();

  DEFINE_STANDARD_RTTIEXT(SketcherPrs_Angle, AIS_AngleDimension)

  /// Returns true if the constraint feature arguments are correcly filled
  /// to build AIS presentation
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  /// \return boolean result value
  static bool IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                               const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  /// Returns sketcher plane
  Standard_EXPORT std::shared_ptr<GeomAPI_Ax3> plane() const
  {
    return mySketcher->coordinatePlane();
  }

protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(
    const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
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

  /// A current sketcher
  SketchPlugin_Sketch* mySketcher;

  Handle(Prs3d_DimensionAspect) myAspect;

  /// Listener to update dimension visualization style
  SketcherPrs_DimensionStyle* myStyleListener;

  /// container of values obtained from the constraint, which are necessary to fill the presentation
  gp_Pnt myFirstPoint; ///< the dimension first point for measured geometry
  gp_Pnt mySecondPoint; ///< the dimension second point for measured geometry
  gp_Pnt myCenterPoint; ///< the dimension center point for measured geometry
  gp_Pnt myFlyOutPoint;  ///< the dimension fly out point for measured geometry

  SketcherPrs_DimensionStyle::DimensionValue myValue; /// the structure filled by constraint
};


#endif
