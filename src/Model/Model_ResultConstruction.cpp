// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <Model_ResultConstruction.h>

#include <Model_Data.h>
#include <ModelAPI_CompositeFeature.h>
#include <GeomAlgoAPI_SketchBuilder.h>
#include <GeomAPI_Tools.h>
#include <ModelAPI_Events.h>
#include <Model_Document.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_Shape.h>
#include <Events_Loop.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>

#include <TDF_ChildIDIterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Builder.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_UAttribute.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_ListOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <NCollection_IndexedDataMap.hxx>

#include <algorithm>

typedef NCollection_IndexedDataMap<TopoDS_Face, TColStd_ListOfInteger> MapFaceToEdgeIndices;

/// Convert each edge of sketch to corresponding integer value
/// \param[in]  theComposite      sketch feature
/// \param[out] theCurvesIndices  map curve to its index
/// \param[out] theEdgesIndices   indexed edge
/// \param[out] theEdgesNames     indexed name for edge
static void indexingSketchEdges(
    const CompositeFeaturePtr& theComposite,
    NCollection_DataMap<Handle(Geom_Curve), int>& theCurvesIndices,
    NCollection_DataMap<int, TopoDS_Edge>& theEdgesIndices,
    std::map<int, std::string>& theEdgesNames);

/// Convert each face to the list of indices of its edges
/// \param[in]  theFaces          list of faces to proceed
/// \param[in]  theCurvesIndices  index of each curve
/// \param[out] theFaceEdges      map face to indices of its edges
static void faceToEdgeIndices(
    const ListOfShape& theFaces,
    const NCollection_DataMap<Handle(Geom_Curve), int>& theCurvesIndices,
    MapFaceToEdgeIndices& theFaceEdges);

/// Assign faces to tags for the specified label
/// \param theDocument        current document
/// \param theShapeLabel      label to store shapes
/// \param theName            name of the object
/// \param theShape           shape to be stored to the label
/// \param theFacesOrder      faces to be assigned to specified tag
/// \param theUnorderedFaces  faces which may be stored to any tag
/// \param theFaceEdges       indices of edges for each face
/// \param theEdgesIndices    indices of edges
/// \param theEdgesNames      named of edges
static void storeFacesOnLabel(std::shared_ptr<Model_Document>& theDocument,
                              TDF_Label& theShapeLabel,
                              const std::string& theName,
                              const TopoDS_Shape& theShape,
                              NCollection_DataMap<int, TopoDS_Face>& theFacesOrder,
                              NCollection_List<TopoDS_Face>& theUnorderedFaces,
                              const MapFaceToEdgeIndices& theFaceEdges,
                              const NCollection_DataMap<int, TopoDS_Edge>& theEdgesIndices,
                              const std::map<int, std::string>& theEdgesNames);


// identifier of the infinite result
Standard_GUID kIS_INFINITE("dea8cc5a-53f2-49c1-94e8-a947bed20a9f");
// identifier of the result not in history
Standard_GUID kIS_IN_HISTORY("a9aec01c-805e-44d1-b5d2-a63f06522f8a");

void Model_ResultConstruction::colorConfigInfo(std::string& theSection, std::string& theName,
                                       std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_construction_color";
  theDefault = DEFAULT_COLOR();
}

void Model_ResultConstruction::setShape(std::shared_ptr<GeomAPI_Shape> theShape)
{
  if (myShape != theShape) {
    storeShape(theShape);
    if (!theShape.get() || !theShape->isEqual(myShape)) {
      static const Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
      ModelAPI_EventCreator::get()->sendUpdated(data()->owner(), anEvent);
    }
    myShape = theShape;
  }
}

std::shared_ptr<GeomAPI_Shape> Model_ResultConstruction::shape()
{
  return myShape;
}

static std::string shortName(
  std::shared_ptr<ModelAPI_ResultConstruction>& theConstr)
{
  std::string aName = theConstr->data()->name();
  // remove "-", "/" and "&" command-symbols
  aName.erase(std::remove(aName.begin(), aName.end(), '-'), aName.end());
  aName.erase(std::remove(aName.begin(), aName.end(), '/'), aName.end());
  aName.erase(std::remove(aName.begin(), aName.end(), '&'), aName.end());
  // remove the last 's', 'e', 'f' and 'r' symbols:
  // they are used as markers of start/end/forward/reversed indicators
  static const std::string aSyms("sefr");
  std::string::iterator aSuffix = aName.end() - 1;
  while(aSyms.find(*aSuffix) != std::string::npos) {
    --aSuffix;
  }
  aName.erase(aSuffix + 1, aName.end());
  return aName;
}

