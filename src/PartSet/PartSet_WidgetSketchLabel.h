// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef PartSet_WidgetSketchLabel_H
#define PartSet_WidgetSketchLabel_H

#include "PartSet.h"

#include "PartSet_Tools.h"

#include <ModuleBase_WidgetValidated.h>
#include <ModuleBase_ViewerFilters.h>

#include <GeomAPI_Dir.h>

#include <TopoDS_Shape.hxx>

#include <QStackedWidget>
#include <QMap>

class PartSet_PreviewPlanes;

class QLabel;
class XGUI_OperationMgr;
class XGUI_Workshop;
class QCheckBox;
class QStackedWidget;
class QLineEdit;
class QPushButton;
class QDialog;

/**
* \ingroup Modules
* A model widget implementation for a label which provides specific behaviour 
* for sketcher starting and launching operations
*/
class PARTSET_EXPORT PartSet_WidgetSketchLabel : public ModuleBase_WidgetValidated
{
Q_OBJECT

public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop a reference to workshop
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param toShowConstraints a current show constraints state
  PartSet_WidgetSketchLabel(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                      const Config_WidgetAPI* theData,
                      const QMap<PartSet_Tools::ConstraintVisibleState, bool>& toShowConstraints);

  virtual ~PartSet_WidgetSketchLabel();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// The method is called by the current operation to process the operation preselection.
  /// It is redefined to do nothing if the plane of the sketch has been already set.
  /// \param theValues the wrapped selection values
  /// \param theToValidate a validation flag
  virtual bool setSelection(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                            const bool theToValidate);

  /// Fills given container with selection modes if the widget has it
  /// \param [out] theModuleSelectionModes module additional modes, -1 means all default modes
  /// \param theModes [out] a container of modes
  virtual void selectionModes(int& theModuleSelectionModes, QIntList& theModes);

  /// Using widget selection filter only if plane is not defined.
  /// \param [out] theModuleSelectionFilters module additional modes, -1 means all default modes
  /// \param [out] selection filters
  virtual void selectionFilters(QIntList& theModuleSelectionFilters,
                                SelectMgr_ListOfFilter& theSelectionFilters);

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// The method called if widget should be activated always
  virtual bool needToBeActivated() { return true; }

  /// Returns sketcher plane
  std::shared_ptr<GeomAPI_Pln> plane() const;

  /// This control accepts focus
  virtual bool focusTo();
  virtual void setHighlighted(bool) { /*do nothing*/ };
  virtual void enableFocusProcessing();

  /// Set current state of show free points
  /// \param theState a state of the corresponded check box
  void setShowPointsState(bool theState);

