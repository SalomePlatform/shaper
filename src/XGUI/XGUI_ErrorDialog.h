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

class XGUI_ErrorDialog : public QDialog
{
Q_OBJECT
 public:
  XGUI_EXPORT XGUI_ErrorDialog(QWidget* parent);XGUI_EXPORT virtual ~XGUI_ErrorDialog();

 public slots:
  XGUI_EXPORT void refresh();XGUI_EXPORT void clear();XGUI_EXPORT void addError(const QString&);XGUI_EXPORT void removeError(
      const QString&);

 private:
  QTextEdit* myErrorLog;
  QStringList myErrors;
};

#endif /* XGUI_ERRORDIALOG_H_ */
