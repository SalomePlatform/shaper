#!/usr/bin/env python3

# Copyright (C) 2024-2026  OPEN CASCADE SAS
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

import salome
import sys
import os
import SalomePyQt
import ModelAPI
from salome.shaper import model


#------------------------------------------------------------------------
def validateSession(logFile):
  """
  Iterate through and write all feature names of each part in the active study
  to the logFile.
  """
  errCode = 0
  try:
    logFile.write("getting SHAPER session\n")
    session = ModelAPI.ModelAPI_Session.get()
    # aFactory = session.validators()
    logFile.write("getting PartSet\n")
    aPartSet = session.moduleDocument()
    numParts = aPartSet.size("Parts")
    for partIdx in range(numParts+1):
      aPart = session.document(partIdx)
      if aPart is None:
        continue
      aPart.setActive(True)
      if partIdx == 0:
        logFile.write("---PartSet:------------------\n")
      else:
        logFile.write(f"---Part_{partIdx}:------------------\n")

      for aFeat in aPart.allFeatures():
        if aFeat.isInHistory():
          logFile.write("  * {} --> [{}]\n".format(aFeat.getKind(), aFeat.data().name()))
        else:
          logFile.write("    - {}\n".format(aFeat.data().name()))
  except:
    errCode = 128

  return errCode


#------------------------------------------------------------------------
def checkHDF(logFile, hdfFile):
  """
  Open and validate a HDF document
  """
  errCode = 0
  try:
    logFile.write("-----CHECKING HDF DOCUMENT-------\n")
    salome.standalone()
    logFile.write("initializing SALOME\n")
    salome.salome_init(hdfFile, embedded=True, forced=True)
    logFile.write("StudyName = {}\n".format(salome.myStudyName))

    logFile.write("getting KERNEL session\n")
    session = salome.naming_service.Resolve('/Kernel/Session')
    logFile.write("connecting to study\n")
    session.emitMessage("connect_to_study")

    logFile.write("activating SHAPER module\n")
    sg = SalomePyQt.SalomePyQt()
    sg.activateModule("Shaper")

    errCode = validateSession(logFile)
  except Exception as ex:
    errCode = 129
    logFile.write(f"Exception caught: {ex}\n")
  except:
    errCode = 129

  return errCode


#------------------------------------------------------------------------
def checkPyScript(logFile, pyFile):
  """
  Load and validate a Python script
  """
  errCode = 0
  try:
    logFile.write("-----CHECKING PYTHON SCRIPT-------\n")
    logFile.write("executing dumped script\n")
    try:
      exec(compile(open(pyFile, 'rb').read(), pyFile, 'exec'))
    except NameError as nex:
      logFile.write(f"WARNING: NameError: {nex}\n")
      # Ignore a NameError which can come from the SMESM module using a Result which has been concealed.
      # This problem can be solved manually by calling model.publishToShaperStudy() right after the
      # feature creation, moving its mesh creation code after the published result, and finally, continue
      # with the SHAPER script.
      # For example, if the script contains (lines are deleted for clarity):
      #    model.begin()
      #    #..create feature #1
      #    #..create feature #2
      #    #..create feature #3
      #    model.end()
      #    model.publishToShaperStudy()
      #    Feat_3_1, = SHAPERSTUDY.shape(model.featureStringId(Feat_3))
      #    smesh = smeshBuilder.New()
      #    Mesh_1 = smesh.Mesh(Feat_1_1,'Feat_1_1') # <-- this line will fail
      #    Mesh_1.Compute()
      #
      # It should be changed to:
      #    model.begin()
      #    #..create feature #1
      #    model.end()
      #    model.publishToShaperStudy()
      #    Feat_1_1, = SHAPERSTUDY.shape(model.featureStringId(Feat_1))
      #    smesh = smeshBuilder.New()
      #    Mesh_1 = smesh.Mesh(Feat_1_1,'Feat_1_1')
      #    Mesh_1.Compute()
      #    model.begin()
      #    #..create feature #2
      #    #..create feature #3
      #    model.end()
      #    model.publishToShaperStudy()
      #    Feat_3_1, = SHAPERSTUDY.shape(model.featureStringId(Feat_3))

    errCode = validateSession(logFile)
  except Exception as ex:
    errCode = 130
    logFile.write(f"Exception caught: {ex}\n")
  except:
    errCode = 130
    logFile.write(f"Unknownd exception caught\n")

  return errCode


#------------------------------------------------------------------------
def validateBackup(fullName):
  """
  Open the backed up file and validate its content.
  This test script will be called once for the stored HDF file
  end once for the dumped Python script.
  """
  errCode = 0
  try:
    outFolder,filename = os.path.split(fullName)
    base,ext = os.path.splitext(filename)
    ext = ext[1:].lower() # remove the dot from the extension

    # Create a log file in the backup folder starting with the same base name
    logFile = os.path.join(outFolder, base+"_valid.log")
    if os.path.exists(logFile):
        append_write = 'a' # append if already exists
    else:
        append_write = 'w' # make a new file if not

    with open(logFile, append_write) as f:
      try:
        f.write(f"fullName = {fullName}\n")
        if ext == "hdf":
          errCode = checkHDF(f, fullName)
        elif ext == "py":
          errCode = checkPyScript(f, fullName)
        else:
          f.write(f"-----UNSUPPORTED FILE TYPE [{ext}]-------\n")
          errCode = 131
        f.write(f"errCode = {errCode}\n")
        if errCode == 0:
          f.write("{} Test - PASSED\n\n".format(ext.upper()))
        else:
          f.write("{} Test - FAILED\n\n".format(ext.upper()))
      except Exception as ex:
        errCode = 132
        f.write(f"Exception caught: {ex}\n")
      except:
        errCode = 132
        f.write("Unknown exception caught\n")
  except:
    errCode = 133

  return errCode


#------------------------------------------------------------------------
errCode = 0
try:
  if (len(sys.argv) != 2):
    errCode = 134
  else:
    fullName = sys.argv[1]
    errCode = validateBackup(fullName)
except:
  errCode = 135

exit(errCode)
