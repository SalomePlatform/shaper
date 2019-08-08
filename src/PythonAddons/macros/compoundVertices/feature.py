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

            # Creating the construction points in the current document
            part = model.activeDocument()
            lVertices = []

            with open(filepath) as file:
                for line in file:
                    coord = line.split(' ')
                    x = float(coord[0]); y = float(coord[1]); z = float(coord[2]);
                    print(line)
                    point = model.addPoint(part, x,y,z)
                    print(line)
                    vertex = model.addVertex(part, [point.result()])
                    print(line)
                    #lVertices.append(vertex)
                file.close()
                return
        
            setError("The file does not exist")
			
