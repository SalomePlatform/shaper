# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

INCLUDE(Common)
INCLUDE(UseQtExt)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(${QT_INCLUDES})

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(UPDATE_TRANSLATION OFF)

SET(PROJECT_HEADERS
  ModuleBase.h
  ModuleBase_ActionInfo.h
  ModuleBase_ActionType.h
  ModuleBase_BRepOwner.h
  ModuleBase_Definitions.h
  ModuleBase_DoubleSpinBox.h
  ModuleBase_Events.h
  ModuleBase_Filter.h
  ModuleBase_FilterValidated.h
  ModuleBase_IErrorMgr.h
  ModuleBase_IModule.h
  ModuleBase_IntSpinBox.h
  ModuleBase_IPrefMgr.h
  ModuleBase_IPropertyPanel.h
  ModuleBase_ISelection.h
  ModuleBase_ISelectionActivate.h
  ModuleBase_IViewWindow.h
  ModuleBase_IViewer.h
  ModuleBase_IWidgetCreator.h
  ModuleBase_IWorkshop.h
  ModuleBase_LabelValue.h
  ModuleBase_ListView.h
  ModuleBase_ModelWidget.h
  ModuleBase_Operation.h
  ModuleBase_OperationAction.h
  ModuleBase_OperationDescription.h
  ModuleBase_OperationFeature.h
  ModuleBase_PageBase.h
  ModuleBase_PageGroupBox.h
  ModuleBase_PageWidget.h
  ModuleBase_PagedContainer.h
  ModuleBase_ParamIntSpinBox.h
  ModuleBase_ParamSpinBox.h
  ModuleBase_Preferences.h
  ModuleBase_ResultPrs.h
  ModuleBase_SelectionValidator.h
  ModuleBase_ToolBox.h
  ModuleBase_Tools.h
  ModuleBase_ViewerFilters.h
  ModuleBase_ViewerPrs.h
  ModuleBase_WidgetAction.h
  ModuleBase_WidgetBoolValue.h
  ModuleBase_WidgetChoice.h
  ModuleBase_WidgetConcealedObjects.h
  ModuleBase_WidgetCreatorFactory.h
  ModuleBase_WidgetDoubleValue.h
  ModuleBase_WidgetEditor.h
  ModuleBase_WidgetExprEditor.h
  ModuleBase_WidgetFactory.h
  ModuleBase_WidgetFeatureSelector.h
  ModuleBase_WidgetFileSelector.h
  ModuleBase_WidgetIntValue.h
  ModuleBase_WidgetLabel.h
  ModuleBase_WidgetLabelValue.h
  ModuleBase_WidgetLineEdit.h
  ModuleBase_WidgetMultiSelector.h
  ModuleBase_WidgetOptionalBox.h
  ModuleBase_WidgetSelector.h
  ModuleBase_WidgetSelectorStore.h
  ModuleBase_WidgetShapeSelector.h
  ModuleBase_WidgetSwitch.h
  ModuleBase_WidgetToolbox.h
  ModuleBase_WidgetValidated.h
  ModuleBase_WidgetValidator.h
  ModuleBase_IconFactory.h
  ModuleBase_Dialog.h
  ModuleBase_ModelDialogWidget.h
  ModuleBase_ActionParameter.h
  ModuleBase_ActionIntParameter.h
  ModuleBase_ChoiceCtrl.h
  ModuleBase_WidgetNameEdit.h
  ModuleBase_WidgetRadiobox.h
  ModuleBase_WidgetPointInput.h
  ModuleBase_ITreeNode.h
  ModuleBase_WidgetSelectionFilter.h
  ModuleBase_IStepPrs.h
  ModuleBase_SelectionFilterType.h
)

