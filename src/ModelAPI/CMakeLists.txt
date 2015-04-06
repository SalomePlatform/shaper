## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    ModelAPI.h
    ModelAPI_Attribute.h
    ModelAPI_AttributeBoolean.h
    ModelAPI_AttributeIntArray.h
    ModelAPI_AttributeDocRef.h
    ModelAPI_AttributeDouble.h
    ModelAPI_AttributeInteger.h
    ModelAPI_AttributeRefAttr.h
    ModelAPI_AttributeReference.h
    ModelAPI_AttributeRefList.h
    ModelAPI_AttributeSelection.h
    ModelAPI_AttributeSelectionList.h
    ModelAPI_AttributeString.h
    ModelAPI_AttributeValidator.h
    ModelAPI_CompositeFeature.h
    ModelAPI_Data.h
    ModelAPI_Document.h
    ModelAPI_Events.h
    ModelAPI_Feature.h
    ModelAPI_FeatureValidator.h
    ModelAPI_Object.h
    ModelAPI_Plugin.h
    ModelAPI_Result.h
    ModelAPI_ResultBody.h
    ModelAPI_ResultConstruction.h
    ModelAPI_ResultGroup.h
    ModelAPI_ResultParameter.h
    ModelAPI_ResultPart.h
    ModelAPI_Session.h
    ModelAPI_Tools.h
    ModelAPI_ShapeValidator.h
    ModelAPI_Validator.h
)

SET(PROJECT_SOURCES
    ModelAPI_Attribute.cpp
    ModelAPI_AttributeBoolean.cpp
    ModelAPI_AttributeIntArray.cpp
    ModelAPI_AttributeDocRef.cpp
    ModelAPI_AttributeDouble.cpp
    ModelAPI_AttributeInteger.cpp
    ModelAPI_AttributeRefAttr.cpp
    ModelAPI_AttributeReference.cpp
    ModelAPI_AttributeRefList.cpp
    ModelAPI_AttributeSelection.cpp
    ModelAPI_AttributeSelectionList.cpp
    ModelAPI_AttributeString.cpp
    ModelAPI_CompositeFeature.cpp
    ModelAPI_Data.cpp
    ModelAPI_Document.cpp
    ModelAPI_Events.cpp
    ModelAPI_Feature.cpp
    ModelAPI_Object.cpp
    ModelAPI_Plugin.cpp
    ModelAPI_Result.cpp
    ModelAPI_ResultBody.cpp
    ModelAPI_ResultConstruction.cpp
    ModelAPI_ResultGroup.cpp
    ModelAPI_ResultPart.cpp
    ModelAPI_Session.cpp
    ModelAPI_ShapeValidator.cpp
    ModelAPI_Tools.cpp
)

SET(PROJECT_LIBRARIES
    Config
)
SET(CMAKE_SWIG_FLAGS -threads -Wall)
ADD_DEFINITIONS(-DMODELAPI_EXPORTS)

ADD_LIBRARY(ModelAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
SET_TARGET_PROPERTIES(ModelAPI PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(ModelAPI ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Config 
                    ${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
)


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
               TestUndoRedo.py
               TestDocument.py)
