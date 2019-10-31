// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "ModuleBase_WidgetSelectionFilter.h"
#include "ModuleBase_Tools.h"
#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_ISelectionActivate.h"
#include "ModuleBase_IModule.h"
#include "ModuleBase_IViewer.h"
#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_PageWidget.h"
#include "ModuleBase_WidgetMultiSelector.h"
#include "ModuleBase_ResultPrs.h"
#include "ModuleBase_WidgetFactory.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_FiltersFactory.h>
#include <ModelAPI_ResultBody.h>
#include <GeomAPI_ShapeExplorer.h>

#include <Events_Loop.h>
#include <Config_ValidatorReader.h>

#include <AIS_InteractiveContext.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>

#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QDialog>
#include <QToolButton>
#include <QCheckBox>
#include <QDir>

static FeaturePtr SelectorFeature;
static std::string AttributeId;


GeomAPI_Shape::ShapeType selectionType(const QString& theType)
{
  QString aType = theType.toUpper();
  if ((aType == "VERTEX") || (aType == "VERTICES"))
    return GeomAPI_Shape::VERTEX;
  else if ((aType == "EDGE") || (aType == "EDGES"))
    return GeomAPI_Shape::EDGE;
  else if ((aType == "WIRE") || (aType == "WIRES"))
    return GeomAPI_Shape::WIRE;
  else if ((aType == "FACE") || (aType == "FACES"))
    return GeomAPI_Shape::FACE;
  else if ((aType == "SHELL") || (aType == "SHELLS"))
    return GeomAPI_Shape::SHELL;
  else if ((aType == "SOLID") || (aType == "SOLIDS"))
    return GeomAPI_Shape::SOLID;
  else if ((aType == "COMPSOLID") || (aType == "COMPSOLIDS"))
    return GeomAPI_Shape::COMPSOLID;
  else if ((aType == "COMPOUND") || (aType == "COMPOUNDS"))
    return GeomAPI_Shape::COMPOUND;
  else
    return GeomAPI_Shape::SHAPE;
}


ModuleBase_FilterStarter::ModuleBase_FilterStarter(const std::string& theFeature,
  QWidget* theParent, ModuleBase_IWorkshop* theWorkshop)
  : QWidget(theParent),
  myFeatureName(theFeature),
  myWorkshop(theWorkshop)
{
  QHBoxLayout* aMainLayout = new QHBoxLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLayout);

  aMainLayout->addStretch(1);
  QPushButton* aLaunchBtn = new QPushButton(tr("Selection by filters"), this);
  connect(aLaunchBtn, SIGNAL(clicked()), SLOT(onFiltersLaunch()));
  aMainLayout->addWidget(aLaunchBtn);
}

