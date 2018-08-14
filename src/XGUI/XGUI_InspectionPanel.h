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

#include <memory>

class XGUI_SelectionMgr;
class QLineEdit;
class QTableWidget;
class QLabel;
class QTextBrowser;

class TopoDS_Shape;

class GeomAPI_Vertex;
class GeomAPI_Edge;
class GeomAPI_Wire;
class GeomAPI_Face;
class GeomAPI_Shell;
class GeomAPI_Solid;
class GeomAPI_Shape;

class GeomAPI_Pln;
class GeomAPI_Sphere;
class GeomAPI_Cylinder;
class GeomAPI_Cone;
class GeomAPI_Torus;
class GeomAPI_Box;

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

private slots:
  void onSelectionChanged();

private:
  void setSubShapeValue(SudShape theId, int theVal);

  void setName(const QString& theName);

  void setShapeContent(const TopoDS_Shape& theShape);

  void setShapeParams(const TopoDS_Shape& theShape);

  void clearContent();


  void fillVertex(const std::shared_ptr<GeomAPI_Vertex>& theVertex);

  void fillEdge(const std::shared_ptr<GeomAPI_Edge>& theEdge);

  void fillWire(const std::shared_ptr<GeomAPI_Wire>& theWire);

  void fillFace(const std::shared_ptr<GeomAPI_Face>& theFace);

  void fillShell(const std::shared_ptr<GeomAPI_Shell>& theShell);

  void fillSolid(const std::shared_ptr<GeomAPI_Solid>& theSolid);

  void fillContainer(const std::shared_ptr<GeomAPI_Shape>& theShape);


  void setPlaneType(const QString& theTitle, const std::shared_ptr<GeomAPI_Pln>& thePlane);

  void setSphereType(const QString& theTitle, const std::shared_ptr<GeomAPI_Sphere>& theSphere);

  void setCylinderType(const QString& theTitle, const std::shared_ptr<GeomAPI_Cylinder>& theCyl);

  void setConeType(const QString& theTitle, const std::shared_ptr<GeomAPI_Cone>& theCone);

  void setTorusType(const QString& theTitle, const std::shared_ptr<GeomAPI_Torus>& theTorus);

  void setBoxType(const QString& theTitle, const std::shared_ptr<GeomAPI_Box>& theBox);

  void setRotatedBoxType(const QString& theTitle, const std::shared_ptr<GeomAPI_Box>& theBox);

private:
  XGUI_SelectionMgr* mySelectionMgr;

  QLineEdit* myNameEdt;
  QTableWidget* mySubShapesTab;
  QLabel* myTypeLbl;
  QTextBrowser* myTypeParams;
};

#endif