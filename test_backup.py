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

import subprocess
import sys, os
#import tempfile


#------------------------------------------------------------------------
def checkFileInSubprocess(logFile, title, args):
  errCode = 0
  try:
    logFile.write(f"starting {title} process:\n")
    logFile.write("  cmd_line = '{}'\n".format(" ".join(args)))
    my_env = os.environ.copy()
    my_env["QT_QPA_PLATFORM"] = "offscreen" # run SALOME in headless mode (=without GUI)
    proc = subprocess.Popen(args, env=my_env)
    try:
      logFile.write(f"  start communication with {title}\n")
      proc.communicate(timeout = 500)
      logFile.write(f"{title} terminated\n")
      errCode = proc.returncode
      logFile.write(f"{title} returned: {errCode}\n")
    except subprocess.TimeoutExpired:
      errCode = 96
      logFile.write(f"{title} timed out\n")
    except Exception as ex:
      errCode = 97
      logFile.write(f"Exception caught: {ex}\n")
    except:
      errCode = 98
      logFile.write("Unknown Exception caught\n")

    logFile.write(f"errCode = {errCode}\n")
  except:
    errCode = 99

  return errCode


#------------------------------------------------------------------------
def checkBackups(backupFolder, baseName, checkScript):
  errCode = 0
  try:
    # Create a log file in the backup folder starting with the same base name
    with open(os.path.join(backupFolder, baseName+"_test.log"), "w") as logFile:
      logFile.write("test_backup script started\n")
      logFile.write(f"  backupFolder = {backupFolder}\n")
      logFile.write(f"  baseName     = {baseName}\n")
      logFile.write(f"  checkScript  = {checkScript}\n")
      hdfFile = os.path.join(backupFolder, baseName+".hdf")
      logFile.write(f"  hdfFile      = {hdfFile}\n")

      args = ["runSalome.py", "--modules", "GEOM,SHAPER,SHAPERSTUDY,SMESH", "--batch", "--splash", "0", checkScript, "args:" + hdfFile]
      errCode = checkFileInSubprocess(logFile, "SALOME", args)
      logFile.write(f"errCode = {errCode}\n")

      if errCode == 0:
        pyFile = os.path.join(backupFolder, baseName+".py")
        logFile.write(f"  pyfile       = {pyFile}\n")

        args = ["python", checkScript, pyFile]
        errCode = checkFileInSubprocess(logFile, "PYTHON", args)
        logFile.write(f"errCode = {errCode}\n")

  except:
    errCode = 100

  return errCode


#------------------------------------------------------------------------
errCode = 0
try:
  if (len(sys.argv) != 4):
    errCode = 101
  else:
    backupFolder = sys.argv[1]
    baseName = sys.argv[2]
    checkScript = sys.argv[3]
    errCode = checkBackups(backupFolder, baseName, checkScript)
except:
  errCode = 102

exit(errCode)
