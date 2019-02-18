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

#ifndef MODULEBASE_PAGEBASE_H_
#define MODULEBASE_PAGEBASE_H_

#include <ModuleBase.h>
#include <ModuleBase_Tools.h>

class ModuleBase_ModelWidget;
class QLayout;
class QWidget;

/*!
 * Represent a property panel's list of ModuleBase_ModelWidgets
 * or other pages widgets derived from ModuleBase_PageBase.
 */
class MODULEBASE_EXPORT ModuleBase_PageBase
{
 public:
  /// Base constructor.
  ModuleBase_PageBase();
  /// Base virtual destructor.
  virtual ~ModuleBase_PageBase();
  /// Cast the page to regular QWidget
  QWidget* pageWidget();
  /// Adds the given ModuleBase_ModelWidget to the page
  void addModelWidget(ModuleBase_ModelWidget* theWidget);
  /// Adds the given ModuleBase_PageBase to the page
  void addPageWidget(ModuleBase_PageBase* theWidget);
  /// Adds the given widget to the page
  void addWidget(QWidget* theWidget);
  /// Removes all items from page's layout
  void clearPage();
  /// Passes focus from page to the first ModuleBase_ModelWidget contained on the page
  bool takeFocus();
  /// Returns list of ModuleBase_ModelWidgets contained on the page
  QList<ModuleBase_ModelWidget*> modelWidgets() const;
  /// Aligns top all widgets on page
  void alignToTop();

 protected:
  /// Pure Virtual. Allows to derived class to lay out the widget properly;
  virtual void placeModelWidget(ModuleBase_ModelWidget* theWidget) = 0;
  /// Pure Virtual. Allows to derived class to lay out the page properly;
  virtual void placePageWidget(ModuleBase_PageBase* theWidget);
  /// Pure Virtual. Allows to derived class to lay out the page properly;
  virtual void placeWidget(QWidget* theWidget) = 0;
  /// Pure Virtual. Returns layout of the page.
  virtual QLayout* pageLayout() = 0;
  /// Pure Virtual. Allows to derived class to insert page stretch properly.
  virtual void addPageStretch() = 0;

 private:
  QList<ModuleBase_ModelWidget*> myWidgetList; ///< list of widgets contained on the page

};

#endif /* MODULEBASE_PAGEBASE_H_ */
