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

    def getKind(self):
        """Override Feature.getKind()"""
        return compoundVertices.ID()


# Initialization of the dialog panel

    def initAttributes(self):
        """Override Feature.initAttributes()"""
        # Creating the input argument of the feature
        self.data().addAttribute(self.FILE_ID(), ModelAPI.ModelAPI_AttributeString_typeId())

# Execution of the Import

    def execute(self):
        """F.execute() -- execute the Feature"""
        # Retrieving the user input
        apath    = self.string(self.FILE_ID())
        filepath = apath.value()
        print("filepath : ", filepath)
        if filepath != "" :
            
            from os.path import basename
            filename = basename(filepath)
            print("filename : ", filename)

            # Creating the construction points in the current document
            part = model.activeDocument()
            lVertices = []

            startPoint = None

            with open(filepath) as file:
                for line in file:
                    coord = line.split(' ')
                    x = float(coord[0]); y = float(coord[1]); z = float(coord[2]);
                    point = model.addPoint(part, x,y,z); point.execute(True)
                    if startPoint == None : startPoint = point
                    vertex = model.addVertex(part, [point.result()]); vertex.execute(True)
                    lVertices.append(vertex.result())
                file.close()
                compound = model.addCompound(part, lVertices)
                compound.execute(True)
                folder = model.addFolder(part, startPoint, compound)
                folder.setName(filename)
                return
        
            setError("The file does not exist")

    def isMacro(self):
        """Override Feature.initAttributes().
        F.isMacro() -> True

        compoundVertices feature is macro: removes itself on the creation transaction
        finish.
        """
        return True			
