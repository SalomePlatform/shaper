// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetPoint2D.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "PartSet_WidgetPoint2d.h"
#include <PartSet_Tools.h>
#include <PartSet_Module.h>

#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_ISelection.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>

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

#include <cfloat>
#include <climits>

const double MaxCoordinate = 1e12;

static QStringList MyFeaturesForCoincedence;

PartSet_WidgetPoint2D::PartSet_WidgetPoint2D(QWidget* theParent, 
                                             ModuleBase_IWorkshop* theWorkshop,
                                             const Config_WidgetAPI* theData,
                                             const std::string& theParentId)
 : ModuleBase_ModelWidget(theParent, theData, theParentId), myWorkshop(theWorkshop)
{
  if (MyFeaturesForCoincedence.isEmpty()) {
    MyFeaturesForCoincedence << SketchPlugin_Line::ID().c_str()
      << SketchPlugin_Arc::ID().c_str()
      << SketchPlugin_Point::ID().c_str()
      << SketchPlugin_Circle::ID().c_str();
  }

  // the control should accept the focus, so the boolen flag is corrected to be true
  myIsObligatory = true;
  //myOptionParam = theData->getProperty(PREVIOUS_FEATURE_PARAM);
  QString aPageName = QString::fromStdString(theData->getProperty(CONTAINER_PAGE_NAME));
  myGroupBox = new QGroupBox(aPageName, theParent);
  myGroupBox->setFlat(false);

  QGridLayout* aGroupLay = new QGridLayout(myGroupBox);
  ModuleBase_Tools::adjustMargins(aGroupLay);
  aGroupLay->setSpacing(2);
  aGroupLay->setColumnStretch(1, 1);
  {
    QLabel* aLabel = new QLabel(myGroupBox);
    aLabel->setText(tr("X "));
    aGroupLay->addWidget(aLabel, 0, 0);

    myXSpin = new ModuleBase_ParamSpinBox(myGroupBox);
    myXSpin->setMinimum(-DBL_MAX);
    myXSpin->setMaximum(DBL_MAX);
    myXSpin->setToolTip(tr("X"));
    aGroupLay->addWidget(myXSpin, 0, 1);

    connect(myXSpin, SIGNAL(valueChanged(const QString&)), this, SIGNAL(valuesModified()));
  }
  {
    QLabel* aLabel = new QLabel(myGroupBox);
    aLabel->setText(tr("Y "));
    aGroupLay->addWidget(aLabel, 1, 0);

    myYSpin = new ModuleBase_ParamSpinBox(myGroupBox);
    myYSpin->setMinimum(-DBL_MAX);
    myYSpin->setMaximum(DBL_MAX);
    myYSpin->setToolTip(tr("Y"));
    aGroupLay->addWidget(myYSpin, 1, 1);

    connect(myYSpin, SIGNAL(valueChanged(const QString&)), this, SIGNAL(valuesModified()));
  }
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);
  aLayout->addWidget(myGroupBox);
  setLayout(aLayout);
}

bool PartSet_WidgetPoint2D::resetCustom()
{
  bool aDone = false;
  if (!isUseReset() || isComputedDefault() || myXSpin->hasVariable() || myYSpin->hasVariable()) {
    aDone = false;
  }
  else {
    bool isOk;
    double aDefValue = QString::fromStdString(getDefaultValue()).toDouble(&isOk);
    // it is important to block the spin box control in order to do not through out the
    // locking of the validating state.
    ModuleBase_Tools::setSpinValue(myXSpin, isOk ? aDefValue : 0.0);
    ModuleBase_Tools::setSpinValue(myYSpin, isOk ? aDefValue : 0.0);
    storeValueCustom();
    aDone = true;
  }
  return aDone;
}

PartSet_WidgetPoint2D::~PartSet_WidgetPoint2D()
{
}

