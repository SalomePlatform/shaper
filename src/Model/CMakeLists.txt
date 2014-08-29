INCLUDE(Common)

SET(PROJECT_HEADERS
    Model.h
    Model_Application.h
    Model_Document.h
    Model_PluginManager.h
    Model_Data.h
    Model_AttributeDouble.h
    Model_AttributeDocRef.h
    Model_AttributeReference.h
    Model_AttributeRefAttr.h
    Model_AttributeRefList.h
    Model_AttributeBoolean.h
    Model_AttributeString.h
    Model_Events.h
    Model_Update.h
    Model_Validator.h
    Model_ResultBody.h
    Model_ResultConstruction.h
    Model_ResultPart.h
	Model_ResultValidators.h
	Model_FeatureValidator.h
)

SET(PROJECT_SOURCES
    Model_Application.cpp
    Model_Document.cpp
    Model_PluginManager.cpp
    Model_Data.cpp
    Model_AttributeDouble.cpp
    Model_AttributeDocRef.cpp
    Model_AttributeReference.cpp
    Model_AttributeRefAttr.cpp
    Model_AttributeRefList.cpp
    Model_AttributeBoolean.cpp
    Model_AttributeString.cpp
    Model_Events.cpp
    Model_Update.cpp
    Model_Validator.cpp
    Model_ResultBody.cpp
    Model_ResultConstruction.cpp
    Model_ResultPart.cpp
	Model_ResultValidators.cpp
	Model_FeatureValidator.cpp
)

SET(PROJECT_LIBRARIES
    ModelAPI 
    Events 
    Config 
    GeomData
    GeomAPI
    ${CAS_OCAF}
    ${CAS_TKCAF}
)



ADD_DEFINITIONS(-DMODEL_EXPORTS ${CAS_DEFINITIONS} ${BOOST_DEFINITIONS})
ADD_LIBRARY(Model SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(Model ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../Events
  ../Config
  ../GeomData
  ../GeomDataAPI
  ../GeomAPI
  ${CAS_INCLUDE_DIRS}
)

INSTALL(TARGETS Model DESTINATION bin)
