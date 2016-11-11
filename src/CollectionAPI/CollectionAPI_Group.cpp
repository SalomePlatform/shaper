// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        CollectionAPI_Group.cpp
// Created:     07 June 2016
// Author:      Dmitry Bobylev

#include "CollectionAPI_Group.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Integer.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
CollectionAPI_Group::CollectionAPI_Group(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
CollectionAPI_Group::CollectionAPI_Group(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theGroupList)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setGroupList(theGroupList);
  }
}

//==================================================================================================
CollectionAPI_Group::~CollectionAPI_Group()
{

}

//==================================================================================================
void CollectionAPI_Group::setGroupList(const std::list<ModelHighAPI_Selection>& theGroupList)
{
  fillAttribute(theGroupList, mygroupList);

  execute();
}

//==================================================================================================
void CollectionAPI_Group::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrList = aBase->selectionList(CollectionPlugin_Group::LIST_ID());

  theDumper << aBase << " = model.addGroup(" << aDocName << ", " << anAttrList << ")" << std::endl;
}

//==================================================================================================
GroupPtr addGroup(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theGroupList)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(CollectionAPI_Group::ID());
  return GroupPtr(new CollectionAPI_Group(aFeature, theGroupList));
}
