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

#include "XGUI_InspectionPanel.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Selection.h"
#include "XGUI_Tools.h"

#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_Result.h>

#include <QLayout>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QTextBrowser>

#include <BRep_Tool.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic

XGUI_InspectionPanel::XGUI_InspectionPanel(QWidget* theParent, XGUI_SelectionMgr* theMgr)
  : QDockWidget(theParent),
  mySelectionMgr(theMgr)
{
  setWindowTitle(tr("Inspection Panel"));
  setObjectName(INSPECTION_PANEL);
  setStyleSheet("::title { position: relative; padding-left: 5px; text-align: left center }");

  QScrollArea* aScrollArea = new QScrollArea(this);
  setWidget(aScrollArea);

  // Create an internal widget
  QWidget* aMainWidget = new QWidget(aScrollArea);

  QVBoxLayout* aMainLayout = new QVBoxLayout(aMainWidget);
  aMainLayout->setContentsMargins(5, 5, 5, 5);

  QWidget* aNameWgt = new QWidget(aMainWidget);
  QHBoxLayout* aNameLayout = new QHBoxLayout(aNameWgt);
  aNameLayout->setContentsMargins(0, 0, 0, 0);
  aNameLayout->addWidget(new QLabel(tr("Object"), aNameWgt));
  myNameEdt = new QLineEdit(aNameWgt);
  myNameEdt->setReadOnly(true);
  aNameLayout->addWidget(myNameEdt);

  aMainLayout->addWidget(aNameWgt);

  // Table with sub-shapes
  mySubShapesTab = new QTableWidget(9, 2, aMainWidget);
  mySubShapesTab->setFocusPolicy(Qt::NoFocus);
  mySubShapesTab->verticalHeader()->hide();
  QStringList aTitles;
  aTitles << tr("Sub-shapes") << tr("Number");
  mySubShapesTab->setHorizontalHeaderLabels(aTitles);

  QStringList aSubShapes;
  aSubShapes << "SHAPE" << "COMPOUND" << "COMPSOLID" <<
    "SOLID" << "SHELL" << "FACE" << "WIRE" << "EDGE" << "VERTEX";
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

  mySubShapesTab->setMaximumWidth(170);
  mySubShapesTab->setMinimumHeight(300);

  aMainLayout->addWidget(mySubShapesTab);

  // Type of object
  QWidget* aTypeWgt = new QWidget(aMainWidget);
  QHBoxLayout* aTypeLayout = new QHBoxLayout(aTypeWgt);
  aTypeLayout->setContentsMargins(0, 0, 0, 0);

  aTypeLayout->addWidget(new QLabel(tr("Type:"), aTypeWgt));
  myTypeLbl = new QLabel("", aTypeWgt);
  aTypeLayout->addWidget(myTypeLbl);

  aMainLayout->addWidget(aTypeWgt);

  myTypeParams = new QTextBrowser(aMainWidget);
  myTypeParams->setMaximumWidth(170);
  myTypeParams->setMaximumHeight(160);
  myTypeParams->setReadOnly(true);
  myTypeParams->setFocusPolicy(Qt::NoFocus);
  myTypeParams->setFrameStyle(QFrame::NoFrame);
  myTypeParams->viewport()->setBackgroundRole(QPalette::Window);

  aMainLayout->addWidget(myTypeParams);

  aScrollArea->setWidget(aMainWidget);

  connect(mySelectionMgr, SIGNAL(selectionChanged()), SLOT(onSelectionChanged()));
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
  setName("");
  for (int i = 0; i <= VertexId; i++) {
    mySubShapesTab->item((SudShape)i, 1)->setText("");
  }
  myTypeLbl->setText("");
  myTypeParams->setText("");
}

//********************************************************************
void XGUI_InspectionPanel::onSelectionChanged()
{
  clearContent();
  XGUI_Selection* aSelection = mySelectionMgr->selection();
  QList<ModuleBase_ViewerPrsPtr> aSelectedList = aSelection->getSelected(ModuleBase_ISelection::Viewer);
  if (aSelectedList.count() > 0) {
    ModuleBase_ViewerPrsPtr aPrs = aSelectedList.first();
    setName(XGUI_Tools::generateName(aPrs));

    TopoDS_Shape aShape = ModuleBase_Tools::getSelectedShape(aPrs);
    setShapeContent(aShape);
    setShapeParams(aShape);
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
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    //SetErrorCode(aFail->GetMessageString());
  }
}

