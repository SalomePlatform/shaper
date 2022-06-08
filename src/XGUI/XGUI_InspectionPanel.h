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
#include <GeomAlgoAPI_ShapeInfo.h>

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

class ModuleBase_PageWidget;

/// Internal name of property panel widget
MAYBE_UNUSED const static char* INSPECTION_PANEL = "inspection_panel_dock";

/**
* \ingroup GUI
* A class which represents an inspection panel: to show content of currently selected objects
*/
class XGUI_EXPORT XGUI_InspectionPanel :
  public QDockWidget, public GeomAlgoAPI_ShapeInfo::Translator
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

  // A translator of resource strings, needed for ShapeInfo.
  virtual std::string translate(const char* theSource) override;


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

  /// Clear content of the window
  void clearContent();

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