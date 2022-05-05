# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

from salome.shaper import model
import codecs, sys

def changeTab(theLine):
    aResult = theLine.split("#")[0].replace("\t"," ")
    aResult += theLine[len(aResult):]
    return aResult

def importParameters(theDocument, theFileName):

    aResult = []
    try:
        aFile = codecs.open(theFileName, 'r', encoding = 'utf_8_sig')
    except IOError:
        return aResult

    for aLine in aFile:
        aLine = aLine.rstrip("\n")
        aLine = changeTab(aLine)

        aName = ""
        aParameter = ""
        aComment = ""

        aFirstText = aLine.split(" ")[0]

        aName = aFirstText.split("#")[0].strip()

        aLine = aLine.lstrip(aName)

        aParameter = aLine.split("#")[0]

        aLine = aLine.lstrip(aParameter)
        aLine = aLine.lstrip("#")

        aComment = aLine

        if(len(aName) > 0):
            try:
                aResult.append(model.addParameter(theDocument, aName, aParameter.strip(), aComment.strip()))
            except SyntaxError as anError:
                print(anError, file = sys.stderr)

    aFile.close()
    return aResult