//********************************************************************
void XGUI_InspectionPanel::setShapeParams(const TopoDS_Shape& theShape)
{
  switch (theShape.ShapeType()) {
  case TopAbs_VERTEX:
    fillVertex(theShape);
    break;
  case TopAbs_EDGE:
    fillEdge(theShape);
    break;
  case TopAbs_FACE:
    fillFace(theShape);
    break;
  case TopAbs_SOLID:
    fillSolid(theShape);
    break;
  case TopAbs_WIRE:
  case TopAbs_SHELL:
  case TopAbs_COMPSOLID:
  case TopAbs_COMPOUND:
    fillContainer(theShape);
    break;
  }
}

//********************************************************************
void XGUI_InspectionPanel::fillVertex(const TopoDS_Shape& theShape)
{
  TopoDS_Vertex aV = TopoDS::Vertex(theShape);
  gp_Pnt aP = BRep_Tool::Pnt(aV);
  setVertexType(aP.XYZ());
}

//********************************************************************
void XGUI_InspectionPanel::fillEdge(const TopoDS_Shape& theShape)
{
  TopoDS_Edge aE = TopoDS::Edge(theShape);

  bool bDegenerated = BRep_Tool::Degenerated(aE);

  double aT1, aT2;
  Handle(Geom_Curve) aC3D = BRep_Tool::Curve(aE, aT1, aT2);
  GeomAdaptor_Curve aGAC(aC3D);
  GeomAbs_CurveType aCT = aGAC.GetType();

  if (aCT == GeomAbs_Line) { // Line
    gp_Pnt aP1, aP2;
    aGAC.D0(aT1, aP1);
    aGAC.D0(aT2, aP2);
    setLineType(aP1.XYZ(), aP2.XYZ());

  } else if (aCT == GeomAbs_Circle) {
    gp_Circ aCirc = aGAC.Circle();
    gp_Pnt aP = aCirc.Location();
    gp_Ax2 aAx2 = aCirc.Position();
    double aR1 = aCirc.Radius();
    gp_Dir aDir = aAx2.Axis().Direction();

    bool isArc = (Abs(aT2 - aT1 - aC3D->Period()) >= Precision::PConfusion());
    if (isArc) {
      gp_Pnt aP1, aP2;
      aGAC.D0(aT1, aP1);
      aGAC.D0(aT2, aP2);
      setArcType(aP.XYZ(), aDir.XYZ(), aR1, aP1.XYZ(), aP2.XYZ());
    } else
      setCircleType(aP.XYZ(), aDir.XYZ(), aR1);

  } else if (aCT == GeomAbs_Ellipse) {
    gp_Elips aElips = aGAC.Ellipse();
    gp_Pnt aP = aElips.Location();
    gp_Ax2 aAx2 = aElips.Position();
    double aR1 = aElips.MajorRadius();
    double aR2 = aElips.MinorRadius();
    gp_Dir aDir = aAx2.Axis().Direction();
    gp_Pnt aP1, aP2;
    aGAC.D0(aT1, aP1);
    aGAC.D0(aT2, aP2);
    bool isArc = aP1.Distance(aP2) > Precision::Confusion();
    if (isArc)
      setEllipseArcType(aP.XYZ(), aDir.XYZ(), aR1, aR2, aP1.XYZ(), aP2.XYZ());
    else
      setEllipseType(aP.XYZ(), aDir.XYZ(), aR1, aR2);
  }
}

//********************************************************************
void XGUI_InspectionPanel::fillFace(const TopoDS_Shape& theShape)
{

}

//********************************************************************
void XGUI_InspectionPanel::fillSolid(const TopoDS_Shape& theShape)
{

}

//********************************************************************
void XGUI_InspectionPanel::fillContainer(const TopoDS_Shape& theShape)
{

}

//********************************************************************
#define TITLE(val) ("<b>" + val + "</b>")

void XGUI_InspectionPanel::setCylinderType(double theX, double theY, double theZ,
  double theDX, double theDY, double theDZ, double theRadius, double theHeight)
{
  myTypeLbl->setText(tr("Cylinder"));
  QString aParams = TITLE(tr("Center")) +
    "<br> X: " + QString::number(theX) +
    "<br> Y: " + QString::number(theY) +
    "<br> Z: " + QString::number(theZ) +
    "<br>" + TITLE(tr("Axis")) +
    "<br> DX: " + QString::number(theDX) +
    "<br> DY: " + QString::number(theDY) +
    "<br> DZ: " + QString::number(theDZ) +
    "<br>" + TITLE(tr("Dimensions")) +
    "<br>" + tr("Radius:") + QString::number(theRadius) +
    "<br>" + tr("Height") + QString::number(theHeight);

  myTypeParams->setText(aParams);
}