void ModuleBase_FilterStarter::onFiltersLaunch()
{
  static QString aHelpFileName = QString("FiltersPlugin") + QDir::separator() +
    QString("FiltersPlugin.html");

  ModuleBase_Operation* aParentOp = myWorkshop->currentOperation();
  ModuleBase_OperationFeature* aFeatureOp = dynamic_cast<ModuleBase_OperationFeature*>(aParentOp);
  if (aFeatureOp)
    // Open transaction on filters operation finish
    aFeatureOp->openTransactionOnResume();

  QWidget* aParent = parentWidget();
  ModuleBase_WidgetMultiSelector* aSelector =
    dynamic_cast<ModuleBase_WidgetMultiSelector*>(aParent);
  while (!aSelector) {
    aParent = aParent->parentWidget();
    aSelector = dynamic_cast<ModuleBase_WidgetMultiSelector*>(aParent);
  }
  if (!aSelector)
    return;
  SelectorFeature = aSelector->feature();
  AttributeId = aSelector->attributeID();

  // Launch Filters operation
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
    (myWorkshop->module()->createOperation(myFeatureName));

  AttributeSelectionListPtr aAttrList = SelectorFeature->selectionList(AttributeId);
  FiltersFeaturePtr aFilters = aAttrList->filters();
  if (aFilters.get())
    aFOperation->setFeature(aFilters);
  aFOperation->setHelpFileName(aHelpFileName);
  myWorkshop->processLaunchOperation(aFOperation);
}

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
ModuleBase_FilterItem::ModuleBase_FilterItem(
  const std::string& theFilter, ModuleBase_WidgetSelectionFilter* theParent)
  : QWidget(theParent->filtersWidget()), myFilterID(theFilter),
    mySelection(std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(theParent->feature()))
{
  std::string aXmlString =
      ModelAPI_Session::get()->filters()->filter(theFilter)->xmlRepresentation();
  if (aXmlString.length() == 0)
    addItemRow(this);
  else {
    ModuleBase_WidgetFactory aFactory(aXmlString, theParent->workshop());
    Config_ValidatorReader aValidatorReader(aXmlString, true);
    aValidatorReader.setFeatureId(mySelection->getKind());
    aValidatorReader.readAll();

    QVBoxLayout* aLayout = new QVBoxLayout(this);
    ModuleBase_Tools::zeroMargins(aLayout);

    QWidget* aItemRow = new QWidget(this);
    addItemRow(aItemRow);
    aLayout->addWidget(aItemRow);

    ModuleBase_PageWidget* aParamsWgt = new ModuleBase_PageWidget(this);
    aParamsWgt->setFrameStyle(QFrame::Box | QFrame::Raised);
    aFactory.createWidget(aParamsWgt);
    ModuleBase_Tools::zeroMargins(aParamsWgt->layout());
    myWidgets = aFactory.getModelWidgets();
    foreach(ModuleBase_ModelWidget* aWidget, myWidgets) {
      aWidget->setFeature(theParent->feature());
      connect(aWidget, SIGNAL(focusInWidget(ModuleBase_ModelWidget*)),
        theParent, SIGNAL(focusInWidget(ModuleBase_ModelWidget*)));
      connect(aWidget, SIGNAL(focusOutWidget(ModuleBase_ModelWidget*)),
        theParent, SIGNAL(focusOutWidget(ModuleBase_ModelWidget*)));
      connect(aWidget, SIGNAL(objectUpdated()), theParent, SLOT(onObjectUpdated()));
    }
    aLayout->addWidget(aParamsWgt);
  }
}

void ModuleBase_FilterItem::addItemRow(QWidget* theParent)
{
  QHBoxLayout* aLayout = new QHBoxLayout(theParent);
  ModuleBase_Tools::zeroMargins(aLayout);

  // Reverse filter button
  myRevBtn = new QToolButton(theParent);
  myRevBtn->setCheckable(true);
  bool isReversed = mySelection->isReversed(myFilterID);
  myRevBtn->setChecked(isReversed);
  myRevBtn->setAutoRaise(true);
  if (isReversed)
    myRevBtn->setIcon(QIcon(":pictures/reverce.png"));
  else
    myRevBtn->setIcon(QIcon(":pictures/add.png"));
  myRevBtn->setToolTip(tr("Reverse the filter"));
  connect(myRevBtn, SIGNAL(toggled(bool)), SLOT(onReverse(bool)));
  aLayout->addWidget(myRevBtn);

  const std::string& aFilterName = ModelAPI_Session::get()->filters()->filter(myFilterID)->name();
  aLayout->addWidget(new QLabel(aFilterName.c_str(), theParent), 1);

  QToolButton* aDelBtn = new QToolButton(theParent);
  aDelBtn->setIcon(QIcon(":pictures/delete.png"));
  aDelBtn->setAutoRaise(true);
  aDelBtn->setToolTip(tr("Delete the filter"));
  connect(aDelBtn, SIGNAL(clicked(bool)), SLOT(onDelete()));
  aLayout->addWidget(aDelBtn);
}

void ModuleBase_FilterItem::onReverse(bool theCheck)
{
  mySelection->setReversed(myFilterID, theCheck);
  if (theCheck)
    myRevBtn->setIcon(QIcon(":pictures/reverce.png"));
  else
    myRevBtn->setIcon(QIcon(":pictures/add.png"));
  emit reversedItem(this);
}

