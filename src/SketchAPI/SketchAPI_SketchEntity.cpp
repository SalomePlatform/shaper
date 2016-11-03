// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : SketchAPI_SketchEntity.cpp
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_SketchEntity.h"
#include <SketchAPI_Arc.h>
#include <SketchAPI_Circle.h>
#include <SketchAPI_IntersectionPoint.h>
#include <SketchAPI_Line.h>
#include <SketchAPI_Point.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_IntersectionPoint.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
//--------------------------------------------------------------------------------------
SketchAPI_SketchEntity::SketchAPI_SketchEntity(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

SketchAPI_SketchEntity::~SketchAPI_SketchEntity()
{

}

//--------------------------------------------------------------------------------------
bool SketchAPI_SketchEntity::initialize()
{
  SET_ATTRIBUTE(Auxiliary, ModelAPI_AttributeBoolean, SketchPlugin_SketchEntity::AUXILIARY_ID())

  return true;
}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelAPI_AttributeBoolean> SketchAPI_SketchEntity::auxiliary() const
{
  return myAuxiliary;
}

void SketchAPI_SketchEntity::setAuxiliary(bool theAuxiliary)
{
  fillAttribute(theAuxiliary, myAuxiliary);

  execute();
}

//--------------------------------------------------------------------------------------
void SketchAPI_SketchEntity::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  AttributeBooleanPtr anAux = aBase->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID());
  if (anAux->value()) {
    const std::string& aName = theDumper.name(aBase);
    theDumper << aName << ".setAuxiliary(" << anAux << ")" <<std::endl;
  }
}

bool SketchAPI_SketchEntity::isCopy() const
{
  // check the feature is a copy of another entity
  AttributeBooleanPtr isCopy = feature()->boolean(SketchPlugin_SketchEntity::COPY_ID());
  return isCopy.get() && isCopy->value();
}

std::list<std::shared_ptr<ModelHighAPI_Interface> >
SketchAPI_SketchEntity::wrap(const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures)
{
  std::list<std::shared_ptr<ModelHighAPI_Interface> > aResult;
  std::list<std::shared_ptr<ModelAPI_Feature> >::const_iterator anIt = theFeatures.begin();
  for (; anIt != theFeatures.end(); ++anIt) {
    if ((*anIt)->getKind() == SketchPlugin_Line::ID())
      aResult.push_back(std::shared_ptr<ModelHighAPI_Interface>(new SketchAPI_Line(*anIt)));
    else if ((*anIt)->getKind() == SketchPlugin_Arc::ID())
      aResult.push_back(std::shared_ptr<ModelHighAPI_Interface>(new SketchAPI_Arc(*anIt)));
    else if ((*anIt)->getKind() == SketchPlugin_Circle::ID())
      aResult.push_back(std::shared_ptr<ModelHighAPI_Interface>(new SketchAPI_Circle(*anIt)));
    else if ((*anIt)->getKind() == SketchPlugin_Point::ID())
      aResult.push_back(std::shared_ptr<ModelHighAPI_Interface>(new SketchAPI_Point(*anIt)));
    else if ((*anIt)->getKind() == SketchPlugin_IntersectionPoint::ID())
      aResult.push_back(std::shared_ptr<ModelHighAPI_Interface>(
                                                    new SketchAPI_IntersectionPoint(*anIt)));
  }
  return aResult;
}
