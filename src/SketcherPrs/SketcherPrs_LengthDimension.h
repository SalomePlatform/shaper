// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef SketcherPrs_LinearDimension_H
#define SketcherPrs_LinearDimension_H

#include <GeomAPI_Ax3.h>
#include <ModelAPI_Feature.h>
#include <AIS_LengthDimension.hxx>
#include <Standard_DefineHandle.hxx>
#include <SketchPlugin_Sketch.h>

#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <string>

#include <SketcherPrs_DimensionStyle.h>

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
    SketchPlugin_Sketch* theSketcher);

  /// Destructor
  Standard_EXPORT ~SketcherPrs_LengthDimension();

  DEFINE_STANDARD_RTTIEXT(SketcherPrs_LengthDimension, AIS_LengthDimension)

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
    const Handle(Prs3d_Presentation)& thePresentation,
    const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer aMode);

private:
  static bool readyToDisplay(ModelAPI_Feature* theConstraint,
                             const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                             gp_Pnt2d& thePnt1, gp_Pnt2d& thePnt2);

  /// Set the direction for horizontal/vertical constraint
  void setDirection(ModelAPI_Feature* theConstraint,
                    const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  /// Constraint feature
  ModelAPI_Feature* myConstraint;

  /// A current sketcher
  SketchPlugin_Sketch* mySketcher;

  //Handle(Prs3d_DimensionAspect) myAspect;

  /// Listener to update dimension visualization style
  SketcherPrs_DimensionStyle* myStyleListener;

  /// container of values obtained from the constraint, which are necessary
  /// to fill the presentation
  gp_Pnt myFirstPoint; ///< the dimension first point for measured geometry
  gp_Pnt mySecondPoint; ///< the dimension first point for measured geometry
  gp_Pln myPlane; ///< the plane(plane of the sketch) for measured geometry
  double myDistance; ///< the flyout distance

  /// the structure filled by constraint
  SketcherPrs_DimensionStyle::DimensionValue myValue;
};

#endif