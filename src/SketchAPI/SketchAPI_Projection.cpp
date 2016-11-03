// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : SketchAPI_Projection.cpp
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_Projection.h"
//--------------------------------------------------------------------------------------
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

SketchAPI_Projection::SketchAPI_Projection(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::string & theExternalName)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalName(theExternalName);
  }
}

SketchAPI_Projection::~SketchAPI_Projection()
{

}

//--------------------------------------------------------------------------------------
void SketchAPI_Projection::setExternalFeature(const ModelHighAPI_Selection & theExternalFeature)
{
  fillAttribute(theExternalFeature, externalFeature());

  execute();
}

void SketchAPI_Projection::setByExternalName(const std::string& theExternalName)
{
  fillAttribute(ModelHighAPI_Selection("EDGE", theExternalName), external());

  execute();
}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelHighAPI_Interface> SketchAPI_Projection::createdFeature() const
{
  AttributeRefAttrPtr aProjectedRefAttr = projectedFeature();
  FeaturePtr aProjectedFeature = ModelAPI_Feature::feature(aProjectedRefAttr->object());

  return std::shared_ptr<ModelHighAPI_Interface>(new ModelHighAPI_Interface(aProjectedFeature));
}

//--------------------------------------------------------------------------------------

void SketchAPI_Projection::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = externalFeature();
  theDumper << aBase << " = " << aSketchName << ".addProjection(" << anExternal << ")" << std::endl;
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);

  // Dump created line feature
  AttributeRefAttrPtr aProjectedRefAttr = projectedFeature();
  FeaturePtr aProjectedFeature = ModelAPI_Feature::feature(aProjectedRefAttr->object());
  std::string aProjectedName = theDumper.name(aProjectedFeature, false);
  theDumper << aProjectedName << " = " << theDumper.name(aBase) << ".createdFeature()" << std::endl;
}
