// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <Model_Data.h>
#include <Model_AttributeDocRef.h>
#include <Model_AttributeInteger.h>
#include <Model_AttributeDouble.h>
#include <Model_AttributeDoubleArray.h>
#include <Model_AttributeReference.h>
#include <Model_AttributeRefAttr.h>
#include <Model_AttributeRefList.h>
#include <Model_AttributeRefAttrList.h>
#include <Model_AttributeBoolean.h>
#include <Model_AttributeString.h>
#include <Model_AttributeStringArray.h>
#include <Model_AttributeSelection.h>
#include <Model_AttributeSelectionList.h>
#include <Model_AttributeIntArray.h>
#include <Model_AttributeTables.h>
#include <Model_Events.h>
#include <Model_Expression.h>
#include <Model_Tools.h>
#include <Model_Validator.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Tools.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point2DArray.h>

#include <GeomData_Point.h>
#include <GeomData_Point2D.h>
#include <GeomData_Point2DArray.h>
#include <GeomData_Dir.h>
#include <Events_Loop.h>
#include <Events_InfoMessage.h>

#include <Locale_Convert.h>

#include <TDataStd_Name.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_ChildIDIterator.hxx>

#include <string>

// myLab contains:
// TDataStd_Name - name of the object
// TDataStd_IntegerArray - state of the object execution, transaction ID of update
// TDataStd_BooleanArray - array of flags of this data:
//                             0 - is in history or not
static const int kFlagInHistory = 0;
//                             1 - is displayed or not
static const int kFlagDisplayed = 1;
//                             2 - is deleted (for results) or not
static const int kFlagDeleted = 2;
// TDataStd_Integer - 0 if the name of the object is generated automatically,
//                    otherwise the name is defined by user
static const Standard_GUID kUSER_DEFINED_NAME("9c694d18-a83c-4a56-bc9b-8020628a8244");

// invalid data
const static std::shared_ptr<ModelAPI_Data> kInvalid(new Model_Data());

static const Standard_GUID kGroupAttributeGroupID("df64ea4c-fc42-4bf8-ad7e-08f7a54bf1b8");
static const Standard_GUID kGroupAttributeID("ebdcb22a-e045-455b-9a7f-cfd38d68e185");

// id of attribute to store the version of the feature
static const Standard_GUID kVERSION_ID("61cdb78a-1ba7-4942-976f-63bea7f4a2b1");


Model_Data::Model_Data() : mySendAttributeUpdated(true), myWasChangedButBlocked(false)
{
}

void Model_Data::setLabel(TDF_Label theLab)
{
  myLab = theLab;
  // set or get the default flags
  if (!myLab.FindAttribute(TDataStd_BooleanArray::GetID(), myFlags)) {
    // set default values if not found
    myFlags = TDataStd_BooleanArray::Set(myLab, 0, 2);
    myFlags->SetValue(kFlagInHistory, Standard_True); // is in history by default is true
    myFlags->SetValue(kFlagDisplayed, Standard_True); // is displayed by default is true
    myFlags->SetValue(kFlagDeleted, Standard_False); // is deleted by default is false
  }
}

std::wstring Model_Data::name()
{
  Handle(TDataStd_Name) aName;
  if (shapeLab().FindAttribute(TDataStd_Name::GetID(), aName)) {
#ifdef DEBUG_NAMES
    myObject->myName = Locale::Convert::toWString(aName->Get().ToExtString());
#endif
    return Locale::Convert::toWString(aName->Get().ToExtString());
  }
  return L"";  // not defined
}

