// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetChoice.h
// Created:     03 Sept 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetChoice_H
#define ModuleBase_WidgetChoice_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class QWidget;
class QLabel;
class QComboBox;

/**
* Implementation of model widget for choice widget definition (combo box)
* It can be defined as following:
* \code
*   <choice id="bool_type" 
*     label="Type" 
*     tooltip="Type of boolean operation"
*     string_list="Cut Fuse Common"
*   />
* \endcode
*/
class MODULEBASE_EXPORT ModuleBase_WidgetChoice : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_WidgetChoice(QWidget* theParent, const Config_WidgetAPI* theData, 
                          const std::string& theParentId);

  virtual ~ModuleBase_WidgetChoice();
  
  virtual bool storeValue() const;

  virtual bool restoreValue();

  virtual bool focusTo();

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const
  {
    return myContainer;
  }

  /// Returns list of widget controls
  /// \return a controls list
  virtual QList<QWidget*> getControls() const;

private slots:
  /// Slot called on combo box index change
  void onCurrentIndexChanged(int theIndex);

private:
  /// Container widget
  QWidget* myContainer;

  /// The label
  QLabel* myLabel;

  /// The control
  QComboBox* myCombo;
};

#endif
