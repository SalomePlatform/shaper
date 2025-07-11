// Copyright (C) 2014-2025  CEA, EDF
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

#include <ModuleBase_WidgetMultiSelector.h>

#include <GeomAPI_AISObject.h>

#include <ModuleBase_ActionIntParameter.h>
#include <ModuleBase_Definitions.h>
#include <ModuleBase_Events.h>
#include <ModuleBase_IconFactory.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_ISelectionActivate.h>
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ListView.h>
#include <ModuleBase_ResultPrs.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_WidgetShapeSelector.h>
#include <ModuleBase_ChoiceCtrl.h>
#include <ModuleBase_WidgetSelectionFilter.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Events.h>

#include <Config_WidgetAPI.h>

#include <AIS_InteractiveObject.hxx>

#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QObject>
#include <QString>
#include <QComboBox>
#include <QEvent>
#include <QApplication>
#include <QClipboard>
#include <QTimer>
#include <QMainWindow>
#include <QCheckBox>
#include <QPushButton>
#include <QPalette>

#include <memory>
#include <string>
#include <iostream>

//#define DEBUG_UNDO_REDO

#ifdef DEBUG_UNDO_REDO
void printHistoryInfo(const QString& theMethodName, int theCurrentHistoryIndex,
  QList<QList<std::shared_ptr<ModuleBase_ViewerPrs> > > theSelectedHistoryValues)
{
  QStringList aSizes;
  for (int i = 0; i < theSelectedHistoryValues.size(); i++)
    aSizes.append(QString::number(theSelectedHistoryValues[i].size()));

  std::cout << theMethodName.toStdString()
            << "  current = " << theCurrentHistoryIndex
            << " size(history) =  " << theSelectedHistoryValues.size()
            << " (" << aSizes.join(", ").toStdString() << ")"
            << std::endl;
}
#endif

// CSS QListView disabled stylesheet, using system colors
const QString aListViewDisabledStyle = QString(
        "QListWidget { background-color: %1; color: %2; selection-background-color: %3; selection-color: %4; }"
        "QListWidget::item:selected { border: 1px solid %5; background: %6; color: %7; }")
        .arg(QApplication::palette().color(QPalette::Disabled, QPalette::Base).name())
        .arg(QApplication::palette().color(QPalette::Disabled, QPalette::Text).name())
        .arg(QApplication::palette().color(QPalette::Disabled, QPalette::Highlight).name())
        .arg(QApplication::palette().color(QPalette::Disabled, QPalette::HighlightedText).name())
        .arg(QApplication::palette().color(QPalette::Disabled, QPalette::Highlight).name())
        .arg(QApplication::palette().color(QPalette::Disabled, QPalette::Highlight).name())
        .arg(QApplication::palette().color(QPalette::Disabled, QPalette::HighlightedText).name());

QStringList getIconsList(const QStringList& theNames)
{
  QStringList aIcons;
  foreach (QString aName, theNames) {
    QString aUName = aName.toUpper();
    if ((aUName == "VERTICES") || (aUName == "VERTEX"))
      aIcons << ":pictures/vertex32.png";
    else if ((aUName == "EDGES") || (aUName == "EDGE"))
      aIcons << ":pictures/edge32.png";
    else if ((aUName == "FACES") || (aUName == "FACE"))
      aIcons << ":pictures/face32.png";
    else if ((aUName == "SOLIDS") || (aUName == "SOLID"))
      aIcons << ":pictures/solid32.png";
  }
  return aIcons;
}

/// Stores default values of selected option (selection mode)
/// It is used only in case if myTypeCtrl is used
static QMap<std::string, std::string> defaultValues;


ModuleBase_WidgetMultiSelector::ModuleBase_WidgetMultiSelector(QWidget* theParent,
                                                               ModuleBase_IWorkshop* theWorkshop,
                                                               const Config_WidgetAPI* theData)
