// Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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

#include <PartSet_WidgetBSplinePoints.h>

#include <PartSet_CenterPrs.h>
#include <PartSet_ExternalObjectsMgr.h>
#include <PartSet_Module.h>
#include <PartSet_SketcherReentrantMgr.h>
#include <PartSet_WidgetPoint2d.h>

#include <XGUI_Tools.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>

#include <ModuleBase_ISelection.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_LabelValue.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_WidgetValidator.h>
#include <ModuleBase_WidgetValidated.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>

#include <ModelAPI_Events.h>
#include <ModelAPI_AttributeDoubleArray.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_CompositeFeature.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point2DArray.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_IPresentable.h>

#include <SketchPlugin_Feature.h>

#include <QGridLayout>
#include <QGroupBox>
#include <QMouseEvent>
#include <QGraphicsEffect>
#include <QScrollArea>

static const double MaxCoordinate = 1e12;

static bool IsPointCreated = false;


PartSet_WidgetBSplinePoints::PartSet_WidgetBSplinePoints(QWidget* theParent,
                                             ModuleBase_IWorkshop* theWorkshop,
                                             const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData), myWorkshop(theWorkshop),
  myValueIsCashed(false), myIsFeatureVisibleInCash(true),
  myXValueInCash(0), myYValueInCash(0),
  myPointIndex(0), myFinished(false)
{
  myRefAttribute = theData->getProperty("reference_attribute");
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aMainLayout);

  // the control should accept the focus, so the boolean flag is corrected to be true
  myIsObligatory = true;
  QString aPageName = translate(theData->getProperty(CONTAINER_PAGE_NAME));
  myBox = new QGroupBox(aPageName, theParent);
  myBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  myBox->setFlat(false);
  aMainLayout->addWidget(myBox);

#ifdef _DEBUG
  bool aAcceptVariables =
#endif
    theData->getBooleanAttribute(DOUBLE_WDG_ACCEPT_EXPRESSIONS, true);

  // B-spline weights attribute
  myWeightsAttr = theData->getProperty("weights");

  QVBoxLayout* aLayout = new QVBoxLayout(myBox);
  ModuleBase_Tools::adjustMargins(aLayout);

  myScrollArea = new QScrollArea(myBox);
  myScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  myScrollArea->setWidgetResizable(true);
  myScrollArea->setFrameStyle(QFrame::NoFrame);
  aLayout->addWidget(myScrollArea);

  QWidget* aContainer = new QWidget(myScrollArea);
  QVBoxLayout* aBoxLay = new QVBoxLayout(aContainer);
  aBoxLay->setContentsMargins(0, 0, 0, 0);

  myGroupBox = new QWidget(aContainer);
  QGridLayout* aGroupLay = new QGridLayout(myGroupBox);
  ModuleBase_Tools::adjustMargins(aGroupLay);
  aGroupLay->setSpacing(4);
  aGroupLay->setColumnStretch(1, 1);
  createNextPoint();
  aBoxLay->addWidget(myGroupBox);
  aBoxLay->addStretch(1);

  myScrollArea->setWidget(aContainer);

  myWidgetValidator = new ModuleBase_WidgetValidator(this, myWorkshop);
  myExternalObjectMgr = new PartSet_ExternalObjectsMgr(theData->getProperty("use_external"),
                                         theData->getProperty("can_create_external"), true);
}

void PartSet_WidgetBSplinePoints::createNextPoint()
{
  storeCurentValue();

  QGridLayout* aGroupLay = dynamic_cast<QGridLayout*>(myGroupBox->layout());
  int row = (int)myXSpin.size();

  QString aPoleStr = translate("Pole %1");
  aPoleStr = aPoleStr.arg(myXSpin.size() + 1);

  QGroupBox* aPoleGroupBox = new QGroupBox(aPoleStr, myGroupBox);
  QGridLayout* aPoleLay = new QGridLayout(aPoleGroupBox);
  ModuleBase_Tools::adjustMargins(aPoleLay);
  aPoleLay->setSpacing(2);
  aPoleLay->setColumnStretch(1, 1);

  myXSpin.push_back(new ModuleBase_LabelValue(aPoleGroupBox, tr("X")));
  aPoleLay->addWidget(myXSpin.back(), 0, 1);
  myYSpin.push_back(new ModuleBase_LabelValue(aPoleGroupBox, tr("Y")));
  aPoleLay->addWidget(myYSpin.back(), 1, 1);

  aGroupLay->addWidget(aPoleGroupBox, row, 1);
  IsPointCreated = true;
}