void Model_Data::setName(const std::wstring& theName)
{
  bool isModified = false;
  std::wstring anOldName = name();
  Handle(TDataStd_Name) aName;
  if (!shapeLab().FindAttribute(TDataStd_Name::GetID(), aName)) {
    TDataStd_Name::Set(shapeLab(), theName.c_str());
    isModified = true;
  } else {
    isModified = !aName->Get().IsEqual(theName.c_str());
    if (isModified) {
      aName->Set(theName.c_str());

      // check the name of result is defined by user
      // (name of result does not composed of the name of feature and the result index)
      bool isUserDefined = true;
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(myObject);
      if (aResult) {
        std::wstring aDefaultName = ModelAPI_Tools::getDefaultName(aResult, false).first;
        isUserDefined = aDefaultName != theName;
      }
      if (isUserDefined) {
        // name is user-defined, thus special attribute is set
        TDataStd_UAttribute::Set(shapeLab(), kUSER_DEFINED_NAME);
      }
    }
  }
  if (mySendAttributeUpdated && isModified)
    ModelAPI_ObjectRenamedMessage::send(myObject, anOldName, theName, this);
  if (isModified && myObject && myObject->document()) {
    std::dynamic_pointer_cast<Model_Document>(myObject->document())->
      changeNamingName(anOldName, theName, shapeLab());
  }
#ifdef DEBUG_NAMES
  myObject->myName = theName;
#endif
}

bool Model_Data::hasUserDefinedName() const
{
  return shapeLab().IsAttribute(kUSER_DEFINED_NAME);
}

std::string Model_Data::version()
{
  Handle(TDataStd_Name) aVersionAttr;
  std::string aVersion;
  if (shapeLab().FindAttribute(kVERSION_ID, aVersionAttr))
    aVersion = TCollection_AsciiString(aVersionAttr->Get()).ToCString();
  return aVersion;
}

void Model_Data::setVersion(const std::string& theVersion)
{
  Handle(TDataStd_Name) aVersionAttr;
  std::string aVersion;
  if (!shapeLab().FindAttribute(kVERSION_ID, aVersionAttr))
    aVersionAttr = TDataStd_Name::Set(shapeLab(), kVERSION_ID, TCollection_ExtendedString());
  aVersionAttr->Set(theVersion.c_str());
}

