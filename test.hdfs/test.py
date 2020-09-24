#!/usr/bin/env python
if __name__ == '__main__':

  import subprocess
  from time import sleep
  import sys, os

  salomeKernelDir = sys.argv[1]
  sourceDir = sys.argv[2]
  testfile = sys.argv[3]

  portlogfile = os.getcwd() + "/.salome_port"
  testlogfile = os.getcwd() + "/test.log"
  # remove port file if any
  try:
    os.remove(portlogfile)
  except:
    pass

  isOk = True
  error = ""

  proc = subprocess.Popen([salomeKernelDir + "/bin/salome/runSalome.py", "--modules", "SHAPER,GEOM", "--gui", "--splash", "0", "--ns-port-log=" + portlogfile, sourceDir + "/test_hdf.py", "args:" + testfile + "," + portlogfile + "," + testlogfile + "," + salomeKernelDir + "," + sourceDir], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
  try:
    proc.communicate(timeout = 600)
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
