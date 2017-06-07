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
  /// The signal about text change in the text editor
  void valueModified();

  /// The signal about key release on the control, that corresponds to the attribute
  /// \param theObject a sender of the event
  /// \param theEvent key release event
  void keyReleased(QObject* theObject, QKeyEvent* theEvent);

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

private:
  QStringListModel* myCompleterModel;
  QCompleter* myCompleter;
  bool myCompletedAndSelected;
  QString myPlaceHolderText;
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
  /// \param thePlaceHolder a placeholder string
  ModuleBase_WidgetExprEditor( QWidget* theParent,
                               const Config_WidgetAPI* theData,
                               const std::string& thePlaceHolder );
  virtual ~ModuleBase_WidgetExprEditor();

  /// The methiod called when widget is activated
  virtual void activateCustom();

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
  virtual bool storeValueCustom();

  /// Redefinition of virtual method
  virtual bool restoreValueCustom();

private:
   /// A line edit control
  QLabel* myResultLabel;
  ExpressionEditor* myEditor;
};

#endif /* MODULEBASE_WIDGETEXPREDITOR_H_ */
