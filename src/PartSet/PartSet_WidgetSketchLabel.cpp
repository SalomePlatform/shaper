// Copyright (C) 2014-2026  CEA, EDF
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

#include "PartSet_WidgetSketchLabel.h"
#include "PartSet_Tools.h"
#include "PartSet_Module.h"
#include "PartSet_PreviewPlanes.h"
#include "PartSet_SketcherReentrantMgr.h"

#include "SketchPlugin_SketchEntity.h"

#include <XGUI_ActionsMgr.h>
#include <XGUI_Displayer.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_SelectionActivate.h>
#include <XGUI_Selection.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_Tools.h>
#include <XGUI_ViewerProxy.h>
#include <XGUI_Workshop.h>

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Events.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_IPropertyPanel.h>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAPI_XYZ.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_ShapeExplorer.h>

#include <SketchPlugin_Sketch.h>
#include <SketcherPrs_Tools.h>

#include <Precision.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <AIS_Shape.hxx>
#include <Bnd_Box.hxx>

#include <Config_WidgetAPI.h>
#include <Config_PropManager.h>

#include <QLabel>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QDoubleSpinBox>
#include <QDialog>
#include <QTimer>
#include <QToolTip>
#include <QRect>

#include <limits>
#include <iostream>
#include <cmath>
#include <utility>


#ifdef WIN32
#pragma warning(disable : 4456) // for nested foreach
#endif

#ifndef DBL_MAX
#define DBL_MAX 1.7976931348623158e+308
#endif



//=============================================================================
//function : setViewProjection
//purpose  : Set the view projection with a specific plane orientation
//=============================================================================
static void setViewProjection(ModuleBase_IWorkshop* theWorkshop, const GeomPlanePtr thePlane, bool theReversed)
{
  GeomDirPtr aDirection = thePlane->direction();
  GeomDirPtr aXDirection = thePlane->xDirection();
  gp_Dir aNormDir = aDirection->impl<gp_Dir>();
  gp_Dir aXDirPln = aXDirection->impl<gp_Dir>();
  if (theReversed) {
    aNormDir.Reverse();
    aXDirPln.Reverse();
  }

  gp_Dir aYDirPln = aNormDir.Crossed(aXDirPln);

  // Set the view projection and up direction
  theWorkshop->viewer()->setViewProjection(aNormDir.X(), aNormDir.Y(), aNormDir.Z(), aYDirPln.X(), aYDirPln.Y(), aYDirPln.Z());
}


PartSet_WidgetSketchLabel::PartSet_WidgetSketchLabel(QWidget* theParent,
  ModuleBase_IWorkshop* theWorkshop,
  const Config_WidgetAPI* theData,
  const QMap<PartSet_Tools::ConstraintVisibleState, bool>& toShowConstraints
) : ModuleBase_WidgetValidated(theParent, theWorkshop, theData),
  mySketchDataIsModified(false), myOpenTransaction(false), myIsSelection(false)
{
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);

  myStackWidget = new QStackedWidget(this);
  myStackWidget->setContentsMargins(0,0,0,0);
  aLayout->addWidget(myStackWidget);

  // Define label for plane selection
  QWidget* aFirstWgt = new QWidget(this);

  // Size of the View control
  mySizeOfViewWidget = new QWidget(aFirstWgt);
  QHBoxLayout* aSizeLayout = new QHBoxLayout(mySizeOfViewWidget);
  aSizeLayout->addWidget(new QLabel(tr("Size of the view"), mySizeOfViewWidget));
  mySizeOfView = new QLineEdit(mySizeOfViewWidget);

  QDoubleValidator* aValidator = new QDoubleValidator(0, DBL_MAX, 12, mySizeOfView);
  aValidator->setLocale(ModuleBase_Tools::doubleLocale());
  aValidator->setNotation(QDoubleValidator::StandardNotation);
  mySizeOfView->setValidator(aValidator);
  aSizeLayout->addWidget(mySizeOfView);

  myPartSetMessage = new QDialog(this, Qt::ToolTip);
  myPartSetMessage->setModal(false);
  myPartSetMessage->setStyleSheet("background-color:lightyellow;");
  QVBoxLayout* aMsgLay = new QVBoxLayout(myPartSetMessage);
  QString aMsg = tr("The Sketch is created in PartSet.\n"
    "It will be necessary to create a Part in order to use this sketch for body creation");
  aMsgLay->addWidget(new QLabel(aMsg, myPartSetMessage));
  myPartSetMessage->hide();

  mySizeMessage = new QDialog(mySizeOfView, Qt::ToolTip);
  mySizeMessage->setModal(false);
  mySizeMessage->setStyleSheet("background-color:lightyellow;");
  aMsgLay = new QVBoxLayout(mySizeMessage);
  aMsg = tr("A size of Sketch view can be defined here.");
  aMsgLay->addWidget(new QLabel(aMsg, mySizeMessage));
  mySizeMessage->hide();

  QString aText = translate(theData->getProperty("title"));
  QLabel* aLabel = new QLabel(aText, aFirstWgt);
  aLabel->setWordWrap(true);
  QString aTooltip = translate(theData->getProperty("tooltip"));
  aLabel->setToolTip(aTooltip);
  aLabel->setIndent(5);

  aLayout = new QVBoxLayout(aFirstWgt);
  ModuleBase_Tools::zeroMargins(aLayout);
  aLayout->addWidget(mySizeOfViewWidget);
  aLayout->addWidget(aLabel);

  myRemoveExternal = new QCheckBox(tr("Remove external dependencies"), aFirstWgt);
  myRemoveExternal->setChecked(false);
  aLayout->addWidget(myRemoveExternal);
  myRemoveExternal->setVisible(false);

  aLayout->addStretch(1);

  myStackWidget->addWidget(aFirstWgt);

  // Define widget for sketch management
  QWidget* aSecondWgt = new QWidget(this);
  aLayout = new QVBoxLayout(aSecondWgt);
  ModuleBase_Tools::zeroMargins(aLayout);

  { // Sketch view controls.
    mySketchViewGroupBox = new QGroupBox(tr("Sketcher plane"), this);
    QGridLayout* aViewLayout = new QGridLayout(mySketchViewGroupBox);

    PartSet_Module* const aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
    PartSet_PreviewSketchPlane* const aPreviewPlane = aModule->sketchMgr()->previewSketchPlane();
    const CompositeFeaturePtr sketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);

    { // Reverse flag.
      myViewInverted = new QCheckBox(tr("Reversed"), mySketchViewGroupBox);
      aViewLayout->addWidget(myViewInverted, 0, 0);
    }

    { // Sketch axes visibility.
      myAxesVisibleCheckBox = new QCheckBox(tr("Axes"), mySketchViewGroupBox);
      myAxesVisibleCheckBox->setChecked(sketch ? PartSet_Tools::sketchPlaneAxesEnabled(sketch)->value() : false);

      aViewLayout->addWidget(myAxesVisibleCheckBox, 0, 1);
      connect(myAxesVisibleCheckBox, SIGNAL(toggled(bool)), this, SLOT(onShowAxes(bool)));
    }

    { // Sketch substrate-plane visibility.
      mySubstrateVisibleCheckBox = new QCheckBox(tr("Show Plane"), mySketchViewGroupBox);
      mySubstrateVisibleCheckBox->setChecked(sketch ? PartSet_Tools::sketchPlaneSubstrateEnabled(sketch)->value() : false);

      aViewLayout->addWidget(mySubstrateVisibleCheckBox, 0, 2);
      connect(mySubstrateVisibleCheckBox, SIGNAL(toggled(bool)), this, SLOT(onShowSubstrate(bool)));
    }

    {
      QPushButton* aSetViewBtn = new QPushButton(QIcon(":icons/plane_view.png"), tr("Set plane view"), mySketchViewGroupBox);
      connect(aSetViewBtn, SIGNAL(clicked(bool)), this, SLOT(onSetPlaneView()));
      aViewLayout->addWidget(aSetViewBtn, 1, 0, 1, 3);
    }

    { // Sketch construction grid.
      QGroupBox* aCGBox = new QGroupBox(tr("Construction grid"), this);
      QGridLayout* aCGLayout = new QGridLayout(aCGBox);

      {
        myGridTypeComboBox = new QComboBox(aCGBox);
        myGridTypeComboBox->addItem(tr("Disabled"),    PartSet_Tools::SketchPlaneGridType::No);
        myGridTypeComboBox->addItem(tr("Rectangular"), PartSet_Tools::SketchPlaneGridType::Rectangular);
        myGridTypeComboBox->addItem(tr("Circular"),    PartSet_Tools::SketchPlaneGridType::Circular);
        myGridTypeComboBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        myGridTypeComboBox->setCurrentIndex(0);
        if (sketch) {
          const int idx = myGridTypeComboBox->findData(PartSet_Tools::getSketchPlaneGridType(sketch));
          if (idx != -1)
            myGridTypeComboBox->setCurrentIndex(idx);
        }

        aCGLayout->addWidget(myGridTypeComboBox, 0, 0);
        connect(myGridTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onGridTypeChanged(int)));
      }

      {
        myGridSnappingModeComboBox = new QComboBox(aCGBox);
        myGridSnappingModeComboBox->addItem(tr("Don't snap"), PartSet_PreviewSketchPlane::GridSnappingMode::Off);
        myGridSnappingModeComboBox->addItem(tr("Snap anyway"), PartSet_PreviewSketchPlane::GridSnappingMode::SnapAnyway);
        myGridSnappingModeComboBox->addItem(tr("Snap in proximity"), PartSet_PreviewSketchPlane::GridSnappingMode::SnapInProximity);
        myGridSnappingModeComboBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        myGridSnappingModeComboBox->setCurrentIndex(int(aPreviewPlane->getGridSnappingMode()));

        aCGLayout->addWidget(myGridSnappingModeComboBox, 0, 1);
        connect(myGridSnappingModeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onGridSnappingModeChanged(int)));
      }

      {
        const auto widgetGrouper = new QWidget(aCGBox);
        aCGLayout->addWidget(widgetGrouper, 1, 0, 1, 2);
        const auto grouperLayout = new QVBoxLayout(widgetGrouper);

        const auto currentGridType = myGridTypeComboBox->currentData();

        myWidgetRectangularGrid = new PartSet_WidgetSketchRectangularGrid(aCGBox, this);
        grouperLayout->addWidget(myWidgetRectangularGrid);
        myWidgetRectangularGrid->setVisible(currentGridType == PartSet_Tools::SketchPlaneGridType::Rectangular);

        myWidgetCircularGrid = new PartSet_WidgetSketchCircularGrid(aCGBox, this);
        grouperLayout->addWidget(myWidgetCircularGrid);
        myWidgetCircularGrid->setVisible(currentGridType == PartSet_Tools::SketchPlaneGridType::Circular);
      }

      aViewLayout->addWidget(aCGBox, 2, 0, 1, 3);
    } // Sketch construction grid.

    aLayout->addWidget(mySketchViewGroupBox);

    aPreviewPlane->setAllUsingSketch(sketch);
    reconfigureSketchViewWidgets();
  } // View box.


  QMap<PartSet_Tools::ConstraintVisibleState, QString> aStates;
  aStates[PartSet_Tools::Geometrical] = tr("Show geometrical constraints");
  aStates[PartSet_Tools::Dimensional] = tr("Show dimensional constraints");
  aStates[PartSet_Tools::Expressions] = tr("Show existing expressions");

  QMap<PartSet_Tools::ConstraintVisibleState, QString>::const_iterator anIt = aStates.begin(),
                                                        aLast = aStates.end();
  for (; anIt != aLast; anIt++) {
    QCheckBox* aShowConstraints = new QCheckBox(anIt.value(), this);
    connect(aShowConstraints, SIGNAL(toggled(bool)), this, SLOT(onShowConstraint(bool)));
    aLayout->addWidget(aShowConstraints);

    PartSet_Tools::ConstraintVisibleState aState = anIt.key();
    myShowConstraints[aState] = aShowConstraints;

    if (toShowConstraints.contains(aState))
      aShowConstraints->setChecked(toShowConstraints[aState]);
  }
  myShowPoints = new QCheckBox(tr("Show free points"), this);
  connect(myShowPoints, SIGNAL(toggled(bool)), this, SIGNAL(showFreePoints(bool)));
  aLayout->addWidget(myShowPoints);

  myAutoConstraints = new QCheckBox(tr("Automatic constraints"), this);
  myAutoConstraints->setToolTip(tr("Automatic vertical and horizontal constraints"));
  connect(myAutoConstraints, SIGNAL(toggled(bool)), this, SIGNAL(autoConstraints(bool)));
  aLayout->addWidget(myAutoConstraints);

  QPushButton* aPlaneBtn = new QPushButton(tr("Change sketch plane"), aSecondWgt);
  connect(aPlaneBtn, SIGNAL(clicked(bool)), SLOT(onChangePlane()));
  aLayout->addWidget(aPlaneBtn);

  aLayout->addSpacing(15);

  myDoFLabel = new QLabel("", aSecondWgt);
  aLayout->addWidget(myDoFLabel);

  myShowDOFBtn = new QPushButton(tr("Show remaining DoFs"), aSecondWgt);
  aLayout->addWidget(myShowDOFBtn);
  myShowDOFBtn->setEnabled(false);
  connect(myShowDOFBtn, SIGNAL(clicked(bool)), SLOT(onShowDOF()));

  myStackWidget->addWidget(aSecondWgt);
  //setLayout(aLayout);

  myPreviewPlanes = new PartSet_PreviewPlanes();
}