void PartSet_WidgetBSplinePoints::removeLastPoint()
{
  QGridLayout* aGroupLay = dynamic_cast<QGridLayout*>(myGroupBox->layout());
  QWidget* aXSpin = myXSpin.back();
  QWidget* aYSpin = myYSpin.back();
  QWidget* aBox = myXSpin.back()->parentWidget();
  myYSpin.pop_back();
  myXSpin.pop_back();

  aGroupLay->removeWidget(aXSpin);
  aGroupLay->removeWidget(aYSpin);
  aGroupLay->removeWidget(aBox);

  aBox->deleteLater();

  // update B-spline feature attributes
  storeValueCustom();
}

bool PartSet_WidgetBSplinePoints::isValidSelectionCustom(const ModuleBase_ViewerPrsPtr& theValue)
{
  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  if (aModule->sketchReentranceMgr()->isInternalEditActive())
    return true; // when internal edit is started a new feature is created. I has not results, AIS

  // the selection is not possible if the current feature has no presentation for the current
  // attribute not in AIS not in results. If so, no object in current feature where make
  // coincidence, so selection is not necessary
  GeomShapePtr anAISShape;
  GeomPresentablePtr aPrs = std::dynamic_pointer_cast<GeomAPI_IPresentable>(myFeature);
  if (aPrs.get()) {
    AISObjectPtr anAIS;
    anAIS = aPrs->getAISObject(anAIS);
    if (anAIS.get()) {
      anAISShape = anAIS->getShape();
    }
  }
  const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = myFeature->results();
  if (!anAISShape.get() && aResults.empty())
    return true;

  AttributeRefAttrListPtr aRefAttrList = attributeRefAttrList();
  if (aRefAttrList)
    return isValidSelectionForAttribute_(theValue, myFeature->attribute(attributeID()));
  return true;
}

bool PartSet_WidgetBSplinePoints::isValidSelectionForAttribute_(
                                            const ModuleBase_ViewerPrsPtr& theValue,
                                            const AttributePtr& theAttribute)
{
  bool aValid = false;

  // stores the current values of the widget attribute
  bool isFlushesActived, isAttributeSetInitializedBlocked, isAttributeSendUpdatedBlocked;

  AttributeRefAttrListPtr aRefAttrList = attributeRefAttrList();
  ModuleBase_WidgetValidated::blockFeatureAttribute(aRefAttrList, myFeature, true,
      isFlushesActived, isAttributeSetInitializedBlocked, isAttributeSendUpdatedBlocked);
  myWidgetValidator->storeAttributeValue(aRefAttrList);

  // saves the owner value to the widget attribute
  aValid = setSelectionCustom(theValue);
  if (aValid)
    // checks the attribute validity
    aValid = myWidgetValidator->isValidAttribute(theAttribute);

  // restores the current values of the widget attribute
  myWidgetValidator->restoreAttributeValue(aRefAttrList, aValid);
  myExternalObjectMgr->removeExternal(sketch(), myFeature, myWorkshop, true);

  ModuleBase_WidgetValidated::blockFeatureAttribute(aRefAttrList, myFeature, false,
      isFlushesActived, isAttributeSetInitializedBlocked, isAttributeSendUpdatedBlocked);
  return aValid;
}

