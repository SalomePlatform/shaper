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

#ifndef SketcherPrs_Radius_H
#define SketcherPrs_Radius_H

#include <GeomAPI_Ax3.h>
#include <ModelAPI_Feature.h>
#include <SketchPlugin_Sketch.h>

#include <AIS_RadiusDimension.hxx>
#include <Standard_DefineHandle.hxx>

#include <SketcherPrs_DimensionStyle.h>

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
    SketchPlugin_Sketch* theSketcher);

  /// Destructor
  Standard_EXPORT ~SketcherPrs_Radius();

  DEFINE_STANDARD_RTTIEXT(SketcherPrs_Radius, AIS_RadiusDimension)

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

private:
  /// Fills the constraint parameters by constraint and plane
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  /// \param theCircle a circle build on the constraint values
  /// \param thePoint an anchor point to show text value
  /// \return boolean result value
  static bool readyToDisplay(ModelAPI_Feature* theConstraint,
                             const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                             gp_Circ& theCircle, gp_Pnt& theAnchorPoint);
protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(
    const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer aMode);

private:
  /// Constraint feature
  ModelAPI_Feature* myConstraint;

  /// A current sketcher
  SketchPlugin_Sketch* mySketcher;

  /// Listener to update dimension visualization style
  SketcherPrs_DimensionStyle* myStyleListener;

  /// container of values obtained from the constraint, which are necessary to fill the presentation
  gp_Circ myCircle; ///< the radius circle
  gp_Pnt myAnchorPoint; ///< an ancor for the radius value visualization

  SketcherPrs_DimensionStyle::DimensionValue myValue; /// the structure filled by constraint
};

#endif