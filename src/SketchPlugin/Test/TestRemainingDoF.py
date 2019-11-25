# Copyright (C) 2019  CEA/DEN, EDF R&D
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

from EventsAPI import *
from ModelAPI import *

class FreeShapesListener(EventsAPI.Events_Listener):
    def __init__(self):
        Events_Listener.__init__(self)
        # register as a listener
        Events_Loop.loop().registerListener(self, Events_Loop.eventByName("DoFObjects"))
        self.myEventProcessed = False

    def __del__(self):
        Events_Loop.loop().removeListener(self)

    def processEvent(self, theMessage):
        message = messageToUpdatedMessage(theMessage)
        objs = message.objects()
        assert(len(objs) == 1)
        assert(objectToFeature(objs[0]).getKind() == "SketchCircle")
        self.myEventProcessed = True


if __name__ == "__main__":
    # create the listener
    listener = FreeShapesListener()

    model.begin()
    partSet = model.moduleDocument()
    Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
    SketchCircle_1 = Sketch_1.addCircle(-30, 0, 14)
    SketchLine_1 = Sketch_1.addLine(-30, 0, 0, 0)
    SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_1.startPoint())
    SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "Origin"), False)
    SketchPoint_1 = SketchProjection_1.createdFeature()
    SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
    SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
    SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 30)
    model.end()

    # send message to find the free shapes in the sketch
    event = Events_Loop.eventByName("GetDoFObjects")
    ModelAPI_EventCreator.get().sendUpdated(Sketch_1.feature(), event);
    Events_Loop.loop().flush(event);

    assert(listener.myEventProcessed)
