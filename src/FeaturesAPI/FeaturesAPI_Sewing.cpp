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

#include "FeaturesAPI_Sewing.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Sewing::FeaturesAPI_Sewing(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Sewing::FeaturesAPI_Sewing(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const std::list<ModelHighAPI_Selection>& theMainObjects,
                                       const ModelHighAPI_Double& theTolerance,
                                       bool theIsAllowNonManifold/*=false*/,
                                       bool theIsAlwaysCreateResult/*=true*/)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theMainObjects, mainObjects());
    setTolerance(theTolerance);
    setIsAllowNonManifold(theIsAllowNonManifold);
    setIsAlwaysCreateResult(theIsAlwaysCreateResult);
  }
}

//==================================================================================================
FeaturesAPI_Sewing::~FeaturesAPI_Sewing()
{
}

//==================================================================================================
void FeaturesAPI_Sewing::setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mainObjects());
  execute();
}

//==================================================================================================
void FeaturesAPI_Sewing::setTolerance(const ModelHighAPI_Double& theTolerance)
{
  fillAttribute(theTolerance, tolerance());
  execute();
}

//==================================================================================================
void FeaturesAPI_Sewing::setIsAllowNonManifold(bool theFlag)
{
  fillAttribute(theFlag, isAllowNonManifold());
  execute();
}

//==================================================================================================
void FeaturesAPI_Sewing::setIsAlwaysCreateResult(bool theFlag)
{
  fillAttribute(theFlag, isAlwaysCreateResult());
  execute();
}

//==================================================================================================
void FeaturesAPI_Sewing::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Sewing::OBJECTS_LIST_ID());
  theDumper << aBase << " = model.addSewing(" << aDocName << ", " << anAttrObjects;

  AttributeDoublePtr anAttrTolerance = aBase->real(FeaturesPlugin_Sewing::TOLERANCE_ID());
  theDumper << ", " << anAttrTolerance;

  AttributeBooleanPtr anAttrAllowNonMFold = aBase->boolean(FeaturesPlugin_Sewing::ALLOW_NON_MANIFOLD_ID());
  theDumper << ", " << anAttrAllowNonMFold;

  AttributeBooleanPtr anAttrAlwaysResult = aBase->boolean(FeaturesPlugin_Sewing::ALWAYS_CREATE_RESULT_ID());
  theDumper << ", " << anAttrAlwaysResult;

  theDumper << ")" << std::endl;
}

//==================================================================================================
SewingPtr addSewing(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const std::list<ModelHighAPI_Selection>& theMainObjects,
                    const ModelHighAPI_Double& theTolerance,
                    const bool theIsAllowNonManifold,
                    const bool theIsAlwaysCreateResult)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Sewing::ID());
  SewingPtr aSewing;
  aSewing.reset(new FeaturesAPI_Sewing(aFeature, theMainObjects, theTolerance, theIsAllowNonManifold, theIsAlwaysCreateResult));
  return aSewing;
}
