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

#include "FeaturesAPI_Boolean.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Integer.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Boolean::FeaturesAPI_Boolean(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Boolean::FeaturesAPI_Boolean(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                         const ModelHighAPI_Integer& theBoolType,
                                         const std::list<ModelHighAPI_Selection>& theMainObjects,
                                         const std::list<ModelHighAPI_Selection>& theToolObjects)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBoolType, myboolType);
    fillAttribute(theMainObjects, mymainObjects);
    fillAttribute(theToolObjects, mytoolObjects);

    execute(false);
  }
}

//==================================================================================================
FeaturesAPI_Boolean::~FeaturesAPI_Boolean()
{

}

//==================================================================================================
void FeaturesAPI_Boolean::setBoolType(const ModelHighAPI_Integer& theBoolType)
{
  fillAttribute(theBoolType, myboolType);

  execute();
}

//==================================================================================================
void FeaturesAPI_Boolean::setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mymainObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Boolean::setToolObjects(const std::list<ModelHighAPI_Selection>& theToolObjects)
{
  fillAttribute(theToolObjects, mytoolObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Boolean::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();

  FeaturesPlugin_Boolean::OperationType aType =
      (FeaturesPlugin_Boolean::OperationType)aBase->integer(
      FeaturesPlugin_Boolean::TYPE_ID())->value();

  theDumper << aBase << " = model.add";

  switch(aType) {
    case FeaturesPlugin_Boolean::BOOL_CUT:    theDumper << "Cut";    break;
    case FeaturesPlugin_Boolean::BOOL_FUSE:   theDumper << "Fuse";   break;
    case FeaturesPlugin_Boolean::BOOL_COMMON: theDumper << "Common"; break;
    case FeaturesPlugin_Boolean::BOOL_FILL:   theDumper << "Fill";   break;
    case FeaturesPlugin_Boolean::BOOL_SMASH:  theDumper << "Smash";  break;
  }

  const std::string& aDocName = theDumper.name(aBase->document());
  AttributeSelectionListPtr anObjects =
    aBase->selectionList(FeaturesPlugin_Boolean::OBJECT_LIST_ID());
  AttributeSelectionListPtr aTools = aBase->selectionList(FeaturesPlugin_Boolean::TOOL_LIST_ID());

  theDumper << "(" << aDocName << ", " << anObjects << ", " << aTools << ")" << std::endl;
}

//==================================================================================================
BooleanPtr addCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theMainObjects,
                  const std::list<ModelHighAPI_Selection>& theToolObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Boolean::ID());
  return BooleanPtr(new FeaturesAPI_Boolean(aFeature,
                                            FeaturesPlugin_Boolean::BOOL_CUT,
                                            theMainObjects,
                                            theToolObjects));
}

//==================================================================================================
BooleanPtr addFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                   const std::list<ModelHighAPI_Selection>& theObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Boolean::ID());
  std::list<ModelHighAPI_Selection> aToolObjects;
  return BooleanPtr(new FeaturesAPI_Boolean(aFeature,
                                            FeaturesPlugin_Boolean::BOOL_FUSE,
                                            theObjects,
                                            aToolObjects));
}

//==================================================================================================
BooleanPtr addFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                   const std::list<ModelHighAPI_Selection>& theMainObjects,
                   const std::list<ModelHighAPI_Selection>& theToolObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Boolean::ID());
  return BooleanPtr(new FeaturesAPI_Boolean(aFeature,
                                            FeaturesPlugin_Boolean::BOOL_FUSE,
                                            theMainObjects,
                                            theToolObjects));
}

//==================================================================================================
BooleanPtr addCommon(const std::shared_ptr<ModelAPI_Document>& thePart,
                     const std::list<ModelHighAPI_Selection>& theMainObjects,
                     const std::list<ModelHighAPI_Selection>& theToolObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Boolean::ID());
  return BooleanPtr(new FeaturesAPI_Boolean(aFeature,
                                            FeaturesPlugin_Boolean::BOOL_COMMON,
                                            theMainObjects,
                                            theToolObjects));
}

//==================================================================================================
BooleanPtr addSmash(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const std::list<ModelHighAPI_Selection>& theMainObjects,
                    const std::list<ModelHighAPI_Selection>& theToolObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Boolean::ID());
  return BooleanPtr(new FeaturesAPI_Boolean(aFeature,
                                            FeaturesPlugin_Boolean::BOOL_SMASH,
                                            theMainObjects,
                                            theToolObjects));
}

//==================================================================================================
BooleanPtr addFill(const std::shared_ptr<ModelAPI_Document>& thePart,
                   const std::list<ModelHighAPI_Selection>& theMainObjects,
                   const std::list<ModelHighAPI_Selection>& theToolObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Boolean::ID());
  return BooleanPtr(new FeaturesAPI_Boolean(aFeature,
                                            FeaturesPlugin_Boolean::BOOL_FILL,
                                            theMainObjects,
                                            theToolObjects));
}
