# Copyright (C) 2025-2026  CEA, EDF
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
      TestExportToGLTF.py
"""
#=========================================================================
# Initialization of the test
#=========================================================================
import os
import json
from tempfile import TemporaryDirectory
from salome.kernel import salome


#=========================================================================
# Helper functions
#=========================================================================
def removeFile(theFileName):
    try: os.remove(theFileName)
    except OSError: pass
    assert not os.path.exists(theFileName), \
            "Can not remove file {0}".format(theFileName)

def checkGltfFormat(theFileName):
    print(f"Checking for GLTF: theFileName={theFileName}")
    # Check whether the file exists
    assert os.path.exists(theFileName), "Error: the gltf file does not exist"
    # Check whether the file is not empty
    file_size = os.path.getsize(theFileName)
    assert file_size > 0, "Error: the gltf file is empty"
    # Check the content of the file (GLTF is a JSON file)
    with open(theFileName, 'r') as aFile:
        aData = json.load(aFile)
    # Check some required fields
    assert "asset" in aData, "Error: the gltf file does not contain the 'asset' field"
    assert "version" in aData["asset"], "Error: the gltf file does not contain the 'version' field"
    assert "buffers" in aData, "Error: the gltf file does not contain the 'buffers' field"
    assert len(aData["buffers"]) > 0, "Error: the gltf file does not contain any buffer"
    # During our tests, we export a single shape, so we should have a single URI, which is an external binary file
    assert "uri" in aData["buffers"][0], "Error: the gltf file does not contain the 'uri' field"
    bin_file = theFileName.replace(".gltf", ".bin")
    assert aData["buffers"][0]["uri"] == os.path.basename(bin_file), "Error: the gltf file does not contain the correct 'uri' field"
    # Check whether the binary file exists
    assert os.path.exists(bin_file), "Error: the binary file does not exist"
    # Check whether the binary file is not empty
    bin_file_length = os.path.getsize(bin_file)
    assert bin_file_length > 0, "Error: the binary file is empty"
    # Check the minimum length of the buffer
    assert aData["buffers"][0]["byteLength"] <= bin_file_length, f"Error: the gltf file does not contain the correct 'byteLength' field [file length={bin_file_length}, min.expected={aData['buffers'][0]['byteLength']}]"

def checkGlbFormat(theFileName):
    print(f"Checking for GLB: theFileName={theFileName}")
    # Check whether the file exists
    assert os.path.exists(theFileName), "Error: the glb file does not exist"
    # Check whether the file is not empty
    file_size = os.path.getsize(theFileName)
    assert file_size > 0, "Error: the glb file is empty"
    # Check the content of the file (GLB is a binary file)
    with open(theFileName, 'rb') as aFile:
        header = aFile.read(20)
    # Check the magic number
    magic_GLTF = header[0:4]
    magic_JSON = header[16:20]
    assert magic_GLTF == b"glTF", f"Error: the glb file does not contain the correct magic number ['{magic_GLTF}' != 'glTF']"
    assert magic_JSON == b"JSON", f"Error: the glb file does not contain the correct JSON chunk ['{magic_JSON}' != 'JSON']"



#=========================================================================
# glTF/glb-specific export test function
#=========================================================================
def testExportGLTF(theFile):
    # Get the file extension
    ext = os.path.splitext(theFile)[1]
    is_binary_fmt = (ext == ".glb")

    # Ensure that the file to export does not exist
    removeFile(theFile)
    if not is_binary_fmt:
        # glTF format stores the shape in a separate binary file
        bin_file = theFile.replace(".gltf", ".bin")
        removeFile(bin_file)

    # Export the Part to glTF/glb
    model.exportToFile(Part_1_doc, theFile, [Box_1.result()])

    # Check the specific output formats for validity
    if is_binary_fmt:
        checkGlbFormat(theFile)
    else:
        checkGltfFormat(theFile)


#=========================================================================
# Main
#=========================================================================
# Initialize the SALOME session
salome.salome_init()

from salome.shaper import model

# Create a Part with a simple Box
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 100, 100, 100)
model.end()

with TemporaryDirectory() as tmp_dir:
    # Export a shape into glTF/glb
    testExportGLTF(os.path.join(tmp_dir, "export.gltf"))
    testExportGLTF(os.path.join(tmp_dir, "export.glb"))

    assert(model.checkPythonDump())
