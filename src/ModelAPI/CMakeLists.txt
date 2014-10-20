FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    ModelAPI.h
    ModelAPI_Session.h
    ModelAPI_Plugin.h
    ModelAPI_Feature.h
    ModelAPI_CompositeFeature.h
    ModelAPI_Data.h
    ModelAPI_Object.h
    ModelAPI_Document.h
    ModelAPI_Attribute.h
    ModelAPI_AttributeInteger.h
    ModelAPI_AttributeDouble.h
    ModelAPI_AttributeDocRef.h
    ModelAPI_AttributeReference.h
    ModelAPI_AttributeRefAttr.h
    ModelAPI_AttributeRefList.h
    ModelAPI_AttributeBoolean.h
    ModelAPI_AttributeString.h
    ModelAPI_AttributeSelection.h
    ModelAPI_Events.h
    ModelAPI_Validator.h
    ModelAPI_FeatureValidator.h
    ModelAPI_Result.h
    ModelAPI_ResultBody.h
    ModelAPI_ResultConstruction.h
    ModelAPI_ResultPart.h
    ModelAPI_ResultParameters.h
    ModelAPI_ResultValidator.h
    ModelAPI_AttributeValidator.h
    ModelAPI_Tools.h
    ModelAPI_RefAttrValidator.h 
)

SET(PROJECT_SOURCES
    ModelAPI_Feature.cpp
    ModelAPI_Session.cpp
    ModelAPI_Tools.cpp
)

SET(PROJECT_LIBRARIES
    Config
)

ADD_DEFINITIONS(-DMODELAPI_EXPORTS)
ADD_LIBRARY(ModelAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
SET_TARGET_PROPERTIES(ModelAPI PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(ModelAPI ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ../Config 
  ../Events
  ../GeomAPI
)

SET(CMAKE_SWIG_FLAGS "")

SET_SOURCE_FILES_PROPERTIES(ModelAPI.i PROPERTIES CPLUSPLUS ON)
# "-includeall" is not needed: it starts to follow the standard inludes (like "string") without success
# SET_SOURCE_FILES_PROPERTIES(ModelAPI.i PROPERTIES SWIG_FLAGS)
SET_SOURCE_FILES_PROPERTIES(ModelAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")
#SET_SOURCE_FILES_PROPERTIES(ModelAPIPYTHON_wrap.cxx PROPERTIES COMPILE_FLAGS "-D_WIN32")

SET(SWIG_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/ModelAPI.py
)

SET(SWIG_LINK_LIBRARIES
  ModelAPI
  GeomAPI 
  ${PYTHON_LIBRARIES}
)

SWIG_ADD_MODULE(ModelAPI python ModelAPI.i ${PROJECT_HEADERS})
SWIG_LINK_LIBRARIES(ModelAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_ModelAPI PROPERTIES DEBUG_OUTPUT_NAME _ModelAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS _ModelAPI DESTINATION swig)
INSTALL(TARGETS ModelAPI DESTINATION bin)
INSTALL(FILES ${SWIG_SCRIPTS} DESTINATION swig)

ADD_UNIT_TESTS(TestConstants.py
               TestUndoRedo.py)
