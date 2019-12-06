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
#include <QSplitter>
#include <QStackedWidget>

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
      myStackWgt->setCurrentIndex(myShapePanelId);
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
      if (aShape.IsNull())
        return;
      GeomShapePtr aShapePtr(new GeomAPI_Shape());
      aShapePtr->setImpl(new TopoDS_Shape(aShape));

      ModuleBase_ViewerPrsPtr aPrsCopy(new ModuleBase_ViewerPrs(aPrs->object(), aShapePtr));
      setName(XGUI_Tools::generateName(aPrsCopy));
      setShapeContent(aShape);
      setShapeParams(aShape);
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
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  Bnd_Box aBB;
  BRepBndLib::Add(aShape, aBB);

  if (aBB.IsVoid())
    return;

  gp_Pnt aMinPnt = aBB.CornerMin();
  GeomPointPtr aMinPoint(new GeomAPI_Pnt(aMinPnt.X(), aMinPnt.Y(), aMinPnt.Z()));

  gp_Pnt aMaxPnt = aBB.CornerMax();
  GeomPointPtr aMaxPoint(new GeomAPI_Pnt(aMaxPnt.X(), aMaxPnt.Y(), aMaxPnt.Z()));

  QString aParams;
  appendGroupNameToParameters(tr("Bounding box"), aParams);
  appendPointToParameters(tr("Minimal corner"), aMinPoint, aParams);
  appendPointToParameters(tr("Maximal corner"), aMaxPoint, aParams);
}

//********************************************************************
void XGUI_InspectionPanel::setPlaneType(const QString& theTitle,
                                        const std::shared_ptr<GeomAPI_Pln>& thePlane)
{
  myTypeLbl->setText(theTitle);
  QString aParams;
  appendPointToParameters(tr("Origin"), thePlane->location(), aParams);
  appendDirToParameters(tr("Normal"), thePlane->direction(), aParams);
  setParamsText(aParams);
}

//********************************************************************
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

//********************************************************************
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

//********************************************************************
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

//********************************************************************
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

//********************************************************************
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

//********************************************************************
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
