# Copyright (C) 2007-2021  CEA/DEN, EDF R&D
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

#!/usr/bin/env python3

import unittest, sys, os
import subprocess

testTimeout = 600

class SalomeSession(object):
    def __init__(self, script):
        isOk = True
        proc = subprocess.Popen(["runSalome.py", "--modules", "SHAPER,GEOM,SHAPERSTUDY", "--gui", "--splash", "0", script], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        try:
            out, err = proc.communicate(timeout = testTimeout)
        except TimeoutExpired:
            isOk = False
            proc.kill()
            out, err = proc.communicate()
            error = "Killed by CPU limit."
            pass
        print(err)

        assert isOk, "Test failed. {}".format(error)

        # # import runSalome
        # # run_script = "runSalome.py"
        # # if sys.platform == 'win32':
            # # module_dir = os.getenv("KERNEL_ROOT_DIR")
            # # if module_dir: run_script = os.path.join(module_dir, "bin", "salome", run_script)
            # # pass
        # # sys.argv  = [run_script]
        # # sys.argv += ["--gui"]
        # # sys.argv += ["--splash=0"]
        # # sys.argv += ["--modules=SHAPER,GEOM,SHAPERSTUDY,SMESH"]
        # # sys.argv += ["%s" % script]
        # # if sys.platform == 'win32':
            # # main_module_path = sys.modules['__main__'].__file__
            # # sys.modules['__main__'].__file__ = ''
        # # clt, d = runSalome.main()
        # # if sys.platform == 'win32':
            # # sys.modules['__main__'].__file__ = main_module_path
        return

    def __del__(self):
        port = os.getenv('NSPORT')
        import killSalomeWithPort
        killSalomeWithPort.killMyPort(port)
        return
    pass

class MyTest(unittest.TestCase):
    def testFunction(self):
        SalomeSession(sys.argv[1])
    pass

unittest.main(argv=sys.argv[:1])