void PartSet_WidgetSketchLabel::setFeature(
  const FeaturePtr& theFeature,
  const bool theToStoreValue,
  const bool isUpdateFlushed
) {
  ModuleBase_WidgetValidated::setFeature(theFeature, theToStoreValue, isUpdateFlushed);
  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  const CompositeFeaturePtr sketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  aModule->sketchMgr()->previewSketchPlane()->setAllUsingSketch(sketch);
  reconfigureSketchViewWidgets();
}

bool PartSet_WidgetSketchLabel::setSelection(QList<ModuleBase_ViewerPrsPtr>& theValues,
                                             const bool theToValidate)
{
  // do not use the given selection if the plane of the sketch has been already set.
  // If this check is absent, a selected plane in the viewer can be set in the sketch
  // even if the sketch is built on another plane.
  if (plane().get())
    return true;

  ModuleBase_ViewerPrsPtr aPrs = theValues.first();
  bool aDone = setSelectionInternal(theValues, theToValidate);
  if (aDone)
    updateByPlaneSelected(aPrs);
  return aDone;
}

QList<QWidget*> PartSet_WidgetSketchLabel::getControls() const
{
  QList<QWidget*> aResult;
  aResult << myStackWidget;
  return aResult;
}

bool PartSet_WidgetSketchLabel::processSelection()
{
  std::shared_ptr<GeomAPI_Pln> aPlane = plane();
  if (aPlane.get())
    return false;

  QList<ModuleBase_ViewerPrsPtr> aSelected = getFilteredSelected();

  if (aSelected.empty())
    return false;
  ModuleBase_ViewerPrsPtr aPrs = aSelected.first();
  bool aDone = setSelectionInternal(aSelected, false);
  if (aDone) {
    updateByPlaneSelected(aPrs);
    updateObject(myFeature);
  }

  return aDone;
}

void PartSet_WidgetSketchLabel::onShowConstraint(bool theOn)
{
  QCheckBox* aSenderCheckBox = qobject_cast<QCheckBox*>(sender());

  QMap<PartSet_Tools::ConstraintVisibleState, QCheckBox*>::const_iterator
                          anIt = myShowConstraints.begin(), aLast = myShowConstraints.end();

  PartSet_Tools::ConstraintVisibleState aState = PartSet_Tools::Geometrical;
  bool aFound = false;
  for (; anIt != aLast && !aFound; anIt++) {
    aFound = anIt.value() == aSenderCheckBox;
    if (aFound)
      aState = anIt.key();
  }
  if (aFound)
    emit showConstraintToggled(aState, theOn);
}