AttributePtr Model_Data::addAttribute(
  const std::string& theID, const std::string theAttrType, const int theIndex)
{
  AttributePtr aResult;
  int anAttrIndex = theIndex == -1 ? int(myAttrs.size()) + 1 : theIndex;
  TDF_Label anAttrLab = myLab.FindChild(anAttrIndex);
  ModelAPI_Attribute* anAttr = 0;
  if (theAttrType == ModelAPI_AttributeDocRef::typeId()) {
    anAttr = new Model_AttributeDocRef(anAttrLab);
  } else if (theAttrType == Model_AttributeInteger::typeId()) {
    anAttr = new Model_AttributeInteger(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeDouble::typeId()) {
    anAttr = new Model_AttributeDouble(anAttrLab);
  } else if (theAttrType == Model_AttributeBoolean::typeId()) {
    anAttr = new Model_AttributeBoolean(anAttrLab);
  } else if (theAttrType == Model_AttributeString::typeId()) {
    anAttr = new Model_AttributeString(anAttrLab);
  } else if (theAttrType == Model_AttributeStringArray::typeId()) {
    anAttr = new Model_AttributeStringArray(anAttrLab);
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
  } else if (theAttrType == ModelAPI_AttributeRefAttrList::typeId()) {
    anAttr = new Model_AttributeRefAttrList(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeIntArray::typeId()) {
    anAttr = new Model_AttributeIntArray(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeDoubleArray::typeId()) {
    anAttr = new Model_AttributeDoubleArray(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeTables::typeId()) {
    anAttr = new Model_AttributeTables(anAttrLab);
  }
  // create also GeomData attributes here because only here the OCAF structure is known
  else if (theAttrType == GeomData_Point::typeId()) {
    GeomData_Point* anAttribute = new GeomData_Point();
    bool anAllInitialized = true;
    for (int aComponent = 0; aComponent < GeomData_Point::NUM_COMPONENTS; ++aComponent) {
      TDF_Label anExpressionLab = anAttrLab.FindChild(aComponent + 1);
      anAttribute->myExpression[aComponent].reset(new Model_ExpressionDouble(anExpressionLab));
      anAllInitialized = anAllInitialized && anAttribute->myExpression[aComponent]->isInitialized();
    }
    anAttribute->myIsInitialized = anAllInitialized;
    anAttr = anAttribute;
  } else if (theAttrType == GeomData_Dir::typeId()) {
    anAttr = new GeomData_Dir(anAttrLab);
  } else if (theAttrType == GeomData_Point2D::typeId()) {
    GeomData_Point2D* anAttribute = new GeomData_Point2D();
    bool anAllInitialized = true;
    for (int aComponent = 0; aComponent < GeomData_Point2D::NUM_COMPONENTS; ++aComponent) {
      TDF_Label anExpressionLab = anAttrLab.FindChild(aComponent + 1);
      anAttribute->myExpression[aComponent].reset(new Model_ExpressionDouble(anExpressionLab));
      anAllInitialized = anAllInitialized && anAttribute->myExpression[aComponent]->isInitialized();
    }
    anAttribute->myIsInitialized = anAllInitialized;
    anAttr = anAttribute;
  } else if (theAttrType == GeomData_Point2DArray::typeId()) {
    anAttr = new GeomData_Point2DArray(anAttrLab);
  }

  if (anAttr) {
    aResult = std::shared_ptr<ModelAPI_Attribute>(anAttr);
    myAttrs[theID] = std::pair<AttributePtr, int>(aResult, anAttrIndex);
    anAttr->setObject(myObject);
    anAttr->setID(theID);
  } else {
    Events_InfoMessage("Model_Data",
      "Can not create unknown type of attribute %1").arg(theAttrType).send();
  }
  return aResult;
}

AttributePtr Model_Data::addFloatingAttribute(
  const std::string& theID, const std::string theAttrType, const std::string& theGroup)
{
  // compute the index of the attribute placement
  int anIndex;
  TDF_Label aLab;
  if (myLab.IsAttribute(TDF_TagSource::GetID())) {
    // check this is re-init of attributes, so, check attribute with this name already there
    TDF_ChildIDIterator anIter(myLab, kGroupAttributeID, false);
    for(; anIter.More(); anIter.Next()) {
      TCollection_AsciiString aThisName(Handle(TDataStd_Name)::DownCast(anIter.Value())->Get());
      if (theID == aThisName.ToCString()) {
        TDF_Label aLab = anIter.Value()->Label();
        Handle(TDataStd_Name) aGName;
        if (aLab.FindAttribute(kGroupAttributeGroupID, aGName)) {
          TCollection_AsciiString aGroupName(aGName->Get());
          if (theGroup == aGroupName.ToCString()) {
            return addAttribute(theGroup + "__" + theID, theAttrType, aLab.Tag());
          }
        }
      }
    }
    aLab = myLab.NewChild(); // already exists a floating attribute, create the next
    anIndex = aLab.Tag();
  } else { // put the first floating attribute, quite far from other standard attributes
    anIndex = int(myAttrs.size()) + 1000;
    TDF_TagSource::Set(myLab)->Set(anIndex);
    aLab = myLab.FindChild(anIndex, true);
  }
  // store the group ID and the attribute ID (to restore correctly)
  TDataStd_Name::Set(aLab, kGroupAttributeGroupID, theGroup.c_str());
  TDataStd_Name::Set(aLab, kGroupAttributeID, theID.c_str());

  return addAttribute(theGroup + "__" + theID, theAttrType, anIndex);
}

void Model_Data::allGroups(std::list<std::string>& theGroups)
{
  std::set<std::string> alreadyThere;
  for(TDF_ChildIDIterator aGroup(myLab, kGroupAttributeGroupID); aGroup.More(); aGroup.Next()) {
    Handle(TDataStd_Name) aGroupAttr = Handle(TDataStd_Name)::DownCast(aGroup.Value());
    std::string aGroupID = TCollection_AsciiString(aGroupAttr->Get()).ToCString();
    if (alreadyThere.find(aGroupID) == alreadyThere.end()) {
      theGroups.push_back(aGroupID);
      alreadyThere.insert(aGroupID);
    }
  }
}

void Model_Data::attributesOfGroup(const std::string& theGroup,
  std::list<std::shared_ptr<ModelAPI_Attribute> >& theAttrs)
{
  for(TDF_ChildIDIterator aGroup(myLab, kGroupAttributeGroupID); aGroup.More(); aGroup.Next()) {
    Handle(TDataStd_Name) aGroupID = Handle(TDataStd_Name)::DownCast(aGroup.Value());
    if (aGroupID->Get().IsEqual(theGroup.c_str())) {
      Handle(TDataStd_Name) anID;
      if (aGroup.Value()->Label().FindAttribute(kGroupAttributeID, anID)) {
        TCollection_AsciiString anAsciiID(aGroupID->Get() + "__" + anID->Get());
        theAttrs.push_back(attribute(anAsciiID.ToCString()));
      }
    }
  }
}

void Model_Data::removeAttributes(const std::string& theGroup)
{
  TDF_LabelList aLabsToRemove; // collect labels that must be erased after the cycle
  for(TDF_ChildIDIterator aGroup(myLab, kGroupAttributeGroupID); aGroup.More(); aGroup.Next()) {
    Handle(TDataStd_Name) aGroupID = Handle(TDataStd_Name)::DownCast(aGroup.Value());
    if (aGroupID->Get().IsEqual(theGroup.c_str())) {
      Handle(TDataStd_Name) anID;
      if (!aGroup.Value()->Label().IsNull() &&
          aGroup.Value()->Label().FindAttribute(kGroupAttributeID, anID)) {
        aLabsToRemove.Append(aGroup.Value()->Label());
      }
      TCollection_AsciiString anAsciiID(aGroupID->Get() + "__" + anID->Get());
      myAttrs.erase(anAsciiID.ToCString());
    }
  }
  for(TDF_LabelList::Iterator aLab(aLabsToRemove); aLab.More(); aLab.Next()) {
    aLab.ChangeValue().ForgetAllAttributes(true);
  }
}

void Model_Data::clearAttributes()
{
  myAttrs.clear();
}



// macro for the generic returning of the attribute by the ID
#define GET_ATTRIBUTE_BY_ID(ATTR_TYPE, METHOD_NAME) \
  std::shared_ptr<ATTR_TYPE> Model_Data::METHOD_NAME(const std::string& theID) { \
    std::shared_ptr<ATTR_TYPE> aRes; \
    AttributeMap::iterator aFound = myAttrs.find(theID); \
    if (aFound != myAttrs.end()) { \
      aRes = std::dynamic_pointer_cast<ATTR_TYPE>(aFound->second.first); \
    } \
    return aRes; \
  }
// implement nice getting methods for all ModelAPI attributes
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeDocRef, document);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeDouble, real);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeInteger, integer);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeBoolean, boolean);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeString, string);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeStringArray, stringArray);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeReference, reference);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeSelection, selection);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeSelectionList, selectionList);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeRefAttr, refattr);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeRefList, reflist);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeRefAttrList, refattrlist);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeIntArray, intArray);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeDoubleArray, realArray);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeTables, tables);

