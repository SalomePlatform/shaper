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

#include <GeomAPI_Ax3.h>
#include <GeomAPI_Box.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_Cone.h>
#include <GeomAPI_Cylinder.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Ellipse.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shell.h>
#include <GeomAPI_Solid.h>
#include <GeomAPI_Sphere.h>
#include <GeomAPI_Torus.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Wire.h>

#include <QLayout>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QTextBrowser>
#include <QResizeEvent>

#include <BRepBndLib.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic

// ================     Auxiliary functions     ================
#define TITLE(val) ("<b>" + (val) + "</b>")

static void appendPointToParameters(const QString& thePointTitle,
                                    const GeomPointPtr& theCoord,
                                          QString& theParams)
{
  theParams += TITLE(thePointTitle) +
    "<br> X: " + QString::number(theCoord->x()) +
    "<br> Y: " + QString::number(theCoord->y()) +
    "<br> Z: " + QString::number(theCoord->z()) +
    "<br>";
}

static void appendDirToParameters(const QString& theDirTitle,
                                  const GeomDirPtr& theDirection,
                                        QString& theParams)
{
  theParams += TITLE(theDirTitle) +
    "<br> DX: " + QString::number(theDirection->x()) +
    "<br> DY: " + QString::number(theDirection->y()) +
    "<br> DZ: " + QString::number(theDirection->z()) +
    "<br>";
}

static void appendGroupNameToParameters(const QString& theGroupTitle, QString& theParams)
{
  theParams += TITLE(theGroupTitle) + "<br>";
}

static void appendNamedValueToParameters(const QString& theName,
                                         const double   theValue,
                                               QString& theParams)
{
  theParams += theName + ": " + QString::number(theValue) + "<br>";
}

static void appendNamedValueToParameters(const QString& theName,
                                         const bool     theValue,
                                               QString& theParams)
{
  theParams += theName + ": " + (theValue ? "True" : "False") + "<br>";
}


// ================     XGUI_InspectionPanel    ================

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
  myMainWidget = new QWidget(aScrollArea);

  myMainLayout = new QVBoxLayout(myMainWidget);
  myMainLayout->setContentsMargins(5, 5, 5, 5);

  QWidget* aNameWgt = new QWidget(myMainWidget);
  QHBoxLayout* aNameLayout = new QHBoxLayout(aNameWgt);
  aNameLayout->setContentsMargins(0, 0, 0, 0);
  aNameLayout->addWidget(new QLabel(tr("Object"), aNameWgt));
  myNameEdt = new QLineEdit(aNameWgt);
  myNameEdt->setReadOnly(true);
  aNameLayout->addWidget(myNameEdt);

  myMainLayout->addWidget(aNameWgt);

  // Table with sub-shapes
  mySubShapesTab = new QTableWidget(9, 2, myMainWidget);
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

  myMainLayout->addWidget(mySubShapesTab);

  // Type of object
  QWidget* aTypeWgt = new QWidget(myMainWidget);
  QHBoxLayout* aTypeLayout = new QHBoxLayout(aTypeWgt);
  aTypeLayout->setContentsMargins(0, 0, 0, 0);

  aTypeLayout->addWidget(new QLabel(tr("Type:"), aTypeWgt));
  myTypeLbl = new QLabel("", aTypeWgt);
  aTypeLayout->addWidget(myTypeLbl);

  myMainLayout->addWidget(aTypeWgt);

  myTypeParams = new QTextBrowser(myMainWidget);
  myTypeParams->setFixedWidth(170);
  myTypeParams->setReadOnly(true);
  myTypeParams->setFocusPolicy(Qt::NoFocus);
  myTypeParams->setFrameStyle(QFrame::NoFrame);
  myTypeParams->viewport()->setBackgroundRole(QPalette::Window);

  myMainLayout->addWidget(myTypeParams, 1);

  aScrollArea->setWidget(myMainWidget);

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
  setParamsText("");
}

