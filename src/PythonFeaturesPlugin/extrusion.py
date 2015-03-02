from ModelAPI import *


def addNew(builder, length, part, edges=None, reverse=False):
    feature = part.addFeature("Extrusion")
    feature.selection("extrusion_face").setValue(builder.geometry(),
                                                 builder.face())
    if length < 0.0000001:
      length = 50
    feature.real("extrusion_size").setValue(length)
    feature.boolean("extrusion_reverse").setValue(reverse)
    feature.execute()
    return feature


def getBody(extrusion):
    return modelAPI_ResultBody(extrusion.firstResult())
