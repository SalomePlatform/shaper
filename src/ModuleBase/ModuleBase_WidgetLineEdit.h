// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
  /// \param thePlaceHolder a string of placeholder
  ModuleBase_WidgetLineEdit( QWidget* theParent,
                             const Config_WidgetAPI* theData,
                             const std::string& thePlaceHolder );
  virtual ~ModuleBase_WidgetLineEdit();

  /// Redefinition of virtual method
  virtual QList<QWidget*> getControls() const;

  /// Returns True if data of its feature was modified during operation
  virtual bool isModified() const;

protected:
  /// Returns true if the event is processed.
  virtual bool processEnter();

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Redefinition of virtual method
  virtual bool restoreValueCustom();

   /// A line edit control
  QLineEdit* myLineEdit;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