//********************************************************************
void XGUI_InspectionPanel::onSelectionChanged()
{
  clearContent();
  XGUI_Selection* aSelection = mySelectionMgr->selection();
  QList<ModuleBase_ViewerPrsPtr> aSelectedList =
    aSelection->getSelected(ModuleBase_ISelection::Viewer);
  if (aSelectedList.count() > 0) {
    ModuleBase_ViewerPrsPtr aPrs = aSelectedList.first();
    TopoDS_Shape aShape = ModuleBase_Tools::getSelectedShape(aPrs);
    if (aShape.IsNull())
      return;
    setName(XGUI_Tools::generateName(aPrs));
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
  }
}

//********************************************************************
void XGUI_InspectionPanel::setShapeParams(const TopoDS_Shape& theShape)
{
  GeomShapePtr aShape(new GeomAPI_Shape);
  aShape->setImpl(new TopoDS_Shape(theShape));

  switch (aShape->shapeType()) {
  case GeomAPI_Shape::VERTEX:
    fillVertex(aShape->vertex());
    break;
  case GeomAPI_Shape::EDGE:
    fillEdge(aShape->edge());
    break;
  case GeomAPI_Shape::FACE:
    fillFace(aShape->face());
    break;
  case GeomAPI_Shape::SOLID:
    fillSolid(aShape->solid());
    break;
  case GeomAPI_Shape::WIRE:
    fillWire(aShape->wire());
    break;
  case GeomAPI_Shape::SHELL:
    fillShell(aShape->shell());
    break;
  case GeomAPI_Shape::COMPSOLID:
  case GeomAPI_Shape::COMPOUND:
    fillContainer(aShape);
    break;
  }
}

//********************************************************************
void XGUI_InspectionPanel::fillVertex(const GeomVertexPtr& theVertex)
{
  GeomPointPtr aPoint = theVertex->point();

  myTypeLbl->setText(tr("Vertex"));

  QString aParams;
  appendPointToParameters(tr("Coordinates"), aPoint, aParams);
  setParamsText(aParams);
}

//********************************************************************
void XGUI_InspectionPanel::fillEdge(const GeomEdgePtr& theEdge)
{
  QString aParams;
  if (theEdge->isDegenerated())
    appendNamedValueToParameters(tr("Degenerated"), true, aParams);

  GeomPointPtr aStartPnt = theEdge->firstPoint();
  GeomPointPtr aEndPnt = theEdge->lastPoint();
  bool addStartEndPoints = false;

  if (theEdge->isLine()) {
    myTypeLbl->setText(tr("Line segment"));
    addStartEndPoints = true;
  }
  else {
    GeomCirclePtr aCircle = theEdge->circle();
    if (aCircle) {
      addStartEndPoints = aStartPnt->distance(aEndPnt) >= Precision::Confusion();
      if (addStartEndPoints)
        myTypeLbl->setText("Arc of circle");
      else
        myTypeLbl->setText("Circle");

      appendPointToParameters(tr("Center"), aCircle->center(), aParams);
      appendDirToParameters(tr("Normal"), aCircle->normal(), aParams);
      appendGroupNameToParameters(tr("Dimensions"), aParams);
      appendNamedValueToParameters(tr("Radius"), aCircle->radius(), aParams);
    }
    else {
      GeomEllipsePtr anEllipse = theEdge->ellipse();
      if (anEllipse) {
        addStartEndPoints = aStartPnt->distance(aEndPnt) >= Precision::Confusion();
        if (addStartEndPoints)
          myTypeLbl->setText("Arc of ellipse");
        else
          myTypeLbl->setText("Ellipse");

        appendPointToParameters(tr("Center"), anEllipse->center(), aParams);
        appendDirToParameters(tr("Normal"), anEllipse->normal(), aParams);
        appendGroupNameToParameters(tr("Dimensions"), aParams);
        appendNamedValueToParameters(tr("Major radius"), anEllipse->majorRadius(), aParams);
        appendNamedValueToParameters(tr("Minor radius"), anEllipse->minorRadius(), aParams);
      }
      else
        // Common case
        myTypeLbl->setText(tr("Edge"));
    }
  }

  if (addStartEndPoints) {
    appendPointToParameters(tr("Start point"), aStartPnt, aParams);
    appendPointToParameters(tr("End point"), aEndPnt, aParams);
  }
  setParamsText(aParams);
}

