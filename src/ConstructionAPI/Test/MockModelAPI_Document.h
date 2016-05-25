// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef MockModelAPI_Document_H_
#define MockModelAPI_Document_H_

#include <gmock/gmock.h>

#include <ModelAPI_Document.h>

class MockModelAPI_Document : public ModelAPI_Document {
 public:
  MOCK_CONST_METHOD0(kind,
      const std::string&());
  MOCK_METHOD1(close,
      void(bool));
  MOCK_METHOD2(addFeature,
      std::shared_ptr<ModelAPI_Feature>(std::string, bool));
  MOCK_METHOD3(refsToFeature,
      void(std::shared_ptr<ModelAPI_Feature> theFeature,
           std::set<std::shared_ptr<ModelAPI_Feature> >& theRefs,
           const bool isSendError));
  MOCK_METHOD1(removeFeature,
      void(std::shared_ptr<ModelAPI_Feature> theFeature));
  MOCK_METHOD2(moveFeature,
      void(std::shared_ptr<ModelAPI_Feature> theMoved, std::shared_ptr<ModelAPI_Feature> theAfterThis));
  MOCK_CONST_METHOD0(id,
      const int());
  MOCK_METHOD2(object,
      std::shared_ptr<ModelAPI_Object>(const std::string& theGroupID, const int theIndex));
  MOCK_METHOD2(objectByName,
      std::shared_ptr<ModelAPI_Object>(const std::string& theGroupID, const std::string& theName));
  MOCK_METHOD1(index,
      const int(std::shared_ptr<ModelAPI_Object> theObject));
  MOCK_METHOD1(size,
      int(const std::string& theGroupID));
  MOCK_METHOD1(currentFeature,
      std::shared_ptr<ModelAPI_Feature>(const bool theVisible));
  MOCK_METHOD2(setCurrentFeature,
      void(std::shared_ptr<ModelAPI_Feature> theCurrent, const bool theVisible));
  MOCK_METHOD0(setCurrentFeatureUp,
      void());
  MOCK_METHOD0(numInternalFeatures,
      int());
  MOCK_METHOD1(internalFeature,
      std::shared_ptr<ModelAPI_Feature>(const int theIndex));
  MOCK_METHOD0(synchronizeTransactions,
      void());
  MOCK_METHOD1(featureById,
      std::shared_ptr<ModelAPI_Feature>(const int theId));
  MOCK_METHOD2(createConstruction,
      std::shared_ptr<ModelAPI_ResultConstruction>(const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex));
  MOCK_METHOD2(createBody,
      std::shared_ptr<ModelAPI_ResultBody>(const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex));
  MOCK_METHOD2(createPart,
      std::shared_ptr<ModelAPI_ResultPart>(const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex));
  MOCK_METHOD3(copyPart,
      std::shared_ptr<ModelAPI_ResultPart>(const std::shared_ptr<ModelAPI_ResultPart>& theOrigin,
                                           const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex));
  MOCK_METHOD2(createGroup,
      std::shared_ptr<ModelAPI_ResultGroup>(const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex));
  MOCK_METHOD2(createParameter,
      std::shared_ptr<ModelAPI_ResultParameter>(const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex));
  MOCK_METHOD1(feature,
      std::shared_ptr<ModelAPI_Feature>(const std::shared_ptr<ModelAPI_Result>& theResult));
  MOCK_METHOD0(allFeatures,
      std::list<std::shared_ptr<ModelAPI_Feature> >());
  MOCK_METHOD1(setActive,
      void(const bool theFlag));
  MOCK_CONST_METHOD0(isActive,
      bool());
  MOCK_METHOD0(isOpened,
      bool());
  MOCK_METHOD2(producedByFeature,
      std::shared_ptr<ModelAPI_Feature>(std::shared_ptr<ModelAPI_Result> theResult, const std::shared_ptr<GeomAPI_Shape>& theShape));
  MOCK_CONST_METHOD2(isLater,
      bool(std::shared_ptr<ModelAPI_Feature> theLater, std::shared_ptr<ModelAPI_Feature> theCurrent));
  MOCK_METHOD1(updateHistory,
      void(const std::shared_ptr<ModelAPI_Object> theObject));
  MOCK_METHOD1(updateHistory,
      void(const std::string theGroup));
};

#endif // MockMockModelAPI_Document_H_

