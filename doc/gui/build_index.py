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

###
## The script collects information about help documents in plugins
## and prepares building of help documentation by sphinx
###

import os
import sys
import shutil
from xml.dom.minidom import parse

aBuildDir = sys.argv[1]
aSourcesDir = sys.argv[2]
aSrcPath = aSourcesDir + os.sep + "../../src"

aConfigPath = aSrcPath + os.sep + "Config/plugins.xml.in"

def findDir(theConfFile):
    """Find a name of a directory where the given config file exists"""
    aSrcList = os.listdir(aSrcPath)
    for aDir in aSrcList:
        aPath = aSrcPath + os.sep + aDir
        aConfPath = aPath + os.sep + theConfFile
        if os.path.isdir(aPath) and (os.path.exists(aConfPath) or os.path.exists(aConfPath + ".in")):
            return aDir
    return None

## Find accessible plugins from plugins.xml configuration file
aPluginList = []
aDomObj = parse(aConfigPath)
aPluginsList = aDomObj.getElementsByTagName("plugin")
for plugin in aPluginsList:
    aLibName = plugin.getAttribute("library")
    if not aLibName:
        aLibName = plugin.getAttribute("script")
    aConfigFile = plugin.getAttribute("configuration")

    if aLibName and aConfigFile:
        aLibDir = findDir(aConfigFile)
        if not aLibDir is None:
            aPluginDocDir = aSrcPath + os.sep + aLibDir + os.sep + "doc"
            if os.path.exists(aPluginDocDir):
                ## Copy all files to a building directory
                aDocDist = aBuildDir + os.sep + aLibName
                if os.path.exists(aDocDist):
                    shutil.rmtree(aDocDist)
                shutil.copytree(aPluginDocDir, aDocDist)
                aPluginList.append(aLibName)

## Modify index.rst file accordingly
aIndexFile = open(aSourcesDir + os.sep + "index.rst.in", 'r')
aIndexLines = aIndexFile.readlines()
aIndexFile.close()

## Add list of plugins after toctree directive
aIndexLines.append('\n')
for aLibName in aPluginList:
    aIndexLines.append("   " + aLibName + "/" + aLibName + ".rst\n")

aNewIndex = open(aBuildDir + os.sep + "index.rst", 'w')
aNewIndex.writelines(aIndexLines)
aNewIndex.close()
