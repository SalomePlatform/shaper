# Copyright (C) 2014-201x CEA/DEN, EDF R&D

# File:        TestAPI_Scale.py
# Created:     25 Jan 2017
# Author:      Clarisse Genrault (CEA)

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Pnt as pnt

# Create Boxes
Box_1 = shaperpy.makeBox(10.,10.,10.)
Box_2 = shaperpy.makeBox(10.,10.,10.)
Box_3 = shaperpy.makeBox(10.,10.,10.)
Box_4 = shaperpy.makeBox(10.,10.,10.)
Box_5 = shaperpy.makeBox(10.,10.,10.)
Box_6 = shaperpy.makeBox(10.,10.,10.)
Box_7 = shaperpy.makeBox(10.,10.,10.)
Box_8 = shaperpy.makeBox(10.,10.,10.)
Box_9 = shaperpy.makeBox(10.,10.,10.)
Box_10 = shaperpy.makeBox(10.,10.,10.)
Box_11 = shaperpy.makeBox(10.,10.,10.)
Box_12 = shaperpy.makeBox(10.,10.,10.)

# Create Points
pntOrigin = pnt(0., 0., 0.)
pnt1 = pnt(-10., -10., -10.)


# Create Scales
Scale_1 = shaperpy.makeScale(Box_1, pntOrigin, 2.)
Scale_2 = shaperpy.makeScale(Box_2, pnt1, 2.)
try :
    Scale_6 = shaperpy.makeScale(Box_3, pntOrigin, 0)
except myExcept,ec :
    assert(ec.what() == "Scale builder :: the scale factor is null.")

Scale_7 = shaperpy.makeScale(Box_4, pntOrigin, -3.)

try :
    Scale_8 = shaperpy.makeScale(Box_5, None, 2.)
except myExcept,ec :
    assert(ec.what() == "Scale builder :: center point is not valid.")

Scale_9 = shaperpy.makeScale(Box_6, pntOrigin, 2., 3., 1.)
Scale_10 = shaperpy.makeScale(Box_7, pnt1, 2., 3., 1.)

try :
    Scale_14 = shaperpy.makeScale(Box_8, pntOrigin, 0., 3., 1.)
except myExcept,ec :
    assert(ec.what() == "Scale builder :: the scale factor in X is null.")
try :
    Scale_15 = shaperpy.makeScale(Box_9, pntOrigin, 2., 0., 1.)
except myExcept,ec :
    assert(ec.what() == "Scale builder :: the scale factor in Y is null.")
try :
    Scale_16 = shaperpy.makeScale(Box_10, pntOrigin, 2., 3., 0.)
except myExcept,ec :
    assert(ec.what() == "Scale builder :: the scale factor in Z is null.")

Scale_17 = shaperpy.makeScale(Box_11, pntOrigin, -2., -3., 0.5)

try :
    Scale_18 = shaperpy.makeScale(Box_12, None, 2., 3., 1.)
except myExcept,ec :
    assert(ec.what() == "Scale builder :: center point is not valid.")