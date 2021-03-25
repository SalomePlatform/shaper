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

from ModelAPI import *

aSession = ModelAPI_Session.get()

# Import a reference part
aSession.startOperation("Add part")
aPartFeature = aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()
aPart = aSession.activeDocument()

# Export a part
aSession.startOperation("Export part")
anExportFeature = aPart.addFeature("Export")
anExportFeature.string("file_format").setValue("BREP")
anExportFeature.string("file_path").setValue("file.brep")
anExportFeature.string("ExportType").setValue("Regular")
aSelectionListAttr = anExportFeature.selectionList("selection_list")
aSelectionListAttr.setSelectionType("solids")
aSelectionListAttr.append("PartSet/OX", "EDGE")
aSession.finishOperation()
assert(anExportFeature.error() != "")
