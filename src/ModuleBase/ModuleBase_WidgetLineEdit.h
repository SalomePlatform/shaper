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

/**
* \ingroup GUI
* Implementation of model widget for line edit widget.
* It can be defined with "stringvalue" keyword.
*/
class MODULEBASE_EXPORT ModuleBase_WidgetLineEdit : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_WidgetLineEdit(QWidget* theParent,
                                const Config_WidgetAPI* theData,
                                const std::string& theParentId);
  virtual ~ModuleBase_WidgetLineEdit();

  virtual bool restoreValue();

  virtual QList<QWidget*> getControls() const;

 public slots:
   /// A slot for processing text changed event
  void onTextChanged();

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValue() const;

private:
   /// A line edit control
  QLineEdit* myLineEdit;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
