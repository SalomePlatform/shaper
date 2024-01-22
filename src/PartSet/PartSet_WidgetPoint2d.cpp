// Copyright (C) 2014-2024  CEA, EDF
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

#include "PartSet_WidgetPoint2d.h"
#include <PartSet_Tools.h>
#include <PartSet_Module.h>
#include <PartSet_SketcherReentrantMgr.h>
#include <PartSet_ExternalObjectsMgr.h>
#include <PartSet_CenterPrs.h>

#include <XGUI_Tools.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>

#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_WidgetValidator.h>
#include <ModuleBase_WidgetValidated.h>
#include <ModuleBase_LabelValue.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <Events_InfoMessage.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>

#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_Vertex.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Point.h>

#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Point.hxx>

#include <cfloat>
#include <climits>

const double MaxCoordinate = 1e12;

static QStringList MyFeaturesForCoincedence;

PartSet_WidgetPoint2D::PartSet_WidgetPoint2D(QWidget* theParent,
                                             ModuleBase_IWorkshop* theWorkshop,
                                             const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData), myWorkshop(theWorkshop),
  myValueIsCashed(false), myIsFeatureVisibleInCash(true),
  myXValueInCash(0), myYValueInCash(0)
{
  myRefAttribute = theData->getProperty("reference_attribute");
  if (MyFeaturesForCoincedence.isEmpty()) {
    MyFeaturesForCoincedence << SketchPlugin_Line::ID().c_str()
      << SketchPlugin_Arc::ID().c_str()
      << SketchPlugin_Point::ID().c_str()
      << SketchPlugin_Circle::ID().c_str();
  }

  // the control should accept the focus, so the boolean flag is corrected to be true
  myIsObligatory = true;
  QString aPageName = translate(theData->getProperty(CONTAINER_PAGE_NAME));
  myGroupBox = new QGroupBox(aPageName, theParent);
  myGroupBox->setFlat(false);

#ifdef _DEBUG
  bool aAcceptVariables =
#endif
    theData->getBooleanAttribute(DOUBLE_WDG_ACCEPT_EXPRESSIONS, true);

  QGridLayout* aGroupLay = new QGridLayout(myGroupBox);
  ModuleBase_Tools::adjustMargins(aGroupLay);
  aGroupLay->setSpacing(2);
  aGroupLay->setColumnStretch(1, 1);

  myXSpin = new ModuleBase_LabelValue(myGroupBox, tr("X"));
  aGroupLay->addWidget(myXSpin, 0, 1);
  myYSpin = new ModuleBase_LabelValue(myGroupBox, tr("Y"));
  aGroupLay->addWidget(myYSpin, 1, 1);

  QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);
  aLayout->addWidget(myGroupBox);
  setLayout(aLayout);

  myWidgetValidator = new ModuleBase_WidgetValidator(this, myWorkshop);
  myExternalObjectMgr = new PartSet_ExternalObjectsMgr(theData->getProperty("use_external"),
                                         theData->getProperty("can_create_external"), true);
}

bool PartSet_WidgetPoint2D::isValidSelectionCustom(const ModuleBase_ViewerPrsPtr& theValue)
{
  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  if (aModule->sketchReentranceMgr()->isInternalEditActive())
    return true; /// when internal edit is started a new feature is created. I has not results, AIS

  /// the selection is not possible if the current feature has no presentation for the current
  /// attribute not in AIS not in results. If so, no object in current feature where make
  /// coincidence, so selection is not necessary
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

  AttributeRefAttrPtr aRefAttr = attributeRefAttr();
  if (aRefAttr.get())
    return isValidSelectionForAttribute_(theValue, myFeature->attribute(attributeID()));
  else {
    bool aFoundPoint = false;
    /// Avoid coincidence build to passed point. Coincidence is build later only if there are no
    /// reference attribute.
    /// The condition is that the selected feature has shape that has after explore a point
    /// equal to clicked one.
    std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
    AttributePoint2DPtr aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aData->attribute(attributeID()));
    std::shared_ptr<GeomAPI_Pnt2d> aPoint = aPointAttr->pnt();
    if (anAISShape.get())
      aFoundPoint = shapeExploreHasVertex(anAISShape, aPoint, mySketch);

    /// analysis of results
    std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.cbegin();
    for (; aRIter != aResults.cend() && !aFoundPoint; aRIter++) {
      ResultPtr aResult = *aRIter;
      if (aResult.get() && aResult->shape().get()) {
        GeomShapePtr aShape = aResult->shape();
        aFoundPoint = shapeExploreHasVertex(aShape, aPoint, mySketch);
      }
    }
    return aFoundPoint;
  }
  return true;
}

