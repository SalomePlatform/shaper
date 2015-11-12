// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetTable.h
 *
 *  Created on: Oct 8, 2014
 *      Author: sbh
 */

#ifndef ModuleBase_WidgetTable_H_
#define ModuleBase_WidgetTable_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

class ModuleBase_TableModel;
class QTableView;

/**
* \ingroup GUI
* Implementation of model widget for line edit widget.
* It can be defined with "stringvalue" keyword.
*/
class MODULEBASE_EXPORT ModuleBase_WidgetTable : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration.
  /// \param theParentId is Id of a parent of the current attribute
  /// \param thePlaceHolder a string of placeholder
  ModuleBase_WidgetTable( QWidget* theParent,
                          const Config_WidgetAPI* theData,
                          const std::string& theParentId );
  virtual ~ModuleBase_WidgetTable();

  /// Redefinition of virtual method
  virtual QList<QWidget*> getControls() const;

protected slots:
  void onAddRow();

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const { return true;};

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom() { return true; }

private:
  /// A line edit control
  ModuleBase_TableModel* myTableModel;
  QTableView*            myTableView;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
