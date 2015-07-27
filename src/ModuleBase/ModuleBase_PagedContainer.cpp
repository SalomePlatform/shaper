/*
 * ModuleBase_PagedContainer.cpp
 *
 *  Created on: Mar 13, 2015
 *      Author: sbh
 */

#include <ModuleBase_PagedContainer.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_AttributeString.h>

#include <QWidget>
#include <QList>
#include <QVBoxLayout>


ModuleBase_PagedContainer::ModuleBase_PagedContainer(QWidget* theParent, const Config_WidgetAPI* theData,
                                                     const std::string& theParentId)
: ModuleBase_ModelWidget(theParent, theData, theParentId),
  myIsFocusOnCurrentPage(false)
{
}

ModuleBase_PagedContainer::~ModuleBase_PagedContainer()
{
}

int ModuleBase_PagedContainer::addPage(ModuleBase_PageBase* thePage,
                                      const QString& theName, const QString& theCaseId)
{
  if (!myPages.count()) {
    setDefaultValue(theCaseId.toStdString());
  }
  myCaseIds << theCaseId;
  myPages << thePage;

  return myPages.count();
}

QList<QWidget*> ModuleBase_PagedContainer::getControls() const
{
  QList<QWidget*> aResult;
  int anIndex = currentPageIndex();
  QList<ModuleBase_ModelWidget*> aModelWidgets = myPages[anIndex]->modelWidgets();
  foreach(ModuleBase_ModelWidget* eachModelWidget, aModelWidgets) {
    aResult << eachModelWidget->getControls();
  }
  return aResult;
}

bool ModuleBase_PagedContainer::focusTo()
{
  int anIndex = currentPageIndex();
  if (anIndex > myPages.count())
    return false;
  return myPages[anIndex]->takeFocus();
}

void ModuleBase_PagedContainer::setHighlighted(bool)
{
  //page containers should not be highlighted, do nothing
}

void ModuleBase_PagedContainer::enableFocusProcessing()
{
  myIsFocusOnCurrentPage = true;
}

bool ModuleBase_PagedContainer::restoreValueCustom()
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
  setCurrentPageIndex(idx);
  return true;
}

void ModuleBase_PagedContainer::activateCustom()
{
  // activate current page
  focusTo();
}

bool ModuleBase_PagedContainer::storeValueCustom() const
{
  // A rare case when plugin was not loaded.
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());
  QString aWidgetValue = myCaseIds.at(currentPageIndex());
  aStringAttr->setValue(aWidgetValue.toStdString());
  updateObject(myFeature); // for preview
  return true;
}


void ModuleBase_PagedContainer::onPageChanged()
{
  storeValue();
  if (myIsFocusOnCurrentPage) focusTo();
}


