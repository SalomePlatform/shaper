// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef SketcherPrs_Coincident_H
#define SketcherPrs_Coincident_H

#include <GeomAPI_Ax3.h>
#include <SketchPlugin_Sketch.h>
#include <ModelAPI_Feature.h>

#include <AIS_InteractiveObject.hxx>
#include <Standard_DefineHandle.hxx>


DEFINE_STANDARD_HANDLE(SketcherPrs_Coincident, AIS_InteractiveObject)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide  
* presentation of coincident constraint
*/
class SketcherPrs_Coincident: public AIS_InteractiveObject
{
public:
  /// Constructor
  /// \param theConstraint a constraint object
  /// \param thePlane plane of a sketch
  Standard_EXPORT SketcherPrs_Coincident(ModelAPI_Feature* theConstraint,
    SketchPlugin_Sketch* theSketch);

  /// Returns true if the constraint feature arguments are correcly filled to build AIS presentation
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  /// \return boolean result value
  static bool IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                               const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  DEFINE_STANDARD_RTTIEXT(SketcherPrs_Coincident, AIS_InteractiveObject)
protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(
    const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
    const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer aMode) ;

private:
  static bool readyToDisplay(ModelAPI_Feature* theConstraint,
                      const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                      gp_Pnt& thePoint);

private:
  ModelAPI_Feature* myConstraint;
  SketchPlugin_Sketch* mySketch;
  gp_Pnt myPoint;
};


#endif