//********************************************************************
bool PartSet_WidgetPoint2D::isValidSelectionForAttribute_(
                                            const ModuleBase_ViewerPrsPtr& theValue,
                                            const AttributePtr& theAttribute)
{
  bool aValid = false;

  // stores the current values of the widget attribute
  bool isFlushesActived, isAttributeSetInitializedBlocked, isAttributeSendUpdatedBlocked;

  AttributeRefAttrPtr aRefAttr = attributeRefAttr();
  ModuleBase_WidgetValidated::blockFeatureAttribute(aRefAttr, myFeature, true,
      isFlushesActived, isAttributeSetInitializedBlocked, isAttributeSendUpdatedBlocked);
  myWidgetValidator->storeAttributeValue(aRefAttr);

  // saves the owner value to the widget attribute
  aValid = setSelectionCustom(theValue);
  if (aValid)
    // checks the attribute validity
    aValid = myWidgetValidator->isValidAttribute(theAttribute);

  // restores the current values of the widget attribute
  myWidgetValidator->restoreAttributeValue(aRefAttr, aValid);
  myExternalObjectMgr->removeExternal(sketch(), myFeature, myWorkshop, true);

  ModuleBase_WidgetValidated::blockFeatureAttribute(aRefAttr, myFeature, false, isFlushesActived,
                                isAttributeSetInitializedBlocked, isAttributeSendUpdatedBlocked);
  return aValid;
}

bool PartSet_WidgetPoint2D::setSelectionCustom(const ModuleBase_ViewerPrsPtr& theValue)
{
  bool isDone = false;
  GeomShapePtr aShape = theValue->shape();
  if (aShape.get() && !aShape->isNull()) {
    Handle(V3d_View) aView = myWorkshop->viewer()->activeView();
    const TopoDS_Shape& aTDShape = aShape->impl<TopoDS_Shape>();
    GeomPnt2dPtr aPnt = PartSet_Tools::getPnt2d(aView, aTDShape, mySketch);
    if (aPnt) {
      fillRefAttribute(aPnt->x(), aPnt->y(), theValue);
      isDone = true;
    }
    else if (aTDShape.ShapeType() == TopAbs_EDGE) {
      fillRefAttribute(theValue);
      isDone = true;
    }
  }
  return isDone;
}

bool PartSet_WidgetPoint2D::resetCustom()
{
  bool aDone = false;
  if (!isUseReset() || isComputedDefault()
      /*|| myXSpin->hasVariable() || myYSpin->hasVariable()*/) {
    aDone = false;
  }
  else {
    if (myValueIsCashed) {
      // if the restored value should be hidden, aDone = true to set
      // reset state for the widget in the parent
      aDone = restoreCurentValue();
      emit objectUpdated();
    }
    else {
      bool isOk;
      double aDefValue = QString::fromStdString(getDefaultValue()).toDouble(&isOk);
      // it is important to block the spin box control in order to do not through out the
      // locking of the validating state.
      myXSpin->setValue(isOk ? aDefValue : 0.0);
      myYSpin->setValue(isOk ? aDefValue : 0.0);

      //ModuleBase_Tools::setSpinValue(myXSpin, isOk ? aDefValue : 0.0);
      //ModuleBase_Tools::setSpinValue(myYSpin, isOk ? aDefValue : 0.0);
      storeValueCustom();
      aDone = true;
    }
  }
  return aDone;
}

PartSet_WidgetPoint2D::~PartSet_WidgetPoint2D()
{
  delete myExternalObjectMgr;
}

bool PartSet_WidgetPoint2D::setSelection(QList<ModuleBase_ViewerPrsPtr>& theValues,
                                         const bool theToValidate)
{
  bool isDone = false;
  if (theValues.empty())
    return isDone;

  ModuleBase_ViewerPrsPtr aValue = theValues.takeFirst();

  if (!theToValidate || myWidgetValidator->isValidSelection(aValue)) {
    GeomShapePtr aShape = aValue->shape();
    if (aShape.get() && !aShape->isNull()) {
      Handle(V3d_View) aView = myWorkshop->viewer()->activeView();
      const TopoDS_Shape& aTDShape = aShape->impl<TopoDS_Shape>();
      GeomPnt2dPtr aPnt = PartSet_Tools::getPnt2d(aView, aTDShape, mySketch);

      if (aPnt) {
        //isDone = setPoint(aPnt->x(), aPnt->y());
        //setConstraintToPoint(aPnt->x(), aPnt->y(), aValue);
        processSelection(aValue, aPnt->x(), aPnt->y());
        isDone = true;
      }
    }
  }
  return isDone;
}

