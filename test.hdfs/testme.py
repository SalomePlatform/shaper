#!/usr/bin/env python3

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

if __name__ == '__main__':

  import subprocess
  from time import sleep
  import sys, os
  import tempfile

  testTimeout = 600
  if len(sys.argv) > 3:
    testTimeout = int(sys.argv[1])
    hdffile = sys.argv[2]
    testdatafile = sys.argv[3]
  else:
    hdffile = sys.argv[1]
    testdatafile = sys.argv[2]

  tempfile = tempfile.NamedTemporaryFile()
  hdffile_basename = os.path.basename(hdffile)
  test_hdfpy = "test_hdf.py"
  if not os.path.exists(test_hdfpy):
    # add absolute path in SHAPER install directory
    test_hdfpy = os.path.join(os.getenv("SHAPER_ROOT_DIR"), "bin", "salome", "test", "HDFs", test_hdfpy)
    if not os.path.exists(test_hdfpy):
      raise Exception("test_hdf.py could not be found. Check your environment.")
  testlogfile = tempfile.name + "_" + hdffile_basename.replace(".", "_")
  tempfile.close()

  isOk = True
  error = ""

  proc = subprocess.Popen(["runSalome.py", "--modules", "SHAPER,GEOM,SHAPERSTUDY", "--gui", "--splash", "0", test_hdfpy, "args:" + hdffile + "," + testdatafile + "," + testlogfile])
  try:
    proc.communicate(timeout = testTimeout)
  except subprocess.TimeoutExpired:
    isOk = False
    import salome_utils
    port = salome_utils.getPortNumber()
    import killSalomeWithPort
    killSalomeWithPort.killMyPort(port)
    error = "Killed by CPU limit."

  assert isOk, "Test failed. {}".format(error)

  with open(testlogfile, 'r') as inputFile:
    s = inputFile.read()
    #print("logfile: ", s)
    if s.find("FAIL") > 0:
      isOk = False
      error = s
    elif s.find("OK") < 0:
      isOk = False
      error = "Test not ended until OK. Maybe a SIGSEGV."

  try:
    os.remove(testlogfile)
  except:
    pass

  assert isOk, "Test failed. {}".format(error)
