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

"""
    UnitTestAxis.py
    Unit Test of ConstructionPlugin_Axis class


    static const std::string CONSTRUCTION_AXIS_KIND("Axis");
    static const std::string METHOD_ATTR("CreationMethod");
    static const std::string POINT_ATTR_FIRST("FirstPoint");
    static const std::string POINT_ATTR_SECOND("SecondPoint");
    static const std::string CYLINDRICAL_FACE_ATTR("CylindricalFace");
    static const std::string X_ATTR("X");
    static const std::string Y_ATTR("Y");
    static const std::string Z_ATTR("Z");


    data()->addAttribute(ConstructionPlugin_Axis::METHOD(), ModelAPI_AttributeString::typeId());

    data()->addAttribute(ConstructionPlugin_Axis::POINT_FIRST(), ModelAPI_AttributeSelection::typeId());
    data()->addAttribute(ConstructionPlugin_Axis::POINT_SECOND(), ModelAPI_AttributeSelection::typeId());

    data()->addAttribute(ConstructionPlugin_Axis::CYLINDRICAL_FACE(), ModelAPI_AttributeSelection::typeId());

    data()->addAttribute(ConstructionPlugin_Axis::X(), ModelAPI_AttributeDouble::typeId());
    data()->addAttribute(ConstructionPlugin_Axis::Y(), ModelAPI_AttributeDouble::typeId());
    data()->addAttribute(ConstructionPlugin_Axis::Z(), ModelAPI_AttributeDouble::typeId());

"""


#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *
import math

__updated__ = "2016-01-04"

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
# Create a part for creation of a box
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)

aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()
#=========================================================================
# Creation of an Axis by coordinates
#=========================================================================
aSession.startOperation()
anAxisByDimensions = aPart.addFeature("Axis")
anAxisByDimensions.string("CreationMethod").setValue("AxisByDimensionsCase")
anAxisByDimensions.real("DX").setValue(2.5)
anAxisByDimensions.real("DY").setValue(3.2)
anAxisByDimensions.real("DZ").setValue(1.4)
anAxisByDimensions.execute()


assert(len(anAxisByDimensions.results()) > 0)
anAxisByDimensions = modelAPI_ResultConstruction(anAxisByDimensions.firstResult())
assert(anAxisByDimensions is not None)

aSession.finishOperation()
#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())
