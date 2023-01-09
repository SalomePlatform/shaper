# -*- coding: utf-8 -*-
# Copyright (C) 2016-2022  CEA/DEN, EDF R&D
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

"""Obtention des surfaces médianes à partir d'une CAO contenue dans un fichier

On sait traiter les faces :
  . planes
  . cylindriques
  . sphériques
  . toriques
  . coniques

Author: Gérald NICOLAS
"""
__revision__ = "V02.04"

import os

from salome.shaper import model

import ModelAPI

from macros.midSurface.surfaceMediane import SurfaceMediane

class midSurface(model.Feature):
    """Création des fibres neutres"""

# Feature initializations

    def __init__(self):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        model.Feature.__init__(self)

    @staticmethod
    def ID():
        """Return Id of the Feature."""
        return "midSurface"

    @staticmethod
    def FILE_ID():
        """Returns ID of the CAD file."""
        return "file_path"

    def getKind(self):
        """Override Feature.getKind()"""
        return midSurface.ID()


#====================================================================================
# Initialization of the dialog panel

    def initAttributes(self):
        """Override Feature.initAttributes()"""
        # Creating the input argument of the feature
        self.data().addAttribute(self.FILE_ID(), ModelAPI.ModelAPI_AttributeString_typeId())

#====================================================================================
# Execution

    def execute(self):
        """F.execute() -- execute the Feature"""
        # Retrieving the user input
        apath    = self.string(self.FILE_ID())

        filepath = apath.value()
        #print("filepath : '{}'".format(filepath))
        if filepath != "" :
            if os.path.isfile(filepath):
                # Lancement du script de création des fibres neutres
                l_options = list()
                #l_options.append("-v")
                l_options.append("-vmax")
                #l_options.append("-export_step")
                print("l_options : '{}'".format(l_options))
                s_med = SurfaceMediane(l_options)
                with open("/tmp/grr_1", "w") as fic :
                      fic.write("{}".format(dir(s_med)))
                      fic.write("\n{}".format(filepath))
                erreur, message = s_med.surf_fic_cao (filepath)
                with open("/tmp/grr_2", "w") as fic :
                      fic.write("erreur = {}, message = '{}'".format(erreur, message))
                del s_med
                if erreur:
                    self.setError(message)
            else:
                self.setError("The file '{}' does not exist".format(filepath))

        return

    def isMacro(self):
        """Override Feature.initAttributes().
        F.isMacro() -> True

        midSurface feature is macro: removes itself on the creation transaction
        finish.
        """
        return False
