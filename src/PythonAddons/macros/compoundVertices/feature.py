"""compound of vertices Feature
Author: Nathalie Gore
"""

from salome.shaper import model
from salome.shaper import geom
import ModelAPI

class compoundVertices(model.Feature):
    """Import of Construction points
    """

# Feature initializations

    def __init__(self):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        model.Feature.__init__(self)

    @staticmethod
    def ID():
        """Return Id of the Feature."""
        return "compoundVertices"

    @staticmethod
    def FILE_ID():
        """Returns ID of the file select parameter."""
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

        self.lfeatures = []
        self.folder = None
        self.separator = " "

# Execution of the Import

    def execute(self):
        """F.execute() -- execute the Feature"""
        # Retrieving the user input
        apath    = self.string(self.FILE_ID())
        aseparator = self.string(self.SEPARATOR_ID()).value()
        if aseparator:
            self.separator = aseparator

        filepath = apath.value()
        if filepath != "" :
            part = model.activeDocument()            
            if self.lfeatures :
                for feature in self.lfeatures:
                   part.removeFeature(feature.feature())
                self.lfeatures = []
                model.removeFolder(self.folder)

            from os.path import basename
            filename = basename(filepath)
            nameRes = "Points_" + filename

            # Creating the construction points in the current document
            lVertices = []

            with open(filepath) as file:
                for line in file:
                    coord = line.split(self.separator)
                    if len(coord) != 3:
                        return
                    x = float(coord[0]); y = float(coord[1]); z = float(coord[2]);
                    point = model.addPoint(part, x,y,z); point.execute(True); self.lfeatures.append(point)
                    #vertex = model.addVertex(part, [point.result()]); vertex.execute(True); self.lfeatures.append(vertex)
                    lVertices.append(point.result())
                file.close()
                compound = model.addCompound(part, lVertices)
                compound.execute(True); self.lfeatures.append(compound)
                compound.result().setName(nameRes)
                self.folder = model.addFolder(part, self.lfeatures[0], compound)
                self.folder.setName(nameRes)
                return
        
            setError("The file does not exist")

    def isMacro(self):
        """Override Feature.initAttributes().
        F.isMacro() -> True

        compoundVertices feature is macro: removes itself on the creation transaction
        finish.
        """
        return False
