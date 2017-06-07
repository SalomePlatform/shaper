// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "SketcherPrs_Rigid.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Curve.h>
#include <GeomAPI_Vertex.h>

#include <ModelAPI_AttributeRefAttr.h>

#include <SketchPlugin_Constraint.h>

#include <gp_Pnt2d.hxx>

#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Root.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <BRep_Tool.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <StdPrs_Point.hxx>
#include <Geom_CartesianPoint.hxx>



IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Rigid, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;



SketcherPrs_Rigid::SketcherPrs_Rigid(ModelAPI_Feature* theConstraint,
                                           const std::shared_ptr<GeomAPI_Ax3>& thePlane)
 : SketcherPrs_SymbolPrs(theConstraint, thePlane)
{
}

bool SketcherPrs_Rigid::IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                                         const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  bool aReadyToDisplay = false;

  std::shared_ptr<ModelAPI_Data> aData = theConstraint->data();
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr =
    aData->refattr(SketchPlugin_Constraint::ENTITY_A());
  AttributePtr aRefAttr = anAttr->attr();
  if (anAttr->isObject()) {
    // The constraint attached to an object
    ObjectPtr aObj = anAttr->object();
    aReadyToDisplay =  SketcherPrs_Tools::getShape(aObj).get() != NULL;

  } else {
    // The constraint attached to a point
    std::shared_ptr<GeomAPI_Pnt2d> aPnt = SketcherPrs_Tools::getPoint(theConstraint,
                                                   SketchPlugin_Constraint::ENTITY_A());
    aReadyToDisplay = aPnt.get() != NULL;
  }
  return aReadyToDisplay;
}

bool SketcherPrs_Rigid::updateIfReadyToDisplay(double theStep, bool withColor) const
{
  if (!IsReadyToDisplay(myConstraint, myPlane))
    return false;

  myPntArray = new Graphic3d_ArrayOfPoints(1, withColor);
  std::shared_ptr<ModelAPI_Data> aData = myConstraint->data();
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr =
    aData->refattr(SketchPlugin_Constraint::ENTITY_A());
  AttributePtr aRefAttr = anAttr->attr();
  if (anAttr->isObject()) {
    // The constraint attached to an object
    ObjectPtr aObj = anAttr->object();

    SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
    gp_Pnt aP1 = aMgr->getPosition(aObj, this, theStep);
    myPntArray->AddVertex(aP1);
  } else {
    // The constraint attached to a point
    std::shared_ptr<GeomAPI_Pnt2d> aPnt = SketcherPrs_Tools::getPoint(myConstraint,
                                                  SketchPlugin_Constraint::ENTITY_A());
    std::shared_ptr<GeomAPI_Pnt> aPoint = myPlane->to3D(aPnt->x(), aPnt->y() + theStep);
    myPntArray->AddVertex(aPoint->impl<gp_Pnt>());
  }
  return true;
}


void SketcherPrs_Rigid::drawLines(const Handle(Prs3d_Presentation)& thePrs,
                                  Quantity_Color theColor) const
{
  ObjectPtr aObj = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
  std::shared_ptr<GeomAPI_Shape> aShape;
  if (aFeature.get()) {
    // If constraint attached to a feature
    const std::list<ResultPtr>& aResults = aFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    // Find a shape
    for (aIt = aResults.cbegin(); aIt != aResults.cend(); aIt++) {
      aShape = (*aIt)->shape();
      if (aShape->isEdge())
        break;
    }
  } else {
    // Else it is a result
    aShape = SketcherPrs_Tools::getShape(aObj);
  }
  if (aShape.get() == NULL)
    return;

  Handle(Prs3d_PointAspect) aPntAspect = new Prs3d_PointAspect(Aspect_TOM_PLUS, theColor, 1);
  myDrawer->SetPointAspect(aPntAspect);
  drawShape(aShape, thePrs, theColor);
}

