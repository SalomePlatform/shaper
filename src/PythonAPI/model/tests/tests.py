from GeomAlgoAPI import *
import math


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