void PartSet_WidgetSketchLabel::blockAttribute(const AttributePtr& theAttribute,
                                               const bool& theToBlock, bool& isFlushesActived,
                                               bool& isAttributeSetInitializedBlocked,
                                               bool& isAttributeSendUpdatedBlocked)
{
  ModuleBase_WidgetValidated::blockAttribute(theAttribute, theToBlock, isFlushesActived,
                                             isAttributeSetInitializedBlocked,
                                             isAttributeSendUpdatedBlocked);
  // We do not restore the previous state of isAttributeSetInitializedBlocked for each of
  // attributes. It it is necessary, these states should be append to the method attributes
  // or stored in the widget

  std::list<AttributePtr> anAttributes = myFeature->data()->attributes("");
  std::list<AttributePtr>::const_iterator anIt = anAttributes.begin(), aLast = anAttributes.end();
  QStringList aValues;
  for(; anIt != aLast; anIt++) {
    AttributePtr anAttribute = *anIt;
    if (theToBlock)
      anAttribute->blockSetInitialized(true);
    else
      anAttribute->blockSetInitialized(isAttributeSetInitializedBlocked);
  }
}

bool PartSet_WidgetSketchLabel::setSelectionInternal(
                                          const QList<ModuleBase_ViewerPrsPtr>& theValues,
                                          const bool theToValidate)
{
  bool aDone = false;
  if (theValues.empty()) {
    // In order to make reselection possible, set empty object and shape should be done
    setSelectionCustom(ModuleBase_ViewerPrsPtr(
                              new ModuleBase_ViewerPrs(ObjectPtr(), GeomShapePtr(), NULL)));
    aDone = false;
  }
  else {
    // it removes the processed value from the parameters list
    ModuleBase_ViewerPrsPtr aValue = theValues.first();//.takeFirst();
    if (!theToValidate || isValidInFilters(aValue)) {
      myIsSelection = true;
      aDone = setSelectionCustom(aValue);
      myIsSelection = false;
    }
  }

  return aDone;
}

void PartSet_WidgetSketchLabel::updateByPlaneSelected(const ModuleBase_ViewerPrsPtr& thePrs)
{
  // Nullify a temporary remembered plane
  if (myTmpPlane.get())
    myTmpPlane.reset();

  GeomPlanePtr aPlane = plane();
  if (!aPlane.get())
    return;

  myPartSetMessage->hide();
  mySizeMessage->hide();

  // Set the type of selected object
  enum SelType {
    Invalid = 0, // should never happen
    PreviewPlane = 1,
    InvisibleConstructionPlane = 2,
    VisibleConstructionPlane = 3,
    PlanarFace = 4
  };
  SelType aSelType = SelType::Invalid;
  Bnd_Box aBBox;
  // Get the selected object
  if (thePrs.get())
  {
    // A preview plane has no object attached
    ObjectPtr aObject = thePrs->object();
    if (!aObject.get())
    {
      aSelType = SelType::PreviewPlane;
    }
    else
    {
      // Distinguish between a construction plane and a planar face
      if (aObject->groupName() == ModelAPI_ResultConstruction::group())
      {
        // Get the bounding box of the selected construction plane, if it is displayed
        ResultConstructionPtr aResultConstruction =
          std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aObject);
        if (aResultConstruction.get() && aResultConstruction->isDisplayed())
        {
          aSelType = SelType::VisibleConstructionPlane;
          GeomShapePtr aShape = aResultConstruction->shape();
          Standard_Real aMinX,aMinY,aMinZ,aMaxX,aMaxY,aMaxZ;
          if (aShape.get() && aShape->computeSize(aMinX,aMinY,aMinZ,aMaxX,aMaxY,aMaxZ))
          {
            aBBox.Update(aMinX,aMinY,aMinZ,aMaxX,aMaxY,aMaxZ);
          }
        }
        else
        {
          aSelType = SelType::InvisibleConstructionPlane;
        }
      }
      else if (aObject->groupName() == ModelAPI_ResultBody::group())
      {
        aSelType = SelType::PlanarFace;
      }
    }
  }

  // 1. Hide main planes if they have been displayed and display sketch preview plane
  myPreviewPlanes->erasePreviewPlanes(myWorkshop);

  bool isValidSizeInput = true;
  double aSizeOfView = mySizeOfView->text().toDouble(&isValidSizeInput);
  if (isValidSizeInput || aSelType == SelType::PreviewPlane || aSelType == SelType::InvisibleConstructionPlane)
  {
    // If the size of the view was given by the user or a preview plane was selected,
    // compute the bounding box based on the default view size.
    if (aSizeOfView < Precision::Confusion() || !isValidSizeInput)
    {
      aSizeOfView = PartSet_PreviewSketchPlane::defaultSketchSize();
    }
    double aHalfSize = aSizeOfView/2.0;
    aBBox.SetVoid();
    aBBox.Update(-aHalfSize, -aHalfSize, -aHalfSize, aHalfSize, aHalfSize, aHalfSize);
  }

  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  if (aModule)
  {
    CompositeFeaturePtr sketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
    PartSet_Tools::sketchPlaneDefaultSize(sketch)->setValue(aSizeOfView);
    aModule->sketchMgr()->previewSketchPlane()->setUseSizeOfView(isValidSizeInput);
    aModule->sketchMgr()->previewSketchPlane()->setAllUsingSketch(sketch);
  }
  reconfigureSketchViewWidgets();

  // 2. If the "Rotate to plane when selected" preference is enabled, change the view projection
  bool aRotate = Config_PropManager::boolean(SKETCH_TAB_NAME, "rotate_to_plane");
  if (aRotate)
  {
    bool aReversed = myViewInverted->isChecked();
    setViewProjection(myWorkshop, aPlane, aReversed);
  }

  if (myOpenTransaction)
  {
    SessionPtr aMgr = ModelAPI_Session::get();
    aMgr->finishOperation();
    myOpenTransaction = false;
  }

  // 3. Clear text in the label
  myStackWidget->setCurrentIndex(1);
  //myLabel->setText("");
  //myLabel->setToolTip("");
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);

  // 4. Clear selection mode and define sketching mode
  emit planeSelected(plane());
  // after the plane is selected in the sketch, the sketch selection should be activated
  // it can not be performed in the sketch label widget because, we don't need to switch off
  // the selection by any label deactivation, but need to switch it off by stop the sketch
  myWorkshop->selectionActivate()->updateSelectionFilters();
  myWorkshop->selectionActivate()->updateSelectionModes();

  if (aModule)
  {
    aModule->onViewTransformed();
  }

  myWorkshop->updateCommandStatus();
  aWorkshop->selector()->clearSelection();
  myWorkshop->viewer()->update();

  myRemoveExternal->setVisible(false);

  // Fit the view
  Handle(V3d_View) aView3d = myWorkshop->viewer()->activeView();
  if (!aView3d.IsNull())
  {
    if (aBBox.IsVoid())
    {
      // If no bbox was computed, then fit the view to the entire scene.
      aView3d->FitAll(0.01, false);
    }
    else
    {
      // Fit the computed bounding box from the construction plane
      aView3d->FitAll(aBBox, 0.01, false);
    }
  }
}

std::shared_ptr<GeomAPI_Pln> PartSet_WidgetSketchLabel::plane() const
{
  CompositeFeaturePtr aSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  return PartSet_Tools::sketchPlane(aSketch);
}

bool PartSet_WidgetSketchLabel::focusTo()
{
  ModuleBase_Tools::setFocus(myStackWidget, "PartSet_WidgetSketchLabel::focusTo()");
  return true;
}

void PartSet_WidgetSketchLabel::enableFocusProcessing()
{
  myStackWidget->installEventFilter(this);
}

void PartSet_WidgetSketchLabel::storeAttributeValue(const AttributePtr& theAttribute)
{
  ModuleBase_WidgetValidated::storeAttributeValue(theAttribute);
}

