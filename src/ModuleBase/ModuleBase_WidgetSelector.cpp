// File:        ModuleBase_WidgetSelector.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov


#include "ModuleBase_WidgetSelector.h"
#include "ModuleBase_IWorkshop.h"

#include <Events_Loop.h>
#include <Model_Events.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
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


typedef QMap<QString, TopAbs_ShapeEnum> ShapeTypes;
static ShapeTypes MyShapeTypes;

TopAbs_ShapeEnum ModuleBase_WidgetSelector::shapeType(const QString& theType)
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




ModuleBase_WidgetSelector::ModuleBase_WidgetSelector(QWidget* theParent, 
                                                     ModuleBase_IWorkshop* theWorkshop, 
                                                     const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData), myWorkshop(theWorkshop), myActivateOnStart(false)
{
  myContainer = new QWidget(theParent);
  QHBoxLayout* aLayout = new QHBoxLayout(myContainer);

  aLayout->setContentsMargins(0, 0, 0, 0);
  QString aLabelText = QString::fromStdString(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  myLabel = new QLabel(aLabelText, myContainer);
  myLabel->setPixmap(QPixmap(aLabelIcon));

  aLayout->addWidget(myLabel);

  QString aToolTip = QString::fromStdString(theData->widgetTooltip());
  myTextLine = new QLineEdit(myContainer);
  myTextLine->setReadOnly(true);
  myTextLine->setToolTip(aToolTip);
  myTextLine->installEventFilter(this);

  aLayout->addWidget(myTextLine);

  myActivateBtn = new QToolButton(myContainer);
  myActivateBtn->setIcon(QIcon(":icons/hand_point.png"));
  myActivateBtn->setCheckable(true);
  myActivateBtn->setToolTip(tr("Activate/Deactivate selection"));
  connect(myActivateBtn, SIGNAL(toggled(bool)), this, SLOT(activateSelection(bool)));

  aLayout->addWidget(myActivateBtn);

  QString aActivateTxt = QString::fromStdString(theData->getProperty("activate"));
  if (!aActivateTxt.isNull()) {
    myActivateOnStart = (aActivateTxt == "true");
  }

  std::string aTypes = theData->getProperty("shape_types");
  myShapeTypes = QString(aTypes.c_str()).split(',');
}

//********************************************************************
ModuleBase_WidgetSelector::~ModuleBase_WidgetSelector()
{
}

//********************************************************************
bool ModuleBase_WidgetSelector::storeValue(FeaturePtr theFeature) const
{
  DataPtr aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeReference> aRef = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeReference>(aData->attribute(attributeID()));

  FeaturePtr aFeature = aRef->value();
  if (!(aFeature && aFeature->isSame(mySelectedFeature))) {
    aRef->setValue(mySelectedFeature);
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  }
  return true;
}

//********************************************************************
bool ModuleBase_WidgetSelector::restoreValue(FeaturePtr theFeature)
{
  DataPtr aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeReference> aRef = aData->reference(attributeID());

  bool isBlocked = this->blockSignals(true);
  mySelectedFeature = aRef->value();
  updateSelectionName(); 

  this->blockSignals(isBlocked);
  return true;
}

//********************************************************************
QList<QWidget*> ModuleBase_WidgetSelector::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myLabel);
  aControls.append(myTextLine);
  aControls.append(myActivateBtn);
  return aControls;
}

//********************************************************************
void ModuleBase_WidgetSelector::onSelectionChanged()
{
  QList<FeaturePtr> aFeatures = myWorkshop->selectedFeatures();
  if (aFeatures.size() > 0) {
    FeaturePtr aFeature = aFeatures.first();
    if ((!mySelectedFeature) && (!aFeature))
      return;
    if (mySelectedFeature && aFeature && mySelectedFeature->isSame(aFeature))
      return;

    // Check that the selection corresponds to selection type
    if (!isAccepted(aFeature)) return;

    mySelectedFeature = aFeature;
    if (mySelectedFeature) {
      updateSelectionName();
      activateSelection(false);
      raisePanel();
    } else {
      myTextLine->setText("");
    }
    emit valuesChanged();
  }
}

//********************************************************************
bool ModuleBase_WidgetSelector::isAccepted(const FeaturePtr theFeature) const
{
  boost::shared_ptr<GeomAPI_Shape> aShapePtr = theFeature->data()->shape();
  if (!aShapePtr) return false;
  TopoDS_Shape aShape = aShapePtr->impl<TopoDS_Shape>();
  if (aShape.IsNull()) return false;

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
void ModuleBase_WidgetSelector::updateSelectionName()
{
  if (mySelectedFeature) {
    std::string aName;
    if (mySelectedFeature->data())
      aName = mySelectedFeature->data()->getName();
    else 
      aName = boost::dynamic_pointer_cast<ModelAPI_Object>(mySelectedFeature)->getName();
 
    myTextLine->setText(QString::fromStdString(aName));
  } else 
    myTextLine->setText("");
}

//********************************************************************
bool ModuleBase_WidgetSelector::eventFilter(QObject* theObj, QEvent* theEvent)
{
  if (theObj == myTextLine) {
    if (theEvent->type() == QEvent::Polish) {
      activateSelection(myActivateOnStart);
      onSelectionChanged();
    }
  }
  return ModuleBase_ModelWidget::eventFilter(theObj, theEvent);
}

//********************************************************************
void ModuleBase_WidgetSelector::enableOthersControls(bool toEnable) const
{
  QWidget* aParent = myContainer->parentWidget();
  QList<QWidget*> aChldList = aParent->findChildren<QWidget*>();
  foreach(QWidget* aWgt, aChldList) {
    if ((aWgt != myLabel) && (aWgt != myActivateBtn) && (aWgt != myTextLine) && (aWgt != myContainer))
      aWgt->setEnabled(toEnable);
  }
}

//********************************************************************
void ModuleBase_WidgetSelector::activateSelection(bool toActivate)
{
  enableOthersControls(!toActivate);
  myTextLine->setEnabled(toActivate);

  if (toActivate)
    connect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
  else
    disconnect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));

  myActivateBtn->setDown(toActivate);
}

//********************************************************************
void ModuleBase_WidgetSelector::raisePanel() const
{
  QWidget* aParent = myContainer->parentWidget();
  QWidget* aLastPanel = 0;
  while (!aParent->inherits("QDockWidget")) {
    aLastPanel = aParent;
    aParent = aParent->parentWidget();
    if (!aParent) return;
  }
  if (aParent->inherits("QDockWidget")) {
    QDockWidget* aTabWgt = (QDockWidget*) aParent;
    aTabWgt->raise();
  }
}