void ModuleBase_FilterItem::onDelete()
{
  emit deleteItem(this);
}


//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
ModuleBase_WidgetSelectionFilter::ModuleBase_WidgetSelectionFilter(QWidget* theParent,
  ModuleBase_IWorkshop* theWorkshop, const Config_WidgetAPI* theData)
  : ModuleBase_ModelWidget(theParent, theData),
  myWorkshop(theWorkshop),
  mySelectorFeature(SelectorFeature),
  mySelectorAttribute(AttributeId)
{
  // Clear Old selection
  AttributeSelectionListPtr aAttrList = mySelectorFeature->selectionList(mySelectorAttribute);
  mySelectionType = selectionType(aAttrList->selectionType().c_str());
  aAttrList->clear();

  // Define widgets
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLayout);

  QGroupBox* aFiltersGroup = new QGroupBox(tr("Filters"), this);
  QVBoxLayout* aGroupLayout = new QVBoxLayout(aFiltersGroup);
  aGroupLayout->setContentsMargins(0, 0, 0, 0);
  aGroupLayout->setSpacing(0);

  myFiltersWgt = new QWidget();
  myFiltersLayout = new QVBoxLayout(myFiltersWgt);
  myFiltersLayout->setContentsMargins(0, 0, 0, 0);
  aGroupLayout->addWidget(myFiltersWgt);

  myFiltersCombo = new QComboBox(aFiltersGroup);
  myFiltersCombo->addItem(tr("Add new filter..."));
  SessionPtr aSession = ModelAPI_Session::get();
  std::list<FilterPtr> allFilters =
    aSession->filters()->filters((GeomAPI_Shape::ShapeType) mySelectionType);
  QStringList aItems;
  std::list<FilterPtr>::const_iterator aIt;
  for (aIt = allFilters.cbegin(); aIt != allFilters.cend(); aIt++) {
    aItems.push_back((*aIt)->name().c_str());
    myFilters.push_back(aSession->filters()->id(*aIt));
  }
  myFiltersCombo->addItems(aItems);
  connect(myFiltersCombo, SIGNAL(currentIndexChanged(int)), SLOT(onAddFilter(int)));

  aGroupLayout->addWidget(myFiltersCombo);
  aMainLayout->addWidget(aFiltersGroup);

  // Select Button
  QWidget* aBtnWgt = new QWidget(this);
  QHBoxLayout* aBtnLayout = new QHBoxLayout(aBtnWgt);
  ModuleBase_Tools::adjustMargins(aBtnLayout);

  aBtnLayout->addStretch(1);

  mySelectBtn = new QPushButton(tr("Select"), aBtnWgt);
  connect(mySelectBtn, SIGNAL(clicked()), SLOT(onSelect()));
  aBtnLayout->addWidget(mySelectBtn);

  aMainLayout->addWidget(aBtnWgt);

  // Label widgets
  QWidget* aLblWgt = new QWidget(this);
  QHBoxLayout* aLblLayout = new QHBoxLayout(aLblWgt);
  ModuleBase_Tools::zeroMargins(aLblLayout);

  aLblLayout->addWidget(new QLabel(tr("Number of selected objects:"), aLblWgt));

  myNbLbl = new QLabel("0", aLblWgt);
  aLblLayout->addWidget(myNbLbl);

  // Show only button
  myShowBtn = new QCheckBox(tr("Show only"), this);
  connect(myShowBtn, SIGNAL(toggled(bool)), SLOT(onShowOnly(bool)));
  aLblLayout->addWidget(myShowBtn);

  aMainLayout->addWidget(aLblWgt);

  aMainLayout->addStretch(1);

  updateSelectBtn();
}

