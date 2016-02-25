// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSketchLabel.cpp
// Created:     07 July 2014
// Author:      Vitaly SMETANNIKOV

#include "PartSet_WidgetSketchLabel.h"
#include "PartSet_Tools.h"
#include "PartSet_Module.h"

#include "SketchPlugin_SketchEntity.h"

#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_Selection.h>
#include <XGUI_ViewerProxy.h>
#include <XGUI_ActionsMgr.h>
#include <XGUI_ModuleConnector.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IModule.h>

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAPI_XYZ.h>

#include <SketchPlugin_Sketch.h>
#include <SketcherPrs_Tools.h>

#include <Precision.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <AIS_Shape.hxx>
#include <AIS_DimensionSelectionMode.hxx>

#include <Config_WidgetAPI.h>
#include <Config_PropManager.h>

#include <QLabel>
#include <QApplication>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QStackedWidget>


PartSet_WidgetSketchLabel::PartSet_WidgetSketchLabel(QWidget* theParent,
                        ModuleBase_IWorkshop* theWorkshop,
                        const Config_WidgetAPI* theData,
                        const std::string& theParentId,
                        const QMap<PartSet_Tools::ConstraintVisibleState, bool>& toShowConstraints)
: ModuleBase_WidgetValidated(theParent, theWorkshop, theData, theParentId),
  myPreviewDisplayed(false)
{
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);

  myStackWidget = new QStackedWidget(this);
  myStackWidget->setContentsMargins(0,0,0,0);
  aLayout->addWidget(myStackWidget);

  // Define label for plane selection
  QWidget* aFirstWgt = new QWidget(this);

  QString aText = QString::fromStdString(theData->getProperty("title"));
  QLabel* aLabel = new QLabel(aText, aFirstWgt);
  aLabel->setWordWrap(true);
  QString aTooltip = QString::fromStdString(theData->getProperty("tooltip"));
  aLabel->setToolTip(aTooltip);
  aLabel->setIndent(5);

  aLayout = new QVBoxLayout(aFirstWgt);
  ModuleBase_Tools::zeroMargins(aLayout);
  aLayout->addWidget(aLabel);

  myStackWidget->addWidget(aFirstWgt);

  // Define widget for sketch manmagement
  QWidget* aSecondWgt = new QWidget(this);
  aLayout = new QVBoxLayout(aSecondWgt);
  ModuleBase_Tools::zeroMargins(aLayout);

  QGroupBox* aViewBox = new QGroupBox(tr("Sketcher plane"), this);
  QVBoxLayout* aViewLayout = new QVBoxLayout(aViewBox);

  myViewInverted = new QCheckBox(tr("Reversed"), aViewBox);
  aViewLayout->addWidget(myViewInverted);

  QPushButton* aSetViewBtn = new QPushButton(QIcon(":icons/plane_view.png"), tr("Set plane view"), aViewBox);
  connect(aSetViewBtn, SIGNAL(clicked(bool)), this, SLOT(onSetPlaneView()));
  aViewLayout->addWidget(aSetViewBtn);

  aLayout->addWidget(aViewBox);

  QMap<PartSet_Tools::ConstraintVisibleState, QString> aStates;
  aStates[PartSet_Tools::Geometrical] = tr("Show geometrical constraints");
  aStates[PartSet_Tools::Dimensional] = tr("Show dimensional constraints");

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

  myStackWidget->addWidget(aSecondWgt);
  //setLayout(aLayout);
}

PartSet_WidgetSketchLabel::~PartSet_WidgetSketchLabel()
{
}

