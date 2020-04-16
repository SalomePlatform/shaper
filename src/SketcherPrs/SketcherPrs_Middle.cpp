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

#include "SketcherPrs_Middle.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <SketchPlugin_Constraint.h>

#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Root.hxx>


IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Middle, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Middle::SketcherPrs_Middle(ModelAPI_Feature* theConstraint,
  SketchPlugin_Sketch* theSketcher)
 : SketcherPrs_SymbolPrs(theConstraint, theSketcher)
{
}

bool SketcherPrs_Middle::IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                                         const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  bool aReadyToDisplay = false;
  if (thePlane) {
    ObjectPtr aObj1 =
      SketcherPrs_Tools::getResult(theConstraint, SketchPlugin_Constraint::ENTITY_A());
    ObjectPtr aObj2 =
      SketcherPrs_Tools::getResult(theConstraint, SketchPlugin_Constraint::ENTITY_B());

    // one object is a feature Line, other object is a point result. We check shape of point result
    aReadyToDisplay = aObj1.get() && aObj2.get() &&
                      (SketcherPrs_Tools::getShape(aObj1).get() != NULL ||
                       SketcherPrs_Tools::getShape(aObj2).get() != NULL);
  }
  return aReadyToDisplay;
}

bool SketcherPrs_Middle::updateIfReadyToDisplay(double theStep, bool withColor) const
{
  if (!IsReadyToDisplay(myConstraint, plane()))
    return false;
  ObjectPtr aPointObject;

  // find a line result to set middle symbol near it
  myPntArray = new Graphic3d_ArrayOfPoints(1, withColor);
  AttributePtr anAttribute =
    SketcherPrs_Tools::getAttribute(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  if (!anAttribute.get()) {
    ObjectPtr aObj =
      SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
    std::shared_ptr<GeomAPI_Shape> aShape = SketcherPrs_Tools::getShape(aObj);
    if (aShape.get() && aShape->isEdge())
      aPointObject = aObj;
  }
  if (!aPointObject.get()) {
    ObjectPtr aObj =
      SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_B());
    std::shared_ptr<GeomAPI_Shape> aShape = SketcherPrs_Tools::getShape(aObj);
    if (aShape.get() && aShape->isEdge())
      aPointObject = aObj;
  }

  if (!aPointObject.get())
    return false;

  // Set points of the presentation
  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  gp_Pnt aP1 = aMgr->getPosition(aPointObject, this, theStep);
  myPntArray->SetVertice(1, aP1);
  return true;
}

void SketcherPrs_Middle::drawLines(const Handle(Prs3d_Presentation)& thePrs,
                                   Quantity_Color theColor) const
{
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(thePrs);

  Handle(Graphic3d_AspectLine3d) aLineAspect =
    new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 2);
  aGroup->SetPrimitivesAspect(aLineAspect);

  // Draw objects
  ObjectPtr aObject =
    SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  drawLine(thePrs, theColor, aObject);

  aObject = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  drawLine(thePrs, theColor, aObject);
}

void SketcherPrs_Middle::drawLine(const Handle(Prs3d_Presentation)& thePrs,
                                  Quantity_Color theColor, const ObjectPtr& theObject) const
{
  FeaturePtr aLineFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (aLineFeature.get()) {
    std::list<ResultPtr> aResults = aLineFeature->results();
    if (aResults.size() == 1) {
      ResultPtr aResult = aResults.front();
      std::shared_ptr<GeomAPI_Shape> aLine = SketcherPrs_Tools::getShape(aResult);
      if (aLine.get() != NULL)
        drawShape(aLine, thePrs, theColor);
    }
  }
  else {
    std::shared_ptr<GeomAPI_Shape> aLine = SketcherPrs_Tools::getShape(theObject);
    if (aLine.get() != NULL)
      drawShape(aLine, thePrs, theColor);
  }
}

