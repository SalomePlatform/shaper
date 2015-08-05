// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Data.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Model_Data.h>
#include <Model_AttributeDocRef.h>
#include <Model_AttributeInteger.h>
#include <Model_AttributeDouble.h>
#include <Model_AttributeReference.h>
#include <Model_AttributeRefAttr.h>
#include <Model_AttributeRefList.h>
#include <Model_AttributeBoolean.h>
#include <Model_AttributeString.h>
#include <Model_AttributeSelection.h>
#include <Model_AttributeSelectionList.h>
#include <Model_AttributeIntArray.h>
#include <Model_Events.h>
#include <Model_Expression.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Tools.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>

#include <GeomData_Point.h>
#include <GeomData_Point2D.h>
#include <GeomData_Dir.h>
#include <Events_Loop.h>
#include <Events_Error.h>

#include <TDataStd_Name.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_RelocationTable.hxx>

#include <string>

// myLab contains:
// TDataStd_Name - name of the object
// TDataStd_IntegerArray - state of the object execution, transaction ID of update
// TDataStd_BooleanArray - array of flags of this data:
//                             0 - is in history or not
static const int kFlagInHistory = 0;
//                             1 - is displayed or not
static const int kFlagDisplayed = 1;

// invalid data
const static std::shared_ptr<ModelAPI_Data> kInvalid(new Model_Data());

Model_Data::Model_Data() : mySendAttributeUpdated(true)
{
}

void Model_Data::setLabel(TDF_Label theLab)
{
  myLab = theLab;
  // set or get the default flags
  if (!myLab.FindAttribute(TDataStd_BooleanArray::GetID(), myFlags)) {
    // set default values if not found
    myFlags = TDataStd_BooleanArray::Set(myLab, 0, 1);
    myFlags->SetValue(kFlagInHistory, Standard_True); // is in history by default is true
    myFlags->SetValue(kFlagDisplayed, Standard_True); // is displayed by default is true
  }
}

std::string Model_Data::name()
{
  Handle(TDataStd_Name) aName;
  if (myLab.FindAttribute(TDataStd_Name::GetID(), aName))
    return std::string(TCollection_AsciiString(aName->Get()).ToCString());
  return "";  // not defined
}

void Model_Data::setName(const std::string& theName)
{
  bool isModified = false;
  std::string anOldName = name();
  Handle(TDataStd_Name) aName;
  if (!myLab.FindAttribute(TDataStd_Name::GetID(), aName)) {
    TDataStd_Name::Set(myLab, theName.c_str());
    isModified = true;
  } else {
    isModified = !aName->Get().IsEqual(theName.c_str());
    if (isModified)
      aName->Set(theName.c_str());
  }
  if (mySendAttributeUpdated && isModified)
    ModelAPI_ObjectRenamedMessage::send(myObject, anOldName, theName, this);
}

