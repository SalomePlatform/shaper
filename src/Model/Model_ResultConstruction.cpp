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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <Model_ResultConstruction.h>

#include <Model_Data.h>
#include <ModelAPI_CompositeFeature.h>
#include <GeomAlgoAPI_SketchBuilder.h>
#include <ModelAPI_Events.h>
#include <Model_Document.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_Shape.h>
#include <Events_Loop.h>

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

#include <algorithm>


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
    if (!isInfinite())
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
  // they are used as markers of start/end/forward/rewersed indicators
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
    TDF_Label& aShapeLab = aData->shapeLab();
    Handle(TNaming_NamedShape) aNS;
    if (aShapeLab.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
      TopoDS_Shape aShape = aNS->Get();
      if (!aShape.IsNull()) {
        GeomShapePtr aGShape(new GeomAPI_Shape);
        aGShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(aShape));
        myShape = aGShape; // restore the sketch sub-components
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
    TDF_Label& aShapeLab = aData->shapeLab();
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
    TDF_Label& aShapeLab = aData->shapeLab();
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

void Model_ResultConstruction::setIsConcealed(const bool theValue)
{
  // do nothing: the construction element is never concealed
}

void Model_ResultConstruction::storeShape(std::shared_ptr<GeomAPI_Shape> theShape)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData && aData->isValid()) {
    std::string aMyName = data()->name();
    TDF_Label& aShapeLab = aData->shapeLab();
    if (!theShape.get() || theShape->isNull()) {
      aShapeLab.ForgetAllAttributes();
      TDataStd_Name::Set(aShapeLab, aMyName.c_str()); // restore name forgotten
      return;
    }
    std::shared_ptr<Model_Document> aMyDoc =
      std::dynamic_pointer_cast<Model_Document>(document());
    const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
    if (aShape.ShapeType() == TopAbs_VERTEX) {
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
      const int aSubNum = aComposite->numberOfSubs();
      for (int a = 0; a < aSubNum; a++) {
        FeaturePtr aSub = aComposite->subFeature(a);
        const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
        std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResults.cbegin();
        for (; aRes != aResults.cend(); aRes++) {
          ResultConstructionPtr aConstr =
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRes);
          if (aConstr->shape() && aConstr->shape()->isEdge()) {
            TopoDS_Edge anEdge = TopoDS::Edge(aConstr->shape()->impl<TopoDS_Shape>());
            Standard_Real aFirst, aLast;
            Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
            aCurvesIndices.Bind(aCurve, a);
            anEdgeIndices.Bind(a, anEdge);
            aComponentsNames[a] = shortName(aConstr);
          }
        }
      }

      std::list<std::shared_ptr<GeomAPI_Shape> > aFaces;
      GeomAlgoAPI_SketchBuilder::createFaces(aWirePtr->origin(), aWirePtr->dirX(),
        aWirePtr->norm(), aWirePtr, aFaces);
      NCollection_DataMap<TopoDS_Face, TColStd_ListOfInteger> aNewIndices; // edges indices
      std::list<std::shared_ptr<GeomAPI_Shape> >::iterator aFIter = aFaces.begin();
      for (; aFIter != aFaces.end(); aFIter++) {
        std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(*aFIter));
        // put them to a label, trying to keep the same faces on the same labels
        if (aFace.get() && !aFace->isNull()) {
          TopoDS_Face aTopoFace = TopoDS::Face(aFace->impl<TopoDS_Shape>());
          aNewIndices.Bind(aTopoFace, TColStd_ListOfInteger());
          // keep new indices of sub-elements used in this face
          for (TopExp_Explorer anEdges(aTopoFace, TopAbs_EDGE); anEdges.More(); anEdges.Next()) {
            TopoDS_Edge anEdge = TopoDS::Edge(anEdges.Current());
            Standard_Real aFirst, aLast;
            Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
            if (aCurvesIndices.IsBound(aCurve)) {
              int anIndex = aCurvesIndices.Find(aCurve);
              if ((aFirst > aLast) != (anEdge.Orientation() == TopAbs_REVERSED))
                anIndex = -anIndex;
              aNewIndices.ChangeFind(aTopoFace).Append(anIndex);
            }
          }
        }
      }
      NCollection_DataMap<int, TopoDS_Face> aFacesOrder; // faces -> tag where they must be set
      NCollection_List<TopoDS_Face> anUnorderedFaces; // faces that may be located at any index
      // searching for the best new candidate to old location
      NCollection_DataMap<TopoDS_Face, TColStd_ListOfInteger>::Iterator aNewIter(aNewIndices);
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
          if (aNotFound < aBestNotFound) {
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
      aShapeLab.ForgetAllAttributes(); // clear all previously stored
      TDataStd_Name::Set(aShapeLab, aMyName.c_str()); // restore name forgotten
      TNaming_Builder aBuilder(aShapeLab); // store the compound to get it ready on open of document
      aBuilder.Generated(aShape);
      aMyDoc->addNamingName(aShapeLab, aMyName);
      // set new faces to the labels
      int aCurrentTag = 1;
      NCollection_List<TopoDS_Face>::Iterator anUnordered(anUnorderedFaces);
      for(int aCurrentTag = 1; !aFacesOrder.IsEmpty() || anUnordered.More(); aCurrentTag++) {
        TopoDS_Face aFaceToPut;
        if (aFacesOrder.IsBound(aCurrentTag)) {
          aFaceToPut = aFacesOrder.Find(aCurrentTag);
          aFacesOrder.UnBind(aCurrentTag);
        } else if (anUnordered.More()){
          aFaceToPut = anUnordered.Value();
          anUnordered.Next();
        }

        if (!aFaceToPut.IsNull()) {
          TopTools_MapOfShape aFaceEdges;
          for(TopExp_Explorer anEdges(aFaceToPut, TopAbs_EDGE); anEdges.More(); anEdges.Next()) {
            aFaceEdges.Add(anEdges.Current());
          }

          TDF_Label aLab = aShapeLab.FindChild(aCurrentTag);
          TNaming_Builder aFaceBuilder(aLab);
          aFaceBuilder.Generated(aFaceToPut);
          // store also indices of the new face edges
          Handle(TDataStd_IntPackedMap) aNewMap = TDataStd_IntPackedMap::Set(aLab);
          const TColStd_ListOfInteger& aNewInd = aNewIndices.Find(aFaceToPut);
          std::stringstream aName;
          aName<<"Face";
          TopExp_Explorer aPutEdges(aFaceToPut, TopAbs_EDGE);
          TNaming_Builder* anEdgesBuilder = 0;
          for(TColStd_ListOfInteger::Iterator anIter(aNewInd); anIter.More(); anIter.Next()) {
            int anIndex = anIter.Value();
            int aModIndex = anIndex > 0 ? anIndex : -anIndex;
            aNewMap->Add(anIndex);
            aName<<"-"<<aComponentsNames[aModIndex];
            if (anIter.Value() > 0)
              aName<<"f";
            else
              aName<<"r";
            // collect all edges of the face which are modified in sub-label of the face
            if (anEdgeIndices.IsBound(aModIndex) &&
                !aFaceEdges.Contains(anEdgeIndices.Find(aModIndex))) {
              if (!anEdgesBuilder) {
                TDF_Label anEdgesLabel = aLab.FindChild(1);
                anEdgesBuilder = new TNaming_Builder(anEdgesLabel);
                TDataStd_Name::Set(anEdgesLabel, "SubEdge");
              }
              anEdgesBuilder->Modify(anEdgeIndices.Find(aModIndex), aPutEdges.Current());
            }
            aPutEdges.Next();
          }
          TDataStd_Name::Set(aLab, TCollection_ExtendedString(aName.str().c_str()));
          aMyDoc->addNamingName(aLab, aName.str());
        }
      }
    }
  }
}