//********************************************************************
void XGUI_InspectionPanel::setSphereType(double theX, double theY, double theZ, double theRadius)
{
  myTypeLbl->setText(tr("Sphere"));
  QString aParams = TITLE(tr("Center")) +
    "<br> X: " + QString::number(theX) +
    "<br> Y: " + QString::number(theY) +
    "<br> Z: " + QString::number(theZ) +
    "<br>" + TITLE(tr("Dimensions")) +
    "<br>" + tr("Radius:") + QString::number(theRadius);
  myTypeParams->setText(aParams);
}

//********************************************************************
void XGUI_InspectionPanel::setBoxType(double theX, double theY, double theZ,
  double theXsize, double theYsize, double theZsize)
{
  myTypeLbl->setText(tr("Box"));
  QString aParams = TITLE(tr("Position")) +
    "<br> X: " + QString::number(theX) +
    "<br> Y: " + QString::number(theY) +
    "<br> Z: " + QString::number(theZ) +
    "<br>" + TITLE(tr("Dimensions")) +
    "<br>" + "Ax :" + QString::number(theXsize) +
    "<br>" + "Ay :" + QString::number(theYsize) +
    "<br>" + "Az :" + QString::number(theZsize);
  myTypeParams->setText(aParams);
}

//********************************************************************
void XGUI_InspectionPanel::setRotatedBoxType(double theX, double theY, double theZ,
  double theZaxisX, double theZaxisY, double theZaxisZ,
  double theXaxisX, double theXaxisY, double theXaxisZ,
  double theXsize, double theYsize, double theZsize)
{
  myTypeLbl->setText(tr("Box"));
  QString aParams = TITLE(tr("Position")) +
    "<br> X: " + QString::number(theX) +
    "<br> Y: " + QString::number(theY) +
    "<br> Z: " + QString::number(theZ) +
    "<br>" + TITLE(tr("Z axis")) +
    "<br> DX: " + QString::number(theZaxisX) +
    "<br> DY: " + QString::number(theZaxisY) +
    "<br> DZ: " + QString::number(theZaxisZ) +
    "<br>" + TITLE(tr("X axis")) +
    "<br> DX: " + QString::number(theXaxisX) +
    "<br> DY: " + QString::number(theXaxisY) +
    "<br> DZ: " + QString::number(theXaxisZ) +
    "<br>" + TITLE(tr("Dimensions")) +
    "<br>" + "Ax :" + QString::number(theXsize) +
    "<br>" + "Ay :" + QString::number(theYsize) +
    "<br>" + "Az :" + QString::number(theZsize);
  myTypeParams->setText(aParams);
}

//********************************************************************
void XGUI_InspectionPanel::setPlaneType(double theX, double theY, double theZ,
  double theDX, double theDY, double theDZ)
{
  myTypeLbl->setText(tr("Plane"));
  QString aParams = TITLE(tr("Center")) +
    "<br> X: " + QString::number(theX) +
    "<br> Y: " + QString::number(theY) +
    "<br> Z: " + QString::number(theZ) +
    "<br>" + TITLE(tr("Normal")) +
    "<br> DX: " + QString::number(theDX) +
    "<br> DY: " + QString::number(theDY) +
    "<br> DZ: " + QString::number(theDZ);
  myTypeParams->setText(aParams);
}

//********************************************************************
void XGUI_InspectionPanel::setVertexType(const gp_XYZ& theLoc)
{
  myTypeLbl->setText(tr("Vertex"));
  QString aParams = TITLE(tr("Coordinates")) +
    "<br> X: " + QString::number(theLoc.X()) +
    "<br> Y: " + QString::number(theLoc.Y()) +
    "<br> Z: " + QString::number(theLoc.Z());
  myTypeParams->setText(aParams);
}

//********************************************************************
void XGUI_InspectionPanel::setCircleType(const gp_XYZ& theLoc, const gp_XYZ& theDir,
  double theRadius)
{
  myTypeLbl->setText(tr("Circle"));
  QString aParams = TITLE(tr("Center")) +
    "<br> X: " + QString::number(theLoc.X()) +
    "<br> Y: " + QString::number(theLoc.Y()) +
    "<br> Z: " + QString::number(theLoc.Z()) +
    "<br>" + TITLE(tr("Normal")) +
    "<br> DX: " + QString::number(theDir.X()) +
    "<br> DY: " + QString::number(theDir.Y()) +
    "<br> DZ: " + QString::number(theDir.Z()) +
    "<br>" + TITLE(tr("Dimensions")) +
    "<br>" + tr("Radius:") + QString::number(theRadius);
  myTypeParams->setText(aParams);
}