  /// Returns True if the selected presentation can be used for plane definition
  /// \param thePrs a presentation
  static bool canFillSketch(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /// If widgets has several panels then this method has to show a page which contains information
  /// for current feature. By default does nothing
  virtual void showInformativePage() {
    if (myStackWidget) myStackWidget->setCurrentIndex(1);
  }

signals:
  /// Signal on plane selection
  void planeSelected(const std::shared_ptr<GeomAPI_Pln>& thePln);

  /// A show constraint toggled signal
  /// \param theType a ConstraintVisibleState value
  /// \param theState a state of the check box
  void showConstraintToggled(int theType, bool theState);

  /// The signal is emitted when user checks "Show free points" button
  /// \param toShow a state of the check box
  void showFreePoints(bool toShow);

  /// The signal is emitted when user checks "Automatic constraints" button
  /// \param isOn a state of the check box
  void autoConstraints(bool isOn);

protected:
  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theAttribute an attribute to be stored
  virtual void storeAttributeValue(const AttributePtr& theAttribute);

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theAttribute an attribute to be restored
  /// \param theValid a boolean flag, if restore happens for valid parameters
  virtual void restoreAttributeValue(const AttributePtr& theAttribute,
                                     const bool theValid);

  /// Fills the attribute with the value of the selected owner
  /// \param thePrs a selected owner
  virtual bool setSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom()
  {
    return true;
  }

  virtual bool restoreValueCustom();

  /// The methiod called when widget is activated
  virtual void activateCustom();

  /// Block the model flush of update and intialization of attribute
  /// In additional to curstom realization it blocks initialization for all feature attributes
  /// as the current attribute is selection but its modification leads to other attributes change
  /// \param theAttribute an attribute
  /// \param theToBlock flag whether the model is blocked or unblocked
  /// \param isFlushesActived out value if model is blocked, in value if model is unblocked
  /// to be used to restore flush state when unblocked
  /// \param isAttributeSetInitializedBlocked out value if model is blocked
  /// in value if model is unblocked to be used to restore previous state when unblocked
  /// \param isAttributeSendUpdatedBlocked out value if model signal is blocked
  virtual void blockAttribute(const AttributePtr& theAttribute, const bool& theToBlock,
                              bool& isFlushesActived,
                              bool& isAttributeSetInitializedBlocked,
                              bool& isAttributeSendUpdatedBlocked);

  /// Returns true if envent is processed.
  virtual bool processSelection();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// The method is called by the current operation to process the operation preselection.
  /// It is redefined to do nothing if the plane of the sketch has been already set.
  /// \param theValues the wrapped selection values
  /// \param theToValidate a validation flag
  bool setSelectionInternal(const QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                            const bool theToValidate);

  /// Erase preview planes, disconnect widget, change the view projection
  /// \param thePrs a selected presentation
  void updateByPlaneSelected(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /// Set sketch plane from selected object
  /// \param theFeature a feature of sketch
  /// \param thePrs a presentation
  bool fillSketchPlaneBySelection(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /// Redefinition of a virtual function
  virtual void showEvent(QShowEvent* theEvent);

  /// Redefinition of a virtual function
  virtual void hideEvent(QHideEvent* theEvent);

  /// Redefinition of a virtual function
  virtual bool eventFilter(QObject* theObj, QEvent* theEvent);

private slots:
  /// A slot called on set sketch plane view
  void onSetPlaneView();

  /// Emits signal about check box state changed with information about ConstraintVisibleState
  /// \param theOn a flag show constraints or not
  void onShowConstraint(bool theOn);

  void onChangePlane();

  void onShowDOF();

  void onShowPanel();

private:
  /// Set sketch plane by shape
  /// \param theShape a planar face
  std::shared_ptr<GeomAPI_Dir> setSketchPlane(const TopoDS_Shape& theShape);

  /// Set sketch plane
  /// \param thePlane a plane
  std::shared_ptr<GeomAPI_Dir> setSketchPlane(std::shared_ptr<GeomAPI_Pln> thePlane);

  /**
  * Returns list of presentations which have displayed shapes with circular edges
  * (circles, arcs) which are in pane of of the given sketch
  * \param theSketch - the sketch
  */
  QList<std::shared_ptr<ModuleBase_ViewerPrs>> findCircularEdgesInPlane();

private:
  /// class to show/hide preview planes
  PartSet_PreviewPlanes* myPreviewPlanes;

  QCheckBox* myViewInverted;
  QCheckBox* myRemoveExternal;
  QCheckBox* myShowPoints;
  QCheckBox* myAutoConstraints;

  QMap<PartSet_Tools::ConstraintVisibleState, QCheckBox*> myShowConstraints;

  QWidget* mySizeOfViewWidget; ///< Size of view widget, visualized if preview planes are shown
  QLineEdit* mySizeOfView; ///< Value of square of size of View
  QStackedWidget* myStackWidget;

  QLabel* myDoFLabel;
  QPushButton* myShowDOFBtn;

  bool myOpenTransaction;
  bool myIsSelection;

  QDialog* myPartSetMessage;
  QDialog* mySizeMessage;

  GeomPlanePtr myTmpPlane;
};

#endif