ModuleBase_WidgetSelectionFilter::~ModuleBase_WidgetSelectionFilter()
{
  myValues.clear();
  if (!myPreview.IsNull()) {
    Handle(AIS_InteractiveContext) aCtx = myWorkshop->viewer()->AISContext();
    aCtx->Remove(myPreview, false);
    myPreview.Nullify();
    if (myListIO.Size() > 0) {
      Handle(AIS_InteractiveContext) aCtx = myWorkshop->viewer()->AISContext();
      AIS_ListOfInteractive::const_iterator aIt;
      Handle(AIS_Shape) aShapeIO;
      for (aIt = myListIO.cbegin(); aIt != myListIO.cend(); aIt++) {
        aShapeIO = Handle(AIS_Shape)::DownCast(*aIt);
        if (!aShapeIO.IsNull()) {
          aCtx->Display(aShapeIO, false);
          std::shared_ptr<GeomAPI_AISObject> anAISObj = AISObjectPtr(new GeomAPI_AISObject());
          anAISObj->setImpl(new Handle(AIS_InteractiveObject)(aShapeIO));
          myWorkshop->applyCurrentSelectionModes(anAISObj);
        }
      }
      myListIO.Clear();
      myShowBtn->setChecked(false);
    }
    myWorkshop->viewer()->update();
  }
  SelectorFeature = FeaturePtr();
  AttributeId = "";
}

void ModuleBase_WidgetSelectionFilter::onAddFilter(int theIndex)
{
  if (theIndex == 0)
    return;

  std::list<std::string>::iterator aIt;
  int i;
  std::string aFilter;
  for (aIt = myFilters.begin(), i = 0; aIt != myFilters.cend(); i++, aIt++) {
    if (i == (theIndex - 1)) {
      aFilter = (*aIt);
      break;
    }
  }
  ModuleBase_FilterItem* aItem = onAddFilter(aFilter);
  FiltersFeaturePtr aFiltersFeature =
    std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(myFeature);
  aFiltersFeature->addFilter(aFilter);

  myFiltersCombo->setCurrentIndex(0);
  myFiltersCombo->removeItem(theIndex);
  updateObject(myFeature);

  if (aItem && (aItem->widgets().size() > 0))
    aItem->widgets().first()->emitFocusInWidget();
  else
    emitFocusInWidget();
}

ModuleBase_FilterItem* ModuleBase_WidgetSelectionFilter::onAddFilter(const std::string& theFilter)
{
  if (theFilter.length() == 0)
    return 0;
  std::list<std::string>::const_iterator aIt;
  for (aIt = myUseFilters.cbegin(); aIt != myUseFilters.cend(); aIt++) {
    if (theFilter == (*aIt))
      return 0;
  }
  myFilters.remove(theFilter);
  myUseFilters.push_back(theFilter);
  ModuleBase_FilterItem* aItem = new ModuleBase_FilterItem(theFilter, this);
  connect(aItem, SIGNAL(deleteItem(ModuleBase_FilterItem*)),
    SLOT(onDeleteItem(ModuleBase_FilterItem*)));
  connect(aItem, SIGNAL(reversedItem(ModuleBase_FilterItem*)),
    SLOT(onReverseItem(ModuleBase_FilterItem*)));
  myFiltersLayout->addWidget(aItem);

  updateSelectBtn();
  clearCurrentSelection(true);
  updateNumberSelected();
  return aItem;
}

void ModuleBase_WidgetSelectionFilter::onDeleteItem(ModuleBase_FilterItem* theItem)
{
  std::string aFilter = theItem->filter();
  QList<ModuleBase_ModelWidget*> aWidgets = theItem->widgets();
  foreach(ModuleBase_ModelWidget* aWgt, aWidgets) {
    aWgt->deactivate();
  }
  myFiltersLayout->removeWidget(theItem);
  theItem->deleteLater();

  myUseFilters.remove(aFilter);
  myFilters.push_back(aFilter);
  myFiltersCombo->addItem(ModelAPI_Session::get()->filters()->filter(aFilter)->name().c_str());

  FiltersFeaturePtr aFiltersFeature =
    std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(myFeature);
  aFiltersFeature->removeFilter(aFilter);

  updateSelectBtn();
  clearCurrentSelection(true);
  updateNumberSelected();

  myWorkshop->deactivateCurrentSelector();
  myWorkshop->selectionActivate()->updateSelectionModes();
  myWorkshop->selectionActivate()->updateSelectionFilters();
  redisplayFeature();
  emitFocusInWidget();
  updateObject(myFeature);
}


