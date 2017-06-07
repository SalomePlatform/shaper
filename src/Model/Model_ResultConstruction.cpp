// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <Model_ResultConstruction.h>

#include <Model_Data.h>
#include <ModelAPI_CompositeFeature.h>
#include <Model_SelectionNaming.h>
#include <ModelAPI_Events.h>
#include <Config_PropManager.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI_SketchBuilder.h>
#include <Events_Loop.h>

#include <TDF_Reference.hxx>
#include <TDF_ChildIterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Builder.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp_Explorer.hxx>
#include <TNaming_Tool.hxx>
#include <Precision.hxx>

// identifier that it is full result selected, but in external document (for internal index is 0)
Standard_GUID kFULL_RESULT_ID("ee87e529-da6f-46af-be25-5e0fefde52f7");


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
    if (!theShape.get() || !theShape->isEqual(myShape)) {
        static const Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
        ModelAPI_EventCreator::get()->sendUpdated(data()->owner(), anEvent);
    }
    myShape = theShape;
    if (theShape.get()) {
      myFacesUpToDate = false;
      myFaces.clear();
    }
  }
}

std::shared_ptr<GeomAPI_Shape> Model_ResultConstruction::shape()
{
  return myShape;
}

Model_ResultConstruction::Model_ResultConstruction()
{
  myIsInHistory = true;
  myIsInfinite = false;
  myFacesUpToDate = false;
}

void Model_ResultConstruction::setIsInHistory(const bool isInHistory)
{
  myIsInHistory = isInHistory;
}

int Model_ResultConstruction::facesNum()
{
  if (!myFacesUpToDate) {
    std::shared_ptr<GeomAPI_PlanarEdges> aWirePtr =
      std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(myShape);
    if (aWirePtr.get()) {
      std::list<std::shared_ptr<GeomAPI_Shape> > aFaces;
      GeomAlgoAPI_SketchBuilder::createFaces(aWirePtr->origin(), aWirePtr->dirX(),
        aWirePtr->norm(), aWirePtr, aFaces);
      std::list<std::shared_ptr<GeomAPI_Shape> >::iterator aFIter = aFaces.begin();
      for(; aFIter != aFaces.end(); aFIter++) {
        std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(*aFIter));
        if (aFace.get() && !aFace->isNull())
          myFaces.push_back(aFace);
      }
    }
    myFacesUpToDate = true;
  }
  return int(myFaces.size());
}

std::shared_ptr<GeomAPI_Face> Model_ResultConstruction::face(const int theIndex)
{
  return myFaces[theIndex];
}

bool Model_ResultConstruction::isInfinite()
{
  return myIsInfinite;
}

void Model_ResultConstruction::setInfinite(const bool theInfinite)
{
  myIsInfinite = theInfinite;
}

void Model_ResultConstruction::setIsConcealed(const bool theValue)
{
  // do nothing: the construction element is never consealed
}

static const int kSTART_VERTEX_DELTA = 1000000;

static void registerSubShape(TDF_Label theMainLabel, TopoDS_Shape theShape, std::string theFullName,
  const int theID, std::shared_ptr<Model_Document> theDoc,
  bool theSelectionMode)
{
  TDF_Label aLab = theID == 0 ? theMainLabel : theMainLabel.FindChild(theID);
  TNaming_Builder aBuilder(aLab);
  // wire never happens as sub, it must be generated to be found
  // by SelectionNaming TNaming_Tool::NamedShape
  if (theSelectionMode && theShape.ShapeType() != TopAbs_WIRE)
    aBuilder.Select(theShape, theShape);
  else
    aBuilder.Generated(theShape);

  theDoc->addNamingName(aLab, theFullName);
  TDataStd_Name::Set(aLab, theFullName.c_str());
}