void PartSet_WidgetSketchLabel::restoreAttributeValue(const AttributePtr& theAttribute,
                                                      const bool theValid)
{
  ModuleBase_WidgetValidated::restoreAttributeValue(theAttribute, theValid);

  // it is not necessary to save the previous plane value because the plane is chosen once
  DataPtr aData = feature()->data();
  AttributeSelectionPtr aSelAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
    (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
  if (aSelAttr) {
    ResultPtr anEmptyResult;
    GeomShapePtr anEmptyShape;
    aSelAttr->setValue(anEmptyResult, anEmptyShape);
  }
  std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
    aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  anOrigin->reset();
  std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
    aData->attribute(SketchPlugin_Sketch::NORM_ID()));
  aNormal->reset();
  std::shared_ptr<GeomDataAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
    aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
  aDirX->reset();
}

bool PartSet_WidgetSketchLabel::setSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs)
{
  if (myIsSelection && myRemoveExternal->isVisible()) {
    if (myRemoveExternal->isChecked()) {
      myFeature->customAction(SketchPlugin_Sketch::ACTION_REMOVE_EXTERNAL());
    }
  }
  return fillSketchPlaneBySelection(thePrs);
}

bool PartSet_WidgetSketchLabel::canFillSketch(const ModuleBase_ViewerPrsPtr& thePrs)
{
  bool aCanFillSketch = true;
  // avoid any selection on sketch object
  ObjectPtr anObject = thePrs->object();
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
  if (aResult.get()) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(aResult);
    if (aFeature->getKind() == SketchPlugin_Sketch::ID())
      aCanFillSketch = false;
  }
  // check plane or planar face of any non-sketch object
  if (aCanFillSketch) {
    GeomShapePtr aGeomShape = thePrs->shape();
    if ((!aGeomShape.get() || aGeomShape->isNull()) && aResult.get()) {
      aGeomShape = aResult->shape();
    }

    if (aGeomShape.get() && aGeomShape->shapeType() == GeomAPI_Shape::FACE) {
      std::shared_ptr<GeomAPI_Face> aGeomFace(new GeomAPI_Face(aGeomShape));
      aCanFillSketch = aGeomFace.get() && aGeomFace->isPlanar();
    }
    else
      aCanFillSketch = false;
  }
  return aCanFillSketch;
}

bool PartSet_WidgetSketchLabel::fillSketchPlaneBySelection(const ModuleBase_ViewerPrsPtr& thePrs)
{
  bool isOwnerSet = false;

  const GeomShapePtr& aShape = thePrs->shape();
  std::shared_ptr<GeomAPI_Dir> aDir;

  if (aShape.get() && !aShape->isNull()) {
    const TopoDS_Shape& aTDShape = aShape->impl<TopoDS_Shape>();
    aDir = setSketchPlane(aTDShape);
    isOwnerSet = aDir.get();
  }
  if (thePrs->object() && (feature() != thePrs->object())) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(thePrs->object());
    DataPtr aData = feature()->data();
    AttributeSelectionPtr aSelAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
      (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
    if (aSelAttr.get()) {
      ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(thePrs->object());
      if (aRes.get()) {
        GeomShapePtr aShapePtr;
        if (!aShape.get() || aShape->isNull()) {  // selection happens in the OCC viewer
          aShapePtr = ModelAPI_Tools::shape(aRes);
        }
        else { // selection happens in OB browser
          aShapePtr = aShape;
        }
        if (aShapePtr.get() && aShapePtr->isFace()) {
          const TopoDS_Shape& aTDShape = aShapePtr->impl<TopoDS_Shape>();
          setSketchPlane(aTDShape);
          aSelAttr->setValue(aRes, aShapePtr);
          isOwnerSet = true;
        }
      }
      else {
        aSelAttr->setValue(aFeature, GeomShapePtr());
        GeomShapePtr aSelShape = aSelAttr->value();
        if (!aSelShape.get() && aSelAttr->contextFeature().get() &&
          aSelAttr->contextFeature()->firstResult().get()) {
          aSelShape = aSelAttr->contextFeature()->firstResult()->shape();
        }
        if (aSelShape.get() && aSelShape->isPlanar()) {
          const TopoDS_Shape& aTDShape = aSelShape->impl<TopoDS_Shape>();
          setSketchPlane(aTDShape);
          isOwnerSet = true;
        }
      }
    }
  }
  return isOwnerSet;
}

void PartSet_WidgetSketchLabel::activateCustom()
{
  QWidget* aTopWidget = window();
  if (aTopWidget)
    aTopWidget->installEventFilter(this);

  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  if (aModule) {
    bool isBlocked = myAutoConstraints->blockSignals(true);
    myAutoConstraints->setChecked(aModule->sketchReentranceMgr()->isAutoConstraints());
    myAutoConstraints->blockSignals(isBlocked);
  }

  std::shared_ptr<GeomAPI_Pln> aPlane = plane();
  if (aPlane.get()) {
    myStackWidget->setCurrentIndex(1);
    return;
  }

  myStackWidget->setCurrentIndex(0);
  bool aBodyIsVisualized = myPreviewPlanes->hasVisualizedBodies(myWorkshop);

  // Clear previous selection mode It is necessary for correct activation of preview planes
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
  XGUI_Displayer* aDisp = aWorkshop->displayer();
  aWorkshop->selectionActivate()->activateObjects(QIntList(), aDisp->displayedObjects(), false);

  if (!aBodyIsVisualized) {
    // We have to select a plane before any operation
    myPreviewPlanes->showPreviewPlanes(myWorkshop);
    mySizeOfViewWidget->setVisible(true);
  }
  else
    mySizeOfViewWidget->setVisible(false);
}

void PartSet_WidgetSketchLabel::showEvent(QShowEvent* theEvent)
{
  ModuleBase_WidgetValidated::showEvent(theEvent);
  QTimer::singleShot(10, this, SLOT(onShowPanel()));
}

void PartSet_WidgetSketchLabel::hideEvent(QHideEvent* theEvent)
{
  ModuleBase_WidgetValidated::hideEvent(theEvent);
  if (myPartSetMessage->isVisible())
    myPartSetMessage->hide();
  if (mySizeMessage->isVisible())
    mySizeMessage->hide();
}


void PartSet_WidgetSketchLabel::onShowDOF()
{
  CompositeFeaturePtr aCompFeature =
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  if (aCompFeature.get()) {
    static const Events_ID anEvent = Events_Loop::eventByName(EVENT_GET_DOF_OBJECTS);
    ModelAPI_EventCreator::get()->sendUpdated(aCompFeature, anEvent);
    Events_Loop::loop()->flush(anEvent);

    // Transfer focus to the current viewport for correct processing of a key event
    QWidget* aViewPort = myWorkshop->viewer()->activeViewPort();
    if (aViewPort)
      aViewPort->setFocus();
  }
}


void PartSet_WidgetSketchLabel::onShowPanel()
{
  //if (mySizeOfViewWidget->isVisible()) {
  if (myStackWidget->currentIndex() == 0) {
    DocumentPtr aDoc = feature()->document();
    DocumentPtr aModDoc = ModelAPI_Session::get()->moduleDocument();
    if (aModDoc == aDoc) {
      myPartSetMessage->move(mapToGlobal(geometry().bottomLeft()));
      myPartSetMessage->show();
    }
    if (mySizeOfViewWidget->isVisible()) {
      QPoint aPnt = mySizeOfView->mapToGlobal(mySizeOfView->geometry().center());
      mySizeMessage->move(aPnt);
      mySizeMessage->show();
    }
  }
}

void PartSet_WidgetSketchLabel::onShowAxes(bool toShow)
{
  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(feature());
  if (sketch) {
    PartSet_Tools::sketchPlaneAxesEnabled(sketch)->setValue(toShow);
    mySketchDataIsModified = true;
  }

  const auto module = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  PartSet_PreviewSketchPlane* const previewPlane = module->sketchMgr()->previewSketchPlane();
  previewPlane->showAxes(toShow);
  myWorkshop->viewer()->update();
}

void PartSet_WidgetSketchLabel::onShowSubstrate(bool toShow)
{
  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(feature());
  if (sketch) {
    PartSet_Tools::sketchPlaneSubstrateEnabled(sketch)->setValue(toShow);
    mySketchDataIsModified = true;
  }

  const auto module = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  PartSet_PreviewSketchPlane* const previewPlane = module->sketchMgr()->previewSketchPlane();
  previewPlane->showSubstrate(toShow);
  myWorkshop->viewer()->update();
}

