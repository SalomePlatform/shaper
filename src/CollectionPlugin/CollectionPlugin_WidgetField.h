// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        CollectionPlugin_WidgetField.h
// Created:     16 Nov 2016
// Author:      Vitaly SMETANNIKOV


#ifndef CollectionPlugin_WidgetField_H
#define CollectionPlugin_WidgetField_H


#include "CollectionPlugin.h"

#include <ModuleBase_ModelWidget.h>

class QWidget;
class QComboBox;
class QSpinBox;
class QLabel;
class QSlider;
class QTableWidget;

/*!
 * \ingroup GUI
 * Represent a content of the property panel to show/modify parameters of a Field feature.
 */
class CollectionPlugin_WidgetField : public ModuleBase_ModelWidget
{
 Q_OBJECT
public:
  CollectionPlugin_WidgetField(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~CollectionPlugin_WidgetField() {}

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom();

private slots:
  void onNbCompChanged(int theVal);

  void onAddStep();

  void onRemoveStep();

  void onStepMove(int theStep);

private:
  void clearData();

  /// Types of shapes selection
  QComboBox* myShapeTypeCombo;

  /// Types of field data
  QComboBox* myFieldTypeCombo;

  /// Number of components
  QSpinBox* myNbComponentsSpn;

  /// Label of current step
  QLabel* myCurStepLbl;

  /// Slider for steps management
  QSlider* myStepSlider;

  /// Stamp value
  QSpinBox* myStampSpn;

  QTableWidget* myDataTbl;

  QLabel* myMaxLbl;
};

#endif