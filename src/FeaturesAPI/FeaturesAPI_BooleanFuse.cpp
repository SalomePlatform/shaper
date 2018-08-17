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

#include "FeaturesAPI_BooleanFuse.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_BooleanFuse::FeaturesAPI_BooleanFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_BooleanFuse::FeaturesAPI_BooleanFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects,
  const bool theRemoveEdges)
  : ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(FeaturesPlugin_BooleanFuse::CREATION_METHOD_SIMPLE(), mycreationMethod);
    fillAttribute(theMainObjects, mymainObjects);
    fillAttribute(theRemoveEdges, myremoveEdges);

    execute(false);
  }
}

//==================================================================================================
FeaturesAPI_BooleanFuse::FeaturesAPI_BooleanFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects,
  const std::list<ModelHighAPI_Selection>& theToolObjects,
  const bool theRemoveEdges)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(FeaturesPlugin_BooleanFuse::CREATION_METHOD_ADVANCED(), mycreationMethod);
    fillAttribute(theMainObjects, mymainObjects);
    fillAttribute(theToolObjects, mytoolObjects);
    fillAttribute(theRemoveEdges, myremoveEdges);

    execute(false);
  }
}

//==================================================================================================
FeaturesAPI_BooleanFuse::~FeaturesAPI_BooleanFuse()
{
}


//==================================================================================================
void FeaturesAPI_BooleanFuse::setMainObjects(
  const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mymainObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanFuse::setToolObjects(
  const std::list<ModelHighAPI_Selection>& theToolObjects)
{
  fillAttribute(theToolObjects, mytoolObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanFuse::setRemoveEdges(const bool theRemoveEdges)
{
  fillAttribute(theRemoveEdges, myremoveEdges);

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanFuse::setAdvancedMode(const bool theMode)
{
  if (theMode) {
    fillAttribute(FeaturesPlugin_BooleanFuse::CREATION_METHOD_ADVANCED(), mycreationMethod);
  } else {
    fillAttribute(FeaturesPlugin_BooleanFuse::CREATION_METHOD_SIMPLE(), mycreationMethod);
  }
}

//==================================================================================================
void FeaturesAPI_BooleanFuse::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();

  theDumper << aBase << " = model.addFuse";

  const std::string& aDocName = theDumper.name(aBase->document());
  AttributeStringPtr aMode = aBase->string(FeaturesPlugin_BooleanFuse::CREATION_METHOD());
  AttributeSelectionListPtr anObjects =
    aBase->selectionList(FeaturesPlugin_BooleanFuse::OBJECT_LIST_ID());
  AttributeSelectionListPtr aTools =
    aBase->selectionList(FeaturesPlugin_BooleanFuse::TOOL_LIST_ID());
  AttributeBooleanPtr aRemoveEdges =
    aBase->boolean(FeaturesPlugin_BooleanFuse::REMOVE_INTERSECTION_EDGES_ID());

  theDumper << "(" << aDocName << ", " << anObjects;

  if (aMode->value() == FeaturesPlugin_BooleanFuse::CREATION_METHOD_ADVANCED()) {
    theDumper << ", " << aTools;
  }

  if (aRemoveEdges->value()) {
    theDumper << ", " << true;
  }

  theDumper << ")" << std::endl;
}

//==================================================================================================
BooleanFusePtr addFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                       const std::list<ModelHighAPI_Selection>& theObjects,
                       const bool theRemoveEdges)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_BooleanFuse::ID());
  return BooleanFusePtr(new FeaturesAPI_BooleanFuse(aFeature,
                                                    theObjects,
                                                    theRemoveEdges));
}

//==================================================================================================
BooleanFusePtr addFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                   const std::list<ModelHighAPI_Selection>& theMainObjects,
                   const std::list<ModelHighAPI_Selection>& theToolObjects,
                   const bool theRemoveEdges)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_BooleanFuse::ID());
  return BooleanFusePtr(new FeaturesAPI_BooleanFuse(aFeature,
                                                    theMainObjects,
                                                    theToolObjects,
                                                    theRemoveEdges));
}
