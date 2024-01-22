// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef PartSet_FieldStepPrs_H
#define PartSet_FieldStepPrs_H

#include "PartSet.h"
#include <ModuleBase_IStepPrs.h>

#include <ModelAPI_ResultField.h>
#include <ModelAPI_Feature.h>

#include <ViewerData_AISShape.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>

#include <QList>

DEFINE_STANDARD_HANDLE(PartSet_FieldStepPrs, ViewerData_AISShape)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide specific behaviour
* for wire presentations based in a one plane
*/
class PartSet_FieldStepPrs : public ViewerData_AISShape, public ModuleBase_IStepPrs
{
public:
  /// Constructor
  /// \param theResult a result object
  Standard_EXPORT PartSet_FieldStepPrs(FieldStepPtr theStep);

  ModelAPI_AttributeTables::ValueType dataType() const;

  bool dataRange(double& theMin, double& theMax) const;

  DEFINE_STANDARD_RTTIEXT(PartSet_FieldStepPrs, ViewerData_AISShape)

protected:
  //! Compute presentation considering sub-shape color map.
  virtual void Compute(const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
    const Handle(Prs3d_Presentation)& thePrs,  const Standard_Integer theMode);

private:
  QList<double> range(double& theMin, double& theMax) const;
  QList<double> booleanValues() const;
  QStringList strings() const;
  bool computeMassCenter(const TopoDS_Shape& theShape, gp_Pnt& theCenter);


  FieldStepPtr myStep;
  FeaturePtr myFeature;
};

#endif