bool PartSet_WidgetPoint2D::setSelection(QList<ModuleBase_ViewerPrs>& theValues,
                                         const bool theToValidate)
{
  bool isDone = false;
  if (theValues.empty())
    return isDone;

  ModuleBase_ViewerPrs aValue = theValues.takeFirst();
  TopoDS_Shape aShape = aValue.shape();
  if (!aShape.IsNull()) {
    Handle(V3d_View) aView = myWorkshop->viewer()->activeView();
    double aX, aY;
    if (getPoint2d(aView, aShape, aX, aY)) {
      isDone = setPoint(aX, aY);
    }
  }
  else if (canBeActivatedByMove()) {
    if (feature()->getKind() == SketchPlugin_Line::ID()) {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aValue.object());
      // Initialise new line with first point equal to end of previous
      if (aFeature.get()) {
        std::shared_ptr<ModelAPI_Data> aData = aFeature->data();
        std::shared_ptr<GeomDataAPI_Point2D> aPoint = 
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                       aData->attribute(SketchPlugin_Line::END_ID()));
        if (aPoint) {
          setPoint(aPoint->x(), aPoint->y());
          PartSet_Tools::setConstraints(mySketch, feature(), attributeID(), aPoint->x(),
                                        aPoint->y());
          isDone = true;
        }
      }
    }
  }
  return isDone;
}

void PartSet_WidgetPoint2D::selectContent()
{
  myXSpin->selectAll();
}

bool PartSet_WidgetPoint2D::setPoint(double theX, double theY)
{
  if (fabs(theX) >= MaxCoordinate)
    return false;
  if (fabs(theY) >= MaxCoordinate)
    return false;

  ModuleBase_Tools::setSpinValue(myXSpin, theX);
  ModuleBase_Tools::setSpinValue(myYSpin, theY);

  storeValue();
  return true;
}

bool PartSet_WidgetPoint2D::storeValueCustom() const
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  if (!aData) // can be on abort of sketcher element
    return false;
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(attributeID()));

  PartSet_WidgetPoint2D* that = (PartSet_WidgetPoint2D*) this;
  bool isBlocked = that->blockSignals(true);
  bool isImmutable = aPoint->setImmutable(true);

  // if text is not empty then setValue will be ignored
  // so we should set the text at first
  aPoint->setText(myXSpin->hasVariable() ? myXSpin->text().toStdString() : "",
                  myYSpin->hasVariable() ? myYSpin->text().toStdString() : "");
  aPoint->setValue(!myXSpin->hasVariable() ? myXSpin->value() : aPoint->x(),
                   !myYSpin->hasVariable() ? myYSpin->value() : aPoint->y());

  // after movement the solver will call the update event: optimization
  moveObject(myFeature);
  aPoint->setImmutable(isImmutable);
  that->blockSignals(isBlocked);

  return true;
}

bool PartSet_WidgetPoint2D::restoreValueCustom()
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(attributeID()));
  QString aTextX = QString::fromStdString(aPoint->textX());
  QString aTextY = QString::fromStdString(aPoint->textY());

  bool isDouble = false;
  double aVal = 0;
  if (aTextX.isEmpty()) {
    ModuleBase_Tools::setSpinValue(myXSpin, aPoint->x());
  } else {
    aVal = aTextX.toDouble(&isDouble);
    if (isDouble)
      ModuleBase_Tools::setSpinValue(myXSpin, aVal);
    else
      ModuleBase_Tools::setSpinText(myXSpin, aTextX);
  }
  if (aTextY.isEmpty()) {
    ModuleBase_Tools::setSpinValue(myYSpin, aPoint->y());
  } else {
    aVal = aTextY.toDouble(&isDouble);
    if (isDouble)
      ModuleBase_Tools::setSpinValue(myYSpin, aVal);
    else
      ModuleBase_Tools::setSpinText(myYSpin, aTextY);
  }
  //if (aTextX.empty() || aTextY.empty()) {
  //  ModuleBase_Tools::setSpinValue(myXSpin, aPoint->x());
  //  ModuleBase_Tools::setSpinValue(myYSpin, aPoint->y());
  //} else {
  //  ModuleBase_Tools::setSpinText(myXSpin, QString::fromStdString(aTextX));
  //  ModuleBase_Tools::setSpinText(myYSpin, QString::fromStdString(aTextY));
  //}
  return true;
}

QList<QWidget*> PartSet_WidgetPoint2D::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myXSpin);
  aControls.append(myYSpin);
  return aControls;
}


void PartSet_WidgetPoint2D::activateCustom()
{
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
  connect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)), 
          this, SLOT(onMouseMove(ModuleBase_IViewWindow*, QMouseEvent*)));
  connect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)), 
          this, SLOT(onMouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)));

  QIntList aModes;
  aModes << TopAbs_VERTEX;
  aModes << TopAbs_EDGE;
  myWorkshop->activateSubShapesSelection(aModes);
}

