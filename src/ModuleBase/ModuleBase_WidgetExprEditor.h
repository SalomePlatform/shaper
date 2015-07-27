// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetExprEditor.h
 *
 *  Created on: Oct 8, 2014
 *      Author: sbh
 */

#ifndef MODULEBASE_WIDGETEXPREDITOR_H_
#define MODULEBASE_WIDGETEXPREDITOR_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

#include <QList>
#include <QLabel>
#include <QString>
#include <QStringList>
#include <QPlainTextEdit>

class QWidget;
class QStringListModel;
class QCompleter;

class ExpressionEditor: public QPlainTextEdit
{
  Q_OBJECT

 public:
  explicit ExpressionEditor(QWidget* theParent = 0);
  virtual ~ExpressionEditor();

  void setCompletionList(QStringList&);

 public slots:
  void insertCompletion(const QString&, bool isSingleWord = false);
  void performCompletion();

 protected:
  void performCompletion(const QString& theCompletionPrefix);
  virtual void keyPressEvent(QKeyEvent* theEvent);
  bool handledCompletedAndSelected(QKeyEvent* theEvent);

 private:
  QStringListModel* myCompleterModel;
  QCompleter* myCompleter;
  bool myCompletedAndSelected;
};

/**
* \ingroup GUI
* TODO(sbh) add doc
*/
class MODULEBASE_EXPORT ModuleBase_WidgetExprEditor : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration.
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_WidgetExprEditor(QWidget* theParent,
                                const Config_WidgetAPI* theData,
                                const std::string& theParentId);
  virtual ~ModuleBase_WidgetExprEditor();

  virtual QList<QWidget*> getControls() const;

 public slots:
   /// A slot for processing text changed event
  void onTextChanged();

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  virtual bool restoreValueCustom();

private:
   /// A line edit control
  QLabel* myResultLabel;
  ExpressionEditor* myEditor;
};

#endif /* MODULEBASE_WIDGETEXPREDITOR_H_ */