bool Model_ResultConstruction::updateShape()
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData && aData->isValid()) {
    TDF_Label aShapeLab = aData->shapeLab();
    Handle(TNaming_NamedShape) aNS;
    if (aShapeLab.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
      TopoDS_Shape aShape = aNS->Get();
      if (!aShape.IsNull()) {
        if (aShape.ShapeType() == TopAbs_COMPOUND) {
          // restore the sketch planar edges object
          std::shared_ptr<GeomAPI_PlanarEdges> aBigWire(new GeomAPI_PlanarEdges);
          aBigWire->setImpl<TopoDS_Shape>(new TopoDS_Shape(aShape));
          FeaturePtr aSketch =
            document()->feature(std::dynamic_pointer_cast<ModelAPI_Result>(data()->owner()));
          std::shared_ptr<GeomDataAPI_Point> anOrigin =
            std::dynamic_pointer_cast<GeomDataAPI_Point>(aSketch->data()->attribute("Origin"));
          std::shared_ptr<GeomDataAPI_Dir> aDirX =
            std::dynamic_pointer_cast<GeomDataAPI_Dir>(aSketch->data()->attribute("DirX"));
          std::shared_ptr<GeomDataAPI_Dir> aNorm =
            std::dynamic_pointer_cast<GeomDataAPI_Dir>(aSketch->data()->attribute("Norm"));
          if (anOrigin.get() && aDirX.get() && aNorm.get()) {
            aBigWire->setPlane(anOrigin->pnt(), aDirX->dir(), aNorm->dir());
            myShape = aBigWire;
            return true;
          }
        }
        // just restore shape
        GeomShapePtr aGShape(new GeomAPI_Shape);
        aGShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(aShape));
        myShape = GeomAPI_Tools::getTypedShape(aGShape); // restore the sketch sub-components
        return true;
      }
    }
  }
  return false;
}

Model_ResultConstruction::Model_ResultConstruction()
{
}

bool Model_ResultConstruction::isInHistory()
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData.get() && aData->isValid()) {
    return !aData->label().IsAttribute(kIS_IN_HISTORY); // by default no attribute, but in history
  }
  return true;  // unknown case
}

void Model_ResultConstruction::setIsInHistory(const bool isInHistory)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData.get() && aData->isValid()) {
    if (!isInHistory)
      TDataStd_UAttribute::Set(aData->label(), kIS_IN_HISTORY);
    else
      aData->label().ForgetAttribute(kIS_IN_HISTORY);
  }
}

bool Model_ResultConstruction::isInfinite()
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData.get() && aData->isValid()) {
    return aData->label().IsAttribute(kIS_INFINITE);
  }
  return false;  // unknown case
}

void Model_ResultConstruction::setInfinite(const bool theInfinite)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData.get() && aData->isValid()) {
    if (theInfinite)
      TDataStd_UAttribute::Set(aData->label(), kIS_INFINITE);
    else
      aData->label().ForgetAttribute(kIS_INFINITE);
  }
}

int Model_ResultConstruction::facesNum(const bool theUpdateNaming)
{
  int aResult = 0;
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData.get() && aData->isValid()) {
    TDF_Label aShapeLab = aData->shapeLab();
    TDF_ChildIDIterator anOldIter(aShapeLab, TDataStd_IntPackedMap::GetID());
    for (; anOldIter.More(); anOldIter.Next()) {
      aResult++;
    }
  }
  return aResult;
}

std::shared_ptr<GeomAPI_Face> Model_ResultConstruction::face(const int theIndex)
{
  std::shared_ptr<GeomAPI_Face> aResult;
  int anIndex = 0;
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData.get() && aData->isValid()) {
    TDF_Label aShapeLab = aData->shapeLab();
    TDF_ChildIDIterator anOldIter(aShapeLab, TDataStd_IntPackedMap::GetID());
    for (; anOldIter.More(); anOldIter.Next()) {
      if (anIndex == theIndex) {
        Handle(TNaming_NamedShape) aNS;
        anOldIter.Value()->Label().FindAttribute(TNaming_NamedShape::GetID(), aNS);
        aResult.reset(new GeomAPI_Face);
        aResult->setImpl(new TopoDS_Shape(aNS->Get()));
        break;
      }
      anIndex++;
    }
  }
  return aResult;
}

