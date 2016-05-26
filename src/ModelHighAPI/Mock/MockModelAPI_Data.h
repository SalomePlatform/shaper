// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef MockModelAPI_Data_H_
#define MockModelAPI_Data_H_

#include <gmock/gmock.h>

#include <ModelAPI_Data.h>

class MockModelAPI_Data : public ModelAPI_Data {
 public:
  MOCK_METHOD0(name,
      std::string());
  MOCK_METHOD1(setName,
      void(const std::string& theName));
  MOCK_METHOD1(document,
      std::shared_ptr<ModelAPI_AttributeDocRef>(const std::string& theID));
  MOCK_METHOD1(real,
      std::shared_ptr<ModelAPI_AttributeDouble>(const std::string& theID));
  MOCK_METHOD1(integer,
      std::shared_ptr<ModelAPI_AttributeInteger>(const std::string& theID));
  MOCK_METHOD1(reference,
      std::shared_ptr<ModelAPI_AttributeReference>(const std::string& theID));
  MOCK_METHOD1(selection,
      std::shared_ptr<ModelAPI_AttributeSelection>(const std::string& theID));
  MOCK_METHOD1(selectionList,
      std::shared_ptr<ModelAPI_AttributeSelectionList>(const std::string& theID));
  MOCK_METHOD1(refattr,
      std::shared_ptr<ModelAPI_AttributeRefAttr>(const std::string& theID));
  MOCK_METHOD1(reflist,
      std::shared_ptr<ModelAPI_AttributeRefList>(const std::string& theID));
  MOCK_METHOD1(refattrlist,
      std::shared_ptr<ModelAPI_AttributeRefAttrList>(const std::string& theID));
  MOCK_METHOD1(boolean,
      std::shared_ptr<ModelAPI_AttributeBoolean>(const std::string& theID));
  MOCK_METHOD1(string,
      std::shared_ptr<ModelAPI_AttributeString>(const std::string& theID));
  MOCK_METHOD1(intArray,
      std::shared_ptr<ModelAPI_AttributeIntArray>(const std::string& theID));
  MOCK_METHOD1(attribute,
      std::shared_ptr<ModelAPI_Attribute>(const std::string& theID));
  MOCK_METHOD1(attributes,
      std::list<std::shared_ptr<ModelAPI_Attribute> >(const std::string& theType));
  MOCK_METHOD1(attributesIDs,
      std::list<std::string>(const std::string& theType));
  MOCK_METHOD1(id,
      const std::string&(const std::shared_ptr<ModelAPI_Attribute>& theAttr));
  MOCK_METHOD1(isEqual,
      bool(const std::shared_ptr<ModelAPI_Data>& theData));
  MOCK_METHOD0(isValid,
      bool());
  MOCK_METHOD2(addAttribute,
      std::shared_ptr<ModelAPI_Attribute>(const std::string& theID, const std::string theAttrType));
  MOCK_METHOD1(sendAttributeUpdated,
      void(ModelAPI_Attribute* theAttr));
  MOCK_METHOD1(blockSendAttributeUpdated,
      void(const bool theBlock));
  MOCK_METHOD0(erase,
      void());
  MOCK_METHOD1(execState,
      void(const ModelAPI_ExecState theState));
  MOCK_METHOD0(execState,
      ModelAPI_ExecState());
  MOCK_METHOD2(setError,
      void(const std::string& theError, bool theSend));
  MOCK_CONST_METHOD0(error,
      std::string());
  MOCK_CONST_METHOD0(featureId,
      int());
  MOCK_METHOD0(refsToMe,
      const std::set<std::shared_ptr<ModelAPI_Attribute> >&());
  MOCK_METHOD1(referencesToObjects,
      void(std::list<std::pair<std::string, std::list<std::shared_ptr<ModelAPI_Object> > > >& theRefs));
  MOCK_METHOD1(copyTo,
      void(std::shared_ptr<ModelAPI_Data> theTarget));
  MOCK_METHOD0(invalidPtr,
      std::shared_ptr<ModelAPI_Data>());
  MOCK_METHOD0(updateID,
      int());
  MOCK_METHOD1(setUpdateID,
      void(const int theID));
  MOCK_METHOD0(owner,
      std::shared_ptr<ModelAPI_Object>());
  MOCK_METHOD0(isDeleted,
      bool());
  MOCK_METHOD1(setIsDeleted,
      void(const bool theFlag));
  MOCK_METHOD0(isInHistory,
      bool());
  MOCK_METHOD1(setIsInHistory,
      void(const bool theFlag));
  MOCK_METHOD0(isDisplayed,
      bool());
  MOCK_METHOD1(setDisplayed,
      void(const bool theDisplay));
};

#endif // MockModelAPI_Data_H_
