// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
    fillAttribute(FeaturesPlugin_MultiTranslation::USE_SECOND_DIR_ID(),useSecondDir());
    fillAttribute(theSecondAxisObject, secondAxisObject());
    fillAttribute(theSecondStep, secondStep());
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
    theDumper << ", " << anAttrSecondNumber;
  }

  if (!aBase->data()->version().empty())
    theDumper << ", keepSubResults = True";

  theDumper << ")" << std::endl;
}

//==================================================================================================
MultiTranslationPtr addMultiTranslation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                        const std::list<ModelHighAPI_Selection>& theMainObjects,
                                        const ModelHighAPI_Selection& theFirstAxisObject,
                                        const ModelHighAPI_Double& theFirstStep,
                                        const ModelHighAPI_Integer& theFirstNumber,
                                        const ModelHighAPI_Selection& theSecondAxisObject,
                                        const ModelHighAPI_Double& theSecondStep,
                                        const ModelHighAPI_Integer& theSecondNumber,
                                        const bool keepSubResults)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesAPI_MultiTranslation::ID());
  if (!keepSubResults)
    aFeature->data()->setVersion("");
  MultiTranslationPtr aMT;
  if (theSecondAxisObject.variantType() == ModelHighAPI_Selection::VT_Empty) {
    aMT.reset(new FeaturesAPI_MultiTranslation(aFeature, theMainObjects,
        theFirstAxisObject, theFirstStep, theFirstNumber));
  }
  else {
    aMT.reset(new FeaturesAPI_MultiTranslation(aFeature, theMainObjects,
        theFirstAxisObject, theFirstStep, theFirstNumber,
        theSecondAxisObject, theSecondStep, theSecondNumber));
  }
  return aMT;
}
