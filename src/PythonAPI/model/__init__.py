# Copyright (C) 2015-2021  CEA/DEN, EDF R&D
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
"""This package defines the Parametric Geometry API of the Modeler.

All features are available via model.add*() functions. Exceptions are:
- exportToFile() - from Exchange plugin
- duplicatePart(), removePart() - from PartSet plugin
- exportToGEOM() - from Connection plugin
"""

# General purpose functions and abstract root classes

from .services  import *
from .roots     import *

# Built-in features

from .build import *
from .sketcher import *
from .connection import *
from .construction import *
from .exchange import *
from .features import *
from .filters import *
from .collection import *
from .parameter import *
from .partset import *
from .primitives import *
from .gdml import *
from .tests import *

# Add-on features

from .addons import *
# move functions from .addons to top level (model) package
import inspect
for attribute_name in dir(addons):
    attribute = getattr(addons, attribute_name)
    if inspect.isfunction(attribute):
        globals()[attribute_name] = attribute
del inspect
