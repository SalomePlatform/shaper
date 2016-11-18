// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        CollectionPlugin_WidgetField.h
// Created:     16 Nov 2016
// Author:      Vitaly SMETANNIKOV


#ifndef CollectionPlugin_WidgetField_H
#define CollectionPlugin_WidgetField_H


#include "CollectionPlugin.h"

#include <ModuleBase_WidgetSelector.h>
#include <ModuleBase_ViewerPrs.h>

#include <QList>
#include <QStringList>


class QWidget;
class QComboBox;
class QSpinBox;
class QLabel;
class QSlider;
class QTableWidget;
class QStackedWidget;
class QPushButton;

/*!
 * \ingroup GUI
 * Represent a content of the property panel to show/modify parameters of a Field feature.
 */
class CollectionPlugin_WidgetField : public ModuleBase_WidgetSelector
{
 Q_OBJECT
public:
  CollectionPlugin_WidgetField(QWidget* theParent, 
                               ModuleBase_IWorkshop* theWorkshop, 
                               const Config_WidgetAPI* theData);

  virtual ~CollectionPlugin_WidgetField() {}

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Checks the widget validity. By default, it returns true.
  /// \param thePrs a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);


protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom();

  /// Retunrs a list of possible shape types
  /// \return a list of shapes
  virtual QIntList shapeTypes() const;


protected slots:
  /// Slot which is called on selection event
  virtual void onSelectionChanged();

private slots:
  void onNbCompChanged(int theVal);

  void onAddStep();

  void onRemoveStep();

  void onStepMove(int theStep);

  void onFieldTypeChanged(int theIdx);

private:
  void clearData();

  void appendStepControls();
  void removeStepControls();

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
  QList<QSpinBox*> myStampSpnList;

  QList<QTableWidget*> myDataTblList;

  QLabel* myMaxLbl;

  QStackedWidget* myStepWgt;

  QStringList myCompNamesList;

  QList<ModuleBase_ViewerPrsPtr> mySelection;

  QPushButton* myRemoveBtn;
};

#endif