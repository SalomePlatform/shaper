// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "CollectionAPI_Field.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Integer.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>
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
void CollectionAPI_Field::setComponentsNames(const std::list<std::string>& theNames)
{
  fillAttribute(theNames, mycomponentsNames);
  execute();
}

//=================================================================================================
void CollectionAPI_Field::setValuesType(const std::string& theType)
{
  myvalues->setType(valueTypeByStr(theType));
  execute();
}

//=================================================================================================
void CollectionAPI_Field::setStepsNum(const ModelHighAPI_Integer& theSteps)
{
  mystamps->setSize(theSteps.intValue());
  execute();
}

//=================================================================================================
void CollectionAPI_Field::setStamps(const std::list<ModelHighAPI_Integer>& theStamps)
{
  fillAttribute(theStamps, mystamps);
  execute();
}

//=================================================================================================
#define addStepImplementation(type, fieldType, type2, fieldType2, type3, fieldType3) \
void CollectionAPI_Field::addStep(const ModelHighAPI_Integer& theStepNum, \
  const ModelHighAPI_Integer& theStamp, \
  const std::list<std::list<type> >& theComponents) \
{ \
  mystamps->setValue(theStepNum.intValue(), theStamp.intValue()); \
  int aRowIndex = 0; \
  std::list<std::list<type> >::const_iterator \
    aRowsIter = theComponents.begin(); \
  for(; aRowsIter != theComponents.end(); aRowsIter++, aRowIndex++) { \
    int aColIndex = 0; \
    std::list<type>::const_iterator aColIter = aRowsIter->begin(); \
    for(; aColIter != aRowsIter->end(); aColIter++, aColIndex++) { \
      ModelAPI_AttributeTables::Value aVal; \
      aVal.fieldType = *aColIter; \
      aVal.fieldType2 = type2(*aColIter); \
      aVal.fieldType3 = type3(*aColIter); \
      myvalues->setValue(aVal, aRowIndex, aColIndex, theStepNum.intValue()); \
    } \
  } \
  execute(); \
}

addStepImplementation(double, myDouble, int, myInt, bool, myBool);
addStepImplementation(int, myInt, double, myDouble, bool, myBool);
addStepImplementation(bool, myBool, int, myInt, double, myDouble);
addStepImplementation(std::string, myStr, std::string, myStr, std::string, myStr);

//=================================================================================================
void CollectionAPI_Field::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper<<aBase<<" = model.addField("<<aDocName<<", "<<myvalues->tables()<<", \""
    <<strByValueType(myvalues->type())<<"\", "<<mycomponentsNames->size()<<", ";
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
          theDumper<<'"'<<myvalues->value(aRow, aCol, aStep).myStr<<'"';
          break;
        }
      }
      theDumper<<"]";
    }
    theDumper<<"])"<<std::endl;
  }
}

std::shared_ptr<ModelAPI_AttributeTables> CollectionAPI_Field::tableValues()
{
  return myvalues;
}

//=================================================================================================
FieldPtr addField(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Integer& theStepsNum,
                  const std::string& theComponentType,
                  const int theComponentsNum,
                  const std::list<std::string>& theComponentNames,
                  const std::list<ModelHighAPI_Selection>& theSelectionList)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(CollectionAPI_Field::ID());
  std::shared_ptr<CollectionAPI_Field> aField(new CollectionAPI_Field(aFeature));
  aField->setStepsNum(theStepsNum);
  aField->setValuesType(theComponentType);
  aField->setComponentsNames(theComponentNames);
  aField->setSelection(theSelectionList);
  aField->tableValues()->setSize(
    int(theSelectionList.size() + 1), theComponentsNum, theStepsNum.intValue());

  return aField;
}