void ModuleBase_WidgetSelectionFilter::redisplayFeature()
{
  static Events_ID aDispEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  ModelAPI_EventCreator::get()->sendUpdated(myFeature, aDispEvent);
  Events_Loop::loop()->flush(aDispEvent);
}

void ModuleBase_WidgetSelectionFilter::onReverseItem(ModuleBase_FilterItem* theItem)
{
  updateSelectBtn();
  clearCurrentSelection(true);
  updateNumberSelected();
}

void ModuleBase_WidgetSelectionFilter::onSelect()
{
  if (myUseFilters.size() == 0)
    return;
  Handle(AIS_InteractiveContext) aCtx = myWorkshop->viewer()->AISContext();
  if (aCtx.IsNull())
    return;

  clearCurrentSelection();

  BRep_Builder aBuilder;
  TopoDS_Compound aComp;
  aBuilder.MakeCompound(aComp);

  DocumentPtr aDoc = myFeature->document();
  int aNb = aDoc->size(ModelAPI_ResultBody::group());
  ObjectPtr aObj;
  ResultBodyPtr aBody;
  GeomShapePtr aShape;
  for (int i = 0; i < aNb; i++) {
    aObj = aDoc->object(ModelAPI_ResultBody::group(), i);
    aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aObj);
    GeomShapePtr aShape = aBody->shape();
    std::list<GeomShapePtr> aSubShapes =
      aShape->subShapes((GeomAPI_Shape::ShapeType)mySelectionType);
    TopTools_MapOfShape alreadyThere;
    std::list<GeomShapePtr>::const_iterator aShapesIt;
    for (aShapesIt = aSubShapes.cbegin(); aShapesIt != aSubShapes.cend(); aShapesIt++) {
      GeomShapePtr aShape = (*aShapesIt);
      TopoDS_Shape aTShape = aShape->impl<TopoDS_Shape>();
      if (!alreadyThere.Add(aTShape))
        continue;
      static SessionPtr aSession = ModelAPI_Session::get();
      bool isValid = aSession->filters()->isValid(myFeature, aBody, aShape);
      if (isValid) {
        aBuilder.Add(aComp, aTShape);
        ModuleBase_ViewerPrsPtr aValue(new ModuleBase_ViewerPrs(aObj, aShape));
        myValues.append(aValue);
      }
    }
  }

  if (myValues.size() > 0)
    updatePreview(aComp);
  updateNumberSelected();
  updateObject(myFeature);
  onShowOnly(myShowBtn->isChecked());
}

void ModuleBase_WidgetSelectionFilter::updatePreview(const TopoDS_Shape& theShape)
{
  Handle(AIS_InteractiveContext) aCtx = myWorkshop->viewer()->AISContext();
  if (aCtx.IsNull())
    return;

  if (myPreview.IsNull()) {
    myPreview = new AIS_Shape(theShape);
    myPreview->SetDisplayMode(AIS_Shaded);
    myPreview->SetColor(Quantity_NOC_BLUE1);
    Handle(Prs3d_Drawer) aDrawer = myPreview->Attributes();
    if (aDrawer->HasOwnPointAspect()) {
      aDrawer->PointAspect()->SetTypeOfMarker(Aspect_TOM_O_STAR);
      aDrawer->PointAspect()->SetColor(Quantity_NOC_BLUE1);
      aDrawer->PointAspect()->SetScale(2.);
    }
    else
      aDrawer->SetPointAspect(new Prs3d_PointAspect(Aspect_TOM_O_STAR, Quantity_NOC_BLUE1, 2.));
    myPreview->SetTransparency();
    aCtx->Display(myPreview, true);
    aCtx->Deactivate(myPreview);
  }
  else {
    myPreview->Set(theShape);
    aCtx->Redisplay(myPreview, true);
  }
}


