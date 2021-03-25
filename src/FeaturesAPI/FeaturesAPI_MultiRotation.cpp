// Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

// File:        FeaturesAPI_MultiRotation.cpp
// Created:     04 Apr 2017
// Author:      Clarisse Genrault (CEA)

#include <FeaturesAPI_MultiRotation.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_MultiRotation::FeaturesAPI_MultiRotation(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}


//==================================================================================================
FeaturesAPI_MultiRotation::FeaturesAPI_MultiRotation(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects,
  const ModelHighAPI_Selection& theAxis,
  const ModelHighAPI_Integer& theNumber)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mainObjects());
    fillAttribute(theAxis, axisAngular());
    fillAttribute("",useStepAngular());
    setNumberAngular(theNumber);
  }
}

//==================================================================================================
FeaturesAPI_MultiRotation::FeaturesAPI_MultiRotation(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects,
  const ModelHighAPI_Selection& theAxis,
  const ModelHighAPI_Double& theStep,
  const ModelHighAPI_Integer& theNumber)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mainObjects());
    fillAttribute(theAxis, axisAngular());
    fillAttribute(FeaturesPlugin_MultiRotation::USE_ANGULAR_STEP_ID(),useStepAngular());
    fillAttribute(theStep, stepAngular());
    setNumberAngular(theNumber);
  }
}

//==================================================================================================
FeaturesAPI_MultiRotation::~FeaturesAPI_MultiRotation()
{
}

//==================================================================================================
void FeaturesAPI_MultiRotation::setMainObjects(
  const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mainObjects());

  execute();
}

//==================================================================================================
void FeaturesAPI_MultiRotation::setAxisAngular(const ModelHighAPI_Selection& theAxis)
{
  fillAttribute(theAxis, axisAngular());

  execute();
}

//==================================================================================================
void FeaturesAPI_MultiRotation::setStepAngular(const ModelHighAPI_Double& theStep)
{
  fillAttribute(theStep, stepAngular());

  execute();
}

//==================================================================================================
void FeaturesAPI_MultiRotation::setNumberAngular(const ModelHighAPI_Integer& theNumber)
{
  fillAttribute(theNumber, nbAngular());

  execute();
}

//==================================================================================================
void FeaturesAPI_MultiRotation::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_MultiRotation::OBJECTS_LIST_ID());
  theDumper << aBase << " = model.addMultiRotation(" << aDocName << ", " << anAttrObjects;

  AttributeSelectionPtr anAttrAxis =
    aBase->selection(FeaturesPlugin_MultiRotation::AXIS_ANGULAR_ID());
  theDumper << ", " << anAttrAxis;

  if (aBase->string(FeaturesPlugin_MultiRotation::USE_ANGULAR_STEP_ID())->isInitialized()
      && !aBase->string(FeaturesPlugin_MultiRotation::USE_ANGULAR_STEP_ID())->value().empty()) {
    AttributeDoublePtr anAttrStepAngular =
      aBase->real(FeaturesPlugin_MultiRotation::STEP_ANGULAR_ID());
      theDumper << ", " << anAttrStepAngular;
  }

  AttributeIntegerPtr anAttrNumberAngular =
    aBase->integer(FeaturesPlugin_MultiRotation::NB_COPIES_ANGULAR_ID());
  theDumper << ", " << anAttrNumberAngular;

  if (!aBase->data()->version().empty())
    theDumper << ", keepSubResults = True";

  theDumper << ")" << std::endl;
}

//==================================================================================================
MultiRotationPtr addMultiRotation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theMainObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Double& theStep,
                                  const ModelHighAPI_Integer& theNumber,
                                  const bool keepSubResults)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesAPI_MultiRotation::ID());
  if (!keepSubResults)
    aFeature->data()->setVersion("");

  MultiRotationPtr aResult;
  if (theNumber.string().empty()) {
    // rotate for the whole circle
    double aStepVal = theStep.value();
    std::wstring aStepStr = theStep.string();
    std::wostringstream aStepValAsStr;
    aStepValAsStr << aStepVal;

    ModelHighAPI_Integer aNumber = aStepStr == aStepValAsStr.str()
                                 ? ModelHighAPI_Integer((int)aStepVal)
                                 : ModelHighAPI_Integer(aStepStr);
    aResult.reset(new FeaturesAPI_MultiRotation(aFeature, theMainObjects, theAxis, aNumber));
  }
  else {
    aResult.reset(new FeaturesAPI_MultiRotation(aFeature, theMainObjects,
                                                theAxis, theStep, theNumber));
  }
  return aResult;
}