void Model_ResultConstruction::setIsConcealed(const bool theValue, const bool theForced)
{
  // the construction element may be concealed only by "delete" feature
  if (!theValue || theForced) {
    ModelAPI_ResultConstruction::setIsConcealed(theValue, theForced);
  }
}

void Model_ResultConstruction::storeShape(std::shared_ptr<GeomAPI_Shape> theShape)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData && aData->isValid()) {
    std::string aMyName = data()->name();
    TDF_Label aShapeLab = aData->shapeLab();
    if (!theShape.get() || theShape->isNull()) {
      aShapeLab.ForgetAllAttributes();
      TDataStd_Name::Set(aShapeLab, aMyName.c_str()); // restore name forgotten
      return;
    }
    std::shared_ptr<Model_Document> aMyDoc =
      std::dynamic_pointer_cast<Model_Document>(document());
    const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
    if (isInfinite() || aShape.ShapeType() == TopAbs_VERTEX) {
      aShapeLab.ForgetAllAttributes(); // clear all previously stored
      TNaming_Builder aBuilder(aShapeLab);
      aBuilder.Generated(aShape);
      TDataStd_Name::Set(aShapeLab, aMyName.c_str());
      aMyDoc->addNamingName(aShapeLab, aMyName);
    } else if (aShape.ShapeType() == TopAbs_EDGE) { // store sub-vertices on sub-labels
      aShapeLab.ForgetAllAttributes(); // clear all previously stored
      TNaming_Builder aBuilder(aShapeLab);
      aBuilder.Generated(aShape);

      TopExp_Explorer anExp(aShape, TopAbs_VERTEX);
      for(int anIndex = 1; anExp.More(); anExp.Next(), anIndex++) {
        TDF_Label aSubLab = aShapeLab.FindChild(anIndex);
        TNaming_Builder aBuilder(aSubLab);
        aBuilder.Generated(anExp.Current());
        std::string aVertexName = aMyName + "_" + (anIndex == 1 ? "StartVertex" : "EndVertex");
        TDataStd_Name::Set(aSubLab, aVertexName.c_str());
        aMyDoc->addNamingName(aSubLab, aVertexName);
      }
      TDataStd_Name::Set(aShapeLab, aMyName.c_str());
      aMyDoc->addNamingName(aShapeLab, aMyName);
    } else { // this is probably sketch, so, work with it as with composite
      std::shared_ptr<GeomAPI_PlanarEdges> aWirePtr =
        std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(theShape);
      if (!aWirePtr.get())
        return; // unknown case
      ResultPtr aThisPtr = std::dynamic_pointer_cast<ModelAPI_Result>(data()->owner());
      FeaturePtr aThisFeature = aMyDoc->feature(aThisPtr);
      CompositeFeaturePtr aComposite =
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aThisFeature);
      if (!aComposite || aComposite->numberOfSubs() == 0)
        return; // unknown case
      // collect indices of curves of current composite
      NCollection_DataMap<Handle(Geom_Curve), int> aCurvesIndices;
      NCollection_DataMap<int, TopoDS_Edge> anEdgeIndices;
      std::map<int, std::string> aComponentsNames; // names of components that lay on index
      indexingSketchEdges(aComposite, aCurvesIndices, anEdgeIndices, aComponentsNames);

      GeomAlgoAPI_SketchBuilder aSketchBuilder(aWirePtr->origin(), aWirePtr->dirX(),
                                               aWirePtr->norm(), aWirePtr);
      const ListOfShape& aFaces = aSketchBuilder.faces();
      // order is important to store faces in the same order if sketch is created from scratch
      MapFaceToEdgeIndices aNewIndices; // edges indices
      faceToEdgeIndices(aFaces, aCurvesIndices, aNewIndices);

      NCollection_DataMap<int, TopoDS_Face> aFacesOrder; // faces -> tag where they must be set
      NCollection_List<TopoDS_Face> anUnorderedFaces; // faces that may be located at any index
      // searching for the best new candidate to old location
      MapFaceToEdgeIndices::Iterator aNewIter(aNewIndices);
      for (; aNewIter.More(); aNewIter.Next()) {
        double aBestFound = 0, aBestNotFound = 1.e+100;
        int aBestTag = 0;
        const TColStd_ListOfInteger& aNewInd = aNewIter.Value();
        // old faces indices where they where located
        TDF_ChildIDIterator anOldIter(aShapeLab, TDataStd_IntPackedMap::GetID());
        for (; anOldIter.More(); anOldIter.Next()) {
          int aTag = anOldIter.Value()->Label().Tag();
          if (aFacesOrder.IsBound(aTag))
            continue; // already found a best candidate
          Handle(TDataStd_IntPackedMap) anOldIndices =
            Handle(TDataStd_IntPackedMap)::DownCast(anOldIter.Value());
          double aFound = 0, aNotFound = 0;
          TColStd_ListOfInteger::Iterator aNewIndIter(aNewInd);
          for (; aNewIndIter.More(); aNewIndIter.Next()) {
            if (anOldIndices->Contains(aNewIndIter.Value())) {
              aFound += 1.;
            }
            else if (anOldIndices->Contains(-aNewIndIter.Value())) { // different orientation
              aFound += 0.001;
            }
            else {
              aNotFound += 1.;
            }
          }
          if (aNotFound <= aBestNotFound) { // less and equal to find better "found": #2859
            if (aFound > aBestFound) {
              aBestNotFound = aNotFound;
              aBestFound = aFound;
              aBestTag = aTag;
            }
          }
        }
        if (aBestTag != 0) { // found an appropriate face
          aFacesOrder.Bind(aBestTag, aNewIter.Key());
        } else {
          anUnorderedFaces.Append(aNewIter.Key());
        }
      }
      storeFacesOnLabel(aMyDoc, aShapeLab, aMyName, aShape, aFacesOrder, anUnorderedFaces,
                        aNewIndices, anEdgeIndices, aComponentsNames);
    }
  }
}

