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

/**
* \ingroup GUI
* A multi-line text editor which lets to input formula and provides a list of completion strings
*/
class ExpressionEditor: public QPlainTextEdit
{
  Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent widget
  explicit ExpressionEditor(QWidget* theParent = 0);
  virtual ~ExpressionEditor();

  /// Set list of completion strings
  void setCompletionList(QStringList&);

  /// Set a text which will be shown when the listr is empty
  void setPlaceHolderText( const QString& );

  /// Returns placeholder list
  QString placeHolderText() const;

  // Returns true if the current value is modified by has not been applyed yet
  bool isModified() const;

  // Clears modified state
  void clearModified();

 public slots:
  /// Insert additional string for completion
  /// \param theCompletion a string to insert
  /// \param isSingleWord a flag shows that inserted string is single word or not
  void insertCompletion(const QString& theCompletion, bool isSingleWord = false);

  /// Perform completion
  void performCompletion();

  /// A slot for processing text changed event
  void onTextChanged();

signals:
  void valueModified();
  /// A signal that is emitted by the "Tab" key event. It is emitted before the key is processed.
  void valueStored();
  void focusNextPrev();

  /// The signal about key release on the control, that corresponds to the attribute
  /// \param theEvent key release event
  void keyReleased(QKeyEvent* theEvent);

 protected:
  /// Perform completion by prefix
  /// \param theCompletionPrefix a prefix for looking for completion
  void performCompletion(const QString& theCompletionPrefix);

  /// Redefinition of virtual method
  /// \param theEvent a key press event
  virtual void keyPressEvent(QKeyEvent* theEvent);

  /// Key events processing
  /// theEvent a key event
  bool handledCompletedAndSelected(QKeyEvent* theEvent);

  /// Redefinition of virtual method
  virtual void paintEvent( QPaintEvent* );

  /// The parent method that processes the "Tab"/"SHIF + Tab" keyboard events
  /// Emits a signal about focus change
  /// If theIsNext is true, this function searches forward, if next is false, it searches backward.
  virtual bool focusNextPrevChild(bool theIsNext);

 private:
  QStringListModel* myCompleterModel;
  QCompleter* myCompleter;
  bool myCompletedAndSelected;
  QString myPlaceHolderText;

  /// Boolean value whether the spin box content is modified
  bool myIsModified;
};

/**
* \ingroup GUI
* A Widget which provides an input of an expression
*/
class MODULEBASE_EXPORT ModuleBase_WidgetExprEditor : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration.
  /// \param theParentId is Id of a parent of the current attribute
  /// \param thePlaceHolder a placeholder string
  ModuleBase_WidgetExprEditor( QWidget* theParent,
                               const Config_WidgetAPI* theData,
                               const std::string& theParentId,
                               const std::string& thePlaceHolder );
  virtual ~ModuleBase_WidgetExprEditor();

  /// Redefinition of virtual method
  virtual QList<QWidget*> getControls() const;

  /// Returns true if the event is processed.
  virtual bool processEnter();

protected slots:
  /// A slot for processing text changed event
  void onTextChanged();

protected:
  /// Do not initialize value on the widget activation
  virtual void initializeValueByActivate();

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  /// Redefinition of virtual method
  virtual bool restoreValueCustom();

private:
   /// A line edit control
  QLabel* myResultLabel;
  ExpressionEditor* myEditor;
};

#endif /* MODULEBASE_WIDGETEXPREDITOR_H_ */
