/*
 * ModuleBase_WidgetToolbox.cpp
 *
 *  Created on: Feb 27, 2015
 *      Author: sbh
 */

#include <ModuleBase_WidgetToolbox.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_AttributeString.h>

#include <QWidget>
#include <Qlist>
#include <QVBoxLayout>

ModuleBase_WidgetToolbox::ModuleBase_WidgetToolbox(QWidget* theParent, const Config_WidgetAPI* theData,
                                                   const std::string& theParentId)
: ModuleBase_ModelWidget(theParent, theData, theParentId),
  myIsPassFocusToCurrentPage(false)
{
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aMainLayout);
  myToolBox = new QToolBox(this);
  // Dark-grey rounded tabs with button-like border #and bold font
  QString css = "QToolBox::tab{background-color:#c8c8c8;"
                              "border-radius:5px;"
                              "border:1px inset;"
                              //"font-weight:700;"
                              "border-color:#fff #505050 #505050 #fff;}";
  myToolBox->setStyleSheet(css);
  // default vertical size policy is preferred
  aMainLayout->addWidget(myToolBox);

  connect(myToolBox, SIGNAL(currentChanged(int)), this, SLOT(onPageChanged()));
}

ModuleBase_WidgetToolbox::~ModuleBase_WidgetToolbox()
{
}

int ModuleBase_WidgetToolbox::addPage(ModuleBase_PageBase* thePage,
                                      const QString& theName, const QString& theCaseId)
{
  myCaseIds << theCaseId;
  myPages << thePage;
  QFrame* aFrame = dynamic_cast<QFrame*>(thePage);
  aFrame->setFrameShape(QFrame::Box);
  aFrame->setFrameStyle(QFrame::Sunken);
  return myToolBox->addItem(aFrame, theName);
}

bool ModuleBase_WidgetToolbox::restoreValue()
{
  // A rare case when plugin was not loaded.
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());
  QString aCaseId = QString::fromStdString(aStringAttr->value());
  int idx = myCaseIds.indexOf(aCaseId);
  if (idx == -1)
    return false;
  bool isSignalsBlocked = myToolBox->blockSignals(true);
  myToolBox->setCurrentIndex(idx);
  myToolBox->blockSignals(isSignalsBlocked);
  return true;
}

QList<QWidget*> ModuleBase_WidgetToolbox::getControls() const
{
  QList<QWidget*> aResult;
  int idx = myToolBox->currentIndex();
  QList<ModuleBase_ModelWidget*> aModelWidgets = myPages[idx]->modelWidgets();
  foreach(ModuleBase_ModelWidget* eachModelWidget, aModelWidgets) {
    aResult << eachModelWidget->getControls();
  }
  return aResult;
}

bool ModuleBase_WidgetToolbox::focusTo()
{
  int idx = myToolBox->currentIndex();
  if (idx > myPages.count())
    return false;
  return myPages[idx]->takeFocus();
}

void ModuleBase_WidgetToolbox::setHighlighted(bool)
{
  //page containers sould not be highlighted, do nothing
}

void ModuleBase_WidgetToolbox::enableFocusProcessing()
{
  myIsPassFocusToCurrentPage = true;
}


void ModuleBase_WidgetToolbox::activateCustom()
{
  // activate current page
  focusTo();
}

bool ModuleBase_WidgetToolbox::storeValueCustom() const
{
  // A rare case when plugin was not loaded.
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());
  QString aWidgetValue = myCaseIds.at(myToolBox->currentIndex());
  aStringAttr->setValue(aWidgetValue.toStdString());
  return true;
}

void ModuleBase_WidgetToolbox::onPageChanged()
{
  storeValue();
  if (myIsPassFocusToCurrentPage) focusTo();
}
