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

#ifndef PartSet_WidgetChoice_H
#define PartSet_WidgetChoice_H

#include "PartSet.h"
#include <ModuleBase_WidgetChoice.h>

/**
* \ingroup GUI
* Implementation of a proxy of choice widget in order to geat access to it on moment 
* of creation in module
*/
class PARTSET_EXPORT PartSet_WidgetChoice : public ModuleBase_WidgetChoice
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  PartSet_WidgetChoice(QWidget* theParent, const Config_WidgetAPI* theData)
    : ModuleBase_WidgetChoice(theParent, theData) {}
};

#endif