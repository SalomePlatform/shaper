// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        CollectionAPI_Field.cpp
// Created:     16 Nov 2016
// Author:      Mikhail Ponikarov

#include "CollectionAPI_Field.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Integer.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>
#include <ModelHighAPI_ComponentValue.h>
#include <ModelAPI_AttributeTables.h>
#include <ModelAPI_AttributeStringArray.h>

#include <algorithm> // for std::transform

//=================================================================================================
CollectionAPI_Field::CollectionAPI_Field(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//=================================================================================================
CollectionAPI_Field::~CollectionAPI_Field()
{
}

//=================================================================================================
void CollectionAPI_Field::setSelection(const std::list<ModelHighAPI_Selection>& theFieldList)
{
  fillAttribute(theFieldList, myselection);
  execute();
}

//=================================================================================================
void CollectionAPI_Field::setComponentsNum(const ModelHighAPI_Integer& theNum)
{
  fillAttribute(theNum, mycomponentsNum);
  execute();
}

//=================================================================================================
void CollectionAPI_Field::setComponentsNames(const std::list<std::string>& theNames)
{
  fillAttribute(theNames, mycomponentsNames);
  execute();
}

//=================================================================================================
void CollectionAPI_Field::setValuesType(const std::string& theType)
{
  fillAttribute(int(valueTypeByStr(theType)), myvaluesType);
  execute();
}

//=================================================================================================
void CollectionAPI_Field::setStepsNum(const ModelHighAPI_Integer& theSteps)
{
  fillAttribute(theSteps, mystepsNum);
  execute();
}

//=================================================================================================
void CollectionAPI_Field::setStamps(const std::list<ModelHighAPI_Integer>& theStamps)
{
  fillAttribute(theStamps, mystamps);
  execute();
}

//=================================================================================================
void CollectionAPI_Field::addStep(const ModelHighAPI_Integer& theStepNum,
  const ModelHighAPI_Integer& theStamp,
  const std::list<std::list<ModelHighAPI_ComponentValue> >& theComponents)
{
  // set the table size to be sure the values are up to date
  myvalues->setSize(myselection->size() + 1 /* with defaults */,
    mycomponentsNum->value(), mystepsNum->value());

  // set values one by one
  int aRowIndex = 0;
  std::list<std::list<ModelHighAPI_ComponentValue> >::const_iterator 
    aRowsIter = theComponents.begin();
  for(; aRowsIter != theComponents.end(); aRowsIter++, aRowIndex++) {
    int aColIndex = 0;
    std::list<ModelHighAPI_ComponentValue>::const_iterator aColIter = aRowsIter->begin();
    for(; aColIter != aRowsIter->end(); aColIter++, aColIndex++) {
      aColIter->fill(myvalues, theStepNum.intValue(), aColIndex, aRowIndex);
    }
  }
  execute();
}

//=================================================================================================
void CollectionAPI_Field::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper<<aBase<<" = model.addField("<<aDocName<<", "<<mystepsNum<<", "
    <<strByValueType(ModelAPI_AttributeTables::ValueType(myvaluesType->value()))<<", "
    <<mycomponentsNum->value()<<", ";
  theDumper<<mycomponentsNames<<", ";
  theDumper<<myselection<<")"<<std::endl;
  // set values step by step
  for(int aStep = 0; aStep < myvalues->tables(); aStep++) {
    theDumper<<aBase<<".addStep("<<aStep<<", "<<mystamps->value(aStep)<<", [";
    for(int aRow = 0; aRow < myvalues->rows(); aRow++) {
      if (aRow != 0)
        theDumper<<", ";
      theDumper<<"[";
      for(int aCol = 0; aCol < myvalues->columns(); aCol++) {
        if (aCol != 0)
          theDumper<<", ";
        switch(myvalues->type()) {
        case ModelAPI_AttributeTables::BOOLEAN:
          theDumper<<myvalues->value(aRow, aCol, aStep).myBool;
          break;
        case ModelAPI_AttributeTables::INTEGER:
          theDumper<<myvalues->value(aRow, aCol, aStep).myInt;
          break;
        case ModelAPI_AttributeTables::DOUBLE:
          theDumper<<myvalues->value(aRow, aCol, aStep).myDouble;
          break;
        case ModelAPI_AttributeTables::STRING:
          theDumper<<myvalues->value(aRow, aCol, aStep).myStr;
          break;
        }
      }
      theDumper<<"]";
    }
    theDumper<<")"<<std::endl;
  }
}

//=================================================================================================
FieldPtr addField(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Integer& theStepsNum,
                  std::string& theComponentType,
                  const int theComponentsNum,
                  const std::list<std::string>& theComponentNames,
                  const std::list<ModelHighAPI_Selection>& theSelectionList)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(CollectionAPI_Field::ID());
  std::shared_ptr<CollectionAPI_Field> aField(new CollectionAPI_Field(aFeature));
  aField->setStepsNum(theStepsNum);
  aField->setValuesType(theComponentType);
  aField->setComponentsNum(theComponentsNum);
  aField->setComponentsNames(theComponentNames);
  aField->setSelection(theSelectionList);

  return aField;
}