bool PartSet_WidgetBSplinePoints::setSelectionCustom(const ModuleBase_ViewerPrsPtr& theValue)
{
  bool isDone = false;
  GeomShapePtr aShape = theValue->shape();
  if (aShape.get() && !aShape->isNull()) {
    Handle(V3d_View) aView = myWorkshop->viewer()->activeView();
    const TopoDS_Shape& aTDShape = aShape->impl<TopoDS_Shape>();
    GeomPnt2dPtr aPnt = PartSet_Tools::getPnt2d(aView, aTDShape, mySketch);
    if (aPnt) {
      fillRefAttribute(aPnt, theValue);
      isDone = true;
    }
    else if (aTDShape.ShapeType() == TopAbs_EDGE) {
      fillRefAttribute(theValue);
      isDone = true;
    }
  }
  return isDone;
}

static void fillLabels(std::vector<ModuleBase_LabelValue*>& theLabels, const double theValue)
{
  for (std::vector<ModuleBase_LabelValue*>::iterator anIt = theLabels.begin();
       anIt != theLabels.end(); ++anIt)
    (*anIt)->setValue(theValue);
}

bool PartSet_WidgetBSplinePoints::resetCustom()
{
  bool aDone = false;
  if (!isUseReset() || isComputedDefault())
    aDone = false;
  else {
    if (myValueIsCashed) {
      // if the restored value should be hidden, aDone = true to set
      // reset state for the widget in the parent
      aDone = restoreCurentValue();
      emit objectUpdated();
    }
    else {
      // it is important to block the spin box control in order to do not through out the
      // locking of the validating state.
      fillLabels(myXSpin, 0.0);
      fillLabels(myYSpin, 0.0);

      storeValueCustom();
      aDone = true;
    }
  }
  return aDone;
}

PartSet_WidgetBSplinePoints::~PartSet_WidgetBSplinePoints()
{
  delete myExternalObjectMgr;
}

bool PartSet_WidgetBSplinePoints::setPoint(double theX, double theY)
{
  if (fabs(theX) >= MaxCoordinate || fabs(theY) >= MaxCoordinate)
    return false;

  myXSpin.back()->setValue(theX);
  myYSpin.back()->setValue(theY);

  storeValue();
  return true;
}

void PartSet_WidgetBSplinePoints::storePolesAndWeights() const
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  AttributePoint2DArrayPtr aPointArray = std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(
      aData->attribute(attributeID()));
  AttributeDoubleArrayPtr aWeightsArray = aData->realArray(myWeightsAttr);

  int aSize = (int)myXSpin.size();
  aPointArray->setSize(aSize);
  aWeightsArray->setSize(aSize);

  std::vector<ModuleBase_LabelValue*>::const_iterator aXIt = myXSpin.begin();
  std::vector<ModuleBase_LabelValue*>::const_iterator aYIt = myYSpin.begin();
  for (int anIndex = 0; aXIt != myXSpin.end() && aYIt != myYSpin.end(); ++anIndex, ++aXIt, ++aYIt)
    aPointArray->setPnt(anIndex, (*aXIt)->value(), (*aYIt)->value());

  double aWeight = Config_PropManager::real(SKETCH_TAB_NAME, "spline_weight");
  for (int anIndex = 0; anIndex < aSize; ++anIndex)
    aWeightsArray->setValue(anIndex, aWeight);
}

bool PartSet_WidgetBSplinePoints::storeValueCustom()
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  if (!aData || !aData->isValid()) // can be on abort of sketcher element
    return false;
  AttributePoint2DArrayPtr aPointArray = std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(
      aData->attribute(attributeID()));
  AttributeDoubleArrayPtr aWeightsArray = aData->realArray(myWeightsAttr);

  PartSet_WidgetBSplinePoints* that = (PartSet_WidgetBSplinePoints*) this;
  bool isBlocked = that->blockSignals(true);
  bool isImmutable = aPointArray->setImmutable(true);

  if (myFeature->isMacro()) {
    // Moving points of macro-features has been processed directly (without solver)
    storePolesAndWeights();
    updateObject(myFeature);

  } else {
    if (!aPointArray->isInitialized()) {
      storePolesAndWeights();
    }

    std::shared_ptr<ModelAPI_ObjectMovedMessage> aMessage(
        new ModelAPI_ObjectMovedMessage(this));
    aMessage->setMovedAttribute(aPointArray, aPointArray->size() - 1);
    aMessage->setOriginalPosition(aPointArray->pnt(aPointArray->size() - 1));
    aMessage->setCurrentPosition(myXSpin.back()->value(), myYSpin.back()->value());
    Events_Loop::loop()->send(aMessage);
  }

  aPointArray->setImmutable(isImmutable);
  that->blockSignals(isBlocked);

  return true;
}