void PartSet_WidgetSketchLabel::onGridTypeChanged(int theComboBoxIdx) {
  (void)theComboBoxIdx;
  const auto module = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  const auto previewPlane = module->sketchMgr()->previewSketchPlane();
  const auto gridType = PartSet_Tools::SketchPlaneGridType::Enum(myGridTypeComboBox->currentData().toInt());

  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(feature());
  if (sketch) {
    PartSet_Tools::setSketchPlaneGridType(sketch, gridType);
    mySketchDataIsModified = true;
  }

  previewPlane->setGridType(gridType);
  myGridSnappingModeComboBox->setEnabled(gridType != PartSet_Tools::SketchPlaneGridType::No);
  myWidgetRectangularGrid->setVisible(gridType == PartSet_Tools::SketchPlaneGridType::Rectangular);
  myWidgetCircularGrid->setVisible(gridType == PartSet_Tools::SketchPlaneGridType::Circular);
  myWorkshop->viewer()->update();
}

void PartSet_WidgetSketchLabel::onGridSnappingModeChanged(int theModeIdx) {
  const auto module = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  PartSet_PreviewSketchPlane* const previewPlane = module->sketchMgr()->previewSketchPlane();
  previewPlane->setGridSnappingMode(static_cast<PartSet_PreviewSketchPlane::GridSnappingMode>(theModeIdx));
}

void PartSet_WidgetSketchLabel::reconfigureSketchViewWidgets()
{
  const auto module = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  if (module) {
    const PartSet_PreviewSketchPlane* const previewPlane = module->sketchMgr()->previewSketchPlane();

    myAxesVisibleCheckBox->setChecked(previewPlane->isShowAxes());
    mySubstrateVisibleCheckBox->setChecked(previewPlane->isShowSubstrate());

    const auto gridType = previewPlane->getGridType();
    const int gridTypeIdx = myGridTypeComboBox->findData(gridType);
    myGridTypeComboBox->setCurrentIndex(gridTypeIdx != -1 ? gridTypeIdx : 0);
    myWidgetRectangularGrid->setVisible(gridType == PartSet_Tools::SketchPlaneGridType::Rectangular);
    myWidgetRectangularGrid->reconfigure();
    myWidgetCircularGrid->setVisible(gridType == PartSet_Tools::SketchPlaneGridType::Circular);
    myWidgetCircularGrid->reconfigure();

    myGridSnappingModeComboBox->setEnabled(gridType != PartSet_Tools::SketchPlaneGridType::No);
    myGridSnappingModeComboBox->setCurrentIndex(int(previewPlane->getGridSnappingMode()));

    mySketchViewGroupBox->setEnabled(true);
  }
  else {
    mySketchViewGroupBox->setEnabled(false);

    myAxesVisibleCheckBox->setChecked(false);
    mySubstrateVisibleCheckBox->setChecked(false);

    myGridTypeComboBox->setCurrentIndex(0);
    myWidgetRectangularGrid->setVisible(false);
    myWidgetRectangularGrid->reconfigure();
    myWidgetCircularGrid->setVisible(false);
    myWidgetCircularGrid->reconfigure();

    myGridSnappingModeComboBox->setEnabled(false);
  }

  mySketchDataIsModified = false;
  myWorkshop->viewer()->update();
}

void PartSet_WidgetSketchLabel::saveSketchViewPreferenceToSkethData()
{
  const auto module = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  if (!module)
    return;

  const PartSet_PreviewSketchPlane* const previewPlane = module->sketchMgr()->previewSketchPlane();
  const auto sketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  previewPlane->savePreferencesIntoSketchData(sketch);
  mySketchDataIsModified = false;
}

void PartSet_WidgetSketchLabel::deactivate()
{
  QWidget* aTopWidget = window();
  if (aTopWidget)
    aTopWidget->removeEventFilter(this);

  // Restore sketch plane before operation finishing
  // to respect transaction margins,
  // as it was nullified in onChangePlane() after operation starting
  if (myTmpPlane.get()) {
    setSketchPlane(myTmpPlane);
    myTmpPlane.reset();
  }

  ModuleBase_WidgetValidated::deactivate();
  bool aHidePreview = myPreviewPlanes->isPreviewDisplayed();
  myPreviewPlanes->erasePreviewPlanes(myWorkshop);

  if (aHidePreview)
    myWorkshop->viewer()->update();

  if (myOpenTransaction) {
    SessionPtr aMgr = ModelAPI_Session::get();
    aMgr->finishOperation();
    myOpenTransaction = false;
  }
}

void PartSet_WidgetSketchLabel::selectionModes(int& theModuleSelectionModes, QIntList& theModes)
{
  theModuleSelectionModes = -1;
  std::shared_ptr<GeomAPI_Pln> aPlane = plane();
  if (!aPlane.get())
    theModes << TopAbs_FACE;
}

void PartSet_WidgetSketchLabel::selectionFilters(QIntList& theModuleSelectionFilters,
                                                 SelectMgr_ListOfFilter& theSelectionFilters)
{
  std::shared_ptr<GeomAPI_Pln> aPlane = plane();
  if (aPlane.get())
    return;
  return ModuleBase_WidgetValidated::selectionFilters(theModuleSelectionFilters,
                                                      theSelectionFilters);
}

std::shared_ptr<GeomAPI_Dir>
  PartSet_WidgetSketchLabel::setSketchPlane(const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
    return std::shared_ptr<GeomAPI_Dir>();

  // get selected shape
  std::shared_ptr<GeomAPI_Shape> aGShape(new GeomAPI_Shape);
  aGShape->setImpl(new TopoDS_Shape(theShape));

  // get plane parameters
  std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(aGShape));
  std::shared_ptr<GeomAPI_Pln> aPlane = aFace->getPlane();
  if (!aPlane.get())
    return std::shared_ptr<GeomAPI_Dir>();
  return setSketchPlane(aPlane);
}

std::shared_ptr<GeomAPI_Dir>
  PartSet_WidgetSketchLabel::setSketchPlane(std::shared_ptr<GeomAPI_Pln> thePlane)
{
  // set plane parameters to feature
  std::shared_ptr<ModelAPI_Data> aData = feature()->data();
  double anA, aB, aC, aD;
  thePlane->coefficients(anA, aB, aC, aD);

  // calculate attributes of the sketch
  std::shared_ptr<GeomAPI_Dir> aNormDir(new GeomAPI_Dir(anA, aB, aC));
  std::shared_ptr<GeomAPI_XYZ> aCoords = aNormDir->xyz();
  std::shared_ptr<GeomAPI_XYZ> aZero(new GeomAPI_XYZ(0, 0, 0));
  aCoords = aCoords->multiplied(-aD * aCoords->distance(aZero));
  std::shared_ptr<GeomAPI_Pnt> anOrigPnt(new GeomAPI_Pnt(aCoords));
  // X axis is preferable to be dirX on the sketch
  const double tol = Precision::Confusion();
  bool isX = fabs(fabs(anA) - 1.0) < tol && fabs(aB) < tol && fabs(aC) < tol;
  std::shared_ptr<GeomAPI_Dir> aTempDir(
      isX ? new GeomAPI_Dir(0, 1, 0) : new GeomAPI_Dir(1, 0, 0));
  std::shared_ptr<GeomAPI_Dir> aYDir(new GeomAPI_Dir(aNormDir->cross(aTempDir)));
  std::shared_ptr<GeomAPI_Dir> aXDir(new GeomAPI_Dir(aYDir->cross(aNormDir)));

  std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  anOrigin->setValue(anOrigPnt);
  std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::NORM_ID()));
  aNormal->setValue(aNormDir);
  std::shared_ptr<GeomDataAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
  aDirX->setValue(aXDir);
  std::shared_ptr<GeomAPI_Dir> aDir = thePlane->direction();
  return aDir;
}

