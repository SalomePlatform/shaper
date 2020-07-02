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

#ifndef MODULEBASE_WIDGETRADIOBOX_H_
#define MODULEBASE_WIDGETRADIOBOX_H_

#include <ModuleBase.h>
#include <ModuleBase_PagedContainer.h>

class QFormLayout;
class QButtonGroup;
class QRadioButton;

class MODULEBASE_EXPORT ModuleBase_WidgetRadiobox : public ModuleBase_PagedContainer
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  ModuleBase_WidgetRadiobox(QWidget* theParent, const Config_WidgetAPI* theData);
  virtual ~ModuleBase_WidgetRadiobox();


  /// Add a page to the widget
  /// \param theWidget a page widget
  /// \param theName a name of page
  /// \param theCaseId an Id of the page
  /// \param theIcon an icon of the page
  virtual int addPage(ModuleBase_PageBase* theWidget,
    const QString& theName,
    const QString& theCaseId,
    const QPixmap& theIcon,
    const QString& theTooltip);

protected:
  /// Implements ModuleBase_PagedContainer
  virtual int currentPageIndex() const;

  /// Implements ModuleBase_PagedContainer
  virtual void setCurrentPageIndex(int);

private:
  QFormLayout* myLayout;
  QButtonGroup* myGroup;
  bool myVerticalAlignment;
};

#endif