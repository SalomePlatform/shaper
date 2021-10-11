from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
model.addParameter(partSet, "angle", "60*3.141/180", "main angle")
model.importParameters(partSet, "File.txt")
model.end()
