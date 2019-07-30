# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OX"), 20, 2, model.selection("EDGE", "PartSet/OY"), 20, 3)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], model.selection("EDGE", "PartSet/OX"), 50)
Filters = model.filters(Part_1_doc, [model.addFilter(name = "VerticalFaces")])
model.end()

Reference = {
    # Faces of the original box
    model.selection("FACE", "LinearCopy_1_1_1/MF:Translated&Box_1_1/Top"): False,
    model.selection("FACE", "LinearCopy_1_1_1/MF:Translated&Box_1_1/Bottom"): False,
    model.selection("FACE", "LinearCopy_1_1_1/MF:Translated&Box_1_1/Front"): True,
    model.selection("FACE", "LinearCopy_1_1_1/MF:Translated&Box_1_1/Back"): True,
    model.selection("FACE", "LinearCopy_1_1_1/MF:Translated&Box_1_1/Right"): True,
    model.selection("FACE", "LinearCopy_1_1_1/MF:Translated&Box_1_1/Left"): True,
    # Translated box
    model.selection("FACE", "LinearCopy_1_1_2/MF:Translated&Box_1_1/Top"): False,
    model.selection("FACE", "LinearCopy_1_1_2/MF:Translated&Box_1_1/Bottom"): False,
    model.selection("FACE", "LinearCopy_1_1_2/MF:Translated&Box_1_1/Front"): True,
    model.selection("FACE", "LinearCopy_1_1_2/MF:Translated&Box_1_1/Back"): True,
    model.selection("FACE", "LinearCopy_1_1_2/MF:Translated&Box_1_1/Right"): True,
    model.selection("FACE", "LinearCopy_1_1_2/MF:Translated&Box_1_1/Left"): True,
    # Box translated along another direction
    model.selection("FACE", "LinearCopy_1_1_3/MF:Translated&Box_1_1/Top"): False,
    model.selection("FACE", "LinearCopy_1_1_3/MF:Translated&Box_1_1/Bottom"): False,
    model.selection("FACE", "LinearCopy_1_1_3/MF:Translated&Box_1_1/Front"): True,
    model.selection("FACE", "LinearCopy_1_1_3/MF:Translated&Box_1_1/Back"): True,
    model.selection("FACE", "LinearCopy_1_1_3/MF:Translated&Box_1_1/Right"): True,
    model.selection("FACE", "LinearCopy_1_1_3/MF:Translated&Box_1_1/Left"): True,
    # Faces of the cylinder
    model.selection("FACE", "Translation_1_1/MF:Translated&Cylinder_1_1/Face_1"): True,
    model.selection("FACE", "Translation_1_1/MF:Translated&Cylinder_1_1/Face_2"): False,
    model.selection("FACE", "Translation_1_1/MF:Translated&Cylinder_1_1/Face_3"): False,
    # Edges and vertices are not applicable
    model.selection("EDGE", "[LinearCopy_1_1_1/MF:Translated&Box_1_1/Left][LinearCopy_1_1_1/MF:Translated&Box_1_1/Top]"): False,
    model.selection("VERTEX", "[LinearCopy_1_1_1/MF:Translated&Box_1_1/Back][LinearCopy_1_1_1/MF:Translated&Box_1_1/Right][LinearCopy_1_1_1/MF:Translated&Box_1_1/Bottom]"): False,
    }
model.checkFilter(Part_1_doc, model, Filters, Reference)
