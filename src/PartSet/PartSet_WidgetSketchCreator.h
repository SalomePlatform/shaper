// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSketchCreator.h
// Created:     08 June 2015
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_WidgetSketchCreator_H
#define PartSet_WidgetSketchCreator_H

#include "PartSet.h"

#include <ModuleBase_WidgetSelector.h>

class QLabel;
class QLineEdit;
class PartSet_Module;
class ModelAPI_Tools;
class ModuleBase_Operation;
class ModuleBase_IWorkshop;
class PartSet_PreviewPlanes;

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

  /// Set focus to the first control of the current widget. The focus policy of the control is checked.
  /// If the widget has the NonFocus focus policy, it is skipped.
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValues the wrapped selection values
  /// \param theToValidate a validation of the values flag
  virtual bool setSelection(QList<ModuleBase_ViewerPrs>& theValues,
                            const bool theToValidate);

  /// Editing mode depends on mode of current operation. This value is defined by it.
  virtual void setEditingMode(bool isEditing);

  /// Checks all widget validator if the owner is valid. Firstly it checks custom widget validating,
  /// next, the attribute's validating. It trying on the give selection to current attribute by
  /// setting the value inside and calling validators. After this, the previous attribute value is
  /// restored.The valid/invalid value is cashed.
  /// \param theValue a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelection(const ModuleBase_ViewerPrs& theValue);

  /// Check if the current and the parent operations are a composite. If the parent operation contains
  /// attribute selection list, the method returns false if it is invalid in this attibute validator
  /// \param theWorkshop a current workshop
  /// \return boolean value
  static bool canCommitCurrentSketch(ModuleBase_IWorkshop* theWorkshop);

  /// Sets the sub-object to list of base.
  static void setSketchObjectToList(const CompositeFeaturePtr& theFeature,
                                    const AttributePtr& theAttribute);

protected:
  /// If there is no operation in current session, start operation for modify parameters
  void openTransaction();

  /// Checks whether the selection presentation contains preview planes
  /// \param theValue a selection information
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const ModuleBase_ViewerPrs& theValue);

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

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
  virtual QIntList getShapeTypes() const;

  /// Emits model changed info, updates the current control by selection change
  /// \param theDone a state whether the selection is set
  void updateOnSelectionChanged(const bool theDone);

protected slots:
  /// Slot which is called on selection event
  virtual void onSelectionChanged();

private:
  /// Returns true if the selection mode is active. This is when composition feature has no
  ///  a sub-object and the attribute list is empty
  /// \return boolean value
  bool isSelectionMode() const;

private slots:
  void onResumed(ModuleBase_Operation* theOp);

private:
  /// Append new Sketch, set the selected plane for the sketch and start Edit operation.
  /// \param theValues a selection list
  /// \return true if the sketch is started
  bool startSketchOperation(const QList<ModuleBase_ViewerPrs>& theValues);

private:
  std::string myAttributeListID;

  PartSet_Module* myModule;

  /// Flag if the attribute() should return the attribute list ID or XML attribute
  /// to store selection and check validation
  bool myIsCustomAttribute;

  /// Label of the widget
  QLabel* myLabel;

  /// Input control of the widget
  QLineEdit* myTextLine;

  /// List of accepting shapes types
  QStringList myShapeTypes;

  /// class to show/hide preview planes
  PartSet_PreviewPlanes* myPreviewPlanes;
};

#endif