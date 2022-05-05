#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Exemple de création d'un objet SHAPER tuyauterie à partir d'une description dans un fichier txte

Copyright 2022 EDF
"""

__revision__ = "V02.01"

#=================================================================================
# theFile = fichier texte de la description du réseau
theFile = "/home/D68518/pipeNetwork_ligne.txt"
#=================================================================================

import salome
salome.standalone()
salome.salome_init()

from ModelAPI import *

aSession = ModelAPI_Session.get()

# Création de la part pour accueillir le réseau
aSession.startOperation("Create part for pipe network")
aPartFeature = aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()
aPart = aSession.activeDocument()

# Importation et traitement du fichier
aSession.startOperation("Import file")
aFeatureKind = "pipeNetwork"
anImportFeature = aPart.addFeature(aFeatureKind)
aFieldName = "file_path"
aFile = anImportFeature.string(aFieldName)
aFile.setValue(theFile)
aSession.finishOperation()

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()
