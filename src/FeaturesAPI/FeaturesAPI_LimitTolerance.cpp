// Copyright (C) 2014-2025  CEA, EDF
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

#include "FeaturesAPI_LimitTolerance.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_LimitTolerance::FeaturesAPI_LimitTolerance(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_LimitTolerance::FeaturesAPI_LimitTolerance(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                       const ModelHighAPI_Selection& theMainObject,
                                                       const ModelHighAPI_Double& theTolerance,
                                                       const bool theExactCheck)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
  {
    setMainObject(theMainObject);
    setTolerance(theTolerance);
    setExactCheck(theExactCheck);
  }
}

//==================================================================================================
FeaturesAPI_LimitTolerance::~FeaturesAPI_LimitTolerance()
{
}

//==================================================================================================
void FeaturesAPI_LimitTolerance::setMainObject(const ModelHighAPI_Selection& theMainObject)
{
  fillAttribute(theMainObject, mainObject());
  execute();
}

//==================================================================================================
void FeaturesAPI_LimitTolerance::setTolerance(const ModelHighAPI_Double& theTolerance)
{
  fillAttribute(theTolerance, tolerance());
  execute();
}

//==================================================================================================
void FeaturesAPI_LimitTolerance::setExactCheck(const bool theExactCheck)
{
  fillAttribute(theExactCheck, exactCheck());
  execute();
}

//==================================================================================================
void FeaturesAPI_LimitTolerance::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionPtr anAttrObject = aBase->selection(FeaturesPlugin_LimitTolerance::OBJECT_ID());
  theDumper << aBase << " = model.addLimitTolerance(" << aDocName << ", " << anAttrObject;

  AttributeDoublePtr anAttrTolerance = aBase->real(FeaturesPlugin_LimitTolerance::TOLERANCE_ID());
  theDumper << ", " << anAttrTolerance;

  AttributeBooleanPtr anAttrExact = aBase->boolean(FeaturesPlugin_LimitTolerance::EXACT_CHECK_ID());
  theDumper << ", " << anAttrExact;

  theDumper << ")" << std::endl;
}

//==================================================================================================
LimitTolerancePtr addLimitTolerance(const std::shared_ptr<ModelAPI_Document>& thePart,
                                    const ModelHighAPI_Selection& theMainObject,
                                    const ModelHighAPI_Double& theTolerance,
                                    const bool theExactCheck)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_LimitTolerance::ID());
  LimitTolerancePtr aLimitTolerance;
  aLimitTolerance.reset(new FeaturesAPI_LimitTolerance(aFeature, theMainObject, theTolerance, theExactCheck));
  return aLimitTolerance;
}