//********************************************************************
void XGUI_InspectionPanel::fillWire(const GeomWirePtr& theWire)
{
  QString aParams;
  appendNamedValueToParameters(tr("Closed"), theWire->isClosed(), aParams);

  // check the wire is a polygon
  std::list<GeomPointPtr> aPolygonPoints;
  if (theWire->isPolygon(aPolygonPoints)) {
    myTypeLbl->setText(tr("Polygon"));
    int aCornerIndex = 0;
    for (std::list<GeomPointPtr>::const_iterator aPtIt = aPolygonPoints.begin();
         aPtIt != aPolygonPoints.end(); ++aPtIt)
       appendPointToParameters(tr("Point") + " " + QString::number(++aCornerIndex),
                               *aPtIt, aParams);
  }
  else
    myTypeLbl->setText(tr("Wire"));

  setParamsText(aParams);
}

//********************************************************************
void XGUI_InspectionPanel::fillFace(const GeomFacePtr& theFace)
{
  QString aParams;
  // 1. Plane and planar faces
  GeomPlanePtr aPlane = theFace->getPlane();
  if (aPlane) {
    bool isCommonCase = true;
    // Check face bounded by circle or ellipse
    std::list<GeomShapePtr> aSubs = theFace->subShapes(GeomAPI_Shape::EDGE);
    if (aSubs.size() == 1) {
      GeomEdgePtr anEdge = aSubs.front()->edge();
      if (anEdge->isCircle() || anEdge->isEllipse()) {
        fillEdge(anEdge);
        isCommonCase = false;
      }
    }
    else {
      // Check face bounded by a single wire which is rectangle
      aSubs = theFace->subShapes(GeomAPI_Shape::WIRE);
      if (aSubs.size() == 1) {
        GeomWirePtr aWire = aSubs.front()->wire();
        std::list<GeomPointPtr> aCorners;
        if (aWire->isRectangle(aCorners)) {
          GeomPointPtr aBaseCorner = aCorners.front();
          aCorners.pop_front();

          double aWidth = aBaseCorner->distance(aCorners.front());
          double aHeight = aBaseCorner->distance(aCorners.back());

          myTypeLbl->setText(tr("Rectangle"));
          appendPointToParameters(tr("Corner"), aBaseCorner, aParams);
          appendDirToParameters(tr("Normal"), aPlane->direction(), aParams);
          appendGroupNameToParameters(tr("Dimensions"), aParams);
          appendNamedValueToParameters(tr("Width"), aWidth, aParams);
          appendNamedValueToParameters(tr("Height"), aHeight, aParams);
          setParamsText(aParams);

          isCommonCase = false;
        }
      }
    }

    if (isCommonCase)
      setPlaneType(tr("Plane"), aPlane);
  }
  else {
    // 2. Sphere
    GeomSpherePtr aSphere = theFace->getSphere();
    if (aSphere)
      setSphereType(tr("Sphere"), aSphere);
    else {
      // 3. Cylinder
      GeomCylinderPtr aCylinder = theFace->getCylinder();
      if (aCylinder)
        setCylinderType(tr("Cylinder"), aCylinder);
      else {
        // 4. Cone
        GeomConePtr aCone = theFace->getCone();
        if (aCone)
          setConeType(tr("Cone"), aCone);
        else {
          // 5. Torus
          GeomTorusPtr aTorus = theFace->getTorus();
          if (aTorus)
            setTorusType(tr("Torus"), aTorus);
          else
            // 6. Common case
            myTypeLbl->setText(tr("Face"));
        }
      }
    }
  }
}

