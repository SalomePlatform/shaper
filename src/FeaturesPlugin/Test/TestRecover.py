# Test made with high level API
# -----------------------------

import model

# Initialisation

model.begin()
mypartset = model.moduleDocument()


# Creating a new Part, two circles, extrusions on them and Boolean CUT

mypart = model.addPart(mypartset).document()

sk1 = model.addSketch(mypart, model.defaultPlane("XOY"))
c1 = sk1.addCircle(0, 0, 100)
bigcyl = model.addExtrusion(mypart, sk1.selectFace(), 100)

sk2 = model.addSketch(mypart, model.defaultPlane("XOY"))
c2 = sk2.addCircle(20, 30, 30)
smallcyl = model.addExtrusion(mypart, sk2.selectFace(), 150)

cut = model.addCut(mypart, bigcyl.result(), smallcyl.result())

model.do()

# check bodies number is 1: only Boolean result
assert(mypart.size("Bodies") == 1)

model.addRecover(mypart, cut, smallcyl.result())

model.end()

# check that one is recovered
assert(mypart.size("Bodies") == 2)

model.undo()

# check bodies number is 1: recover is canceled
assert(mypart.size("Bodies") == 1)


# check persistent flag of recover: never concealed
model.begin()

recover = model.addRecover(mypart, cut, smallcyl.result(), True)
assert(mypart.size("Bodies") == 2)
sk3 = model.addSketch(mypart, model.defaultPlane("XOY"))
c3 = sk3.addCircle(0, 0, 90)
big2 = model.addExtrusion(mypart, sk3.selectFace(), 110)

cut2 = model.addCut(mypart, big2.result(), smallcyl.result())

model.end()

# two booleans and small cylinder
assert(mypart.size("Bodies") == 3)


# make the flag as not-persistent => cylinder must be disappeared
model.begin()
recover.setIsPersistent(False)
model.end()
# only two booleans
assert(mypart.size("Bodies") == 2)