std::shared_ptr<ModelAPI_Attribute> Model_Data::attribute(const std::string& theID)
{
  std::shared_ptr<ModelAPI_Attribute> aResult;
  if (myAttrs.find(theID) == myAttrs.end())  // no such attribute
    return aResult;
  return myAttrs[theID].first;
}

const std::string& Model_Data::id(const std::shared_ptr<ModelAPI_Attribute>& theAttr)
{
  AttributeMap::iterator anAttr = myAttrs.begin();
  for (; anAttr != myAttrs.end(); anAttr++) {
    if (anAttr->second.first == theAttr)
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
  AttributeMap::iterator anAttrsIter = myAttrs.begin();
  for (; anAttrsIter != myAttrs.end(); anAttrsIter++) {
    AttributePtr anAttr = anAttrsIter->second.first;
    if (theType.empty() || anAttr->attributeType() == theType) {
      aResult.push_back(anAttr);
    }
  }
  return aResult;
}

std::list<std::string> Model_Data::attributesIDs(const std::string& theType)
{
  std::list<std::string> aResult;
  AttributeMap::iterator anAttrsIter = myAttrs.begin();
  for (; anAttrsIter != myAttrs.end(); anAttrsIter++) {
    AttributePtr anAttr = anAttrsIter->second.first;
    if (theType.empty() || anAttr->attributeType() == theType) {
      aResult.push_back(anAttrsIter->first);
    }
  }
  return aResult;
}

void Model_Data::sendAttributeUpdated(ModelAPI_Attribute* theAttr)
{
  theAttr->setInitialized();
  if (theAttr->isArgument()) {
    if (mySendAttributeUpdated) {
      if (myObject) {
        try {
            myObject->attributeChanged(theAttr->id());
        } catch(...) {
          if (owner().get() && owner()->data().get() && owner()->data()->isValid()) {
            Events_InfoMessage("Model_Data",
              "%1 has failed during the update").arg(owner()->data()->name()).send();
          }
        }
        static const Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
        ModelAPI_EventCreator::get()->sendUpdated(myObject, anEvent);
      }
    } else {
      // to avoid too many duplications do not add the same like the last
      if (myWasChangedButBlocked.empty() || *(myWasChangedButBlocked.rbegin()) != theAttr)
        myWasChangedButBlocked.push_back(theAttr);
    }
  } else {
    // trim: need to redisplay or set color in the python script
    if (myObject && (theAttr->attributeType() == "Point2D" || theAttr->id() == "Color" ||
      theAttr->id() == "Transparency" || theAttr->id() == "Deflection" ||
      theAttr->id() == "Iso_lines" || theAttr->id() == "Show_Iso_lines")) {
      static const Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
      ModelAPI_EventCreator::get()->sendUpdated(myObject, anEvent);
    }
  }
}

bool Model_Data::blockSendAttributeUpdated(const bool theBlock, const bool theSendMessage)
{
  bool aWasBlocked = !mySendAttributeUpdated;
  if (mySendAttributeUpdated == theBlock) {
    mySendAttributeUpdated = !theBlock;
    if (mySendAttributeUpdated && !myWasChangedButBlocked.empty()) {
      // so, now it is ok to send the update signal
      if (theSendMessage) {
        // make a copy to avoid iteration on modified list
        // (may be cleared by attribute changed call)
        std::list<ModelAPI_Attribute*> aWasChangedButBlocked = myWasChangedButBlocked;
        myWasChangedButBlocked.clear();
        std::list<ModelAPI_Attribute*>::iterator aChangedIter = aWasChangedButBlocked.begin();
        for(; aChangedIter != aWasChangedButBlocked.end(); aChangedIter++) {
          try {
            myObject->attributeChanged((*aChangedIter)->id());
          } catch(...) {
            if (owner().get() && owner()->data().get() && owner()->data()->isValid()) {
              Events_InfoMessage("Model_Data",
                "%1 has failed during the update").arg(owner()->data()->name()).send();
            }
          }
        }
        static const Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
        ModelAPI_EventCreator::get()->sendUpdated(myObject, anEvent);
      } else {
        myWasChangedButBlocked.clear();
      }
    }
  }
  return aWasBlocked;
}

void Model_Data::erase()
{
  if (!myLab.IsNull()) {
    if (myLab.HasAttribute()) {
      // remove in order to clear back references in other objects
      std::list<std::pair<std::string, std::list<ObjectPtr> > > aRefs;
      referencesToObjects(aRefs);
      std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator
        anAttrIter = aRefs.begin();
      for(; anAttrIter != aRefs.end(); anAttrIter++) {
        std::list<ObjectPtr>::iterator aReferenced = anAttrIter->second.begin();
        for(; aReferenced != anAttrIter->second.end(); aReferenced++) {
          if (aReferenced->get() && (*aReferenced)->data()->isValid()) {
            std::shared_ptr<Model_Data> aData =
              std::dynamic_pointer_cast<Model_Data>((*aReferenced)->data());
            aData->removeBackReference(myAttrs[anAttrIter->first].first);
          }
        }
      }
    }
    myAttrs.clear();
    myLab.ForgetAllAttributes();
  }
}

// indexes in the state array
enum StatesIndexes {
  STATE_INDEX_STATE = 1, // the state type itself
  STATE_INDEX_TRANSACTION = 2, // transaction ID
};

/// Returns the label array, initializes it by default values if not exists
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
    if (stateArray(myLab)->Value(STATE_INDEX_STATE) != (int)theState) {
      stateArray(myLab)->SetValue(STATE_INDEX_STATE, (int)theState);
    }
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
    Events_InfoMessage("Model_Data", theError).send();
  }
  TDataStd_AsciiString::Set(myLab, theError.c_str());
}

