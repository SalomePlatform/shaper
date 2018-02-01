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

#include <memory>
#include <string>

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
  myIsFirst(true)
{
  std::string aPropertyTypes = theData->getProperty("type_choice");
  QString aTypesStr = aPropertyTypes.c_str();
  myShapeTypes = aTypesStr.split(' ', QString::SkipEmptyParts);
  myIsUseChoice = theData->getBooleanAttribute("use_choice", false);

  QGridLayout* aMainLay = new QGridLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLay);

  QStringList aIconsList = getIconsList(myShapeTypes);
  myTypeCtrl = new ModuleBase_ChoiceCtrl(this, myShapeTypes, aIconsList);
  myTypeCtrl->setLabel(tr("Type"));
  myTypeCtrl->setValue(0);
  aMainLay->addWidget(myTypeCtrl, 0, 0, 1, 2);
  myDefMode = myShapeTypes.first().toStdString();

  // There is no sense to parameterize list of types while we can not parameterize selection mode
  // if the xml definition contains one type, the controls to select a type should not be shown
  if (myShapeTypes.size() <= 1 || !myIsUseChoice) {
    myTypeCtrl->setVisible(false);
  }

  QString aLabelText = translate(theData->getProperty("label"));
  QLabel* aListLabel = new QLabel(aLabelText, this);
  aMainLay->addWidget(aListLabel, 1, 0);
  // if the xml definition contains one type, an information label
  // should be shown near to the latest
  if (myShapeTypes.size() <= 1) {
    QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
    if (!aLabelIcon.isEmpty()) {
      QLabel* aSelectedLabel = new QLabel("", this);
      aSelectedLabel->setPixmap(ModuleBase_IconFactory::loadPixmap(aLabelIcon));
      aMainLay->addWidget(aSelectedLabel, 1, 1);
    }
    aMainLay->setColumnStretch(2, 1);
  }

  QString aToolTip = QString::fromStdString(theData->widgetTooltip());
  QString anObjName = QString::fromStdString(attributeID());
  myListView = new ModuleBase_ListView(this, anObjName, aToolTip);
  connect(myListView->getControl(), SIGNAL(itemSelectionChanged()), SLOT(onListSelection()));
  connect(myListView, SIGNAL(deleteActionClicked()), SLOT(onDeleteItem()));

  aMainLay->addWidget(myListView->getControl(), 2, 0, 1, -1);
  aMainLay->setRowStretch(2, 1);
  //aMainLay->addWidget(new QLabel(this)); //FIXME(sbh)???
  //aMainLay->setRowMinimumHeight(3, 20);
  //this->setLayout(aMainLay);
  connect(myTypeCtrl, SIGNAL(valueChanged(int)), this, SLOT(onSelectionTypeChanged()));

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

  myWorkshop->module()->activateCustomPrs(myFeature,
                            ModuleBase_IModule::CustomizeHighlightedObjects, true);
  clearSelectedHistory();
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::deactivate()
{
  ModuleBase_WidgetSelector::deactivate();

  myWorkshop->module()->deactivateCustomPrs(ModuleBase_IModule::CustomizeHighlightedObjects, true);
  clearSelectedHistory();
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

    std::string aMode = myTypeCtrl->textValue().toStdString();
    if (myTypeCtrl->isVisible() && myIsFirst && (!myDefMode.empty()))
      aMode = myDefMode;

    aSelectionListAttr->setSelectionType(aMode);
    myIsFirst = false;
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
  std::string aType = anAttribute->attributeType();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = myFeature->data()->selectionList(attributeID());
    // Restore shape type
    std::string aSelectionType = aSelectionListAttr->selectionType().c_str();
    if (!aSelectionType.empty()) {
      setCurrentShapeType(ModuleBase_Tools::shapeType(aSelectionType.c_str()));
      myDefMode = aSelectionType;
      myIsFirst = false;
    }
  }
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
  // updateObject - to update/redisplay feature
  // it is commented in order to perfom it outside the method
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
    myWorkshop->module()->customizeObject(myFeature, ModuleBase_IModule::CustomizeArguments,
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
  bool aValid = ModuleBase_WidgetSelector::isValidSelectionCustom(thePrs);
  if (aValid) {
    ResultPtr aResult = myWorkshop->selection()->getResult(thePrs);
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
    myWorkshop->module()->customizeObject(myFeature, ModuleBase_IModule::CustomizeArguments,
                              true); /// hope that something is redisplayed by object updated
  }

  // Restore selection
  myListView->restoreSelection(anIndices);

  appendSelectionInHistory();
  return aDone;
}

