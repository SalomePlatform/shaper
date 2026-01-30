# Copyright (C) 2014-2026  CEA, EDF
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
    TestRectangle.py
    Unit test of SketchPlugin_Rectangle class (centered rectangle case)

"""
import sys
from pathlib import Path
import subprocess as sp
import TestRectangleCentered1Slave

nbTurns = 10

for i in range(nbTurns):
    cmd = ["python3", TestRectangleCentered1Slave.__file__]
    proc = sp.Popen( cmd )
    proc.communicate()
    if proc.returncode == 0:
        # to keep track of specificity of this test
        print( f"Sucessful Wraped launch in {__file__}" )
        sys.exit( 0 )
    # 2nd chance
    print( f'{100*">"}' )
    print( f'{100*">"} {i}# try of {Path(TestRectangleCentered1Slave.__file__).name} fails. Try one more time' )
    print( f'{100*">"}' )

raise RuntimeError(f"Subprocess finished  {Path(TestRectangleCentered1Slave.__file__).name} with a non zero status ({proc.returncode}). Command returning non zero status was : {cmd} after {nbTurns} turns")