//********************************************************************
void XGUI_InspectionPanel::setArcType(const gp_XYZ& theLoc, const gp_XYZ& theDir,
  double theRadius, const gp_XYZ& theP1, const gp_XYZ& theP2)
{
  myTypeLbl->setText(tr("Arc"));
  QString aParams = TITLE(tr("Center")) +
    "<br> X: " + QString::number(theLoc.X()) +
    "<br> Y: " + QString::number(theLoc.Y()) +
    "<br> Z: " + QString::number(theLoc.Z()) +
    "<br>" + TITLE(tr("Normal")) +
    "<br> DX: " + QString::number(theDir.X()) +
    "<br> DY: " + QString::number(theDir.Y()) +
    "<br> DZ: " + QString::number(theDir.Z()) +
    "<br>" + TITLE(tr("Dimensions")) +
    "<br>" + tr("Radius:") + QString::number(theRadius) +
    "<br>" + TITLE(tr("Point 1")) +
    "<br> X: " + QString::number(theP1.X()) +
    "<br> Y: " + QString::number(theP1.Y()) +
    "<br> Z: " + QString::number(theP1.Z()) +
    "<br>" + TITLE(tr("Point 2")) +
    "<br> X: " + QString::number(theP2.X()) +
    "<br> Y: " + QString::number(theP2.Y()) +
    "<br> Z: " + QString::number(theP2.Z());
  myTypeParams->setText(aParams);
}

//********************************************************************
void XGUI_InspectionPanel::setEllipseType(const gp_XYZ& theLoc, const gp_XYZ& theDir,
  double theMajorRad, double theMinorRad)
{
  myTypeLbl->setText(tr("Ellipse"));
  QString aParams = TITLE(tr("Center")) +
    "<br> X: " + QString::number(theLoc.X()) +
    "<br> Y: " + QString::number(theLoc.Y()) +
    "<br> Z: " + QString::number(theLoc.Z()) +
    "<br>" + TITLE(tr("Normal")) +
    "<br> DX: " + QString::number(theDir.X()) +
    "<br> DY: " + QString::number(theDir.Y()) +
    "<br> DZ: " + QString::number(theDir.Z()) +
    "<br>" + TITLE(tr("Dimensions")) +
    "<br>" + tr("Major radius:") + QString::number(theMajorRad) +
    "<br>" + tr("Minor radius:") + QString::number(theMinorRad);
  myTypeParams->setText(aParams);
}

//********************************************************************
void XGUI_InspectionPanel::setEllipseArcType(const gp_XYZ& theLoc, const gp_XYZ& theDir,
  double theMajorRad, double theMinorRad, const gp_XYZ& theP1, const gp_XYZ& theP2)
{
  myTypeLbl->setText(tr("Elliptical arc"));
  QString aParams = TITLE(tr("Center")) +
    "<br> X: " + QString::number(theLoc.X()) +
    "<br> Y: " + QString::number(theLoc.Y()) +
    "<br> Z: " + QString::number(theLoc.Z()) +
    "<br>" + TITLE(tr("Normal")) +
    "<br> DX: " + QString::number(theDir.X()) +
    "<br> DY: " + QString::number(theDir.Y()) +
    "<br> DZ: " + QString::number(theDir.Z()) +
    "<br>" + TITLE(tr("Dimensions")) +
    "<br>" + tr("Major radius:") + QString::number(theMajorRad) +
    "<br>" + tr("Minor radius:") + QString::number(theMinorRad) +
    "<br>" + TITLE(tr("Point 1")) +
    "<br> X: " + QString::number(theP1.X()) +
    "<br> Y: " + QString::number(theP1.Y()) +
    "<br> Z: " + QString::number(theP1.Z()) +
    "<br>" + TITLE(tr("Point 2")) +
    "<br> X: " + QString::number(theP2.X()) +
    "<br> Y: " + QString::number(theP2.Y()) +
    "<br> Z: " + QString::number(theP2.Z());
  myTypeParams->setText(aParams);
}

void XGUI_InspectionPanel::setLineType(const gp_XYZ& theP1, const gp_XYZ& theP2)
{
  myTypeLbl->setText(tr("Line"));
  QString aParams = TITLE(tr("Point 1")) +
    "<br> X: " + QString::number(theP1.X()) +
    "<br> Y: " + QString::number(theP1.Y()) +
    "<br> Z: " + QString::number(theP1.Z()) +
    "<br>" + TITLE(tr("Point 2")) +
    "<br> X: " + QString::number(theP2.X()) +
    "<br> Y: " + QString::number(theP2.Y()) +
    "<br> Z: " + QString::number(theP2.Z());
  myTypeParams->setText(aParams);
}
