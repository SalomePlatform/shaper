// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "SketcherPrs_Offset.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <SketchPlugin_Offset.h>

#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeDouble.h>

#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Root.hxx>


IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Offset, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Offset::SketcherPrs_Offset(ModelAPI_Feature* theConstraint,
  SketchPlugin_Sketch* theSketcher)
  : SketcherPrs_SymbolPrs(theConstraint, theSketcher)
{
}

bool SketcherPrs_Offset::IsReadyToDisplay(ModelAPI_Feature* theConstraint,
  const std::shared_ptr<GeomAPI_Ax3>&/* thePlane*/)
{
  bool aReadyToDisplay = false;

  AttributeDoublePtr aValueAttr = theConstraint->real(SketchPlugin_Offset::VALUE_ID());
  if (aValueAttr->isInitialized()) {
    AttributeRefListPtr aSelectedEdges = theConstraint->reflist(SketchPlugin_Offset::ENTITY_A());
    aReadyToDisplay = (aSelectedEdges->list().size() > 0);
    if (aReadyToDisplay) {
      AttributeRefListPtr aOffcetEdges = theConstraint->reflist(SketchPlugin_Offset::ENTITY_B());
      aReadyToDisplay = (aOffcetEdges->list().size() > 0);
    }
  }
  return aReadyToDisplay;
}

bool SketcherPrs_Offset::updateIfReadyToDisplay(double theStep, bool withColor) const
{
  if (!IsReadyToDisplay(myConstraint, plane()))
    return false;
  if (!plane())
    return false;

  // Set points of the presentation
  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();

  AttributeRefListPtr aSelectedEdges = myConstraint->reflist(SketchPlugin_Offset::ENTITY_A());
  int aNb = aSelectedEdges->size();

  AttributeRefListPtr aOffcetEdges = myConstraint->reflist(SketchPlugin_Offset::ENTITY_B());
  int aOffNb = aOffcetEdges->size();

  myPntArray = new Graphic3d_ArrayOfPoints(aNb + aOffNb, withColor);
  int i;
  ObjectPtr aObj;
  gp_Pnt aP1;
  // get position for each source object
  for (i = 0; i < aNb; i++) {
    aObj = aSelectedEdges->object(i);
    if (SketcherPrs_Tools::getShape(aObj).get() == NULL)
      continue;
    aP1 = aMgr->getPosition(aObj, this, theStep);
    myPntArray->SetVertice(i + 1, aP1);
  }
  for (i = aNb; i < aNb + aOffNb; i++) {
    aObj = aOffcetEdges->object(i - aNb);
    if (SketcherPrs_Tools::getShape(aObj).get() == NULL)
      continue;
    aP1 = aMgr->getPosition(aObj, this, theStep);
    myPntArray->SetVertice(i + 1, aP1);
  }
  return true;
}

void SketcherPrs_Offset::drawLines(const Handle(Prs3d_Presentation)& thePrs,
  Quantity_Color theColor) const
{
  AttributeRefListPtr aSelectedEdges = myConstraint->reflist(SketchPlugin_Offset::ENTITY_A());
  if (aSelectedEdges.get() == NULL)
    return;
  AttributeRefListPtr aOffcetEdges = myConstraint->reflist(SketchPlugin_Offset::ENTITY_B());
  if (aOffcetEdges.get() == NULL)
    return;

  if (aSelectedEdges->size() == 0)
    return;

  if (aOffcetEdges->size() == 0)
    return;

  // Draw source objects
  drawListOfShapes(aSelectedEdges, thePrs, theColor);
  // Draw offcet objects
  drawListOfShapes(aOffcetEdges, thePrs, theColor);
}
