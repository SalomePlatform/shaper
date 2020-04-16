// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include <QWidgetAction>

class QFrame;
class QAction;
class QWidget;
class QToolButton;

/*!
 * \ingroup Salome
 * Custom (nested) button in salome mode.
 */
class SHAPERGUI_NestedButton : public QWidgetAction
{
  Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent objects
   /// \param theNestedActions a list of nested actions
  SHAPERGUI_NestedButton(QObject* theParent, const QList<QAction*>& theNestedActions);
  virtual ~SHAPERGUI_NestedButton();

 private slots:
  /// Shows/hides the additional buttons widget
  void showAdditionalButtons(bool);

  /// Slot called on action state
  void actionStateChanged();

 protected:
  /// Creates the button representation
  /// \param theParent a parent widget
  virtual QWidget * createWidget(QWidget * theParent);

  virtual bool event(QEvent* theEvent);

 private:
  QList<QAction*> myNestedActions; ///< list of nested actions
  QWidget* myAdditionalButtonsWidget; ///< widget to precess additional buttons visibility
  QFrame* myButtonFrame; ///< frame arround button representation
  QToolButton* myThisButton; ///< main button
};

#endif /* SRC_SHAPERGUI_NESTEDBUTTON_H_ */
