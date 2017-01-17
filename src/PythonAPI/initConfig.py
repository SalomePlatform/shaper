"""This script inspects existing modules in SALOME and registrs them in
Module reader
"""

import salome
import SALOME_ModuleCatalog
import ConfigAPI

obj = salome.naming_service.Resolve('Kernel/ModulCatalog')
catalog = obj._narrow(SALOME_ModuleCatalog.ModuleCatalog)
aCorbaModules = catalog.GetComponentList()
for aName in aCorbaModules:
  ConfigAPI.Config_ModuleReader.addDependencyModule(aName)
