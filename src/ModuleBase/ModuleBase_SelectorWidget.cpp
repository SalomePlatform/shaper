// File:        ModuleBase_SelectorWidget.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov


#include "ModuleBase_SelectorWidget.h"
#include "ModuleBase_IWorkshop.h"

#include <Events_Loop.h>
#include <Model_Events.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeReference.h>
#include <Config_WidgetAPI.h>

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QString>
#include <QEvent>
#include <QDockWidget>


ModuleBase_SelectorWidget::ModuleBase_SelectorWidget(QWidget* theParent, 
                                                     ModuleBase_IWorkshop* theWorkshop, 
                                                     const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent), myWorkshop(theWorkshop), myActivateOnStart(false)
{
  myFeatureAttributeID = theData->widgetId();

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
}

//********************************************************************
ModuleBase_SelectorWidget::~ModuleBase_SelectorWidget()
{
}

//********************************************************************
bool ModuleBase_SelectorWidget::storeValue(FeaturePtr theFeature) const
{
  DataPtr aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeReference> aRef = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeReference>(aData->attribute(myFeatureAttributeID));

  FeaturePtr aFeature = aRef->value();
  if (!(aFeature && aFeature->isSame(mySelectedFeature))) {
    aRef->setValue(mySelectedFeature);
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_UPDATED));
  }
  return true;
}

//********************************************************************
bool ModuleBase_SelectorWidget::restoreValue(FeaturePtr theFeature)
{
  DataPtr aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeReference> aRef = aData->reference(myFeatureAttributeID);

  bool isBlocked = this->blockSignals(true);
  mySelectedFeature = aRef->value();
  updateSelectionName(); 

  this->blockSignals(isBlocked);
  return true;
}

//********************************************************************
QList<QWidget*> ModuleBase_SelectorWidget::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myLabel);
  aControls.append(myTextLine);
  aControls.append(myActivateBtn);
  return aControls;
}

//********************************************************************
void ModuleBase_SelectorWidget::onSelectionChanged()
{
  QList<FeaturePtr> aFeatures = myWorkshop->selectedFeatures();
  if (aFeatures.size() > 0) {
    FeaturePtr aFeature = aFeatures.first();
    if ((!mySelectedFeature) && (!aFeature))
      return;
    if (mySelectedFeature && aFeature && mySelectedFeature->isSame(aFeature))
      return;

    // TODO: Check that the selection corresponds to selection type
    // TODO: Use the feature kind definition like SKETCH_KIND instead of the direct text
    if (aFeature->getKind().compare("Sketch") != 0)
      return;

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
void ModuleBase_SelectorWidget::updateSelectionName()
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
bool ModuleBase_SelectorWidget::eventFilter(QObject* theObj, QEvent* theEvent)
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
void ModuleBase_SelectorWidget::enableOthersControls(bool toEnable) const
{
  QWidget* aParent = myContainer->parentWidget();
  QList<QWidget*> aChldList = aParent->findChildren<QWidget*>();
  foreach(QWidget* aWgt, aChldList) {
    if ((aWgt != myLabel) && (aWgt != myActivateBtn) && (aWgt != myTextLine) && (aWgt != myContainer))
      aWgt->setEnabled(toEnable);
  }
}

//********************************************************************
void ModuleBase_SelectorWidget::activateSelection(bool toActivate)
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
void ModuleBase_SelectorWidget::raisePanel() const
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