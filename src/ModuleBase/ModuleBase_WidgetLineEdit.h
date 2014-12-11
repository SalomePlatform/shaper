// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetLineEdit.h
 *
 *  Created on: Oct 8, 2014
 *      Author: sbh
 */

#ifndef MODULEBASE_WIDGETLINEEDIT_H_
#define MODULEBASE_WIDGETLINEEDIT_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

#include <QList>
#include <QString>
#include <QStringList>

class QWidget;
class QLineEdit;

class MODULEBASE_EXPORT ModuleBase_WidgetLineEdit : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
  ModuleBase_WidgetLineEdit(QWidget* theParent,
                                const Config_WidgetAPI* theData,
                                const std::string& theParentId);
  virtual ~ModuleBase_WidgetLineEdit();

  /// Saves the internal parameters to the given feature
  /// \param theObject a model feature to be changed
  virtual bool storeValue() const;

  virtual bool restoreValue();

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const;

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

 public slots:
  void onTextChanged();

 private:
  QLineEdit* myLineEdit;
  QWidget* myMainWidget;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
