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
*          type="open"
*          title="Import file"
*          path="">
*          <validator id="ExchangePlugin_ImportFormat" parameters="BREP:BREPImport,STEP:STEPImport" />
*      </file_selector>
*  \endcode
* It can use following parameters:
* - id - name of object attribute
* - type - type of dialog. Possible values: open, save.
*/
class MODULEBASE_EXPORT ModuleBase_WidgetFileSelector : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_WidgetFileSelector(QWidget* theParent,
                                const Config_WidgetAPI* theData,
                                const std::string& theParentId);
  virtual ~ModuleBase_WidgetFileSelector();

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

  virtual bool restoreValueCustom();

protected:
  /// Converts format to filter string
  static QString formatToFilter(const QString & theFormat);

  /// Returns a short format string of theFilter string
  static QString filterToShortFormat(const QString & theFilter);

  /// Returns a list of extensions from theFilter
  static QStringList filterToExtensions(const QString & theFilter);

  /// Returns list of validator formats
  QStringList getValidatorFormats() const;

  /// Returns string containing formats
  QString filterString() const;

  static QString applyExtension(const QString& theFileName, const QString& theFilter);

protected:
   /// A control for path input
  QLineEdit* myPathField;

  /// A title of open file dialog box
  QString myTitle;

  /// A current format
  QString mySelectedFilter;

  /// A title of open file dialog box
  enum { WFS_OPEN, WFS_SAVE } myType;

  /// Default path
  QString myDefaultPath;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
