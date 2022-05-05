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

#ifndef XGUI_INSPECTIONPANEL_H_
#define XGUI_INSPECTIONPANEL_H_

#include "XGUI.h"

#include <ModelAPI_Feature.h>

#include <QDockWidget>

#include <memory>

class XGUI_Workshop;
class QLineEdit;
class QTableWidget;
class QLabel;
class QTextBrowser;
class QVBoxLayout;
class QResizeEvent;
class QStackedWidget;
class QGridLayout;

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
class ModuleBase_PageWidget;

/// Internal name of property panel widget
MAYBE_UNUSED const static char* INSPECTION_PANEL = "inspection_panel_dock";

/**
* \ingroup GUI
* A class which represents an inspection panel: to show content of currently selected objects
*/
class XGUI_EXPORT XGUI_InspectionPanel : public QDockWidget
{
  Q_OBJECT
public:
  /// Type of selected objects
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
  XGUI_InspectionPanel(QWidget* theParent, XGUI_Workshop* theWorkshop);

  // Destructor
  virtual ~XGUI_InspectionPanel();

protected:
  virtual void showEvent(QShowEvent* theEvent);

private slots:
  /// A slot to react on selection changed
  void onSelectionChanged();

private:
  /// Set counts of a sub-shapes
  /// \param theId an id of sub-shape type
  /// \param theVal a number of sub-shapes of corresponded type
  void setSubShapeValue(SudShape theId, int theVal);

  /// Set name of current selection
  /// \param theName the name
  void setName(const QString& theName);

  /// Set content of selected shape into table
  /// \param theShape the shape
  void setShapeContent(const TopoDS_Shape& theShape);

  /// Set parameters of the selected shape
  /// \param theShape the shape
  void setShapeParams(const TopoDS_Shape& theShape);

  /// Clear content of the window
  void clearContent();

  /// Show parameters of a vertex
  /// \param theVertex the vertex
  void fillVertex(const std::shared_ptr<GeomAPI_Vertex>& theVertex);

  /// Show parameters of a edge
  /// \param theEdge the edge
  void fillEdge(const std::shared_ptr<GeomAPI_Edge>& theEdge);

  /// Show parameters of a wire
  /// \param theWire the wire
  void fillWire(const std::shared_ptr<GeomAPI_Wire>& theWire);

  /// Show parameters of a face
  /// \param theFace the face
  void fillFace(const std::shared_ptr<GeomAPI_Face>& theFace);

  /// Show parameters of a shell
  /// \param theShell the shell
  void fillShell(const std::shared_ptr<GeomAPI_Shell>& theShell);

  /// Show parameters of a solid
  /// \param theSolid the solid
  void fillSolid(const std::shared_ptr<GeomAPI_Solid>& theSolid);

  /// Show parameters of a compound
  /// \param theShape the compound
  void fillContainer(const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Show parameters of a plane
  /// \param theTitle a title of the object
  /// \param thePlane the plane
  void setPlaneType(const QString& theTitle, const std::shared_ptr<GeomAPI_Pln>& thePlane);

  /// Show parameters of a sphere
  /// \param theTitle a title of the object
  /// \param theSphere the sphere
  void setSphereType(const QString& theTitle, const std::shared_ptr<GeomAPI_Sphere>& theSphere);

  /// Show parameters of a cylinder
  /// \param theTitle a title of the object
  /// \param theCyl the cylinder
  void setCylinderType(const QString& theTitle, const std::shared_ptr<GeomAPI_Cylinder>& theCyl);

  /// Show parameters of a cone
  /// \param theTitle a title of the object
  /// \param theCone the cone
  void setConeType(const QString& theTitle, const std::shared_ptr<GeomAPI_Cone>& theCone);

  /// Show parameters of a torus
  /// \param theTitle a title of the object
  /// \param theTorus the torus
  void setTorusType(const QString& theTitle, const std::shared_ptr<GeomAPI_Torus>& theTorus);

  /// Show parameters of a box
  /// \param theTitle a title of the object
  /// \param theBox the box
  void setBoxType(const QString& theTitle, const std::shared_ptr<GeomAPI_Box>& theBox);

  /// Show parameters of a rotated box
  /// \param theTitle a title of the object
  /// \param theBox the box
  void setRotatedBoxType(const QString& theTitle, const std::shared_ptr<GeomAPI_Box>& theBox);

  /// Set text into parameters area
  /// \param theText the text
  void setParamsText(const QString& theText);

  /// Fills Feature panel with controls specific to the given feature
  /// \param theFeature the selected feature
  void buildFeaturePane(const FeaturePtr& theFeature);

private:
  XGUI_Workshop* myWorkshop; //> selection manager

  QLineEdit* myNameEdt; ///> Name field
  QTableWidget* mySubShapesTab; ///> table of sub-shapes
  QLabel* myTypeLbl; ///> label of a type
  QTextBrowser* myTypeParams; ///> parameters area
  QVBoxLayout* myMainLayout; ///> main layout
  ModuleBase_PageWidget* myFeaturePane; ///> Content of feature property panel
  QGridLayout* myFeatureLayout; ///> Layout of feature panel
  QStackedWidget* myStackWgt; ///> base widget of the panel
  int myShapePanelId;
  int myFeaturePanelId;
};

#endif