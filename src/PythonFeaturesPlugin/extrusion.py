from ModelAPI import *


def addNew(builder, length, part, edges=None, reverse=False):
    feature = part.addFeature("Extrusion")
    feature.selection("extrusion_face").setValue(builder.geometry(),
                                                 builder.face())
    feature.real("extrusion_size").setValue(length)
    feature.boolean("extrusion_reverse").setValue(reverse)
    feature.execute()
    return feature


def getBody(extrusion):
    return extrusion.firstResult()
