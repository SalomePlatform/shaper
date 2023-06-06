// Copyright (C) 2019-2022  CEA/DEN, EDF R&D
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

#include "SketchAPI_MacroMiddlePoint.h"

#include <ModelHighAPI_RefAttr.h>

#include <SketchPlugin_ConstraintMiddle.h>

#include <cmath>

SketchAPI_MacroMiddlePoint::SketchAPI_MacroMiddlePoint(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_Point(theFeature)
{
  ConstraintPtr aConstraint = std::dynamic_pointer_cast<SketchPlugin_Constraint>(theFeature);
  if (aConstraint)
    initialize();
}

SketchAPI_MacroMiddlePoint::SketchAPI_MacroMiddlePoint(const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const ModelHighAPI_RefAttr& theLine, const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
  : SketchAPI_Point(theFeature, thePoint)
{
  createConstraint(theLine);
}

void SketchAPI_MacroMiddlePoint::createConstraint(const ModelHighAPI_RefAttr& theLine)
{
  // Find sketch
  CompositeFeaturePtr aSketch;
  const std::set<AttributePtr>& aRefs = feature()->data()->refsToMe();
  for (std::set<AttributePtr>::const_iterator anIt = aRefs.begin(); anIt != aRefs.end(); ++anIt)
    if ((*anIt)->id() == SketchPlugin_Sketch::FEATURES_ID())
    {
      aSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>((*anIt)->owner());
      break;
    }
  if (!aSketch)
    return;

  // Create middle constraint for line and poiunt
  std::shared_ptr<ModelAPI_Feature> aConstrFeature =
    aSketch->addFeature(SketchPlugin_ConstraintMiddle::ID());
  aConstrFeature->string(SketchPlugin_ConstraintMiddle::MIDDLE_TYPE())->setValue(SketchPlugin_ConstraintMiddle::MIDDLE_TYPE_BY_LINE());
  aConstrFeature->refattr(SketchPlugin_Constraint::ENTITY_A())->setObject(theLine.object());
  aConstrFeature->refattr(SketchPlugin_Constraint::ENTITY_B())->setAttr(coordinates());
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aConstrFeature->attribute(SketchPlugin_ConstraintMiddle::POINT_REF_ID()))->setValue(coordinates()->x(), coordinates()->y());

  aConstrFeature->execute();

  feature()->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue(true);
  feature()->reference(SketchPlugin_SketchEntity::PARENT_ID())->setValue(aConstrFeature);
  feature()->execute();
}
