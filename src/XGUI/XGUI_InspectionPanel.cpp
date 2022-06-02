// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "XGUI_InspectionPanel.h"
#include "XGUI_Workshop.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Selection.h"
#include "XGUI_Tools.h"
#include "XGUI_ModuleConnector.h"

#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_WidgetFactory.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_PageWidget.h>

#include <ModelAPI_ResultField.h>

#include <ModelAPI_Result.h>

#include <QLayout>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QTextBrowser>
#include <QResizeEvent>
#include <QSplitter>
#include <QStackedWidget>

#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Iterator.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic


// ================     XGUI_InspectionPanel    ================
XGUI_InspectionPanel::XGUI_InspectionPanel(QWidget* theParent, XGUI_Workshop* theWorkshop)
  : QDockWidget(theParent),
  myWorkshop(theWorkshop)
{
  setWindowTitle(tr("Inspection Panel"));
  setObjectName(INSPECTION_PANEL);
  setStyleSheet("::title { position: relative; padding-left: 5px; text-align: left center }");

  myStackWgt = new QStackedWidget(this);

  // Create shape selection page
  QSplitter* aSplitter = new QSplitter(Qt::Vertical, myStackWgt);

  // Create an internal widget
  QWidget* aNameWgt = new QWidget(aSplitter);
  QHBoxLayout* aNameLayout = new QHBoxLayout(aNameWgt);
  aNameLayout->setContentsMargins(3, 0, 3, 0);
  aNameLayout->addWidget(new QLabel(tr("Object"), aNameWgt));
  myNameEdt = new QLineEdit(aNameWgt);
  myNameEdt->setReadOnly(true);
  aNameLayout->addWidget(myNameEdt);

  aSplitter->addWidget(aNameWgt);

  // Table with sub-shapes
  mySubShapesTab = new QTableWidget(9, 2, aSplitter);
  mySubShapesTab->setFocusPolicy(Qt::NoFocus);
  mySubShapesTab->verticalHeader()->hide();
  QStringList aTitles;
  aTitles << tr("Sub-shapes") << tr("Number");
  mySubShapesTab->setHorizontalHeaderLabels(aTitles);

  QStringList aSubShapes;
  aSubShapes << tr("SHAPE") << tr("COMPOUND") << tr("COMPSOLID") <<
    tr("SOLID") << tr("SHELL") << tr("FACE") << tr("WIRE") << tr("EDGE") << tr("VERTEX");
  int i = 0;
  foreach(QString aType, aSubShapes) {
    QTableWidgetItem* aItem = new QTableWidgetItem(aType);
    aItem->setFlags(Qt::ItemIsEnabled);
    mySubShapesTab->setItem(i++, 0, aItem);
  }
  for (i = 0; i < 9; i++) {
    QTableWidgetItem* aItem = new QTableWidgetItem("");
    aItem->setFlags(Qt::ItemIsEnabled);
    aItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    mySubShapesTab->setItem(i, 1, aItem);
  }
  mySubShapesTab->setColumnWidth(0, 90);
  mySubShapesTab->setColumnWidth(1, 70);

  aSplitter->addWidget(mySubShapesTab);

  // Type of object
  QWidget* aTypeWgt = new QWidget(aSplitter);
  QHBoxLayout* aTypeLayout = new QHBoxLayout(aTypeWgt);
  aTypeLayout->setContentsMargins(3, 0, 3, 0);

  aTypeLayout->addWidget(new QLabel(tr("Type:"), aTypeWgt));
  myTypeLbl = new QLabel("", aTypeWgt);
  aTypeLayout->addWidget(myTypeLbl);

  aSplitter->addWidget(aTypeWgt);

  myTypeParams = new QTextBrowser(aSplitter);
  myTypeParams->setReadOnly(true);
  myTypeParams->setFocusPolicy(Qt::NoFocus);
  myTypeParams->viewport()->setBackgroundRole(QPalette::Window);

  aSplitter->addWidget(myTypeParams);

  aSplitter->setCollapsible(0, false);
  aSplitter->setCollapsible(1, false);
  aSplitter->setCollapsible(2, false);
  aSplitter->setCollapsible(3, false);

  QList<int> aSizes;
  aSizes << 10 << 140 << 10;
  aSplitter->setSizes(aSizes);

  myShapePanelId = myStackWgt->addWidget(aSplitter);

  // Create feature selection page
  QScrollArea* aScroll = new QScrollArea(myStackWgt);
  aScroll->setWidgetResizable(true);
  aScroll->setFrameStyle(QFrame::NoFrame);

  myFeaturePane = new ModuleBase_PageWidget(aScroll);
  myFeatureLayout = new QGridLayout(myFeaturePane);
  myFeatureLayout->setContentsMargins(3, 3, 3, 3);
  aScroll->setWidget(myFeaturePane);
  //myFeaturePane->setEnabled(false);

  myFeaturePanelId = myStackWgt->addWidget(aScroll);

  setWidget(myStackWgt);
  connect(myWorkshop->selector(), SIGNAL(selectionChanged()), SLOT(onSelectionChanged()));
}