void ModuleBase_WidgetSelectionFilter::onShowOnly(bool theShow)
{
  if (myPreview.IsNull())
    return;
  Handle(AIS_InteractiveContext) aCtx = myWorkshop->viewer()->AISContext();

  if (theShow) {
    AIS_ListOfInteractive aList;
    aCtx->DisplayedObjects(AIS_KOI_Shape, -1, aList);
    aList.Remove(myPreview);
    if (aList.Size() > 0)
      myListIO = aList;
  }
  AIS_ListOfInteractive::const_iterator aIt;
  Handle(AIS_Shape) aShapeIO;
  for (aIt = myListIO.cbegin(); aIt != myListIO.cend(); aIt++) {
    aShapeIO = Handle(AIS_Shape)::DownCast(*aIt);
    if (!aShapeIO.IsNull()) {
      if (theShow)
        aCtx->Erase(aShapeIO, false);
      else {
        aCtx->Display(aShapeIO, false);
        std::shared_ptr<GeomAPI_AISObject> anAISObj = AISObjectPtr(new GeomAPI_AISObject());
        anAISObj->setImpl(new Handle(AIS_InteractiveObject)(aShapeIO));
        myWorkshop->applyCurrentSelectionModes(anAISObj);
      }
    }
  }
  myWorkshop->viewer()->update();
}

void ModuleBase_WidgetSelectionFilter::updateSelectBtn()
{
  mySelectBtn->setEnabled(myUseFilters.size() > 0);
}

void ModuleBase_WidgetSelectionFilter::updateNumberSelected()
{
  int aNb = myValues.size();
  myNbLbl->setText(QString::number(aNb));
  //QString aErr = () ? tr("Selection is empty") : "";
  if (aNb == 0)
    myFeature->setError(tr("Selection is empty").toStdString(), false, false);
  else {
    myFeature->setError("", false, false);
    myFeature->data()->execState(ModelAPI_StateDone);
  }
}

QList<QWidget*> ModuleBase_WidgetSelectionFilter::getControls() const
{
  QList<QWidget*> aWidgets;
  aWidgets.append(myFiltersCombo);
  return aWidgets;
}

void ModuleBase_WidgetSelectionFilter::clearCurrentSelection(bool toUpdate)
{
  myValues.clear();
  if (!myPreview.IsNull()) {
    Handle(AIS_InteractiveContext) aCtx = myWorkshop->viewer()->AISContext();
    aCtx->Remove(myPreview, toUpdate);
    myPreview.Nullify();
  }
}

void replaceSubShapesByResult(QList<ModuleBase_ViewerPrsPtr>& theResults, int theShapeType)
{
  QMap<ObjectPtr, QList<GeomShapePtr>> myResShapes;
  // Sort sub-shapes by result
  foreach (ModuleBase_ViewerPrsPtr aPrs, theResults) {
    if (myResShapes.contains(aPrs->object()))
      myResShapes[aPrs->object()].append(aPrs->shape());
    else {
      QList<GeomShapePtr> aShapes;
      aShapes << aPrs->shape();
      myResShapes[aPrs->object()] = aShapes;
    }
  }
  // Find Results to replace by whole result
  QList<GeomShapePtr> aShapes;
  QList<ObjectPtr> aToReplace;
  std::list<GeomShapePtr> aSubShapes;
  foreach(ObjectPtr aObj, myResShapes.keys()) {
    aShapes = myResShapes[aObj];
    ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
    TopTools_MapOfShape aShapesMap;
    if (aRes.get()) {
      GeomShapePtr aSubShape = aRes->shape();
      const TopoDS_Shape& aShape = aSubShape->impl<TopoDS_Shape>();
      for (TopExp_Explorer anExp(aShape, (TopAbs_ShapeEnum)theShapeType);
        anExp.More(); anExp.Next()) {
        aShapesMap.Add(anExp.Current());
      }
    }
    if (aShapes.count() == aShapesMap.Size())
      aToReplace.append(aObj);
  }
  // Replace the found results
  QList<ModuleBase_ViewerPrsPtr>::iterator aIt;
  foreach(ObjectPtr aObj, aToReplace) {
    for (aIt = theResults.begin(); aIt != theResults.end(); aIt++) {
      if ((*aIt)->object() == aObj) {
        theResults.removeAll(*aIt);
        aIt--;
      }
    }
    ModuleBase_ViewerPrsPtr aValue(new ModuleBase_ViewerPrs(aObj));
    theResults.append(aValue);
  }
}

