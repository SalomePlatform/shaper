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
  virtual bool storeValueCustom() const;

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom();

  /// The method called when widget is activated
  virtual void activateCustom();

private slots:
  void onDoubleClick(const QModelIndex&);
  void onCloseEditor(QWidget* theEditor, QAbstractItemDelegate::EndEditHint theHint);

private:

  QTreeWidget* myTable;
  QTreeWidgetItem* myFeatures;
  QTreeWidgetItem* myParameters;
  ParametersPlugin_ItemDelegate* myDelegate;
  QModelIndex myEditingIndex;

  QList<FeaturePtr> myFeatureList;
};


#endif