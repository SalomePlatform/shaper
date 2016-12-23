# Test made with high level API
# -----------------------------

from salome.shaper import model

# Initialisation

model.begin()
mypartset = model.moduleDocument()


# Creating a new Part, two circles, extrusions on them and Boolean CUT

mypart = model.addPart(mypartset).document()

sk1 = model.addSketch(mypart, model.defaultPlane("XOY"))
c1 = sk1.addCircle(0, 0, 100)
model.do()
bigcyl = model.addExtrusion(mypart, sk1.selectFace(), 100)

sk2 = model.addSketch(mypart, model.defaultPlane("XOY"))
c2 = sk2.addCircle(20, 30, 30)
model.do()
smallcyl = model.addExtrusion(mypart, sk2.selectFace(), 150)

cut = model.addCut(mypart, bigcyl.results(), smallcyl.results())

model.do()

# check bodies number is 1: only Boolean result
assert(mypart.size("Bodies") == 1)

model.addRecover(mypart, cut, smallcyl.results())

model.end()

# check that one is recovered
assert(mypart.size("Bodies") == 2)

model.undo()

# check bodies number is 1: recover is canceled
assert(mypart.size("Bodies") == 1)


# check persistent flag of recover: never concealed
model.begin()
recover = model.addRecover(mypart, cut, smallcyl.results())
model.end()

assert(mypart.size("Bodies") == 2)

model.begin()
sk3 = model.addSketch(mypart, model.defaultPlane("XOY"))
c3 = sk3.addCircle(0, 0, 90)
model.do()
big2 = model.addExtrusion(mypart, sk3.selectFace(), 110)

cut2 = model.addCut(mypart, big2.results(), smallcyl.results())

model.end()

# two booleans and small cylinder
assert(mypart.size("Bodies") == 3)

assert(model.checkPythonDump())
