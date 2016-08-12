// Name   : SketchAPI_SketchEntity.cpp
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_SketchEntity.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>
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
  std::shared_ptr<SketchPlugin_SketchEntity> aSketchEntity =
      std::dynamic_pointer_cast<SketchPlugin_SketchEntity>(feature());
  return aSketchEntity && aSketchEntity->isCopy();
}