//********************************************************************
void XGUI_InspectionPanel::fillShell(const GeomShellPtr& theShell)
{
  // 1. Sphere
  GeomSpherePtr aSphere = theShell->getSphere();
  if (aSphere)
    setSphereType(tr("Sphere"), aSphere);
  else {
    // 2. Cylinder
    GeomCylinderPtr aCylinder = theShell->getCylinder();
    if (aCylinder)
      setCylinderType(tr("Cylinder"), aCylinder);
    else {
      // 3. Cone
      GeomConePtr aCone = theShell->getCone();
      if (aCone)
        setConeType(tr("Cone"), aCone);
      else {
        // 4. Torus
        GeomTorusPtr aTorus = theShell->getTorus();
        if (aTorus)
          setTorusType(tr("Torus"), aTorus);
        else {
          // 5. Axis-aligned/Rotated Box
          GeomBoxPtr aBox = theShell->getParallelepiped();
          if (aBox) {
            if (aBox->isAxesAligned())
              setBoxType(tr("Box"), aBox);
            else
              setRotatedBoxType(tr("Rotated Box"), aBox);
          }
          else
            // 6. Common case
            myTypeLbl->setText(tr("Shell"));
        }
      }
    }
  }
}

//********************************************************************
void XGUI_InspectionPanel::fillSolid(const GeomSolidPtr& theSolid)
{
  // 1. Sphere
  GeomSpherePtr aSphere = theSolid->getSphere();
  if (aSphere)
    setSphereType(tr("Sphere"), aSphere);
  else {
    // 2. Cylinder
    GeomCylinderPtr aCylinder = theSolid->getCylinder();
    if (aCylinder)
      setCylinderType(tr("Cylinder"), aCylinder);
    else {
      // 3. Cone
      GeomConePtr aCone = theSolid->getCone();
      if (aCone)
        setConeType(tr("Cone"), aCone);
      else {
        // 4. Torus
        GeomTorusPtr aTorus = theSolid->getTorus();
        if (aTorus)
          setTorusType(tr("Torus"), aTorus);
        else {
          // 5. Axis-aligned/Rotated Box
          GeomBoxPtr aBox = theSolid->getParallelepiped();
          if (aBox) {
            if (aBox->isAxesAligned())
              setBoxType(tr("Box"), aBox);
            else
              setRotatedBoxType(tr("Rotated Box"), aBox);
          }
          else
            // 6. Common case
            myTypeLbl->setText(tr("Solid"));
        }
      }
    }
  }
}

//********************************************************************
void XGUI_InspectionPanel::fillContainer(const GeomShapePtr& theShape)
{
  if (theShape->shapeType() == GeomAPI_Shape::COMPSOLID)
    myTypeLbl->setText("CompSolid");
  else if (theShape->shapeType() == GeomAPI_Shape::COMPOUND)
    myTypeLbl->setText("Compound");

  // fill bounding box
  Bnd_Box aBB;
  BRepBndLib::Add(theShape->impl<TopoDS_Shape>(), aBB);

  gp_Pnt aMinPnt = aBB.CornerMin();
  GeomPointPtr aMinPoint(new GeomAPI_Pnt(aMinPnt.X(), aMinPnt.Y(), aMinPnt.Z()));

  gp_Pnt aMaxPnt = aBB.CornerMax();
  GeomPointPtr aMaxPoint(new GeomAPI_Pnt(aMaxPnt.X(), aMaxPnt.Y(), aMaxPnt.Z()));

  QString aParams;
  appendGroupNameToParameters(tr("Bounding box"), aParams);
  appendPointToParameters(tr("Minimal corner"), aMinPoint, aParams);
  appendPointToParameters(tr("Maximal corner"), aMaxPoint, aParams);
}

void XGUI_InspectionPanel::setPlaneType(const QString& theTitle,
                                        const std::shared_ptr<GeomAPI_Pln>& thePlane)
{
  myTypeLbl->setText(theTitle);
  QString aParams;
  appendPointToParameters(tr("Origin"), thePlane->location(), aParams);
  appendDirToParameters(tr("Normal"), thePlane->direction(), aParams);
  setParamsText(aParams);
}

void XGUI_InspectionPanel::setSphereType(const QString& theTitle,
                                         const std::shared_ptr<GeomAPI_Sphere>& theSphere)
{
  myTypeLbl->setText(theTitle);
  QString aParams;
  appendPointToParameters(tr("Center"), theSphere->center(), aParams);
  appendGroupNameToParameters(tr("Dimensions"), aParams);
  appendNamedValueToParameters(tr("Radius"), theSphere->radius(), aParams);
  setParamsText(aParams);
}

