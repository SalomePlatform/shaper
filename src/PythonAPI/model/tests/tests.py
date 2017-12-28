## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from GeomAlgoAPI import *
from GeomAPI import *
from GeomDataAPI import *
from ModelAPI import ModelAPI_Feature
import math
from salome.shaper.model import sketcher

TOLERANCE = 1.e-7

aShapeTypes = {
  GeomAPI_Shape.SOLID:  "GeomAPI_Shape.SOLID",
  GeomAPI_Shape.FACE:   "GeomAPI_Shape.FACE",
  GeomAPI_Shape.EDGE:   "GeomAPI_Shape.EDGE",
  GeomAPI_Shape.VERTEX: "GeomAPI_Shape.VERTEX"}


def generateTests(theFeature, theFeatureName, theTestsList = []):
  """ Generates tests for theFeature.
  :param theFeature: feature to test. Should be ModelHighAPI_Interface.
  :param theFeatureName: feature name to put in test commands.
  :param theTestsList: list of test to be generated. If empty generates all tests.
  """
  if "testNbResults" in theTestsList or len(theTestsList) == 0:
    aNbResults = len(theFeature.results())
    print "model.testNbResults({}, {})".format(theFeatureName, aNbResults)

  if "testNbSubResults" in theTestsList or len(theTestsList) == 0:
    aNbResults = len(theFeature.results())
    aNbSubResults = []
    for anIndex in range(0, aNbResults):
      aNbSubResults.append(theFeature.results()[anIndex].numberOfSubs())
    print "model.testNbSubResults({}, {})".format(theFeatureName, aNbSubResults)

  if "testNbSubShapes" in theTestsList or len(theTestsList) == 0:
    aNbResults = len(theFeature.results())
    for aShapeType in aShapeTypes:
      aNbSubShapes = []
      for anIndex in range(0, aNbResults):
        aShape = theFeature.results()[anIndex].resultSubShapePair()[0].shape()
        aNbResultSubShapes = 0
        aShapeExplorer = GeomAPI_ShapeExplorer(aShape, aShapeType)
        while aShapeExplorer.more():
          aNbResultSubShapes += 1
          aShapeExplorer.next();
        aNbSubShapes.append(aNbResultSubShapes)
      print "model.testNbSubShapes({}, {}, {})".format(theFeatureName, aShapeTypes[aShapeType], aNbSubShapes)

  if "testResultsVolumes" in theTestsList or len(theTestsList) == 0:
    aNbResults = len(theFeature.results())
    aResultsVolumes = []
    for anIndex in range(0, aNbResults):
      aResultsVolumes.append(GeomAlgoAPI_ShapeTools_volume(theFeature.results()[anIndex].resultSubShapePair()[0].shape()))
    print "model.testResultsVolumes({}, [{}])".format(theFeatureName, ", ".join("{:0.27f}".format(i) for i in aResultsVolumes))


def testNbResults(theFeature, theExpectedNbResults):
  """ Tests number of feature results.
  :param theFeature: feature to test.
  :param theExpectedNbResults: expected number of results.
  """
  aNbResults = len(theFeature.results())
  assert (aNbResults == theExpectedNbResults), "Number of results: {}. Expected: {}.".format(aNbResults, theExpectedNbResults)


def testNbSubResults(theFeature, theExpectedNbSubResults):
  """ Tests number of feature sub-results for each result.
  :param theFeature: feature to test.
  :param theExpectedNbSubResults: list of sub-results numbers. Size of list should be equal to len(theFeature.results()).
  """
  aNbResults = len(theFeature.results())
  aListSize = len(theExpectedNbSubResults)
  assert (aNbResults == aListSize), "Number of results: {} not equal to list size: {}.".format(aNbResults, aListSize)
  for anIndex in range(0, aNbResults):
    aNbSubResults = theFeature.results()[anIndex].numberOfSubs()
    anExpectedNbSubResults = theExpectedNbSubResults[anIndex]
    assert (aNbSubResults == anExpectedNbSubResults), "Number of sub-results for result[{}]: {}. Expected: {}.".format(anIndex, aNbSubResults, anExpectedNbSubResults)


