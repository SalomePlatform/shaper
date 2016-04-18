// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ParametersPlugin_WidgetParamsMgr.h
// Created:     11 Apr 2016
// Author:      Vitaly SMETANNIKOV

#ifndef ParametersPlugin_WidgetParamsMgr_H_
#define ParametersPlugin_WidgetParamsMgr_H_

#include <ModuleBase_ModelWidget.h>
#include <QModelIndex>
#include <QAbstractItemDelegate>

class QTreeWidget;
class QTreeWidgetItem;
class ParametersPlugin_ItemDelegate;

/*!
 * \ingroup GUI
 * Represent a content of the property panel to show/modify parameters of some feature.
 */
class ParametersPlugin_WidgetParamsMgr : public ModuleBase_ModelWidget
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

  void sendWarning();

private:
  void updateFeaturesPart();

  bool hasName(const QString& theName) const;

  QTreeWidget* myTable;
  QTreeWidgetItem* myFeatures;
  QTreeWidgetItem* myParameters;
  ParametersPlugin_ItemDelegate* myDelegate;
  QModelIndex myEditingIndex;

  QList<FeaturePtr> myParametersList;

  QString myMessage;
};


#endif