// Copyright (C) 2014-2023  CEA, EDF
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
#include <ModelAPI_Tools.h>
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
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QDialog>
#include <QTimer>

#ifdef WIN32
#pragma warning(disable : 4456) // for nested foreach
#endif

#ifndef DBL_MAX
#define DBL_MAX 1.7976931348623158e+308
#endif

PartSet_WidgetSketchLabel::PartSet_WidgetSketchLabel(QWidget* theParent,
                        ModuleBase_IWorkshop* theWorkshop,
                        const Config_WidgetAPI* theData,
                        const QMap<PartSet_Tools::ConstraintVisibleState, bool>& toShowConstraints)
: ModuleBase_WidgetValidated(theParent, theWorkshop, theData), myOpenTransaction(false),
myIsSelection(false)
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

  // Define widget for sketch manmagement
  QWidget* aSecondWgt = new QWidget(this);
  aLayout = new QVBoxLayout(aSecondWgt);
  ModuleBase_Tools::zeroMargins(aLayout);

  QGroupBox* aViewBox = new QGroupBox(tr("Sketcher plane"), this);
  QGridLayout* aViewLayout = new QGridLayout(aViewBox);

  myViewInverted = new QCheckBox(tr("Reversed"), aViewBox);
  aViewLayout->addWidget(myViewInverted, 0, 0);

  // Sketch plane visibility
  myViewVisible = new QCheckBox(tr("Visible"), aViewBox);
  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  PartSet_PreviewSketchPlane* aPreviewPlane = aModule->sketchMgr()->previewSketchPlane();
  if (aPreviewPlane->isPlaneCreated())
    // init with current state
    myViewVisible->setChecked(aPreviewPlane->isDisplayed());
  else
    // true by default (at start of sketch creation)
    myViewVisible->setChecked(true);

  aViewLayout->addWidget(myViewVisible, 0, 1, Qt::AlignRight);
  connect(myViewVisible, SIGNAL(toggled(bool)), this, SLOT(onShowViewPlane(bool)));

  QPushButton* aSetViewBtn =
    new QPushButton(QIcon(":icons/plane_view.png"), tr("Set plane view"), aViewBox);
  connect(aSetViewBtn, SIGNAL(clicked(bool)), this, SLOT(onSetPlaneView()));
  aViewLayout->addWidget(aSetViewBtn, 1, 0, 1, 2);

  aLayout->addWidget(aViewBox);

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