bool PartSet_WidgetBSplinePoints::restoreValueCustom()
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  AttributePoint2DArrayPtr aPointArray = std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(
      aData->attribute(attributeID()));
  AttributeDoubleArrayPtr aWeightsArray = aData->realArray(myWeightsAttr);

  if (aPointArray->isInitialized()) {
    while ((int)myXSpin.size() < aPointArray->size())
      createNextPoint();

    std::vector<ModuleBase_LabelValue*>::iterator aXIt = myXSpin.begin();
    std::vector<ModuleBase_LabelValue*>::iterator aYIt = myYSpin.begin();
    for (int anIndex = 0; aXIt != myXSpin.end() && aYIt != myYSpin.end();
         ++anIndex, ++aXIt, ++aYIt) {
      GeomPnt2dPtr aPoint = aPointArray->pnt(anIndex);
      (*aXIt)->setValue(aPoint->x());
      (*aYIt)->setValue(aPoint->y());
    }
  }
  else {
    if (myXSpin.empty())
      createNextPoint();

    myXSpin.back()->setValue(0.0);
    myYSpin.back()->setValue(0.0);
  }

  return true;
}

static void storeArray(const std::vector<ModuleBase_LabelValue*>& theLabels,
                       std::vector<double>& theValues)
{
  theValues.clear();
  theValues.reserve(theLabels.size());
  for (std::vector<ModuleBase_LabelValue*>::const_iterator anIt = theLabels.begin();
       anIt != theLabels.end(); ++anIt)
    theValues.push_back((*anIt)->value());
}

void PartSet_WidgetBSplinePoints::storeCurentValue()
{
  myValueIsCashed = true;
  myIsFeatureVisibleInCash = XGUI_Displayer::isVisible(
                       XGUI_Tools::workshop(myWorkshop)->displayer(), myFeature);

  storeArray(myXSpin, myXValueInCash);
  storeArray(myYSpin, myYValueInCash);
}

static void restoreArray(std::vector<double>& theCacheValues,
                         std::vector<ModuleBase_LabelValue*>& theLabels)
{
  std::vector<double>::iterator aCIt = theCacheValues.begin();
  std::vector<ModuleBase_LabelValue*>::iterator anIt = theLabels.begin();
  for (; anIt != theLabels.end(); ++anIt) {
    if (aCIt != theCacheValues.end())
      (*anIt)->setValue(*aCIt++);
    else
      (*anIt)->setValue(0.0);
  }
  theCacheValues.clear();
}

bool PartSet_WidgetBSplinePoints::restoreCurentValue()
{
  bool aRestoredAndHidden = true;

  bool isVisible = myIsFeatureVisibleInCash;

  myValueIsCashed = false;
  myIsFeatureVisibleInCash = true;
  // fill the control widgets by the cashed value
  restoreArray(myXValueInCash, myXSpin);
  restoreArray(myYValueInCash, myYSpin);

  // store value to the model
  storeValueCustom();
  if (isVisible) {
    setValueState(Stored);
    aRestoredAndHidden = false;
  }
  else
    aRestoredAndHidden = true;

  return aRestoredAndHidden;
}

QList<QWidget*> PartSet_WidgetBSplinePoints::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myScrollArea);
  return aControls;
}