void ModuleBase_WidgetSelectionFilter::onFeatureAccepted()
{
  AttributePtr aAttr = mySelectorFeature->attribute(mySelectorAttribute);
  AttributeSelectionListPtr aSelListAttr =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aAttr);
  aSelListAttr->clear();
  if (aSelListAttr->isWholeResultAllowed())
    replaceSubShapesByResult(myValues, selectionType(aSelListAttr->selectionType().c_str()));
  foreach(ModuleBase_ViewerPrsPtr aPrs, myValues) {
    aSelListAttr->append(aPrs->object(), aPrs->shape());
  }
}

bool ModuleBase_WidgetSelectionFilter::storeValueCustom()
{
  ModuleBase_ModelWidget* aActive = myWorkshop->propertyPanel()->activeWidget();
  if (aActive)
    return aActive->storeValue();
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetSelectionFilter::restoreValueCustom()
{
  ModelAPI_FiltersFactory* aFactory = ModelAPI_Session::get()->filters();
  FiltersFeaturePtr aFiltersFeature = std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(myFeature);
  std::list<std::string> aFilters = aFiltersFeature->filters();
  std::list<std::string>::const_iterator aIt;
  for (aIt = aFilters.cbegin(); aIt != aFilters.cend(); aIt++) {
    std::string aStr = (*aIt);
    onAddFilter(aStr);
    FilterPtr aFilterObj = aFactory->filter(aStr);
    int aId = myFiltersCombo->findText(aFilterObj->name().c_str());
    if (aId != -1)
      myFiltersCombo->removeItem(aId);
  }
  // Init filters member of the parent attribute
  AttributeSelectionListPtr aAttrList = mySelectorFeature->selectionList(mySelectorAttribute);
  if (aAttrList->filters() != aFiltersFeature) {
    aAttrList->setFilters(aFiltersFeature);
  }

  QList<QWidget*> aWidgets;
  QList<ModuleBase_FilterItem*> aItems = myFiltersLayout->findChildren<ModuleBase_FilterItem*>();
  foreach(ModuleBase_FilterItem* aItem, aItems) {
    QList<ModuleBase_ModelWidget*> aSubList = aItem->widgets();
    foreach(ModuleBase_ModelWidget* aWgt, aSubList) {
      aWgt->restoreValue();
    }
  }
  return true;
}

QString ModuleBase_WidgetSelectionFilter::getError(const bool theValueStateChecked) const
{
  QString aErrorMsg = ModuleBase_ModelWidget::getError(theValueStateChecked);
  if (aErrorMsg.isEmpty()) {
    if (myValues.size() == 0)
      aErrorMsg = tr("Selection is empty");
  }
  return aErrorMsg;
}

void ModuleBase_WidgetSelectionFilter::onObjectUpdated()
{
  myShowBtn->setChecked(false);
  clearCurrentSelection(true);
  updateNumberSelected();

  QList<ModuleBase_FilterItem*> aItemsList = myFiltersWgt->findChildren<ModuleBase_FilterItem*>();
  foreach(ModuleBase_FilterItem* aItem, aItemsList) {
    QList<ModuleBase_ModelWidget*> aWidgetsList = aItem->widgets();
    foreach(ModuleBase_ModelWidget* aWidget, aWidgetsList) {
      if (!aWidget->feature().get())
        aWidget->setFeature(myFeature);
      aWidget->restoreValue();
    }
  }
  updateObject(myFeature);

  // Redisplay the feature on order to Customize presentations from filters with selectors
  static Events_ID EVENT_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  ModelAPI_EventCreator::get()->sendUpdated(myFeature, EVENT_DISP);
  Events_Loop::loop()->flush(EVENT_DISP);
}
