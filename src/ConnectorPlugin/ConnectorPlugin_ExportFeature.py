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

## @package Plugins
#  ExportFeature class definition

import ModelAPI
import ExchangeAPI

import salome
from salome.geom import geomBuilder

from salome.shaper import model

import os

def getTmpFileName(ext):
    import tempfile
    tempdir = tempfile.gettempdir()
    tmp_file = tempfile.NamedTemporaryFile(suffix=".%s"%ext, prefix='shaper_', dir=tempdir, delete=False)
    tmp_filename = tmp_file.name
    if os.name == "nt":
        tmp_filename.replace("\\", "/")
    return tmp_filename

## @ingroup Plugins
#  Feature to export all shapes and groups into the GEOM module
class ExportFeature(ModelAPI.ModelAPI_Feature):

    ## The constructor.
    def __init__(self):
        ModelAPI.ModelAPI_Feature.__init__(self)
        pass

    @staticmethod
    ## Export kind. Static.
    def ID():
        return "ExportToGEOM"

    ## Returns the kind of a feature.
    def getKind(self):
        return ExportFeature.ID()

    ## This feature is action: has no property pannel and executes immediately.
    def isAction(self):
        return True

    ## This feature has no attributes, as it is action.
    def initAttributes(self):
      pass

    ## Export the results, groups and fields via XAO
    def exportViaXAO(self):
        tmpXAOFile = getTmpFileName("xao")
        self.tmpXAOFile = tmpXAOFile
        #print "Export to %s"%tmpXAOFile
        exportXAO = ExchangeAPI.exportToXAO(self.Part, tmpXAOFile, "automatic_shaper_export_to_XAO")
        if not os.path.exists(tmpXAOFile) or os.stat(tmpXAOFile).st_size == 0:
            exportXAO.feature().setError("Error in exportToXAO. No XAO file has been created.")
            return
        imported, shape, subShapes, groups, fields = self.geompy.ImportXAO(tmpXAOFile)
        self.geompy.addToStudy( shape, shape.GetName() )
        # add sub-shapes and groups to the object browser
        for obj in subShapes + groups:
            name = obj.GetName()
            self.geompy.addToStudyInFather(shape, obj, name)
        # add fields to the object browser
        for field in fields:
            name = field.GetName()
            self.geompy.addToStudyInFather(shape, field, name)
            # add steps to the object browser
            steps = field.getSteps()
            for i_step in steps:
                step = field.getStep(i_step)
                i_stamp = step.GetStamp()
                step_name = "Step %i %i"%(i_step, i_stamp)
                self.geompy.addToStudyInFather( field, step, step_name )
        # Remove the temporary file
        os.remove(tmpXAOFile)
        pass

    ## Exports all shapes and groups into the GEOM module.
    def execute(self):
        aSession = ModelAPI.ModelAPI_Session.get()
        ## Get active document
        self.Part = aSession.activeDocument()
        ## List of objects created in the old geom for later use
        self.geomObjects = []
        ## geomBuilder tool
        salome.salome_init(1)
        self.geompy = geomBuilder.New()

        self.exportViaXAO()

        pass