//void PartSet_WidgetPoint2D::selectContent()
//{
// // myXSpin->selectAll();
//}

bool PartSet_WidgetPoint2D::setPoint(double theX, double theY)
{
  if (fabs(theX) >= MaxCoordinate)
    return false;
  if (fabs(theY) >= MaxCoordinate)
    return false;

  myXSpin->setValue(theX);
  myYSpin->setValue(theY);

  //ModuleBase_Tools::setSpinValue(myXSpin, theX);
  //ModuleBase_Tools::setSpinValue(myYSpin, theY);

  storeValue();
  return true;
}

bool PartSet_WidgetPoint2D::storeValueCustom()
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  if (!aData || !aData->isValid()) // can be on abort of sketcher element
    return false;
  AttributePoint2DPtr aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(attributeID()));

  PartSet_WidgetPoint2D* that = (PartSet_WidgetPoint2D*) this;
  bool isBlocked = that->blockSignals(true);
  bool isImmutable = aPoint->setImmutable(true);

  // if text is not empty then setValue will be ignored
  // so we should set the text at first
  //aPoint->setText(myXSpin->hasVariable() ? myXSpin->text().toStdString() : "",
  //                myYSpin->hasVariable() ? myYSpin->text().toStdString() : "");
  //aPoint->setValue(!myXSpin->hasVariable() ? myXSpin->value() : aPoint->x(),
  //                 !myYSpin->hasVariable() ? myYSpin->value() : aPoint->y());

  if (myFeature->isMacro()) {
    // Moving points of macro-features has been processed directly (without solver)
    aPoint->setValue(myXSpin->value(), myYSpin->value());
    updateObject(myFeature);

  } else {
    if (!aPoint->isInitialized())
      aPoint->setValue(0., 0.);

    std::shared_ptr<ModelAPI_ObjectMovedMessage> aMessage(
        new ModelAPI_ObjectMovedMessage(this));
    aMessage->setMovedAttribute(aPoint);
    aMessage->setOriginalPosition(aPoint->pnt());
    aMessage->setCurrentPosition(myXSpin->value(), myYSpin->value());
    Events_Loop::loop()->send(aMessage);
  }

  aPoint->setImmutable(isImmutable);
  that->blockSignals(isBlocked);

  return true;
}

bool PartSet_WidgetPoint2D::restoreValueCustom()
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  AttributePoint2DPtr aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(attributeID()));
  double aValueX = aPoint->isInitialized() ? aPoint->x() : 0.;
  double aValueY = aPoint->isInitialized() ? aPoint->y() : 0.;
  myXSpin->setValue(aValueX);
  myYSpin->setValue(aValueY);

  return true;
}

void PartSet_WidgetPoint2D::storeCurentValue()
{
  // do not use cash if a variable is used
  //if (myXSpin->hasVariable() || myYSpin->hasVariable())
  //  return;

  myValueIsCashed = true;
  myIsFeatureVisibleInCash = XGUI_Displayer::isVisible(
                       XGUI_Tools::workshop(myWorkshop)->displayer(), myFeature);
  myXValueInCash = myXSpin->value();
  myYValueInCash = myYSpin->value();
}

bool PartSet_WidgetPoint2D::restoreCurentValue()
{
  bool aRestoredAndHidden = true;

  bool isVisible = myIsFeatureVisibleInCash;
  // fill the control widgets by the cashed value

  myValueIsCashed = false;
  myIsFeatureVisibleInCash = true;
  myXSpin->setValue(myXValueInCash);
  myYSpin->setValue(myYValueInCash);
  //ModuleBase_Tools::setSpinValue(myXSpin, myXValueInCash);
  //ModuleBase_Tools::setSpinValue(myYSpin, myYValueInCash);

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

QList<QWidget*> PartSet_WidgetPoint2D::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myXSpin);
  aControls.append(myYSpin);
  return aControls;
}

//********************************************************************
void PartSet_WidgetPoint2D::selectionModes(int& theModuleSelectionModes, QIntList& theModes)
{
  theModuleSelectionModes = -1;
  theModes << TopAbs_VERTEX;
  theModes << TopAbs_EDGE;
}

//********************************************************************
void PartSet_WidgetPoint2D::activateCustom()
{
  if (!isEditingMode()) {
    FeaturePtr aFeature = feature();
    if (aFeature.get() && aFeature->getKind() == SketchPlugin_Point::ID())
      storeValue();
  }
}

void PartSet_WidgetPoint2D::setHighlighted(bool isHighlighted)
{
}

