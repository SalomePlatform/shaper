"""General purpose Interface
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import ModelAPI
import GeomAPI

import geom  # To be removed when gp_Ax3 will be Pythonized


def moduleDocument ():
    """Return the main document (the Partset) created or open from the Modeler.

    This document is unique in the application session.
    """
    return ModelAPI.ModelAPI_Session.get().moduleDocument()


def activeDocument ():
    """Return the active document.

    This document can be either the main application document (i.e. the Partset) or one of documents
    referred to by the main document (a Part).
    """
    return ModelAPI.ModelAPI_Session.get().activeDocument()


def defaultPlane (name):
    """Return one of the three planes defined by the global coordinate system.

    These planes are respectively referred to by name "XOY" (Z=0), "XOZ" (Y=0) or "YOZ" (X=0).
    """
    # Temporary implementation before the availability of default planes.
    o = GeomAPI.GeomAPI_Pnt(0, 0, 0)
    if   name == "XOY":
        n = GeomAPI.GeomAPI_Dir(0, 0, 1)
        x = GeomAPI.GeomAPI_Dir(1, 0, 0)
    elif name == "XOZ":
        n = GeomAPI.GeomAPI_Dir(0, -1, 0)
        x = GeomAPI.GeomAPI_Dir(1, 0, 0)
    elif name == "YOZ":
        n = GeomAPI.GeomAPI_Dir(1, 0, 0)
        x = GeomAPI.GeomAPI_Dir(0, 1, 0)

    return geom.Ax3(o, n, x)


def begin ():
    """Start a data structure transaction.

    Make a control point for being able to discard or undo
    all operations done during this transaction.
    """
    ModelAPI.ModelAPI_Session.get().startOperation()


def end ():
    """Commit the data structure transaction.

    Make all operations done since the last control point undo-able.
    """
    ModelAPI.ModelAPI_Session.get().finishOperation()


def do ():
    """Commit the data structure transaction and start the new one.

    Make all operations done since the last control point undo-able
    and continue with the new transaction.
    """
    session = ModelAPI.ModelAPI_Session.get()
    session.finishOperation()
    session.startOperation()


def undo ():
    """Roll-back the data structure to the previous control point."""
    ModelAPI.ModelAPI_Session.get().undo()


def redo ():
    """Restore the data structure rolled-back by the last undo."""
    ModelAPI.ModelAPI_Session.get().redo()


def reset ():
    """Reset the data structure to initial state."""
    ModelAPI.ModelAPI_Session.get().closeAll()
