"""
    TestSnowflake.py
"""
from GeomAPI import *
from GeomDataAPI import *
from ModelAPI import *
import collections
import math
import random


#=========================================================================
# Useful subroutines
#=========================================================================
def mirrorDiagonal(theSketch, allLines):
    result = []
    for aLine in allLines:
        aStartPoint = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
        anEndPoint = geomDataAPI_Point2D(aLine.attribute("EndPoint"))

        aNewLine = aSketchFeature.addFeature("SketchLine")
        aNewStartPoint = geomDataAPI_Point2D(aNewLine.attribute("StartPoint"))
        aNewEndPoint = geomDataAPI_Point2D(aNewLine.attribute("EndPoint"))
        aNewStartPoint.setValue(aStartPoint.y(), aStartPoint.x())
        aNewEndPoint.setValue(anEndPoint.y(), anEndPoint.x())
        result.append(aNewLine)
    allLines.extend(result)


def mirrorX(theSketch, allLines):
    result = []
    for aLine in allLines:
        aStartPoint = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
        anEndPoint = geomDataAPI_Point2D(aLine.attribute("EndPoint"))

        aNewLine = aSketchFeature.addFeature("SketchLine")
        aNewStartPoint = geomDataAPI_Point2D(aNewLine.attribute("StartPoint"))
        aNewEndPoint = geomDataAPI_Point2D(aNewLine.attribute("EndPoint"))
        aNewStartPoint.setValue(aStartPoint.x() * -1., aStartPoint.y())
        aNewEndPoint.setValue(anEndPoint.x() * -1., anEndPoint.y())
        result.append(aNewLine)
    allLines.extend(result)


def mirrorY(theSketch, allLines):
    result = []
    for aLine in allLines:
        aStartPoint = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
        anEndPoint = geomDataAPI_Point2D(aLine.attribute("EndPoint"))

        aNewLine = aSketchFeature.addFeature("SketchLine")
        aNewStartPoint = geomDataAPI_Point2D(aNewLine.attribute("StartPoint"))
        aNewEndPoint = geomDataAPI_Point2D(aNewLine.attribute("EndPoint"))
        aNewStartPoint.setValue(aStartPoint.x(), aStartPoint.y() * -1.)
        aNewEndPoint.setValue(anEndPoint.x(), anEndPoint.y() * -1.)
        result.append(aNewLine)
    allLines.extend(result)


def initContour(theNumLines):
    prevPoint = (35, 0)
    result = []
    deltaY = random.uniform(20, 25)
    for i in xrange(1, theNumLines):
        rangeXMax = (prevPoint[1] + deltaY) * (30. / i if i % 2 != 0 else 2)
        newX = random.uniform(prevPoint[1] + deltaY,
                              max(rangeXMax, prevPoint[1] + deltaY + 25.))
        newPoint = (round(newX, 4), round(prevPoint[1] + deltaY, 4))
        result.append((prevPoint, newPoint))
        prevPoint = newPoint
    # Close the contour
    result.append((prevPoint, (prevPoint[1], prevPoint[1])))
    return result


def makeLinesCoincident(theSketch, allLines):
    allPoints = collections.defaultdict(list)
    for aLine in allLines:
        aStartPoint = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
        anEndPoint = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
        aStartPointValue = (aStartPoint.x(), aStartPoint.y())
        anEndPointValue = (anEndPoint.x(), anEndPoint.y())
        allPoints[aStartPointValue].append(aStartPoint)
        allPoints[anEndPointValue].append(anEndPoint)
    for keyPoint, valuePoints in allPoints.iteritems():
        if len(valuePoints) != 2:
            print "Strange point: ", keyPoint, "has in coincidence: ", len(valuePoints)
            continue
        aConstraint = theSketch.addFeature("SketchConstraintCoincidence")
        aConstraint.refattr("ConstraintEntityA").setAttr(valuePoints[0])
        aConstraint.refattr("ConstraintEntityB").setAttr(valuePoints[1])