def testNbSubShapes(theFeature, theShapeType, theExpectedNbSubShapes):
  """ Tests number of feature sub-shapes of passed type for each result.
  :param theFeature: feature to test.
  :param theShapeType: shape type of sub-shapes to test.
  :param theExpectedNbSubShapes: list of sub-shapes numbers. Size of list should be equal to len(theFeature.results()).
  """
  aNbResults = len(theFeature.results())
  aListSize = len(theExpectedNbSubShapes)
  assert (aNbResults == aListSize), "Number of results: {} not equal to list size: {}.".format(aNbResults, aListSize)
  for anIndex in range(0, aNbResults):
    aNbResultSubShapes = 0
    anExpectedNbSubShapes = theExpectedNbSubShapes[anIndex]
    aShape = theFeature.results()[anIndex].resultSubShapePair()[0].shape()
    aShapeExplorer = GeomAPI_ShapeExplorer(aShape, theShapeType)
    while aShapeExplorer.more():
      aNbResultSubShapes += 1
      aShapeExplorer.next();
    assert (aNbResultSubShapes == anExpectedNbSubShapes), "Number of sub-shapes of type {} for result[{}]: {}. Expected: {}.".format(aShapeTypes[theShapeType], anIndex, aNbResultSubShapes, anExpectedNbSubShapes)


def testResultsVolumes(theFeature, theExpectedResultsVolumes, theNbSignificantDigits = 7):
  """ Tests results volumes.
  :param theFeature: feature to test.
  :param theExpectedResultsVolumes: list of results volumes. Size of list should be equal to len(theFeature.results()).
  """
  aTolerance = 10**(-theNbSignificantDigits)
  aNbResults = len(theFeature.results())
  aListSize = len(theExpectedResultsVolumes)
  assert (aNbResults == aListSize), "Number of results: {} not equal to list size: {}.".format(aNbResults, aListSize)
  for anIndex in range(0, aNbResults):
    aResultVolume = GeomAlgoAPI_ShapeTools_volume(theFeature.results()[anIndex].resultSubShapePair()[0].shape())
    aResultVolumeStr = "{:0.27f}".format(aResultVolume).lstrip("0").lstrip(".").lstrip("0")
    anExpectedResultVolume = theExpectedResultsVolumes[anIndex]
    anExpectedResultVolumeStr = "{:0.27f}".format(anExpectedResultVolume).lstrip("0").lstrip(".").lstrip("0")
    assert math.fabs(aResultVolume - anExpectedResultVolume) < aTolerance * anExpectedResultVolume, "Volume of result[{}]: {:0.27f}. Expected: {:0.27f}. The first {} significant digits not equal.".format(anIndex, aResultVolume, anExpectedResultVolume, theNbSignificantDigits)

def testHaveNamingFaces(theFeature, theModel, thePartDoc) :
  """ Tests if all faces of result have a name
  :param theFeature: feature to test.
  """
  # open transaction since all the checking are performed in tests after model.end() call
  theModel.begin()
  # Get feature result/sub-result
  aResult = theFeature.results()[0].resultSubShapePair()[0]
  # Get result/sub-result shape
  shape = aResult.shape()
  # Create shape explorer with desired shape type
  shapeExplorer = GeomAPI_ShapeExplorer(shape, GeomAPI_Shape.FACE)
  # Create list, and store selections in it
  selectionList = []
  while shapeExplorer.more():
    selection = theModel.selection(aResult, shapeExplorer.current()) # First argument should be result/sub-result, second is sub-shape on this result/sub-result
    selectionList.append(selection)
    shapeExplorer.next()
  # Create group with this selection list
  Group_1 = theModel.addGroup(thePartDoc, selectionList)
  theModel.end()

  # Now you can check that all selected shapes in group have right shape type and name.
  groupFeature = Group_1.feature()
  groupSelectionList = groupFeature.selectionList("group_list")
  assert(groupSelectionList.size() == len(selectionList))
  for index in range(0, groupSelectionList.size()):
    attrSelection = groupSelectionList.value(index)
    shape = attrSelection.value()
    name = attrSelection.namingName()
    assert(shape.isFace())
    assert(name != ""), "String empty"

