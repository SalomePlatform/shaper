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

#include "SketchPlugin_ConstraintCoincidence.h"

#include <SketchPlugin_Point.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>

#include <ModelAPI_Events.h>
#include <ModelGeomAlgo_Point2D.h>
#include <GeomDataAPI_Point2D.h>

#include <SketcherPrs_Factory.h>

#include <Events_Loop.h>

SketchPlugin_ConstraintCoincidence::SketchPlugin_ConstraintCoincidence()
{
}

void SketchPlugin_ConstraintCoincidence::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
}

void SketchPlugin_ConstraintCoincidence::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintCoincidence::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::coincidentConstraint(this, sketch(), thePrevious);
  return anAIS;
}

AttributePoint2DPtr SketchPlugin_ConstraintCoincidence::getPoint(const FeaturePtr& theFeature)
{
  AttributePoint2DPtr aPoint = ModelGeomAlgo_Point2D::getPointOfRefAttr(theFeature.get(),
                                                         SketchPlugin_Constraint::ENTITY_A(),
                                 SketchPlugin_Point::ID(), SketchPlugin_Point::COORD_ID());
  if (!aPoint.get())
    aPoint = ModelGeomAlgo_Point2D::getPointOfRefAttr(theFeature.get(),
                                                      SketchPlugin_Constraint::ENTITY_B(),
                                 SketchPlugin_Point::ID(), SketchPlugin_Point::COORD_ID());
  return aPoint;
}

void SketchPlugin_ConstraintCoincidence::createCoincidenceFeature(SketchPlugin_Sketch* theSketch,
                                           const std::shared_ptr<GeomDataAPI_Point2D>& thePoint1,
                                           const std::shared_ptr<GeomDataAPI_Point2D>& thePoint2)
{
  FeaturePtr aFeature;
  if (theSketch) {
    aFeature = theSketch->addFeature(SketchPlugin_ConstraintCoincidence::ID());
  } else {
    std::shared_ptr<ModelAPI_Document> aDoc = theSketch->document();
    aFeature = aDoc->addFeature(SketchPlugin_ConstraintCoincidence::ID());
  }

  std::shared_ptr<ModelAPI_Data> aData = aFeature->data();

  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef1 = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  aRef1->setAttr(thePoint1);

  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef2 = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  aRef2->setAttr(thePoint2);

  // we need to flush created signal in order to coincidence is processed by solver
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
}