void Model_Data::eraseErrorString()
{
  myLab.ForgetAttribute(TDataStd_AsciiString::GetID());
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

void Model_Data::removeBackReference(ObjectPtr theObject, std::string theAttrID)
{
  AttributePtr anAttribute = theObject->data()->attribute(theAttrID);
  removeBackReference(anAttribute);
}

void Model_Data::removeBackReference(AttributePtr theAttr)
{
  if (myRefsToMe.find(theAttr) == myRefsToMe.end())
    return;

  myRefsToMe.erase(theAttr);

  // remove concealment immediately: on deselection it must be possible to reselect in GUI the same
  FeaturePtr aFeatureOwner = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttr->owner());
  if (aFeatureOwner.get() &&
    ModelAPI_Session::get()->validators()->isConcealed(aFeatureOwner->getKind(), theAttr->id())) {
    updateConcealmentFlag();
  }
}

void Model_Data::addBackReference(FeaturePtr theFeature, std::string theAttrID,
   const bool theApplyConcealment)
{
  addBackReference(ObjectPtr(theFeature), theAttrID);

  if (theApplyConcealment &&  theFeature->isStable() &&
      ModelAPI_Session::get()->validators()->isConcealed(theFeature->getKind(), theAttrID)) {
    std::shared_ptr<ModelAPI_Result> aRes = std::dynamic_pointer_cast<ModelAPI_Result>(myObject);
    // the second condition is for history upper than concealment causer, so the feature result may
    // be displayed and previewed; also for avoiding of quick show/hide on history
    // moving deep down
    if (aRes && !theFeature->isDisabled()) {
      aRes->setIsConcealed(true, theFeature->getKind() == "RemoveResults");
    }
  }
}