bool PartSet_WidgetSketchLabel::setSelection(QList<ModuleBase_ViewerPrs>& theValues,
                                             const bool theToValidate)
{
  // do not use the given selection if the plane of the sketch has been already set.
  // If this check is absent, a selected plane in the viewer can be set in the sketch
  // even if the sketch is built on another plane.
  if (plane().get())
    return true;

  ModuleBase_ViewerPrs aPrs = theValues.first();
  bool aDone = ModuleBase_WidgetValidated::setSelection(theValues, theToValidate);
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

void PartSet_WidgetSketchLabel::onSelectionChanged()
{
  QList<ModuleBase_ViewerPrs> aSelected = getFilteredSelected();

  if (aSelected.empty())
    return;
  ModuleBase_ViewerPrs aPrs = aSelected.first();

  bool aDone = ModuleBase_WidgetValidated::setSelection(aSelected, false);
  if (aDone) {
    updateByPlaneSelected(aPrs);
    updateObject(myFeature);
  }
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

void PartSet_WidgetSketchLabel::blockAttribute(const bool& theToBlock, bool& isFlushesActived,
                                               bool& isAttributeSetInitializedBlocked)
{
  ModuleBase_WidgetValidated::blockAttribute(theToBlock, isFlushesActived,
                                             isAttributeSetInitializedBlocked);
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

void PartSet_WidgetSketchLabel::updateByPlaneSelected(const ModuleBase_ViewerPrs& thePrs)
{
  // 1. hide main planes if they have been displayed
  erasePreviewPlanes();
  // 2. if the planes were displayed, change the view projection
  TopoDS_Shape aShape = thePrs.shape();
  std::shared_ptr<GeomAPI_Shape> aGShape;
  std::shared_ptr<GeomAPI_Shape> aBaseShape;

  DataPtr aData = feature()->data();
  AttributeSelectionPtr aSelAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
                            (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));

  // selection happens in OCC viewer
  if (!aShape.IsNull()) {
    aGShape =  std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
    aGShape->setImpl(new TopoDS_Shape(aShape));

    if (aSelAttr && aSelAttr->context()) {
      aBaseShape = aSelAttr->context()->shape();
    }
  }
  else { // selection happens in OCC viewer(on body) of in the OB browser
    if (aSelAttr) {
      aGShape = aSelAttr->value();
    }
  }
  if (aGShape.get() != NULL) {
    // get plane parameters
    std::shared_ptr<GeomAPI_Pln> aPlane = GeomAlgoAPI_FaceBuilder::plane(aGShape);
    std::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();
    gp_XYZ aXYZ = aDir->impl<gp_Dir>().XYZ();
    double aTwist = 0.0;

    // orienting projection is not needed: it is done in GeomAlgoAPI_FaceBuilder::plane
    /*if (aGShape->impl<TopoDS_Shape>().Orientation() == TopAbs_REVERSED) {
      aXYZ.Reverse();
    }*/

    // Rotate view if the sketcher plane is selected only from preview planes
    // Preview planes are created only if there is no any shape
    bool aRotate = Config_PropManager::boolean("Sketch planes", "rotate_to_plane", "false");
    if (aRotate) {
      myWorkshop->viewer()->setViewProjection(aXYZ.X(), aXYZ.Y(), aXYZ.Z(), aTwist);
      PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
      if (aModule)
        aModule->onViewTransformed();
    }
  }
  // 3. Clear text in the label
  myStackWidget->setCurrentIndex(1);
  //myLabel->setText("");
  //myLabel->setToolTip("");
  disconnect(workshop()->selector(), SIGNAL(selectionChanged()), 
              this, SLOT(onSelectionChanged()));
  // 4. deactivate face selection filter
  activateFilters(false);

  // 5. Clear selection mode and define sketching mode
  //XGUI_Displayer* aDisp = workshop()->displayer();
  //aDisp->closeLocalContexts();
  emit planeSelected(plane());
  // after the plane is selected in the sketch, the sketch selection should be activated
  // it can not be performed in the sketch label widget because, we don't need to switch off
  // the selection by any label deactivation, but need to switch it off by stop the sketch
  activateSelection(true);

  // 6. Update sketcher actions
  XGUI_ActionsMgr* anActMgr = workshop()->actionsMgr();
  myWorkshop->updateCommandStatus();
  myWorkshop->viewer()->update();
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

void PartSet_WidgetSketchLabel::storeAttributeValue()
{
  ModuleBase_WidgetValidated::storeAttributeValue();
}

void PartSet_WidgetSketchLabel::restoreAttributeValue(const bool theValid)
{
  ModuleBase_WidgetValidated::restoreAttributeValue(theValid);

  // it is not necessary to save the previous plane value because the plane is chosen once
  DataPtr aData = feature()->data();
  AttributeSelectionPtr aSelAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
    (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
  if (aSelAttr) {
    ResultPtr anEmptyResult;
    GeomShapePtr anEmptyShape;
    aSelAttr->setValue(anEmptyResult, anEmptyShape);
  }
}

bool PartSet_WidgetSketchLabel::setSelectionCustom(const ModuleBase_ViewerPrs& thePrs)
{
  bool isOwnerSet = false;

  const TopoDS_Shape& aShape = thePrs.shape();
  std::shared_ptr<GeomAPI_Dir> aDir;

  if (thePrs.object() && (feature() != thePrs.object())) {
    DataPtr aData = feature()->data();
    AttributeSelectionPtr aSelAttr = 
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
      (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
    if (aSelAttr) {
      ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(thePrs.object());
      if (aRes) {
        GeomShapePtr aShapePtr(new GeomAPI_Shape());
        if (aShape.IsNull()) {  // selection happens in the OCC viewer
          aShapePtr = ModelAPI_Tools::shape(aRes);
        }
        else { // selection happens in OB browser
          aShapePtr->setImpl(new TopoDS_Shape(aShape));
        }
        if (aShapePtr.get() != NULL) {
          aSelAttr->setValue(aRes, aShapePtr);
          isOwnerSet = true;
        }
      }
    }
  }
  else if (!aShape.IsNull()) {
    aDir = setSketchPlane(aShape);
    isOwnerSet = aDir.get();
  }
  return isOwnerSet;
}

void PartSet_WidgetSketchLabel::activateCustom()
{
  std::shared_ptr<GeomAPI_Pln> aPlane = plane();
  if (aPlane.get()) {
    myStackWidget->setCurrentIndex(1);
    activateSelection(true);
    return;
  }

  myStackWidget->setCurrentIndex(0);
  bool aBodyIsVisualized = false;
  XGUI_Displayer* aDisp = workshop()->displayer();
  QObjectPtrList aDisplayed = aDisp->displayedObjects();
  foreach (ObjectPtr anObj, aDisplayed) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObj);
    if (aResult.get() != NULL) {
      aBodyIsVisualized = aResult->groupName() == ModelAPI_ResultBody::group();
      if (aBodyIsVisualized)
        break;
    }
  }

  if (!aBodyIsVisualized) {
    // We have to select a plane before any operation
    showPreviewPlanes();
  }
  activateSelection(true);

  //myLabel->setText(myText);
  //myLabel->setToolTip(myTooltip);

  connect(workshop()->selector(), SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
  activateFilters(true);
}

void PartSet_WidgetSketchLabel::deactivate()
{
  ModuleBase_ModelWidget::deactivate();
  bool aHidePreview = myPreviewDisplayed;
  erasePreviewPlanes();
  activateSelection(false);

  activateFilters(false);
  if (aHidePreview)
    myWorkshop->viewer()->update();
}

void PartSet_WidgetSketchLabel::activateSelection(bool toActivate)
{
  if (toActivate) {
    QIntList aModes;
    std::shared_ptr<GeomAPI_Pln> aPlane = plane();
    if (aPlane.get()) {
      myWorkshop->module()->activeSelectionModes(aModes);
    }
    else {
      aModes << TopAbs_FACE;
    }
    myWorkshop->activateSubShapesSelection(aModes);
  } else {
    myWorkshop->deactivateSubShapesSelection();
  }
}

void PartSet_WidgetSketchLabel::erasePreviewPlanes()
{
  if (myPreviewDisplayed) {
    XGUI_Displayer* aDisp = workshop()->displayer();
    aDisp->eraseAIS(myYZPlane, false);
    aDisp->eraseAIS(myXZPlane, false);
    aDisp->eraseAIS(myXYPlane, false);
    myPreviewDisplayed = false;
  }
}

void PartSet_WidgetSketchLabel::showPreviewPlanes()
{
  if (myPreviewDisplayed)
    return;

  if (!myYZPlane) { // If planes are not created
    // Create Preview
    std::shared_ptr<GeomAPI_Pnt> anOrigin(new GeomAPI_Pnt(0, 0, 0));
    std::shared_ptr<GeomAPI_Dir> aYZDir(new GeomAPI_Dir(1, 0, 0));
    // -1, not 1 for correct internal sketch coords (issue 898)
    std::shared_ptr<GeomAPI_Dir> aXZDir(new GeomAPI_Dir(0, -1, 0));
    std::shared_ptr<GeomAPI_Dir> aXYDir(new GeomAPI_Dir(0, 0, 1));

    std::vector<int> aYZRGB, aXZRGB, aXYRGB;
    aYZRGB = Config_PropManager::color("Visualization", "yz_plane_color",
                                                        YZ_PLANE_COLOR);
    aXZRGB = Config_PropManager::color("Visualization", "xz_plane_color",
                                                        XZ_PLANE_COLOR);
    aXYRGB = Config_PropManager::color("Visualization", "xy_plane_color",
                                                        XY_PLANE_COLOR);
    int aR[] = {aYZRGB[0], aYZRGB[1], aYZRGB[2]};
    int aG[] = {aXZRGB[0], aXZRGB[1], aXZRGB[2]};
    int aB[] = {aXYRGB[0], aXYRGB[1], aXYRGB[2]};

    myYZPlane = createPreviewPlane(anOrigin, aYZDir, aR);
    myXZPlane = createPreviewPlane(anOrigin, aXZDir, aG);
    myXYPlane = createPreviewPlane(anOrigin, aXYDir, aB);
  }
  XGUI_Displayer* aDisp = workshop()->displayer();
  aDisp->displayAIS(myYZPlane, true, false);
  aDisp->displayAIS(myXZPlane, true, false);
  aDisp->displayAIS(myXYPlane, true, false);
  myPreviewDisplayed = true;
}


AISObjectPtr PartSet_WidgetSketchLabel::createPreviewPlane(std::shared_ptr<GeomAPI_Pnt> theOrigin, 
                                                           std::shared_ptr<GeomAPI_Dir> theNorm, 
                                                           const int theRGB[3])
{
  double aSize = Config_PropManager::integer("Sketch planes", "planes_size", PLANE_SIZE);
  std::shared_ptr<GeomAPI_Shape> aFace = GeomAlgoAPI_FaceBuilder::square(theOrigin, theNorm, aSize);
  AISObjectPtr aAIS = AISObjectPtr(new GeomAPI_AISObject());
  aAIS->createShape(aFace);
  aAIS->setWidth(Config_PropManager::integer("Sketch planes", "planes_thickness", SKETCH_WIDTH));
  aAIS->setColor(theRGB[0], theRGB[1], theRGB[2]);
  return aAIS;
}


std::shared_ptr<GeomAPI_Dir> PartSet_WidgetSketchLabel::setSketchPlane(const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
    return std::shared_ptr<GeomAPI_Dir>();

  // get selected shape
  std::shared_ptr<GeomAPI_Shape> aGShape(new GeomAPI_Shape);
  aGShape->setImpl(new TopoDS_Shape(theShape));



  // get plane parameters
  std::shared_ptr<GeomAPI_Pln> aPlane = GeomAlgoAPI_FaceBuilder::plane(aGShape);
  if (!aPlane.get())
    return std::shared_ptr<GeomAPI_Dir>();

  // set plane parameters to feature
  std::shared_ptr<ModelAPI_Data> aData = feature()->data();
  double anA, aB, aC, aD;
  aPlane->coefficients(anA, aB, aC, aD);

  // calculate attributes of the sketch
  std::shared_ptr<GeomAPI_Dir> aNormDir(new GeomAPI_Dir(anA, aB, aC));
  std::shared_ptr<GeomAPI_XYZ> aCoords = aNormDir->xyz();
  std::shared_ptr<GeomAPI_XYZ> aZero(new GeomAPI_XYZ(0, 0, 0));
  aCoords = aCoords->multiplied(-aD * aCoords->distance(aZero));
  std::shared_ptr<GeomAPI_Pnt> anOrigPnt(new GeomAPI_Pnt(aCoords));
  // X axis is preferable to be dirX on the sketch
  const double tol = Precision::Confusion();
  bool isX = fabs(anA - 1.0) < tol && fabs(aB) < tol && fabs(aC) < tol;
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
  std::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();
  return aDir;
}

XGUI_Workshop* PartSet_WidgetSketchLabel::workshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  return aConnector->workshop();
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
