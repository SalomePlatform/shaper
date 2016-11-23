"""Package for Sketch plugin for the Parametric Geometry API of the Modeler.
"""

# DEBIAN 6.0 WORKAROUND START
"""
  Following code is used to support Debian 6.0.
  It is kept, because SketchPlugin should be loaded before SketchAPI is first used.
  Otherwise, the runtime error will be generated while some static inline methods are broken.
""" 
from ModelAPI import *
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
aSession.startOperation()
aDocument.addFeature("Sketch")
aSession.finishOperation()
aSession.abortOperation()
# DEBIAN 6.0 WORKAROUND FINISH

from SketchAPI import addSketch
from tools import addPolyline, addPolygon
