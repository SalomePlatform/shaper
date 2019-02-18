// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "FeaturesAPI_Pipe.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Pipe::FeaturesAPI_Pipe(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Pipe::FeaturesAPI_Pipe(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                   const ModelHighAPI_Selection& thePath)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByBasePath(theBaseObjects, thePath);
  }
}

//==================================================================================================
FeaturesAPI_Pipe::FeaturesAPI_Pipe(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                   const ModelHighAPI_Selection& thePath,
                                   const ModelHighAPI_Selection& theBiNoramal)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByBasePathBiNormal(theBaseObjects, thePath, theBiNoramal);
  }
}

//==================================================================================================
FeaturesAPI_Pipe::FeaturesAPI_Pipe(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                   const ModelHighAPI_Selection& thePath,
                                   const std::list<ModelHighAPI_Selection>& theLocations)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByBasePathLocations(theBaseObjects, thePath, theLocations);
  }
}

//==================================================================================================
FeaturesAPI_Pipe::~FeaturesAPI_Pipe()
{

}

//==================================================================================================
void FeaturesAPI_Pipe::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Pipe::setPath(const ModelHighAPI_Selection& thePath)
{
  fillAttribute(thePath, mypath);

  execute();
}

//==================================================================================================
void FeaturesAPI_Pipe::setByBasePath(const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                     const ModelHighAPI_Selection& thePath)
{
  fillAttribute(FeaturesPlugin_Pipe::CREATION_METHOD_SIMPLE(), mycreationMethod);
  fillAttribute(theBaseObjects, mybaseObjects);
  fillAttribute(thePath, mypath);

  execute();
}

//==================================================================================================
void FeaturesAPI_Pipe::setByBasePathBiNormal(
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Selection& thePath,
  const ModelHighAPI_Selection& theBiNoramal)
{
  fillAttribute(FeaturesPlugin_Pipe::CREATION_METHOD_BINORMAL(), mycreationMethod);
  fillAttribute(theBaseObjects, mybaseObjects);
  fillAttribute(thePath, mypath);
  fillAttribute(theBiNoramal, mybiNormal);

  execute();
}

//==================================================================================================
void FeaturesAPI_Pipe::setByBasePathLocations(
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const ModelHighAPI_Selection& thePath,
  const std::list<ModelHighAPI_Selection>& theLocations)
{
  fillAttribute(FeaturesPlugin_Pipe::CREATION_METHOD_LOCATIONS(), mycreationMethod);
  fillAttribute(theBaseObjects, mybaseObjects);
  fillAttribute(thePath, mypath);
  fillAttribute(theLocations, mylocations);

  execute();
}

//==================================================================================================
void FeaturesAPI_Pipe::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Pipe::BASE_OBJECTS_ID());
  AttributeSelectionPtr anAttrPath = aBase->selection(FeaturesPlugin_Pipe::PATH_OBJECT_ID());

  theDumper << aBase << " = model.addPipe(" << aDocName << ", "
            << anAttrObjects << ", " << anAttrPath;

  std::string aCreationMethod = aBase->string(FeaturesPlugin_Pipe::CREATION_METHOD())->value();

  if(aCreationMethod == FeaturesPlugin_Pipe::CREATION_METHOD_SIMPLE()) {
    // Do nothing;
  } else if(aCreationMethod == FeaturesPlugin_Pipe::CREATION_METHOD_BINORMAL()) {
    AttributeSelectionPtr anAttrBiNormal = aBase->selection(FeaturesPlugin_Pipe::BINORMAL_ID());

    theDumper << ", " << anAttrBiNormal;
  } else if(aCreationMethod == FeaturesPlugin_Pipe::CREATION_METHOD_LOCATIONS()) {
    AttributeSelectionListPtr anAttrLocations =
      aBase->selectionList(FeaturesPlugin_Pipe::LOCATIONS_ID());

    theDumper << ", " << anAttrLocations;
  }

  theDumper << ")" << std::endl;
}

//==================================================================================================
PipePtr addPipe(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                const ModelHighAPI_Selection& thePath)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Pipe::ID());
  return PipePtr(new FeaturesAPI_Pipe(aFeature, theBaseObjects, thePath));
}

//==================================================================================================
PipePtr addPipe(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                const ModelHighAPI_Selection& thePath,
                const ModelHighAPI_Selection& theBiNoramal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Pipe::ID());
  return PipePtr(new FeaturesAPI_Pipe(aFeature, theBaseObjects, thePath, theBiNoramal));
}

//==================================================================================================
PipePtr addPipe(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                const ModelHighAPI_Selection& thePath,
                const std::list<ModelHighAPI_Selection>& theLocations)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Pipe::ID());
  return PipePtr(new FeaturesAPI_Pipe(aFeature, theBaseObjects, thePath, theLocations));
}
