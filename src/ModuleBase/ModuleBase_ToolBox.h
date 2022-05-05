// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef ModuleBase_ToolBox_H
#define ModuleBase_ToolBox_H

#include <ModuleBase.h>
#include <QFrame>

class QButtonGroup;
class QFrame;
class QHBoxLayout;
class QStackedWidget;
class ModuleBase_ModelWidget;

/**
 * \class ModuleBase_ToolBox
 * \ingroup GUI
 * \brief An extension of QFrame object
 */
class MODULEBASE_EXPORT ModuleBase_ToolBox : public QFrame
{
  Q_OBJECT

public:
  /// Constructor
  /// \param theParent a parent widget
  /// \param theUseFrameStyleBox a flag if the tool box should have box covered
  /// buttons and current page
  ModuleBase_ToolBox(QWidget* theParent, const bool theUseFrameStyleBox = false);
  virtual ~ModuleBase_ToolBox();

  ///  Add a new item to the tool box
  /// \param thePage a widget of the new item
  /// \param theName a name of the item
  /// \param theIcon an icon of the item
  void addItem(QWidget* thePage, const QString& theName, const QPixmap& theIcon);

  /// \return number of items
  int count() const;

  /// \return index of current widget
  int currentIndex() const;

  /// Set current item
  /// \param theIdx an index
  void setCurrentIndex(const int theIdx);

  /// Found in the controls of the model widget parent in Stacked Widget
  /// returns whether this controls are in the current widget of the stacked widgets
  /// \param theWidget a model widget
  /// \return boolean result
  static bool isOffToolBoxParent(ModuleBase_ModelWidget* theWidget);

signals:
  /// A signal which is emited on current item changed
  void currentChanged(int);

private slots:
  /// A slot called on button press
  void onButton(int);

private:
  QButtonGroup*   myButtonsGroup;
  QFrame*         myButtonsFrame;
  QHBoxLayout*    myButtonsLayout;
  QStackedWidget* myStack;
};
#endif