void PartSet_WidgetPoint2D::deactivate()
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

bool PartSet_WidgetPoint2D::setConstraintToPoint(double theClickedX, double theClickedY,
                                  const std::shared_ptr<ModuleBase_ViewerPrs>& theValue)
{
  AttributeRefAttrPtr aRefAttr = attributeRefAttr();
  if (aRefAttr.get())
    fillRefAttribute(theClickedX, theClickedY, theValue);
  else {
    FeaturePtr aFeature = feature();
    std::string anAttribute = attributeID();

    if (!aFeature.get())
      return false;

    std::shared_ptr<GeomAPI_Pnt2d> aClickedPoint = std::shared_ptr<GeomAPI_Pnt2d>(
                                     new GeomAPI_Pnt2d(theClickedX, theClickedY));
    AttributePoint2DPtr aClickedFeaturePoint = findFirstEqualPointInSketch(mySketch,
                                                           aFeature, aClickedPoint);
    if (!aClickedFeaturePoint.get())
      return false;

  //  aRefAttr->setAttr(aClickedFeaturePoint);
  //else {
    // find a feature point by the selection mode
    AttributePoint2DPtr aFeaturePoint;
    if (aFeature->isMacro()) {
      // the macro feature will be removed after the operation is stopped, so we need to build
      // coincidence to possible sub-features
      aFeaturePoint = findFirstEqualPointInArgumentFeatures(aFeature, aClickedPoint);
    }
    else {
      aFeaturePoint = std::dynamic_pointer_cast<
                                     GeomDataAPI_Point2D>(aFeature->data()->attribute(anAttribute));
    }
    if (!aFeaturePoint.get())
      return false;

    PartSet_Tools::createConstraint(mySketch, aClickedFeaturePoint, aFeaturePoint);
  }
  return true;
}

bool PartSet_WidgetPoint2D::setConstraintToObject(const ObjectPtr& theObject)
{
  AttributeRefAttrPtr aRefAttr = attributeRefAttr();
  if (aRefAttr.get()) {
    fillRefAttribute(theObject);
  }
  else {
    AttributePoint2DPtr aFeaturePoint;

    if (feature()->isMacro()) {
      AttributePtr aThisAttr = feature()->data()->attribute(attributeID());
      AttributePoint2DPtr anAttrPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aThisAttr);
      if (anAttrPoint.get()) {
        // the macro feature will be removed after the operation is stopped, so we need to build
        // coincidence to possible sub-features
        aFeaturePoint = findFirstEqualPointInArgumentFeatures(feature(),
                                                                   anAttrPoint->pnt());
      }
    }
    else {
      AttributePtr aThisAttr = feature()->data()->attribute(attributeID());
      aFeaturePoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aThisAttr);
    }
    if (!aFeaturePoint.get())
      return false;

    // Create point-edge coincedence
    FeaturePtr aFeature = mySketch->addFeature(SketchPlugin_ConstraintCoincidence::ID());
    std::shared_ptr<ModelAPI_Data> aData = aFeature->data();

    std::shared_ptr<ModelAPI_AttributeRefAttr> aRef1 = std::dynamic_pointer_cast<
        ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));

    aRef1->setAttr(aFeaturePoint);

    std::shared_ptr<ModelAPI_AttributeRefAttr> aRef2 = std::dynamic_pointer_cast<
        ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
    aRef2->setObject(theObject);

    // we need to flush created signal in order to coincidence is processed by solver
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  }
  return true;
}

void PartSet_WidgetPoint2D::mouseReleased(ModuleBase_IViewWindow* theWindow, QMouseEvent* theEvent)
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

  double aX=0, aY=0;
  bool aHasPoint = false;
  if (aFirstValue.get()) {
    GeomShapePtr aShape = aFirstValue->shape();
    if (aShape.get() && aShape->shapeType() == GeomAPI_Shape::VERTEX) {
      const TopoDS_Shape& aTDShape = aShape->impl<TopoDS_Shape>();
      GeomPnt2dPtr aPnt = PartSet_Tools::getPnt2d(aView, aTDShape, mySketch);
      aX = aPnt->x();
      aY = aPnt->y();
      aHasPoint = true;
    }
  }
  if (!aHasPoint) {
    gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);
    PartSet_Tools::convertTo2D(aPoint, mySketch, aView, aX, aY);
  }
  processSelection(aFirstValue, aX, aY);
}