void PartSet_WidgetSketchLabel::onSetPlaneView()
{
  std::shared_ptr<GeomAPI_Pln> aPlane = plane();
  if (aPlane.get()) {
    bool aReversed = myViewInverted->isChecked();
    setViewProjection(myWorkshop, aPlane, aReversed);

    PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
    if (aModule)
      aModule->onViewTransformed();
  }
}

//******************************************************
QList<ModuleBase_ViewerPrsPtr> PartSet_WidgetSketchLabel::findCircularEdgesInPlane()
{
  QList<std::shared_ptr<ModuleBase_ViewerPrs>> aResult;
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
  QObjectPtrList aDispObjects = aDisplayer->displayedObjects();

  std::shared_ptr<GeomAPI_Pln> aPlane = plane();
  foreach(ObjectPtr aObj, aDispObjects) {
    ResultPtr aResObj = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
    if (aResObj.get()) {
      GeomShapePtr aShape = aResObj->shape();
      if (aShape.get()) {
        GeomAPI_ShapeExplorer aExplorer(aShape, GeomAPI_Shape::EDGE);
        for(; aExplorer.more(); aExplorer.next()) {
          GeomShapePtr aEdgeShape = aExplorer.current();
          GeomAPI_Edge anEdge(aEdgeShape);
          if ((anEdge.isCircle() || anEdge.isArc() || anEdge.isEllipse()) &&
               anEdge.isInPlane(aPlane)) {
            bool isContains = false;
            // Check that edge is not used.
            // It is possible that the same edge will be taken from different faces
            foreach(ModuleBase_ViewerPrsPtr aPrs, aResult) {
              GeomAPI_Edge aUsedEdge(aPrs->shape());
              if (aUsedEdge.isEqual(aEdgeShape)) {
                isContains = true;
                break;
              }
            }
            if (!isContains) {
              ModuleBase_ViewerPrsPtr aPrs(new ModuleBase_ViewerPrs(aResObj, aEdgeShape));
              aResult.append(aPrs);
            }
          }
        }
      }
    }
  }
  return aResult;
}

//******************************************************
void PartSet_WidgetSketchLabel::onChangePlane()
{
  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  if (aModule) {
    SessionPtr aMgr = ModelAPI_Session::get();
    aMgr->startOperation("Change Sketch plane");
    myOpenTransaction = true;

    mySizeOfViewWidget->setVisible(false);
    myRemoveExternal->setVisible(true);
    myStackWidget->setCurrentIndex(0);

    bool aBodyIsVisualized = myPreviewPlanes->hasVisualizedBodies(myWorkshop);

    if (!aBodyIsVisualized) {
      // We have to select a plane before any operation
      myPreviewPlanes->showPreviewPlanes(myWorkshop);
    }

    // Nullify sketch plane after operation starting
    // and then restore it in deactivate() before operation finishing
    // to respect transaction margins
    CompositeFeaturePtr aSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
    myTmpPlane = PartSet_Tools::sketchPlane(aSketch);
    PartSet_Tools::nullifySketchPlane(aSketch);

    Handle(SelectMgr_Filter) aFilter = aModule->selectionFilter(SF_SketchPlaneFilter);
    if (!aFilter.IsNull()) {
      std::shared_ptr<GeomAPI_Pln> aPln;
      Handle(ModuleBase_ShapeInPlaneFilter)::DownCast(aFilter)->setPlane(aPln);
    }
    XGUI_Workshop* aWorkshop = aModule->getWorkshop();

    aWorkshop->selectionActivate()->updateSelectionFilters();
    aWorkshop->selectionActivate()->updateSelectionModes();

    myWorkshop->viewer()->update();
  }
}

void PartSet_WidgetSketchLabel::setShowPointsState(bool theState)
{
  bool aBlock = myShowPoints->blockSignals(true);
  myShowPoints->setChecked(theState);
  myShowPoints->blockSignals(aBlock);
}

bool PartSet_WidgetSketchLabel::storeValueCustom()
{
  if (mySketchDataIsModified)
    saveSketchViewPreferenceToSkethData();

  return true;
}

bool PartSet_WidgetSketchLabel::restoreValueCustom()
{
  if (myFeature.get()) {
    CompositeFeaturePtr aSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
    if (aSketch.get() && (aSketch->numberOfSubs() > 0)) {
      AttributeStringPtr aDOFStr = aSketch->string("SolverDOF");
      if (aDOFStr.get()) {
        QString aVal(aDOFStr->value().c_str());
        if (aVal.contains('=')) {
          // to support old data
          aVal = aVal.right(aVal.length() - aVal.lastIndexOf('='));
        }
        int aDoF = aVal.toInt();
        if (aDoF == 0) {
          myDoFLabel->setText(tr("Sketch is fully fixed (DoF = 0)"));
          myShowDOFBtn->setEnabled(false);
        } else {
          myDoFLabel->setText(tr("DoF (degrees of freedom) = ") + aVal);
          myShowDOFBtn->setEnabled(true);
        }
      }

      PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
      if (aModule)
        aModule->sketchMgr()->previewSketchPlane()->setAllUsingSketch(aSketch);

      reconfigureSketchViewWidgets();
    }
    else {
      myDoFLabel->setText("");
      myShowDOFBtn->setEnabled(false);
    }
  }
  return true;
}

bool PartSet_WidgetSketchLabel::eventFilter(QObject* theObj, QEvent* theEvent)
{
  if (theObj == window()) {
    int aType = theEvent->type();
    if ((aType == QEvent::Hide) || (aType == QEvent::WindowDeactivate)) {
      if (myPartSetMessage->isVisible())
        myPartSetMessage->hide();
      if (mySizeMessage->isVisible())
        mySizeMessage->hide();
    }
    else if ((aType == QEvent::Show) || (aType == QEvent::WindowActivate))
      onShowPanel();
  }
  return ModuleBase_WidgetValidated::eventFilter(theObj, theEvent);
}


PitchSpinBox::PitchSpinBox(QWidget* theParent) : QDoubleSpinBox(theParent)
{
  setRange(std::pow(10, -PartSet_WidgetSketchGrid::NUM_OF_DECIMAL_DIGITS_TRANS), std::numeric_limits<double>::max());
  setDecimals(PartSet_WidgetSketchGrid::NUM_OF_DECIMAL_DIGITS_TRANS);
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  setMinimumWidth(PartSet_WidgetSketchGrid::SPIN_BOX_MIN_WIDTH);
  myPrevVal = 1;

  connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged()));
  connect(this, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
}

void PitchSpinBox::setValue(double theVal)
{
  myPrevVal = theVal;
  QDoubleSpinBox::setValue(theVal);
}

void PitchSpinBox::onTextChanged()
{
  if (value() <= Precision::Confusion())
    return;

  setSingleStep(PartSet_WidgetSketchGrid::reasonablePitchIncrement(value()));
  emit valueSet(value());
}

void PitchSpinBox::onEditingFinished()
{
  if (value() <= Precision::Confusion()) {
    QDoubleSpinBox::setValue(myPrevVal);
  }
  else {
    myPrevVal = value();
    emit valueSet(value());
  }
}


