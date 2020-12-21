#!/usr/bin/env python3

# Copyright (C) 2020  CEA/DEN, EDF R&D
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

  tempdir = tempfile.gettempdir()
  portlogfile = tempdir + "/.salome_port"
  testlogfile = tempdir + "/test.log"
  # remove port file if any
  try:
    os.remove(portlogfile)
  except:
    pass

  isOk = True
  error = ""

  proc = subprocess.Popen(["runSalome.py", "--modules", "SHAPER,GEOM,SHAPERSTUDY", "--gui", "--splash", "0", "--ns-port-log=" + portlogfile, "test_hdf.py", "args:" + hdffile + "," + testdatafile + "," + portlogfile + "," + testlogfile], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
  try:
    proc.communicate(timeout = testTimeout)
  except TimeoutExpired:
    isOk = False
    proc.kill()
    out, err = proc.communicate()
    error = "Killed by CPU limit."
    print(err)

  with open(testlogfile, 'r') as inputFile:
    s = inputFile.read()
    print(s)
    isOk = isOk and s.find("FAIL") < 0
  try:
    os.remove(testlogfile)
  except:
    pass

  assert isOk, "Test failed. {}".format(error)
