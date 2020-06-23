# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

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
    ModelAPI_AttributeDoubleArray.h
    ModelAPI_AttributeInteger.h
    ModelAPI_AttributeRefAttr.h
    ModelAPI_AttributeReference.h
    ModelAPI_AttributeRefAttrList.h
    ModelAPI_AttributeRefList.h
    ModelAPI_AttributeSelection.h
    ModelAPI_AttributeSelectionList.h
    ModelAPI_AttributeString.h
    ModelAPI_AttributeStringArray.h
    ModelAPI_AttributeTables.h
    ModelAPI_AttributeValidator.h
    ModelAPI_BodyBuilder.h
    ModelAPI_CompositeFeature.h
    ModelAPI_Data.h
    ModelAPI_Document.h
    ModelAPI_Entity.h
    ModelAPI_EventReentrantMessage.h
    ModelAPI_Events.h
    ModelAPI_Expression.h
    ModelAPI_Feature.h
    ModelAPI_FeatureValidator.h
    ModelAPI_Filter.h
    ModelAPI_FiltersArgs.h
    ModelAPI_FiltersFactory.h
    ModelAPI_FiltersFeature.h
    ModelAPI_Folder.h
    ModelAPI_IReentrant.h
    ModelAPI_Object.h
    ModelAPI_Plugin.h
    ModelAPI_Result.h
    ModelAPI_ResultBody.h
    ModelAPI_ResultConstruction.h
    ModelAPI_ResultField.h
    ModelAPI_ResultGroup.h
    ModelAPI_ResultParameter.h
    ModelAPI_ResultPart.h
    ModelAPI_Session.h
    ModelAPI_Tools.h
    ModelAPI_Validator.h
)

SET(PROJECT_SOURCES
    ModelAPI_Attribute.cpp
    ModelAPI_AttributeBoolean.cpp
    ModelAPI_AttributeIntArray.cpp
    ModelAPI_AttributeDocRef.cpp
    ModelAPI_AttributeDouble.cpp
    ModelAPI_AttributeDoubleArray.cpp
    ModelAPI_AttributeInteger.cpp
    ModelAPI_AttributeRefAttr.cpp
    ModelAPI_AttributeReference.cpp
    ModelAPI_AttributeRefAttrList.cpp
    ModelAPI_AttributeRefList.cpp
    ModelAPI_AttributeSelection.cpp
    ModelAPI_AttributeSelectionList.cpp
    ModelAPI_AttributeString.cpp
    ModelAPI_AttributeStringArray.cpp
    ModelAPI_AttributeTables.cpp
    ModelAPI_AttributeValidator.cpp
    ModelAPI_BodyBuilder.cpp
    ModelAPI_CompositeFeature.cpp
    ModelAPI_Data.cpp
    ModelAPI_Document.cpp
    ModelAPI_EventReentrantMessage.cpp
    ModelAPI_Events.cpp
    ModelAPI_Expression.cpp
    ModelAPI_Feature.cpp
    ModelAPI_FeatureValidator.cpp
    ModelAPI_Filter.cpp
    ModelAPI_Folder.cpp
    ModelAPI_IReentrant.cpp
    ModelAPI_Object.cpp
    ModelAPI_Plugin.cpp
    ModelAPI_Result.cpp
    ModelAPI_ResultBody.cpp
    ModelAPI_ResultConstruction.cpp
    ModelAPI_ResultField.cpp
    ModelAPI_ResultGroup.cpp
    ModelAPI_ResultPart.cpp
    ModelAPI_ResultParameter.cpp
    ModelAPI_Session.cpp
    ModelAPI_Tools.cpp
)

SET(PROJECT_LIBRARIES
    Config
    GeomAPI
)
SET(CMAKE_SWIG_FLAGS -threads -w325,321,362,383,302,403,451,473)
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

IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(ModelAPI LANGUAGE python SOURCES ModelAPI.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(ModelAPI python ModelAPI.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(ModelAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_ModelAPI PROPERTIES DEBUG_OUTPUT_NAME _ModelAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS _ModelAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(TARGETS ModelAPI DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${SWIG_SCRIPTS} DESTINATION ${SHAPER_INSTALL_SWIG})

ADD_UNIT_TESTS(TestConstants.py
               TestUndoRedo.py
               TestDocument.py
               Test1064.py
               TestResults.py
               TestIntArray.py
               Test1512.py
               Test1562.py
               TestDoubleArray.py
               Test1757.py
               Test1998.py
               Test1995.py
               Test2170.py
               TestExternalConstruction.py
               Test2228.py
               Test2241.py
               Test2252.py
               Test2276.py
               Test2389.py
               Test2391.py
               TestCustomName_BooleanCut.py
               TestCustomName_CommonCompSolid.py
               TestCustomName_CutCompSolid.py
               TestCustomName_CutGroup.py
               TestCustomName_DefaultName.py
               TestCustomName_ExtrudeFace.py
               TestCustomName_ExtrusionCut.py
               TestCustomName_ExtrusionCutFace.py
               TestCustomName_MultiTranslation.py
               TestCustomName_Partition.py
               TestCustomName_Placement.py
               TestCustomName_Recover.py
               TestCustomName_Rename.py
               TestCustomName_RotateGroup.py
               TestCustomName_Translation.py
               TestFolder_Create.py
               TestFolder_Update.py
               TestFolder_Remove.py
               TestFolder_Stability.py
               TestFolder_CustomName.py
               TestFolder_Empty.py
               TestFolder_Sketch.py
               Test2358_1.py
               Test2358_2.py
               Test2396.py
               Test2401.py
               Test2413.py
               Test2496.py
               Test2510.py
               TestFeatureSelection_1.py
               TestFeatureSelection_2.py
               Test2607.py
               TestBlockRecalculation.py
               TestSelectionScope.py
               TestSelectorShell.py
               TestWeakNaming2125.py
               TestWeakNaming2222.py
               TestWeakNaming2245.py
               TestWeakNaming2247.py
               TestWeakNaming2248.py
               TestWeakNaming2261.py
               TestWeakNaming2291.py
               TestWeakNaming2335.py
               TestWeakNaming2375.py
               TestWeakNaming2437.py
               TestWeakNaming2445.py
               TestWeakNaming2446.py
               TestWeakNaming2452.py
               TestWeakNamingSortOrder.py
               Test2685.py
               TestGeomNamingPlacement.py
               TestGeomNamingRevolution.py
               TestGeomNamingEdgeByFilter.py
               TestGeomNamingSketchPlane.py
               TestGeomNamingBoxWithFillet.py
               TestContainerSelector.py
               TestSaveOpen1.py
               TestSaveOpen2.py
               TestSelectionInitialization.py
               TestSelectionCircleCenter.py
               TestSelectionInPart.py
               Test2828.py
               TestSelectionRestore.py
               Test2491.py
               Test2493.py
               Test2627.py
               Test2859.py
               Test2873.py
               Test2901.py
               Test2903.py
               Test3020.py
               Test3116.py
               Test19019_1.py
               Test19019_2.py
               Test19031.py
               Test19058.py
               Test19217.py
               Test19707.py
)
