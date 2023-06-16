// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#include <ModuleBase_WidgetShapeSelector.h>
#include <ModuleBase_Definitions.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_Filter.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_IconFactory.h>

#include <Config_Translator.h>
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
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeValidator.h>

#include <Config_WidgetAPI.h>

#include <GeomAPI_Shape.h>

#include <SelectMgr_ListIteratorOfListOfFilter.hxx>
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
#include <QApplication>
#include <QFormLayout>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

#include <memory>

#include <list>
#include <string>

ModuleBase_WidgetShapeSelector::ModuleBase_WidgetShapeSelector(QWidget* theParent,
                                                     ModuleBase_IWorkshop* theWorkshop,
                                                     const Config_WidgetAPI* theData)
: ModuleBase_WidgetSelector(theParent, theWorkshop, theData)
{
  QFormLayout* aLayout = new QFormLayout(this);
  ModuleBase_Tools::adjustMargins(aLayout);

  QString aLabelText = translate(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  myLabel = new QLabel(aLabelText, this);
  if (!aLabelIcon.isEmpty())
    myLabel->setPixmap(ModuleBase_IconFactory::loadPixmap(aLabelIcon));


  QString aToolTip = translate(theData->widgetTooltip());
  myTextLine = new QLineEdit(this);
  QString anObjName = QString::fromStdString(attributeID());
  myTextLine->setObjectName(anObjName);
  myTextLine->setReadOnly(true);
  myTextLine->setToolTip(aToolTip);
  myTextLine->installEventFilter(this);

  aLayout->addRow(myLabel, myTextLine);
  myLabel->setToolTip(aToolTip);

  std::string aTypes = theData->getProperty("shape_types");
  myShapeTypes = QString(aTypes.c_str()).split(' ', QString::SkipEmptyParts);
}

//********************************************************************
ModuleBase_WidgetShapeSelector::~ModuleBase_WidgetShapeSelector()
{
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::storeValueCustom()
{
  // the value is stored on the selection changed signal processing
  return true;
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::setSelection(QList<ModuleBase_ViewerPrsPtr>& theValues,
                                                  const bool theToValidate)
{
  if (theValues.empty()) {
    // In order to make reselection possible, set empty object and shape should be done
    setSelectionCustom(std::shared_ptr<ModuleBase_ViewerPrs>(new ModuleBase_ViewerPrs(
                                                  ObjectPtr(), GeomShapePtr(), NULL)));
    return false;
  }
  // it removes the processed value from the parameters list
  ModuleBase_ViewerPrsPtr aValue = theValues.takeFirst();
  bool isDone = false;

  if (!theToValidate || isValidInFilters(aValue)) {
    isDone = setSelectionCustom(aValue);
    // updateObject - to update/redisplay feature
    // it is commented in order to perfom it outside the method
    //updateObject(myFeature);
    // to storeValue()
    //emit valuesChanged();
  }
  return isDone;
}

//********************************************************************
QList<ModuleBase_ViewerPrsPtr> ModuleBase_WidgetShapeSelector::getAttributeSelection() const
{
  QList<ModuleBase_ViewerPrsPtr> aSelected;
  if(myFeature) {
    DataPtr aData = myFeature->data();
    AttributePtr anAttribute = myFeature->attribute(attributeID());

    ObjectPtr anObject = ModuleBase_Tools::getObject(anAttribute);
    std::shared_ptr<GeomAPI_Shape> aShapePtr = getShape();
    ModuleBase_ViewerPrsPtr aPrs(new ModuleBase_ViewerPrs(anObject, aShapePtr, NULL));
    aSelected.append(aPrs);
  }
  return aSelected;
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::restoreValueCustom()
{
  bool isBlocked = this->blockSignals(true);
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

void ModuleBase_WidgetShapeSelector::updateFocus()
{
  emit focusOutWidget(this);
}

//********************************************************************
QIntList ModuleBase_WidgetShapeSelector::shapeTypes() const
{
  QIntList aShapeTypes;
  foreach(QString aType, myShapeTypes) {
    aShapeTypes.append(ModuleBase_Tools::shapeType(aType));
  }
  return aShapeTypes;
}

//********************************************************************
GeomShapePtr ModuleBase_WidgetShapeSelector::getShape() const
{
  GeomShapePtr aShape;
  DataPtr aData = myFeature->data();
  if (aData->isValid()) {
    AttributePtr anAttribute = myFeature->data()->attribute(attributeID());
    aShape = ModuleBase_Tools::getShape(anAttribute, myWorkshop);
  }
  return aShape;
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::updateSelectionName()
{
  DataPtr aData = myFeature->data();
  if (!aData->isValid())
    return;

  bool isNameUpdated = false;
  AttributeSelectionPtr aSelect = aData->selection(attributeID());
  if (aSelect) {
    std::wstring aDefault = translate(getDefaultValue()).toStdWString();
    myTextLine->setText(QString::fromStdWString(aSelect->namingName(aDefault)));
    isNameUpdated = true;
  }
  if (!isNameUpdated) {
    ObjectPtr anObject = ModuleBase_Tools::getObject(myFeature->attribute(attributeID()));
    if (anObject.get() != NULL) {
      std::wstring aName = anObject->data()->name();
      myTextLine->setText(QString::fromStdWString(aName));
    } else {
      AttributeRefAttrPtr aRefAttr = aData->refattr(attributeID());
      if (aRefAttr && aRefAttr->attr().get() != NULL) {
        //myIsObject = aRefAttr->isObject();
        std::wstring anAttrName = ModuleBase_Tools::generateName(aRefAttr->attr(), myWorkshop);
        myTextLine->setText(QString::fromStdWString(anAttrName));
      }
      else {
        myTextLine->setText(translate(getDefaultValue()));
      }
    }
  }
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::isModified() const
{
  return !myTextLine->text().isEmpty();
}