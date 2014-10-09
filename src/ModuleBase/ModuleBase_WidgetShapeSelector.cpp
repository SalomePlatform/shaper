// File:        ModuleBase_WidgetShapeSelector.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#include "ModuleBase_WidgetShapeSelector.h"
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ISelection.h>
#include "ModuleBase_WidgetValue.h"
#include <ModuleBase_Tools.h>
#include "ModuleBase_WidgetValueFeature.h"

#include <Events_Loop.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Tools.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_AttributeReference.h>
#include <Config_WidgetAPI.h>

#include <GeomAPI_Shape.h>

#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QString>
#include <QEvent>
#include <QDockWidget>

#include <stdexcept>

typedef QMap<QString, TopAbs_ShapeEnum> ShapeTypes;
static ShapeTypes MyShapeTypes;

TopAbs_ShapeEnum ModuleBase_WidgetShapeSelector::shapeType(const QString& theType)
{
  if (MyShapeTypes.count() == 0) {
    MyShapeTypes["face"] = TopAbs_FACE;
    MyShapeTypes["vertex"] = TopAbs_VERTEX;
    MyShapeTypes["wire"] = TopAbs_WIRE;
    MyShapeTypes["edge"] = TopAbs_EDGE;
    MyShapeTypes["shell"] = TopAbs_SHELL;
    MyShapeTypes["solid"] = TopAbs_SOLID;
  }
  if (MyShapeTypes.contains(theType))
    return MyShapeTypes[theType];
  throw std::invalid_argument("Shape type defined in XML is not implemented!");
}

