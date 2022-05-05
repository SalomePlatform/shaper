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

#ifndef PARTSET_WIDGETFILESELECTOR_H_
#define PARTSET_WIDGETFILESELECTOR_H_

#include "PartSet.h"

#include <ModuleBase_WidgetFileSelector.h>

class ModuleBase_IWorkshop;

/**
* \ingroup Modules
* Customization of ModuleBase_WidgetFileSelector in order to write
* format of exported file.
*/
class PARTSET_EXPORT PartSet_WidgetFileSelector : public ModuleBase_WidgetFileSelector
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  PartSet_WidgetFileSelector(QWidget* theParent,
                             ModuleBase_IWorkshop* theWorkshop,
                             const Config_WidgetAPI* theData);

  virtual ~PartSet_WidgetFileSelector() {}

protected:
  /// Reimplemented from ModuleBase_WidgetFileSelector::storeValueCustom()
  virtual bool storeValueCustom();

  /// Reimplemented from ModuleBase_WidgetFileSelector::restoreValue()
  virtual bool restoreValueCustom();

  /// Returns a full format string for the short format
  QString shortFormatToFullFormat( const QString & theShortFormat ) const;

private:
  ModuleBase_IWorkshop* myWorkshop; // the current workshop
};

#endif /* PARTSET_WIDGETFILESELECTOR_H_ */
