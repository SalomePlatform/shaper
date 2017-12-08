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

#include <ModuleBase_WidgetFeatureSelector.h>

#include <ModuleBase_Definitions.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_Filter.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_IconFactory.h>
#include <ModuleBase_ResultPrs.h>

#include <Config_WidgetAPI.h>
#include <Events_Loop.h>
#include <Events_Message.h>
#include <GeomAPI_Interface.h>
#include <GeomAPI_Shape.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Session.h>

#include <Config_WidgetAPI.h>

#include <GeomAPI_Shape.h>

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QString>
#include <QEvent>
#include <QApplication>
#include <QFormLayout>

#include <memory>

#include <list>
#include <string>


ModuleBase_WidgetFeatureSelector::ModuleBase_WidgetFeatureSelector(QWidget* theParent,
                                                     ModuleBase_IWorkshop* theWorkshop,
                                                     const Config_WidgetAPI* theData)
: ModuleBase_WidgetValidated(theParent, theWorkshop, theData)
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
}

//********************************************************************
ModuleBase_WidgetFeatureSelector::~ModuleBase_WidgetFeatureSelector()
{
}

//********************************************************************
bool ModuleBase_WidgetFeatureSelector::setSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs)
{
  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  ObjectPtr anObject = ModelAPI_Feature::feature(thePrs->object());
  GeomShapePtr aShape;

  FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
  // the last flag is to be depending on hasObject is called before. To be corrected later
  return ModuleBase_Tools::setObject(attribute(), aFeature, aShape,
                                     myWorkshop, myIsInValidate, true);
}

//********************************************************************
void ModuleBase_WidgetFeatureSelector::deactivate()
{
  ModuleBase_ModelWidget::deactivate();
  activateFilters(false);
  myWorkshop->deactivateSubShapesSelection();
}

//********************************************************************
bool ModuleBase_WidgetFeatureSelector::processAction(ModuleBase_ActionType theActionType,
                                                     const ActionParamPtr& theParam)
{
  if (theActionType == ActionSelection)
    onSelectionChanged();
  else
    return ModuleBase_WidgetValidated::processAction(theActionType, theParam);

  return true;
}

//********************************************************************
void ModuleBase_WidgetFeatureSelector::activateCustom()
{
  activateFilters(true);

  QIntList aShapeTypes;
  aShapeTypes.push_back(ModuleBase_ResultPrs::Sel_Result);
  myWorkshop->activateSubShapesSelection(aShapeTypes);

  // Restore selection in the viewer by the attribute selection list
  // it should be postponed to have current widget as active to validate restored selection
  //static Events_ID anEvent = Events_Loop::eventByName(EVENT_UPDATE_BY_WIDGET_SELECTION);
  //ModelAPI_EventCreator::get()->sendUpdated(myFeature, anEvent);
}

//********************************************************************
bool ModuleBase_WidgetFeatureSelector::storeValueCustom()
{
  // the value is stored on the selection changed signal processing
  return true;
}

//********************************************************************
bool ModuleBase_WidgetFeatureSelector::setSelection(QList<ModuleBase_ViewerPrsPtr>& theValues,
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

  if (!theToValidate || isValidSelection(aValue))
    isDone = setSelectionCustom(aValue);

  return isDone;
}

//********************************************************************
bool ModuleBase_WidgetFeatureSelector::restoreValueCustom()
{
  bool isBlocked = this->blockSignals(true);
  updateSelectionName();
  this->blockSignals(isBlocked);

  return true;
}

//********************************************************************
QList<QWidget*> ModuleBase_WidgetFeatureSelector::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myTextLine);
  return aControls;
}

void ModuleBase_WidgetFeatureSelector::updateFocus()
{
  emit focusOutWidget(this);
}

//********************************************************************
void ModuleBase_WidgetFeatureSelector::updateSelectionName()
{
  DataPtr aData = myFeature->data();
  if (!aData->isValid())
    return;

  ObjectPtr anObject = ModuleBase_Tools::getObject(myFeature->attribute(attributeID()));
  if (anObject.get() != NULL) {
    std::string aName = anObject->data()->name();
    myTextLine->setText(QString::fromStdString(aName));
  } else {
    myTextLine->clear();
  }
}

//********************************************************************
bool ModuleBase_WidgetFeatureSelector::isValidInFilters(const ModuleBase_ViewerPrsPtr& thePrs)
{
  bool aValid = false;

  ObjectPtr anObject = thePrs->object();
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anObject);
  aValid = aFeature.get();
  if (!aValid) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
    aValid = aResult.get() && aResult->shape() == thePrs->shape();
  }

  return aValid;
}

//********************************************************************
void ModuleBase_WidgetFeatureSelector::onSelectionChanged()
{
  QList<ModuleBase_ViewerPrsPtr> aSelected = myWorkshop->selection()->getSelected(
                                                              ModuleBase_ISelection::AllControls);

  bool isDone = setSelection(aSelected, true/*false*/);
  updateOnSelectionChanged(isDone);
}

//********************************************************************
void ModuleBase_WidgetFeatureSelector::updateOnSelectionChanged(const bool theDone)
{
  // "false" flag should be used here, it connects to the #26658 OCC bug, when the user click in
  // the same place repeatedly without mouse moved. In the case validation by filters is not
  // perfromed, so an invalid object is selected. E.g. distance constraint, selection of a point.
  // the 3rd click in the same point allow using this point.
  emit valuesChanged();
  // the updateObject method should be called to flush the updated sigal. The workshop listens it,
  // calls validators for the feature and, as a result, updates the Apply button state.
  updateObject(myFeature);

  // we need to forget about previous validation result as the current selection can influence on it
  clearValidatedCash();

  if (theDone)
    updateFocus();
}