void PartSet_WidgetPoint2D::processSelection(const ModuleBase_ViewerPrsPtr& theValue,
  double theX, double theY)
{
  if (!setPoint(theX, theY))
    return;
  // if we have selection and use it
  if (theValue.get() && isValidSelectionCustom(theValue) && theValue->shape().get()) {
    /// Trihedron Axis may be selected, but shape is empty
    GeomShapePtr aGeomShape = theValue->shape();
    TopoDS_Shape aShape = aGeomShape->impl<TopoDS_Shape>();
    ObjectPtr aObject = theValue->object();

    FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(aObject);
    bool anExternal = false;
    std::shared_ptr<SketchPlugin_Feature> aSPFeature;
    if (aSelectedFeature.get() != NULL)
      aSPFeature = std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);

    ResultPtr aFixedObject;
    bool aSketchExternalFeature = aSPFeature.get() && aSPFeature->isExternal();
    if ((!aSPFeature && !aShape.IsNull()) || aSketchExternalFeature) {
      aFixedObject = PartSet_Tools::findFixedObjectByExternal(aShape, aObject, mySketch);
      if (aSketchExternalFeature && !aFixedObject.get()) {/// local selection on external feature
        anExternal = false;
      }
      else {
        anExternal = true;
        if (!aFixedObject.get())
        {
          FeaturePtr aCreatedFeature;
          aFixedObject = PartSet_Tools::createFixedObjectByExternal(aGeomShape, aObject, mySketch,
            false, aCreatedFeature);
        }
      }
    }
    if (anExternal) {
      if (!isFeatureContainsPoint(myFeature, theX, theY)) {
        if (aShape.ShapeType() == TopAbs_EDGE) {
          setValueState(Stored); // in case of edge selection, Apply state should also be updated
        }
        bool anOrphanPoint = aShape.ShapeType() == TopAbs_VERTEX ||
                              isOrphanPoint(aSelectedFeature, mySketch, theX, theY);
        if (anExternal) {
          // we should not stop reentrant operation on external objects because
          anOrphanPoint = true;
          // they are not participate in the contour creation excepting external vertices
          if (aShape.ShapeType() == TopAbs_VERTEX) {
            FeaturePtr aFixedFeature = ModelAPI_Feature::feature(aFixedObject);
            if (aFixedFeature.get() && aFixedFeature->getKind() == SketchPlugin_Point::ID()) {
              anOrphanPoint = isOrphanPoint(aFixedFeature, mySketch, theX, theY);
            }
          }
        }
        if (aFixedObject.get())
          setConstraintToObject(aFixedObject);
        // fignal updated should be flushed in order to visualize possible created
        // external objects e.g. selection of trihedron axis when input end arc point
        updateObject(feature());

        if (!anOrphanPoint)
          emit vertexSelected(); // it stops the reentrant operation

        myPreSelected.reset();
        emit focusOutWidget(this);
      }
    }
    else {
      if (!isFeatureContainsPoint(myFeature, theX, theY)) {
        double aX = 0, aY = 0;
        bool anOrphanPoint = isOrphanPoint(aSelectedFeature, mySketch, aX, aY);
        // do not set a coincidence constraint in the attribute if the feature contains a point
        // with the same coordinates. It is important for line creation in order to do not set
        // the same constraints for the same points, oterwise the result line has zero length.
        bool isAuxiliaryFeature = false;
        if (aShape.ShapeType() == TopAbs_VERTEX) {
          setConstraintToPoint(theX, theY, theValue);
        }
        else if (aShape.ShapeType() == TopAbs_EDGE) {
          // point is taken from mouse event and set in attribute. It should be done before setting
          // coinident constraint to the external line. If a point is created, it should be in
          // the mouse clicked point
          setConstraintToObject(aObject);
          setValueState(Stored); // in case of edge selection, Apply state should also be updated
          isAuxiliaryFeature = PartSet_Tools::isAuxiliarySketchEntity(aObject);
        }
        // it is important to perform updateObject() in order to the current value is
        // processed by Sketch Solver. Test case: line is created from a previous point
        // to some distance, but in the area of the highlighting of the point. Constraint
        // coincidence is created, after the solver is performed, the distance between the
        // points of the line becomes less than the tolerance. Validator of the line returns
        // false, the line will be aborted, but sketch stays valid.
        updateObject(feature());
        if (!anOrphanPoint && !anExternal && !isAuxiliaryFeature)
          emit vertexSelected();
        myPreSelected.reset();
        emit focusOutWidget(this);
      }
    }
  }
  // The selection could be a center of an external circular object
  else if (theValue.get() && (!theValue->interactive().IsNull())) {
    Handle(PartSet_CenterPrs) aAIS =
        Handle(PartSet_CenterPrs)::DownCast(theValue->interactive());
    if (!aAIS.IsNull()) {
      gp_Pnt aPntComp = aAIS->Component()->Pnt();
      GeomVertexPtr aVertPtr(new GeomAPI_Vertex(aPntComp.X(), aPntComp.Y(), aPntComp.Z()));
      TopoDS_Shape aShape = aVertPtr->impl<TopoDS_Shape>();

      ResultPtr aFixedObject =
          PartSet_Tools::findFixedObjectByExternal(aShape, aAIS->object(), mySketch);
      if (!aFixedObject.get())
      {
        FeaturePtr aCreatedFeature;
        aFixedObject = PartSet_Tools::createFixedByExternalCenter(aAIS->object(), aAIS->edge(),
          aAIS->centerType(), mySketch, false, aCreatedFeature);
      }
      if (aFixedObject.get())
        setConstraintToObject(aFixedObject);
      // fignal updated should be flushed in order to visualize possible created
      // external objects e.g. selection of trihedron axis when input end arc point
      updateObject(feature());

      emit vertexSelected(); // it stops the reentrant operation
      myPreSelected.reset();
      emit focusOutWidget(this);
    }
  }
  else {
    // A case when point is taken from mouse event

    // if the feature contains the point, focus is not switched
    if (isFeatureContainsPoint(myFeature, theX, theY))
      return;

    myPreSelected.reset();
    emit focusOutWidget(this);
  }
}

