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
#include "ModuleBase_IModule.h"
#include "ModuleBase_IViewer.h"
#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_PageWidget.h"
#include "ModuleBase_WidgetMultiSelector.h"
#include "ModuleBase_ResultPrs.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <GeomAPI_ShapeExplorer.h>

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
  ModuleBase_WidgetMultiSelector* aSelector =
    dynamic_cast<ModuleBase_WidgetMultiSelector*>(parent());
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
  const std::string& theFilter, FeaturePtr theSelection, QWidget* theParent)
  : QWidget(theParent), myFilterID(theFilter),
    mySelection(std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(theSelection))
{
  QHBoxLayout* aLayout = new QHBoxLayout(this);
  aLayout->setContentsMargins(0, 0, 0, 0);

  // Reverse filter button
  myRevBtn = new QToolButton(this);
  myRevBtn->setCheckable(true);
  myRevBtn->setChecked(false);
  myRevBtn->setAutoRaise(true);
  myRevBtn->setIcon(QIcon(":pictures/accept.png"));
  myRevBtn->setToolTip(tr("Reverse the filter"));
  connect(myRevBtn, SIGNAL(toggled(bool)), SLOT(onReverse(bool)));
  aLayout->addWidget(myRevBtn);

  const std::string& aFilterName = ModelAPI_Session::get()->filters()->filter(myFilterID)->name();
  aLayout->addWidget(new QLabel(aFilterName.c_str(), this), 1);

  QToolButton* aDelBtn = new QToolButton(this);
  aDelBtn->setIcon(QIcon(":pictures/button_cancel.png"));
  aDelBtn->setAutoRaise(true);
  aDelBtn->setToolTip(tr("Delete the filter"));
  connect(aDelBtn, SIGNAL(clicked(bool)), SLOT(onDelete()));
  aLayout->addWidget(aDelBtn);

  myRevBtn->setChecked(mySelection->isReversed(myFilterID));
}


void ModuleBase_FilterItem::onReverse(bool theCheck)
{
  mySelection->setReversed(myFilterID, theCheck);
  if (theCheck)
    myRevBtn->setIcon(QIcon(":pictures/stop.png"));
  else
    myRevBtn->setIcon(QIcon(":pictures/accept.png"));
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
  AttributePtr aAttr = SelectorFeature->attribute(AttributeId);
  AttributeSelectionListPtr aSelListAttr =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aAttr);
  mySelectionType = selectionType(aSelListAttr->selectionType().c_str());
  aSelListAttr->clear();

  // Define widgets
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLayout);

  myFiltersGroup = new QGroupBox(tr("Dynamic Filters"), this);
  myGroupLayout = new QVBoxLayout(myFiltersGroup);
  myGroupLayout->setContentsMargins(0, 0, 0, 0);
  myGroupLayout->setSpacing(0);

  QWidget* aFiltersWgt = new QWidget(myFiltersGroup);
  QHBoxLayout* aFiltersLay = new QHBoxLayout(aFiltersWgt);
  ModuleBase_Tools::adjustMargins(aFiltersLay);

  QLabel* aFilterLbl = new QLabel(aFiltersWgt);
  aFilterLbl->setPixmap(QPixmap(":pictures/filter.png"));

  myFiltersCombo = new QComboBox(aFiltersWgt);
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

  QToolButton* aAddBtn = new QToolButton(aFiltersWgt);
  aAddBtn->setIcon(QIcon(":pictures/add.png"));
  aAddBtn->setAutoRaise(true);
  aAddBtn->setToolTip(tr("Add the current filter"));
  connect(aAddBtn, SIGNAL(clicked()), SLOT(onAddItem()));

  aFiltersLay->addWidget(aFilterLbl);
  aFiltersLay->addWidget(myFiltersCombo, 1);
  aFiltersLay->addWidget(aAddBtn);

  myGroupLayout->addWidget(aFiltersWgt);

  aMainLayout->addWidget(myFiltersGroup);

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
  ModuleBase_Tools::adjustMargins(aLblLayout);

  aLblLayout->addWidget(new QLabel(tr("Number of selected objects:"), aLblWgt));

  myNbLbl = new QLabel("0", aLblWgt);
  aLblLayout->addWidget(myNbLbl);

  aMainLayout->addWidget(aLblWgt);

  // Show only button
  QWidget* aBtn2Wgt = new QWidget(this);
  QHBoxLayout* aBtn2Layout = new QHBoxLayout(aBtn2Wgt);
  ModuleBase_Tools::adjustMargins(aBtn2Layout);

  aBtn2Layout->addStretch(1);

  myShowBtn = new QPushButton(tr("Show only"), aBtn2Wgt);
  myShowBtn->setCheckable(true);
  connect(myShowBtn, SIGNAL(toggled(bool)), SLOT(onShowOnly(bool)));
  aBtn2Layout->addWidget(myShowBtn);

  aMainLayout->addWidget(aBtn2Wgt);

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

