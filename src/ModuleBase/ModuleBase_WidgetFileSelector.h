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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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
  ModuleBase_WidgetFileSelector(QWidget* theParent,
                                const Config_WidgetAPI* theData);
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
  virtual bool storeValueCustom();

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

  /// Add an extension to file name if the extension is not defined
  /// \param theFileName a file name
  /// \param theFilter a file filter
  /// \return the file name with extension
  static QString applyExtension(const QString& theFileName, const QString& theFilter);

protected:
   /// A control for path input
  QLineEdit* myPathField;

  /// A title of open file dialog box
  QString myTitle;

  /// A current format
  QString mySelectedFilter;

  /// A title of open file dialog box
  enum {
    WFS_OPEN, ///< open file
    WFS_SAVE ///< save file
  } myType; ///< type of dialog

  /// Default path
  QString myDefaultPath;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
