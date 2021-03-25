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

#include "SketchAPI_Projection.h"

#include <SketchPlugin_Line.h>
#include <SketchPlugin_BSpline.h>
#include <SketchPlugin_BSplinePeriodic.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_EllipticArc.h>
#include <SketchPlugin_Point.h>

#include <SketchAPI_Arc.h>
#include <SketchAPI_BSpline.h>
#include <SketchAPI_Circle.h>
#include <SketchAPI_Ellipse.h>
#include <SketchAPI_EllipticArc.h>
#include <SketchAPI_Line.h>
#include <SketchAPI_Point.h>

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
SketchAPI_Projection::SketchAPI_Projection(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_Projection::SketchAPI_Projection(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Selection & theExternalFeature )
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setExternalFeature(theExternalFeature);
  }
}

SketchAPI_Projection::~SketchAPI_Projection()
{

}

//--------------------------------------------------------------------------------------
void SketchAPI_Projection::setExternalFeature(const ModelHighAPI_Selection & theExternalFeature)
{
  fillAttribute(theExternalFeature, externalFeature());

  execute(true);
}

void SketchAPI_Projection::setIncludeToResult(bool theKeepResult)
{
  fillAttribute(theKeepResult, includeToResult());
  execute(true);
}

void SketchAPI_Projection::setKeepReferenceToOriginal(bool theKeepRefToOriginal)
{
  // the Fixed constraint should be assigned explicitly
  fillAttribute(false, feature()->boolean(SketchPlugin_Projection::MAKE_FIXED()));
  fillAttribute(theKeepRefToOriginal ? "true" : "false",
                feature()->string(SketchPlugin_Projection::KEEP_REFERENCE_ID()));
  execute(true);
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_SketchEntity> SketchAPI_Projection::createdFeature() const
{
  AttributeRefAttrPtr aProjectedRefAttr = projectedFeature();
  FeaturePtr aProjectedFeature = ModelAPI_Feature::feature(aProjectedRefAttr->object());
  std::shared_ptr<SketchAPI_SketchEntity> anEntity;

  if(!aProjectedFeature.get()) {
    return anEntity;
  }

  if (aProjectedFeature->getKind() == SketchPlugin_Line::ID())
    anEntity.reset(new SketchAPI_Line(aProjectedFeature));
  else if (aProjectedFeature->getKind() == SketchPlugin_Circle::ID())
    anEntity.reset(new SketchAPI_Circle(aProjectedFeature));
  else if (aProjectedFeature->getKind() == SketchPlugin_Arc::ID())
    anEntity.reset(new SketchAPI_Arc(aProjectedFeature));
  else if (aProjectedFeature->getKind() == SketchPlugin_Ellipse::ID())
    anEntity.reset(new SketchAPI_Ellipse(aProjectedFeature));
  else if (aProjectedFeature->getKind() == SketchPlugin_EllipticArc::ID())
    anEntity.reset(new SketchAPI_EllipticArc(aProjectedFeature));
  else if (aProjectedFeature->getKind() == SketchPlugin_BSpline::ID())
    anEntity.reset(new SketchAPI_BSpline(aProjectedFeature));
  else if (aProjectedFeature->getKind() == SketchPlugin_BSplinePeriodic::ID())
    anEntity.reset(new SketchAPI_BSplinePeriodic(aProjectedFeature));
  else if (aProjectedFeature->getKind() == SketchPlugin_Point::ID())
    anEntity.reset(new SketchAPI_Point(aProjectedFeature));

  return anEntity;
}

//--------------------------------------------------------------------------------------

void SketchAPI_Projection::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = externalFeature();
  AttributeBooleanPtr isIncludeToRes = includeToResult();
  theDumper << aBase << " = " << aSketchName << ".addProjection("
            << anExternal << ", " << isIncludeToRes << ")" << std::endl;
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);

  // Dump created line feature
  AttributeRefAttrPtr aProjectedRefAttr = projectedFeature();
  FeaturePtr aProjectedFeature = ModelAPI_Feature::feature(aProjectedRefAttr->object());
  theDumper << aProjectedFeature << " = "
    << theDumper.name(aBase) << ".createdFeature()" << std::endl;
}