bool PartSet_WidgetPoint2D::canBeActivatedByMove()
{
  bool aCanBeActivated = false;
  if (feature()->getKind() == SketchPlugin_Line::ID() &&
      attributeID() == SketchPlugin_Line::START_ID())
    aCanBeActivated = true;

  return aCanBeActivated;
}

void PartSet_WidgetPoint2D::deactivate()
{
  ModuleBase_ModelWidget::deactivate();
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
  disconnect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)),
             this, SLOT(onMouseMove(ModuleBase_IViewWindow*, QMouseEvent*)));
  disconnect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)), 
             this, SLOT(onMouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)));

  myWorkshop->deactivateSubShapesSelection();
}

bool PartSet_WidgetPoint2D::getPoint2d(const Handle(V3d_View)& theView, 
                                       const TopoDS_Shape& theShape, 
                                       double& theX, double& theY) const
{
  if (!theShape.IsNull()) {
    if (theShape.ShapeType() == TopAbs_VERTEX) {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(theShape);
      if (!aVertex.IsNull()) {
        // A case when point is taken from existing vertex
        gp_Pnt aPoint = BRep_Tool::Pnt(aVertex);
        PartSet_Tools::convertTo2D(aPoint, mySketch, theView, theX, theY);
        return true;
      }
    }
  }
  return false;
}

void PartSet_WidgetPoint2D::setConstraintWith(const ObjectPtr& theObject)
{
  // Create point-edge coincedence
  FeaturePtr aFeature = mySketch->addFeature(SketchPlugin_ConstraintCoincidence::ID());
  std::shared_ptr<ModelAPI_Data> aData = aFeature->data();

  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef1 = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributePtr aThisAttr = feature()->data()->attribute(attributeID());
  std::shared_ptr<GeomDataAPI_Point2D> aThisPoint = 
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aThisAttr);
  aRef1->setAttr(aThisPoint);

  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef2 = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  aRef2->setObject(theObject);

  aFeature->execute();
}