//********************************************************************
QList<QWidget*> ModuleBase_WidgetMultiSelector::getControls() const
{
  QList<QWidget*> result;
  result << myListView->getControl();
  return result;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::onSelectionTypeChanged()
{
  // Clear current selection in order to avoid updating of object browser with obsolete indexes
  // which can appear because of results deletetion after changing a type of selection
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
    aSelectionListAttr->setSelectionType(myTypeCtrl->textValue().toStdString());
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
  myWorkshop->module()->customizeObject(myFeature, ModuleBase_IModule::CustomizeArguments,
                            true); /// hope that something is redisplayed by object updated
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
    aShapeTypes.append(ModuleBase_Tools::shapeType(myTypeCtrl->textValue()));
  }
  else {
    foreach (QString aType, myShapeTypes) {
      aShapeTypes.append(ModuleBase_Tools::shapeType(aType));
    }
  }
  return aShapeTypes;
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::setCurrentShapeType(const int theShapeType)
{
  QString aShapeTypeName;

  int idx = 0;
  foreach (QString aShapeTypeName, myShapeTypes) {
    int aRefType = ModuleBase_Tools::shapeType(aShapeTypeName);
    if(aRefType == theShapeType && idx != myTypeCtrl->value()) {
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
  myListView->getControl()->clear();

  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = aData->attribute(attributeID());
  std::string aType = anAttribute->attributeType();
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = aData->selectionList(attributeID());
    for (int i = 0; i < aSelectionListAttr->size(); i++) {
      AttributeSelectionPtr aAttr = aSelectionListAttr->value(i);
      myListView->addItem(aAttr->namingName().c_str(), i);
    }
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr = aData->reflist(attributeID());
    for (int i = 0; i < aRefListAttr->size(); i++) {
      ObjectPtr anObject = aRefListAttr->object(i);
      if (anObject.get()) {
        myListView->addItem(anObject->data()->name().c_str(), i);
      }
    }
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr = aData->refattrlist(attributeID());
    for (int i = 0; i < aRefAttrListAttr->size(); i++) {
      AttributePtr anAttribute = aRefAttrListAttr->attribute(i);
      QString aName;
      if (anAttribute.get()) {
        std::string anAttrName = generateName(anAttribute, myWorkshop);
        aName = QString::fromStdString(anAttrName);
      }
      else {
        ObjectPtr anObject = aRefAttrListAttr->object(i);
        if (anObject.get()) {
          aName = anObject->data()->name().c_str();
        }
      }
      myListView->addItem(aName, i);
    }
  }

  // We have to call repaint because sometimes the List control is not updated
  myListView->getControl()->repaint();
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
  myWorkshop->module()->customizeObject(myFeature, ModuleBase_IModule::CustomizeHighlightedObjects,
                                        true);
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
      ResultPtr anObject = anAttr->context();
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
      AttributePtr anAttribute = aRefAttrListAttr->attribute(i);
      if (anAttribute.get()) {
        GeomShapePtr aGeomShape = ModuleBase_Tools::getShape(anAttribute, myWorkshop);
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
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    // iteration through data model to find not selected elements to remove them
    AttributeSelectionListPtr aSelectionListAttr = aData->selectionList(attributeID());
    for (int i = 0; i < aSelectionListAttr->size(); i++) {
      AttributeSelectionPtr anAttr = aSelectionListAttr->value(i);
      bool aFound = findInSelection(anAttr->context(), anAttr->value(), aGeomSelection,
                                    myWorkshop);
      if (!aFound)
        anIndicesToBeRemoved.insert(i);
    }
    isDone = anIndicesToBeRemoved.size() > 0;
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
        AttributePtr anAttribute = aRefAttrListAttr->attribute(i);
        aFound = anAttributes.find(anAttribute) != anAttributes.end();
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
    }
  }
  return aList;
}


void ModuleBase_WidgetMultiSelector::onFeatureAccepted()
{
  defaultValues[myFeatureId + attributeID()] = myDefMode;
}
