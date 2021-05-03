# -*- coding: utf-8 -*-
# Copyright (C) 2016-2021  CEA/DEN, EDF R&D
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

"""compound of vertices Feature
Author: Nathalie Gore
"""
import os

from salome.shaper import model

import ModelAPI

class compoundVertices(model.Feature):
    """Import of Construction points
    """

# Feature initializations

    lfeatures = list()
    folder = None
    separator = " "

    def __init__(self):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        model.Feature.__init__(self)

    @staticmethod
    def ID():
        """Return Id of the Feature."""
        return "compoundVertices"

    @staticmethod
    def FILE_ID():
        """Returns ID of the file."""
        return "file_path"

    @staticmethod
    def SEPARATOR_ID():
        """Returns ID of the separator parameter."""
        return "separator"

    def getKind(self):
        """Override Feature.getKind()"""
        return compoundVertices.ID()


# Initialization of the dialog panel

    def initAttributes(self):
        """Override Feature.initAttributes()"""
        # Creating the input argument of the feature
        self.data().addAttribute(self.FILE_ID(), ModelAPI.ModelAPI_AttributeString_typeId())
        self.data().addAttribute(self.SEPARATOR_ID(), ModelAPI.ModelAPI_AttributeString_typeId())

# Execution of the Import

    def execute(self):
        """F.execute() -- execute the Feature"""
        # Retrieving the user input
        apath    = self.string(self.FILE_ID())
        aseparator = self.string(self.SEPARATOR_ID()).value()
        if aseparator:
            self.separator = aseparator

        filepath = apath.value()
        #print("filepath : '{}'".format(filepath))
        if filepath != "" :
            if os.path.isfile(filepath):
                part = model.activeDocument()
                if self.lfeatures :
                    for feature in self.lfeatures:
                        part.removeFeature(feature.feature())
                    self.lfeatures = list()
                    model.removeFolder(self.folder)

                filename = os.path.basename(filepath)
                nameRes = "Points_" + filename

                # Creating the construction points in the current document
                lVertices = list()

                with open(filepath) as fic:
                    for line in fic:
                        coord = line.split(self.separator)
                        if len(coord) != 3:
                            return
                        point = model.addPoint(part, float(coord[0]),float(coord[1]),float(coord[2]))
                        point.execute(True)
                        self.lfeatures.append(point)
                        #vertex = model.addVertex(part, [point.result()]); vertex.execute(True); self.lfeatures.append(vertex)
                        lVertices.append(point.result())
                    fic.close()
                    compound = model.addCompound(part, lVertices)
                    compound.execute(True)
                    self.lfeatures.append(compound)
                    compound.result().setName(nameRes)
                    self.folder = model.addFolder(part, self.lfeatures[0], compound)
                    self.folder.setName(nameRes)
            else:
                self.setError("The file '{}' does not exist".format(filepath))

        return

    def isMacro(self):
        """Override Feature.initAttributes().
        F.isMacro() -> True

        compoundVertices feature is macro: removes itself on the creation transaction
        finish.
        """
        return False
