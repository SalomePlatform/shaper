// File:        ModuleBase_SelectorWidget.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov


#include "ModuleBase_SelectorWidget.h"
#include "ModuleBase_IWorkshop.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeReference.h>
#include <Config_WidgetAPI.h>

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>


ModuleBase_SelectorWidget::ModuleBase_SelectorWidget(QWidget* theParent, 
                                                     ModuleBase_IWorkshop* theWorkshop, 
                                                     const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent), myWorkshop(theWorkshop)
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

  aLayout->addWidget(myTextLine);

  myActivateBtn = new QToolButton(myContainer);
  myActivateBtn->setIcon(QIcon(":icons/hand_point.png"));
  myActivateBtn->setCheckable(true);
  myActivateBtn->setToolTip(tr("Activate/Deactivate selection"));

  aLayout->addWidget(myActivateBtn);

  connect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

//********************************************************************
ModuleBase_SelectorWidget::~ModuleBase_SelectorWidget()
{
}

//********************************************************************
bool ModuleBase_SelectorWidget::storeValue(FeaturePtr theFeature)
{
  DataPtr aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeReference> aRef = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeReference>(aData->attribute(myFeatureAttributeID));

  return true;
}

//********************************************************************
bool ModuleBase_SelectorWidget::restoreValue(FeaturePtr theFeature)
{
  DataPtr aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeReference> aRef = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeReference>(aData->attribute(myFeatureAttributeID));
  
  return true;
}

//********************************************************************
bool ModuleBase_SelectorWidget::canFocusTo(const std::string& theAttributeName)
{
  return false;
}

//********************************************************************
void ModuleBase_SelectorWidget::focusTo()
{
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
  int aCount = aFeatures.size();
}