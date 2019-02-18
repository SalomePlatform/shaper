# Copyright (C) 2015-2019  CEA/DEN, EDF R&D
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

