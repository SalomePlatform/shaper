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
#include <GeomAPI_ShapeExplorer.h>

#include <Events_Loop.h>

#include <AIS_InteractiveContext.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>

#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QDialog>
#include <QToolButton>
#include <QCheckBox>

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

  myFilterLbl = new QLabel(this);
  myFilterLbl->setPixmap(QPixmap(":pictures/filter.png"));
  aMainLayout->addWidget(myFilterLbl);

  myModifyLbl = new QLabel(this);
  myModifyLbl->setPixmap(QPixmap(":pictures/plus_minus.png"));
  aMainLayout->addWidget(myModifyLbl);
  aMainLayout->addStretch(1);

  myFilterLbl->hide();
  myModifyLbl->hide();
}

void ModuleBase_FilterStarter::onFiltersLaunch()
{
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
  std::string aXmlString = ModelAPI_Session::get()->filters()->filter(theFilter)->xmlRepresentation();
  if (aXmlString.length() == 0)
    addItemRow(this);
  else {
    ModuleBase_WidgetFactory aFactory(aXmlString, theParent->workshop());
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
  myRevBtn->setChecked(false);
  myRevBtn->setAutoRaise(true);
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

QList<QWidget*>  ModuleBase_FilterItem::getControls() const
{
  QList<QWidget*> aWidgetsList;
  foreach(ModuleBase_ModelWidget* aWgt, myWidgets) {
    QList<QWidget*> aSubList = aWgt->getControls();
    foreach(QWidget* aSub, aSubList) {
      aWidgetsList.append(aSub);
    }
  }
  return aWidgetsList;
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
  AttributePtr aAttr = SelectorFeature->attribute(AttributeId);
  AttributeSelectionListPtr aSelListAttr =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aAttr);
  mySelectionType = selectionType(aSelListAttr->selectionType().c_str());
  aSelListAttr->clear();

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
    if (myShowBtn->isChecked()) {
      AIS_ListOfInteractive::const_iterator aIt;
      Handle(AIS_Shape) aShapeIO;
      for (aIt = myListIO.cbegin(); aIt != myListIO.cend(); aIt++) {
        aShapeIO = Handle(AIS_Shape)::DownCast(*aIt);
        if (!aShapeIO.IsNull()) {
          aCtx->Display(aShapeIO, false);
        }
      }
    }
    aCtx->UpdateCurrentViewer();
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
      myFilters.erase(aIt);
      break;
    }
  }
  if (!aFilter.empty()) {
    myUseFilters.push_back(aFilter);
    ModuleBase_FilterItem* aItem = new ModuleBase_FilterItem(aFilter, this);
    connect(aItem, SIGNAL(deleteItem(ModuleBase_FilterItem*)),
      SLOT(onDeleteItem(ModuleBase_FilterItem*)));
    connect(aItem, SIGNAL(reversedItem(ModuleBase_FilterItem*)),
      SLOT(onReverseItem(ModuleBase_FilterItem*)));
    myFiltersLayout->addWidget(aItem);

    FiltersFeaturePtr aFiltersFeature =
      std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(myFeature);
    aFiltersFeature->addFilter(aFilter);
  }
  updateSelectBtn();
  clearCurrentSelection(true);
  updateNumberSelected();
  myFiltersCombo->setCurrentIndex(0);
  myFiltersCombo->removeItem(theIndex);

  enableFocusProcessing();
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

  enableFocusProcessing();
  myWorkshop->deactivateCurrentSelector();
  myWorkshop->selectionActivate()->updateSelectionModes();
  myWorkshop->selectionActivate()->updateSelectionFilters();
  redisplayFeature();
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

  if (!myShowBtn->isChecked()) {
    myListIO.Clear();
    aCtx->DisplayedObjects(AIS_KOI_Shape, -1, myListIO);
    if (!myPreview.IsNull())
      myListIO.Remove(myPreview);
  }
  AIS_ListOfInteractive::const_iterator aIt;
  Handle(ModuleBase_ResultPrs) aShapeIO;
  for (aIt = myListIO.cbegin(); aIt != myListIO.cend(); aIt++) {
    aShapeIO = Handle(ModuleBase_ResultPrs)::DownCast(*aIt);
    if (!aShapeIO.IsNull()) {
      GeomShapePtr aShape(new GeomAPI_Shape);
      aShape->setImpl(new TopoDS_Shape(aShapeIO->Shape()));
      std::list<GeomShapePtr> aSubShapes =
        aShape->subShapes((GeomAPI_Shape::ShapeType)mySelectionType);
      std::list<GeomShapePtr>::const_iterator aShapesIt;
      for (aShapesIt = aSubShapes.cbegin(); aShapesIt != aSubShapes.cend(); aShapesIt++) {
        GeomShapePtr aShape = (*aShapesIt);
        SessionPtr aSession = ModelAPI_Session::get();
        bool isValid = aSession->filters()->isValid(myFeature, aShape);
        if (isValid) {
          TopoDS_Shape aTShape = aShape->impl<TopoDS_Shape>();
          Handle(StdSelect_BRepOwner) aOwner = new StdSelect_BRepOwner(aTShape, aShapeIO, true);
          aBuilder.Add(aComp, aTShape);

          ModuleBase_ViewerPrsPtr aValue(new ModuleBase_ViewerPrs(aShapeIO->getResult(), aShape, aOwner));
          myValues.append(aValue);
        }
      }
    }
  }
  if (myValues.size() > 0)
    updatePreview(aComp);
  updateNumberSelected();
}