//********************************************************************
XGUI_InspectionPanel::~XGUI_InspectionPanel()
{
}

//********************************************************************
void XGUI_InspectionPanel::setSubShapeValue(SudShape theId, int theVal)
{
  mySubShapesTab->item(theId, 1)->setText(QString::number(theVal));
}

//********************************************************************
void XGUI_InspectionPanel::clearContent()
{
  myStackWgt->setCurrentIndex(myFeaturePanelId);
  setName("");
  for (int i = 0; i <= VertexId; i++) {
    mySubShapesTab->item((SudShape)i, 1)->setText("");
  }
  myTypeLbl->setText("");
  setParamsText("");

  myFeaturePane->clearPage();
}

//********************************************************************
void XGUI_InspectionPanel::onSelectionChanged()
{
  if (!isVisible())
    return;

  clearContent();

  XGUI_Selection* aSelection = myWorkshop->selector()->selection();
  QList<ModuleBase_ViewerPrsPtr> aSelectedList =
    aSelection->getSelected(myWorkshop->selector()->placeOfSelection());

  if (aSelectedList.count() > 0) {
    ModuleBase_ViewerPrsPtr aPrs = aSelectedList.first();
    FieldStepPtr aStep =
      std::dynamic_pointer_cast<ModelAPI_ResultField::ModelAPI_FieldStep>(aPrs->object());
    if (aStep)
      return;
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aPrs->object());
    if (aFeature.get()) {
      myStackWgt->setCurrentIndex(myFeaturePanelId);
      buildFeaturePane(aFeature);
    }
    else {
      TopoDS_Shape aShape = ModuleBase_Tools::getSelectedShape(aPrs);
      if (aShape.IsNull()) {
        ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aPrs->object());
        if (aRes.get()) {
          GeomShapePtr aShpPtr = aRes->shape();
          if (aShpPtr.get()) {
            aShape = aShpPtr->impl<TopoDS_Shape>();
          }
        }
      }
      if (aShape.IsNull()) {
        myStackWgt->setCurrentIndex(myFeaturePanelId);
        return;
      }
      myStackWgt->setCurrentIndex(myShapePanelId);
      GeomShapePtr aShapePtr(new GeomAPI_Shape());
      aShapePtr->setImpl(new TopoDS_Shape(aShape));

      ModuleBase_ViewerPrsPtr aPrsCopy(new ModuleBase_ViewerPrs(aPrs->object(), aShapePtr));
      setName(XGUI_Tools::generateName(aPrsCopy));
      setShapeContent(aShape);

      GeomShapePtr aGShape(new GeomAPI_Shape);
      aGShape->setImpl(new TopoDS_Shape(aShape));
      GeomAlgoAPI_ShapeInfo aShapeInfo(aGShape);
      QString aParams = QString::fromStdString(aShapeInfo.html(this));
      setParamsText(aParams);
      myTypeLbl->setText(tr(aShapeInfo.shapeType().c_str()));
    }
  }
}