ModuleBase_WidgetShapeSelector::ModuleBase_WidgetShapeSelector(QWidget* theParent,
                                                     ModuleBase_IWorkshop* theWorkshop,
                                                     const Config_WidgetAPI* theData,
                                                     const std::string& theParentId)
    : ModuleBase_ModelWidget(theParent, theData, theParentId),
      myWorkshop(theWorkshop), myIsActive(false), myUseSubShapes(false)
{
  myContainer = new QWidget(theParent);
  QHBoxLayout* aLayout = new QHBoxLayout(myContainer);
  ModuleBase_Tools::adjustMargins(aLayout);

  QString aLabelText = QString::fromStdString(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  myLabel = new QLabel(aLabelText, myContainer);
  if (!aLabelIcon.isEmpty())
    myLabel->setPixmap(QPixmap(aLabelIcon));

  aLayout->addWidget(myLabel);

  QString aToolTip = QString::fromStdString(theData->widgetTooltip());
  myTextLine = new QLineEdit(myContainer);
  myTextLine->setReadOnly(true);
  myTextLine->setToolTip(aToolTip);
  myTextLine->installEventFilter(this);

  myBasePalet = myTextLine->palette();
  myInactivePalet = myBasePalet;
  myInactivePalet.setBrush(QPalette::Base, QBrush(Qt::gray, Qt::Dense6Pattern));
  myTextLine->setPalette(myInactivePalet);

  aLayout->addWidget(myTextLine, 1);

  std::string aTypes = theData->getProperty("shape_types");
  myShapeTypes = QString(aTypes.c_str()).split(' ');

  std::string aUseSubShapes = theData->getProperty("use_subshapes");
  if (aUseSubShapes.length() > 0) {
    QString aVal(aUseSubShapes.c_str());
    myUseSubShapes = (aVal.toUpper() == "TRUE");
  }
}

//********************************************************************
ModuleBase_WidgetShapeSelector::~ModuleBase_WidgetShapeSelector()
{
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::storeValue() const
{
  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(mySelectedObject);
  if (aSelectedFeature == myFeature)  // In order to avoid selection of the same object
    return false;

  DataPtr aData = myFeature->data();
  boost::shared_ptr<ModelAPI_AttributeReference> aRef = boost::dynamic_pointer_cast<
      ModelAPI_AttributeReference>(aData->attribute(attributeID()));

  ObjectPtr aObject = aRef->value();
  if (!(aObject && aObject->isSame(mySelectedObject))) {
    aRef->setValue(mySelectedObject);
    updateObject(myFeature);
  }
  return true;
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::restoreValue()
{
  DataPtr aData = myFeature->data();
  boost::shared_ptr<ModelAPI_AttributeReference> aRef = aData->reference(attributeID());

  bool isBlocked = this->blockSignals(true);
  mySelectedObject = aRef->value();
  updateSelectionName();

  this->blockSignals(isBlocked);
  return true;
}

//********************************************************************
QList<QWidget*> ModuleBase_WidgetShapeSelector::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myLabel);
  aControls.append(myTextLine);
  return aControls;
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::onSelectionChanged()
{
  QList<ObjectPtr> aObjects = myWorkshop->selection()->selectedObjects();
  if (aObjects.size() > 0) {
    ObjectPtr aObject = aObjects.first();
    if ((!mySelectedObject) && (!aObject))
      return;
    if (mySelectedObject && aObject && mySelectedObject->isSame(aObject))
      return;

    // Check that the selection corresponds to selection type
    if (!isAccepted(aObject))
      return;

    setObject(aObject);
    emit focusOutWidget(this);
  }
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::setObject(ObjectPtr theObj)
{
  if (mySelectedObject == theObj)
    return;
  mySelectedObject = theObj;
  if (mySelectedObject) {
    raisePanel();
    static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_TOHIDE);
    ModelAPI_EventCreator::get()->sendUpdated(mySelectedObject, anEvent);
    Events_Loop::loop()->flush(anEvent);
  } 
  updateSelectionName();
  activateSelection(false);
  emit valuesChanged();
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::isAccepted(const ObjectPtr theResult) const
{
  ResultPtr aResult = boost::dynamic_pointer_cast<ModelAPI_Result>(theResult);
  if (myFeature) {
    // We can not select a result of our feature
    const std::list<boost::shared_ptr<ModelAPI_Result>>& aRes = myFeature->results();
    std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aIt;
    for (aIt = aRes.cbegin(); aIt != aRes.cend(); ++aIt) {
      if ((*aIt) == aResult)
        return false;
    }
  }
  // Check that object belongs to active document or PartSet
  DocumentPtr aDoc = aResult->document();
  SessionPtr aMgr = ModelAPI_Session::get();
  if (!(aDoc == aMgr->activeDocument()) || (aDoc == aMgr->moduleDocument()))
    return false;

  // Check that the shape of necessary type
  boost::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(aResult);
  if (!aShapePtr)
    return false;
  TopoDS_Shape aShape = aShapePtr->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return false;

  TopAbs_ShapeEnum aShapeType = aShape.ShapeType();
  if (aShapeType == TopAbs_COMPOUND) {
    foreach (QString aType, myShapeTypes) {
      TopExp_Explorer aEx(aShape, shapeType(aType));
      if (aEx.More())
        return true;
    }
  } else {
    foreach (QString aType, myShapeTypes) {
      if (shapeType(aType) == aShapeType)
        return true;
    }
  }
  return false;
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::updateSelectionName()
{
  if (mySelectedObject) {
    std::string aName = mySelectedObject->data()->name();
    myTextLine->setText(QString::fromStdString(aName));
  } else {
    if (myIsActive) {
      QString aMsg = tr("Select a ");
      int i = 0;
      foreach (QString aType, myShapeTypes) {
        if (i > 0)
          aMsg += " or ";
        aMsg += aType;
        i++;
      }
      myTextLine->setText(aMsg);
    } else
      myTextLine->setText(tr("No object selected"));
  }
}


//********************************************************************
void ModuleBase_WidgetShapeSelector::activateSelection(bool toActivate)
{
  myIsActive = toActivate;
  if (myIsActive)
    myTextLine->setPalette(myBasePalet);
  else
    myTextLine->setPalette(myInactivePalet);
  updateSelectionName();

  if (myIsActive) {
    connect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    if (myUseSubShapes) {
      QIntList aList;
      foreach (QString aType, myShapeTypes)
        aList.append(shapeType(aType));
      myWorkshop->activateSubShapesSelection(aList);
    }
  } else {
    disconnect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    if (myUseSubShapes) 
      myWorkshop->deactivateSubShapesSelection();
  }
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::raisePanel() const
{
  QWidget* aParent = myContainer->parentWidget();
  QWidget* aLastPanel = 0;
  while (!aParent->inherits("QDockWidget")) {
    aLastPanel = aParent;
    aParent = aParent->parentWidget();
    if (!aParent)
      return;
  }
  if (aParent->inherits("QDockWidget")) {
    QDockWidget* aTabWgt = (QDockWidget*) aParent;
    aTabWgt->raise();
  }
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::focusTo()
{
  activateSelection(true);
  return true;
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::eventFilter(QObject* theObj, QEvent* theEvent)
{
  if (theObj == myTextLine) {
    if (theEvent->type() == QEvent::FocusIn)
      activateSelection(true);
  }
  return ModuleBase_ModelWidget::eventFilter(theObj, theEvent);
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::setValue(ModuleBase_WidgetValue* theValue)
{
  if (theValue) {
    ModuleBase_WidgetValueFeature* aFeatureValue =
        dynamic_cast<ModuleBase_WidgetValueFeature*>(theValue);
    if (aFeatureValue && aFeatureValue->object()) {
      ObjectPtr aObject = aFeatureValue->object();
      if (isAccepted(aObject)) {
        setObject(aObject);
        return true;
      }
    }
  }
  return false;
}

