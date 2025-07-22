# Copyright (C) 2021-2025  CEA, EDF
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

import os
from salome.shaper import model

import salome
salome.standalone()
salome.salome_init(embedded=True)

data_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")

def testImportSTL():
    model.begin()
    partSet = model.moduleDocument()

    ### Create Part
    Part_1 = model.addPart(partSet)
    Part_1_doc = Part_1.document()

    ### Create Import
    Import_1 = model.addImport(Part_1_doc, os.path.join(data_dir, "cube_ascii.stl"))
    model.do()
    Import_1.setName("cube_ascii")
    Import_1.result().setName("cube_ascii")
    BoundingBox_1 = model.getBoundingBox(Part_1_doc, model.selection("SOLID", "cube_ascii"))

    Import_2 = model.addImport(Part_1_doc, os.path.join(data_dir, "cube_binary.stl"))
    model.do()
    Import_2.setName("cube_binary")
    Import_2.result().setName("cube_binary")

    Import_3 = model.addImport(Part_1_doc, os.path.join(data_dir, "open_cube_ascii.stl"))
    model.do()
    Import_3.setName("cube_open_ascii")
    Import_3.result().setName("cube_open_ascii")

    Import_4 = model.addImport(Part_1_doc, os.path.join(data_dir, "open_cube_binary.stl"))
    model.do()
    Import_4.setName("cube_open_binary")
    Import_4.result().setName("cube_open_binary")

    model.end()

    #=============================================================================
    # Tests :
    #=============================================================================
    # ASCII STL file :
    model.checkResult(Import_1, model, 1, [0], [1], [12], [36], [72])
    r=Import_1.defaultResult()
    s=r.shape()
    dim=s.computeSize()
    dim=dim[1:]
    dx=abs(dim[3]-dim[0])
    dy=abs(dim[4]-dim[1])
    dz=abs(dim[5]-dim[2])
    tol=1e-06
    assert(abs(dx-14) <= tol)
    assert(abs(dy-8) <= tol)
    assert(abs(dz-5) <= tol)
    model.testResultsVolumes(Import_1, [560.0000222], theNbSignificantDigits = 7)

    # Binary STL file :
    model.checkResult(Import_2, model, 1, [0], [1], [12], [36], [72])
    r=Import_2.defaultResult()
    s=r.shape()
    dim=s.computeSize()
    dim=dim[1:]
    dx=abs(dim[3]-dim[0])
    dy=abs(dim[4]-dim[1])
    dz=abs(dim[5]-dim[2])
    assert(abs(dx-10) <= tol)
    assert(abs(dy-10) <= tol)
    assert(abs(dz-10) <= tol)
    model.testResultsVolumes(Import_2, [1000], theNbSignificantDigits = 7)

    # ASCII STL file (shell, open cube) :
    model.checkResult(Import_3, model, 1, [0], [0], [10], [30], [60])
    # Checks nb of unique shapes (shell included)
    model.testResults(Import_3, 1, [0], [1], [10], [17], [8])
    r=Import_1.defaultResult()
    s=r.shape()
    dim=s.computeSize()
    dim=dim[1:]
    dx=abs(dim[3]-dim[0])
    dy=abs(dim[4]-dim[1])
    dz=abs(dim[5]-dim[2])
    tol=1e-06
    assert(abs(dx-14) <= tol)
    assert(abs(dy-8) <= tol)
    assert(abs(dz-5) <= tol)

    # Binary STL file (shell, open cube) :
    model.checkResult(Import_4, model, 1, [0], [0], [10], [30], [60])
    # Checks nb of unique shapes (shell included)
    model.testResults(Import_4, 1, [0], [1], [10], [17], [8])
    r=Import_1.defaultResult()
    s=r.shape()
    dim=s.computeSize()
    dim=dim[1:]
    dx=abs(dim[3]-dim[0])
    dy=abs(dim[4]-dim[1])
    dz=abs(dim[5]-dim[2])
    tol=1e-06
    assert(abs(dx-14) <= tol)
    assert(abs(dy-8) <= tol)
    assert(abs(dz-5) <= tol)

if __name__ == '__main__':
    testImportSTL()
