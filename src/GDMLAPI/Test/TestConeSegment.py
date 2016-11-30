"""
Test case for GDML Primitive Cone Segment feature. 
Written on High API.
"""
from ModelAPI import *
from GeomAPI import *

import model

# Get session
aSession = ModelAPI_Session.get()

# Create a part
aDocument = aSession.activeDocument()
aSession.startOperation()
model.addPart(aDocument)
aDocument = aSession.activeDocument()
aSession.finishOperation()

# Create several "cone segment"
aSession.startOperation()
aCS1 = model.addConeSegment(aDocument,7.,11.,5.,8.,12.,0.,270.).result()
aSession.finishOperation()
assert (aCS1 is not None)

aSession.startOperation()
aCS2 = model.addConeSegment(aDocument,5.,10.,7.,15.,15.,45.,180.).result()
aSession.finishOperation()
assert (aCS2 is not None)

aSession.startOperation()
aCS3 = model.addConeSegment(aDocument,5.,10.,7.,15.,15.,-45.,180.).result()
aSession.finishOperation()
assert (aCS3 is not None)

aSession.startOperation()
aCS4 = model.addConeSegment(aDocument,0.,10.,2.,7.,3.,0.,270.).result()
aSession.finishOperation()
assert (aCS4 is not None)

aSession.startOperation()
aCS5 = model.addConeSegment(aDocument,5.,8.,0.,6.,10.,45.,270.).result()
aSession.finishOperation()
assert (aCS5 is not None)

aSession.startOperation()
aCS6 = model.addConeSegment(aDocument,0.,6.,0.,4.,17.,0.,270.).result()
aSession.finishOperation()
assert (aCS6 is not None)

aSession.startOperation()
aCS7 = model.addConeSegment(aDocument,7.,11.,5.,8.,12.,0.,360.).result()
aSession.finishOperation()
assert (aCS7 is not None)

aSession.startOperation()
aCS8 = model.addConeSegment(aDocument,-5.,11.,5.,8.,12.,0.,270.)
aSession.finishOperation()
assert (len(aCS8.results()) == 0)

aSession.startOperation()
aCS9 = model.addConeSegment(aDocument,11.,7.,5.,8.,12.,0.,270.)
aSession.finishOperation()
assert (len(aCS9.results()) == 0)

aSession.startOperation()
aCS10 = model.addConeSegment(aDocument,7.,11.,8.,5.,12.,0.,270.)
aSession.finishOperation()
assert (len(aCS10.results()) == 0)

aSession.startOperation()
aCS11 = model.addConeSegment(aDocument,7.,11.,-3.,5.,12.,0.,270.)
aSession.finishOperation()
assert (len(aCS11.results()) == 0)

aSession.startOperation()
aCS12 = model.addConeSegment(aDocument,7.,11.,5.,8.,0.,0.,270.)
aSession.finishOperation()
assert (len(aCS12.results()) == 0)

aSession.startOperation()
aCS13 = model.addConeSegment(aDocument,7.,11.,5.,8.,-10.,0.,270.)
aSession.finishOperation()
assert (len(aCS13.results()) == 0)

aSession.startOperation()
aCS14 = model.addConeSegment(aDocument,7.,11.,5.,8.,12.,-45.,-45.)
aSession.finishOperation()
assert (len(aCS14.results()) == 0)

aSession.startOperation()
aCS15 = model.addConeSegment(aDocument,7.,11.,5.,8.,12.,45.,450.)
aSession.finishOperation()
assert (len(aCS15.results()) == 0)

aSession.startOperation()
model.addParameter(aDocument,"rmin1","7.")
model.addParameter(aDocument,"rmax1","11.")
model.addParameter(aDocument,"rmin2","5.")
model.addParameter(aDocument,"rmax2","8.")
model.addParameter(aDocument,"z","12.")
model.addParameter(aDocument,"startphi","0.")
model.addParameter(aDocument,"deltaphi","270.")
aCS16 = model.addConeSegment(aDocument,"rmin1","rmax1","rmin2","rmax2","z","startphi","deltaphi").result()
aSession.finishOperation()
assert (aCS16 is not None)