void Model_Data::addBackReference(ObjectPtr theObject, std::string theAttrID)
{
  // it is possible to add the same attribute twice: may be last time the owner was not Stable...
  AttributePtr anAttribute = theObject->data()->attribute(theAttrID);
  if (myRefsToMe.find(anAttribute) == myRefsToMe.end())
    myRefsToMe.insert(anAttribute);
}

void Model_Data::updateConcealmentFlag()
{
  std::set<AttributePtr>::iterator aRefsIter = myRefsToMe.begin();
  for(; aRefsIter != myRefsToMe.end(); aRefsIter++) {
    if (aRefsIter->get()) {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefsIter)->owner());
      if (aFeature.get() && !aFeature->isDisabled() && aFeature->isStable()) {
        if (ModelAPI_Session::get()->validators()->isConcealed(
              aFeature->getKind(), (*aRefsIter)->id())) {
          std::shared_ptr<ModelAPI_Result> aRes =
            std::dynamic_pointer_cast<ModelAPI_Result>(myObject);
          if (aRes.get()) {
            if (aRes->groupName() != ModelAPI_ResultConstruction::group()) {
              aRes->setIsConcealed(true); // set concealed
              return;
            } else if (aFeature->getKind() == "RemoveResults") {
              aRes->setIsConcealed(true, true);
              return;
            }
          }
        }
      }
    }
  }
  std::shared_ptr<ModelAPI_Result> aRes =
    std::dynamic_pointer_cast<ModelAPI_Result>(myObject);
  if (aRes.get()) {
    aRes->setIsConcealed(false);
  }
}

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
    // theSearcher is not needed here: in expressions
    // of features the parameters history is not needed
    if (ModelAPI_Tools::findVariable(FeaturePtr(), aName, aValue, aParam, theDocument))
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

  AttributeMap::iterator anAttrIt = myAttrs.begin();
  std::list<ObjectPtr> aReferenced; // not inside of cycle to avoid excess memory management
  for(; anAttrIt != myAttrs.end(); anAttrIt++) {
    AttributePtr anAttr = anAttrIt->second.first;
    // skip not-case attributes, that really may refer to anything not-used (issue 671)
    if (aMyFeature.get() && !aValidators->isCase(aMyFeature, anAttr->id()))
      continue;

    std::string aType = anAttr->attributeType();
    if (aType == ModelAPI_AttributeReference::typeId()) { // reference to object
      std::shared_ptr<ModelAPI_AttributeReference> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeReference>(anAttr);
      aReferenced.push_back(aRef->value());
    } else if (aType == ModelAPI_AttributeRefAttr::typeId()) { // reference to attribute or object
      std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeRefAttr>(anAttr);
      if (aRef->isObject()) {
        aReferenced.push_back(aRef->object());
      } else {
        AttributePtr anAttr = aRef->attr();
        if (anAttr.get())
          aReferenced.push_back(anAttr->owner());
      }
    } else if (aType == ModelAPI_AttributeRefList::typeId()) { // list of references
      aReferenced = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(anAttr)->list();
    }
    else if (aType == ModelAPI_AttributeSelection::typeId()) { // selection attribute
      std::shared_ptr<ModelAPI_AttributeSelection> aRef = std::dynamic_pointer_cast<
        ModelAPI_AttributeSelection>(anAttr);
      FeaturePtr aRefFeat = aRef->contextFeature();
      if (aRefFeat.get()) { // reference to all results of the referenced feature
        const std::list<ResultPtr>& allRes = aRefFeat->results();
        std::list<ResultPtr>::const_iterator aRefRes = allRes.cbegin();
        for(; aRefRes != allRes.cend(); aRefRes++) {
          aReferenced.push_back(*aRefRes);
        }
      } else {
        aReferenced.push_back(aRef->context());
      }
    } else if (aType == ModelAPI_AttributeSelectionList::typeId()) { // list of selection attributes
      std::shared_ptr<ModelAPI_AttributeSelectionList> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeSelectionList>(anAttr);
      for(int a = 0, aSize = aRef->size(); a < aSize; ++a) {
        FeaturePtr aRefFeat = aRef->value(a)->contextFeature();
        if (aRefFeat.get()) { // reference to all results of the referenced feature
          const std::list<ResultPtr>& allRes = aRefFeat->results();
          std::list<ResultPtr>::const_iterator aRefRes = allRes.cbegin();
          for (; aRefRes != allRes.cend(); aRefRes++) {
            aReferenced.push_back(*aRefRes);
          }
        } else {
          aReferenced.push_back(aRef->value(a)->context());
        }
      }
    } else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
      std::shared_ptr<ModelAPI_AttributeRefAttrList> aRefAttr = std::dynamic_pointer_cast<
          ModelAPI_AttributeRefAttrList>(anAttr);
      std::list<std::pair<ObjectPtr, AttributePtr> > aRefs = aRefAttr->list();
      std::list<std::pair<ObjectPtr, AttributePtr> >::const_iterator anIt = aRefs.begin(),
                                                                     aLast = aRefs.end();
      for (; anIt != aLast; anIt++) {
        aReferenced.push_back(anIt->first);
      }
    } else if (aType == ModelAPI_AttributeInteger::typeId()) { // integer attribute
      AttributeIntegerPtr anAttribute =
          std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(anAttr);
      std::set<std::string> anUsedParameters = anAttribute->usedParameters();
      std::list<ResultParameterPtr> aParameters =
        findVariables(anUsedParameters, owner()->document());
      aReferenced.insert(aReferenced.end(), aParameters.begin(), aParameters.end());
    } else if (aType == ModelAPI_AttributeDouble::typeId()) { // double attribute
      AttributeDoublePtr anAttribute =
          std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttr);
      std::set<std::string> anUsedParameters = anAttribute->usedParameters();
      std::list<ResultParameterPtr> aParameters =
        findVariables(anUsedParameters, owner()->document());
      aReferenced.insert(aReferenced.end(), aParameters.begin(), aParameters.end());
    } else if (aType == GeomDataAPI_Point::typeId()) { // point attribute
      AttributePointPtr anAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point>(anAttr);
      std::set<std::string> anUsedParameters = usedParameters(anAttribute);
      std::list<ResultParameterPtr> aParameters =
        findVariables(anUsedParameters, owner()->document());
      aReferenced.insert(aReferenced.end(), aParameters.begin(), aParameters.end());
    } else if (aType == GeomDataAPI_Point2D::typeId()) { // point attribute
      AttributePoint2DPtr anAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr);
      std::set<std::string> anUsedParameters = usedParameters(anAttribute);
      std::list<ResultParameterPtr> aParameters =
        findVariables(anUsedParameters, owner()->document());
      aReferenced.insert(aReferenced.end(), aParameters.begin(), aParameters.end());
    } else
      continue; // nothing to do, not reference

    if (!aReferenced.empty()) {
      theRefs.push_back(
          std::pair<std::string, std::list<ObjectPtr> >(anAttrIt->first, aReferenced));
      aReferenced.clear();
    }
  }
}

