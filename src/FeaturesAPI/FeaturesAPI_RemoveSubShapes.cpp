// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_RemoveSubShapes.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_RemoveSubShapes.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_RemoveSubShapes::FeaturesAPI_RemoveSubShapes(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_RemoveSubShapes::FeaturesAPI_RemoveSubShapes(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const ModelHighAPI_Selection& theBase)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setBase(theBase);

    execute();
  }
}

//==================================================================================================
FeaturesAPI_RemoveSubShapes::~FeaturesAPI_RemoveSubShapes()
{

}

//==================================================================================================
void FeaturesAPI_RemoveSubShapes::setBase(const ModelHighAPI_Selection& theBase)
{
  fillAttribute(theBase, mybase);

  execute();
}

//==================================================================================================
void FeaturesAPI_RemoveSubShapes::setSubShapesToKeep(
  const std::list<ModelHighAPI_Selection>& theSubShapes)
{
  fillAttribute(theSubShapes, mysubshapes);

  execute();
}

//==================================================================================================
void FeaturesAPI_RemoveSubShapes::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionPtr anAttrBaseShape =
    aBase->selection(FeaturesPlugin_RemoveSubShapes::BASE_SHAPE_ID());
  AttributeSelectionListPtr anAttrSubShapes =
    aBase->selectionList(FeaturesPlugin_RemoveSubShapes::SUBSHAPES_ID());

  theDumper << aBase << " = model.addRemoveSubShapes(" << aDocName <<
    ", " << anAttrBaseShape << ")" << std::endl;
  theDumper << aBase << ".setSubShapesToKeep(" << anAttrSubShapes << ")" << std::endl;
}

//==================================================================================================
RemoveSubShapesPtr addRemoveSubShapes(const std::shared_ptr<ModelAPI_Document>& thePart,
                                      const ModelHighAPI_Selection& theBase)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesAPI_RemoveSubShapes::ID());
  return RemoveSubShapesPtr(new FeaturesAPI_RemoveSubShapes(aFeature, theBase));
}
