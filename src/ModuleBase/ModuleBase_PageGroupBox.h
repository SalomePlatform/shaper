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

#ifndef MODULEBASE_PAGEGROUPBOX_H_
#define MODULEBASE_PAGEGROUPBOX_H_

#include <ModuleBase.h>
#include <ModuleBase_PageBase.h>

#include <QGroupBox>
#include <QList>

class ModuleBase_ModelWidget;
class QGridLayout;


/*!
 * \ingroup GUI
 * Represent a property panel's list of ModuleBase_ModelWidgets.
 */
class MODULEBASE_EXPORT ModuleBase_PageGroupBox : public QGroupBox, public ModuleBase_PageBase
{
  Q_OBJECT
 public:
  /// Constructs a page that looks like a QGroupBox
  explicit ModuleBase_PageGroupBox(QWidget* theParent = 0);
  /// Destructs the page
  virtual ~ModuleBase_PageGroupBox();

 protected:
  /// Adds the given widget to page's layout
  virtual void placeModelWidget(ModuleBase_ModelWidget* theWidget);
  /// Adds the given page to page's layout
  virtual void placeWidget(QWidget* theWidget);
  /// Returns page's layout (QGridLayout)
  virtual QLayout* pageLayout();
  /// Adds a stretch to page's layout
  virtual void addPageStretch();

 private:
  QGridLayout* myMainLayout; ///< page's layout
};

#endif /* MODULEBASE_PAGEGROUPBOX_H_ */