void PartSet_WidgetPoint2D::setPreSelection(
                               const std::shared_ptr<ModuleBase_ViewerPrs>& thePreSelected,
                               ModuleBase_IViewWindow* theWnd,
                               QMouseEvent* theEvent)
{
  myPreSelected = thePreSelected;
  mouseReleased(theWnd, theEvent);
}

void PartSet_WidgetPoint2D::getGeomSelection_(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue,
                                              ObjectPtr& theObject,
                                              GeomShapePtr& theShape)
{
  myExternalObjectMgr->getGeomSelection(theValue, theObject, theShape, myWorkshop, sketch(), true);
}

void PartSet_WidgetPoint2D::mouseMoved(ModuleBase_IViewWindow* theWindow, QMouseEvent* theEvent)
{
  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());

  if (isEditingMode() || aModule->sketchReentranceMgr()->isInternalEditActive())
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
}

double PartSet_WidgetPoint2D::x() const
{
  return myXSpin->value();
}

double PartSet_WidgetPoint2D::y() const
{
  return myYSpin->value();
}


bool PartSet_WidgetPoint2D::isFeatureContainsPoint(const FeaturePtr& theFeature,
                                                   double theX, double theY)
{
  bool aPointIsFound = false;

  if (feature()->getKind() != SketchPlugin_Line::ID())
    return aPointIsFound;

  AttributePtr aWidgetAttribute = myFeature->attribute(attributeID());

  std::shared_ptr<GeomAPI_Pnt2d> aPnt2d =
                                    std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(theX, theY));
  std::list<AttributePtr> anAttributes =
                                myFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::list<AttributePtr>::iterator anIter = anAttributes.begin();
  for(; anIter != anAttributes.end() && !aPointIsFound; anIter++) {
    AttributePoint2DPtr aPoint2DAttribute =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIter);
    if (aPoint2DAttribute == aWidgetAttribute)
      continue;
    if (aPoint2DAttribute.get() && aPoint2DAttribute->isInitialized()) {
      aPointIsFound = aPoint2DAttribute->pnt()->isEqual(aPnt2d);
    }
  }
  return aPointIsFound;
}

void PartSet_WidgetPoint2D::initializeValueByActivate()
{
}

/*void PartSet_WidgetPoint2D::onValuesChanged()
{
  emit valuesChanged();
}*/

bool PartSet_WidgetPoint2D::processEnter()
{
  return false;
  /*bool isModified = getValueState() == ModifiedInPP;
  if (isModified) {
    bool isXModified = myXSpin->hasFocus();
    emit valuesChanged();
    if (isXModified)
      myXSpin->selectAll();
    else
      myYSpin->selectAll();
  }
  return isModified;*/
}

bool PartSet_WidgetPoint2D::useSelectedShapes() const
{
  return true;
}

