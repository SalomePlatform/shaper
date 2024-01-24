# Copyright (C) 2020-2024  CEA, EDF
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
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *
import sys
import os

if __name__ == "__main__":
  session = ModelAPI.ModelAPI_Session.get()
  aPartSet = session.moduleDocument()

  for aPartIndex in range(aPartSet.size("Parts")):
    session.startOperation()
    aPart = ModelAPI.modelAPI_ResultPart(ModelAPI.objectToResult(aPartSet.object("Parts", aPartIndex)))
    aPart.activate()
    session.finishOperation()
     
    session.startOperation()
    session.setActiveDocument(aPartSet)
    session.finishOperation()
    
    Part_1_doc = aPart.partDoc()
    for aFeat in Part_1_doc.allFeatures():
      if aFeat.isInHistory():
        if(aFeat.data().name() == "Extrusion_1"):
          aSelectionList = aFeat.selectionList("base")
          for index in range(0, aSelectionList.size()):
            attrSelection = aSelectionList.value(index)
            shape = attrSelection.value()
            name = attrSelection.namingName()
            assert(name)
