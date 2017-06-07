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
 * XGUI_ErrorDialog.h
 *
 *  Created on: Apr 28, 2014
 *      Author: sbh
 */

#ifndef XGUI_ERRORDIALOG_H_
#define XGUI_ERRORDIALOG_H_

#include <XGUI.h>
#include <QDialog>
#include <Events_InfoMessage.h>

class QTextEdit;

/**
* \ingroup GUI
* A class of dialog to show error message
*/
class XGUI_ErrorDialog : public QDialog
{
Q_OBJECT
 public:
   /// Constructor
   /// \param parent a parent widget
  XGUI_EXPORT XGUI_ErrorDialog(QWidget* parent);
  XGUI_EXPORT virtual ~XGUI_ErrorDialog();

 public slots:
   /// Update dialog box
  XGUI_EXPORT void refresh();

  /// Clear messages
  XGUI_EXPORT void clear();

  /// Add error message
  XGUI_EXPORT void addError(std::shared_ptr<Events_InfoMessage> theMsg);

  /// Remove error message
  XGUI_EXPORT void removeError(const QString&);

 private:
   /// Widget for error log
  QTextEdit* myErrorLog;

  /// List of errors
  QStringList myErrors;
};

#endif /* XGUI_ERRORDIALOG_H_ */
