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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef ParametersPlugin_WidgetParamsMgr_H_
#define ParametersPlugin_WidgetParamsMgr_H_

#include <ModuleBase_ModelDialogWidget.h>
#include <QModelIndex>
#include <QAbstractItemDelegate>
#include <QTreeWidget>

class QTreeWidgetItem;
class ParametersPlugin_ItemDelegate;
class QPushButton;
class QToolButton;


/*!
 * \ingroup GUI
 * Redefinition of QTreeWidget for processing of closeEditor event
 */
class ParametersPlugin_TreeWidget: public QTreeWidget
{
 Q_OBJECT
public:
  /// Constructor
  /// \param theParent a parent widget
  ParametersPlugin_TreeWidget(QWidget* theParent = 0) : QTreeWidget(theParent) {}

protected slots:
  /// Redefinition of virtual method
  /// \param theEditor a editor widget
  /// \param theHint end of editing hint
  virtual void closeEditor(QWidget* theEditor, QAbstractItemDelegate::EndEditHint theHint);
};



/*!
 * \ingroup GUI
 * Represent a content of the property panel to show/modify parameters of some feature.
 */
class ParametersPlugin_WidgetParamsMgr : public ModuleBase_ModelDialogWidget
{
 Q_OBJECT
public:
  /// Constructs a model widget
  ParametersPlugin_WidgetParamsMgr(QWidget* theParent, const Config_WidgetAPI* theData);

  /// Destructs the model widget
  virtual ~ParametersPlugin_WidgetParamsMgr() {}

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom();

  /// The method called when widget is activated
  virtual void activateCustom();

private slots:
  /// Slot for reaction on double click in the table (start editing)
  /// \param theIndex the clicked index
  void onDoubleClick(const QModelIndex& theIndex);

  /// Slot for reaction on end of cell editing
  /// \param theEditor the editor widget
  /// \param theHint end of edit type
  void onCloseEditor(QWidget* theEditor, QAbstractItemDelegate::EndEditHint theHint);

  /// Slot for reaction on add parameter
  void onAdd();

  /// Slot for reaction on insert parameter
  void onInsert();

  /// Slot for reaction on remove parameter
  void onRemove();

  /// Slot for reaction on shift up
  void onUp();

  /// Slot for reaction on shift down
  void onDown();

  /// Slot to show message on closing of editor
  void sendWarning();

  /// Slot for reaction on selection in the table
  void onSelectionChanged();

private:
  /// Creates a new parameter feature
  FeaturePtr createParameter() const;

  /// Creates a new item
  QTreeWidgetItem* createNewItem(QTreeWidgetItem* theParent) const;

  /// Returns currently selected item
  QTreeWidgetItem* selectedItem() const;

  /// Select the given Item and scroll the table to make it visible
  void selectItemScroll(QTreeWidgetItem* theItem);

  /// Update values in features part
  void updateItem(QTreeWidgetItem* theItem, const QList<QStringList>& theFeaturesList);

  void updateFeaturesPart();

  void updateParametersPart();

  /// Returns true if values in the widget are valid
  bool isValid();

  /// Returns true if parameter with the given name already exists
  bool hasName(const QString& theName) const;

  /// Enable or disable buttons for parameters managemnt
  void enableButtons(bool theEnable);

  QList<QStringList> featuresItems(const QList<FeaturePtr>& theFeatures,
                                   QList<FeaturePtr>& theFeatureList) const;
  QList<QStringList> parametersItems(const QList<FeaturePtr>& theFeatures) const;

  void updateParametersFeatures();

  ParametersPlugin_TreeWidget* myTable;
  QTreeWidgetItem* myFeatures;
  QTreeWidgetItem* myParameters;
  ParametersPlugin_ItemDelegate* myDelegate;

  QList<FeaturePtr> myParametersList;

  QString myMessage;

  QPushButton* myAddBtn;
  QPushButton* myInsertBtn;
  QPushButton* myRemoveBtn;
  QToolButton* myUpBtn;
  QToolButton* myDownBtn;
};


#endif