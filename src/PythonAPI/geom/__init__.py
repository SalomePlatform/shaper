"""This package defines the Direct Geometry API of the Modeler.
"""

# Swigged interfaces to Modeling Data

from GeomAPI import GeomAPI_Circ   as Circ
from GeomAPI import GeomAPI_Circ2d as Circ2d
from GeomAPI import GeomAPI_Dir    as Dir
from GeomAPI import GeomAPI_Lin    as Lin
from GeomAPI import GeomAPI_Lin2d  as Lin2d
from GeomAPI import GeomAPI_Pnt    as Pnt
from GeomAPI import GeomAPI_Pnt2d  as Pnt2d
from GeomAPI import GeomAPI_XY     as XY
from GeomAPI import GeomAPI_XYZ    as XYZ

from GeomAPI import GeomAPI_Shape  as Shape


# Swigged interfaces to Modeling Algorithms

from GeomAlgoAPI import GeomAlgoAPI_Boolean   as Boolean
from GeomAlgoAPI import GeomAlgoAPI_Extrusion as Extrusion


# Emulation of interfaces not yet swigged

from missed  import *