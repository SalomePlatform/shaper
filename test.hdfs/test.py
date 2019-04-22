#!/usr/bin/env python
if __name__ == '__main__':

  from subprocess import Popen
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

  # ===========================================
  dbgFile = open(os.getcwd() + "/dbgfile", 'w')
  print("TEST start", file=dbgFile)
  dbgFile.close()
  # ===========================================
  proc = Popen([salomeKernelDir + "/bin/salome/runSalome.py", "--modules", "SHAPER,GEOM", "--gui", "--ns-port-log=" + portlogfile, sourceDir + "/test_hdf.py", "args:" + testfile + "," + portlogfile + "," + testlogfile + "," + salomeKernelDir + "," + sourceDir])
  # ===========================================
  dbgFile = open(os.getcwd() + "/dbgfile", 'a')
  print("TEST Salome started", file=dbgFile)
  dbgFile.close()
  # ===========================================

  iter = 0
  while not os.path.exists(portlogfile) and iter < 100:
    sleep(0.1)
    iter += 1

  while os.path.exists(portlogfile):
    sleep(0.1)

  # ===========================================
  dbgFile = open(os.getcwd() + "/dbgfile", 'a')
  print("TEST process log file", file=dbgFile)
  dbgFile.close()
  # ===========================================
  isOk = True
  with open(testlogfile, 'r') as inputFile:
    s = inputFile.read()
    print(s)
    for line in s:
      isOk = isOk and s.find("FAIL") < 0
  try:
    os.remove(testlogfile)
  except:
    pass

  # ===========================================
  dbgFile = open(os.getcwd() + "/dbgfile", 'a')
  print("TEST finished {}".format(isOk), file=dbgFile)
  dbgFile.close()
  # ===========================================
  assert isOk, "Test failed"
