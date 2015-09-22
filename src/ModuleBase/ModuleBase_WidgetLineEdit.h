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
  /// \param theData the widget configuration.
  /// \param theParentId is Id of a parent of the current attribute
  /// \param thePlaceHolder a string of placeholder
  ModuleBase_WidgetLineEdit( QWidget* theParent,
                             const Config_WidgetAPI* theData,
                             const std::string& theParentId,
                             const std::string& thePlaceHolder );
  virtual ~ModuleBase_WidgetLineEdit();

  /// Redefinition of virtual method
  virtual QList<QWidget*> getControls() const;

 public slots:
   /// A slot for processing text changed event
  void onTextChanged();

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  /// Redefinition of virtual method
  virtual bool restoreValueCustom();

private:
   /// A line edit control
  QLineEdit* myLineEdit;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
