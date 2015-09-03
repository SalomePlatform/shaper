#!/bin/env python

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
