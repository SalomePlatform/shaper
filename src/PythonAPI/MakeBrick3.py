# Creation of a box using the end-user API
# Author: Daniel Brunier-Coulin
# -----------------------------

import modeler
import extension


# Initialisation

modeler.begin()
mypartset = modeler.moduleDocument()


# Creating a new Part

mypart = modeler.addPart(mypartset).document()


# Creating the base of the box

extension.addBox( mypart, 10, 20, 30 )
modeler.end()

