# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

"""Package for dumping purposes.
"""

import ModelHighAPI

import ModelAPI
import SketchAPI

import sys
import inspect

def singleton(cls):
    instance = cls()
    instance.__call__ = lambda: instance
    return instance


## @ingroup CPPHighAPI
#  Collect information about features that may be dumped and stores the model as a Python script
@singleton
class DumpAssistant(ModelHighAPI.ModelHighAPI_Dumper):

    ## Constructor
    def __init__(self):
        ModelHighAPI.ModelHighAPI_Dumper.__init__(self)
        ModelHighAPI.ModelHighAPI_Dumper.setInstance(self)
        self.collectFeatures()
        self.myEngine = None

    ## Collect feature wrappers, which allow dumping (have method dump)
    def collectFeatures(self):
        self.myFeatures = {}
        self.myWrapperNames = {}
        for aModule in sys.modules:
            for aName, anObj in inspect.getmembers(sys.modules[aModule], inspect.isclass):
                if issubclass(anObj, ModelHighAPI.ModelHighAPI_Interface) and hasattr(anObj, "ID") and anObj.dump != ModelHighAPI.ModelHighAPI_Interface.dump:
                    self.myFeatures[anObj.ID()] = anObj
                    self.myWrapperNames[anObj.ID()] = aName

    ## Create wrapper for a given feature and dump it
    def dumpFeature(self, theFeature, theForce):
        aDumper = None
        aFeatureKind = theFeature.getKind()
        if aFeatureKind in self.myFeatures:
            # Dump only feature created by user (in history).
            # Also dump Export and RemoveResults features (hard-coded here in order not to change the data model).
            # For all other features, just keep their name.
            if theForce or theFeature.isInHistory() or aFeatureKind=="Export" or aFeatureKind=="RemoveResults":
                aDumper = self.myFeatures[aFeatureKind](theFeature)
                # Dump comment for the operation before the dumping of the feature to improve the readability of a script.
                if self.dumpCommentBeforeFeature(theFeature):
                    self.__print__("\n### Create " + theFeature.getKind())
                    self.newline()
            else:
                self.name(theFeature)
                self.clearNotDumped()
        else:
            # Probably the feature is a constraint, try to dump it with SketchAPI_Constraint.
            # In case of theFeature is not a constraint, it will not be dumped.
            self.name(theFeature, False, True, True)
            aDumper = self.myFeatures[SketchAPI.SketchAPI_Constraint.ID()](theFeature)
        if aDumper is not None:
            aDumper.dump(self)

    ## Create wrapper for a folder and dump it
    def dumpFolder(self, theFolder):
        if theFolder.ID() in self.myFeatures:
            self.myFeatures[theFolder.ID()](theFolder).dump(self)

    ## Dump all parameters
    def dumpParameter(self, theFeature):
        aFeatureKind = theFeature.getKind()
        if aFeatureKind == "Parameter" and aFeatureKind in self.myFeatures:
            self.myFeatures[aFeatureKind](theFeature).dump(self)

    ## Return getter for specified attribute
    def attributeGetter(self, theFeature, theAttrName):
        aFeatureKind = theFeature.getKind()
        if aFeatureKind in self.myFeatures:
            return self.myFeatures[aFeatureKind](theFeature).attributeGetter(theAttrName)
        return std_string()

    ## Return name of wrapper feature
    def featureWrapper(self, theFeature):
        aFeatureKind = theFeature.getKind()
        if aFeatureKind in self.myWrapperNames:
            return self.myWrapperNames[aFeatureKind]
        return std_string()

    ## Exports the dumped variables names to the SHAPERSTUDY
    def exportVariable(self, theEntry, theVarName):
        try:
          if self.myEngine == None:
            import SHAPERSTUDY_utils
            aComponent = SHAPERSTUDY_utils.findOrCreateComponent()
            self.myEngine = SHAPERSTUDY_utils.getEngine()
          if self.myEngine != 1:
            self.myEngine.StoreVariableName(theEntry, theVarName)
        except:
          self.myEngine = 1 # invalid access
        pass

# Instance of dumper
dumper = DumpAssistant
