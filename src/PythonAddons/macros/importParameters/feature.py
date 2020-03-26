"""importParameters
Author: Nathalie Gore
"""

from salome.shaper import model
from salome.shaper import geom
import ModelAPI
import ParametersAPI

class importParameters(model.Feature):
    """Import of Construction points
    """

# Feature initializations

    def __init__(self):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        model.Feature.__init__(self)

    @staticmethod
    def ID():
        """Return Id of the Feature."""
        return "importParameters"

    @staticmethod
    def FILE_ID():
        """Returns ID of the file select parameter."""
        return "file_path"

    def getKind(self):
        """Override Feature.getKind()"""
        return importParameters.ID()


# Initialization of the dialog panel

    def initAttributes(self):
        """Override Feature.initAttributes()"""
        # Creating the input argument of the feature
        self.data().addAttribute(self.FILE_ID(), ModelAPI.ModelAPI_AttributeString_typeId())


# Get existing parameters names

    def existingParameters(self):
        """ Returns list of already existing parameters names"""
        aDoc = model.activeDocument()
        aNbFeatures = aDoc.numInternalFeatures();
        aNames = []
        for i in range(aNbFeatures):
            aParamFeature = aDoc.internalFeature(i);
            if aParamFeature is not None:
                if aParamFeature.getKind() == ParametersAPI.ParametersAPI_Parameter.ID():
                    aNames.append(aParamFeature.name())
        return aNames


# Execution of the Import

    def execute(self):
        """F.execute() -- execute the Feature"""
        # Retrieving the user input
        apath    = self.string(self.FILE_ID())
        filepath = apath.value()
        #print("filepath : ", filepath)
        if filepath != "" :

            # Creating the parameters in the current document
            part = model.activeDocument()
            aNames = self.existingParameters()

            with open(filepath) as file:
                for line in file:
                    defParameters = line.replace("\n","").split(' ')
                    if len(defParameters) == 2 :
                        if defParameters[0] not in aNames:
                            model.addParameter(part, defParameters[0], defParameters[1])
                            aNames.append(defParameters[0])
                file.close()
                return

            setError("The file does not exist")

    def isMacro(self):
        """Override Feature.initAttributes().
        F.isMacro() -> True

        importParameters feature is macro: removes itself on the creation transaction
        finish.
        """
        return True