PartSet_WidgetSketchLabel::~PartSet_WidgetSketchLabel()
{
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

  // 1. hide main planes if they have been displayed and display sketch preview plane
  myPreviewPlanes->erasePreviewPlanes(myWorkshop);

  QString aSizeOfViewStr = mySizeOfView->text();
  bool isSetSizeOfView = false;
  double aSizeOfView = 0;
  if (!aSizeOfViewStr.isEmpty()) {
    aSizeOfView = aSizeOfViewStr.toDouble(&isSetSizeOfView);
    if (isSetSizeOfView && aSizeOfView <= 0) {
      isSetSizeOfView = false;
    }
  }
  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  if (aModule) {
    CompositeFeaturePtr aSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
    aModule->sketchMgr()->previewSketchPlane()->setSizeOfView(aSizeOfView, isSetSizeOfView);
    if (myViewVisible->isChecked())
      aModule->sketchMgr()->previewSketchPlane()->createSketchPlane(aSketch, myWorkshop);
    else
      aModule->sketchMgr()->previewSketchPlane()->clearPlanePreview();
  }
  // 2. if the planes were displayed, change the view projection

  std::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();
  gp_XYZ aXYZ = aDir->impl<gp_Dir>().XYZ();
  double aTwist = 0.0;

  // Rotate view if the sketcher plane is selected only from preview planes
  // Preview planes are created only if there is no any shape
  bool aRotate = Config_PropManager::boolean(SKETCH_TAB_NAME, "rotate_to_plane");
  if (aRotate) {
    myWorkshop->viewer()->setViewProjection(aXYZ.X(), aXYZ.Y(), aXYZ.Z(), aTwist);
  }
  if (isSetSizeOfView && aSizeOfView > 0) {
    Handle(V3d_View) aView3d = myWorkshop->viewer()->activeView();
    if (!aView3d.IsNull()) {
      Bnd_Box aBndBox;
      double aHalfSize = aSizeOfView/2.0;
      aBndBox.Update(-aHalfSize, -aHalfSize, -aHalfSize, aHalfSize, aHalfSize, aHalfSize);
      aView3d->FitAll(aBndBox, 0.01, false);
    }
  }
  if (myOpenTransaction) {
    SessionPtr aMgr = ModelAPI_Session::get();
    aMgr->finishOperation();
    myOpenTransaction = false;
  }
  // 3. Clear text in the label
  myStackWidget->setCurrentIndex(1);
  //myLabel->setText("");
  //myLabel->setToolTip("");
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);

  // 5. Clear selection mode and define sketching mode
  emit planeSelected(plane());
  // after the plane is selected in the sketch, the sketch selection should be activated
  // it can not be performed in the sketch label widget because, we don't need to switch off
  // the selection by any label deactivation, but need to switch it off by stop the sketch
  myWorkshop->selectionActivate()->updateSelectionFilters();
  myWorkshop->selectionActivate()->updateSelectionModes();

  if (aModule)
    aModule->onViewTransformed();

  myWorkshop->updateCommandStatus();
  aWorkshop->selector()->clearSelection();
  myWorkshop->viewer()->update();

  myRemoveExternal->setVisible(false);
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

void PartSet_WidgetSketchLabel::deactivate()
{
  QWidget* aTopWidget = window();
  if (aTopWidget)
    aTopWidget->removeEventFilter(this);

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
    std::shared_ptr<GeomAPI_Dir> aDirection = aPlane->direction();
    gp_Dir aDir = aDirection->impl<gp_Dir>();
    if (myViewInverted->isChecked())
      aDir.Reverse();
    myWorkshop->viewer()->setViewProjection(aDir.X(), aDir.Y(), aDir.Z(), 0.);
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
    mySizeOfViewWidget->setVisible(false);
    myRemoveExternal->setVisible(true);
    myStackWidget->setCurrentIndex(0);

    bool aBodyIsVisualized = myPreviewPlanes->hasVisualizedBodies(myWorkshop);

    if (!aBodyIsVisualized) {
      // We have to select a plane before any operation
      myPreviewPlanes->showPreviewPlanes(myWorkshop);
    }

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

    SessionPtr aMgr = ModelAPI_Session::get();
    aMgr->startOperation("Change Sketch plane");
    myOpenTransaction = true;
    myWorkshop->viewer()->update();
  }
}

void PartSet_WidgetSketchLabel::setShowPointsState(bool theState)
{
  bool aBlock = myShowPoints->blockSignals(true);
  myShowPoints->setChecked(theState);
  myShowPoints->blockSignals(aBlock);
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
    }
    else {
      myDoFLabel->setText("");
      myShowDOFBtn->setEnabled(false);
    }
  }
  return true;
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

void PartSet_WidgetSketchLabel::onShowViewPlane(bool toShow)
{
  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  PartSet_PreviewSketchPlane* aPreviewPlane = aModule->sketchMgr()->previewSketchPlane();
  if (toShow) {
    CompositeFeaturePtr aSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
    if (aPreviewPlane->isPlaneCreated())
      aPreviewPlane->displaySketchPlane(myWorkshop);
    else
      aPreviewPlane->createSketchPlane(aSketch, myWorkshop);
  }
  else {
    aPreviewPlane->eraseSketchPlane(myWorkshop, false);
  }
  myWorkshop->viewer()->update();
}
