// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetFeatureSelector.h
// Created:     5 Sep 2016
// Author:      Natalia Ermolaeva

#ifndef ModuleBase_WidgetFeatureSelector_H
#define ModuleBase_WidgetFeatureSelector_H

#include "ModuleBase.h"
#include "ModuleBase_WidgetSelector.h"

class Config_WidgetAPI;
class QWidget;
class QLabel;
class QLineEdit;

/**
* \ingroup GUI
* Implementation of widget for feature selection.
* This type of widget can be defined in XML file with 'feature_selector' keyword.
* For example:
* \code
*   <feature_selector id="main_object" 
*    label="Main object" 
*    tooltip="Select an object solid"
*  />
* \endcode
* It can use following parameters:
* - id - name of object attribute
* - label - content of widget's label
* - tooltip - the witget's tooltip text
*/
class MODULEBASE_EXPORT ModuleBase_WidgetFeatureSelector : public ModuleBase_WidgetValidated
{
Q_OBJECT
 public:

  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetFeatureSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                                   const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetFeatureSelector();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// The method is called by the current operation to process the operation preselection.
  /// It is redefined to check the value validity and if it is, fill the attribute with by value
  /// \param theValues the wrapped selection values
  /// \param theToValidate a flag on validation of the values
  virtual bool setSelection(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                            const bool theToValidate);

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Fills the attribute with the value of the selected owner
  /// \param thePrs a selected owner
  virtual bool setSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /// The method called when widget is deactivated
  virtual void deactivate();

protected:
  /// The method called when widget is activated
  virtual void activateCustom();

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  virtual bool restoreValueCustom();

  /// Computes and updates name of selected object in the widget
  virtual void updateSelectionName();

  // Update focus after the attribute value change
  virtual void updateFocus();

  /// Checks whether all active viewer filters validate the presentation
  /// \param thePrs a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidInFilters(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /// Emits model changed info, updates the current control by selection change
  /// \param theDone a state whether the selection is set
  virtual void updateOnSelectionChanged(const bool theDone);

protected slots:
  /// Called on selection changed event
  virtual void onSelectionChanged();

  //----------- Class members -------------
protected:
  /// Label of the widget
  QLabel* myLabel;

  /// Input control of the widget
  QLineEdit* myTextLine;
};

#endif
