## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)
INCLUDE(UseQtExt)

# additional include directories
INCLUDE_DIRECTORIES(${QT_INCLUDES})

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(PROJECT_HEADERS
    PartSet.h
    PartSet_Constants.h
    PartSet_CustomPrs.h
    PartSet_ExternalObjectsMgr.h
    PartSet_FilterInfinite.h
    PartSet_Filters.h
    PartSet_IconFactory.h
    PartSet_MenuMgr.h
    PartSet_Module.h
    PartSet_MouseProcessor.h
    PartSet_OperationPrs.h
    PartSet_OverconstraintListener.h
    PartSet_PreviewPlanes.h
    PartSet_ResultSketchPrs.h
    PartSet_SketcherMgr.h
    PartSet_SketcherReentrantMgr.h
    PartSet_Tools.h
    PartSet_Validators.h
    PartSet_WidgetChoice.h
    PartSet_WidgetEditor.h
    PartSet_WidgetFeaturePointSelector.h
    PartSet_WidgetFileSelector.h
    PartSet_WidgetMultiSelector.h
    PartSet_WidgetPoint2d.h
    PartSet_WidgetPoint2DFlyout.h
    PartSet_WidgetShapeSelector.h
    PartSet_WidgetSketchCreator.h
    PartSet_WidgetSketchLabel.h
)

SET(PROJECT_MOC_HEADERS

    PartSet_MenuMgr.h
    PartSet_Module.h
    PartSet_SketcherMgr.h
    PartSet_SketcherReentrantMgr.h
    PartSet_WidgetChoice.h
    PartSet_WidgetEditor.h
    PartSet_WidgetFeaturePointSelector.h
    PartSet_WidgetFileSelector.h
    PartSet_WidgetMultiSelector.h
    PartSet_WidgetPoint2d.h
    PartSet_WidgetPoint2DFlyout.h
    PartSet_WidgetShapeSelector.h
    PartSet_WidgetSketchCreator.h
    PartSet_WidgetSketchLabel.h
)

SET(PROJECT_SOURCES
    PartSet_CustomPrs.cpp
    PartSet_ExternalObjectsMgr.cpp
    PartSet_FilterInfinite.cpp
    PartSet_Filters.cpp
    PartSet_IconFactory.cpp
    PartSet_MenuMgr.cpp
    PartSet_Module.cpp
    PartSet_OperationPrs.cpp
    PartSet_OverconstraintListener.cpp
    PartSet_PreviewPlanes.cpp
    PartSet_ResultSketchPrs.cpp
    PartSet_SketcherMgr.cpp
    PartSet_SketcherReentrantMgr.cpp
    PartSet_Tools.cpp
    PartSet_Validators.cpp
    PartSet_WidgetEditor.cpp
    PartSet_WidgetFeaturePointSelector.cpp
    PartSet_WidgetFileSelector.cpp
    PartSet_WidgetMultiSelector.cpp
    PartSet_WidgetPoint2d.cpp
    PartSet_WidgetPoint2DFlyout.cpp
    PartSet_WidgetShapeSelector.cpp
    PartSet_WidgetSketchCreator.cpp
    PartSet_WidgetSketchLabel.cpp
)

SET(PROJECT_RESOURCES
    PartSet_icons.qrc
)

#SET(TEXT_RESOURCES
#    PartSet_msg_fr.ts
#)

SET(PROJECT_LIBRARIES
    ModuleBase
    ModelGeomAlgo
    Config
    GeomAPI
    GeomDataAPI
    SketcherPrs
    ${QT_LIBRARIES}
    ${CAS_KERNEL}
    ${CAS_SHAPE}
)

# sources / moc wrappings
QT_WRAP_MOC(PROJECT_AUTOMOC ${PROJECT_MOC_HEADERS})

# sources / rcc wrappings
QT_ADD_RESOURCES(PROJECT_COMPILED_RESOURCES ${PROJECT_RESOURCES})
#QT4_ADD_TRANSLATION(QM_RESOURCES ${TEXT_RESOURCES})
#QT4_CREATE_TRANSLATION(QM_RESOURCES
#                       ${PROJECT_SOURCES}
#                       ${TEXT_RESOURCES}
#                       OPTIONS -extensions cpp -no-recursive
#                       )

#SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES} ${QM_RESOURCES})
SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES})
#SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES} ${PROJECT_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${PROJECT_RESOURCES})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/XGUI
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/ModuleBase
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/ModelGeomAlgo
                    ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
                    ${PROJECT_SOURCE_DIR}/src/SketchPlugin
                    ${PROJECT_SOURCE_DIR}/src/ExchangePlugin
                    ${PROJECT_SOURCE_DIR}/src/SketcherPrs
                    ${PROJECT_SOURCE_DIR}/src/FeaturesPlugin
                    ${PROJECT_SOURCE_DIR}/src/PartSetPlugin
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${CAS_INCLUDE_DIRS}
                    ${SUIT_INCLUDE}
)

IF(${HAVE_SALOME})
  INCLUDE_DIRECTORIES(${SALOME_KERNEL_INCLUDE})
ELSE(${HAVE_SALOME})
  INCLUDE_DIRECTORIES(${APPELEMENTS_INCLUDE_DIR})
ENDIF(${HAVE_SALOME})


ADD_DEFINITIONS(-DPARTSET_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(PartSet SHARED
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_COMPILED_RESOURCES}
    ${PROJECT_AUTOMOC}
#    ${TEXT_RESOURCES}
#    ${QM_RESOURCES}
)

# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
TARGET_LINK_LIBRARIES(PartSet ${PROJECT_LIBRARIES} XGUI ModelAPI GeomAlgoAPI)

ADD_DEPENDENCIES(PartSet ModuleBase)

INSTALL(TARGETS PartSet DESTINATION ${SHAPER_INSTALL_BIN})
#INSTALL(FILES ${QM_RESOURCES} DESTINATION bin)
