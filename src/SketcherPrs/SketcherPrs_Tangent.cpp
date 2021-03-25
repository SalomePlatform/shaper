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

#include "SketcherPrs_Tangent.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <GeomAPI_Curve.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Vertex.h>

#include <SketchPlugin_Constraint.h>

#include <GeomAdaptor_Curve.hxx>
#include <Prs3d_Root.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <StdPrs_DeflectionCurve.hxx>


IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Tangent, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Tangent::SketcherPrs_Tangent(ModelAPI_Feature* theConstraint,
  SketchPlugin_Sketch* theSketcher)
 : SketcherPrs_SymbolPrs(theConstraint, theSketcher)
{
}

bool SketcherPrs_Tangent::IsReadyToDisplay(ModelAPI_Feature* theConstraint,
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

bool SketcherPrs_Tangent::updateIfReadyToDisplay(double theStep, bool withColor) const
{
  if (!IsReadyToDisplay(myConstraint, plane()))
    return false;

  ObjectPtr aObj1 =
    SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  ObjectPtr aObj2 =
    SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_B());

  GeomShapePtr aShp1 = SketcherPrs_Tools::getShape(aObj1);
  GeomShapePtr aShp2 = SketcherPrs_Tools::getShape(aObj2);

  GeomCurvePtr aCurv1 = std::shared_ptr<GeomAPI_Curve>(new GeomAPI_Curve(aShp1));
  GeomCurvePtr aCurv2 = std::shared_ptr<GeomAPI_Curve>(new GeomAPI_Curve(aShp2));

  GeomPointPtr aPnt;
  GeomShapePtr aIntPnt = aShp1->intersect(aShp2);
  if (aIntPnt.get() && aIntPnt->isVertex()) {
    GeomVertexPtr aVetrex(new GeomAPI_Vertex(aIntPnt));
    aPnt = aVetrex->point();
  }

  // Compute points coordinates
  if (aPnt.get()) {
    SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
    gp_Pnt aP1 = aMgr->getPosition(aObj1, this, theStep, aPnt);
    myPntArray = new Graphic3d_ArrayOfPoints(1, withColor);
    myPntArray->AddVertex(aP1);
  } else {
    SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
    gp_Pnt aP1 = aMgr->getPosition(aObj1, this, theStep);
    gp_Pnt aP2 = aMgr->getPosition(aObj2, this, theStep);
    myPntArray = new Graphic3d_ArrayOfPoints(2, withColor);
    myPntArray->AddVertex(aP1);
    myPntArray->AddVertex(aP2);
  }
  return true;
}

void SketcherPrs_Tangent::drawLines(const Handle(Prs3d_Presentation)& thePrs,
                                    Quantity_Color theColor) const
{
  ObjectPtr aObj1 = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  ObjectPtr aObj2 = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_B());

  std::shared_ptr<GeomAPI_Shape> aShape1 = SketcherPrs_Tools::getShape(aObj1);
  std::shared_ptr<GeomAPI_Shape> aShape2 = SketcherPrs_Tools::getShape(aObj2);

  if ((aShape1.get() == NULL) || (aShape2.get() == NULL))
    return;
  drawShape(aShape1, thePrs, theColor);
  drawShape(aShape2, thePrs, theColor);
}

