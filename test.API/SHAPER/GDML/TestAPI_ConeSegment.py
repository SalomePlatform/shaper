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

CS1 = shaperpy.makeConeSegment(7.,11.,5.,8.,12.,0.,270.)
CS2 = shaperpy.makeConeSegment(5.,10.,7.,15.,15.,45.,180.)
CS3 = shaperpy.makeConeSegment(5.,10.,7.,15.,15.,-45.,180.)
CS4 = shaperpy.makeConeSegment(0.,10.,2.,7.,3.,0.,270.)
CS5 = shaperpy.makeConeSegment(5.,8.,0.,6.,10.,45.,270.)
CS6 = shaperpy.makeConeSegment(0.,6.,0.,4.,17.,0.,270.)
CS7 = shaperpy.makeConeSegment(7.,11.,5.,8.,12.,0.,360.)

try :
    CS8 = shaperpy.makeConeSegment(-5.,11.,5.,8.,12.,0.,270.)
except myExcept as ec:
    assert(ec.what() == "Cone Segment builder :: rmin1 is negative.")

try :
    CS9 = shaperpy.makeConeSegment(11.,7.,5.,8.,12.,0.,270.)
except myExcept as ec:
    assert(ec.what() == "Cone Segment builder :: rmin1 is larger than rmax1.")

try :
    CS10 = shaperpy.makeConeSegment(7.,11.,8.,5.,12.,0.,270.)
except myExcept as ec:
    assert(ec.what() == "Cone Segment builder :: rmin2 is larger than rmax2.")

try :
    CS11 = shaperpy.makeConeSegment(7.,11.,-3.,5.,12.,0.,270.)
except myExcept as ec:
    assert(ec.what() == "Cone Segment builder :: rmin2 is negative.")

try :
    CS12 = shaperpy.makeConeSegment(7.,11.,5.,8.,0.,0.,270.)
except myExcept as ec:
    assert(ec.what() == "Cone Segment builder :: z is negative or null.")

try :
    CS13 = shaperpy.makeConeSegment(7.,11.,5.,8.,-10.,0.,270.)
except myExcept as ec:
    assert(ec.what() == "Cone Segment builder :: z is negative or null.")

try :
    CS14 = shaperpy.makeConeSegment(7.,11.,5.,8.,12.,-45.,-45.)
except myExcept as ec:
    assert(ec.what() == "Cone Segment builder :: deltaphi is negative or null.")

try :
    CS15 = shaperpy.makeConeSegment(7.,11.,5.,8.,12.,45.,450.)
except myExcept as ec:
    assert(ec.what() == "Cone Segment builder :: deltaphi is larger than 360 degree.")