// Copyright (C) 2014-2024  CEA, EDF
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

/*
 * SHAPERGUI_NestedButton.h
 *
 *  Created on: Apr 13, 2015
 *      Author: sbh
 */

#ifndef SRC_SHAPERGUI_NESTEDBUTTON_H_
#define SRC_SHAPERGUI_NESTEDBUTTON_H_

#include <QtxAction.h>

class QFrame;
class QAction;
class QWidget;
class QToolButton;

/*!
 * \ingroup Salome
 * Custom (nested) button in salome mode.
 */
class SHAPERGUI_NestedButton : public QtxAction
{
  Q_OBJECT

public:
  /// Constructor
  /// \param theParent a parent objects
  /// \param theID Application-unique action ID is used by ShortcutMgr, FindAction dialog, etc.
  /// \param theNestedActions a list of nested actions
  SHAPERGUI_NestedButton(QObject* theParent, const QString& theID, const QList<QAction*>& theNestedActions);
  virtual ~SHAPERGUI_NestedButton() = default;

  virtual void setEnabled(bool theOn);

private slots:
  /// Shows/hides the additional buttons widget
  void showAdditionalButtons(bool);

protected:
  /// Creates the button representation
  /// \param theParent a parent widget
  virtual QWidget * createWidget(QWidget * theParent);

private:
  QList<QAction*> myNestedActions; ///< list of nested actions
  QWidget* myAdditionalButtonsWidget; ///< widget to precess additional buttons visibility
  QFrame* myButtonFrame; ///< frame arround button representation
  QToolButton* myThisButton; ///< main button
};

#endif /* SRC_SHAPERGUI_NESTEDBUTTON_H_ */