PartSet_WidgetSketchGrid::PartSet_WidgetSketchGrid(QWidget* theParent, PartSet_WidgetSketchLabel* theSketchLabel)
: QWidget(theParent), mySketchLabel(theSketchLabel), myPreviewPlane(nullptr)
{
  static const double MAX_DOUBLE = std::numeric_limits<double>::max();
  static const double MIN_DOUBLE = std::numeric_limits<double>::lowest();

  myResetButton = new QPushButton(tr("Reset"), this);
  myResetButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  myResetButton->setToolTip(tr("Set default pitches and zero offsets."));

  const auto offsetAngleLabel = new QLabel(this);
  offsetAngleLabel->setText(tr("Offset angle,°"));

  myOffsetAngleSpinBox = new QDoubleSpinBox(this);
  myOffsetAngleSpinBox->setRange(-180, 180);
  myOffsetAngleSpinBox->setDecimals(PartSet_WidgetSketchGrid::NUM_OF_DECIMAL_DIGITS_ROTAT);
  myOffsetAngleSpinBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  myOffsetAngleSpinBox->setMinimumWidth(PartSet_WidgetSketchGrid::SPIN_BOX_MIN_WIDTH);

  const auto offsetXLabel = new QLabel(this);
  offsetXLabel->setText(tr("Offset") + " X'");

  myOffsetXSpinBox = new QDoubleSpinBox(this);
  myOffsetXSpinBox->setRange(MIN_DOUBLE, MAX_DOUBLE);
  myOffsetXSpinBox->setDecimals(PartSet_WidgetSketchGrid::NUM_OF_DECIMAL_DIGITS_TRANS);
  myOffsetXSpinBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  myOffsetXSpinBox->setMinimumWidth(PartSet_WidgetSketchGrid::SPIN_BOX_MIN_WIDTH);

  const auto offsetYLabel = new QLabel(this);
  offsetYLabel->setText(tr("Offset")  + " Y'");

  myOffsetYSpinBox = new QDoubleSpinBox(this);
  myOffsetYSpinBox->setRange(MIN_DOUBLE, MAX_DOUBLE);
  myOffsetYSpinBox->setDecimals(PartSet_WidgetSketchGrid::NUM_OF_DECIMAL_DIGITS_TRANS);
  myOffsetYSpinBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  myOffsetYSpinBox->setMinimumWidth(PartSet_WidgetSketchGrid::SPIN_BOX_MIN_WIDTH);

  myLayout = new QGridLayout(this);
  myLayout->addWidget(myResetButton , 2, 0);
  myLayout->addWidget(offsetAngleLabel, 3, 0);
  myLayout->addWidget(offsetXLabel    , 3, 1);
  myLayout->addWidget(offsetYLabel    , 3, 2);
  myLayout->addWidget(myOffsetAngleSpinBox, 4, 0);
  myLayout->addWidget(myOffsetXSpinBox    , 4, 1);
  myLayout->addWidget(myOffsetYSpinBox    , 4, 2);

  connect(myResetButton, SIGNAL(clicked(bool)), this, SLOT(onResetClicked()));
  connect(myOffsetAngleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onOffsetAngleChanged(double)));
  connect(myOffsetXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onOffsetXChanged(double)));
  connect(myOffsetYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onOffsetYChanged(double)));

  setEnabled(false);
}

/*static*/ double PartSet_WidgetSketchGrid::clampValue(double theValue, double theIntervalWidth)
{
  theValue = std::remainder(theValue, theIntervalWidth);
  if (theValue > theIntervalWidth/2)
    theValue = theValue - theIntervalWidth;
  else if (theValue <= -theIntervalWidth/2)
    theValue = theIntervalWidth + theValue;

  return theValue;
}

/*static*/ double PartSet_WidgetSketchGrid::reasonableOffsetIncrement(double theStep)
{
  const double stepTenth = theStep / 10;
  if (std::abs(stepTenth) < Precision::Confusion())
    return 0;

  return std::pow(10, std::floor(std::log10(stepTenth)));
}

/*static*/ double PartSet_WidgetSketchGrid::reasonablePitchIncrement(double theStep)
{
  return PartSet_WidgetSketchGrid::reasonableOffsetIncrement(theStep);
}

void PartSet_WidgetSketchGrid::retrieveSketchAndPlane()
{
  const auto module = dynamic_cast<PartSet_Module*>(mySketchLabel->myWorkshop->module());
  if (!module)
    myPreviewPlane = nullptr;
  else
    myPreviewPlane = module->sketchMgr()->previewSketchPlane();
}

/*static*/ const int PartSet_WidgetSketchGrid::NUM_OF_DECIMAL_DIGITS_TRANS = 3;
/*static*/ const int PartSet_WidgetSketchGrid::NUM_OF_DECIMAL_DIGITS_ROTAT = 4; // Fits angular second.
/*static*/ const int PartSet_WidgetSketchGrid::SPIN_BOX_MIN_WIDTH = 80;


PartSet_WidgetSketchRectangularGrid::PartSet_WidgetSketchRectangularGrid(QWidget* theParent, PartSet_WidgetSketchLabel* theSketchLabel)
: PartSet_WidgetSketchGrid(theParent, theSketchLabel)
{
  const auto stepXLabel = new QLabel(this);
  stepXLabel->setText(tr("Pitch") + " X'");

  myStepXSpinBox = new PitchSpinBox(this);

  const auto stepYLabel = new QLabel(this);
  stepYLabel->setText(tr("Pitch") + " Y'");

  myStepYSpinBox = new PitchSpinBox(this);

  myLayout->addWidget(stepXLabel    , 1, 1);
  myLayout->addWidget(stepYLabel    , 1, 2);
  myLayout->addWidget(myStepXSpinBox, 2, 1);
  myLayout->addWidget(myStepYSpinBox, 2, 2);

  connect(myStepXSpinBox, SIGNAL(valueSet(double)), this, SLOT(onStepXSet(double)));
  connect(myStepYSpinBox, SIGNAL(valueSet(double)), this, SLOT(onStepYSet(double)));
}

void PartSet_WidgetSketchRectangularGrid::reconfigure()
{
  retrieveSketchAndPlane();

  if (myPreviewPlane) {
    const auto steps = myPreviewPlane->getRectangularGridSteps();
    myStepXSpinBox->setValue(steps.first);
    myStepYSpinBox->setValue(steps.second);

    const auto offsets = myPreviewPlane->getRectangularGridOffsets();
    myOffsetAngleSpinBox->setValue(PartSet_WidgetSketchGrid::clampValue(offsets.second, 360));
    myOffsetXSpinBox->setValue(offsets.first.first);
    myOffsetYSpinBox->setValue(offsets.first.second);

    setEnabled(true);
  }
  else {
    setEnabled(false);

    myStepXSpinBox->setValue(0);
    myStepYSpinBox->setValue(0);
    myOffsetAngleSpinBox->setValue(0);
    myOffsetXSpinBox->setValue(0);
    myOffsetYSpinBox->setValue(0);
  }
}

void PartSet_WidgetSketchRectangularGrid::onStepXSet(double theStep)
{
  myOffsetXSpinBox->setSingleStep(PartSet_WidgetSketchGrid::reasonableOffsetIncrement(theStep));

  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(mySketchLabel->myFeature);
  if (sketch) {
    PartSet_Tools::sketchPlaneRectangularGridStepX(sketch)->setValue(theStep);
    mySketchLabel->mySketchDataIsModified = true;
  }

  if (myPreviewPlane) {
    myPreviewPlane->setRectangularGridStepX(theStep);
    myPreviewPlane->reconfigureGrid();
    mySketchLabel->myWorkshop->viewer()->update();
  }
}

void PartSet_WidgetSketchRectangularGrid::onStepYSet(double theStep)
{
  myOffsetYSpinBox->setSingleStep(PartSet_WidgetSketchGrid::reasonableOffsetIncrement(theStep));

  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(mySketchLabel->myFeature);
  if (sketch) {
    PartSet_Tools::sketchPlaneRectangularGridStepY(sketch)->setValue(theStep);
    mySketchLabel->mySketchDataIsModified = true;
  }

  if (myPreviewPlane) {
    myPreviewPlane->setRectangularGridStepY(theStep);
    myPreviewPlane->reconfigureGrid();
    mySketchLabel->myWorkshop->viewer()->update();
  }
}

void PartSet_WidgetSketchRectangularGrid::onResetClicked()
{
  if (!myPreviewPlane)
    return;

  myPreviewPlane->resetRectangularGrid();
  reconfigure();

  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(mySketchLabel->feature());
  if (sketch) {
    myPreviewPlane->saveRectangularGridPreferencesIntoSketchData(sketch);
    mySketchLabel->mySketchDataIsModified = true;
  }

  mySketchLabel->myWorkshop->viewer()->update();
}

void PartSet_WidgetSketchRectangularGrid::onOffsetAngleChanged(double theOffset)
{
  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(mySketchLabel->feature());
  if (sketch) {
    PartSet_Tools::sketchPlaneRectangularGridOffsetAngle(sketch)->setValue(theOffset);
    mySketchLabel->mySketchDataIsModified = true;
  }

  if (myPreviewPlane) {
    myPreviewPlane->setRectangularGridOffsetA(theOffset);
    myPreviewPlane->reconfigureGrid();
    mySketchLabel->myWorkshop->viewer()->update();
  }
}