def testHaveNamingEdges(theFeature, theModel, thePartDoc) :
  """ Tests if all edges of result have a name
  :param theFeature: feature to test.
  """
  # Get feature result/sub-result
  aResult = theFeature.results()[0].resultSubShapePair()[0]
  # Get result/sub-result shape
  shape = aResult.shape()
  # Create shape explorer with desired shape type
  shapeExplorer = GeomAPI_ShapeExplorer(shape, GeomAPI_Shape.EDGE)
  # Create list, and store selections in it
  selectionList = []
  while shapeExplorer.more():
    selection = theModel.selection(aResult, shapeExplorer.current()) # First argument should be result/sub-result, second is sub-shape on this result/sub-result
    selectionList.append(selection)
    shapeExplorer.next()
  # Create group with this selection list
  Group_1 = theModel.addGroup(thePartDoc, selectionList)
  theModel.do()
  theModel.end()

  # Now you can check that all selected shapes in group have right shape type and name.
  groupFeature = Group_1.feature()
  groupSelectionList = groupFeature.selectionList("group_list")
  theModel.end()
  assert(groupSelectionList.size() == len(selectionList))
  for index in range(0, groupSelectionList.size()):
    attrSelection = groupSelectionList.value(index)
    shape = attrSelection.value()
    name = attrSelection.namingName()
    assert(shape.isEdge())
    assert(name != ""), "String empty"

def testHaveNamingByType(theFeature, theModel, thePartDoc, theSubshapeType) :
  """ Tests if all sub-shapes of result have a unique name
  :param theFeature: feature to test.
  :param theSubshapeType: type of sub-shape
  """
  aFirstRes = theFeature.results()[0]
  # Get number of sub-results
  hasSubs = True
  nbSubs = aFirstRes.numberOfSubs()
  if nbSubs == 0:
    # no sub-results => treat current result as a sub
    hasSubs = False
    nbSubs = 1

  selectionList = []
  shapesList = [] # to append only unique shapes (not isSame)
  for sub in range(0, nbSubs):
    # Get feature result/sub-result
    if hasSubs:
      aResult = aFirstRes.subResult(sub).resultSubShapePair()[0]
    else:
      aResult = aFirstRes.resultSubShapePair()[0]
    # Get result/sub-result shape
    shape = aResult.shape()
    # Create shape explorer with desired shape type
    shapeExplorer = GeomAPI_ShapeExplorer(shape, theSubshapeType)
    # Create list, and store selections in it
    while shapeExplorer.more():
      aDuplicate = False
      for alreadyThere in shapesList:
        if alreadyThere.isSame(shapeExplorer.current()):
          aDuplicate = True
      if aDuplicate:
        shapeExplorer.next()
        continue
      shapesList.append(shapeExplorer.current())
      selection = theModel.selection(aResult, shapeExplorer.current()) # First argument should be result/sub-result, second is sub-shape on this result/sub-result
      selectionList.append(selection)
      shapeExplorer.next()
  # Create group with this selection list
  Group_1 = theModel.addGroup(thePartDoc, selectionList)
  theModel.do()

  groupSelectionList = Group_1.feature().selectionList("group_list")
  assert(groupSelectionList.size() == len(selectionList))

  # Check that all selected shapes in group have right shape type and unique name.
  checkGroup(Group_1, theSubshapeType)

def testHaveNamingSubshapes(theFeature, theModel, thePartDoc) :
  """ Tests if all vertices/edges/faces of result have a unique name
  :param theFeature: feature to test.
  """
  testHaveNamingByType(theFeature, theModel, thePartDoc, GeomAPI_Shape.VERTEX)
  testHaveNamingByType(theFeature, theModel, thePartDoc, GeomAPI_Shape.EDGE)
  testHaveNamingByType(theFeature, theModel, thePartDoc, GeomAPI_Shape.FACE)

