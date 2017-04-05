// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_MultiRotation.cpp
// Created:     04 Apr 2017
// Author:      Clarisse Genrault (CEA)

#include <FeaturesAPI_MultiRotation.h>

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

  theDumper << ")" << std::endl;
}

//==================================================================================================
MultiRotationPtr addMultiRotation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theMainObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Integer& theNumber)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesAPI_MultiRotation::ID());
  return MultiRotationPtr(new FeaturesAPI_MultiRotation(aFeature, theMainObjects,
                                                        theAxis, theNumber));
}

//==================================================================================================
MultiRotationPtr addMultiRotation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theMainObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Double& theStep,
                                  const ModelHighAPI_Integer& theNumber)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesAPI_MultiRotation::ID());
  return MultiRotationPtr(new FeaturesAPI_MultiRotation(aFeature, theMainObjects,
                                                        theAxis, theStep, theNumber));
}