void PartSet_WidgetBSplinePoints::selectionModes(int& theModuleSelectionModes, QIntList& theModes)
{
  theModuleSelectionModes = -1;
  theModes << TopAbs_VERTEX;
  theModes << TopAbs_EDGE;
}

void PartSet_WidgetBSplinePoints::deactivate()
{
  // the value of the control should be stored to model if it was not
  // initialized yet. It is important when we leave this control by Tab key.
  // It should not be performed by the widget activation as the preview
  // is visualized with default value. Line point is moved to origin.
  AttributePtr anAttribute = myFeature->data()->attribute(attributeID());
  if (anAttribute && !anAttribute->isInitialized())
    storeValue();

  ModuleBase_ModelWidget::deactivate();
}

void PartSet_WidgetBSplinePoints::mouseReleased(ModuleBase_IViewWindow* theWindow,
                                                QMouseEvent* theEvent)
{
  // the contex menu release by the right button should not be processed by this widget
  if (theEvent->button() != Qt::LeftButton)
    return;

  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  Handle(V3d_View) aView = theWindow->v3dView();

  QList<ModuleBase_ViewerPrsPtr> aList = aSelection->getSelected(ModuleBase_ISelection::Viewer);
  ModuleBase_ViewerPrsPtr aFirstValue =
    aList.size() > 0 ? aList.first() : ModuleBase_ViewerPrsPtr();
  if (!aFirstValue.get() && myPreSelected.get()) {
    aFirstValue = myPreSelected;
  }

  TopoDS_Shape aSelectedShape;
  ObjectPtr aSelectedObject;

  // if we have selection and use it
  if (aFirstValue.get() && isValidSelectionCustom(aFirstValue) &&
      aFirstValue->shape().get()) { // Trihedron Axis may be selected, but shape is empty
    GeomShapePtr aGeomShape = aFirstValue->shape();
    aSelectedShape = aGeomShape->impl<TopoDS_Shape>();
    aSelectedObject = aFirstValue->object();

    FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(aSelectedObject);
    std::shared_ptr<SketchPlugin_Feature> aSPFeature;
    if (aSelectedFeature.get())
      aSPFeature = std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);

    bool isSketchExternalFeature = aSPFeature.get() && aSPFeature->isExternal();
    if ((!aSPFeature && !aSelectedShape.IsNull()) || isSketchExternalFeature) {
      ObjectPtr aFixedObject =
          PartSet_Tools::findFixedObjectByExternal(aSelectedShape, aSelectedObject, mySketch);
      if (aFixedObject)
        aSelectedObject = aFixedObject;
      else if (!isSketchExternalFeature) {
        FeaturePtr aCreatedFeature;
        aSelectedObject = PartSet_Tools::createFixedObjectByExternal(
            aGeomShape, aSelectedObject, mySketch, false, aCreatedFeature);
      }
    }
  }
  // The selection could be a center of an external circular object
  else if (aFirstValue.get() && (!aFirstValue->interactive().IsNull())) {
    Handle(PartSet_CenterPrs) aAIS =
        Handle(PartSet_CenterPrs)::DownCast(aFirstValue->interactive());
    if (!aAIS.IsNull()) {
      gp_Pnt aPntComp = aAIS->Component()->Pnt();
      GeomVertexPtr aVertPtr(new GeomAPI_Vertex(aPntComp.X(), aPntComp.Y(), aPntComp.Z()));
      aSelectedShape = aVertPtr->impl<TopoDS_Shape>();

      aSelectedObject =
          PartSet_Tools::findFixedObjectByExternal(aSelectedShape, aAIS->object(), mySketch);
      if (!aSelectedObject.get())
      {
        FeaturePtr aCreatedFeature;
        aSelectedObject = PartSet_Tools::createFixedByExternalCenter(aAIS->object(), aAIS->edge(),
            aAIS->centerType(), mySketch, false, aCreatedFeature);
      }
    }
  }

  GeomPnt2dPtr aSelectedPoint = PartSet_Tools::getPnt2d(aView, aSelectedShape, mySketch);
  if (!aSelectedPoint) {
    aSelectedPoint = PartSet_Tools::getPnt2d(theEvent, theWindow, mySketch);
    setValueState(Stored); // in case of edge selection, Apply state should also be updated
  }
  if (aSelectedObject)
    fillRefAttribute(aSelectedObject);
  else
    fillRefAttribute(aSelectedPoint, aFirstValue);

  // next pole of B-spline
  createNextPoint();
}

