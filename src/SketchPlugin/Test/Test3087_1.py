# Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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
        assert(len(objs) == 3)
        self.myEventProcessed = True


if __name__ == "__main__":
    # create the listener
    listener = FreeShapesListener()

    model.begin()
    partSet = model.moduleDocument()
    Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
    SketchLine_1 = Sketch_1.addLine(10, 10, -10, 10)
    SketchLine_2 = Sketch_1.addLine(-10, 10, -10, -10)
    SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
    SketchLine_3 = Sketch_1.addLine(-10, -10, 10, -10)
    SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
    SketchConstraintRigid_1 = Sketch_1.setFixed(SketchLine_1.startPoint())
    SketchConstraintRigid_2 = Sketch_1.setFixed(SketchLine_3.endPoint())
    model.end()

    # send message to find the free shapes in the sketch
    event = Events_Loop.eventByName("GetDoFObjects")
    ModelAPI_EventCreator.get().sendUpdated(Sketch_1.feature(), event);
    Events_Loop.loop().flush(event);

    assert(listener.myEventProcessed)
    # explicitly remove the listener to improve code coverage
    listener.__del__()