AttributePtr Model_Data::addAttribute(const std::string& theID, const std::string theAttrType)
{
  AttributePtr aResult;
  TDF_Label anAttrLab = myLab.FindChild(myAttrs.size() + 1);
  ModelAPI_Attribute* anAttr = 0;
  if (theAttrType == ModelAPI_AttributeDocRef::typeId()) {
    anAttr = new Model_AttributeDocRef(anAttrLab);
  } else if (theAttrType == Model_AttributeInteger::typeId()) {
    anAttr = new Model_AttributeInteger(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeDouble::typeId()) {
    Model_AttributeDouble* anAttribute = new Model_AttributeDouble(anAttrLab);
    TDF_Label anExpressionLab = anAttrLab.FindChild(anAttrLab.NbChildren() + 1);
    anAttribute->myExpression.reset(new Model_Expression(anExpressionLab));
    anAttribute->myIsInitialized = anAttribute->myIsInitialized && anAttribute->myExpression->isInitialized(); 
    anAttr = anAttribute;
  } else if (theAttrType == Model_AttributeBoolean::typeId()) {
    anAttr = new Model_AttributeBoolean(anAttrLab);
  } else if (theAttrType == Model_AttributeString::typeId()) {
    anAttr = new Model_AttributeString(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeReference::typeId()) {
    anAttr = new Model_AttributeReference(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeSelection::typeId()) {
    anAttr = new Model_AttributeSelection(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeSelectionList::typeId()) {
    anAttr = new Model_AttributeSelectionList(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeRefAttr::typeId()) {
    anAttr = new Model_AttributeRefAttr(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeRefList::typeId()) {
    anAttr = new Model_AttributeRefList(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeIntArray::typeId()) {
    anAttr = new Model_AttributeIntArray(anAttrLab);
  } 
  // create also GeomData attributes here because only here the OCAF structure is known
  else if (theAttrType == GeomData_Point::typeId()) {
    GeomData_Point* anAttribute = new GeomData_Point(anAttrLab);
    for (int aComponent = 0; aComponent < GeomData_Point::NUM_COMPONENTS; ++aComponent) {
      TDF_Label anExpressionLab = anAttrLab.FindChild(anAttrLab.NbChildren() + 1);
      anAttribute->myExpression[aComponent].reset(new Model_Expression(anExpressionLab));
      anAttribute->myIsInitialized = anAttribute->myIsInitialized && anAttribute->myExpression[aComponent]->isInitialized(); 
    }
    anAttr = anAttribute;
  } else if (theAttrType == GeomData_Dir::typeId()) {
    anAttr = new GeomData_Dir(anAttrLab);
  } else if (theAttrType == GeomData_Point2D::typeId()) {
    GeomData_Point2D* anAttribute = new GeomData_Point2D(anAttrLab);
    for (int aComponent = 0; aComponent < GeomData_Point2D::NUM_COMPONENTS; ++aComponent) {
      TDF_Label anExpressionLab = anAttrLab.FindChild(anAttrLab.NbChildren() + 1);
      anAttribute->myExpression[aComponent].reset(new Model_Expression(anExpressionLab));
      anAttribute->myIsInitialized = anAttribute->myIsInitialized && anAttribute->myExpression[aComponent]->isInitialized(); 
    }
    anAttr = anAttribute;
  }
  if (anAttr) {
    aResult = std::shared_ptr<ModelAPI_Attribute>(anAttr);
    myAttrs[theID] = aResult;
    anAttr->setObject(myObject);
    anAttr->setID(theID);
  } else {
    Events_Error::send("Can not create unknown type of attribute " + theAttrType);
  }
  return aResult;
}

// macro for gthe generic returning of the attribute by the ID
#define GET_ATTRIBUTE_BY_ID(ATTR_TYPE, METHOD_NAME) \
  std::shared_ptr<ATTR_TYPE> Model_Data::METHOD_NAME(const std::string& theID) { \
    std::shared_ptr<ATTR_TYPE> aRes; \
    std::map<std::string, AttributePtr >::iterator aFound = \
      myAttrs.find(theID); \
    if (aFound != myAttrs.end()) { \
      aRes = std::dynamic_pointer_cast<ATTR_TYPE>(aFound->second); \
    } \
    return aRes; \
  }
// implement nice getting methods for all ModelAPI attributes
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeDocRef, document);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeDouble, real);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeInteger, integer);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeBoolean, boolean);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeString, string);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeReference, reference);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeSelection, selection);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeSelectionList, selectionList);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeRefAttr, refattr);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeRefList, reflist);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeIntArray, intArray);

std::shared_ptr<ModelAPI_Attribute> Model_Data::attribute(const std::string& theID)
{
  std::shared_ptr<ModelAPI_Attribute> aResult;
  if (myAttrs.find(theID) == myAttrs.end())  // no such attribute
    return aResult;
  return myAttrs[theID];
}

const std::string& Model_Data::id(const std::shared_ptr<ModelAPI_Attribute>& theAttr)
{
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = 
    myAttrs.begin();
  for (; anAttr != myAttrs.end(); anAttr++) {
    if (anAttr->second == theAttr)
      return anAttr->first;
  }
  // not found
  static std::string anEmpty;
  return anEmpty;
}

bool Model_Data::isEqual(const std::shared_ptr<ModelAPI_Data>& theData)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theData);
  if (aData)
    return myLab.IsEqual(aData->myLab) == Standard_True ;
  return false;
}

bool Model_Data::isValid()
{
  return !myLab.IsNull() && myLab.HasAttribute();
}

std::list<std::shared_ptr<ModelAPI_Attribute> > Model_Data::attributes(const std::string& theType)
{
  std::list<std::shared_ptr<ModelAPI_Attribute> > aResult;
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> >::iterator anAttrsIter = 
    myAttrs.begin();
  for (; anAttrsIter != myAttrs.end(); anAttrsIter++) {
    if (theType.empty() || anAttrsIter->second->attributeType() == theType) {
      aResult.push_back(anAttrsIter->second);
    }
  }
  return aResult;
}

std::list<std::string> Model_Data::attributesIDs(const std::string& theType) 
{
  std::list<std::string> aResult;
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> >::iterator anAttrsIter = 
    myAttrs.begin();
  for (; anAttrsIter != myAttrs.end(); anAttrsIter++) {
    if (theType.empty() || anAttrsIter->second->attributeType() == theType) {
      aResult.push_back(anAttrsIter->first);
    }
  }
  return aResult;
}