//********************************************************************
void XGUI_InspectionPanel::setName(const QString& theName)
{
  myNameEdt->setText(theName);
}

//********************************************************************
void XGUI_InspectionPanel::setShapeContent(const TopoDS_Shape& theShape)
{
  try {
    OCC_CATCH_SIGNALS;
    int iType, nbTypes[TopAbs_SHAPE];
    for (iType = 0; iType < TopAbs_SHAPE; ++iType) {
      nbTypes[iType] = 0;
    }
    nbTypes[theShape.ShapeType()]++;

    TopTools_MapOfShape aMapOfShape;
    aMapOfShape.Add(theShape);
    TopTools_ListOfShape aListOfShape;
    aListOfShape.Append(theShape);

    TopTools_ListIteratorOfListOfShape itL(aListOfShape);
    for (; itL.More(); itL.Next()) {
      TopoDS_Shape sp = itL.Value();
      TopoDS_Iterator it(sp);
      for (; it.More(); it.Next()) {
        TopoDS_Shape s = it.Value();
        if (aMapOfShape.Add(s)) {
          aListOfShape.Append(s);
          nbTypes[s.ShapeType()]++;
        }
      }
    }
    setSubShapeValue(VertexId, nbTypes[TopAbs_VERTEX]);
    setSubShapeValue(EdgeId, nbTypes[TopAbs_EDGE]);
    setSubShapeValue(WireId, nbTypes[TopAbs_WIRE]);
    setSubShapeValue(FaceId, nbTypes[TopAbs_FACE]);
    setSubShapeValue(ShellId, nbTypes[TopAbs_SHELL]);
    setSubShapeValue(SolidId, nbTypes[TopAbs_SOLID]);
    setSubShapeValue(CompsolidId, nbTypes[TopAbs_COMPSOLID]);
    setSubShapeValue(CompoundId, nbTypes[TopAbs_COMPOUND]);
    setSubShapeValue(ShapeId, aMapOfShape.Extent());
  }
  catch (Standard_Failure) {
  }
}

std::string XGUI_InspectionPanel::translate(const char* theSource)
{
  return std::string(tr(theSource).toStdString());
}

//********************************************************************
void XGUI_InspectionPanel::setParamsText(const QString& theText)
{
  myTypeParams->setText(theText);
}

//********************************************************************
void XGUI_InspectionPanel::buildFeaturePane(const FeaturePtr& theFeature)
{
  std::string aXmlCfg, aDescription;
  myWorkshop->module()->getXMLRepresentation(theFeature->getKind(), aXmlCfg, aDescription);
  if (!aXmlCfg.empty()) {
    QList<ModuleBase_ModelWidget*> aWidgets;
    if (!myWorkshop->module()->createWidgets(theFeature, aXmlCfg.c_str(), aWidgets)) {
      ModuleBase_WidgetFactory aFactory(aXmlCfg, myWorkshop->moduleConnector());
      aFactory.createWidget(myFeaturePane);
      aWidgets = aFactory.getModelWidgets();
    }
    foreach(ModuleBase_ModelWidget* aWgt, aWidgets) {
      if (aWgt->isInformative()) {
        aWgt->setFeature(theFeature, false, false);
        aWgt->setReadOnly(true);
        aWgt->setEditingMode(true);
        aWgt->restoreValue();
        aWgt->showInformativePage();
      }
      else {
        aWgt->setFeature(theFeature, false, false);
        aWgt->setEditingMode(true);
        aWgt->hide();
      }
    }
  }
}

void XGUI_InspectionPanel::showEvent(QShowEvent* theEvent)
{
  QDockWidget::showEvent(theEvent);
  onSelectionChanged();
}
