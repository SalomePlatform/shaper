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
      Unit test of Check shared faces
"""
#=========================================================================
# Initialization of the test
#=========================================================================


import os
import math

from ModelAPI import *
from GeomAPI import *
from salome.shaper import model


__updated__ = "2020-11-12"


if __name__ == '__main__':

    model.begin()
    partSet = model.moduleDocument()
    Part_1 = model.addPart(partSet)
    Part_1_doc = Part_1.document()
    ### Create Box
    Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
    ### Create Point
    Point_2 = model.addPoint(Part_1_doc, 20, 10, 10)
    ### Create Box
    Box_2 = model.addBox(Part_1_doc, model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Bottom]"), model.selection("VERTEX", "Point_1"))
    ### Create CompSolid
    CompSolid_1 = model.addCompSolid(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Box_2_1")])
    ### Create Box
    Box_3 = model.addBox(Part_1_doc, 10, 10, 10)
    ### Create Box
    Box_4 = model.addBox(Part_1_doc, 10, 10, 10)
    ### Create Translation
    Translation_1 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "all-in-Box_4")], axis = model.selection("EDGE", "PartSet/OX"),
                                         distance = 10, keepSubResults = True)
    ### Create CompSolid
    CompSolid_2 = model.addCompSolid(Part_1_doc, [model.selection("SOLID", "Box_3_1"), model.selection("COMPOUND", "Translation_1_1")])
    ### Create Box
    Box_5 = model.addBox(Part_1_doc, 10, 10, 10)
    ### Create Translation
    Translation_2 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_5_1")], vector = [10, 0, 10], keepSubResults = True)
    ### Create Compound
    Compound_1 = model.addCompound(Part_1_doc, [model.selection("COMPSOLID", "CompSolid_1_1"), model.selection("SOLID", "Translation_2_1")])
    Compound_1.result().setTransparency(0.5)
    Compound_1.result().subResult(0).setTransparency(0.5)
    Compound_1.result().subResult(0).subResult(0).setTransparency(0.5)
    Compound_1.result().subResult(0).subResult(1).setTransparency(0.5)
    Compound_1.result().subResult(1).setTransparency(0.5)

    ### Create Shared_faces for compsolid
    Shared_faces_1 = model.getSharedFaces(Part_1_doc, model.selection("COMPSOLID", "CompSolid_1_1"), "faces_compsolid")

    ### Create Shared_faces for compound
    Shared_faces_2 = model.getSharedFaces(Part_1_doc, model.selection("COMPOUND", "Compound_1_1"), 'faces_compound')

    model.do()

     # Check results
    assert(Part_1_doc.size("Groups") == 2)
    Shared_faces_1_Feature = Shared_faces_1.feature()
    assert Shared_faces_1_Feature.error() == ''
    assert Shared_faces_1_Feature.name() == "Shared_faces_1"

    aSelectionList = Shared_faces_1_Feature.selectionList("group_list")
    assert aSelectionList.size() == 1

    Shared_faces_2_Feature = Shared_faces_2.feature()
    assert Shared_faces_2_Feature.error() == ''
    assert Shared_faces_2_Feature.name() == "Shared_faces_2"

    aSelectionList = Shared_faces_2_Feature.selectionList("group_list")
    assert aSelectionList.size() == 1

    resShape_1 = modelAPI_Result(Part_1_doc.object("Groups", 0)).shape()
    assert(not resShape_1.isNull())

    # the group result is a face, check that this is one face
    aShapeExplorer = GeomAPI_ShapeExplorer(resShape_1, GeomAPI_Shape.FACE)
    assert(aShapeExplorer.more())
    assert(aShapeExplorer.current().isFace())
    aShapeExplorer.next()
    assert(not aShapeExplorer.more())

    resShape_2 = modelAPI_Result(Part_1_doc.object("Groups", 1)).shape()
    assert(not resShape_2.isNull())

    # the group result is a face, check that this is one face
    aShapeExplorer = GeomAPI_ShapeExplorer(resShape_2, GeomAPI_Shape.FACE)
    assert(aShapeExplorer.more())
    assert(aShapeExplorer.current().isFace())
    aShapeExplorer.next()
    assert(not aShapeExplorer.more())

    model.end()

    #=========================================================================
    # End of test
    #=========================================================================
