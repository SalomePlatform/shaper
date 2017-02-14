// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_MultiTranslation.cpp
// Created:     09 Feb 2017
// Author:      Clarisse Genrault (CEA)

#include <FeaturesAPI_MultiTranslation.h>

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_MultiTranslation::FeaturesAPI_MultiTranslation(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_MultiTranslation::FeaturesAPI_MultiTranslation(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects,
  const ModelHighAPI_Selection& theAxisObject,
  const ModelHighAPI_Double& theStep,
  const ModelHighAPI_Integer& theNumber)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mainObjects());
    fillAttribute(theAxisObject, firstAxisObject());
    fillAttribute(theStep, firstStep());
    fillAttribute("",useSecondDir());
    setFirstNumber(theNumber);
  }
}

//==================================================================================================
FeaturesAPI_MultiTranslation::FeaturesAPI_MultiTranslation(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects,
  const ModelHighAPI_Selection& theFirstAxisObject,
  const ModelHighAPI_Double& theFirstStep,
  const ModelHighAPI_Integer& theFirstNumber,
  const ModelHighAPI_Selection& theSecondAxisObject,
  const ModelHighAPI_Double& theSecondStep,
  const ModelHighAPI_Integer& theSecondNumber)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mainObjects());
    fillAttribute(theFirstAxisObject, firstAxisObject());
    fillAttribute(theFirstStep, firstStep());
    fillAttribute(theFirstNumber, firstNumber());
    fillAttribute(theSecondAxisObject, secondAxisObject());
    fillAttribute(theSecondStep, secondStep());
    fillAttribute("true",useSecondDir());
    setSecondNumber(theSecondNumber);
  }
}

//==================================================================================================
FeaturesAPI_MultiTranslation::~FeaturesAPI_MultiTranslation()
{
}

//==================================================================================================
void FeaturesAPI_MultiTranslation::setMainObjects(
  const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mainObjects());

  execute();
}

//==================================================================================================
void FeaturesAPI_MultiTranslation::setFirstAxisAndDistance(
  const ModelHighAPI_Selection& theAxisObject,
  const ModelHighAPI_Double& theDistance)
{
  fillAttribute(theAxisObject, firstAxisObject());
  fillAttribute(theDistance, firstStep());

  execute();
}

//==================================================================================================
void FeaturesAPI_MultiTranslation::setSecondAxisAndDistance(
  const ModelHighAPI_Selection& theAxisObject,
  const ModelHighAPI_Double& theDistance)
{
  fillAttribute(theAxisObject, secondAxisObject());
  fillAttribute(theDistance, secondStep());

  execute();
}

//==================================================================================================
void FeaturesAPI_MultiTranslation::setFirstNumber(const ModelHighAPI_Integer& theFirstNumber)
{
  fillAttribute(theFirstNumber, firstNumber());

  execute();
}

//==================================================================================================
void FeaturesAPI_MultiTranslation::setSecondNumber(const ModelHighAPI_Integer& theSecondNumber)
{
  fillAttribute(theSecondNumber, secondNumber());

  execute();
}

//==================================================================================================
void FeaturesAPI_MultiTranslation::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_MultiTranslation::OBJECTS_LIST_ID());
  theDumper << aBase << " = model.addMultiTranslation(" << aDocName << ", " << anAttrObjects;
  
  AttributeSelectionPtr anAttrFirstAxis =
    aBase->selection(FeaturesPlugin_MultiTranslation::AXIS_FIRST_DIR_ID());
  AttributeDoublePtr anAttrFirstStep =
    aBase->real(FeaturesPlugin_MultiTranslation::STEP_FIRST_DIR_ID());
  AttributeIntegerPtr anAttrFirstNumber =
    aBase->integer(FeaturesPlugin_MultiTranslation::NB_COPIES_FIRST_DIR_ID());
  theDumper << ", " << anAttrFirstAxis << ", " << anAttrFirstStep;
  theDumper << ", " << anAttrFirstNumber;
      
  if (aBase->string(FeaturesPlugin_MultiTranslation::USE_SECOND_DIR_ID())->isInitialized()
      && !aBase->string(FeaturesPlugin_MultiTranslation::USE_SECOND_DIR_ID())->value().empty()) {
    AttributeSelectionPtr anAttrSecondAxis =
      aBase->selection(FeaturesPlugin_MultiTranslation::AXIS_SECOND_DIR_ID());
    AttributeDoublePtr anAttrSecondStep =
      aBase->real(FeaturesPlugin_MultiTranslation::STEP_SECOND_DIR_ID());
    AttributeIntegerPtr anAttrSecondNumber =
      aBase->integer(FeaturesPlugin_MultiTranslation::NB_COPIES_SECOND_DIR_ID());
    theDumper << ", " << anAttrSecondAxis << ", " << anAttrSecondStep;
    theDumper << ", " << anAttrSecondStep;
  }

  theDumper << ")" << std::endl;
}

//==================================================================================================
MultiTranslationPtr addMultiTranslation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                        const std::list<ModelHighAPI_Selection>& theMainObjects,
                                        const ModelHighAPI_Selection& theAxisObject,
                                        const ModelHighAPI_Double& theStep,
                                        const ModelHighAPI_Integer& theNumber)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesAPI_MultiTranslation::ID());
  return MultiTranslationPtr(new FeaturesAPI_MultiTranslation(aFeature, theMainObjects,
                                                              theAxisObject, theStep, theNumber));
}

//==================================================================================================
MultiTranslationPtr addMultiTranslation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                        const std::list<ModelHighAPI_Selection>& theMainObjects,
                                        const ModelHighAPI_Selection& theFirstAxisObject,
                                        const ModelHighAPI_Double& theFirstStep,
                                        const ModelHighAPI_Integer& theFirstNumber,
                                        const ModelHighAPI_Selection& theSecondAxisObject,
                                        const ModelHighAPI_Double& theSecondStep,
                                        const ModelHighAPI_Integer& theSecondNumber)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesAPI_MultiTranslation::ID());
  return MultiTranslationPtr(new FeaturesAPI_MultiTranslation(aFeature, theMainObjects,
                                                              theFirstAxisObject, theFirstStep,
                                                              theFirstNumber,
                                                              theSecondAxisObject, theSecondStep,
                                                              theSecondNumber));
}
