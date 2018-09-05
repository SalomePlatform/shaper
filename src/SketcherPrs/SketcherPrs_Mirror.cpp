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

#include "SketcherPrs_Mirror.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <SketchPlugin_Constraint.h>

#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_LineAspect.hxx>



IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Mirror, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Mirror::SketcherPrs_Mirror(ModelAPI_Feature* theConstraint,
  SketchPlugin_Sketch* theSketcher)
 : SketcherPrs_SymbolPrs(theConstraint, theSketcher)
{
}

bool SketcherPrs_Mirror::IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                                          const std::shared_ptr<GeomAPI_Ax3>&/* thePlane*/)
{
  bool aReadyToDisplay = false;

  // Get axis of mirror
  ObjectPtr aAxisObj =
    SketcherPrs_Tools::getResult(theConstraint, SketchPlugin_Constraint::ENTITY_A());
  if (SketcherPrs_Tools::getShape(aAxisObj).get() == NULL)
    return aReadyToDisplay;

  std::shared_ptr<ModelAPI_Data> aData = theConstraint->data();
  // Get source objects
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrB =
    aData->reflist(SketchPlugin_Constraint::ENTITY_B());
  if (anAttrB.get() == NULL)
    return aReadyToDisplay;
  // Get mirrored objects
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrC =
    aData->reflist(SketchPlugin_Constraint::ENTITY_C());
  if (anAttrC.get() == NULL)
    return aReadyToDisplay;

  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  int aNb = anAttrB->size();
  // If size of source objects and mirrored ones is not equal then the constraint is not computed
  if (aNb != anAttrC->size())
    return aReadyToDisplay;

  aReadyToDisplay = true;
  return aReadyToDisplay;
}

bool SketcherPrs_Mirror::updateIfReadyToDisplay(double theStep, bool withColor) const
{
  if (!IsReadyToDisplay(myConstraint, myPlane))
    return false;

  // Get axis of mirror
  ObjectPtr aAxisObj =
    SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());

  std::shared_ptr<ModelAPI_Data> aData = myConstraint->data();
  // Get source objects
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrB =
    aData->reflist(SketchPlugin_Constraint::ENTITY_B());
  // Get mirrored objects
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrC =
    aData->reflist(SketchPlugin_Constraint::ENTITY_C());

  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  int aNb = anAttrB->size();

  myPntArray = new Graphic3d_ArrayOfPoints(2 * aNb, withColor);
  int i;
  ObjectPtr aObj;
  gp_Pnt aP1;
  // get position for each source object
  for (i = 0; i < aNb; i++) {
    aObj = anAttrB->object(i);
    if (SketcherPrs_Tools::getShape(aObj).get() == NULL)
      continue;
    aP1 = aMgr->getPosition(aObj, this, theStep);
    myPntArray->SetVertice(i + 1, aP1);
  }
  // Get position of each mirrored object
  for (i = 0; i < aNb; i++) {
    aObj = anAttrC->object(i);
    if (SketcherPrs_Tools::getShape(aObj).get() == NULL)
      continue;
    aP1 = aMgr->getPosition(aObj, this, theStep);
    myPntArray->SetVertice(aNb + i + 1, aP1);
  }
  return true;
}


void SketcherPrs_Mirror::drawLines(const Handle(Prs3d_Presentation)& thePrs,
  Quantity_Color theColor) const
{
  std::shared_ptr<ModelAPI_Data> aData = myConstraint->data();
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrB =
    aData->reflist(SketchPlugin_Constraint::ENTITY_B());
  if (anAttrB.get() == NULL)
    return;
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrC =
    aData->reflist(SketchPlugin_Constraint::ENTITY_C());
  if (anAttrC.get() == NULL)
    return;

  int aNb = anAttrB->size();
  if (aNb != anAttrC->size())
    return;

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(thePrs);

  // drawListOfShapes uses myDrawer for attributes definition
  Handle(Prs3d_LineAspect) aLnAspect = new Prs3d_LineAspect(theColor, Aspect_TOL_SOLID, 1);
  myDrawer->SetLineAspect(aLnAspect);

  // Draw axis line
  addLine(aGroup, SketchPlugin_Constraint::ENTITY_A());

  // Draw source objects
  drawListOfShapes(anAttrB, thePrs, theColor);

  // draw mirrored objects
  drawListOfShapes(anAttrC, thePrs, theColor);
}

