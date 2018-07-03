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

#include <QLayout>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QTextBrowser>

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
    QTableWidgetItem* aItem = new QTableWidgetItem("0");
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
  myTypeLbl = new QLabel("No-type", aTypeWgt);
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

  // Test
  setVertexType(0, 0, 0);
}

XGUI_InspectionPanel::~XGUI_InspectionPanel()
{
}

void XGUI_InspectionPanel::setSubShapeValue(SudShape theId, int theVal)
{
  mySubShapesTab->item(theId, 1)->setText(QString::number(theVal));
}

void XGUI_InspectionPanel::onSelectionChanged()
{
  QObjectPtrList aObjects = mySelectionMgr->selection()->selectedObjects();
  if (aObjects.count() > 0) {
    ObjectPtr aObj = aObjects.first();
    setName(aObj->data()->name().c_str());
  }
}

void XGUI_InspectionPanel::setName(const QString& theName)
{
  myNameEdt->setText(theName);
}

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

void XGUI_InspectionPanel::setVertexType(double theX, double theY, double theZ)
{
  myTypeLbl->setText(tr("Vertex"));
  QString aParams = TITLE(tr("Coordinates")) +
    "<br> X: " + QString::number(theX) +
    "<br> Y: " + QString::number(theY) +
    "<br> Z: " + QString::number(theZ);
  myTypeParams->setText(aParams);
}