void ModuleBase_WidgetSelectionFilter::onAddItem()
{
  int aId = myFiltersCombo->currentIndex();
  myFiltersCombo->removeItem(aId);

  std::list<std::string>::iterator aIt;
  int i;
  std::string aFilter;
  for (aIt = myFilters.begin(), i = 0; aIt != myFilters.cend(); i++, aIt++) {
    if (i == aId) {
      aFilter = (*aIt);
      myFilters.erase(aIt);
      break;
    }
  }
  if (!aFilter.empty()) {
    myUseFilters.push_back(aFilter);
    ModuleBase_FilterItem* aItem =
      new ModuleBase_FilterItem(aFilter, mySelectorFeature, myFiltersGroup);
    connect(aItem, SIGNAL(deleteItem(ModuleBase_FilterItem*)),
      SLOT(onDeleteItem(ModuleBase_FilterItem*)));
    myGroupLayout->addWidget(aItem);
  }
  updateSelectBtn();
  clearCurrentSelection(true);
  updateNumberSelected();
}

void ModuleBase_WidgetSelectionFilter::onDeleteItem(ModuleBase_FilterItem* theItem)
{
  std::string aFilter = theItem->filter();
  myGroupLayout->removeWidget(theItem);
  theItem->deleteLater();

  myUseFilters.remove(aFilter);
  myFilters.push_back(aFilter);
  myFiltersCombo->addItem(ModelAPI_Session::get()->filters()->filter(aFilter)->name().c_str());

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
    myPreview->SetDisplayMode(myShowBtn->isChecked()? AIS_Shaded : AIS_WireFrame);
    myPreview->SetColor(Quantity_NOC_YELLOW);
    myPreview->SetTransparency();
    aCtx->Display(myPreview, true);
    aCtx->Deactivate(myPreview);
  }
  else {
    myPreview->Set(theShape);
    aCtx->Redisplay(myPreview, true);
  }
}


void ModuleBase_WidgetSelectionFilter::onShowOnly(bool theErase)
{
  if (myPreview.IsNull())
    return;
  Handle(AIS_InteractiveContext) aCtx = myWorkshop->viewer()->AISContext();

  if (theErase) {
    aCtx->SetDisplayMode(myPreview, AIS_Shaded, false);
    myListIO.Clear();
    aCtx->DisplayedObjects(AIS_KOI_Shape, -1, myListIO);
    myListIO.Remove(myPreview);
  }
  else {
    aCtx->SetDisplayMode(myPreview, AIS_WireFrame, false);
  }
  AIS_ListOfInteractive::const_iterator aIt;
  Handle(AIS_Shape) aShapeIO;
  for (aIt = myListIO.cbegin(); aIt != myListIO.cend(); aIt++) {
    aShapeIO = Handle(AIS_Shape)::DownCast(*aIt);
    if (!aShapeIO.IsNull()) {
      if (theErase)
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
  return QList<QWidget*>();
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