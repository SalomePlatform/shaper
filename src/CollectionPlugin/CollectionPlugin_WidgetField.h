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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef CollectionPlugin_WidgetField_H
#define CollectionPlugin_WidgetField_H


#include "CollectionPlugin.h"

#include <ModuleBase_WidgetSelector.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModelAPI_AttributeTables.h>

#include <QList>
#include <QStringList>
#include <QStyledItemDelegate>


class QWidget;
class QComboBox;
class QSpinBox;
class QLabel;
class QSlider;
class QTableWidget;
class QStackedWidget;
class QPushButton;
class QTableWidgetItem;
class QLineEdit;


class DataTableItemDelegate : public QStyledItemDelegate
{
 Q_OBJECT
public:
  DataTableItemDelegate(ModelAPI_AttributeTables::ValueType theType);

  virtual QWidget* createEditor(QWidget* theParent,
                                const QStyleOptionViewItem & theOption,
                                const QModelIndex& theIndex) const;

  ModelAPI_AttributeTables::ValueType dataType() const { return myType; }

  void setDataType(ModelAPI_AttributeTables::ValueType theType) { myType = theType; }

private slots:
  void onEditItem(const QString& theText);

private:
  ModelAPI_AttributeTables::ValueType myType;
};



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
  virtual bool isValidSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

  /// Returns true if the event is processed.
  virtual bool processEnter();

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValues the wrapped selection values
  /// \param theToValidate a validation of the values flag
  virtual bool setSelection(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                            const bool theToValidate);

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom();

  /// Retunrs a list of possible shape types
  /// \return a list of shapes
  virtual QIntList shapeTypes() const;

  /// Redefinition of virtual function
  /// \param theObject an object for the event
  /// \param theEvent an event
  virtual bool eventFilter(QObject* theObject, QEvent* theEvent);

  //virtual void showEvent(QShowEvent* theEvent);

  /// Return the attribute values wrapped in a list of viewer presentations
  /// \return a list of viewer presentations, which contains an attribute result and
  /// a shape. If the attribute do not uses the shape, it is empty
  virtual QList<std::shared_ptr<ModuleBase_ViewerPrs>> getAttributeSelection() const;

private slots:
  /// Slot called on number of component changed
  /// \param theVal - a new components number
  void onNbCompChanged(int theVal);

  /// Slot called on add a step
  void onAddStep();

  /// Slot called on remove a step
  void onRemoveStep();

  /// Slot called on a navigation between steps
  /// \param theStep - a current step
  void onStepMove(int theStep);

  /// Slot called on a navigation between steps
  /// \param theIdx - a current step
  void onFieldTypeChanged(int theIdx);

  /// Slot called on editing of a table cell
  /// \param theRow a row of the cell
  /// \param theCol a column of the cell
  void onTableEdited(int theRow, int theCol);

  /// Slot called on selection mode changed
  /// \param theType a new choice
  void onShapeTypeChanged(int theType);

  /// Slot called on widget focus changed
  /// \param theOld a widget wgich lost focus
  /// \param theNew a widget which get focus
  void onFocusChanged(QWidget* theOld, QWidget* theNew);

  /// Slot called on a slider navigation changed
  /// \param theMin - a minimal value
  /// \param theMax a maximal value
  void onRangeChanged(int theMin, int theMax);

  void onColumnResize(int theIndex, int theOld, int theNew);

private:
  /// Clear existing tables
  void clearData();

  /// Append controls for management of a new step
  void appendStepControls();

  /// Remove current step controls
  void removeStepControls();

  /// Update header of a table
  /// \param theDataTbl a table widget
  void updateHeaders(QTableWidget* theDataTbl) const;

  /// Return Item Id of myShapeTypeCombo by selection mode
  /// \param theStr a selection mode
  int getSelectionType(const std::string& theStr) const;

  /// Return selection mode by Item Id of myShapeTypeCombo
  /// \param theType an item id
  std::string getSelectionType(int theType) const;

  /// Create default table item
  QTableWidgetItem* createDefaultItem() const;

  /// Create a table item from the given value
  /// \param theVal a value for the item
  QTableWidgetItem* createValueItem(ModelAPI_AttributeTables::Value& theVal) const;

  QString getValueText(ModelAPI_AttributeTables::Value& theVal) const;

  /// Return a value from the string
  /// \param theStrVal a string
  ModelAPI_AttributeTables::Value getValue(QString theStrVal) const;

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

  /// List of created tables
  QList<QTableWidget*> myDataTblList;

  /// Max value Label for the slider
  QLabel* myMaxLbl;

  /// A container for step controls
  QStackedWidget* myStepWgt;

  /// A list for component names
  QStringList myCompNamesList;

  /// Remove button
  QPushButton* myRemoveBtn;

  /// Editor for table header
  QLineEdit* myHeaderEditor;

  /// Index of header section under editing
  int myEditIndex;

  /// Stae of a table editing
  bool myIsTabEdit;

  bool myActivation;

  DataTableItemDelegate* myDelegate;
};

#endif