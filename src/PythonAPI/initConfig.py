"""This script inspects existing modules in SALOME and registrs them in
Module reader
"""

import salome
import SALOME_ModuleCatalog
import ConfigAPI

# needed on immediate start of SHAPER from SALOME: salome.naming_service is not initialized then
salome.salome_init()
obj = salome.naming_service.Resolve('Kernel/ModulCatalog')
catalog = obj._narrow(SALOME_ModuleCatalog.ModuleCatalog)
aCorbaModules = catalog.GetComponentList()
for aName in aCorbaModules:
  ConfigAPI.Config_ModuleReader.addDependencyModule(aName)
