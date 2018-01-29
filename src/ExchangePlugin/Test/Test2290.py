## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 5, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 50, 8)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_2_1"), model.selection("SOLID", "Box_1_1")])
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "Partition_1_1_3/Modified_Face_2_3"), model.selection("FACE", "Partition_1_1_1/Modified_Face_2_3")])
model.do()
# test export to XAO
anExportFeature = Part_1_doc.addFeature("Export")
anExportFeature.string("xao_file_path").setValue("Data/export2290.xao")
anExportFeature.string("file_format").setValue("XAO")
anExportFeature.string("ExportType").setValue("XAO")
anExportFeature.string("xao_author").setValue("me")
anExportFeature.string("xao_geometry_name").setValue("mygeom")
model.end()

assert anExportFeature.error() == ""