void Model_ResultConstruction::setFacesOrder(const std::list<GeomFacePtr>& theFaces)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData && aData->isValid()) {
    std::string aMyName = data()->name();
    TDF_Label aShapeLab = aData->shapeLab();
    GeomShapePtr aResShape = shape();
    if (!aResShape.get() || aResShape->isNull()) {
      // do nothing
      return;
    }
    std::shared_ptr<Model_Document> aMyDoc =
        std::dynamic_pointer_cast<Model_Document>(document());
    const TopoDS_Shape& aShape = aResShape->impl<TopoDS_Shape>();
    if (aShape.ShapeType() != TopAbs_VERTEX &&
        aShape.ShapeType() != TopAbs_EDGE) {
      ResultPtr aThisPtr = std::dynamic_pointer_cast<ModelAPI_Result>(data()->owner());
      FeaturePtr aThisFeature = aMyDoc->feature(aThisPtr);
      CompositeFeaturePtr aComposite =
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aThisFeature);
      if (!aComposite || aComposite->numberOfSubs() == 0)
        return; // unknown case
      // collect indices of curves of current composite
      NCollection_DataMap<Handle(Geom_Curve), int> aCurvesIndices;
      NCollection_DataMap<int, TopoDS_Edge> anEdgeIndices;
      std::map<int, std::string> aComponentsNames; // names of components that lay on index
      indexingSketchEdges(aComposite, aCurvesIndices, anEdgeIndices, aComponentsNames);

      ListOfShape aFaces;
      NCollection_DataMap<int, TopoDS_Face> aFacesOrder; // faces -> tag where they must be set
      NCollection_List<TopoDS_Face> anUnorderedFaces; // unordered faces are empty in this case
      int aTagId = 0;
      for (std::list<GeomFacePtr>::const_iterator aFIt = theFaces.begin();
           aFIt != theFaces.end(); ++aFIt) {
        aFaces.push_back(*aFIt);
        aFacesOrder.Bind(++aTagId, (*aFIt)->impl<TopoDS_Face>());
      }

      MapFaceToEdgeIndices aNewIndices; // edges indices
      faceToEdgeIndices(aFaces, aCurvesIndices, aNewIndices);

      storeFacesOnLabel(aMyDoc, aShapeLab, aMyName, aShape, aFacesOrder, anUnorderedFaces,
                        aNewIndices, anEdgeIndices, aComponentsNames);
    }
  }
}