: ModuleBase_WidgetSelector(theParent, theWorkshop, theData),
  myIsSetSelectionBlocked(false), myCurrentHistoryIndex(-1),
  myIsFirst(true), myFiltersWgt(0), myShowOnlyBtn(0)
{
  std::string aPropertyTypes = theData->getProperty("shape_types");
  QString aTypesStr = aPropertyTypes.c_str();
  myShapeTypes = aTypesStr.split(' ', QString::SkipEmptyParts);
  myIsUseChoice = theData->getBooleanAttribute("use_choice", false);

  QString aAllowedList(theData->getProperty("allow_objects").c_str());
  if (!aAllowedList.isEmpty())
    myAllowedObjects = aAllowedList.split(' ', QString::SkipEmptyParts);

  myMainLayout = new QVBoxLayout(this);
  ModuleBase_Tools::adjustMargins(myMainLayout);


  QStringList aIconsList;
  std::string aIcons = theData->getProperty("type_icons");
  if (aIcons.size() > 0)
    aIconsList = QString(aIcons.c_str()).split(' ', QString::SkipEmptyParts);

  if (aIconsList.size() != myShapeTypes.size())
    aIconsList = getIconsList(myShapeTypes);

  myTypeCtrl = new ModuleBase_ChoiceCtrl(this, myShapeTypes, aIconsList);
  myTypeCtrl->setLabel(tr("Type"));
  if (!myShapeTypes.empty()) {
    std::string aDefType = theData->getProperty("default_type");
    if (aDefType.size() > 0) {
      bool aOk = false;
      int aId = QString(aDefType.c_str()).toInt(&aOk);
      if (aOk) {
        myTypeCtrl->setValue(aId);
        myDefMode = myShapeTypes.at(aId).toStdString();
      }
    }
    if (myDefMode.size() == 0) {
      myTypeCtrl->setValue(0);
      myDefMode = myShapeTypes.first().toStdString();
    }
  }
  myMainLayout->addWidget(myTypeCtrl);

  // There is no sense to parameterize list of types while we can not parameterize selection mode
  // if the xml definition contains one type, the controls to select a type should not be shown
  if (myShapeTypes.size() <= 1 || !myIsUseChoice) {
    myTypeCtrl->setVisible(false);
  }

  QString aLabelText = translate(theData->getProperty("label"));
  if (aLabelText.size() > 0) {
    QWidget* aLabelWgt = new QWidget(this);
    QHBoxLayout* aLabelLayout = new QHBoxLayout(aLabelWgt);
    aLabelLayout->setContentsMargins(0, 0, 0, 0);
    myMainLayout->addWidget(aLabelWgt);

    QLabel* aListLabel = new QLabel(aLabelText, this);
    aLabelLayout->addWidget(aListLabel);
    // if the xml definition contains one type, an information label
    // should be shown near to the latest
    if (myShapeTypes.size() <= 1) {
      QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
      if (!aLabelIcon.isEmpty()) {
        QLabel* aSelectedLabel = new QLabel("", this);
        aSelectedLabel->setPixmap(ModuleBase_IconFactory::loadPixmap(aLabelIcon));
        aLabelLayout->addWidget(aSelectedLabel);
        aLabelLayout->addStretch(1);
      }
    }
  }

  QString aToolTip = translate(theData->widgetTooltip());
  QString anObjName = QString::fromStdString(attributeID());
  myListView = new ModuleBase_ListView(this, anObjName, aToolTip);
  connect(myListView->getControl(), SIGNAL(itemSelectionChanged()), SLOT(onListSelection()));
  connect(myListView, SIGNAL(deleteActionClicked()), SLOT(onDeleteItem()));
  connect(myListView, SIGNAL(listActivated()), SLOT(onListActivated()));

  myMainLayout->addWidget(myListView->getControl());
  connect(myTypeCtrl, SIGNAL(valueChanged(int)), this, SLOT(onSelectionTypeChanged()));

  myUseFilters = theData->getProperty("use_filters");
  if (myUseFilters.length() > 0) {
    QWidget* aFltrWgt = new QWidget(this);
    QHBoxLayout* aFltrLayout = new QHBoxLayout(aFltrWgt);

    myFiltersWgt = new ModuleBase_FilterStarter(myUseFilters, aFltrWgt, theWorkshop);
    aFltrLayout->addWidget(myFiltersWgt);

    aFltrLayout->addStretch();

    myShowOnlyBtn = new QPushButton(tr("Show only"), aFltrWgt);
    myShowOnlyBtn->setCheckable(true);
    myShowOnlyBtn->setChecked(false);
    connect(myShowOnlyBtn, SIGNAL(toggled(bool)), SLOT(onShowOnly(bool)));

    myRemoveFiltersBtn = new QPushButton(tr("Remove filters"), aFltrWgt);
    myRemoveFiltersBtn->setEnabled(false);
    connect(myRemoveFiltersBtn, SIGNAL(clicked()), SLOT(onRemoveFilters()));

    aFltrLayout->addWidget(myRemoveFiltersBtn);
    aFltrLayout->addStretch();
    aFltrLayout->addWidget(myShowOnlyBtn);

    myMainLayout->addWidget(aFltrWgt);
  }

  bool aSameTop = theData->getBooleanAttribute("same_topology", false);
  if (aSameTop) {
    myGeomCheck = new QCheckBox(tr("Add elements that share the same topology"), this);
    myMainLayout->addWidget(myGeomCheck);
    connect(myGeomCheck, SIGNAL(toggled(bool)), SLOT(onSameTopology(bool)));
  }
  else
    myGeomCheck = 0;

  myIsNeutralPointClear = theData->getBooleanAttribute("clear_in_neutral_point", true);
  if (myShapeTypes.size() > 1 || myIsUseChoice) {
    if (defaultValues.contains(myFeatureId + attributeID())) {
      myDefMode = defaultValues[myFeatureId + attributeID()];
      myTypeCtrl->setValue(myDefMode.c_str());
    }
  }
}

