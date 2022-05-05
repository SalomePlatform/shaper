// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "FeaturesAPI_Copy.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//================================================================================================
FeaturesAPI_Copy::FeaturesAPI_Copy(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//================================================================================================
FeaturesAPI_Copy::FeaturesAPI_Copy(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theObjects,
                                     const int theNumber)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setNumber(theNumber);
    setObjects(theObjects);
  }
}

//================================================================================================
FeaturesAPI_Copy::~FeaturesAPI_Copy() {}

//=================================================================================================
void FeaturesAPI_Copy::setObjects(const std::list<ModelHighAPI_Selection>& theObjects)
{
  fillAttribute(theObjects, myobjects);
  execute();
}
//=================================================================================================
void FeaturesAPI_Copy::setNumber(const int theNumber)
{
  fillAttribute(theNumber, mynumber);
  execute();
}

//=================================================================================================
void FeaturesAPI_Copy::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anObjects = aBase->selectionList(FeaturesPlugin_Copy::OBJECTS());
  AttributeIntegerPtr aNumber = aBase->integer(FeaturesPlugin_Copy::NUMBER());

  theDumper << aBase << " = model.addCopy("
            << aDocName << ", " << anObjects << ", " << aNumber << ")" << std::endl;
}

//=================================================================================================
CopyPtr addCopy(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theObjects,
                const int theNumber)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Copy::ID());
  return CopyPtr(new FeaturesAPI_Copy(aFeature, theObjects, theNumber));
}