bool PartSet_WidgetPoint2D::isOrphanPoint(const FeaturePtr& theFeature,
                                          const CompositeFeaturePtr& theSketch,
                                          double theX, double theY)
{
  bool anOrphanPoint = false;
  if (theFeature.get()) {
    AttributePoint2DPtr aPointAttr;
    std::string aFeatureKind = theFeature->getKind();
    if (aFeatureKind == SketchPlugin_Point::ID())
      aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                       theFeature->attribute(SketchPlugin_Point::COORD_ID()));
    else if (aFeatureKind == SketchPlugin_Circle::ID())
      aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                       theFeature->attribute(SketchPlugin_Circle::CENTER_ID()));

    else if (aFeatureKind == SketchPlugin_Arc::ID())
      aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                       theFeature->attribute(SketchPlugin_Arc::CENTER_ID()));

    /// check that the geometry point with the given coordinates is the checked point
    /// e.g. in arc the (x,y) point can not coicide to the center point and it automatically
    /// means that this point is not an orphant one.
    if (aPointAttr.get()) {
      std::shared_ptr<GeomAPI_Pnt2d> aCheckedPoint = std::shared_ptr<GeomAPI_Pnt2d>(
                                                    new GeomAPI_Pnt2d(theX, theY));
      if (!aCheckedPoint->isEqual(aPointAttr->pnt()))
        return anOrphanPoint;
    }

    if (aPointAttr.get()) {
      std::shared_ptr<GeomAPI_Pnt2d> aPoint = aPointAttr->pnt();
      // we need to find coincidence features in results also, because external object(point)
      // uses refs to me in another feature.
      FeaturePtr aCoincidence = PartSet_Tools::findFirstCoincidence(theFeature, aPoint);
      anOrphanPoint = true;
      // if there is at least one concident line to the point, the point is not an orphant
      if (aCoincidence.get()) {
        QList<FeaturePtr> aCoinsideLines;
        QList<FeaturePtr> aCoins;
        QList<bool> anIsAttributes;
        PartSet_Tools::findCoincidences(aCoincidence, aCoinsideLines, aCoins,
                                        SketchPlugin_ConstraintCoincidence::ENTITY_A(),
                                        anIsAttributes);
        PartSet_Tools::findCoincidences(aCoincidence, aCoinsideLines, aCoins,
                                        SketchPlugin_ConstraintCoincidence::ENTITY_B(),
                                        anIsAttributes);
        QList<FeaturePtr>::const_iterator anIt = aCoinsideLines.begin(),
                                          aLast = aCoinsideLines.end();
        for (; anIt != aLast && anOrphanPoint; anIt++) {
          anOrphanPoint = (*anIt)->getKind() != SketchPlugin_Line::ID();
        }
      }
    }
  }
  return anOrphanPoint;
}

bool PartSet_WidgetPoint2D::shapeExploreHasVertex(const GeomShapePtr& theShape,
                                                  const std::shared_ptr<GeomAPI_Pnt2d>& thePoint,
                                                  const CompositeFeaturePtr& theSketch)
{
  std::shared_ptr<GeomAPI_Pnt> aPoint = PartSet_Tools::point3D(thePoint, theSketch);

  bool aContainPoint = false;
  GeomAPI_ShapeExplorer anExp(theShape, GeomAPI_Shape::VERTEX);
  for(; anExp.more() && !aContainPoint; anExp.next()) {
    std::shared_ptr<GeomAPI_Shape> aVertexInCompSolid = anExp.current();
    std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(aVertexInCompSolid));
    if (aVertex.get())
      aContainPoint = aPoint->isEqual(aVertex->point());
  }
  return aContainPoint;
}

AttributeRefAttrPtr PartSet_WidgetPoint2D::attributeRefAttr() const
{
  AttributeRefAttrPtr anAttribute;
  if (myRefAttribute.empty())
    return anAttribute;

  AttributePtr anAttributeRef = feature()->attribute(myRefAttribute);
  if (!anAttributeRef.get())
    return anAttribute;

  return std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttributeRef);
}

void PartSet_WidgetPoint2D::fillRefAttribute(double theClickedX, double theClickedY,
                              const std::shared_ptr<ModuleBase_ViewerPrs>& theValue)
{
  AttributeRefAttrPtr aRefAttr = attributeRefAttr();
  if (!aRefAttr.get())
    return;

  FeaturePtr aFeature = feature();
  std::string anAttribute = attributeID();

  if (aFeature.get()) {
    std::shared_ptr<GeomAPI_Pnt2d> aClickedPoint = std::shared_ptr<GeomAPI_Pnt2d>(
                                      new GeomAPI_Pnt2d(theClickedX, theClickedY));
    AttributePoint2DPtr aClickedFeaturePoint = findFirstEqualPointInSketch(mySketch,
                                                            aFeature, aClickedPoint);
    if (aClickedFeaturePoint.get())
      aRefAttr->setAttr(aClickedFeaturePoint);
    else {
      ObjectPtr anObject = getGeomSelection(theValue);
      if (anObject.get())
        aRefAttr->setObject(anObject);
    }
  }
}

