// File:        ModuleBase_WidgetShapeSelector.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#include <ModuleBase_WidgetShapeSelector.h>
#include <ModuleBase_Definitions.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_WidgetValueFeature.h>

#include <Config_WidgetAPI.h>
#include <Events_Loop.h>
#include <Events_Message.h>
#include <GeomAPI_Interface.h>
#include <GeomAPI_Shape.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultBody.h>
#include <Config_WidgetAPI.h>
#include <Events_Error.h>

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

#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

#include <memory>

#include <list>
#include <string>

typedef QMap<QString, TopAbs_ShapeEnum> ShapeTypes;
static ShapeTypes MyShapeTypes;

TopAbs_ShapeEnum ModuleBase_WidgetShapeSelector::shapeType(const QString& theType)
{
  if (MyShapeTypes.count() == 0) {
    MyShapeTypes["face"] = TopAbs_FACE;
    MyShapeTypes["faces"] = TopAbs_FACE;
    MyShapeTypes["vertex"] = TopAbs_VERTEX;
    MyShapeTypes["vertices"] = TopAbs_VERTEX;
    MyShapeTypes["wire"] = TopAbs_WIRE;
    MyShapeTypes["edge"] = TopAbs_EDGE;
    MyShapeTypes["edges"] = TopAbs_EDGE;
    MyShapeTypes["shell"] = TopAbs_SHELL;
    MyShapeTypes["solid"] = TopAbs_SOLID;
    MyShapeTypes["solids"] = TopAbs_SOLID;
  }
  QString aType = theType.toLower();
  if (MyShapeTypes.contains(aType))
    return MyShapeTypes[aType];
  Events_Error::send("Shape type defined in XML is not implemented!");
  return TopAbs_SHAPE;
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

  aLayout->addWidget(myTextLine, 1);

  std::string aTypes = theData->getProperty("shape_types");
  myShapeTypes = QString(aTypes.c_str()).split(' ', QString::SkipEmptyParts);

  std::string aObjTypes = theData->getProperty("object_types");
  myObjectTypes = QString(aObjTypes.c_str()).split(' ', QString::SkipEmptyParts);

  myUseSubShapes = theData->getBooleanAttribute("use_subshapes", false); 
}