void Model_Data::sendAttributeUpdated(ModelAPI_Attribute* theAttr)
{
  theAttr->setInitialized();
  if (theAttr->isArgument()) {
    static const Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
    ModelAPI_EventCreator::get()->sendUpdated(myObject, anEvent);
    if (mySendAttributeUpdated && myObject) {
      myObject->attributeChanged(theAttr->id());
    }
  }
}

void Model_Data::blockSendAttributeUpdated(const bool theBlock)
{
  mySendAttributeUpdated = !theBlock;
}

void Model_Data::erase()
{
  if (!myLab.IsNull())
    myLab.ForgetAllAttributes();
}

// indexes in the state array
enum StatesIndexes {
  STATE_INDEX_STATE = 1, // the state type itself
  STATE_INDEX_TRANSACTION = 2, // transaction ID
};

/// Returns the label array, initialises it by default values if not exists
static Handle(TDataStd_IntegerArray) stateArray(TDF_Label& theLab)
{
  Handle(TDataStd_IntegerArray) aStateArray;
  if (!theLab.FindAttribute(TDataStd_IntegerArray::GetID(), aStateArray)) {
    aStateArray = TDataStd_IntegerArray::Set(theLab, 1, 2);
    aStateArray->SetValue(STATE_INDEX_STATE, ModelAPI_StateMustBeUpdated); // default state
    aStateArray->SetValue(STATE_INDEX_TRANSACTION, 0); // default transaction ID (not existing)
  }
  return aStateArray;
}

void Model_Data::execState(const ModelAPI_ExecState theState)
{
  if (theState != ModelAPI_StateNothing) {
    stateArray(myLab)->SetValue(STATE_INDEX_STATE, (int)theState);
  }
}

ModelAPI_ExecState Model_Data::execState()
{
  return ModelAPI_ExecState(stateArray(myLab)->Value(STATE_INDEX_STATE));
}

int Model_Data::updateID()
{
  return stateArray(myLab)->Value(STATE_INDEX_TRANSACTION);
}

void Model_Data::setUpdateID(const int theID)
{
  stateArray(myLab)->SetValue(STATE_INDEX_TRANSACTION, theID);
}

void Model_Data::setError(const std::string& theError, bool theSend)
{
  execState(ModelAPI_StateExecFailed);
  if (theSend) {
    Events_Error::send(theError);
  }
  TDataStd_AsciiString::Set(myLab, theError.c_str());
}

std::string Model_Data::error() const
{
  Handle(TDataStd_AsciiString) anErrorAttr;
  if (myLab.FindAttribute(TDataStd_AsciiString::GetID(), anErrorAttr)) {
    return std::string(anErrorAttr->Get().ToCString());
  }
  return std::string();
}

int Model_Data::featureId() const
{
  return myLab.Father().Tag(); // tag of the feature label
}

void Model_Data::eraseBackReferences()
{
  myRefsToMe.clear();
  std::shared_ptr<ModelAPI_Result> aRes = 
    std::dynamic_pointer_cast<ModelAPI_Result>(myObject);
  if (aRes)
    aRes->setIsConcealed(false);
}

void Model_Data::removeBackReference(FeaturePtr theFeature, std::string theAttrID)
{
  AttributePtr anAttribute = theFeature->data()->attribute(theAttrID);
  if (myRefsToMe.find(anAttribute) == myRefsToMe.end())
    return;

  myRefsToMe.erase(anAttribute);

  // remove concealment immideately: on deselection it must be posible to reselect in GUI the same
  if (ModelAPI_Session::get()->validators()->isConcealed(theFeature->getKind(), theAttrID)) {
    updateConcealmentFlag();
  }
}

void Model_Data::addBackReference(FeaturePtr theFeature, std::string theAttrID, 
   const bool theApplyConcealment)
{
  // do not add the same attribute twice
  AttributePtr anAttribute = theFeature->data()->attribute(theAttrID);
  if (myRefsToMe.find(anAttribute) != myRefsToMe.end())
    return;

  myRefsToMe.insert(theFeature->data()->attribute(theAttrID));
  if (theApplyConcealment && 
      ModelAPI_Session::get()->validators()->isConcealed(theFeature->getKind(), theAttrID)) {
    std::shared_ptr<ModelAPI_Result> aRes = 
      std::dynamic_pointer_cast<ModelAPI_Result>(myObject);
    // the second condition is for history upper than concealment causer, so the feature result may
    // be displayed and previewed; also for avoiding of quick show/hide on history
    // moving deep down
    if (aRes && !theFeature->isDisabled()) {
      aRes->setIsConcealed(true);
    }
  }
}

