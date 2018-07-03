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

#ifndef XGUI_INSPECTIONPANEL_H_
#define XGUI_INSPECTIONPANEL_H_

#include "XGUI.h"

#include <QDockWidget>

class XGUI_SelectionMgr;
class QLineEdit;
class QTableWidget;
class QLabel;
class QTextBrowser;

/// Internal name of property panel widget
const static char* INSPECTION_PANEL = "inspection_panel_dock";

class XGUI_EXPORT XGUI_InspectionPanel : public QDockWidget
{
  Q_OBJECT
public:
  enum SudShape {
    ShapeId,
    CompoundId,
    CompsolidId,
    SolidId,
    ShellId,
    FaceId,
    WireId,
    EdgeId,
    VertexId
  };

  /// Constructor
  /// \param theParent is a parent of the property panel
  /// \param theMgr operation manager
  XGUI_InspectionPanel(QWidget* theParent, XGUI_SelectionMgr* theMgr);

  virtual ~XGUI_InspectionPanel();

  void setSubShapeValue(SudShape theId, int theVal);

  void setName(const QString& theName);

  // Set of type parameters
  void setCylinderType(double theX, double theY, double theZ,
    double theDX, double theDY, double theDZ, double theRadius, double theHeight);

  void setSphereType(double theX, double theY, double theZ, double theRadius);

  void setBoxType(double theX, double theY, double theZ,
    double theXsize, double theYsize, double theZsize);

  void setRotatedBoxType(double theX, double theY, double theZ,
    double theZaxisX, double theZaxisY, double theZaxisZ,
    double theXaxisX, double theXaxisY, double theXaxisZ,
    double theXsize, double theYsize, double theZsize);

  void setPlaneType(double theX, double theY, double theZ,
    double theDX, double theDY, double theDZ);

  void setVertexType(double theX, double theY, double theZ);

private slots:
  void onSelectionChanged();

private:
  XGUI_SelectionMgr* mySelectionMgr;

  QLineEdit* myNameEdt;
  QTableWidget* mySubShapesTab;
  QLabel* myTypeLbl;
  QTextBrowser* myTypeParams;
};

#endif