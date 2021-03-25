// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef PartSet_IconFactory_H
#define PartSet_IconFactory_H

#include "PartSet.h"
#include <ModuleBase_IconFactory.h>
#include <Events_Listener.h>

#include <QMap>


class PartSet_Module;

/**\class PartSet_IconFactory
 * \ingroup GUI
 * \brief This is a class is redefined in order to provide
 * icons of objects for object browser specific for PartSetModule
 */
class PARTSET_EXPORT PartSet_IconFactory : public ModuleBase_IconFactory, public Events_Listener
{
public:
  /// Constructor
  PartSet_IconFactory(PartSet_Module* theModule);

  /// Returns Icon for the given object
  /// \param theObj an object
  virtual QIcon getIcon(ObjectPtr theObj);

  /// Event Listener method
  /// \param theMessage an event message
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

private:
  static QMap<QString, QString> myIcons;
  PartSet_Module* myModule;
};

#endif