void PartSet_WidgetPoint2D::fillRefAttribute(const ModuleBase_ViewerPrsPtr& theValue)
{
  fillRefAttribute(getGeomSelection(theValue));
}

void PartSet_WidgetPoint2D::fillRefAttribute(const ObjectPtr& theObject)
{
  AttributeRefAttrPtr aRefAttr = attributeRefAttr();
  if (aRefAttr.get())
    aRefAttr->setObject(theObject);
}

std::shared_ptr<GeomDataAPI_Point2D> PartSet_WidgetPoint2D::findFirstEqualPointInArgumentFeatures(
                  const FeaturePtr& theFeature, const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  std::shared_ptr<GeomDataAPI_Point2D> aFeaturePoint;

  // may be feature is not updated yet, execute is not performed and references features
  // are not created. Case: rectangle macro feature
  ModuleBase_Tools::flushUpdated(theFeature);

  std::list<AttributePtr> anAttributes = theFeature->data()->attributes(
                                          ModelAPI_AttributeRefList::typeId());
  std::list<AttributePtr>::const_iterator anIt = anAttributes.begin(), aLast = anAttributes.end();
  for (; anIt != aLast && !aFeaturePoint.get(); anIt++) {
    std::shared_ptr<ModelAPI_AttributeRefList> aCurSelList =
                                      std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*anIt);
    for (int i = 0, aNb = aCurSelList->size(); i < aNb && !aFeaturePoint.get(); i++) {
      ObjectPtr anObject = aCurSelList->object(i);
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anObject);
      if (aFeature.get())
        aFeaturePoint = findFirstEqualPoint(aFeature, thePoint);
    }
  }
  return aFeaturePoint;
}

std::shared_ptr<GeomDataAPI_Point2D> PartSet_WidgetPoint2D::findFirstEqualPoint(
                                              const FeaturePtr& theFeature,
                                              const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  std::shared_ptr<GeomDataAPI_Point2D> aFPoint;

  // find the given point in the feature attributes
  std::list<std::shared_ptr<ModelAPI_Attribute> > anAttiributes =
                                    theFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::list<std::shared_ptr<ModelAPI_Attribute> >::const_iterator anIt = anAttiributes.begin(),
      aLast = anAttiributes.end();
  ModelAPI_ValidatorsFactory* aValidators = ModelAPI_Session::get()->validators();

  for (; anIt != aLast && !aFPoint; anIt++) {
    std::shared_ptr<GeomDataAPI_Point2D> aCurPoint =
                                             std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIt);
    if (aCurPoint && aCurPoint->isInitialized() &&
        aValidators->isCase(theFeature, aCurPoint->id()) &&
        (aCurPoint->pnt()->distance(thePoint) < Precision::Confusion())) {
      aFPoint = aCurPoint;
      break;
    }
  }
  return aFPoint;
}

std::shared_ptr<GeomDataAPI_Point2D> PartSet_WidgetPoint2D::findFirstEqualPointInSketch(
                                    const CompositeFeaturePtr& theSketch,
                                    const FeaturePtr& theSkipFeature,
                                    const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  // get all sketch features. If the point with the given coordinates belong to any sketch feature,
  // the constraint is created between the feature point and the found sketch point
  std::shared_ptr<ModelAPI_Data> aData = theSketch->data();
  std::shared_ptr<ModelAPI_AttributeRefList> aRefList = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(aData->attribute(SketchPlugin_Sketch::FEATURES_ID()));

  std::list<ObjectPtr> aFeatures = aRefList->list();
  std::list<ObjectPtr>::const_iterator anIt = aFeatures.begin(), aLast = aFeatures.end();
  std::list<std::shared_ptr<ModelAPI_Attribute> > anAttiributes;

  std::shared_ptr<GeomDataAPI_Point2D> aFPoint;
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*anIt);
    if (!aFeature.get() || (theSkipFeature.get() && theSkipFeature == aFeature))
      continue;
    aFPoint = PartSet_WidgetPoint2D::findFirstEqualPoint(aFeature, thePoint);
    if (aFPoint.get())
      break;
  }
  return aFPoint;
}

ObjectPtr PartSet_WidgetPoint2D::getGeomSelection(const ModuleBase_ViewerPrsPtr& theValue)
{
  ObjectPtr anObject;
  GeomShapePtr aShape;
  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  anObject = aSelection->getResult(theValue);
  aShape = aSelection->getShape(theValue);
  myExternalObjectMgr->getGeomSelection(theValue, anObject, aShape, myWorkshop, sketch(), true);

  return anObject;
}
