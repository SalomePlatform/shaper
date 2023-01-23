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

#include "FeaturesAPI_GlueFaces.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_GlueFaces::FeaturesAPI_GlueFaces(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_GlueFaces::FeaturesAPI_GlueFaces(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<ModelHighAPI_Selection>& theMainObjects,
                                             const ModelHighAPI_Double& theTolerance,
                                             bool theIsKeepNonSolids)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theMainObjects, mainObjects());
    setTolerance(theTolerance);
    setKeepNonSolids(theIsKeepNonSolids);
  }
}

//==================================================================================================
FeaturesAPI_GlueFaces::~FeaturesAPI_GlueFaces()
{
}

//==================================================================================================
void FeaturesAPI_GlueFaces::setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mainObjects());
  execute();
}

//==================================================================================================
void FeaturesAPI_GlueFaces::setTolerance(const ModelHighAPI_Double& theTolerance)
{
  fillAttribute(theTolerance, tolerance());
  execute();
}

//==================================================================================================
void FeaturesAPI_GlueFaces::setKeepNonSolids(bool theFlag)
{
  fillAttribute(theFlag, keepNonSolids());
  execute();
}

//==================================================================================================
void FeaturesAPI_GlueFaces::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_GlueFaces::OBJECTS_LIST_ID());
  theDumper << aBase << " = model.addGlueFaces(" << aDocName << ", " << anAttrObjects;

  AttributeDoublePtr anAttrTolerance = aBase->real(FeaturesPlugin_GlueFaces::TOLERANCE_ID());
  theDumper << ", " << anAttrTolerance;

  AttributeBooleanPtr anAttrKeepNonSolids = aBase->boolean(FeaturesPlugin_GlueFaces::KEEP_NON_SOLIDS_ID());
  theDumper << ", " << anAttrKeepNonSolids;

  theDumper << ")" << std::endl;
}

//==================================================================================================
GlueFacesPtr addGlueFaces(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theMainObjects,
                          const ModelHighAPI_Double& theTolerance,
                          const bool theKeepNonSolids)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_GlueFaces::ID());
  GlueFacesPtr aGlueFaces;
  aGlueFaces.reset(new FeaturesAPI_GlueFaces(aFeature, theMainObjects, theTolerance, theKeepNonSolids));
  return aGlueFaces;
}
