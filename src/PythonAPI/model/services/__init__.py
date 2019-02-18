# Copyright (C) 2016-2019  CEA/DEN, EDF R&D
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
"""Package for services for the Parametric Geometry API of the Modeler.
"""

from ModelHighAPI import moduleDocument, activeDocument
from ModelHighAPI import defaultPlane, standardPlane
from ModelHighAPI import begin, end
from ModelHighAPI import apply as do
from ModelHighAPI import updateFeatures
from ModelHighAPI import undo, redo
from ModelHighAPI import reset
from ModelHighAPI import addFolder
from ModelHighAPI import ModelHighAPI_Selection as selection
from ModelHighAPI import checkPythonDump as checkPythonDump
