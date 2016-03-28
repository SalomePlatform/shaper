// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

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
  XGUI_EXPORT void addError(const QString&);

  /// Remove error message
  XGUI_EXPORT void removeError(const QString&);

 private:
   /// Widget for error log
  QTextEdit* myErrorLog;

  /// List of errors
  QStringList myErrors;
};

#endif /* XGUI_ERRORDIALOG_H_ */
