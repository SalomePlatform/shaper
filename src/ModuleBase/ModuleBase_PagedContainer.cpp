// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <ModuleBase_PagedContainer.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_AttributeString.h>

#include <QWidget>
#include <QList>
#include <QVBoxLayout>


static QMap<std::string, std::string> defaultValues;

ModuleBase_PagedContainer::ModuleBase_PagedContainer(QWidget* theParent,
                                                     const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData),
  myIsFocusOnCurrentPage(false), myIsFirst(true)
{
  // it is not obligatory to be ignored when property panel tries to activate next active widget
  // but if focus is moved to this control, it can accept it.
  myIsObligatory = false;
  if (defaultValues.contains(myFeatureId + attributeID()))
    myDefValue = defaultValues[myFeatureId + attributeID()];
}

ModuleBase_PagedContainer::~ModuleBase_PagedContainer()
{
}

int ModuleBase_PagedContainer::addPage(ModuleBase_PageBase* thePage,
                                      const QString& theName, const QString& theCaseId,
                                      const QPixmap& theIcon )
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
  QString aCaseId = QString::fromStdString(myDefValue.empty()?
                                           aStringAttr->value() : myDefValue);
  myIsFirst = false;
  int idx = myCaseIds.indexOf(aCaseId);
  if (idx == -1)
    idx = currentPageIndex();
  setCurrentPageIndex(idx);
  return true;
}

void ModuleBase_PagedContainer::activateCustom()
{
  // activate current page
  focusTo();
}

bool ModuleBase_PagedContainer::storeValueCustom()
{
  // A rare case when plugin was not loaded.
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();

  AttributeStringPtr aStringAttr = aData->string(attributeID());
  std::string aWidgetValue;
  if (myIsFirst)
    aWidgetValue = myDefValue.empty()?
        myCaseIds.at(currentPageIndex()).toStdString() : myDefValue;
  else
    aWidgetValue = myCaseIds.at(currentPageIndex()).toStdString();
  myDefValue = aWidgetValue;
  aStringAttr->setValue(aWidgetValue);

  myIsFirst = false;

  updateObject(myFeature); // for preview
  return true;
}


void ModuleBase_PagedContainer::onPageChanged()
{
  if (!storeValue())
    return;
  // focus might be changed only if the value is correcly stored
  // if it is not stored, reentrant manager will handle by this widget
  // after it will restart operation, the widget might be removed
  if (myIsFocusOnCurrentPage)
    focusTo();
}

void ModuleBase_PagedContainer::onFeatureAccepted()
{
  defaultValues[myFeatureId + attributeID()] = myDefValue;
}

