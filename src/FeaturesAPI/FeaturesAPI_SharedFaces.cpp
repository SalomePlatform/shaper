// Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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

#include "FeaturesAPI_SharedFaces.h"

#include <FeaturesPlugin_GroupSharedFaces.h>

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Services.h>
#include <ModelHighAPI_Tools.h>

//=================================================================================================
FeaturesAPI_SharedFaces::FeaturesAPI_SharedFaces(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//=================================================================================================
FeaturesAPI_SharedFaces::FeaturesAPI_SharedFaces(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const ModelHighAPI_Selection& theObject,
  const std::string & theNameGroup)
:ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theObject, myobjectselected);
    fillAttribute(theNameGroup, mygroupname);
    execute();
  }
}

//=================================================================================================
FeaturesAPI_SharedFaces::~FeaturesAPI_SharedFaces()
{
}

//=================================================================================================
void FeaturesAPI_SharedFaces::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionPtr anAttrObject;
  anAttrObject = aBase->selection(FeaturesPlugin_GroupSharedFaces::OBJECT_ID());

  theDumper << aBase << " = model.getSharedFaces(" << aDocName << ", " << anAttrObject;
  theDumper << ", " << aBase->string(FeaturesPlugin_GroupSharedFaces::GROUP_NAME_ID());
  theDumper << ")" << std::endl;
}

//=================================================================================================
SharedFacesPtr getSharedFaces(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const ModelHighAPI_Selection& theObject,
                              const std::string & theNameGroup)
{
  FeaturePtr aFeature = thePart->addFeature(FeaturesPlugin_GroupSharedFaces::ID());
  SharedFacesPtr aSharedFaces;

  aSharedFaces.reset(new FeaturesAPI_SharedFaces(aFeature,
                                                 theObject,
                                                 theNameGroup));
  return aSharedFaces;
}

