// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetFileSelector.h
 *
 *  Created on: Aug 28, 2014
 *      Author: sbh
 */

#ifndef MODULEBASE_WIDGETFILESELECTOR_H_
#define MODULEBASE_WIDGETFILESELECTOR_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

#include <QList>
#include <QString>
#include <QStringList>

class QWidget;
class QLineEdit;

/**
* \ingroup GUI
*  Implementation of model widget for open file widget.
*  It can be defined as following:
*  \code
*      <file_selector
*          id="import_file_selector"
*          title="Import file"
*          path="">
*          <validator id="ExchangePlugin_ImportFormat" parameters="BREP:BREPImport,STEP:STEPImport" />
*      </file_selector>
*  \endcode
*/
class MODULEBASE_EXPORT ModuleBase_WidgetFileSelector : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_WidgetFileSelector(QWidget* theParent,
                                const Config_WidgetAPI* theData,
                                const std::string& theParentId);
  virtual ~ModuleBase_WidgetFileSelector();

  virtual bool restoreValue();

  virtual QList<QWidget*> getControls() const;

  /// Returns true if a file on the current path in the line edit
  /// exists and has supported format
  bool isCurrentPathValid();

 public slots:
   /// Processing of path selection button press
  void onPathSelectionBtn();

  /// Processing of path changing
  void onPathChanged();

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

 protected:
   /// Returns string containing formats
  QString formatsString() const;

  /// Return list of validator formats
  QStringList getValidatorFormats() const;

 private:
   /// A control for path input
  QLineEdit* myPathField;

  /// A title of open file dialog box
  QString myTitle;

  /// Default path
  QString myDefaultPath;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
