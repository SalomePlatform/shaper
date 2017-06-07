#!/usr/bin/env python
import os

cppHeader=[
'// Copyright (C) 2014-2017  CEA/DEN, EDF R&D\n',
'//\n',
'// This library is free software; you can redistribute it and/or\n',
'// modify it under the terms of the GNU Lesser General Public\n',
'// License as published by the Free Software Foundation; either\n',
'// version 2.1 of the License, or (at your option) any later version.\n',
'//\n',
'// This library is distributed in the hope that it will be useful,\n',
'// but WITHOUT ANY WARRANTY; without even the implied warranty of\n',
'// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n',
'// Lesser General Public License for more details.\n',
'//\n',
'// You should have received a copy of the GNU Lesser General Public\n',
'// License along with this library; if not, write to the Free Software\n',
'// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA\n',
'//\n',
'// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>\n',
'//\n'
]

aDirList = [
'BuildAPI',
'BuildPlugin',
'CollectionAPI',
'CollectionPlugin',
'Config',
'ConnectorAPI',
'ConnectorPlugin',
'ConstructionAPI',
'ConstructionPlugin',
'Events',
'ExchangeAPI',
'ExchangePlugin',
'FeaturesAPI',
'FeaturesPlugin',
'GDMLAPI',
'GDMLPlugin',
'GeomAPI',
'GeomAlgoAPI',
'GeomAlgoImpl',
'GeomData',
'GeomDataAPI',
'GeomValidators',
'InitializationPlugin',
'Model',
'ModelAPI',
'ModelGeomAlgo',
'ModelHighAPI',
'ModuleBase',
'ParametersAPI',
'ParametersPlugin',
'PartSet',
'PartSetAPI',
'PartSetPlugin',
'PrimitivesAPI',
'PrimitivesPlugin',
'PythonAPI',
'PythonAddons',
'SHAPERGUI',
'SamplePanelPlugin',
'SketchAPI',
'SketchPlugin',
'SketchSolver',
'SketcherPrs',
'XGUI'
]

for aDir in aDirList:
    aFilesDir = os.listdir(aDir)
    for aFile in aFilesDir:
        if aFile.endswith(".cpp") or aFile.endswith(".h") or aFile.endswith(".i"):
            print("Processing", aFile)
            aFPath = aDir + os.sep + aFile
            aF = open(aFPath)
            aLines = aF.readlines()
            aF.close()
            aF = open(aFPath, "w")
            for aLine in cppHeader:
                aF.write(aLine)
            aF.write('\n')
            isWriting = False
            for aLine in aLines:
                if isWriting:
                    aF.write(aLine)
                else:
                    if len(aLine) > 1:
                        isWriting = not aLine.startswith('//')
                    if isWriting:
                        aF.write(aLine)
                    #print(aLine, len(aLine), isWriting)
            aF.close()