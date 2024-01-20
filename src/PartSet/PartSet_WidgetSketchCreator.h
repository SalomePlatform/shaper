// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef PartSet_WidgetSketchCreator_H
#define PartSet_WidgetSketchCreator_H

#include "PartSet.h"

#include <ModuleBase_WidgetSelector.h>

class QLabel;
class QLineEdit;
class PartSet_Module;
class ModuleBase_Operation;
class ModuleBase_IWorkshop;
class PartSet_PreviewPlanes;
class ModuleBase_ChoiceCtrl;

/**
* \ingroup Modules
* A widget which allow creation of a sketch in another operation.
* It creates sketch on aqctivation of this widget. If sketch object is already created then
* it will transfer a focus to next widget. It is supposed that the widget will be placed as 
* a first widget in property panel
*/
class PARTSET_EXPORT PartSet_WidgetSketchCreator : public ModuleBase_WidgetSelector
{
Q_OBJECT

public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theModule a reference to a module object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  PartSet_WidgetSketchCreator(QWidget* theParent, PartSet_Module* theModule,
                              const Config_WidgetAPI* theData);

  virtual ~PartSet_WidgetSketchCreator();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Set focus to the first control of the current widget.
  /// The focus policy of the control is checked.
  /// If the widget has the NonFocus focus policy, it is skipped.
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValues the wrapped selection values
  /// \param theToValidate a validation of the values flag
  virtual bool setSelection(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                            const bool theToValidate);

  /// Editing mode depends on mode of current operation. This value is defined by it.
  virtual void setEditingMode(bool isEditing);

  /// Checks all widget validator if the owner is valid. Firstly it checks custom widget validating,
  /// next, the attribute's validating. It trying on the give selection to current attribute by
  /// setting the value inside and calling validators. After this, the previous attribute value is
  /// restored.The valid/invalid value is cashed.
  /// \param theValue a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

  /// Returns True in case if the widget contains useful information for inspection tool
  virtual bool isInformative() const { return false; }

protected:
  /// If there is no operation in current session, start operation for modify parameters
  /// \return true if the operation was not opened
  void openExtrusionTransaction();

  /// Checks whether the selection presentation contains preview planes
  /// \param theValue a selection information
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  virtual bool restoreValueCustom();

  /// Retunrs attribute, which should be validated. In default implementation,
  /// this is an attribute of ID
  /// \return an attribute
  virtual AttributePtr attribute() const;

  /// Sets the selection control visible and set the current widget as active in property panel
  /// It leads to connect to onSelectionChanged slot
  void activateSelectionControl();

  /// Visualization of the current control or others in PP
  /// \param theSelectionControl state whether the control should be shown/hidden
  void setVisibleSelectionControl(const bool theSelectionControl);

  /// Retunrs a list of possible shape types
  /// \return a list of shapes
  virtual QIntList shapeTypes() const;

  /// Emits model changed info, updates the current control by selection change
  /// \param theDone a state whether the selection is set
  void updateOnSelectionChanged(const bool theDone);

protected:
  /// Returns true if envent is processed.
  virtual bool processSelection();

private:
  /// Returns true if the selection mode is active. This is when composition feature has no
  ///  a sub-object and the attribute list is empty
  /// \return boolean value
  bool isSelectionMode() const;

  /// Returns true if the current composite feature contains at least one sub-object
  /// \return boolean value
  bool hasSubObjects() const;

private slots:
  void onResumed(ModuleBase_Operation* theOp);

  /// Slot is called on selection type changed
  void onSelectionTypeChanged();

private:
  /// Append new Sketch, set the selected plane for the sketch and start Edit operation.
  /// \param theValues a selection list
  /// \return true if the sketch is started
  bool startSketchOperation(const QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues);

  /// Sets the sub-object to list of base.
  static void setSketchObjectToList(const CompositeFeaturePtr& theFeature,
                                    const AttributePtr& theAttribute);

  /// Calls validator for attribute list and warn if it is not valid
  /// \return true if validation succeed
  bool validateSelectionList() const;

  /// Change enable state of controls in the model widget by the attribute identifier
  /// \param theModelWidget a model widget
  /// \param theEnabled a state if the controls should be enabled/disabled
  void setEnabledModelWidget(ModuleBase_ModelWidget* theModelWidget,
                             const bool theEnabled);

private:
  std::string myAttributeListID;

  PartSet_Module* myModule;

  /// Flag if the attribute() should return the attribute list ID or XML attribute
  /// to store selection and check validation
  bool myIsCustomAttribute;

  QWidget* mySizeOfViewWidget;///< Size of view widget, visualized if preview planes are shown
  QLineEdit* mySizeOfView; ///< Value of square of size of View
  /// Label of the widget
  QLabel* myLabel;

  /// List of accepting shapes types
  QStringList myShapeTypes;

  /// class to show/hide preview planes
  PartSet_PreviewPlanes* myPreviewPlanes;

  bool myIsUseChoice; ///< A flag to store use_choice parameter state

  /// Control for types
  ModuleBase_ChoiceCtrl* myTypeCtrl;
  std::string myDefMode;
};

#endif