SET(PROJECT_MOC_HEADERS
  ModuleBase_Dialog.h
  ModuleBase_DoubleSpinBox.h
  ModuleBase_IErrorMgr.h
  ModuleBase_IModule.h
  ModuleBase_IntSpinBox.h
  ModuleBase_IPropertyPanel.h
  ModuleBase_IViewer.h
  ModuleBase_IWorkshop.h
  ModuleBase_LabelValue.h
  ModuleBase_ListView.h
  ModuleBase_ModelDialogWidget.h
  ModuleBase_ModelWidget.h
  ModuleBase_Operation.h
  ModuleBase_OperationAction.h
  ModuleBase_OperationFeature.h
  ModuleBase_PagedContainer.h
  ModuleBase_PageGroupBox.h
  ModuleBase_PageWidget.h
  ModuleBase_ParamIntSpinBox.h
  ModuleBase_ParamSpinBox.h
  ModuleBase_Preferences.h
  ModuleBase_ToolBox.h
  ModuleBase_WidgetAction.h
  ModuleBase_WidgetBoolValue.h
  ModuleBase_WidgetChoice.h
  ModuleBase_WidgetConcealedObjects.h
  ModuleBase_WidgetDoubleValue.h
  ModuleBase_WidgetEditor.h
  ModuleBase_WidgetExprEditor.h
  ModuleBase_WidgetFeatureSelector.h
  ModuleBase_WidgetFileSelector.h
  ModuleBase_WidgetIntValue.h
  ModuleBase_WidgetLabel.h
  ModuleBase_WidgetLabelValue.h
  ModuleBase_WidgetLineEdit.h
  ModuleBase_WidgetMultiSelector.h
  ModuleBase_WidgetOptionalBox.h
  ModuleBase_WidgetSelector.h
  ModuleBase_WidgetShapeSelector.h
  ModuleBase_WidgetSwitch.h
  ModuleBase_WidgetToolbox.h
  ModuleBase_WidgetValidated.h
  ModuleBase_ChoiceCtrl.h
  ModuleBase_WidgetNameEdit.h
  ModuleBase_WidgetRadiobox.h
  ModuleBase_WidgetPointInput.h
  ModuleBase_WidgetSelectionFilter.h
)

SET(PROJECT_SOURCES
  ModuleBase_ActionInfo.cpp
  ModuleBase_BRepOwner.cpp
  ModuleBase_DoubleSpinBox.cpp
  ModuleBase_Filter.cpp
  ModuleBase_FilterValidated.cpp
  ModuleBase_IErrorMgr.cpp
  ModuleBase_IModule.cpp
  ModuleBase_IntSpinBox.cpp
  ModuleBase_IPrefMgr.cpp
  ModuleBase_IPropertyPanel.cpp
  ModuleBase_ISelection.cpp
  ModuleBase_IViewWindow.cpp
  ModuleBase_IViewer.cpp
  ModuleBase_IWidgetCreator.cpp
  ModuleBase_IWorkshop.cpp
  ModuleBase_LabelValue.cpp
  ModuleBase_ListView.cpp
  ModuleBase_ModelWidget.cpp
  ModuleBase_Operation.cpp
  ModuleBase_OperationAction.cpp
  ModuleBase_OperationDescription.cpp
  ModuleBase_OperationFeature.cpp
  ModuleBase_PageBase.cpp
  ModuleBase_PageGroupBox.cpp
  ModuleBase_PageWidget.cpp
  ModuleBase_PagedContainer.cpp
  ModuleBase_ParamIntSpinBox.cpp
  ModuleBase_ParamSpinBox.cpp
  ModuleBase_Preferences.cpp
  ModuleBase_ResultPrs.cpp
  ModuleBase_ToolBox.cpp
  ModuleBase_Tools.cpp
  ModuleBase_ViewerFilters.cpp
  ModuleBase_ViewerPrs.cpp
  ModuleBase_WidgetAction.cpp
  ModuleBase_WidgetBoolValue.cpp
  ModuleBase_WidgetChoice.cpp
  ModuleBase_WidgetConcealedObjects.cpp
  ModuleBase_WidgetCreatorFactory.cpp
  ModuleBase_WidgetDoubleValue.cpp
  ModuleBase_WidgetEditor.cpp
  ModuleBase_WidgetExprEditor.cpp
  ModuleBase_WidgetFactory.cpp
  ModuleBase_WidgetFeatureSelector.cpp
  ModuleBase_WidgetFileSelector.cpp
  ModuleBase_WidgetIntValue.cpp
  ModuleBase_WidgetLabel.cpp
  ModuleBase_WidgetLabelValue.cpp
  ModuleBase_WidgetLineEdit.cpp
  ModuleBase_WidgetMultiSelector.cpp
  ModuleBase_WidgetOptionalBox.cpp
  ModuleBase_WidgetSelector.cpp
  ModuleBase_WidgetSelectorStore.cpp
  ModuleBase_WidgetShapeSelector.cpp
  ModuleBase_WidgetSwitch.cpp
  ModuleBase_WidgetToolbox.cpp
  ModuleBase_WidgetValidated.cpp
  ModuleBase_WidgetValidator.cpp
  ModuleBase_IconFactory.cpp
  ModuleBase_SelectionValidator.cpp
  ModuleBase_Dialog.cpp
  ModuleBase_ChoiceCtrl.cpp
  ModuleBase_WidgetNameEdit.cpp
  ModuleBase_WidgetRadiobox.cpp
  ModuleBase_WidgetPointInput.cpp
  ModuleBase_WidgetSelectionFilter.cpp
  ModuleBase_IStepPrs.cpp
)