ModuleBase_WidgetMultiSelector::~ModuleBase_WidgetMultiSelector()
{
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::activateCustom()
{
  ModuleBase_WidgetSelector::activateCustom();

  ModuleBase_IModule* aModule = myWorkshop->module();
  aModule->activateCustomPrs(myFeature,
                            ModuleBase_IModule::CustomizeHighlightedObjects, true);
  clearSelectedHistory();
  if (myAllowedObjects.length() > 0) {
    Handle(SelectMgr_Filter) aFilter = aModule->selectionFilter(SF_GlobalFilter);
    if (!aFilter.IsNull()) {
      Handle(ModuleBase_ShapeDocumentFilter) aDocFilter =
        Handle(ModuleBase_ShapeDocumentFilter)::DownCast(aFilter);
      if (!aDocFilter.IsNull()) {
        QStringList aSelFilters = aDocFilter->nonSelectableTypes();
        foreach(QString aType, aSelFilters) {
          if (aSelFilters.contains(aType)) {
            aDocFilter->removeNonSelectableType(aType);
            myTmpAllowed.append(aType);
          }
        }
      }
    }
  }
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::deactivate()
{
  myWorkshop->module()->enableCustomModes();

  ModuleBase_WidgetSelector::deactivate();
  if (myVisibleObjects.size())
    myShowOnlyBtn->setChecked(false);

  myWorkshop->module()->deactivateCustomPrs(ModuleBase_IModule::CustomizeHighlightedObjects, true);
  clearSelectedHistory();
  if (myTmpAllowed.length() > 0) {
    ModuleBase_IModule* aModule = myWorkshop->module();
    Handle(SelectMgr_Filter) aFilter = aModule->selectionFilter(SF_GlobalFilter);
    if (!aFilter.IsNull()) {
      Handle(ModuleBase_ShapeDocumentFilter) aDocFilter =
        Handle(ModuleBase_ShapeDocumentFilter)::DownCast(aFilter);
      if (!aDocFilter.IsNull()) {
        foreach(QString aType, myTmpAllowed) {
          aDocFilter->addNonSelectableType(aType);
        }
      }
    }
    myTmpAllowed.clear();
  }
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::updateAfterDeactivation()
{
  // restore previous Undo/Redo workshop state
  myWorkshop->updateCommandStatus();
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::updateAfterActivation()
{
  // fill Undo/Redo actions with current information
  myWorkshop->updateCommandStatus();
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::storeValueCustom()
{
  // the value is stored on the selection changed signal processing
  // A rare case when plugin was not loaded.
  if (!myFeature)
    return false;

  AttributePtr anAttribute = myFeature->data()->attribute(attributeID());
  std::string aType = anAttribute->attributeType();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
    if (myTypeCtrl->isVisible()) {
      std::string aMode = myTypeCtrl->textValue().toStdString();
      if (myIsFirst && (!myDefMode.empty()))
        aMode = myDefMode;

      aSelectionListAttr->setSelectionType(aMode);
      myIsFirst = false;
    } else { // no type, set the type as a first element of the list shape type when it is appeared
      if (aSelectionListAttr->size()) {
        AttributeSelectionPtr aSel = aSelectionListAttr->value(0);
        GeomShapePtr aFirstVal = aSel->value();
        if (!aFirstVal.get() && aSel->context().get())
          aFirstVal = aSel->context()->shape();
        if (aFirstVal.get() && !aFirstVal->isNull())
          aSelectionListAttr->setSelectionType(aFirstVal->shapeTypeStr());
      }
    }
  }
  return true;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::restoreValueCustom()
{
  // A rare case when plugin was not loaded.
  if (!myFeature)
    return false;

  AttributePtr anAttribute = myFeature->data()->attribute(attributeID());
  AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
  std::string aType = anAttribute->attributeType();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    // Restore shape type
    std::string aSelectionType = aSelectionListAttr->selectionType().c_str();
    if (aSelectionType.empty())
      aSelectionListAttr->setSelectionType(myDefMode);
    else {
      setCurrentShapeType(aSelectionType.c_str());
      myDefMode = aSelectionType;
      myIsFirst = false;
    }
  }
  if (myGeomCheck)
    myGeomCheck->setChecked(aSelectionListAttr->isGeometricalSelection());
  updateSelectionList();
  return true;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::setSelection(QList<ModuleBase_ViewerPrsPtr>& theValues,
                                                  const bool theToValidate)
{
  if (myIsSetSelectionBlocked)
    return false;

  AttributeSelectionListPtr aSelectionListAttr;
  if (attribute()->attributeType() == ModelAPI_AttributeSelectionList::typeId())
    aSelectionListAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(attribute());
  if (aSelectionListAttr.get())
    aSelectionListAttr->cashValues(true);

  /// remove unused objects from the model attribute.
  /// It should be performed before new attributes append.
  bool isDone = removeUnusedAttributeObjects(theValues);

  QList<ModuleBase_ViewerPrsPtr> anInvalidValues;
  QList<ModuleBase_ViewerPrsPtr> anAttributeValues;
  QList<ModuleBase_ViewerPrsPtr>::const_iterator anIt = theValues.begin(), aLast = theValues.end();
  for (; anIt != aLast; anIt++) {
    ModuleBase_ViewerPrsPtr aValue = *anIt;
    // do not validate and append to attribute selection presentation if it exists in the attribute
    ObjectPtr anObject;
    GeomShapePtr aShape;
    getGeomSelection(aValue, anObject, aShape);
    if (ModuleBase_Tools::hasObject(attribute(), anObject, aShape, myWorkshop, myIsInValidate)) {
      anAttributeValues.append(aValue);
      continue;
    }
    if (theToValidate && !isValidInFilters(aValue))
      anInvalidValues.append(aValue);
  }
  bool aHasInvalidValues = anInvalidValues.size() > 0;

  for (anIt = theValues.begin(); anIt != aLast; anIt++) {
    ModuleBase_ViewerPrsPtr aValue = *anIt;
    bool aProcessed = false;
    if ((aHasInvalidValues && anInvalidValues.contains(aValue)) ||
        anAttributeValues.contains(aValue))
      continue;
    aProcessed = setSelectionCustom(aValue); /// it is not optimal as hasObject() is already checked
    // if there is at least one set, the result is true
    isDone = isDone || aProcessed;
  }
  // Check the selection with validators
  QString aError = getError();
  if (aError.length() > 0) {
    aSelectionListAttr->clear();
    isDone = false;
  }
  // updateObject - to update/redisplay feature
  // it is commented in order to perform it outside the method
  //if (isDone) {
    //updateObject(myFeature);
    // this emit is necessary to call store/restore method an restore type of selection
    //emit valuesChanged();
  //}

  if (aSelectionListAttr.get())
    aSelectionListAttr->cashValues(false);

  theValues.clear();
  if (!anInvalidValues.empty())
    theValues.append(anInvalidValues);

  if (isDone) // may be the feature's result is not displayed, but attributes should be
    myWorkshop->module()->customizeFeature(myFeature, ModuleBase_IModule::CustomizeArguments,
                             true);/// hope that something is redisplayed by object updated

  return isDone;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::getHighlighted(QList<ModuleBase_ViewerPrsPtr>& theValues)
{
  std::set<int> anAttributeIds;
  getSelectedAttributeIndices(anAttributeIds);
  if (!anAttributeIds.empty())
    convertIndicesToViewerSelection(anAttributeIds, theValues);
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::canProcessAction(ModuleBase_ActionType theActionType,
                                                      bool& isActionEnabled)
{
  isActionEnabled = false;
  bool aCanProcess = false;
  switch (theActionType) {
    case ActionUndo:
    case ActionRedo: {
      aCanProcess = true;
      isActionEnabled = theActionType == ActionUndo ? myCurrentHistoryIndex > 0
          : (mySelectedHistoryValues.size() > 0 &&
             myCurrentHistoryIndex < mySelectedHistoryValues.size() - 1);
    }
    break;
    default:
      aCanProcess = ModuleBase_WidgetSelector::canProcessAction(theActionType, isActionEnabled);
    break;
  }
  return aCanProcess;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::processAction(ModuleBase_ActionType theActionType,
                                                   const ActionParamPtr& theParam)
{
  switch (theActionType) {
    case ActionUndo:
    case ActionRedo: {
      ActionIntParamPtr aParam =
        std::dynamic_pointer_cast<ModuleBase_ActionIntParameter>(theParam);
      int aNb = aParam->value();
      if (theActionType == ActionUndo)
        myCurrentHistoryIndex -= aNb;
      else
        myCurrentHistoryIndex += aNb;
      QList<ModuleBase_ViewerPrsPtr> aSelected = mySelectedHistoryValues[myCurrentHistoryIndex];
      // equal vertices should not be used here
      ModuleBase_ISelection::filterSelectionOnEqualPoints(aSelected);
      bool isDone = setSelection(aSelected,
                                 false /*need not validate because values already was in list*/);
      updateOnSelectionChanged(isDone);

      myWorkshop->updateCommandStatus();
#ifdef DEBUG_UNDO_REDO
      printHistoryInfo(QString("processAction %1").arg(theActionType == ActionUndo ? "Undo"
        : "Redo"), myCurrentHistoryIndex, mySelectedHistoryValues);
#endif
      return true;
    }
    default:
      return ModuleBase_ModelWidget::processAction(theActionType, theParam);
  }
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::isValidSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs)
{
  // avoid the highlighting when hovering over shapes when the selection is made using filters
  AttributeSelectionListPtr aAttrList = feature()->selectionList(attributeID());
  if (aAttrList.get()) {
    FiltersFeaturePtr aFilters = aAttrList->filters();
    if (aFilters.get()) {
      if (!aFilters->filters().empty()) {
        return false;
      }
    }
  }

  bool aValid = ModuleBase_WidgetSelector::isValidSelectionCustom(thePrs);
  if (aValid) {
    ResultPtr aResult = myWorkshop->selection()->getResult(thePrs);
    if (!aResult.get()) { // In case if a feature was selected
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(thePrs->object());
      if (aFeature.get())
        aResult = aFeature->firstResult();
    }
    aValid = aResult.get() != NULL;
    if (aValid) {
      if (myFeature) {
        // We can not select a result of our feature
        std::list<ResultPtr> aResults;
        ModelAPI_Tools::allResults(myFeature, aResults);
        std::list<ResultPtr>::const_iterator aIt;
        bool isSkipSelf = false;
        for (aIt = aResults.cbegin(); aIt != aResults.cend(); ++aIt) {
          if ((*aIt) == aResult) {
            isSkipSelf = true;
            break;
          }
        }
        if (isSkipSelf)
          aValid = false;
      }
    }
  }
  return aValid;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::processDelete()
{
  appendFirstSelectionInHistory();

  // find attribute indices to delete
  std::set<int> anAttributeIds;
  getSelectedAttributeIndices(anAttributeIds);

  QModelIndexList anIndices = myListView->getControl()->selectionModel()->selectedIndexes();

  // refill attribute by the items which indices are not in the list of ids
  bool aDone = false;
  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = aData->attribute(attributeID());
  std::string aType = anAttribute->attributeType();
  aDone = !anAttributeIds.empty();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = aData->selectionList(attributeID());
    aSelectionListAttr->remove(anAttributeIds);

  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr = aData->reflist(attributeID());
    aRefListAttr->remove(anAttributeIds);
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr = aData->refattrlist(attributeID());
    aRefAttrListAttr->remove(anAttributeIds);
  }

  if (aDone) {
    // update object is necessary to flush update signal. It leads to objects references map update
    // and the operation presentation will not contain deleted items visualized as parameters of
    // the feature.
    updateObject(myFeature);

    restoreValue();
    myWorkshop->setSelected(getAttributeSelection());

    // may be the feature's result is not displayed, but attributes should be
    myWorkshop->module()->customizeFeature(myFeature, ModuleBase_IModule::CustomizeArguments,
                              true); /// hope that something is redisplayed by object updated
  }

  // Restore selection
  myListView->restoreSelection(anIndices);

  appendSelectionInHistory();
  return true/*aDone*/; // following #2438 Delete should be processed even if nothing is delete
}

//********************************************************************
QList<QWidget*> ModuleBase_WidgetMultiSelector::getControls() const
{
  QList<QWidget*> result;
  if (myTypeCtrl->isVisible())
    result << myTypeCtrl;
  result << myListView->getControl();
  return result;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::onSelectionTypeChanged()
{
  // Remove filters when changing selection type
  AttributeSelectionListPtr aAttrList = feature()->selectionList(attributeID());
  if (aAttrList.get())
    aAttrList->removeFilters();

  // Clear current selection in order to avoid updating of object browser with obsolete indexes
  // which can appear because of results deletetion after changing a type of selection
  QString aSelectionType = myTypeCtrl->textValue();
  QList<ModuleBase_ViewerPrsPtr> aEmptyList;
  myWorkshop->setSelected(aEmptyList);

  updateSelectionModesAndFilters(true);
  myWorkshop->selectionActivate()->updateSelectionModes();

  if (!myFeature)
    return;
  /// store the selected type
  AttributePtr anAttribute = myFeature->data()->attribute(attributeID());
  std::string aType = anAttribute->attributeType();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
    aSelectionListAttr->setSelectionType(aSelectionType.toStdString());
  }

  // clear attribute values
  DataPtr aData = myFeature->data();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = aData->selectionList(attributeID());
    aSelectionListAttr->clear();
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr = aData->reflist(attributeID());
    aRefListAttr->clear();
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr = aData->refattrlist(attributeID());
    aRefAttrListAttr->clear();
  }

  // update object is necessary to flush update signal. It leads to objects references map update
  // and the operation presentation will not contain deleted items visualized as parameters of
  // the feature.
  updateObject(myFeature);
  restoreValue();
  myWorkshop->setSelected(getAttributeSelection());
  // may be the feature's result is not displayed, but attributes should be
  // hope that something is redisplayed by object updated
  myWorkshop->module()->customizeFeature(myFeature, ModuleBase_IModule::CustomizeArguments, false);
  myWorkshop->module()->customizeFeature(myFeature, ModuleBase_IModule::CustomizeResults, true);
  // clear history should follow after set selected to do not increase history by setSelected
  clearSelectedHistory();

  if (myWorkshop->propertyPanel()->activeWidget() != this)
    myWorkshop->propertyPanel()->activateWidget(this);
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::processSelection()
{
  if (!myIsNeutralPointClear) {
    QList<ModuleBase_ViewerPrsPtr> aSelected = getFilteredSelected();
    // do not clear selected object
    if (aSelected.size() == 0) {
      if (!getAttributeSelection().empty()) {
        // Restore selection in the viewer by the attribute selection list
        // it should be postponed to exit from the selectionChanged processing
        static Events_ID anEvent = Events_Loop::eventByName(EVENT_UPDATE_BY_WIDGET_SELECTION);
        ModelAPI_EventCreator::get()->sendUpdated(myFeature, anEvent);
        Events_Loop::loop()->flush(anEvent);
        return true;
      }
    }
  }
  appendFirstSelectionInHistory();
  bool aDone = ModuleBase_WidgetSelector::processSelection();
  appendSelectionInHistory();
  return aDone;
}

void ModuleBase_WidgetMultiSelector::appendFirstSelectionInHistory()
{
  if (mySelectedHistoryValues.empty()) {
    myCurrentHistoryIndex++;
    mySelectedHistoryValues.append(getAttributeSelection());

#ifdef DEBUG_UNDO_REDO
    printHistoryInfo("appendSelectionInHistory", myCurrentHistoryIndex, mySelectedHistoryValues);
#endif
  }
}

void ModuleBase_WidgetMultiSelector::appendSelectionInHistory()
{
  while (myCurrentHistoryIndex != mySelectedHistoryValues.count() - 1)
    mySelectedHistoryValues.removeLast();

  QList<ModuleBase_ViewerPrsPtr> aSelected = getFilteredSelected();
  myCurrentHistoryIndex++;
  mySelectedHistoryValues.append(aSelected);
  myWorkshop->updateCommandStatus();

#ifdef DEBUG_UNDO_REDO
  printHistoryInfo("appendSelectionInHistory", myCurrentHistoryIndex, mySelectedHistoryValues);
#endif
}

void ModuleBase_WidgetMultiSelector::clearSelectedHistory()
{
  mySelectedHistoryValues.clear();
  myCurrentHistoryIndex = -1;
  myWorkshop->updateCommandStatus();

#ifdef DEBUG_UNDO_REDO
  printHistoryInfo("clearSelectedHistory", myCurrentHistoryIndex, mySelectedHistoryValues);
#endif
}

void ModuleBase_WidgetMultiSelector::updateFocus()
{
  // Set focus to List control in order to make possible
  // to use Tab key for transfer the focus to next widgets
  ModuleBase_Tools::setFocus(myListView->getControl(),
                             "ModuleBase_WidgetMultiSelector::onSelectionTypeChanged()");
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::updateSelectionName()
{
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::updateOnSelectionChanged(const bool theDone)
{
  if (myIsSetSelectionBlocked)
    return;
  ModuleBase_WidgetSelector::updateOnSelectionChanged(theDone);

  // according to #2154 we need to update OB selection when selection in the viewer happens
  // it is important to flush sinchronize selection signal after flush of Update/Create/Delete.
  // because we need that Object Browser has been already updated when synchronize happens.

  // Restore selection in the viewer by the attribute selection list
  // it is possible that diring selection attribute filling, selection in Object Browser
  // is changed(some items were removed/added) and as result, selection in the viewer
  // differs from the selection come to this method. By next rows, we restore selection
  // in the viewer according to content of selection attribute. Case is Edge selection in Group
  myIsSetSelectionBlocked = true;
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_UPDATE_BY_WIDGET_SELECTION);
  ModelAPI_EventCreator::get()->sendUpdated(myFeature, anEvent);
  Events_Loop::loop()->flush(anEvent);
  myIsSetSelectionBlocked = false;
}

//********************************************************************
QIntList ModuleBase_WidgetMultiSelector::shapeTypes() const
{
  QIntList aShapeTypes;

  if (myShapeTypes.length() > 1 && myIsUseChoice) {
    QStringList aTypes = myTypeCtrl->textValue().split("|", QString::SkipEmptyParts);
    for(QString aType: aTypes) {
      aShapeTypes.append(ModuleBase_Tools::shapeType(aType));
    }
  }
  else {
    foreach (QString aType, myShapeTypes) {
      QStringList aSubTypes = aType.split("|", QString::SkipEmptyParts);
      for(QString aSubType: aSubTypes) {
        aShapeTypes.append(ModuleBase_Tools::shapeType(aSubType));
      }
    }
  }
  return aShapeTypes;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::setCurrentShapeType(const QString& theShapeType)
{
  int idx = 0;
  GeomAPI_Shape::ShapeType aShapeType = GeomAPI_Shape::shapeTypeByStr(theShapeType.toStdString());
  foreach (QString aShapeTypeName, myShapeTypes) {
    if(GeomAPI_Shape::shapeTypeByStr(aShapeTypeName.toStdString()) == aShapeType &&
       idx != myTypeCtrl->value()) {
      updateSelectionModesAndFilters(false);
      bool isBlocked = myTypeCtrl->blockSignals(true);
      myTypeCtrl->setValue(idx);
      myTypeCtrl->blockSignals(isBlocked);
      updateSelectionModesAndFilters(true);
      break;
    }
    idx++;
  }
}

QList<ModuleBase_ViewerPrsPtr> ModuleBase_WidgetMultiSelector::getAttributeSelection() const
{
  QList<ModuleBase_ViewerPrsPtr> aSelected;
  convertIndicesToViewerSelection(std::set<int>(), aSelected);
  return aSelected;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::updateSelectionList()
{
  // resets the style in case the filters have been removed in a group
  if (myUseFilters.length() > 0) {
    myListView->getControl()->setStyleSheet(""); // set default style sheet for the QListView
    if (myRemoveFiltersBtn)
      myRemoveFiltersBtn->setEnabled(false);
  }

  myListView->getControl()->clear();

  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = aData->attribute(attributeID());
  std::string aType = anAttribute->attributeType();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = aData->selectionList(attributeID());
    for (int i = 0; i < aSelectionListAttr->size(); i++) {
      AttributeSelectionPtr aAttr = aSelectionListAttr->value(i);
      myListView->addItem(QString::fromStdWString(aAttr->namingName()), i);
      FiltersFeaturePtr aFilters = aSelectionListAttr->filters();
      if (aFilters.get()) {
        if (!aFilters->filters().empty()) {
          myRemoveFiltersBtn->setEnabled(true);
          myListView->getControl()->setStyleSheet(aListViewDisabledStyle); // set disabled style sheet
        }
      }
    }
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr = aData->reflist(attributeID());
    for (int i = 0; i < aRefListAttr->size(); i++) {
      ObjectPtr anObject = aRefListAttr->object(i);
      if (anObject.get()) {
        myListView->addItem(QString::fromStdWString(anObject->data()->name()), i);
      }
    }
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr = aData->refattrlist(attributeID());
    for (int i = 0; i < aRefAttrListAttr->size(); i++) {
      AttributePtr anAttr = aRefAttrListAttr->attribute(i);
      QString aName;
      if (anAttr.get()) {
        std::wstring anAttrName = ModuleBase_Tools::generateName(anAttr, myWorkshop);
        aName = QString::fromStdWString(anAttrName);
      }
      else {
        ObjectPtr anObject = aRefAttrListAttr->object(i);
        if (anObject.get()) {
          aName = QString::fromStdWString(anObject->data()->name());
        }
      }
      myListView->addItem(aName, i);
    }
  }

  // We have to call repaint because sometimes the List control is not updated
  myListView->getControl()->update();
}

//********************************************************************
std::string ModuleBase_WidgetMultiSelector::validatorType(const QString& theType) const
{
  std::string aType;

  if (theType == "Vertices")
    aType = "vertex";
  else if (theType == "Edges")
    aType = "edge";
  else if (theType == "Faces")
    aType = "face";
  else if (theType == "Solids")
    aType = "solid";

  return aType;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::clearSelection()
{
  bool isClearInNeutralPoint = myIsNeutralPointClear;
  myIsNeutralPointClear = true;

  QList<ModuleBase_ViewerPrsPtr> anEmptyList;
  // This method will call Selection changed event which will call onSelectionChanged
  // To clear mySelection, myListView and storeValue()
  // So, we don't need to call it
  myWorkshop->setSelected(anEmptyList);

  myIsNeutralPointClear = isClearInNeutralPoint;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::onDeleteItem()
{
  processDelete();
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::onListSelection()
{
  myWorkshop->module()->customizeFeature(myFeature, ModuleBase_IModule::CustomizeHighlightedObjects,
                                         true);

  // disabling the delete action when selecting an item in a list created using filters
  AttributeSelectionListPtr aAttrList = feature()->selectionList(attributeID());
  if (aAttrList.get()) {
    FiltersFeaturePtr aFilters = aAttrList->filters();
    if (aFilters.get()) {
      if (!aFilters->filters().empty()) {
        QList<QAction*> aActions = myListView->getControl()->actions();
        for (QAction* anAction : aActions) {
          if (anAction->objectName() == "deleteAction") {
              anAction->setEnabled(false);
          }
        }
      }
    }
  }
}


//********************************************************************
void ModuleBase_WidgetMultiSelector::onRemoveFilters()
{
  AttributeSelectionListPtr aAttrList = feature()->selectionList(attributeID());
  if (aAttrList.get()) {
    aAttrList->removeFilters();
  }

  // call the same post process as onSelectionTypeChanged() slot
  updateObject(myFeature);
  restoreValue();
  myWorkshop->setSelected(getAttributeSelection());
  // may be the feature's result is not displayed, but attributes should be
  // hope that something is redisplayed by object updated
  myWorkshop->module()->customizeFeature(myFeature, ModuleBase_IModule::CustomizeArguments, false);
  myWorkshop->module()->customizeFeature(myFeature, ModuleBase_IModule::CustomizeResults, true);
  // clear history should follow after set selected to do not increase history by setSelected
  clearSelectedHistory();

  if (myWorkshop->propertyPanel()->activeWidget() != this)
    myWorkshop->propertyPanel()->activateWidget(this);
}


//********************************************************************
void ModuleBase_WidgetMultiSelector::getSelectedAttributeIndices(std::set<int>& theAttributeIds)
{
  myListView->getSelectedIndices(theAttributeIds);
}

void ModuleBase_WidgetMultiSelector::convertIndicesToViewerSelection(std::set<int> theAttributeIds,
                                                   QList<ModuleBase_ViewerPrsPtr>& theValues) const
{
  if(myFeature.get() == NULL)
    return;

  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = aData->attribute(attributeID());
  std::string aType = anAttribute->attributeType();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = aData->selectionList(attributeID());
    for (int i = 0; i < aSelectionListAttr->size(); i++) {
      // filter by attribute indices only if the container is not empty otherwise return all items
      if (!theAttributeIds.empty() && theAttributeIds.find(i) == theAttributeIds.end())
        continue;
      AttributeSelectionPtr anAttr = aSelectionListAttr->value(i);
      ObjectPtr anObject = anAttr->contextObject();
      if (anObject.get())
        theValues.append(std::shared_ptr<ModuleBase_ViewerPrs>(
               new ModuleBase_ViewerPrs(anObject, anAttr->value(), NULL)));
    }
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr = aData->reflist(attributeID());
    for (int i = 0; i < aRefListAttr->size(); i++) {
      // filter by attribute indices only if the container is not empty otherwise return all items
      if (!theAttributeIds.empty() && theAttributeIds.find(i) == theAttributeIds.end())
        continue;
      ObjectPtr anObject = aRefListAttr->object(i);
      if (anObject.get()) {
        theValues.append(std::shared_ptr<ModuleBase_ViewerPrs>(
               new ModuleBase_ViewerPrs(anObject, GeomShapePtr(), NULL)));
      }
    }
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr = aData->refattrlist(attributeID());
    for (int i = 0; i < aRefAttrListAttr->size(); i++) {
      // filter by attribute indices only if the container is not empty otherwise return all items
      if (!theAttributeIds.empty() && theAttributeIds.find(i) == theAttributeIds.end())
        continue;
      ObjectPtr anObject = aRefAttrListAttr->object(i);
      if (!anObject.get())
        continue;
      TopoDS_Shape aShape;
      AttributePtr anAttr = aRefAttrListAttr->attribute(i);
      if (anAttr.get()) {
        GeomShapePtr aGeomShape = ModuleBase_Tools::getShape(anAttr, myWorkshop);
        theValues.append(std::shared_ptr<ModuleBase_ViewerPrs>(
               new ModuleBase_ViewerPrs(anObject, aGeomShape, NULL)));
      }
    }
  }
}

bool ModuleBase_WidgetMultiSelector::removeUnusedAttributeObjects
                                                 (QList<ModuleBase_ViewerPrsPtr>& theValues)
{
  bool isDone = false;

  std::map<ObjectPtr, std::set<GeomShapePtr> > aGeomSelection = convertSelection(theValues);
  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = aData->attribute(attributeID());
  std::string aType = anAttribute->attributeType();
  std::set<GeomShapePtr> aShapes;
  std::set<int> anIndicesToBeRemoved;
  FeaturePtr aFeature;
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    // iteration through data model to find not selected elements to remove them
    AttributeSelectionListPtr aSelectionListAttr = aData->selectionList(attributeID());
    for (int i = 0; i < aSelectionListAttr->size(); i++) {
      AttributeSelectionPtr anAttr = aSelectionListAttr->value(i);
      ObjectPtr aContextObject = anAttr->contextObject();
      GeomShapePtr aShape;
      aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aContextObject);
      if (!aFeature.get())
        aShape = anAttr->value();

      bool aFound = findInSelection(aContextObject, aShape, aGeomSelection, myWorkshop);
      if (!aFound)
        anIndicesToBeRemoved.insert(i);
    }
    isDone = anIndicesToBeRemoved.size() > 0;
    if (isDone)
      aSelectionListAttr->remove(anIndicesToBeRemoved);
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr = aData->reflist(attributeID());
    for (int i = 0; i < aRefListAttr->size(); i++) {
      ObjectPtr anObject = aRefListAttr->object(i);
      if (anObject.get()) {
        bool aFound = findInSelection(anObject, GeomShapePtr(), aGeomSelection,
                                      myWorkshop);
        if (!aFound)
          anIndicesToBeRemoved.insert(i);
      }
    }
    isDone = anIndicesToBeRemoved.size() > 0;
    aRefListAttr->remove(anIndicesToBeRemoved);
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    std::set<AttributePtr> anAttributes;
    QList<ModuleBase_ViewerPrsPtr>::const_iterator
      anIt = theValues.begin(), aLast = theValues.end();
    ObjectPtr anObject;
    GeomShapePtr aShape;
    for (; anIt != aLast; anIt++) {
      ModuleBase_ViewerPrsPtr aPrs = *anIt;
      getGeomSelection(aPrs, anObject, aShape);
      AttributePtr anAttr = myWorkshop->module()->findAttribute(anObject, aShape);
      if (anAttr.get() && anAttributes.find(anAttr) == anAttributes.end())
        anAttributes.insert(anAttr);
    }

    AttributeRefAttrListPtr aRefAttrListAttr = aData->refattrlist(attributeID());
    for (int i = 0; i < aRefAttrListAttr->size(); i++) {
      bool aFound = false;
      if (aRefAttrListAttr->isAttribute(i)) {
        AttributePtr anAttr = aRefAttrListAttr->attribute(i);
        aFound = anAttributes.find(anAttr) != anAttributes.end();
      }
      else {
        aFound = findInSelection(aRefAttrListAttr->object(i), GeomShapePtr(), aGeomSelection,
                                 myWorkshop);
      }
      if (!aFound)
        anIndicesToBeRemoved.insert(i);
    }
    isDone = anIndicesToBeRemoved.size() > 0;
    aRefAttrListAttr->remove(anIndicesToBeRemoved);
  }

  return isDone;
}

std::map<ObjectPtr, std::set<GeomShapePtr> > ModuleBase_WidgetMultiSelector::convertSelection
                                                     (QList<ModuleBase_ViewerPrsPtr>& theValues)
{
  // convert prs list to objects map
  std::map<ObjectPtr, std::set<GeomShapePtr> > aGeomSelection;
  std::set<GeomShapePtr> aShapes;
  QList<ModuleBase_ViewerPrsPtr>::const_iterator anIt = theValues.begin(), aLast = theValues.end();
  ObjectPtr anObject;
  GeomShapePtr aShape;
  GeomShapePtr anEmptyShape(new GeomAPI_Shape());
  for (; anIt != aLast; anIt++) {
    ModuleBase_ViewerPrsPtr aPrs = *anIt;
    getGeomSelection(aPrs, anObject, aShape);
    aShapes.clear();
    if (aGeomSelection.find(anObject) != aGeomSelection.end()) // found
      aShapes = aGeomSelection[anObject];
    // we need to know if there was an empty shape in selection for the object
    if (!aShape.get())
      aShape = anEmptyShape;
    if (aShape.get() && aShapes.find(aShape) == aShapes.end()) // not found
      aShapes.insert(aShape);
    aGeomSelection[anObject] = aShapes;
  }
  return aGeomSelection;
}

bool ModuleBase_WidgetMultiSelector::findInSelection(const ObjectPtr& theObject,
                              GeomShapePtr theShape,
                              const std::map<ObjectPtr, std::set<GeomShapePtr> >& theGeomSelection,
                              ModuleBase_IWorkshop* theWorkshop)
{
  if (!theObject.get())
    return false;
  // issue #2154: we should not remove from list objects hidden in the viewer if selection
  // was done with SHIFT button
  if (theWorkshop->hasSHIFTPressed() && !theObject->isDisplayed())
    return true;

  bool aFound = false;
  GeomShapePtr aShape = theShape;
  if (!aShape.get()) {
    // #2429 (the preselection of a sketch is not taken into account)
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aResult.get())
      aShape = aResult->shape();
  }
  if (theGeomSelection.find(theObject) != theGeomSelection.end()) {// found
    const std::set<GeomShapePtr>& aShapes = theGeomSelection.at(theObject);
    std::set<GeomShapePtr>::const_iterator anIt = aShapes.begin(), aLast = aShapes.end();
    for (; anIt != aLast && !aFound; anIt++) {
      GeomShapePtr aCShape = *anIt;
      if (aCShape.get())
      {
        // treat shape equal to context as null: 2219, keep order of shapes in list
        if (aCShape->isNull()) { // in selection, shape of result is equal to selected shape
          // if so, here we need to check shape of result
          ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
          if (aResult.get())
            aCShape = aResult->shape();
        }
        aFound = aCShape->isSame(aShape);
      }
    }
  }

  // issue #2903: (Possibility to hide faces) - check whether given shape is a hidden sub-shape
  if (!aFound && theShape.get() && theWorkshop->hasSHIFTPressed() && theObject->isDisplayed()) {
    AISObjectPtr anAIS = theWorkshop->findPresentation(theObject);
    if (anAIS.get() != NULL) {
      Handle(AIS_InteractiveObject) anAISIO = anAIS->impl<Handle(AIS_InteractiveObject)>();

      Handle(ModuleBase_ResultPrs) aResultPrs = Handle(ModuleBase_ResultPrs)::DownCast(anAISIO);
      if (!aResultPrs.IsNull() && aResultPrs->isSubShapeHidden(theShape->impl<TopoDS_Shape>()))
        return true;
    }
  }

  return aFound;
}

QList<ActionInfo>
  ModuleBase_WidgetMultiSelector::actionsList(ModuleBase_ActionType theActionType) const
{
  QList<ActionInfo> aList;
  if (myCurrentHistoryIndex > -1) {
    int i = 0;
    QString aTitle("Selection %1 items");
    QString aTit("Selection %1 item");
    QIcon aIcon(":pictures/selection.png");
    int aNb;
    switch (theActionType) {
    case ActionUndo:
      i = 1;
      while (i <= myCurrentHistoryIndex) {
        aNb = mySelectedHistoryValues.at(i).count();
        if (aNb == 1) {
          ActionInfo aInfo(aIcon, aTit.arg(aNb));
          aList.insert(0, aInfo);
        } else {
          ActionInfo aInfo(aIcon, aTitle.arg(aNb));
          aList.insert(0, aInfo);
        }
        i++;
      }
      break;
    case ActionRedo:
      i = mySelectedHistoryValues.length() - 1;
      while (i > myCurrentHistoryIndex) {
        aNb = mySelectedHistoryValues.at(i).count();
        if (aNb == 1) {
          ActionInfo aInfo(aIcon, aTit.arg(mySelectedHistoryValues.at(i).count()));
          aList.insert(0, aInfo);
        } else {
          ActionInfo aInfo(aIcon, aTitle.arg(mySelectedHistoryValues.at(i).count()));
          aList.insert(0, aInfo);
        }
        i--;
      }
      break;
    default: // [to avoid compilation warning]
      break;
    }
  }
  return aList;
}


void ModuleBase_WidgetMultiSelector::onFeatureAccepted()
{
  defaultValues[myFeatureId + attributeID()] = myDefMode;
}

void ModuleBase_WidgetMultiSelector::onListActivated()
{
  //focusTo();
  emitFocusInWidget();
}

void ModuleBase_WidgetMultiSelector::onSameTopology(bool theOn)
{
  AttributePtr anAttribute = myFeature->data()->attribute(attributeID());
  std::string aType = anAttribute->attributeType();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
    aSelectionListAttr->setGeometricalSelection(theOn);
    updateObject(myFeature);
  }
}

void ModuleBase_WidgetMultiSelector::onShowOnly(bool theChecked)
{
  std::list<ResultPtr> aResults = myFeature->results();
   std::list<ResultPtr>::const_iterator aIt;
  if (theChecked) {
    myVisibleObjects = myWorkshop->displayedObjects();
    for (aIt = aResults.cbegin(); aIt != aResults.cend(); aIt++) {
      myVisibleObjects.removeAll(*aIt);
    }
    myWorkshop->module()->disableCustomMode(ModuleBase_IModule::CustomizeArguments);
  }
  else
    myWorkshop->module()->enableCustomModes();

  foreach(ObjectPtr aObj, myVisibleObjects) {
    aObj->setDisplayed(!theChecked);
  }

  if (!theChecked) {
    // Hide and show the group result in order to make it above all objects
    bool aOldState = myWorkshop->enableUpdateViewer(false);
    for (aIt = aResults.cbegin(); aIt != aResults.cend(); aIt++) {
      (*aIt)->setDisplayed(false);
    }
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
    for (aIt = aResults.cbegin(); aIt != aResults.cend(); aIt++) {
      (*aIt)->setDisplayed(true);
    }
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
    myWorkshop->enableUpdateViewer(aOldState);

    myVisibleObjects.clear();
  } else
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  myWorkshop->viewer()->update();
}

bool ModuleBase_WidgetMultiSelector::isModified() const
{
  return myListView->getControl()->count() > 0;
}


void ModuleBase_WidgetMultiSelector::setReadOnly(bool isReadOnly)
{
  ModuleBase_WidgetSelector::setReadOnly(isReadOnly);
  if (myShowOnlyBtn)
    myShowOnlyBtn->hide();
  if (myFiltersWgt) {
    myFiltersWgt->hide();

    AttributeSelectionListPtr aAttrList = feature()->selectionList(attributeID());
    if (aAttrList.get()) {
      FiltersFeaturePtr aFilters = aAttrList->filters();
      if (aFilters.get()) {
        ModuleBase_WidgetSelectionFilter::SelectorFeature = feature();
        ModuleBase_WidgetSelectionFilter::AttributeId = attributeID();

        std::string aXmlCfg, aDescription;
        myWorkshop->module()->getXMLRepresentation(myUseFilters, aXmlCfg, aDescription);

        ModuleBase_WidgetSelectionFilter* aWgt =
          new ModuleBase_WidgetSelectionFilter(this, myWorkshop,
            new Config_WidgetAPI(aDescription), true);
        aWgt->setFeature(aFilters);
        aWgt->restoreValue();
        myMainLayout->addWidget(aWgt);
      }
    }
  }
}