//********************************************************************
ModuleBase_WidgetShapeSelector::~ModuleBase_WidgetShapeSelector()
{
  activateSelection(false);
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::storeValue() const
{
  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(mySelectedObject);
  if (aSelectedFeature == myFeature)  // In order to avoid selection of the same object
    return false;

  DataPtr aData = myFeature->data();
  if (myUseSubShapes) {
    std::shared_ptr<ModelAPI_AttributeSelection> aSelect = 
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(aData->attribute(attributeID()));

    ResultPtr aBody = std::dynamic_pointer_cast<ModelAPI_Result>(mySelectedObject);
    if (aBody) {
      aSelect->setValue(aBody, myShape);
      updateObject(myFeature);
      return true;
    }
  } else {
    std::shared_ptr<ModelAPI_AttributeReference> aRef = 
      std::dynamic_pointer_cast<ModelAPI_AttributeReference>(aData->attribute(attributeID()));

    ObjectPtr aObject = aRef->value();
    if (!(aObject && aObject->isSame(mySelectedObject))) {
      aRef->setValue(mySelectedObject);
      updateObject(myFeature);
      return true;
    }
  }
  return false;
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::restoreValue()
{
  DataPtr aData = myFeature->data();
  bool isBlocked = this->blockSignals(true);
  if (myUseSubShapes) {
    std::shared_ptr<ModelAPI_AttributeSelection> aSelect = aData->selection(attributeID());
    if (aSelect) {
      mySelectedObject = aSelect->context();
      myShape = aSelect->value();
    }
  } else {
    std::shared_ptr<ModelAPI_AttributeReference> aRef = aData->reference(attributeID());
    mySelectedObject = aRef->value();
  }
  updateSelectionName();

  this->blockSignals(isBlocked);
  return true;
}

//********************************************************************
QList<QWidget*> ModuleBase_WidgetShapeSelector::getControls() const
{
  QList<QWidget*> aControls;
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

    // Check that the selected object is result (others can not be accepted)
    ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aObject);
    if (!aRes)
      return;

    if (myFeature) {
      // We can not select a result of our feature
      const std::list<std::shared_ptr<ModelAPI_Result>>& aResList = myFeature->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aIt;
      for (aIt = aResList.cbegin(); aIt != aResList.cend(); ++aIt) {
        if ((*aIt) == aRes)
          return;
      }
    }
    // Check that object belongs to active document or PartSet
    DocumentPtr aDoc = aRes->document();
    SessionPtr aMgr = ModelAPI_Session::get();
    if (!(aDoc == aMgr->activeDocument()) || (aDoc == aMgr->moduleDocument()))
      return;

    // Check that the result has a shape
    GeomShapePtr aShape = ModelAPI_Tools::shape(aRes);
    if (!aShape)
      return;

    /// Check that object has acceptable type
    if (!acceptObjectType(aObject)) 
      return;

    // Get sub-shapes from local selection
    if (myUseSubShapes) {
      NCollection_List<TopoDS_Shape> aShapeList;
      std::list<ObjectPtr> aOwners;
      myWorkshop->selection()->selectedShapes(aShapeList, aOwners);
      if (aShapeList.Extent() > 0) {
        aShape = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
        aShape->setImpl(new TopoDS_Shape(aShapeList.First()));
      }
    }

    // Check that the selection corresponds to selection type
    if (myUseSubShapes) {
      if (!acceptSubShape(aShape))
        return;
    } else {
      if (!acceptObjectShape(aObject))
        return;
    }
    setObject(aObject, aShape);
    //activateSelection(false);
    emit focusOutWidget(this);
  }
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::setObject(ObjectPtr theObj, std::shared_ptr<GeomAPI_Shape> theShape)
{
  mySelectedObject = theObj;
  myShape = theShape;
  if (mySelectedObject) {
    raisePanel();
    if (!myUseSubShapes) {
      static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_TOHIDE);
      ModelAPI_EventCreator::get()->sendUpdated(mySelectedObject, anEvent);
    }
  } 
  updateSelectionName();
  //activateSelection(false);
  emit valuesChanged();
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::acceptObjectShape(const ObjectPtr theResult) const
{
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theResult);

  // Check that the shape of necessary type
  std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(aResult);
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
bool ModuleBase_WidgetShapeSelector::acceptSubShape(std::shared_ptr<GeomAPI_Shape> theShape) const
{
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  foreach (QString aType, myShapeTypes) {
    if (aShape.ShapeType() == shapeType(aType))
      return true;
  }
  return false;
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::acceptObjectType(const ObjectPtr theObject) const
{
  // Definition of types is not obligatory. If types are not defined then
  // it means that accepted any type
  if (myObjectTypes.isEmpty())
    return true;

  foreach (QString aType, myObjectTypes) {
    if (aType.toLower() == "construction") {
      ResultConstructionPtr aConstr = 
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theObject);
      return (aConstr != NULL);
    } // ToDo: Process other types of objects
  }
  // Object type is defined but not found
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
      myTextLine->setText("");
    }
  }
}


//********************************************************************
void ModuleBase_WidgetShapeSelector::activateSelection(bool toActivate)
{
  if (myIsActive == toActivate)
    return;
  myIsActive = toActivate;
  updateSelectionName();

  if (myIsActive) {
    connect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    if (myUseSubShapes) {

      QIntList aList;
      foreach (QString aType, myShapeTypes)
        aList.append(shapeType(aType));
      myWorkshop->activateSubShapesSelection(aList);
      if (!myObjectTypes.isEmpty()) {
        myObjTypeFilter = new ModuleBase_ObjectTypesFilter(myWorkshop, myObjectTypes);
        myWorkshop->viewer()->clearSelectionFilters();
        myWorkshop->viewer()->addSelectionFilter(myObjTypeFilter);
      }
    }
  } else {
    disconnect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    if (myUseSubShapes) {
      if (!myObjTypeFilter.IsNull()) {
        myWorkshop->viewer()->removeSelectionFilter(myObjTypeFilter);
        myObjTypeFilter.Nullify();
      }
      myWorkshop->deactivateSubShapesSelection();
    }
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
  //activateSelection(true);
  return ModuleBase_ModelWidget::focusTo();
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::eventFilter(QObject* theObj, QEvent* theEvent)
{
  //if (theObj == myTextLine) {
  //  if (theEvent->type() == QEvent::FocusIn)
  //    activateSelection(true);
  //}
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
      if (acceptObjectShape(aObject)) {
        setObject(aObject);
        return true;
      }
    }
  }
  return false;
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::activate()
{
  activateSelection(true);
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::deactivate()
{
  activateSelection(false);
}
