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

#include "SketcherPrs_Perpendicular.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <SketchPlugin_Constraint.h>

#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Root.hxx>

#include <GeomAPI_Curve.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>


IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Perpendicular, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Perpendicular::SketcherPrs_Perpendicular(ModelAPI_Feature* theConstraint,
  SketchPlugin_Sketch* theSketcher)
 : SketcherPrs_SymbolPrs(theConstraint, theSketcher)
{
}

bool SketcherPrs_Perpendicular::IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                                                 const std::shared_ptr<GeomAPI_Ax3>&/* thePlane*/)
{
  bool aReadyToDisplay = false;

  ObjectPtr aObj1 =
    SketcherPrs_Tools::getResult(theConstraint, SketchPlugin_Constraint::ENTITY_A());
  ObjectPtr aObj2 =
    SketcherPrs_Tools::getResult(theConstraint, SketchPlugin_Constraint::ENTITY_B());

  aReadyToDisplay = SketcherPrs_Tools::getShape(aObj1).get() != NULL &&
                    SketcherPrs_Tools::getShape(aObj2).get() != NULL;
  return aReadyToDisplay;
}

bool SketcherPrs_Perpendicular::updateIfReadyToDisplay(double theStep, bool withColor) const
{
  if (!IsReadyToDisplay(myConstraint, myPlane))
    return false;

  ObjectPtr aObj1 =
    SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  ObjectPtr aObj2 =
    SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_B());

  GeomShapePtr aShp1 = SketcherPrs_Tools::getShape(aObj1);
  GeomShapePtr aShp2 = SketcherPrs_Tools::getShape(aObj2);

  GeomEdgePtr aEdge1(new GeomAPI_Edge(aShp1));
  GeomEdgePtr aEdge2(new GeomAPI_Edge(aShp2));

  std::shared_ptr<GeomAPI_Lin> aLin1 = aEdge1->line();
  std::shared_ptr<GeomAPI_Lin> aLin2 = aEdge2->line();

  std::shared_ptr<GeomAPI_Pnt> aPnt = aLin1->intersect(aLin2);
  double aParam1 = aLin1->projParam(aPnt);
  double aParam2 = aLin2->projParam(aPnt);

  GeomAPI_Curve aCurve1(aShp1);
  GeomAPI_Curve aCurve2(aShp2);
  bool isInside1 = (aParam1 >= (aCurve1.startParam() - Precision::Confusion())) &&
    (aParam1 <= (aCurve1.endParam() + Precision::Confusion()));
  bool isInside2 = (aParam2 >= (aCurve2.startParam() - Precision::Confusion())) &&
    (aParam2 <= (aCurve2.endParam() + Precision::Confusion()));

  if (!(isInside1 && isInside2))
    aPnt = std::shared_ptr<GeomAPI_Pnt>();

  // Compute position of symbols
  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  gp_Pnt aP1 = aMgr->getPosition(aObj1, this, theStep, aPnt);
  myPntArray = new Graphic3d_ArrayOfPoints(aPnt.get()? 1 : 2, withColor);
  myPntArray->AddVertex(aP1);
  if (!aPnt.get()) {
    gp_Pnt aP2 = aMgr->getPosition(aObj2, this, theStep);
    myPntArray->AddVertex(aP2);
  }
  return true;
}


void SketcherPrs_Perpendicular::drawLines(const Handle(Prs3d_Presentation)& thePrs,
                                          Quantity_Color theColor) const
{
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(thePrs);

  Handle(Graphic3d_AspectLine3d) aLineAspect =
    new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 2);
  aGroup->SetPrimitivesAspect(aLineAspect);

  // Draw constrained lines
  addLine(aGroup, SketchPlugin_Constraint::ENTITY_A());
  addLine(aGroup, SketchPlugin_Constraint::ENTITY_B());
}