void PartSet_WidgetSketchRectangularGrid::onOffsetXChanged(double theOffset)
{
  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(mySketchLabel->feature());
  if (sketch) {
    PartSet_Tools::sketchPlaneRectangularGridOffsetX(sketch)->setValue(theOffset);
    mySketchLabel->mySketchDataIsModified = true;
  }

  if (myPreviewPlane) {
    myPreviewPlane->setRectangularGridOffsetX(theOffset);
    myPreviewPlane->reconfigureGrid();
    mySketchLabel->myWorkshop->viewer()->update();
  }
}

void PartSet_WidgetSketchRectangularGrid::onOffsetYChanged(double theOffset)
{
  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(mySketchLabel->feature());
  if (sketch) {
    PartSet_Tools::sketchPlaneRectangularGridOffsetY(sketch)->setValue(theOffset);
    mySketchLabel->mySketchDataIsModified = true;
  }

  if (myPreviewPlane) {
    myPreviewPlane->setRectangularGridOffsetY(theOffset);
    myPreviewPlane->reconfigureGrid();
    mySketchLabel->myWorkshop->viewer()->update();
  }
}


PartSet_WidgetSketchCircularGrid::PartSet_WidgetSketchCircularGrid(QWidget* theParent, PartSet_WidgetSketchLabel* theSketchLabel)
: PartSet_WidgetSketchGrid(theParent, theSketchLabel)
{
  const auto stepRLabel = new QLabel(this);
  stepRLabel->setText(tr("Pitch") + " R");

  myStepRSpinBox = new PitchSpinBox(this);

  const auto aNASLabel = new QLabel(this);
  aNASLabel->setText(tr("Num of angular segments"));

  myNASSpinBox = new QSpinBox(this);
  myNASSpinBox->setRange(2, std::numeric_limits<int>::max());
  myNASSpinBox->setSingleStep(2);
  myNASSpinBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  myNASSpinBox->setToolTip(tr("Even numbers only."));
  myNASSpinBox->setMinimumWidth(PartSet_WidgetSketchGrid::SPIN_BOX_MIN_WIDTH);

  myLayout->addWidget(stepRLabel    , 1, 1);
  myLayout->addWidget(aNASLabel     , 1, 2);
  myLayout->addWidget(myStepRSpinBox, 2, 1);
  myLayout->addWidget(myNASSpinBox  , 2, 2);

  connect(myStepRSpinBox, SIGNAL(valueSet(double)), this, SLOT(onStepRChanged(double)));
  connect(myNASSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onNumOfAngularSegmentsChanged(int)));
}

void PartSet_WidgetSketchCircularGrid::reconfigure()
{
  retrieveSketchAndPlane();

  if (myPreviewPlane) {
    const auto stepAndNum = myPreviewPlane->getCircularGrid_dR_and_NAS();
    myStepRSpinBox->setValue(stepAndNum.first);
    myNASSpinBox->setValue(stepAndNum.second);
    updateSegmentsToolTip();

    const auto offsets = myPreviewPlane->getCircularGridOffsets();
    myOffsetAngleSpinBox->setValue(PartSet_WidgetSketchGrid::clampValue(offsets.second, 360));
    myOffsetXSpinBox->setValue(offsets.first.first);
    myOffsetYSpinBox->setValue(offsets.first.second);

    setEnabled(true);
  }
  else {
    setEnabled(false);

    myStepRSpinBox->setValue(0);
    myNASSpinBox->setValue(1);
    updateSegmentsToolTip();
    myOffsetAngleSpinBox->setValue(0);
    myOffsetXSpinBox->setValue(0);
    myOffsetYSpinBox->setValue(0);
  }
}

void PartSet_WidgetSketchCircularGrid::onStepRChanged(double theStep)
{
  const double increment = PartSet_WidgetSketchGrid::reasonableOffsetIncrement(theStep);
  myOffsetXSpinBox->setSingleStep(increment);
  myOffsetYSpinBox->setSingleStep(increment);

  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(mySketchLabel->myFeature);
  if (sketch) {
    PartSet_Tools::sketchPlaneCircularGridStepR(sketch)->setValue(theStep);
    mySketchLabel->mySketchDataIsModified = true;
  }

  if (myPreviewPlane) {
    myPreviewPlane->setCircularGridRadialStep(theStep);
    myPreviewPlane->reconfigureGrid();
    mySketchLabel->myWorkshop->viewer()->update();
  }
}

void PartSet_WidgetSketchCircularGrid::onNumOfAngularSegmentsChanged(int theNum)
{
  theNum = theNum + theNum % 2;
  myNASSpinBox->setValue(theNum);
  updateSegmentsToolTip();

  const double increment = PartSet_WidgetSketchGrid::reasonableOffsetIncrement(double(360)/theNum);
  myOffsetAngleSpinBox->setSingleStep(increment);

  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(mySketchLabel->myFeature);
  if (sketch) {
    PartSet_Tools::sketchPlaneCircularGridNumOfAngSegments(sketch)->setValue(theNum);
    mySketchLabel->mySketchDataIsModified = true;
  }

  if (myPreviewPlane) {
    myPreviewPlane->setCircularGridNumOfAngularSegments(theNum);
    myPreviewPlane->reconfigureGrid();
    mySketchLabel->myWorkshop->viewer()->update();
  }
}

void PartSet_WidgetSketchCircularGrid::onResetClicked()
{
  if (!myPreviewPlane)
    return;

  myPreviewPlane->resetCircularGrid();
  reconfigure();

  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(mySketchLabel->feature());
  if (sketch) {
    myPreviewPlane->saveCircularGridPreferencesIntoSketchData(sketch);
    mySketchLabel->mySketchDataIsModified = true;
  }

  mySketchLabel->myWorkshop->viewer()->update();
}

void PartSet_WidgetSketchCircularGrid::onOffsetAngleChanged(double theOffset)
{
  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(mySketchLabel->feature());
  if (sketch) {
    PartSet_Tools::sketchPlaneCircularGridOffsetAngle(sketch)->setValue(theOffset);
    mySketchLabel->mySketchDataIsModified = true;
  }

  if (myPreviewPlane) {
    myPreviewPlane->setCircularGridOffsetA(theOffset);
    myPreviewPlane->reconfigureGrid();
    mySketchLabel->myWorkshop->viewer()->update();
  }
}

void PartSet_WidgetSketchCircularGrid::onOffsetXChanged(double theOffset)
{
  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(mySketchLabel->feature());
  if (sketch) {
    PartSet_Tools::sketchPlaneCircularGridOffsetX(sketch)->setValue(theOffset);
    mySketchLabel->mySketchDataIsModified = true;
  }

  if (myPreviewPlane) {
    myPreviewPlane->setCircularGridOffsetX(theOffset);
    myPreviewPlane->reconfigureGrid();
    mySketchLabel->myWorkshop->viewer()->update();
  }
}

void PartSet_WidgetSketchCircularGrid::onOffsetYChanged(double theOffset)
{
  const auto sketch = std::static_pointer_cast<ModelAPI_CompositeFeature>(mySketchLabel->feature());
  if (sketch) {
    PartSet_Tools::sketchPlaneCircularGridOffsetY(sketch)->setValue(theOffset);
    mySketchLabel->mySketchDataIsModified = true;
  }

  if (myPreviewPlane) {
    myPreviewPlane->setCircularGridOffsetY(theOffset);
    myPreviewPlane->reconfigureGrid();
    mySketchLabel->myWorkshop->viewer()->update();
  }
}

void PartSet_WidgetSketchCircularGrid::updateSegmentsToolTip()
{
   const int N = myNASSpinBox->value();
   QString toolTip = tr("Even numbers only.") + "\nAngle " + QString::number(180 - double(N-2)/N * 180, 'f', PartSet_WidgetSketchGrid::NUM_OF_DECIMAL_DIGITS_ROTAT) + "°";
   myNASSpinBox->setToolTip(toolTip);
}
