# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

"""This script inspects existing modules in SALOME and registrs them in
Module reader
"""

import salome
import SALOME_ModuleCatalog
import ConfigAPI

# needed on immediate start of SHAPER from SALOME: salome.naming_service is not initialized then
salome.salome_init()
aCorbaModules = salome.modulcat.GetComponentList()
for aName in aCorbaModules:
  ConfigAPI.Config_ModuleReader.addDependencyModule(aName)