void XGUI_InspectionPanel::setCylinderType(const QString& theTitle,
                                           const std::shared_ptr<GeomAPI_Cylinder>& theCyl)
{
  myTypeLbl->setText(theTitle);
  QString aParams;
  appendPointToParameters(tr("Position"), theCyl->location(), aParams);
  appendDirToParameters(tr("Axis"), theCyl->axis(), aParams);
  appendGroupNameToParameters(tr("Dimensions"), aParams);
  appendNamedValueToParameters(tr("Radius"), theCyl->radius(), aParams);
  appendNamedValueToParameters(tr("Height"), theCyl->height(), aParams);
  setParamsText(aParams);
}

void XGUI_InspectionPanel::setConeType(const QString& theTitle,
                                       const std::shared_ptr<GeomAPI_Cone>& theCone)
{
  myTypeLbl->setText(theTitle);
  QString aParams;
  appendPointToParameters(tr("Position"), theCone->location(), aParams);
  appendDirToParameters(tr("Axis"), theCone->axis(), aParams);
  appendGroupNameToParameters(tr("Dimensions"), aParams);
  appendNamedValueToParameters(tr("Radius 1"), theCone->radius1(), aParams);
  appendNamedValueToParameters(tr("Radius 2"), theCone->radius2(), aParams);
  appendNamedValueToParameters(tr("Height"), theCone->height(), aParams);
  setParamsText(aParams);
}

void XGUI_InspectionPanel::setTorusType(const QString& theTitle,
                                        const std::shared_ptr<GeomAPI_Torus>& theTorus)
{
  myTypeLbl->setText(theTitle);
  QString aParams;
  appendPointToParameters(tr("Center"), theTorus->center(), aParams);
  appendDirToParameters(tr("Axis"), theTorus->direction(), aParams);
  appendGroupNameToParameters(tr("Dimensions"), aParams);
  appendNamedValueToParameters(tr("Major radius"), theTorus->majorRadius(), aParams);
  appendNamedValueToParameters(tr("Minor radius"), theTorus->minorRadius(), aParams);
  setParamsText(aParams);
}

void XGUI_InspectionPanel::setBoxType(const QString& theTitle,
                                      const std::shared_ptr<GeomAPI_Box>& theBox)
{
  myTypeLbl->setText(theTitle);
  QString aParams;
  appendPointToParameters(tr("Position"), theBox->axes()->origin(), aParams);
  appendGroupNameToParameters(tr("Dimensions"), aParams);
  appendNamedValueToParameters(tr("Width"), theBox->width(), aParams);
  appendNamedValueToParameters(tr("Depth"), theBox->depth(), aParams);
  appendNamedValueToParameters(tr("Height"), theBox->height(), aParams);
  setParamsText(aParams);
}

void XGUI_InspectionPanel::setRotatedBoxType(const QString& theTitle,
                                             const std::shared_ptr<GeomAPI_Box>& theBox)
{
  myTypeLbl->setText(theTitle);
  QString aParams;
  std::shared_ptr<GeomAPI_Ax3> anAxes = theBox->axes();
  appendPointToParameters(tr("Position"), anAxes->origin(), aParams);
  appendDirToParameters(tr("Z axis"), anAxes->normal(), aParams);
  appendDirToParameters(tr("X axis"), anAxes->dirX(), aParams);
  appendGroupNameToParameters(tr("Dimensions"), aParams);
  appendNamedValueToParameters(tr("Width"), theBox->width(), aParams);
  appendNamedValueToParameters(tr("Depth"), theBox->depth(), aParams);
  appendNamedValueToParameters(tr("Height"), theBox->height(), aParams);
  setParamsText(aParams);
}


void XGUI_InspectionPanel::setParamsText(const QString& theText)
{
  myTypeParams->setText(theText);
}

void XGUI_InspectionPanel::resizeEvent(QResizeEvent* theEvent)
{
  QSize aSize = theEvent->size();

  int aHeight = aSize.height();

  if (aHeight > 450) // 450 is a a minimal height
    myMainWidget->setFixedHeight(aHeight - 30);
}