#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2014-11-27"

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
#=========================================================================
# Creation of a sketch
#=========================================================================
aSession.startOperation()
aSketchCommonFeature = aDocument.addFeature("Sketch")
aSketchFeature = featureToCompositeFeature(aSketchCommonFeature)
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
#=========================================================================
# 1 With the predefined contour or a new random generated in X0Y
# 2 Make complete contour using the diagonal, X and Y mirror symmetry
# 3 Link all lines in contour with coincidence
#=========================================================================
initialContour = [((35, 0), (134.8224, 22.0494)), ((134.8224, 22.0494), (71.987, 44.0988)),
                  ((71.987, 44.0988), (205.1448, 66.1482)), ((205.1448, 66.1482), (127.862, 88.1976)),
                  ((127.862, 88.1976), (416.861, 110.247)), ((416.861, 110.247), (233.0475, 132.2964)),
                  ((233.0475, 132.2964), (251.1518, 154.3458)), ((251.1518, 154.3458), (351.0993, 176.3952)),
                  ((351.0993, 176.3952), (432.3397, 198.4446)), ((432.3397, 198.4446), (223.3246, 220.494)),
                  ((223.3246, 220.494), (617.4069, 242.5434)), ((617.4069, 242.5434), (479.6524, 264.5928)),
                  ((479.6524, 264.5928), (453.5778, 286.6422)), ((453.5778, 286.6422), (541.3463, 308.6916)),
                  ((541.3463, 308.6916), (564.5509, 330.741)), ((564.5509, 330.741), (636.9284, 352.7904)),
                  ((636.9284, 352.7904), (383.5946, 374.8398)), ((383.5946, 374.8398), (403.3764, 396.8892)),
                  ((403.3764, 396.8892), (463.9447, 418.9386)), ((463.9447, 418.9386), (669.1751, 440.988)),
                  ((669.1751, 440.988), (602.2044, 463.0374)), ((602.2044, 463.0374), (942.0456, 485.0868)),
                  ((942.0456, 485.0868), (526.574, 507.1362)), ((526.574, 507.1362), (826.3619, 529.1856)),
                  ((826.3619, 529.1856), (576.9488, 551.235)), ((576.9488, 551.235), (624.5595, 573.2844)),
                  ((624.5595, 573.2844), (648.7146, 595.3338)), ((648.7146, 595.3338), (1194.6944, 617.3832)),
                  ((1194.6944, 617.3832), (646.6597, 639.4326)), ((646.6597, 639.4326), (839.8201, 661.482)),
                  ((839.8201, 661.482), (690.7487, 683.5314)), ((690.7487, 683.5314), (1350.2538, 705.5808)),
                  ((1350.2538, 705.5808), (731.0722, 727.6302)), ((731.0722, 727.6302), (1324.0992, 749.6796)),
                  ((1324.0992, 749.6796), (790.4873, 771.729)), ((790.4873, 771.729), (813.9883, 793.7784)),
                  ((813.9883, 793.7784), (828.9997, 815.8278)), ((828.9997, 815.8278), (1321.9798, 837.8772)),
                  ((1321.9798, 837.8772), (872.1503, 859.9266)), ((872.1503, 859.9266), (859.9266, 859.9266))]

# Regenerate contour
# initialContour = initContour(40)
allLines = []
for begin, end in initialContour:
    aNewLine = aSketchFeature.addFeature("SketchLine")
    aStartPoint = geomDataAPI_Point2D(aNewLine.attribute("StartPoint"))
    anEndPoint = geomDataAPI_Point2D(aNewLine.attribute("EndPoint"))
    aStartPoint.setValue(begin[0], begin[1])
    anEndPoint.setValue(end[0], end[1])
    allLines.append(aNewLine)
mirrorDiagonal(aSketchFeature, allLines)
mirrorX(aSketchFeature, allLines)
mirrorY(aSketchFeature, allLines)
makeLinesCoincident(aSketchFeature, allLines)
aSession.finishOperation()
#=========================================================================
# End of test
#=========================================================================
