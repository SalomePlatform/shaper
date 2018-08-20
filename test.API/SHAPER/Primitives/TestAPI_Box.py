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

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Pnt as pnt

# Create a box with dimensions
Box_1 = shaperpy.makeBox(10.,10.,10.)

try :
  Box_2 = shaperpy.makeBox(0.,10.,10.)
except myExcept as ec:
  assert(ec.what() == "Box builder with dimensions :: Dx is null or negative.")

try :
  Box_3 = shaperpy.makeBox(10.,0.,10.)
except myExcept as ec:
  assert(ec.what() == "Box builder with dimensions :: Dy is null or negative.")

try :
  Box_4 = shaperpy.makeBox(10.,10.,0.)
except myExcept as ec:
  assert(ec.what() == "Box builder with dimensions :: Dz is null or negative.")

try :
  Box_5 = shaperpy.makeBox(-10.,10.,10.)
except myExcept as ec:
  assert(ec.what() == "Box builder with dimensions :: Dx is null or negative.")

try :
  Box_6 = shaperpy.makeBox(10.,-10.,10.)
except myExcept as ec:
  assert(ec.what() == "Box builder with dimensions :: Dy is null or negative.")

try :
  Box_7 = shaperpy.makeBox(10.,10.,-10.)
except myExcept as ec:
  assert(ec.what() == "Box builder with dimensions :: Dz is null or negative.")

# Create a box with two points defining the diagonal
pnt1 = pnt(0.,0.,0.)
pnt2 = pnt(50.,50.,50.)
Box_8 = shaperpy.makeBox(pnt1,pnt2)

try :
  Box_9 = shaperpy.makeBox(pnt1,pnt1)
except myExcept as ec:
  assert(ec.what() == "Box builder with points :: the distance between the two points is null.")

try :
  pnt3 = pnt(0.,50.,50.)
  Box_10 = shaperpy.makeBox(pnt1,pnt3)
except myExcept as ec:
  assert(ec.what() == "Box builder with points :: the points belong both to one of the OXY, OYZ or OZX planes.")

try :
  pnt4 = pnt(50.,0.,50.)
  Box_11 = shaperpy.makeBox(pnt1,pnt4)
except myExcept as ec:
  assert(ec.what() == "Box builder with points :: the points belong both to one of the OXY, OYZ or OZX planes.")

try :
  pnt5 = pnt(50.,50.,0.)
  Box_12 = shaperpy.makeBox(pnt1,pnt5)
except myExcept as ec:
  assert(ec.what() == "Box builder with points :: the points belong both to one of the OXY, OYZ or OZX planes.")

try :
  Box_13 = shaperpy.makeBox(None, pnt2)
except myExcept as ec:
  assert(ec.what() == "Box builder with points :: the first point is not valid.")

try :
  Box_14 = shaperpy.makeBox(pnt2, None)
except myExcept as ec:
  assert(ec.what() == "Box builder with points :: the second point is not valid.")