SET(TEXT_RESOURCES
    ModuleBase_msg_fr.ts
)

SET(PROJECT_LIBRARIES
	Config
	Events
	ModelAPI
	ModelGeomAlgo
	GeomAPI
	GeomDataAPI
	GeomAlgoAPI
	${QT_LIBRARIES}
	${OpenCASCADE_FoundationClasses_LIBRARIES}
	${OpenCASCADE_Visualization_LIBRARIES}
	${suit}
	${qtx}
	${ViewerData}
)

# sources / moc wrappings
QT_WRAP_MOC(PROJECT_AUTOMOC ${PROJECT_MOC_HEADERS})

IF (${UPDATE_TRANSLATION})
    SET(PROJECT_FILES ${PROJECT_SOURCES} ${PROJECT_HEADERS} )
    QT5_CREATE_TRANSLATION(QM_RESOURCES
                           ${PROJECT_FILES}
                           ${TEXT_RESOURCES}
                           OPTIONS -extensions cpp -no-recursive
                          )
ELSE(${UPDATE_TRANSLATION})
    IF(${MAKE_TRANSLATION})
        QT5_ADD_TRANSLATION(QM_RESOURCES ${TEXT_RESOURCES})
    ENDIF(${MAKE_TRANSLATION})
ENDIF(${UPDATE_TRANSLATION})

SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES} ${QM_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES} ${PROJECT_RESOURCES})

INCLUDE_DIRECTORIES(
    ${OpenCASCADE_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/src/Config
    ${PROJECT_SOURCE_DIR}/src/Events
    ${PROJECT_SOURCE_DIR}/src/ModelAPI
    ${PROJECT_SOURCE_DIR}/src/ModelGeomAlgo
    ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
    ${PROJECT_SOURCE_DIR}/src/GeomAPI
    ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
    ${SUIT_INCLUDE}
)

IF(${HAVE_SALOME})
  INCLUDE_DIRECTORIES(${SALOME_KERNEL_INCLUDE})
ENDIF(${HAVE_SALOME})

ADD_DEFINITIONS(-DMODULEBASE_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(ModuleBase SHARED
	${PROJECT_SOURCES}
	${PROJECT_HEADERS}
	${PROJECT_AUTOMOC}
        ${TEXT_RESOURCES}
        ${QM_RESOURCES}
)

TARGET_LINK_LIBRARIES(ModuleBase GeomAPI ${PROJECT_LIBRARIES})

INSTALL(TARGETS ModuleBase DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${QM_RESOURCES} DESTINATION ${SHAPER_INSTALL_QM_RESOURCES})
