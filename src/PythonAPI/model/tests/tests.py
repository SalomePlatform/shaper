from GeomAlgoAPI import *
from GeomAPI import *
import math


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


def testResultsVolumes(theFeature, theExpectedResultsVolumes, theNbSignificantDigits = 10):
  """ Tests results volumes.
  :param theFeature: feature to test.
  :param theExpectedResultsVolumes: list of results volumes. Size of list should be equal to len(theFeature.results()).
  """
  aNbResults = len(theFeature.results())
  aListSize = len(theExpectedResultsVolumes)
  assert (aNbResults == aListSize), "Number of results: {} not equal to list size: {}.".format(aNbResults, aListSize)
  for anIndex in range(0, aNbResults):
    aResultVolume = GeomAlgoAPI_ShapeTools_volume(theFeature.results()[anIndex].resultSubShapePair()[0].shape())
    aResultVolumeStr = "{:0.27f}".format(aResultVolume).lstrip("0").lstrip(".").lstrip("0")
    anExpectedResultVolume = theExpectedResultsVolumes[anIndex]
    anExpectedResultVolumeStr = "{:0.27f}".format(anExpectedResultVolume).lstrip("0").lstrip(".").lstrip("0")
    assert (aResultVolumeStr[:theNbSignificantDigits] == anExpectedResultVolumeStr[:theNbSignificantDigits]), "Volume of result[{}]: {:0.27f}. Expected: {:0.27f}. The first {} significant digits not equal.".format(anIndex, aResultVolume, anExpectedResultVolume, theNbSignificantDigits)

def testHaveNamingFaces(theFeature, theModel, thePartDoc) :
  """ Tests if all faces of result have a name
  :param theFeature: feature to test.
  """
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
    assert(shape.isFace())
    assert(name != ""), "String empty"