void Model_Data::updateConcealmentFlag()
{
  std::set<AttributePtr>::iterator aRefsIter = myRefsToMe.begin();
  for(; aRefsIter != myRefsToMe.end(); aRefsIter++) {
    if (aRefsIter->get()) {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefsIter)->owner());
      if (aFeature.get() && !aFeature->isDisabled()) {
        if (ModelAPI_Session::get()->validators()->isConcealed(
              aFeature->getKind(), (*aRefsIter)->id())) {
          return; // it is still concealed, nothing to do
        }
      }
    }
  }
  // thus, no concealment references anymore => make not-concealed
  std::shared_ptr<ModelAPI_Result> aRes = 
    std::dynamic_pointer_cast<ModelAPI_Result>(myObject);
  if (aRes.get()) {
    aRes->setIsConcealed(false);
    static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
    ModelAPI_EventCreator::get()->sendUpdated(aRes, anEvent);
    Events_Loop::loop()->flush(anEvent);
  }
}

#include <Model_Validator.h>

std::set<std::string> set_union(const std::set<std::string>& theLeft, 
                                const std::set<std::string>& theRight)
{
  std::set<std::string> aResult;
  aResult.insert(theLeft.begin(), theLeft.end());
  aResult.insert(theRight.begin(), theRight.end());
  return aResult;
}

std::set<std::string> usedParameters(const AttributePointPtr& theAttribute)
{
  std::set<std::string> anUsedParameters;
  for (int aComponent = 0; aComponent < 3; ++aComponent)
    anUsedParameters = set_union(anUsedParameters, theAttribute->usedParameters(aComponent));
  return anUsedParameters;
}

std::set<std::string> usedParameters(const AttributePoint2DPtr& theAttribute)
{
  std::set<std::string> anUsedParameters;
  for (int aComponent = 0; aComponent < 2; ++aComponent)
    anUsedParameters = set_union(anUsedParameters, theAttribute->usedParameters(aComponent));
  return anUsedParameters;
}

std::list<ResultParameterPtr> findVariables(const std::set<std::string>& theParameters, 
                                            const DocumentPtr& theDocument)
{
  std::list<ResultParameterPtr> aResult;
  std::set<std::string>::const_iterator aParamIt = theParameters.cbegin();
  for (; aParamIt != theParameters.cend(); ++aParamIt) {
    const std::string& aName = *aParamIt;
    double aValue;
    ResultParameterPtr aParam;
    if (ModelAPI_Tools::findVariable(aName, aValue, aParam, theDocument))
      aResult.push_back(aParam);
  }
  return aResult;
}

void Model_Data::referencesToObjects(
  std::list<std::pair<std::string, std::list<ObjectPtr> > >& theRefs)
{
  static Model_ValidatorsFactory* aValidators = 
    static_cast<Model_ValidatorsFactory*>(ModelAPI_Session::get()->validators());
  FeaturePtr aMyFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(myObject);

  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = myAttrs.begin();
  std::list<ObjectPtr> aReferenced; // not inside of cycle to avoid excess memory management
  for(; anAttr != myAttrs.end(); anAttr++) {
    // skip not-case attributes, that really may refer to anything not-used (issue 671)
    if (aMyFeature.get() && !aValidators->isCase(aMyFeature, anAttr->second->id()))
      continue;

    std::string aType = anAttr->second->attributeType();
    if (aType == ModelAPI_AttributeReference::typeId()) { // reference to object
      std::shared_ptr<ModelAPI_AttributeReference> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeReference>(anAttr->second);
      aReferenced.push_back(aRef->value());
    } else if (aType == ModelAPI_AttributeRefAttr::typeId()) { // reference to attribute or object
      std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeRefAttr>(anAttr->second);
      aReferenced.push_back(aRef->isObject() ? aRef->object() : aRef->attr()->owner());
    } else if (aType == ModelAPI_AttributeRefList::typeId()) { // list of references
      aReferenced = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(anAttr->second)->list();
    } else if (aType == ModelAPI_AttributeSelection::typeId()) { // selection attribute
      std::shared_ptr<ModelAPI_AttributeSelection> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeSelection>(anAttr->second);
      aReferenced.push_back(aRef->context());
    } else if (aType == ModelAPI_AttributeSelectionList::typeId()) { // list of selection attributes
      std::shared_ptr<ModelAPI_AttributeSelectionList> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeSelectionList>(anAttr->second);
      for(int a = aRef->size() - 1; a >= 0; a--) {
        aReferenced.push_back(aRef->value(a)->context());
      }
    } else if (aType == ModelAPI_AttributeDouble::typeId()) { // double attribute
      AttributeDoublePtr anAttribute =
          std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttr->second);
      std::set<std::string> anUsedParameters = anAttribute->usedParameters();
      std::list<ResultParameterPtr> aParameters = findVariables(anUsedParameters, aMyFeature->document());
      aReferenced.insert(aReferenced.end(), aParameters.begin(), aParameters.end());
    } else if (aType == GeomDataAPI_Point::typeId()) { // point attribute
      AttributePointPtr anAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point>(anAttr->second);
      std::set<std::string> anUsedParameters = usedParameters(anAttribute);
      std::list<ResultParameterPtr> aParameters = findVariables(anUsedParameters, aMyFeature->document());
      aReferenced.insert(aReferenced.end(), aParameters.begin(), aParameters.end());
    } else if (aType == GeomDataAPI_Point2D::typeId()) { // point attribute
      AttributePoint2DPtr anAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->second);
      std::set<std::string> anUsedParameters = usedParameters(anAttribute);
      std::list<ResultParameterPtr> aParameters = findVariables(anUsedParameters, aMyFeature->document());
      aReferenced.insert(aReferenced.end(), aParameters.begin(), aParameters.end());
    } else
      continue; // nothing to do, not reference

    if (!aReferenced.empty()) {
      theRefs.push_back(std::pair<std::string, std::list<ObjectPtr> >(anAttr->first, aReferenced));
      aReferenced.clear();
    }
  }
}

