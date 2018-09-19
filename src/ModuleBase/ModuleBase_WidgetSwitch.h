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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef MODULEBASE_WIDGETSWITCH_H_
#define MODULEBASE_WIDGETSWITCH_H_

#include <ModuleBase.h>
#include <ModuleBase_PagedContainer.h>

class QComboBox;
class QStackedLayout;

/**
* \ingroup GUI
* Implements a model widget for switch as a container widget. It can be defined in XML with "switch" keyword
*/
class MODULEBASE_EXPORT ModuleBase_WidgetSwitch : public ModuleBase_PagedContainer
{
  Q_OBJECT
 public:
   /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  ModuleBase_WidgetSwitch(QWidget* theParent,
                          const Config_WidgetAPI* theData);
  virtual ~ModuleBase_WidgetSwitch();

  /// Defines if it is supported to set the value in this widget
  /// It returns false because this is an info widget
  virtual bool canAcceptFocus() const { return false; };

  /// Add a page to the widget
  /// \param theWidget a page widget
  /// \param theName a name of page
  /// \param theCaseId an Id of the page
  /// \param theIcon an icon of the page
  virtual int addPage( ModuleBase_PageBase* theWidget,
                       const QString& theName,
                       const QString& theCaseId,
                       const QPixmap& theIcon,
                       const QString& theTooltip);

 protected:
   /// Returns index of the current page
  virtual int currentPageIndex() const;

  /// Set current page by index
  /// \param index index of the page
  virtual void setCurrentPageIndex(int index);

 private:
  /// Combo box
  QComboBox* myCombo;
  QStackedLayout* myPagesLayout;
};

#endif /* ModuleBase_WidgetSwitch_H_ */