void ModuleBase_WidgetSelectionFilter::updatePreview(const TopoDS_Shape& theShape)
{
  Handle(AIS_InteractiveContext) aCtx = myWorkshop->viewer()->AISContext();
  if (aCtx.IsNull())
    return;

  if (myPreview.IsNull()) {
    myPreview = new AIS_Shape(theShape);
    //myPreview->SetDisplayMode(myShowBtn->isChecked()? AIS_Shaded : AIS_WireFrame);
    myPreview->SetDisplayMode(AIS_Shaded);
    myPreview->SetColor(Quantity_NOC_BLUE1);
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
    myListIO.Clear();
    aCtx->DisplayedObjects(AIS_KOI_Shape, -1, myListIO);
    myListIO.Remove(myPreview);
  }
  AIS_ListOfInteractive::const_iterator aIt;
  Handle(AIS_Shape) aShapeIO;
  for (aIt = myListIO.cbegin(); aIt != myListIO.cend(); aIt++) {
    aShapeIO = Handle(AIS_Shape)::DownCast(*aIt);
    if (!aShapeIO.IsNull()) {
      if (theShow)
        aCtx->Erase(aShapeIO, false);
      else
        aCtx->Display(aShapeIO, false);
    }
  }
  aCtx->UpdateCurrentViewer();
}

void ModuleBase_WidgetSelectionFilter::updateSelectBtn()
{
  mySelectBtn->setEnabled(myUseFilters.size() > 0);
}

void ModuleBase_WidgetSelectionFilter::updateNumberSelected()
{
  myNbLbl->setText(QString::number(myValues.size()));
}
QList<QWidget*> ModuleBase_WidgetSelectionFilter::getControls() const
{
  QList<QWidget*> aWidgets;
  QList<ModuleBase_FilterItem*> aItems = myFiltersWgt->findChildren<ModuleBase_FilterItem*>();
  foreach(ModuleBase_FilterItem* aItem, aItems) {
    QList<QWidget*> aSubList = aItem->getControls();
    foreach(QWidget* aWgt, aSubList) {
      aWidgets.append(aWgt);
    }
  }
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

void ModuleBase_WidgetSelectionFilter::onFeatureAccepted()
{
  AttributePtr aAttr = mySelectorFeature->attribute(mySelectorAttribute);
  AttributeSelectionListPtr aSelListAttr =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aAttr);
  aSelListAttr->clear();
  foreach(ModuleBase_ViewerPrsPtr aPrs, myValues) {
    aSelListAttr->append(aPrs->object(), aPrs->shape());
  }
}

bool ModuleBase_WidgetSelectionFilter::storeValueCustom()
{
  return true;
}

bool ModuleBase_WidgetSelectionFilter::restoreValueCustom()
{
  ModuleBase_ModelWidget* aActive = myWorkshop->propertyPanel()->activeWidget();
  if (aActive)
    return aActive->restoreValue();
  return true;
}