// ==========================     Auxiliary functions     =========================================

void storeFacesOnLabel(std::shared_ptr<Model_Document>& theDocument,
                       TDF_Label& theShapeLabel,
                       const std::string& theName,
                       const TopoDS_Shape& theShape,
                       NCollection_DataMap<int, TopoDS_Face>& theFacesOrder,
                       NCollection_List<TopoDS_Face>& theUnorderedFaces,
                       const MapFaceToEdgeIndices& theFaceEdges,
                       const NCollection_DataMap<int, TopoDS_Edge>& theEdgesIndices,
                       const std::map<int, std::string>& theEdgesNames)
{
  theShapeLabel.ForgetAllAttributes(); // clear all previously stored
  TDataStd_Name::Set(theShapeLabel, theName.c_str()); // restore name forgotten
  TNaming_Builder aBuilder(theShapeLabel); // store the compound to get it ready on open of document
  aBuilder.Generated(theShape);
  theDocument->addNamingName(theShapeLabel, theName);
  // set new faces to the labels
  NCollection_List<TopoDS_Face>::Iterator anUnordered(theUnorderedFaces);
  for (int aCurrentTag = 1; !theFacesOrder.IsEmpty() || anUnordered.More(); aCurrentTag++) {
    TopoDS_Face aFaceToPut;
    if (theFacesOrder.IsBound(aCurrentTag)) {
      aFaceToPut = theFacesOrder.Find(aCurrentTag);
      theFacesOrder.UnBind(aCurrentTag);
    }
    else if (anUnordered.More()) {
      aFaceToPut = anUnordered.Value();
      anUnordered.Next();
    }

    if (aFaceToPut.IsNull())
      continue;

    TopTools_MapOfShape aFaceEdges;
    for (TopExp_Explorer anEdges(aFaceToPut, TopAbs_EDGE); anEdges.More(); anEdges.Next())
      aFaceEdges.Add(anEdges.Current());

    TDF_Label aLab = theShapeLabel.FindChild(aCurrentTag);
    TNaming_Builder aFaceBuilder(aLab);
    aFaceBuilder.Generated(aFaceToPut);
    // store also indices of the new face edges
    Handle(TDataStd_IntPackedMap) aNewMap = TDataStd_IntPackedMap::Set(aLab);
    const TColStd_ListOfInteger& aNewInd = theFaceEdges.FindFromKey(aFaceToPut);
    std::stringstream aName;
    aName<<"Face";
    TopExp_Explorer aPutEdges(aFaceToPut, TopAbs_EDGE);
    TNaming_Builder *anEdgesBuilder = 0, *aVerticesBuilder = 0;
    for(TColStd_ListOfInteger::Iterator anIter(aNewInd); anIter.More(); anIter.Next()) {
      int anIndex = anIter.Value();
      int aModIndex = anIndex > 0 ? anIndex : -anIndex;
      aNewMap->Add(anIndex);
      aName<<"-"<<theEdgesNames.find(aModIndex)->second;
      if (anIter.Value() > 0)
        aName<<"f";
      else
        aName<<"r";
      // collect all edges of the face which are modified in sub-label of the face
      if (theEdgesIndices.IsBound(aModIndex) &&
          !aFaceEdges.Contains(theEdgesIndices.Find(aModIndex))) {
        if (!anEdgesBuilder) {
          TDF_Label anEdgesLabel = aLab.FindChild(1);
          anEdgesBuilder = new TNaming_Builder(anEdgesLabel);
          std::ostringstream aSubName;
          // tag is needed for Test1922 to distinguish sub-edges of different faces
          aSubName<<"SubEdge_"<<aCurrentTag;
          TDataStd_Name::Set(anEdgesLabel, aSubName.str().c_str());
        }
        anEdgesBuilder->Modify(theEdgesIndices.Find(aModIndex), aPutEdges.Current());
      }
      // put also modified vertices, otherwise vertex of original edge has no history
      if (theEdgesIndices.IsBound(aModIndex)) {
        TopExp_Explorer aVExpOld(theEdgesIndices.Find(aModIndex), TopAbs_VERTEX);
        TopExp_Explorer aVExpNew(aPutEdges.Current(), TopAbs_VERTEX);
        for(; aVExpNew.More() && aVExpOld.More(); aVExpNew.Next(), aVExpOld.Next()) {
          if (!aVExpOld.Current().IsSame(aVExpNew.Current())) {
            if (!aVerticesBuilder) {
              TDF_Label aVertLabel = aLab.FindChild(2);
              aVerticesBuilder = new TNaming_Builder(aVertLabel);
              std::ostringstream aSubName;
              // tag is needed for Test1922 to distinguish sub-edges of different faces
              aSubName<<"SubVertex_"<<aCurrentTag;
              TDataStd_Name::Set(aVertLabel, aSubName.str().c_str());
            }
            aVerticesBuilder->Modify(aVExpOld.Current(), aVExpNew.Current());

          }
        }
      }
      aPutEdges.Next();
    }
    if (anEdgesBuilder)
      delete anEdgesBuilder;
    if (aVerticesBuilder)
      delete aVerticesBuilder;
    TDataStd_Name::Set(aLab, TCollection_ExtendedString(aName.str().c_str()));
    theDocument->addNamingName(aLab, aName.str());
    // put also wires to sub-labels to correctly select them instead of collection by edges
    int aWireTag = 3; // first tag is for SubEdge-s, second - for vertices
    for(TopExp_Explorer aWires(aFaceToPut, TopAbs_WIRE); aWires.More(); aWires.Next()) {
      TDF_Label aWireLab = aLab.FindChild(aWireTag);
      TNaming_Builder aWireBuilder(aWireLab);
      aWireBuilder.Generated(aWires.Current());
      std::ostringstream aWireName;
      aWireName<<aName.str()<<"_wire";
      if (aWireTag > 3)
        aWireName<<"_"<<aWireTag - 2;
      TDataStd_Name::Set(aWireLab, aWireName.str().c_str());
      theDocument->addNamingName(aWireLab, aWireName.str());
      aWireTag++;
    }
  }
}