void Model_Data::copyTo(std::shared_ptr<ModelAPI_Data> theTarget)
{
  TDF_Label aTargetRoot = std::dynamic_pointer_cast<Model_Data>(theTarget)->label();
  Model_Tools::copyAttrs(myLab, aTargetRoot);
  // reinitialize Model_Attributes by TDF_Attributes set
  std::shared_ptr<Model_Data> aTData = std::dynamic_pointer_cast<Model_Data>(theTarget);
  aTData->myAttrs.clear();
  theTarget->owner()->initAttributes(); // reinitialize feature attributes
}

bool Model_Data::isInHistory()
{
  return myFlags->Value(kFlagInHistory) == Standard_True;
}

void Model_Data::setIsInHistory(const bool theFlag)
{
  return myFlags->SetValue(kFlagInHistory, theFlag);
}

bool Model_Data::isDeleted()
{
  return myFlags->Value(kFlagDeleted) == Standard_True;
}

void Model_Data::setIsDeleted(const bool theFlag)
{
  return myFlags->SetValue(kFlagDeleted, theFlag);
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

std::shared_ptr<ModelAPI_Object> Model_Data::owner()
{
  return myObject;
}

bool Model_Data::isPrecedingAttribute(const std::string& theAttribute1,
                                      const std::string& theAttribute2) const
{
  AttributeMap::const_iterator aFound1 = myAttrs.find(theAttribute1);
  AttributeMap::const_iterator aFound2 = myAttrs.find(theAttribute2);
  if (aFound2 == myAttrs.end())
    return true;
  else if (aFound1 == myAttrs.end())
    return false;
  return aFound1->second.second < aFound2->second.second;
}