void PartSet_WidgetPoint2D::onMouseRelease(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  // the contex menu release by the right button should not be processed by this widget
  if (theEvent->button() != Qt::LeftButton)
    return;

  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  Handle(V3d_View) aView = theWnd->v3dView();
  // TODO: This fragment doesn't work because bug in OCC Viewer. It can be used after fixing.
  NCollection_List<TopoDS_Shape> aShapes;
  std::list<ObjectPtr> aObjects;
  aSelection->selectedShapes(aShapes, aObjects);
  // if we have selection
  if (aShapes.Extent() > 0) {
    TopoDS_Shape aShape = aShapes.First();
    ObjectPtr aObject = aObjects.front();
    FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(aObject);
    bool anExternal = false;
    if (aSelectedFeature.get() != NULL) {
      std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
              std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);
      if ((!aSPFeature) && (!aShape.IsNull())) {
        anExternal = true;
        ResultPtr aFixedObject = PartSet_Tools::findFixedObjectByExternal(aShape, aObject, mySketch);
        if (!aFixedObject.get())
          aObject = PartSet_Tools::createFixedObjectByExternal(aShape, aObject, mySketch);

        double aX, aY;
        if (getPoint2d(aView, aShape, aX, aY) && isFeatureContainsPoint(myFeature, aX, aY)) {
          // do not create a constraint to the point, which already used by the feature
          // if the feature contains the point, focus is not switched
          setPoint(aX, aY);
        }
        else {
          if (getPoint2d(aView, aShape, aX, aY))
            setPoint(aX, aY);
          bool anOrphanPoint = isOrphanPoint(aSelectedFeature, mySketch);
          setConstraintWith(aObject);
          if (!anOrphanPoint)
            emit vertexSelected();
          emit focusOutWidget(this);
        }
      }
    }
    if (!anExternal) {
      double aX, aY;
      bool isProcessed = false;
      if (getPoint2d(aView, aShape, aX, aY) && isFeatureContainsPoint(myFeature, aX, aY)) {
        // when the point is selected, the coordinates of the point should be set into the attribute
        // if the feature contains the point, focus is not switched
        setPoint(aX, aY);
      }
      else {
        bool anOrphanPoint = isOrphanPoint(aSelectedFeature, mySketch);
        // do not set a coincidence constraint in the attribute if the feature contains a point
        // with the same coordinates. It is important for line creation in order to do not set
        // the same constraints for the same points, oterwise the result line has zero length.
        if (getPoint2d(aView, aShape, aX, aY)) {
          setPoint(aX, aY);
          PartSet_Tools::setConstraints(mySketch, feature(), attributeID(), aX, aY);
        }
        else if (aShape.ShapeType() == TopAbs_EDGE) {
          if (MyFeaturesForCoincedence.contains(myFeature->getKind().c_str()))
            setConstraintWith(aObject);
        }
        // it is important to perform updateObject() in order to the current value is 
        // processed by Sketch Solver. Test case: line is created from a previous point
        // to some distance, but in the area of the highlighting of the point. Constraint
        // coincidence is created, after the solver is performed, the distance between the
        // points of the line becomes less than the tolerance. Validator of the line returns
        // false, the line will be aborted, but sketch stays valid.
        updateObject(feature());
        if (!anOrphanPoint)
          emit vertexSelected();
        emit focusOutWidget(this);
      }
    }
  }
  // End of Bug dependent fragment
  else {
    // A case when point is taken from mouse event
    gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWnd->v3dView());
    double aX, anY;
    PartSet_Tools::convertTo2D(aPoint, mySketch, aView, aX, anY);

    // if the feature contains the point, focus is not switched
    if (!setPoint(aX, anY) || isFeatureContainsPoint(myFeature, aX, anY))
      return;

    /// Start alternative code
    //std::shared_ptr<GeomDataAPI_Point2D> aFeaturePoint = std::dynamic_pointer_cast<
    //    GeomDataAPI_Point2D>(feature()->data()->attribute(attributeID()));
    //QList<FeaturePtr> aIgnore;
    //aIgnore.append(feature());

    //double aTolerance = aView->Convert(7);
    //std::shared_ptr<GeomDataAPI_Point2D> aAttrPnt = 
    //  PartSet_Tools::findAttributePoint(mySketch, aX, anY, aTolerance, aIgnore);
    //if (aAttrPnt.get() != NULL) {
    //  aFeaturePoint->setValue(aAttrPnt->pnt());
    //  PartSet_Tools::createConstraint(mySketch, aAttrPnt, aFeaturePoint);
    //  emit vertexSelected();
    //}
    /// End alternative code
    emit focusOutWidget(this);
  }
}


void PartSet_WidgetPoint2D::onMouseMove(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (isEditingMode())
    return;

  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWnd->v3dView());

  double aX, anY;
  PartSet_Tools::convertTo2D(aPoint, mySketch, theWnd->v3dView(), aX, anY);
  // we need to block the value state change 
  bool isBlocked = blockValueState(true);
  setPoint(aX, anY);
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

/*void PartSet_WidgetPoint2D::onValuesChanged()
{
  emit valuesChanged();
}*/

bool PartSet_WidgetPoint2D::processEnter()
{
  bool isModified = myXSpin->isModified() || myYSpin->isModified();
  if (isModified) {
    bool isXModified = myXSpin->isModified();
    emit valuesChanged();
    //onValuesChanged();
    myXSpin->clearModified();
    myYSpin->clearModified();
    if (isXModified)
      myXSpin->selectAll();
    else
      myYSpin->selectAll();
  }
  return isModified;
}

bool PartSet_WidgetPoint2D::isOrphanPoint(const FeaturePtr& theFeature,
                                          const CompositeFeaturePtr& theSketch)
{
  bool anOrphanPoint = false;
  if (theFeature.get()) {
    std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;
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

    if (aPointAttr.get()) {
      std::shared_ptr<GeomAPI_Pnt2d> aPoint = aPointAttr->pnt();
      FeaturePtr aCoincidence = PartSet_Tools::findFirstCoincidence(theFeature, aPoint);
      anOrphanPoint = true;
      // if there is at least one concident line to the point, the point is not an orphant
      if (aCoincidence.get()) {
        QList<FeaturePtr> aCoinsideLines;
        PartSet_Tools::findCoincidences(aCoincidence, aCoinsideLines,
                                        SketchPlugin_ConstraintCoincidence::ENTITY_A());
        PartSet_Tools::findCoincidences(aCoincidence, aCoinsideLines,
                                        SketchPlugin_ConstraintCoincidence::ENTITY_B());
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
