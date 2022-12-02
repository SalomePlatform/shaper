# Copyright (C) 2015-2022  CEA/DEN, EDF R&D
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
"""Package for Features plugin for the Parametric Geometry API of the Modeler.
"""

from FeaturesAPI import addPlacement, addRotation, addScale, addSymmetry, addTranslation
from FeaturesAPI import addMultiTranslation, addMultiRotation
from FeaturesAPI import addExtrusion, addExtrusionCut, addExtrusionFuse
from FeaturesAPI import addRevolution, addRevolutionCut, addRevolutionFuse
from FeaturesAPI import addPipe, addLoft
from FeaturesAPI import addCut, addFuse, addCommon, addSmash, addSplit
from FeaturesAPI import addIntersection, addPartition, addUnion, addRemoveSubShapes
from FeaturesAPI import addRecover
from FeaturesAPI import addFillet, addChamfer
from FeaturesAPI import addFusionFaces
from FeaturesAPI import measureLength, measureDistance, measureRadius, measureAngle
from FeaturesAPI import getPointCoordinates, getGeometryCalculation, getBoundingBox
from FeaturesAPI import getNormal
from FeaturesAPI import makeVertexInsideFace
from FeaturesAPI import addRemoveResults
from FeaturesAPI import addCopy, addImportResult
from FeaturesAPI import addDefeaturing
from FeaturesAPI import addSewing