void PartSet_WidgetBSplinePoints::mouseMoved(ModuleBase_IViewWindow* theWindow,
                                             QMouseEvent* theEvent)
{
  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());

  if (myFinished || isEditingMode() || aModule->sketchReentranceMgr()->isInternalEditActive())
    return;

  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWindow->v3dView());

  double aX = 0, aY = 0;
  PartSet_Tools::convertTo2D(aPoint, mySketch, theWindow->v3dView(), aX, aY);
  if (myState != ModifiedInViewer)
    storeCurentValue();
  // we need to block the value state change
  bool isBlocked = blockValueState(true);
  setPoint(aX, aY);
  blockValueState(isBlocked);
  setValueState(ModifiedInViewer);

  if (IsPointCreated) {
    QPoint aPnt = myGroupBox->geometry().bottomLeft();
    myScrollArea->ensureVisible(aPnt.x(), aPnt.y());
    IsPointCreated = false;
  }
}

bool PartSet_WidgetBSplinePoints::processEscape()
{
  bool isProcessed = !isEditingMode();
  if (isProcessed) {
    // remove widgets corrsponding to the last pole/weight of B-spline
    removeLastPoint();
    myFinished = true;

    emit focusOutWidget(this);
  }
  return isProcessed;
}

bool PartSet_WidgetBSplinePoints::useSelectedShapes() const
{
  return true;
}

AttributeRefAttrListPtr PartSet_WidgetBSplinePoints::attributeRefAttrList() const
{
  if (myRefAttribute.empty())
    return AttributeRefAttrListPtr();

  AttributePtr anAttributeRef = feature()->attribute(myRefAttribute);
  if (!anAttributeRef.get())
    return AttributeRefAttrListPtr();

  return std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(anAttributeRef);
}

void PartSet_WidgetBSplinePoints::fillRefAttribute(GeomPnt2dPtr theClickedPoint,
                              const std::shared_ptr<ModuleBase_ViewerPrs>& theValue)
{
  AttributeRefAttrListPtr aRefAttrList = attributeRefAttrList();
  if (!aRefAttrList.get())
    return;

  FeaturePtr aFeature = feature();
  std::string anAttribute = attributeID();

  if (aFeature.get()) {
    AttributePoint2DPtr aClickedFeaturePoint =
        PartSet_WidgetPoint2D::findFirstEqualPointInSketch(mySketch, aFeature, theClickedPoint);
    if (aClickedFeaturePoint.get())
      aRefAttrList->append(aClickedFeaturePoint);
    else
      fillRefAttribute(theValue);
  }
}

void PartSet_WidgetBSplinePoints::fillRefAttribute(const ModuleBase_ViewerPrsPtr& theValue)
{
  ObjectPtr anObject;
  if (theValue)
    anObject = getGeomSelection(theValue);
  fillRefAttribute(anObject);
}

void PartSet_WidgetBSplinePoints::fillRefAttribute(const ObjectPtr& theObject)
{
  AttributeRefAttrListPtr aRefAttrList = attributeRefAttrList();
  if (aRefAttrList.get())
    aRefAttrList->append(theObject);
}

ObjectPtr PartSet_WidgetBSplinePoints::getGeomSelection(const ModuleBase_ViewerPrsPtr& theValue)
{
  ObjectPtr anObject;
  GeomShapePtr aShape;
  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  anObject = aSelection->getResult(theValue);
  aShape = aSelection->getShape(theValue);
  myExternalObjectMgr->getGeomSelection(theValue, anObject, aShape, myWorkshop, sketch(), true);

  return anObject;
}
