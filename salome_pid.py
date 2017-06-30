#!/bin/env python

## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

import sys, pickle
from killSalome import *

process = sys.argv[2] #SALOME_Session_Server"

salome_port = sys.argv[1]
filedict = getPiDict(salome_port)
process_ids = pickle.load(open(filedict, 'r'))

pid = -1
for list_item in process_ids:
  for _pid, name_list in list_item.items():
    if process in name_list:
      pid = _pid

print pid
