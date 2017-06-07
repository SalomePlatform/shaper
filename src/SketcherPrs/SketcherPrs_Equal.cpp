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

#include "SketcherPrs_Equal.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <SketchPlugin_Constraint.h>

#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Root.hxx>


IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Equal, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Equal::SketcherPrs_Equal(ModelAPI_Feature* theConstraint,
                                     const std::shared_ptr<GeomAPI_Ax3>& thePlane)
 : SketcherPrs_SymbolPrs(theConstraint, thePlane)
{
}

bool SketcherPrs_Equal::IsReadyToDisplay(ModelAPI_Feature* theConstraint,
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

bool SketcherPrs_Equal::updateIfReadyToDisplay(double theStep, bool withColor) const
{
  if (!IsReadyToDisplay(myConstraint, myPlane))
    return false;

  ObjectPtr aObj1 =
    SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  ObjectPtr aObj2 =
    SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_B());

  // Set points of the presentation
  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  gp_Pnt aP1 = aMgr->getPosition(aObj1, this, theStep);
  gp_Pnt aP2 = aMgr->getPosition(aObj2, this, theStep);

  myPntArray = new Graphic3d_ArrayOfPoints(2, withColor);
  myPntArray->AddVertex(aP1);
  myPntArray->AddVertex(aP2);
  return true;
}

void SketcherPrs_Equal::drawLines(const Handle(Prs3d_Presentation)& thePrs,
                                  Quantity_Color theColor) const
{
  // Draw first line
  ObjectPtr aObj = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  std::shared_ptr<GeomAPI_Shape> aLine = SketcherPrs_Tools::getShape(aObj);
  if (aLine.get() == NULL)
    return;
  drawShape(aLine, thePrs, theColor);

  // Draw second line
  aObj = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_B());
  aLine = SketcherPrs_Tools::getShape(aObj);
  if (aLine.get() == NULL)
    return;
  drawShape(aLine, thePrs, theColor);
}

