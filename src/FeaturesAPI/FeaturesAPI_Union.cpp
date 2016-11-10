// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Union.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_Union.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//================================================================================================
FeaturesAPI_Union::FeaturesAPI_Union(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//================================================================================================
FeaturesAPI_Union::FeaturesAPI_Union(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theBaseObjects)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setBase(theBaseObjects);
  }
}

//================================================================================================
FeaturesAPI_Union::~FeaturesAPI_Union()
{

}

//==================================================================================================
void FeaturesAPI_Union::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Union::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Union::BASE_OBJECTS_ID());

  theDumper << aBase << " = model.addUnion(" << aDocName <<
    ", " << anAttrObjects << ")" << std::endl;
}

//==================================================================================================
UnionPtr addUnion(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Union::ID());
  return UnionPtr(new FeaturesAPI_Union(aFeature, theBaseObjects));
}