/// makes copy of all attributes on the given label and all sub-labels
static void copyAttrs(TDF_Label theSource, TDF_Label theDestination) {
  TDF_AttributeIterator anAttrIter(theSource);
  for(; anAttrIter.More(); anAttrIter.Next()) {
    Handle(TDF_Attribute) aTargetAttr;
    if (!theDestination.FindAttribute(anAttrIter.Value()->ID(), aTargetAttr)) {
      // create a new attribute if not yet exists in the destination
	    aTargetAttr = anAttrIter.Value()->NewEmpty();
      theDestination.AddAttribute(aTargetAttr);
    }
    Handle(TDF_RelocationTable) aRelocTable = new TDF_RelocationTable(); // no relocation, empty map
    anAttrIter.Value()->Paste(aTargetAttr, aRelocTable);
  }
  // copy the sub-labels content
  TDF_ChildIterator aSubLabsIter(theSource);
  for(; aSubLabsIter.More(); aSubLabsIter.Next()) {
    copyAttrs(aSubLabsIter.Value(), theDestination.FindChild(aSubLabsIter.Value().Tag()));
  }
}

void Model_Data::copyTo(std::shared_ptr<ModelAPI_Data> theTarget)
{
  TDF_Label aTargetRoot = std::dynamic_pointer_cast<Model_Data>(theTarget)->label();
  copyAttrs(myLab, aTargetRoot);
  // make initialized the initialized attributes
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> >::iterator aMyIter = myAttrs.begin();
  for(; aMyIter != myAttrs.end(); aMyIter++) {
    if (aMyIter->second->isInitialized()) {
      theTarget->attribute(aMyIter->first)->setInitialized();
    }
  }
}

bool Model_Data::isInHistory()
{
  return myFlags->Value(kFlagInHistory) == Standard_True;
}

void Model_Data::setIsInHistory(const bool theFlag)
{
  return myFlags->SetValue(kFlagInHistory, theFlag);
}

bool Model_Data::isDisplayed()
{
  if (!myObject.get() || !myObject->document().get() || // object is in valid
      myFlags->Value(kFlagDisplayed) != Standard_True) // or it was not displayed before
    return false;
  if (myObject->document()->isActive()) // for active documents it must be ok anyway
    return true;
  // any object from the root document except part result may be displayed
  if (myObject->document() == ModelAPI_Session::get()->moduleDocument() &&
      myObject->groupName() != ModelAPI_ResultPart::group())
    return true;
  return false;
}

void Model_Data::setDisplayed(const bool theDisplay)
{
  if (theDisplay != isDisplayed()) {
    myFlags->SetValue(kFlagDisplayed, theDisplay);
    static Events_Loop* aLoop = Events_Loop::loop();
    static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
    aECreator->sendUpdated(myObject, EVENT_DISP);
  }
}

std::shared_ptr<ModelAPI_Data> Model_Data::invalidPtr()
{
  return kInvalid;
}

std::shared_ptr<ModelAPI_Data> Model_Data::invalidData()
{
  return kInvalid;
}

bool Model_Data::isOwner(ModelAPI_Object* theOwner)
{
  return theOwner == myObject.get();
}