def testNbSubFeatures(theComposite, theKindOfSub, theExpectedCount):
  """ Tests number of sub-features of the given type
  :param theComposite     composite feature to check its subs
  :param theKindOfSub     kind of sub-feature to calculate count
  :param theExpectedCount expected number of sub-features
  """
  count = 0
  for aSub in theComposite.features().list():
    aFeature = ModelAPI_Feature.feature(aSub)
    if aFeature is not None and aFeature.getKind() == theKindOfSub:
       count += 1
  assert (count == theExpectedCount), "Number of sub-features of type {}: {}, expected {}".format(theKindOfSub, count, theExpectedCount)

def assertSketchArc(theArcFeature):
  """ Tests whether the arc is correctly defined
  """
  aCenterPnt = geomDataAPI_Point2D(theArcFeature.attribute("center_point"))
  aStartPnt = geomDataAPI_Point2D(theArcFeature.attribute("start_point"))
  aEndPnt = geomDataAPI_Point2D(theArcFeature.attribute("end_point"))
  aRadius = theArcFeature.real("radius")
  aDistCS = sketcher.tools.distancePointPoint(aCenterPnt, aStartPnt)
  aDistCE = sketcher.tools.distancePointPoint(aCenterPnt, aEndPnt)
  assert math.fabs(aDistCS - aDistCE) < TOLERANCE, "Wrong arc: center-start distance {}, center-end distance {}".format(aDistCS, aDistCE)
  assert math.fabs(aRadius.value() -aDistCS) < TOLERANCE, "Wrong arc: radius is {0}, expected {1}".format(aRadius.value(), aDistCS)

def checkBooleansResult(theFeature,theModel,NbRes,NbSubRes,NbSolid,NbFace,NbEdge,NbVertex):
  """ Tests numbers of sub-shapes in results (used in Boolean operations tests)
  """
  theModel.testNbResults(theFeature, NbRes)
  theModel.testNbSubResults(theFeature, NbSubRes)
  theModel.testNbSubShapes(theFeature, GeomAPI_Shape.SOLID, NbSolid)
  theModel.testNbSubShapes(theFeature, GeomAPI_Shape.FACE, NbFace)
  theModel.testNbSubShapes(theFeature, GeomAPI_Shape.EDGE, NbEdge)
  theModel.testNbSubShapes(theFeature, GeomAPI_Shape.VERTEX, NbVertex)

def checkSketch(theSketch, theDOF = -1):
  """ Tests the sketch is valid and DoF is equal to the given
  """
  assert(theSketch.feature().error() == ""), "Sketch failed: {}".format(theSketch.feature().error())
  assert(theSketch.solverError().value() == ""), "Sketch solver failed: {}".format(theSketch.solverError().value())
  if theDOF != -1:
    aDOF = sketcher.tools.dof(theSketch)
    assert(aDOF == theDOF), "Sketch DoF {} is wrong. Expected {}".format(aDOF, theDOF)

def checkGroup(theGroup, theShapeType):
  """ Check that all selected shapes in group have correct shape type and unique name
  """
  groupFeature = theGroup.feature()
  groupSelectionList = groupFeature.selectionList("group_list")
  presented_names = set()
  for index in range(0, groupSelectionList.size()):
    attrSelection = groupSelectionList.value(index)
    shape = attrSelection.value()
    name = attrSelection.namingName()
    if theShapeType == GeomAPI_Shape.VERTEX:
      assert(shape.isVertex())
    elif theShapeType == GeomAPI_Shape.EDGE:
      assert(shape.isEdge())
    elif theShapeType == GeomAPI_Shape.FACE:
      assert(shape.isFace())
    assert(name != ""), "String empty"
    presented_names.add(name)
  assert(len(presented_names) == groupSelectionList.size()), "Some names are not unique"