void indexingSketchEdges(const CompositeFeaturePtr& theComposite,
                         NCollection_DataMap<Handle(Geom_Curve), int>& theCurvesIndices,
                         NCollection_DataMap<int, TopoDS_Edge>& theEdgesIndices,
                         std::map<int, std::string>& theEdgesNames)
{
  const int aSubNum = theComposite->numberOfSubs();
  for (int a = 0; a < aSubNum; a++) {
    FeaturePtr aSub = theComposite->subFeature(a);
    const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
    std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResults.cbegin();
    for (; aRes != aResults.cend(); aRes++) {
      ResultConstructionPtr aConstr =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRes);
      if (aConstr->shape() && aConstr->shape()->isEdge()) {
        TopoDS_Edge anEdge = TopoDS::Edge(aConstr->shape()->impl<TopoDS_Shape>());
        Standard_Real aFirst, aLast;
        Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
        theCurvesIndices.Bind(aCurve, a);
        theEdgesIndices.Bind(a, anEdge);
        theEdgesNames[a] = shortName(aConstr);
      }
    }
  }
}

void faceToEdgeIndices(const ListOfShape& theFaces,
                       const NCollection_DataMap<Handle(Geom_Curve), int>& theCurvesIndices,
                       MapFaceToEdgeIndices& theFaceEdges)
{
  std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator aFIter = theFaces.begin();
  for (; aFIter != theFaces.end(); aFIter++) {
    std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(*aFIter));
    // put them to a label, trying to keep the same faces on the same labels
    if (aFace.get() && !aFace->isNull()) {
      TopoDS_Face aTopoFace = TopoDS::Face(aFace->impl<TopoDS_Shape>());
      theFaceEdges.Add(aTopoFace, TColStd_ListOfInteger());
      // keep new indices of sub-elements used in this face
      for (TopExp_Explorer anEdges(aTopoFace, TopAbs_EDGE); anEdges.More(); anEdges.Next()) {
        TopoDS_Edge anEdge = TopoDS::Edge(anEdges.Current());
        Standard_Real aFirst, aLast;
        Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
        if (theCurvesIndices.IsBound(aCurve)) {
          int anIndex = theCurvesIndices.Find(aCurve);
          if ((aFirst > aLast) != (anEdge.Orientation() == TopAbs_REVERSED))
            anIndex = -anIndex;
          theFaceEdges.ChangeFromKey(aTopoFace).Append(anIndex);
        }
      }
    }
  }
}