// generates a full-name for sub-element of the composite feature (sketch)
std::string fullName(CompositeFeaturePtr theComposite, const TopoDS_Shape& theSubShape,
  Handle(TDataStd_IntPackedMap) theRefs = Handle(TDataStd_IntPackedMap)())
{
  TopAbs_ShapeEnum aShapeType = theSubShape.ShapeType();
  gp_Pnt aVertexPos;
  TColStd_MapOfTransient allCurves;
  if (aShapeType == TopAbs_VERTEX) { // compare positions
    aVertexPos = BRep_Tool::Pnt(TopoDS::Vertex(theSubShape));
  } else {
    for(TopExp_Explorer anEdgeExp(theSubShape, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next()) {
      TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExp.Current());
      Standard_Real aFirst, aLast;
      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
      allCurves.Add(aCurve);
    }
  }
  std::map<int, int> anOrientations; //map from edges IDs to orientations of these edges in face
  std::map<int, std::string> aSubNames; //map from edges IDs to names of edges
  TColStd_PackedMapOfInteger aRefs; // indixes of sub-elements in composite

  const int aSubNum = theComposite->numberOfSubs();
  for(int a = 0; a < aSubNum; a++) {
    FeaturePtr aSub = theComposite->subFeature(a);
    const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
    std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResults.cbegin();
    // there may be many shapes (circle and center): register if at least one is in selection
    for(; aRes != aResults.cend(); aRes++) {
      ResultConstructionPtr aConstr =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRes);
      if (!aConstr->shape()) {
        continue;
      }
      if (aShapeType == TopAbs_VERTEX) {
        if (aConstr->shape()->isVertex()) { // compare vertices positions
          const TopoDS_Shape& aVertex = aConstr->shape()->impl<TopoDS_Shape>();
          gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aVertex));
          if (aPnt.IsEqual(aVertexPos, Precision::Confusion())) {
            aRefs.Add(theComposite->subFeatureId(a));
            aSubNames[theComposite->subFeatureId(a)] = Model_SelectionNaming::shortName(aConstr);
          }
        } else { // get first or last vertex of the edge: last is stored with additional delta
          const TopoDS_Shape& anEdge = aConstr->shape()->impl<TopoDS_Shape>();
          int aDelta = kSTART_VERTEX_DELTA;
          for(TopExp_Explorer aVExp(anEdge, TopAbs_VERTEX); aVExp.More(); aVExp.Next()) {
            gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aVExp.Current()));
            if (aPnt.IsEqual(aVertexPos, Precision::Confusion())) {
              aRefs.Add(aDelta + theComposite->subFeatureId(a));
              aSubNames[aDelta + theComposite->subFeatureId(a)] =
                Model_SelectionNaming::shortName(aConstr, aDelta / kSTART_VERTEX_DELTA);
              break;
            }
            aDelta += kSTART_VERTEX_DELTA;
          }
        }
      } else {
        if (aConstr->shape()->isEdge()) {
          const TopoDS_Shape& aResShape = aConstr->shape()->impl<TopoDS_Shape>();
          TopoDS_Edge anEdge = TopoDS::Edge(aResShape);
          if (!anEdge.IsNull()) {
            Standard_Real aFirst, aLast;
            Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
            if (allCurves.Contains(aCurve)) {
              int anID = theComposite->subFeatureId(a);
              aRefs.Add(anID);
              aSubNames[anID] = Model_SelectionNaming::shortName(aConstr);
              if (aShapeType != TopAbs_EDGE) { // face needs the sub-edges on sub-labels
                // add edges to sub-label to support naming for edges selection
                TopExp_Explorer anEdgeExp(theSubShape, TopAbs_EDGE);
                for(; anEdgeExp.More(); anEdgeExp.Next()) {
                  TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExp.Current());
                  Standard_Real aFirst, aLast;
                  Handle(Geom_Curve) aFaceCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
                  if (aFaceCurve == aCurve) {
                    int anOrient = Model_SelectionNaming::edgeOrientation(theSubShape, anEdge);
                    anOrientations[anID] = anOrient;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  std::stringstream aName;
  // #1839 : do not store name of the feature in the tree, since this name could be changed
  if (theSubShape.ShapeType() != TopAbs_COMPOUND) { // compound means the whole construction result
    if (theSubShape.ShapeType() == TopAbs_FACE) aName<<"Face";
    else if (theSubShape.ShapeType() == TopAbs_WIRE) aName<<"Wire";
    else if (theSubShape.ShapeType() == TopAbs_EDGE) aName<<"Edge";
    else if (theSubShape.ShapeType() == TopAbs_VERTEX) aName<<"Vertex";

    // make a composite name from all sub-elements indexes: "1_2_3_4"
    TColStd_MapIteratorOfPackedMapOfInteger aRef(aRefs);
    for(; aRef.More(); aRef.Next()) {
      aName<<"-"<<aSubNames[aRef.Key()];
      if (anOrientations.find(aRef.Key()) != anOrientations.end()) {
        if (anOrientations[aRef.Key()] == 1)
          aName<<"f";
        else if (anOrientations[aRef.Key()] == -1)
          aName<<"r";
      }
    }
  }
  if (!theRefs.IsNull()) {
    Handle(TColStd_HPackedMapOfInteger) aMap = new TColStd_HPackedMapOfInteger(aRefs);
    theRefs->ChangeMap(aMap);
  }
  return aName.str();
}

// stores shape and name on sub-label of the main stored shape
static void saveSubName(TDF_Label& theLab, const bool isSelectionMode, const TopoDS_Shape& aSub,
  std::shared_ptr<Model_Document> theDoc, std::string theFullName)
{
  TNaming_Builder aBuilder(theLab);
  if (isSelectionMode)
    aBuilder.Select(aSub, aSub);
  else
    aBuilder.Generated(aSub);
  theDoc->addNamingName(theLab, theFullName.c_str());
  TDataStd_Name::Set(theLab, theFullName.c_str());
}


TDF_Label Model_ResultConstruction::startLabel(
  const std::shared_ptr<ModelAPI_Document> theExtDoc, bool& theExternal)
{
  theExternal = theExtDoc.get() && theExtDoc != document();
  if (theExternal) { // external document is used
    std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(theExtDoc);
    return aDoc->extConstructionsLabel();
  }
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  return aData->label();
}

int Model_ResultConstruction::select(const std::shared_ptr<GeomAPI_Shape>& theSubShape,
  const std::shared_ptr<ModelAPI_Document> theExtDoc, const int theIndex)
{
  int anIndex; // resulting index of the sub-label
  TopoDS_Shape aSubShape;
  if (theSubShape.get()) {
    aSubShape = theSubShape->impl<TopoDS_Shape>();
  } else if (shape().get()) {
    aSubShape = shape()->impl<TopoDS_Shape>();
  }
  // if external document requires this selection, put the naming structures to this doc
  // to support the naming mechanism in this document correctly
  bool anExternal;
  TDF_Label aDataLab = startLabel(theExtDoc, anExternal);
  if (theIndex == -1) {
    anIndex = anExternal ? 2 : 1; // for the external doc don't mind about the main shape

    if (theSubShape.get() || anExternal) { // searching for already selected sub (or whole for ext)
      // iterate all the already presented shapes to see the same
      TDF_ChildIterator aSubsIter(aDataLab, Standard_False);
      for(; aSubsIter.More(); aSubsIter.Next()) {
        const TDF_Label aLab = aSubsIter.Value();
        if (aLab.Tag() == 1) // skip the root shape label
          continue;
        Handle(TNaming_NamedShape) aNS;
        if (aLab.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
          if (aNS->Get().IsSame(aSubShape)) {
            return aLab.Tag() - 1; // found exactly the needed shape, nothing else to do
          }
        }
        anIndex = aLab.Tag(); // searching for the latest index
      }
      anIndex = (anIndex == 1) ? 2 : (anIndex + 1); // next after 1-root, or next after all
    }
  } else {
    anIndex = theIndex + 1;
  }

  // set the naming structure at index
  TDF_Label aLab = aDataLab.FindChild(anIndex, Standard_True);

  // if the subshape is part of a result face, select the whole face (#1997)
  bool isSelectionMode = false; // and other don't set shapes - all the naming is in face label
  if (!aSubShape.IsNull() && aSubShape.ShapeType() > TopAbs_FACE) {
    for(int aFaceIndex = 0; aFaceIndex < facesNum(); aFaceIndex++) {
      TopExp_Explorer anExp(face(aFaceIndex)->impl<TopoDS_Shape>(), aSubShape.ShapeType());
      for(; anExp.More(); anExp.Next()) {
        if (aSubShape.IsSame(anExp.Current())) { // this is the case: select the whole face
          // here just store the face index (to update face if update of edge is needed)
          TNaming_Builder aBuilder(aLab);
          aBuilder.Select(aSubShape, aSubShape);
          int aFaceSelID = select(face(aFaceIndex), theExtDoc, -1);
          TDF_Reference::Set(aLab, aLab.Father().FindChild(aFaceSelID));
          isSelectionMode = true;
          break;
        }
      }
    }
  }

  // external full result is not identified by index == 0, so, add here the ID
  if (!theSubShape.get()) {
    TDataStd_UAttribute::Set(aLab, kFULL_RESULT_ID);
    // empty NS
    TNaming_Builder aBuilder(aLab);
    // store all sub-faces naming since faces may be used for extrusion, where all edges are needed
    Handle(TDataStd_IntPackedMap) anIndices = TDataStd_IntPackedMap::Set(aLab);
    std::list<int> aFacesIndexes;
    for(int a = 0; a < facesNum(); a++) {
      anIndices->Add(select(face(a), theExtDoc, -1));
    }
    return anIndex - 1;
  }

  { // this to have erased Builder after the shape was generated (NS on this label may be changed)
    TNaming_Builder aBuilder(aLab);
    if (aSubShape.IsNull()) {
      return anIndex - 1; // just keep empty named shape
    }
    // wire never happens as sub, it must be generated to be found
    // by SelectionNaming TNaming_Tool::NamedShape
    if (isSelectionMode && aSubShape.ShapeType() != TopAbs_WIRE) {
      aBuilder.Select(aSubShape, aSubShape);
    } else {
      aBuilder.Generated(aSubShape);
    }
  }

  if (anIndex == 1 && isInfinite()) { // infinitive results has no sub-selection
    return anIndex - 1;
  }
  ResultPtr aThisPtr = std::dynamic_pointer_cast<ModelAPI_Result>(data()->owner());
  FeaturePtr aThisFeature = document()->feature(aThisPtr);
  CompositeFeaturePtr aComposite =
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aThisFeature);
  if (!aComposite || aComposite->numberOfSubs() == 0) {
    // saving of context is enough: result construction contains exactly the needed shape
    return anIndex - 1;
  }

  // identify the results of sub-object of the composite by edges
  // save type of the selected shape in integer attribute
  TopAbs_ShapeEnum aShapeType = aSubShape.ShapeType();
  TDataStd_Integer::Set(aLab, (int)aShapeType);
  gp_Pnt aVertexPos;
  TColStd_MapOfTransient allCurves;
  if (aShapeType == TopAbs_VERTEX) { // compare positions
    aVertexPos = BRep_Tool::Pnt(TopoDS::Vertex(aSubShape));
  } else {
    for(TopExp_Explorer anEdgeExp(aSubShape, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next()) {
      TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExp.Current());
      Standard_Real aFirst, aLast;
      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
      allCurves.Add(aCurve);
    }
  }
  std::shared_ptr<Model_Document> aMyDoc =
    std::dynamic_pointer_cast<Model_Document>(document());
  // iterate and store the result ids of sub-elements and sub-elements to sub-labels
  Handle(TDataStd_IntPackedMap) aRefs = TDataStd_IntPackedMap::Set(aLab);
  const int aSubNum = aComposite->numberOfSubs();
  for(int a = 0; a < aSubNum; a++) {
    FeaturePtr aSub = aComposite->subFeature(a);
    const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
    std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResults.cbegin();
    // there may be many shapes (circle and center): register if at least one is in selection
    for(; aRes != aResults.cend(); aRes++) {
      ResultConstructionPtr aConstr =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRes);
      if (!aConstr->shape()) {
        continue;
      }
      if (aShapeType != TopAbs_VERTEX) {
        if (aConstr->shape()->isEdge()) {
          const TopoDS_Shape& aResShape = aConstr->shape()->impl<TopoDS_Shape>();
          TopoDS_Edge anEdge = TopoDS::Edge(aResShape);
          if (!anEdge.IsNull()) {
            Standard_Real aFirst, aLast;
            Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
            if (allCurves.Contains(aCurve)) {
              int anID = aComposite->subFeatureId(a);
              if (aShapeType != TopAbs_EDGE) { // face needs the sub-edges on sub-labels
                // add edges to sub-label to support naming for edges selection
                TopExp_Explorer anEdgeExp(aSubShape, TopAbs_EDGE);
                for(; anEdgeExp.More(); anEdgeExp.Next()) {
                  TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExp.Current());
                  Standard_Real aFirst, aLast;
                  Handle(Geom_Curve) aFaceCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
                  if (aFaceCurve == aCurve) {
                    TDF_Label aSubLab = aLab.FindChild(anID);
                    std::string aFullNameSub = fullName(aComposite, anEdge);
                    saveSubName(aSubLab, isSelectionMode, anEdge, aMyDoc, aFullNameSub);

                    int anOrient = Model_SelectionNaming::edgeOrientation(aSubShape, anEdge);
                    if (anOrient != 0) {
                      // store the orientation of edge relatively to face if needed
                      TDataStd_Integer::Set(aSubLab, anOrient);
                    }
                  }
                }
              } else { // put vertices of the selected edge to sub-labels
                // add edges to sub-label to support naming for edges selection
                int aTagIndex = anID + kSTART_VERTEX_DELTA;
                for(TopExp_Explorer anEdgeExp(aSubShape, TopAbs_VERTEX);
                  anEdgeExp.More();
                  anEdgeExp.Next(), aTagIndex += kSTART_VERTEX_DELTA) {
                    TopoDS_Vertex aV = TopoDS::Vertex(anEdgeExp.Current());
                    TDF_Label aSubLab = aLab.FindChild(aTagIndex);
                    std::string aFullNameSub = fullName(aComposite, aV);
                    saveSubName(aSubLab, isSelectionMode, aV, aMyDoc, aFullNameSub);
                }
              }
            }
          }
        }
      }
    }
  }
  std::string aFullName = fullName(aComposite, aSubShape, aRefs);
  // store the selected as primitive
  registerSubShape(aLab, aSubShape, aFullName, 0, aMyDoc, isSelectionMode);
  return anIndex - 1;
}

std::shared_ptr<GeomAPI_Shape> Model_ResultConstruction::shape(const int theIndex,
  const std::shared_ptr<ModelAPI_Document> theExtDoc)
{
  std::shared_ptr<GeomAPI_Shape> aResult;
  if (theIndex == 0)
    return aResult; // the whole shape, so, NULL

  bool isExt;
  TDF_Label aLab = startLabel(theExtDoc, isExt).FindChild(theIndex + 1);
  if (!aLab.IsNull()) { // index is not bad
    Handle(TNaming_NamedShape) aSelection;
    if (aLab.FindAttribute(TNaming_NamedShape::GetID(), aSelection)) {
      TopoDS_Shape aSelShape = aSelection->Get();
      if (aSelShape.IsNull())
        return aResult; // shape equal to context => null
      aResult = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape);
      aResult->setImpl(new TopoDS_Shape(aSelShape));
    }
  }

  return aResult;
}

bool Model_ResultConstruction::update(const int theIndex,
  const std::shared_ptr<ModelAPI_Document> theExtDoc, bool& theModified)
{
  theModified = false;
  bool anExt;
  TDF_Label aLab = startLabel(theExtDoc, anExt).FindChild(theIndex + 1, Standard_True);
  if (theIndex == 0 || aLab.IsAttribute(kFULL_RESULT_ID)) { // full for external same as index == 0
    // it is just reference to construction, not sub-shape
    // if there is a sketch, the sketch-naming must be updated
    if (!isInfinite()) {
      // update all faces named by the whole result
      bool aRes = true;
      Handle(TDataStd_IntPackedMap) anIndices;
      if (aLab.FindAttribute(TDataStd_IntPackedMap::GetID(), anIndices)) {
        NCollection_Map<TopoDS_Shape> aFaces; // collect faces, updated in the tree
        TColStd_MapIteratorOfPackedMapOfInteger anIndexIter(anIndices->GetMap());
        Handle(TColStd_HPackedMapOfInteger) aNewPackedMap =
          new TColStd_HPackedMapOfInteger; // with only faces that are ok
        // iterate to find existing faces, updated
        for(; anIndexIter.More(); anIndexIter.Next()) {
          if (update(anIndexIter.Key(), theExtDoc, theModified)) {
            GeomShapePtr aFace = shape(anIndexIter.Key(), theExtDoc);
            if (!aFaces.Contains(aFace->impl<TopoDS_Shape>())) {
              aNewPackedMap->ChangeMap().Add(anIndexIter.Key());
              aFaces.Add(aFace->impl<TopoDS_Shape>());
            }
          }
        }
        // then iterate all existing faces to find new faces
        int aCurrentFacesNum = facesNum();
        for(int a = 0; a < aCurrentFacesNum; a++) {
          GeomShapePtr aFace = face(a);
          if (!aFaces.Contains(aFace->impl<TopoDS_Shape>())) {
            // add this one
            int aNewFaceIndex = select(aFace, theExtDoc, -1);
            if (aNewFaceIndex > 0) {
              aNewPackedMap->ChangeMap().Add(aNewFaceIndex);
            }
          }
        }
        anIndices->ChangeMap(aNewPackedMap);
      }
      return aRes;
    } else {
      // For correct naming selection, put the shape into the naming structure.
      // It seems sub-shapes are not needed: only this shape is (and can be ) selected.
      TNaming_Builder aBuilder(aLab);
      aBuilder.Generated(shape()->impl<TopoDS_Shape>());
    }
    return shape() && !shape()->isNull();
  }
  // construction: identification by the results indexes, recompute faces and
  // take the face that more close by the indexes
  ResultPtr aThisPtr = std::dynamic_pointer_cast<ModelAPI_Result>(data()->owner());
  FeaturePtr aContextFeature = document()->feature(aThisPtr);

  // sketch sub-element
  if (std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aContextFeature).get())
  {
    // update the referenced object if it is sub
    Handle(TDF_Reference) aRef;
    if (aLab.FindAttribute(TDF_Reference::GetID(), aRef)) {
      int aFaceIndex = aRef->Get().Tag();
      // don't check selection ,since face may disappear, but the shape stays correct
      Model_ResultConstruction::update(aFaceIndex, theExtDoc, theModified);
    }
    // getting a type of selected shape
    Handle(TDataStd_Integer) aTypeAttr;
    if (!aLab.FindAttribute(TDataStd_Integer::GetID(), aTypeAttr)) {
      return false;
    }
    TopAbs_ShapeEnum aShapeType = (TopAbs_ShapeEnum)(aTypeAttr->Get());
    // selected indexes will be needed in each "if"
    Handle(TDataStd_IntPackedMap) aSubIds;
    std::shared_ptr<GeomAPI_Shape> aNewSelected;
    bool aNoIndexes =
      !aLab.FindAttribute(TDataStd_IntPackedMap::GetID(), aSubIds) || aSubIds->Extent() == 0;
    // for now working only with composite features
    CompositeFeaturePtr aComposite =
      std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aContextFeature);
    if (!aComposite.get() || aComposite->numberOfSubs() == 0) {
      return false;
    }

    if (aShapeType == TopAbs_FACE || aShapeType == TopAbs_WIRE) {
      // compound is for the whole sketch selection
      // If this is a wire with plane defined then it is a sketch-like object
      if (!facesNum()) // no faces, update can not work correctly
        return false;
      // if there is no edges indexes, any face can be used: take the first
      std::shared_ptr<GeomAPI_Shape> aNewSelected;
      if (aNoIndexes) {
        aNewSelected = face(0);
      } else { // searching for most looks-like initial face by the indexes
        // prepare edges of the current result for the fast searching
        // curves and orientations of edges
        NCollection_DataMap<Handle(Geom_Curve), int> allCurves;
        const int aSubNum = aComposite->numberOfSubs();
        for(int a = 0; a < aSubNum; a++) {
          int aSubID = aComposite->subFeatureId(a);
          if (aSubIds->Contains(aSubID)) {
            FeaturePtr aSub = aComposite->subFeature(a);
            const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
            std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes;
            for(aRes = aResults.cbegin(); aRes != aResults.cend(); aRes++) {
              ResultConstructionPtr aConstr =
                std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRes);
              if (aConstr->shape() && aConstr->shape()->isEdge()) {
                const TopoDS_Shape& aResShape = aConstr->shape()->impl<TopoDS_Shape>();
                TopoDS_Edge anEdge = TopoDS::Edge(aResShape);
                if (!anEdge.IsNull()) {
                  Standard_Real aFirst, aLast;
                  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
                  // searching for orientation information
                  int anOrient = 0;
                  Handle(TDataStd_Integer) anInt;
                  if (aLab.FindChild(aSubID).FindAttribute(TDataStd_Integer::GetID(), anInt)) {
                    anOrient = anInt->Get();
                  }
                  allCurves.Bind(aCurve, anOrient);
                }
              }
            }
          }
        }
        aNewSelected = Model_SelectionNaming::findAppropriateFace(
          aThisPtr, allCurves, aShapeType == TopAbs_WIRE);
      }
      if (aNewSelected) { // store this new selection
        select(aNewSelected, theExtDoc, theIndex);
        theModified = true;
        return true;
      } else {
        // if the selection is not found, put the empty shape:
        // it's better to have disappeared shape, than the old, the lost one
        TNaming_Builder anEmptyBuilder(aLab);
        return false;
      }
    } else if (aShapeType == TopAbs_EDGE) {
      // just reselect the edge by the id
      const int aSubNum = aComposite->numberOfSubs();
      for(int a = 0; a < aSubNum; a++) {
        // if aSubIds take any, the first appropriate
        if (aSubIds->IsEmpty() || aSubIds->Contains(aComposite->subFeatureId(a))) {
          // found the appropriate feature
          FeaturePtr aFeature = aComposite->subFeature(a);
          std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResIter =
            aFeature->results().cbegin();
          for(;aResIter != aFeature->results().cend(); aResIter++) {
            ResultConstructionPtr aRes =
              std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aResIter);
            if (aRes && aRes->shape() && aRes->shape()->isEdge()) { // found!
              select(aRes->shape(), theExtDoc, theIndex);
              theModified = true;
              return true;
            }
          }
        }
      }
    } else if (aShapeType == TopAbs_VERTEX) {
      // just reselect the vertex by the id of edge
      const int aSubNum = aComposite->numberOfSubs();
      for(int a = 0; a < aSubNum; a++) {
        // if aSubIds take any, the first appropriate
        int aFeatureID = aComposite->subFeatureId(a);
        if (aSubIds->IsEmpty() || aSubIds->Contains(aFeatureID) ||
          aSubIds->Contains(aFeatureID + kSTART_VERTEX_DELTA) ||
          aSubIds->Contains(aFeatureID + kSTART_VERTEX_DELTA * 2)) {
            // searching for deltas
            int aVertexNum = 0;
            if (aSubIds->Contains(aFeatureID + kSTART_VERTEX_DELTA)) aVertexNum = 1;
            else if (aSubIds->Contains(aFeatureID + kSTART_VERTEX_DELTA * 2)) aVertexNum = 2;
            // found the feature with appropriate edge
            FeaturePtr aFeature = aComposite->subFeature(a);
            std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResIter =
              aFeature->results().cbegin();
            for(;aResIter != aFeature->results().cend(); aResIter++) {
              ResultConstructionPtr aRes =
                std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aResIter);
              if (aRes && aRes->shape()) {
                if (aRes->shape()->isVertex() && aVertexNum == 0) { // found!
                  select(aRes->shape(), theExtDoc, theIndex);
                  theModified = true;
                  return true;
                } else if (aRes->shape()->isEdge() && aVertexNum > 0) {
                  const TopoDS_Shape& anEdge = aRes->shape()->impl<TopoDS_Shape>();
                  int aVIndex = 1;
                  for(TopExp_Explorer aVExp(anEdge, TopAbs_VERTEX); aVExp.More(); aVExp.Next()) {
                    if (aVIndex == aVertexNum) { // found!
                      std::shared_ptr<GeomAPI_Shape> aVertex(new GeomAPI_Shape);
                      aVertex->setImpl(new TopoDS_Shape(aVExp.Current()));
                      select(aVertex, theExtDoc, theIndex);
                      theModified = true;
                      return true;
                    }
                    aVIndex++;
                  }
                }
              }
            }
        }
      }
    }
  } else { // simple construction element: the selected is that needed
    select(shape(), theExtDoc, theIndex);
    theModified = true;
    return true;
  }
  return false; // unknown case
}
