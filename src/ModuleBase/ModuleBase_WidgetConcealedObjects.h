// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetConcealedObjects.h
// Created:     29 Jul 2016
// Author:      Natalia ERMOLAEVA

#ifndef MODULEBASE_WIDGETCONCEALEDOBJECTS_H_
#define MODULEBASE_WIDGETCONCEALEDOBJECTS_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

#include <string>
#include <map>
#include <memory>

class ModelAPI_Object;

class QWidget;
class QTableWidget;

/**
* \ingroup GUI
* Implementation of model widget for table of concealed objects.
*/
class MODULEBASE_EXPORT ModuleBase_WidgetConcealedObjects : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration.
  ModuleBase_WidgetConcealedObjects(QWidget* theParent,
                                    const Config_WidgetAPI* theData);
  virtual ~ModuleBase_WidgetConcealedObjects();

  /// Redefinition of virtual method
  virtual QList<QWidget*> getControls() const;

  /// The widget can not accept focus
  virtual bool canAcceptFocus() const { return false; };

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Redefinition of virtual method
  virtual bool restoreValueCustom();

private:
  /// Creates a row in view
  /// \param theResult a result for the row
  void addViewRow(const std::shared_ptr<ModelAPI_Result>& theResult);

private slots:
  /// Processes the display button click in the view.
  /// \param theState a new state
  void onItemToggled(bool theState);

private:
  std::string myBaseShapeAttribute; ///< attribute of the base object
  QTableWidget* myView; ///< table view of visibility states

  std::shared_ptr<ModelAPI_Feature> myBaseFeature; ///< backup of feature of filling the table view
  /// backup of visualized in the view concealed objects, stored to avoid table rebuild by restore
  std::map<int, std::shared_ptr<ModelAPI_Result> > myConcealedResults;
};

#endif /* MODULEBASE_WIDGETCONCEALEDOBJECTS_H_ */
