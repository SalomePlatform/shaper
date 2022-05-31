# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
from tempfile import TemporaryDirectory
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Point_2 = model.addPoint(partSet, 1, 2, 3)
Point_name = Point_2.name()
model.end()

# check save document in a current folder
with TemporaryDirectory() as tmp_dir:
  aSession = ModelAPI_Session.get()
  aFiles = StringList()
  aSession.save(tmp_dir, aFiles)
  assert(len(aFiles) == 1)

# check open of the same document
  assert(aSession.load(tmp_dir) == False) # error because this document is already opened

# close the current document
  aSession.closeAll()

# open again: it must be correct now
  assert(aSession.load(tmp_dir))

# check the created point is opened
  partSet = model.moduleDocument()
  assert(partSet.size("Features") == 1)
  assert(partSet.object("Features", 0).data().name() == Point_name)
