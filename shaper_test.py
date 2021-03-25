#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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

import unittest

class TestShaper(unittest.TestCase):

    def setUp(self):
        import salome
        salome.salome_init()

    def test_shaper(self):
        """Quick test for Shaper module"""

        print()
        print('Testing Shaper module')

        from SketchAPI import SketchAPI_Point

        from salome.shaper import model

        model.begin()
        print('... Create Part document')
        partSet = model.moduleDocument()
        Part_1 = model.addPart(partSet)
        Part_1_doc = Part_1.document()
        self.assertIsNotNone(Part_1_doc)

        print('... Create Sketch')
        Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
        self.assertIsNotNone(Sketch_1)
        SketchLine_1 = Sketch_1.addLine(10, 10, -10, 10)
        SketchLine_2 = Sketch_1.addLine(-10, 10, -10, 30)
        SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
        SketchArc_1 = Sketch_1.addArc(10, 30, 10, 10, -10, 30, False)
        SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.startPoint())
        SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_2.endPoint())
        SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_1.result())
        SketchConstraintTangent_2 = Sketch_1.setTangent(SketchLine_2.result(), SketchArc_1.results()[1])
        SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_2.result())
        SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 20)
        SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
        SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
        SketchPoint_1 = SketchProjection_1.createdFeature()
        SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_1.endPoint(), 10)
        SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_1.endPoint(), SketchAPI_Point(SketchPoint_1).coordinates(), 10)
        model.do()
        self.assertEqual(len(Sketch_1.results()), 1)

        print('... Create Extrusion')
        Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), 10, 0)
        model.do()
        self.assertIsNotNone(Extrusion_1)
        self.assertEqual(len(Extrusion_1.results()), 1)
        self.assertEqual(Extrusion_1.result().shapeType(), 'SOLID')

        print('... Create Partition')
        Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("FACE", "PartSet/YOZ")])
        model.end()
        self.assertIsNotNone(Partition_1)
        self.assertEqual(len(Partition_1.results()), 1)
        self.assertEqual(Partition_1.result().shapeType(), 'COMPSOLID')
        self.assertEqual(Partition_1.result().numberOfSubs(), 2)

if __name__ == '__main__':
    unittest.main()
