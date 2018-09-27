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

#include <Model_BodyBuilder.h>

#include <Model_Data.h>
#include <Model_Document.h>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_Tool.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Integer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Reference.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopExp.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools_History.hxx>
#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAlgoAPI_SortListOfShapes.h>
#include <Config_PropManager.h>
// DEB
//#include <TCollection_AsciiString.hxx>
//#include <TDF_Tool.hxx>
//#define DEB_IMPORT 1

Model_BodyBuilder::Model_BodyBuilder(ModelAPI_Object* theOwner)
: ModelAPI_BodyBuilder(theOwner),
  myDividedIndex(1),
  myVIndex(1),
  myEIndex(1),
  myFIndex(1)
{
}

// Converts evolution of naming shape to selection evelution and back to avoid
// naming support on the disabled results. Deeply in the labels tree, recursively.
static void evolutionToSelectionRec(TDF_Label theLab, const bool theFlag) {
  std::list<std::pair<TopoDS_Shape, TopoDS_Shape> > aShapePairs; // to store old and new shapes
  Handle(TNaming_NamedShape) aName;
  int anEvolution = -1;
  if (theLab.FindAttribute(TNaming_NamedShape::GetID(), aName)) {
    TNaming_Evolution aNSEvol = aName->Evolution();
    if ((aNSEvol == TNaming_SELECTED && theFlag) ||
        (aNSEvol != TNaming_SELECTED && !theFlag)) { // nothing to do, it is already correct
      return;
    }
    anEvolution = (int)(aNSEvol);
    if (!theFlag) {
      Handle(TDataStd_Integer) anAttrEvol;
      if (theLab.FindAttribute(TDataStd_Integer::GetID(), anAttrEvol)) {
        anEvolution = anAttrEvol->Get();
      }
    } else {
      TDataStd_Integer::Set(theLab, anEvolution);
    }

    for(TNaming_Iterator anIter(aName); anIter.More(); anIter.Next()) {
      // iterator goes in reversed order relatively to the Builder, to, make the list reversed
      aShapePairs.push_front(std::pair<TopoDS_Shape, TopoDS_Shape>
        (anIter.OldShape(), anIter.NewShape()));
    }

    // create new
    TNaming_Builder aBuilder(theLab);
    TNaming_Evolution anEvol = (TNaming_Evolution)(anEvolution);
    std::list<std::pair<TopoDS_Shape, TopoDS_Shape> >::iterator aPairsIter = aShapePairs.begin();
    for(; aPairsIter != aShapePairs.end(); aPairsIter++) {
      if (theFlag) { // disabled => make selection
        if (anEvolution == TNaming_DELETE) // issue 2274 : don't put too many same null shapes
          aBuilder.Select(aPairsIter->first, aPairsIter->first);
        else if (anEvolution == TNaming_PRIMITIVE)
          aBuilder.Select(aPairsIter->second, aPairsIter->second);
        else
          aBuilder.Select(aPairsIter->second, aPairsIter->first);
      } else if (anEvol == TNaming_GENERATED) {
        aBuilder.Generated(aPairsIter->first, aPairsIter->second);
      } else if (anEvol == TNaming_MODIFY) {
        aBuilder.Modify(aPairsIter->first, aPairsIter->second);
      } else if (anEvol == TNaming_DELETE) {
        aBuilder.Delete(aPairsIter->first);
      } else if (anEvol == TNaming_PRIMITIVE) {
        aBuilder.Generated(aPairsIter->second);
      } else if (anEvol == TNaming_SELECTED) {
        aBuilder.Select(aPairsIter->second, aPairsIter->first);
      }
    }
  }
  // recursive call for all sub-labels
  TDF_ChildIterator anIter(theLab, Standard_False);
  for(; anIter.More(); anIter.Next()) {
    evolutionToSelectionRec(anIter.Value(), theFlag);
  }
}

void Model_BodyBuilder::evolutionToSelection(const bool theFlag)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (!aData || !aData->isValid()) // unknown case
    return;
  TDF_Label& aShapeLab = aData->shapeLab();
  evolutionToSelectionRec(aShapeLab, theFlag);
}

void Model_BodyBuilder::store(const std::shared_ptr<GeomAPI_Shape>& theShape,
                              const bool theIsStoreSameShapes)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label& aShapeLab = aData->shapeLab();
    // clean builders
    clean();
    // store the new shape as primitive
    TNaming_Builder aBuilder(aShapeLab);
    if (!theShape)
      return;  // bad shape
    TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
    if (aShape.IsNull())
      return;  // null shape inside

    if(!theIsStoreSameShapes) {
      Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(aShape, aShapeLab);
      if(!aNS.IsNull() && !aNS->IsEmpty()) {
        // This shape is already in document, store reference instead of shape;
        const TDF_Label aFoundLabel = aNS->Label();
        TDF_Reference::Set(aShapeLab, aFoundLabel);
        aShapeLab.ForgetAttribute(TNaming_NamedShape::GetID());
        return;
      }
    }

    aBuilder.Generated(aShape);
    // register name
    aShapeLab.ForgetAttribute(TDF_Reference::GetID());
    if(!aBuilder.NamedShape()->IsEmpty()) {
      Handle(TDataStd_Name) anAttr;
      if(aBuilder.NamedShape()->Label().FindAttribute(TDataStd_Name::GetID(),anAttr)) {
        std::string aName (TCollection_AsciiString(anAttr->Get()).ToCString());
        if(!aName.empty()) {
          std::shared_ptr<Model_Document> aDoc =
            std::dynamic_pointer_cast<Model_Document>(document());
          aDoc->addNamingName(aBuilder.NamedShape()->Label(), aName);
        }
      }
    }
  }
}

void Model_BodyBuilder::storeGenerated(const std::shared_ptr<GeomAPI_Shape>& theFromShape,
  const std::shared_ptr<GeomAPI_Shape>& theToShape)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label& aShapeLab = aData->shapeLab();
    // clean builders
    clean();
    // store the new shape as primitive
    TNaming_Builder aBuilder(aShapeLab);
    if (!theFromShape || !theToShape)
      return;  // bad shape
    TopoDS_Shape aShapeBasis = theFromShape->impl<TopoDS_Shape>();
    if (aShapeBasis.IsNull())
      return;  // null shape inside
    TopoDS_Shape aShapeNew = theToShape->impl<TopoDS_Shape>();
    if (aShapeNew.IsNull())
      return;  // null shape inside
    aBuilder.Generated(aShapeBasis, aShapeNew);
    // register name
    if(!aBuilder.NamedShape()->IsEmpty()) {
      Handle(TDataStd_Name) anAttr;
      if(aBuilder.NamedShape()->Label().FindAttribute(TDataStd_Name::GetID(),anAttr)) {
        std::string aName (TCollection_AsciiString(anAttr->Get()).ToCString());
        if(!aName.empty()) {
          std::shared_ptr<Model_Document> aDoc =
            std::dynamic_pointer_cast<Model_Document>(document());
          aDoc->addNamingName(aBuilder.NamedShape()->Label(), aName);
        }
      }
    }
  }
}

TNaming_Builder* Model_BodyBuilder::builder(const int theTag)
{
  std::map<int, TNaming_Builder*>::iterator aFind = myBuilders.find(theTag);
  if (aFind == myBuilders.end()) {
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
    myBuilders[theTag] = new TNaming_Builder(
      theTag == 0 ? aData->shapeLab() : aData->shapeLab().FindChild(theTag));
    aFind = myBuilders.find(theTag);
  }
  return aFind->second;
}

void Model_BodyBuilder::storeModified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
  const std::shared_ptr<GeomAPI_Shape>& theNewShape, const int theDecomposeSolidsTag)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label& aShapeLab = aData->shapeLab();
    // clean builders
    if (theDecomposeSolidsTag != -2)
      clean();
    // store the new shape as primitive
    TNaming_Builder* aBuilder = builder(0);
    if (!theOldShape || !theNewShape)
      return;  // bad shape
    TopoDS_Shape aShapeOld = theOldShape->impl<TopoDS_Shape>();
    if (aShapeOld.IsNull())
      return;  // null shape inside
    TopoDS_Shape aShapeNew = theNewShape->impl<TopoDS_Shape>();
    if (aShapeNew.IsNull())
      return;  // null shape inside
    aBuilder->Modify(aShapeOld, aShapeNew);
    if(!aBuilder->NamedShape()->IsEmpty()) {
      Handle(TDataStd_Name) anAttr;
      if(aBuilder->NamedShape()->Label().FindAttribute(TDataStd_Name::GetID(),anAttr)) {
        std::string aName (TCollection_AsciiString(anAttr->Get()).ToCString());
        if(!aName.empty()) {
          std::shared_ptr<Model_Document> aDoc =
            std::dynamic_pointer_cast<Model_Document>(document());
          aDoc->addNamingName(aBuilder->NamedShape()->Label(), aName);
        }
      }
    }
  }
}

void  Model_BodyBuilder::storeWithoutNaming(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    clean();
    if (!theShape.get())
      return; // bad shape
    TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
    if (aShape.IsNull())
      return;  // null shape inside
    TNaming_Builder aBuilder(aData->shapeLab());
    aBuilder.Select(aShape, aShape);
  }
}

void Model_BodyBuilder::clean()
{
  TDF_Label aLab = std::dynamic_pointer_cast<Model_Data>(data())->shapeLab();
  if (aLab.IsNull())
    return;
  std::map<int, TNaming_Builder*>::iterator aBuilder = myBuilders.begin();
  for(; aBuilder != myBuilders.end(); aBuilder++) {
    delete aBuilder->second;
    // clear also shapes on cleaned sub-labels (#2241)
    Handle(TNaming_NamedShape) aNS;
    if (aLab.FindChild(aBuilder->first).FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
      aNS->Clear();
    }
  }
  myBuilders.clear();
  // remove the old reference (if any)
  aLab.ForgetAttribute(TDF_Reference::GetID());
  myDividedIndex = 1;
  myVIndex = 1;
  myEIndex = 1;
  myFIndex = 1;
}

Model_BodyBuilder::~Model_BodyBuilder()
{
  clean();
}

void Model_BodyBuilder::buildName(const int theTag, const std::string& theName)
{
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(document());
  //aDoc->addNamingName(builder(theTag)->NamedShape()->Label(), theName);
  TDataStd_Name::Set(builder(theTag)->NamedShape()->Label(), theName.c_str());
}
void Model_BodyBuilder::generated(
  const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName, const int theTag)
{
  TopoDS_Shape aShape = theNewShape->impl<TopoDS_Shape>();
  builder(theTag)->Generated(aShape);
  if(!theName.empty())
    buildName(theTag, theName);
}

void Model_BodyBuilder::generated(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
  const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName, const int theTag)
{
  TopoDS_Shape anOldShape = theOldShape->impl<TopoDS_Shape>();
  TopoDS_Shape aNewShape = theNewShape->impl<TopoDS_Shape>();
  builder(theTag)->Generated(anOldShape, aNewShape);
  if(!theName.empty())
    buildName(theTag, theName);
  TopAbs_ShapeEnum aGenShapeType = aNewShape.ShapeType();
  if(aGenShapeType == TopAbs_WIRE || aGenShapeType == TopAbs_SHELL) {
    TopAbs_ShapeEnum anExplodeShapeType = aGenShapeType == TopAbs_WIRE ? TopAbs_EDGE : TopAbs_FACE;
    const TDF_Label aLabel = builder(theTag)->NamedShape()->Label();
    int aTag = 1;
    std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(document());
    for(TopExp_Explorer anExp(aNewShape, anExplodeShapeType); anExp.More(); anExp.Next()) {
      TDF_Label aChildLabel = aLabel.FindChild(aTag);
      TNaming_Builder aBuilder(aChildLabel);
      aBuilder.Generated(anOldShape, anExp.Current());
      TCollection_AsciiString aChildName = TCollection_AsciiString((theName + "_").c_str()) + aTag;
      //aDoc->addNamingName(aChildLabel, aChildName.ToCString());
      TDataStd_Name::Set(aChildLabel, aChildName.ToCString());
      aTag++;
    }
  }
}


void Model_BodyBuilder::modified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
  const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName, const int theTag)
{
  TopoDS_Shape anOldShape = theOldShape->impl<TopoDS_Shape>();
  TopoDS_Shape aNewShape = theNewShape->impl<TopoDS_Shape>();
  builder(theTag)->Modify(anOldShape, aNewShape);
  if(!theName.empty())
    buildName(theTag, theName);
}

void Model_BodyBuilder::deleted(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
  const int theTag)
{
  TopoDS_Shape aShape = theOldShape->impl<TopoDS_Shape>();
  builder(theTag)->Delete(aShape);
}

void Model_BodyBuilder::loadDeletedShapes (GeomAlgoAPI_MakeShape* theMS,
  std::shared_ptr<GeomAPI_Shape>  theShapeIn,
  const int  theKindOfShape,
  const int  theTag,
  const GeomShapePtr theShapes)
{
  TopoDS_Shape aShapeIn = theShapeIn->impl<TopoDS_Shape>();
  TopTools_MapOfShape aView;
  TopExp_Explorer ShapeExplorer (aShapeIn, (TopAbs_ShapeEnum)theKindOfShape);
  GeomShapePtr aResultShape = shape();
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& aRoot = ShapeExplorer.Current ();
    if (!aView.Add(aRoot)) continue;
    std::shared_ptr<GeomAPI_Shape> aRShape(new GeomAPI_Shape());
    aRShape->setImpl((new TopoDS_Shape(aRoot)));
    if (!theMS->isDeleted(aRShape)
        || aResultShape->isSubShape(aRShape, false)
        || (theShapes.get() && theShapes->isSubShape(aRShape, false))) {
      continue;
    }

    ListOfShape aHist;
    if (BRepTools_History::IsSupportedType(aRoot)) // to avoid crash in #2572
      theMS->modified(aRShape, aHist);
    if (aHist.size() == 0 || (aHist.size() == 1 && aHist.front()->isSame(aRShape)))
      builder(theTag)->Delete(aRoot);
  }
}

static void removeBadShapes(ListOfShape& theShapes)
{
  ListOfShape::iterator anIt = theShapes.begin();
  while (anIt != theShapes.end()) {
    TopoDS_Shape aNewShape = (*anIt)->impl<TopoDS_Shape>();
    bool aSkip = aNewShape.IsNull()
      || (aNewShape.ShapeType() == TopAbs_EDGE && BRep_Tool::Degenerated(TopoDS::Edge(aNewShape)));
    if (aSkip) {
      ListOfShape::iterator aRemoveIt = anIt++;
      theShapes.erase(aRemoveIt);
    } else {
      ++anIt;
    }
  }
}

// Keep only the shapes with minimal shape type
static void keepTopLevelShapes(ListOfShape& theShapes, const TopoDS_Shape& theRoot,
  const GeomShapePtr& theResultShape = GeomShapePtr())
{
  GeomAPI_Shape::ShapeType aKeepShapeType = GeomAPI_Shape::SHAPE;
  ListOfShape::iterator anIt = theShapes.begin();
  while (anIt != theShapes.end()) {
    TopoDS_Shape aNewShape = (*anIt)->impl<TopoDS_Shape>();
    bool aSkip = aNewShape.IsNull() ||
      (aNewShape.ShapeType() == TopAbs_EDGE && BRep_Tool::Degenerated(TopoDS::Edge(aNewShape)));
    if (aSkip || theRoot.IsSame(aNewShape) || (theResultShape &&
        (!theResultShape->isSubShape(*anIt, false) || theResultShape->isSame(*anIt)))) {
      ListOfShape::iterator aRemoveIt = anIt++;
      theShapes.erase(aRemoveIt);
    } else {
      GeomAPI_Shape::ShapeType aType = (*anIt)->shapeType();
      if (aType < aKeepShapeType) {
        // found a shape with lesser shape type => remove all previous shapes
        aKeepShapeType = aType;
        theShapes.erase(theShapes.begin(), anIt);
        ++anIt;
      } else if (aType > aKeepShapeType) {
        // shapes with greater shape type should be removed from the list
        ListOfShape::iterator aRemoveIt = anIt++;
        theShapes.erase(aRemoveIt);
      } else
        ++anIt;
    }
  }
}

// returns an ancestor shape-type thaty used for naming-definition of the sub-type
TopAbs_ShapeEnum typeOfAncestor(const TopAbs_ShapeEnum theSubType) {
  if (theSubType == TopAbs_VERTEX)
    return TopAbs_EDGE;
  if (theSubType == TopAbs_EDGE)
    return TopAbs_FACE;
  return TopAbs_VERTEX; // bad case
}

void Model_BodyBuilder::loadAndOrientModifiedShapes (
  GeomAlgoAPI_MakeShape* theMS,
  std::shared_ptr<GeomAPI_Shape>  theShapeIn,
  const int  theKindOfShape,
  const int  theTag,
  const std::string& theName,
  GeomAPI_DataMapOfShapeShape& theSubShapes,
  const bool theIsStoreSeparate,
  const bool theIsStoreAsGenerated)
{
  static const int THE_ANCHOR_TAG = 100000;

  int anIndex = 1;
  int aTag = theTag;
  bool isBuilt = !theName.empty();
  std::string aName = theName;
  std::ostringstream aStream;
  GeomShapePtr aResultShape = shape();
  TopoDS_Shape aShapeIn = theShapeIn->impl<TopoDS_Shape>();
  TopTools_MapOfShape aView;
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  TopExp_Explorer aShapeExplorer (aShapeIn, (TopAbs_ShapeEnum)theKindOfShape);
  for (; aShapeExplorer.More(); aShapeExplorer.Next ()) {
    const TopoDS_Shape& aRoot = aShapeExplorer.Current ();
    if (!aView.Add(aRoot)) continue;

    bool aNotInTree =
      TNaming_Tool::NamedShape(aRoot, aData->shapeLab()).IsNull();
    if (aNotInTree && !theIsStoreSeparate) {
      // there is no sense to write history if old shape does not exist in the document
      continue; // but if it is stored separately, it will be builded as a primitive
    }
    ListOfShape aList;
    std::shared_ptr<GeomAPI_Shape> aRShape(new GeomAPI_Shape());
    aRShape->setImpl((new TopoDS_Shape(aRoot)));
    theMS->modified(aRShape, aList);
    if (!theIsStoreSeparate) {
      //keepTopLevelShapes(aList, aRoot, aResultShape);
      removeBadShapes(aList);
    }
    // sort the list of images before naming
    GeomAlgoAPI_SortListOfShapes::sort(aList);

    // to trace situation where several objects are produced by one parent (#2317)
    int aSameParentShapes = (aShapeIn.ShapeType() == TopAbs_WIRE
                             || aShapeIn.ShapeType() == TopAbs_SHELL
                             || aShapeIn.ShapeType() == TopAbs_COMPOUND) ? 0 : -1;
    std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator
      anIt = aList.begin(), aLast = aList.end();
    for (; anIt != aLast; anIt++) {
      TopoDS_Shape aNewShape = (*anIt)->impl<TopoDS_Shape>();
      if (theSubShapes.isBound(*anIt)) {
        std::shared_ptr<GeomAPI_Shape> aMapShape(theSubShapes.find(*anIt));
        aNewShape.Orientation(aMapShape->impl<TopoDS_Shape>().Orientation());
      }
      isBuilt = !theName.empty();
      if(!aRoot.IsSame(aNewShape)
         && aResultShape->isSubShape((*anIt), false)
         && !aResultShape->isSame(*anIt)) // to avoid put of same shape on main label and sub
      {
        if (!theIsStoreSeparate) {
          aSameParentShapes++;
        } else if (aNotInTree) // check this new shape can not be represented as
                              // a sub-shape of higher level sub-shapes
        {
          TopAbs_ShapeEnum aNewType = aNewShape.ShapeType();
          TopAbs_ShapeEnum anAncestorType = typeOfAncestor(aNewType);
          if (anAncestorType != TopAbs_VERTEX) {
            bool aFound = false;
            TopoDS_Shape aResultTShape = aResultShape->impl<TopoDS_Shape>();
            TopExp_Explorer anAncestorExp(aResultTShape, anAncestorType);
            for(; anAncestorExp.More() && !aFound; anAncestorExp.Next()) {
              if (aResultTShape.IsSame(anAncestorExp.Current()))
                continue;
              TopExp_Explorer aSubExp(anAncestorExp.Current(), aNewType);
              for(; aSubExp.More(); aSubExp.Next()) {
                if (aNewShape.IsSame(aSubExp.Current())) {
                  aFound = true;
                  break;
                }
              }
            }
            if (aFound) {
              continue; // not need to store this shape in the BRep structure
            }
          }
        }

        int aFoundTag = 0;
        bool isFoundSameOld = false;
        bool isFoundDiffOld = false;

        // Check if new shape was already stored.
        for (std::map<int, TNaming_Builder*>::iterator aBuildersIt = myBuilders.begin();
             aBuildersIt != myBuilders.end();
             ++aBuildersIt)
        {
          TNaming_Builder* aBuilder = aBuildersIt->second;
          for (TNaming_Iterator aNamingIt(aBuilder->NamedShape());
               aNamingIt.More();
               aNamingIt.Next())
          {
            if (aNamingIt.NewShape().IsSame(aNewShape))
            {
              aNamingIt.OldShape().IsSame(aRoot) ? isFoundSameOld = true
                                                 : isFoundDiffOld = true;
              aFoundTag = aBuildersIt->first;
            }
          }

          if (isFoundSameOld || isFoundDiffOld) break;
        }

        if (isFoundSameOld) {
          // Builder already contains same old->new shapes, don't store it twice.
          continue;
        }

        int aBuilderTag = aSameParentShapes > 0 ? THE_ANCHOR_TAG : aTag;

        int aCurShapeType = (int)((*anIt)->shapeType());
        bool needSuffix = false; // suffix for the name based on the shape type
        if (aCurShapeType != theKindOfShape) {
          // modified shape has different type => set another tag
          // to avoid shapes of different types on the same label
          aBuilderTag = THE_ANCHOR_TAG;
          needSuffix = true;
        }
        std::string aSuffix;
        if (needSuffix) {
          switch (aCurShapeType) {
            case GeomAPI_Shape::VERTEX: aSuffix = "_v_" + std::to_string(myVIndex++); break;
            case GeomAPI_Shape::EDGE:   aSuffix = "_e_" + std::to_string(myEIndex++); break;
            case GeomAPI_Shape::FACE:   aSuffix = "_f_" + std::to_string(myFIndex++); break;
            default: break;
          }
        }

        std::vector<std::pair<TopoDS_Shape, TopoDS_Shape>> aKeepShapes, aMoveShapes;
        if (isFoundDiffOld) {
          // Found same new shape with different old shape.
          if (aFoundTag >= THE_ANCHOR_TAG) {
            // Found on separated tag.
            aBuilderTag = aFoundTag; // Store it on the same tag.
            isBuilt = false; // Don't change name;
          } else {
            // Found on previous tag.
            if (aBuilderTag < THE_ANCHOR_TAG) {
              // New shape shouls not be separated.
              aBuilderTag = aFoundTag; // Store it on the same tag.
              isBuilt = false; // Don't change name;
            } else {
              // New shape should be separated from others. Move shapes from found tag to new tag.
              while (myBuilders.find(aBuilderTag) != myBuilders.end()) {
                ++aBuilderTag;
              }

              TNaming_Builder* aFoundBuilder = myBuilders.at(aFoundTag);
              Handle(TNaming_NamedShape) aFoundNamedShape = aFoundBuilder->NamedShape();
              TDF_Label aFoundLabel = aFoundNamedShape->Label();
              TNaming_Evolution anEvolution = aFoundNamedShape->Evolution();
              for (TNaming_Iterator aNamingIt(aFoundNamedShape);
                   aNamingIt.More();
                   aNamingIt.Next())
              {
                std::pair<TopoDS_Shape, TopoDS_Shape> aShapesPair =
                  std::make_pair(aNamingIt.OldShape(), aNamingIt.NewShape());
                aNamingIt.NewShape().IsSame(aNewShape) ? aMoveShapes.push_back(aShapesPair)
                                                       : aKeepShapes.push_back(aShapesPair);
              }

              aFoundNamedShape->Clear();
              for (std::vector<std::pair<TopoDS_Shape, TopoDS_Shape>>::iterator aKeepIt =
                     aKeepShapes.begin();
                   aKeepIt != aKeepShapes.end();
                   ++aKeepIt)
              {
                if (anEvolution == TNaming_GENERATED) {
                  aFoundBuilder->Generated(aKeepIt->first, aKeepIt->second);
                } else {
                  aFoundBuilder->Modify(aKeepIt->first, aKeepIt->second);
                }
              }
            }
          }
        } else if (aBuilderTag == THE_ANCHOR_TAG) {
          while (myBuilders.find(aBuilderTag) != myBuilders.end()) {
            ++aBuilderTag;
          }
        }

        if(theIsStoreAsGenerated) {
          // Here we store shapes as generated, to avoid problem when one parent shape produce
          // several child shapes. In this case naming could not determine which shape to select.
          builder(aBuilderTag)->Generated(aRoot, aNewShape);
          for (std::vector<std::pair<TopoDS_Shape, TopoDS_Shape>>::iterator aMoveIt =
               aMoveShapes.begin();
               aMoveIt != aMoveShapes.end();
               ++aMoveIt)
          {
            builder(aBuilderTag)->Generated(aMoveIt->first, aMoveIt->second);
          }
        } else if (aNotInTree) {
          // not in tree -> store as primitive (stored as separated)
          builder(aBuilderTag)->Generated(aNewShape);
        } else if (aCurShapeType != theKindOfShape) {
           // if different shape type is produced, make it as generated
          builder(aBuilderTag)->Generated(aRoot, aNewShape);
        } else {
          builder(aBuilderTag)->Modify(aRoot, aNewShape);
          for (std::vector<std::pair<TopoDS_Shape, TopoDS_Shape>>::iterator aMoveIt =
               aMoveShapes.begin();
               aMoveIt != aMoveShapes.end();
               ++aMoveIt) {
            builder(aBuilderTag)->Modify(aMoveIt->first, aMoveIt->second);
          }
        }
        if(isBuilt) {
          aStream.str(std::string());
          aStream.clear();
          aStream << theName;
          if (theIsStoreSeparate && !isFoundDiffOld)
             aStream << "_" << anIndex++;

          if (aSameParentShapes > 0) {
            aStream.str(std::string());
            aStream.clear();
            aStream << aName << "_" << "divided" << "_" << myDividedIndex++;
          }

          aStream << aSuffix;
          buildName(aBuilderTag, aStream.str());
        }
        if(theIsStoreSeparate && !isFoundDiffOld) {
          aTag++;
        }
      } else if (aResultShape->isSame(*anIt)) {
        // keep the modification evolution on the root level (2241 - history propagation issue)
        TNaming_Builder* aBuilder = builder(0);
        TDF_Label aShapeLab = aBuilder->NamedShape()->Label();
        Handle(TDF_Reference) aRef;
        // Store only in case if it does not have reference.
        if (!aShapeLab.FindAttribute(TDF_Reference::GetID(), aRef)) {
          if (theIsStoreAsGenerated) {
            TNaming_Builder* aBuilder = builder(0);
            if (!aBuilder->NamedShape().IsNull() &&
                aBuilder->NamedShape()->Evolution() != TNaming_GENERATED) {
              myBuilders.erase(0); // clear old builder to avoid different evolutions crash
            }
            builder(0)->Generated(aRoot, aNewShape);
          } else {
            TNaming_Builder* aBuilder = builder(0);
            if (!aBuilder->NamedShape().IsNull() &&
              aBuilder->NamedShape()->Evolution() != TNaming_MODIFY) {
              myBuilders.erase(0); // clear old builder to avoid different evolutions crash
            }
            builder(0)->Modify(aRoot, aNewShape);
          }
        }
      }
    }
  }
}

void Model_BodyBuilder::loadAndOrientGeneratedShapes (
  GeomAlgoAPI_MakeShape* theMS,
  std::shared_ptr<GeomAPI_Shape>  theShapeIn,
  const int  theKindOfShape,
  const int  theTag,
  const std::string& theName,
  GeomAPI_DataMapOfShapeShape& theSubShapes)
{
  TopoDS_Shape aShapeIn = theShapeIn->impl<TopoDS_Shape>();
  TopTools_MapOfShape aView;
  bool isBuilt = !theName.empty();
  TopExp_Explorer aShapeExplorer (aShapeIn, (TopAbs_ShapeEnum)theKindOfShape);
  for (; aShapeExplorer.More(); aShapeExplorer.Next ()) {
    const TopoDS_Shape& aRoot = aShapeExplorer.Current ();
    if (!aView.Add(aRoot)) continue;
    //if (TNaming_Tool::NamedShape(aRoot, builder(theTag)->NamedShape()->Label()).IsNull())
    //  continue; // there is no sense to write history if old shape does not exist in the document
    ListOfShape aList;
    std::shared_ptr<GeomAPI_Shape> aRShape(new GeomAPI_Shape());
    aRShape->setImpl((new TopoDS_Shape(aRoot)));
    theMS->generated(aRShape, aList);
    keepTopLevelShapes(aList, aRoot);
    std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator
      anIt = aList.begin(), aLast = aList.end();
    for (; anIt != aLast; anIt++) {
      TopoDS_Shape aNewShape = (*anIt)->impl<TopoDS_Shape>();
      if (theSubShapes.isBound(*anIt)) {
        std::shared_ptr<GeomAPI_Shape> aMapShape(theSubShapes.find(*anIt));
        aNewShape.Orientation(aMapShape->impl<TopoDS_Shape>().Orientation());
      }
      if (!aRoot.IsSame (aNewShape)) {
        builder(theTag)->Generated(aRoot,aNewShape);
        if(isBuilt)
          buildName(theTag, theName);
      }
      TopAbs_ShapeEnum aGenShapeType = aNewShape.ShapeType();
      if(aGenShapeType == TopAbs_WIRE || aGenShapeType == TopAbs_SHELL) {
        TopAbs_ShapeEnum anExplodeShapeType =
          aGenShapeType == TopAbs_WIRE ? TopAbs_EDGE : TopAbs_FACE;
        const TDF_Label aLabel = builder(theTag)->NamedShape()->Label();
        int aTag = 1;
        std::shared_ptr<Model_Document> aDoc =
          std::dynamic_pointer_cast<Model_Document>(document());
        for(TopExp_Explorer anExp(aNewShape, anExplodeShapeType); anExp.More(); anExp.Next()) {
          TDF_Label aChildLabel = aLabel.FindChild(aTag);
          TNaming_Builder aBuilder(aChildLabel);
          aBuilder.Generated(aRoot, anExp.Current());
          TCollection_AsciiString aChildName =
            TCollection_AsciiString((theName + "_").c_str()) + aTag;
          TDataStd_Name::Set(aChildLabel, aChildName.ToCString());
          aTag++;
        }
      }
    }
  }
}

//=======================================================================
int getDangleShapes(const TopoDS_Shape&           theShapeIn,
  const TopAbs_ShapeEnum        theGeneratedFrom,
  TopTools_DataMapOfShapeShape& theDangles)
{
  theDangles.Clear();
  TopTools_IndexedDataMapOfShapeListOfShape subShapeAndAncestors;
  TopAbs_ShapeEnum GeneratedTo;
  if (theGeneratedFrom == TopAbs_FACE) GeneratedTo = TopAbs_EDGE;
  else if (theGeneratedFrom == TopAbs_EDGE) GeneratedTo = TopAbs_VERTEX;
  else return Standard_False;
  TopExp::MapShapesAndAncestors(theShapeIn, GeneratedTo, theGeneratedFrom, subShapeAndAncestors);
  for (Standard_Integer i = 1; i <= subShapeAndAncestors.Extent(); i++) {
    const TopoDS_Shape& mayBeDangle = subShapeAndAncestors.FindKey(i);
    const TopTools_ListOfShape& ancestors = subShapeAndAncestors.FindFromIndex(i);
    if (ancestors.Extent() == 1) theDangles.Bind(ancestors.First(), mayBeDangle);
  }
  return theDangles.Extent();
}

//=======================================================================
void loadGeneratedDangleShapes(
  const TopoDS_Shape&      theShapeIn,
  const TopAbs_ShapeEnum   theGeneratedFrom,
  TNaming_Builder *        theBuilder)
{
  TopTools_DataMapOfShapeShape dangles;
  if (!getDangleShapes(theShapeIn, theGeneratedFrom, dangles)) return;
  TopTools_DataMapIteratorOfDataMapOfShapeShape itr(dangles);
  for (; itr.More(); itr.Next())
    theBuilder->Generated(itr.Key(), itr.Value());
}

//=======================================================================
void Model_BodyBuilder::loadNextLevels(std::shared_ptr<GeomAPI_Shape> theShape,
  const std::string& theName, int&  theTag)
{
  if(theShape->isNull()) return;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  std::string aName;
  if (aShape.ShapeType() == TopAbs_SOLID) {
    TopExp_Explorer expl(aShape, TopAbs_FACE);
    for (; expl.More(); expl.Next()) {
      builder(theTag)->Generated(expl.Current());
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
      theTag++;
    }
  }
  else if (aShape.ShapeType() == TopAbs_SHELL || aShape.ShapeType() == TopAbs_FACE) {
    // load faces and all the free edges
    TopTools_IndexedMapOfShape Faces;
    TopExp::MapShapes(aShape, TopAbs_FACE, Faces);
    if (Faces.Extent() > 1 || (aShape.ShapeType() == TopAbs_SHELL && Faces.Extent() == 1)) {
      TopExp_Explorer expl(aShape, TopAbs_FACE);
      for (; expl.More(); expl.Next()) {
        builder(theTag)->Generated(expl.Current());
        TCollection_AsciiString aStr(theTag);
        aName = theName + aStr.ToCString();
        buildName(theTag, aName);
        theTag++;
      }
    }
    TopTools_IndexedDataMapOfShapeListOfShape anEdgeAndNeighbourFaces;
    TopExp::MapShapesAndAncestors(aShape, TopAbs_EDGE, TopAbs_FACE, anEdgeAndNeighbourFaces);
    for (Standard_Integer i = 1; i <= anEdgeAndNeighbourFaces.Extent(); i++)
    {
      const TopTools_ListOfShape& aLL = anEdgeAndNeighbourFaces.FindFromIndex(i);
      if (aLL.Extent() < 2) {
        if (BRep_Tool::Degenerated(TopoDS::Edge(anEdgeAndNeighbourFaces.FindKey(i))))
          continue;
        builder(theTag)->Generated(anEdgeAndNeighbourFaces.FindKey(i));
        TCollection_AsciiString aStr(theTag);
        aName = theName + aStr.ToCString();
        buildName(theTag, aName);
        theTag++;
      } else {
        TopTools_ListIteratorOfListOfShape anIter(aLL);
        const TopoDS_Face& aFace = TopoDS::Face(anIter.Value());
        anIter.Next();
        if(aFace.IsEqual(anIter.Value())) {
          builder(theTag)->Generated(anEdgeAndNeighbourFaces.FindKey(i));
          TCollection_AsciiString aStr(theTag);
          aName = theName + aStr.ToCString();
          buildName(theTag, aName);
          theTag++;
        }
      }
    }
  } else if (aShape.ShapeType() == TopAbs_WIRE) {
    TopTools_IndexedMapOfShape Edges;
    BRepTools::Map3DEdges(aShape, Edges);
    if (Edges.Extent() == 1) {
      builder(theTag++)->Generated(Edges.FindKey(1));
      TopExp_Explorer expl(aShape, TopAbs_VERTEX);
      for (; expl.More(); expl.Next()) {
        builder(theTag)->Generated(expl.Current());
        TCollection_AsciiString aStr(theTag);
        aName = theName + aStr.ToCString();
        buildName(theTag, aName);
        theTag++;
      }
    } else {
      TopExp_Explorer expl(aShape, TopAbs_EDGE);
      for (; expl.More(); expl.Next()) {
        builder(theTag)->Generated(expl.Current());
        TCollection_AsciiString aStr(theTag);
        aName = theName + aStr.ToCString();
        buildName(theTag, aName);
        theTag++;
      }
      // and load generated vertices.
      TopTools_DataMapOfShapeShape generated;
      if (getDangleShapes(aShape, TopAbs_EDGE, generated))
      {
        TNaming_Builder* pBuilder = builder(theTag++);
        loadGeneratedDangleShapes(aShape, TopAbs_EDGE, pBuilder);
      }
    }
  } else if (aShape.ShapeType() == TopAbs_EDGE) {
    TopExp_Explorer expl(aShape, TopAbs_VERTEX);
    for (; expl.More(); expl.Next()) {
      builder(theTag)->Generated(expl.Current());
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
      theTag++;
    }
  }
}

//=======================================================================
int findAmbiguities(const TopoDS_Shape&           theShapeIn,
  TopTools_ListOfShape&   theList)
{
  theList.Clear();
  // edges -> ancestor faces list
  TopTools_IndexedDataMapOfShapeListOfShape aSubShapeAndAncestors;
  TopExp::MapShapesAndAncestors(theShapeIn, TopAbs_EDGE, TopAbs_FACE, aSubShapeAndAncestors);
  // keeps the shapes which are already in the resulting list
  TopTools_MapOfShape alreadyThere;
  // map from faces identifier (combination of hash-codes) to list of edges produced such ID
  NCollection_DataMap<int, NCollection_List<TopoDS_Shape> > aFacesIDs;

  TopTools_IndexedDataMapOfShapeListOfShape::Iterator anAncestorsIter(aSubShapeAndAncestors);
  for (; anAncestorsIter.More(); anAncestorsIter.Next()) {
    const TopTools_ListOfShape& ancestors = anAncestorsIter.Value();
    if (ancestors.Extent() < 2)
      continue;
    Standard_Integer anID = 0;
    for(TopTools_ListIteratorOfListOfShape aFaceIt(ancestors); aFaceIt.More(); aFaceIt.Next()) {
      anID ^= HashCode(aFaceIt.ChangeValue(), 1990657); // Pierpont prime
    }
    if (aFacesIDs.IsBound(anID)) { // there found same edge, check they really have same faces
      const NCollection_List<TopoDS_Shape>& aSameFaces1 =
        aSubShapeAndAncestors.FindFromKey(anAncestorsIter.Key());
      NCollection_List<TopoDS_Shape>::Iterator aSameEdge(aFacesIDs.ChangeFind(anID));
      for(; aSameEdge.More(); aSameEdge.Next()) {
        const NCollection_List<TopoDS_Shape>& aSameFaces2 =
          aSubShapeAndAncestors.FindFromKey(aSameEdge.Value());
        if (aSameFaces2.Extent() != aSameFaces1.Extent()) // the number of faces is different
          break;

        NCollection_List<TopoDS_Shape>::Iterator aFaceIter1(aSameFaces1);
        for(; aFaceIter1.More(); aFaceIter1.Next()) {
          NCollection_List<TopoDS_Shape>::Iterator aFaceIter2(aSameFaces2);
          for(; aFaceIter2.More(); aFaceIter2.Next()) {
            if (aFaceIter1.Value().IsSame(aFaceIter2.Value()))
              break;
          }
          if (!aFaceIter2.More()) // aFaceIter1 contains a face, which is not in aFaceIter2
            break;
        }
        if (!aFaceIter1.More()) { // all the faces are same => put to the result
          if (alreadyThere.Add(aSameEdge.Value()))
            theList.Append(aSameEdge.Value());
          if (alreadyThere.Add(anAncestorsIter.Key()))
            theList.Append(anAncestorsIter.Key());
        }
      }
    } else { // ID is unique, just add this edge
      aFacesIDs.Bind(anID, NCollection_List<TopoDS_Shape>());
    }
    aFacesIDs.ChangeFind(anID).Append(anAncestorsIter.Key()); // add to the list anyway
  }
  return theList.Extent();
}

//=======================================================================
void Model_BodyBuilder::loadFirstLevel(
  std::shared_ptr<GeomAPI_Shape> theShape, const std::string& theName, int&  theTag)
{
  if(theShape->isNull()) return;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  std::string aName;
  if (aShape.ShapeType() == TopAbs_COMPOUND || aShape.ShapeType() == TopAbs_COMPSOLID) {
    TopoDS_Iterator itr(aShape);
    for (; itr.More(); itr.Next()) {
      builder(theTag)->Generated(itr.Value());
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
      if(!theName.empty()) buildName(theTag, aName);
      theTag++;
      if (itr.Value().ShapeType() == TopAbs_COMPOUND ||
        itr.Value().ShapeType() == TopAbs_COMPSOLID)
      {
        std::shared_ptr<GeomAPI_Shape> itrShape(new GeomAPI_Shape());
        itrShape->setImpl(new TopoDS_Shape(itr.Value()));
        loadFirstLevel(itrShape, theName, theTag);
      } else {
        std::shared_ptr<GeomAPI_Shape> itrShape(new GeomAPI_Shape());
        itrShape->setImpl(new TopoDS_Shape(itr.Value()));
        loadNextLevels(itrShape, theName, theTag);
      }
    }
  } else {
    std::shared_ptr<GeomAPI_Shape> itrShape(new GeomAPI_Shape());
    itrShape->setImpl(new TopoDS_Shape(aShape));
    loadNextLevels(itrShape, theName, theTag);
  }
  TopTools_ListOfShape   aList;
  if(findAmbiguities(aShape, aList)) {
    TopTools_ListIteratorOfListOfShape it(aList);
    for (; it.More(); it.Next(),theTag++) {
      builder(theTag)->Generated(it.Value());
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
    }
  }
}

//=======================================================================
void Model_BodyBuilder::loadDisconnectedEdges(
  std::shared_ptr<GeomAPI_Shape> theShape, const std::string& theName, int&  theTag)
{
  if(theShape->isNull()) return;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  TopTools_DataMapOfShapeListOfShape edgeNaborFaces;
  TopTools_ListOfShape empty;
  TopExp_Explorer explF(aShape, TopAbs_FACE);
  for (; explF.More(); explF.Next()) {
    const TopoDS_Shape& aFace = explF.Current();
    TopExp_Explorer explV(aFace, TopAbs_EDGE);
    for (; explV.More(); explV.Next()) {
      const TopoDS_Shape& anEdge = explV.Current();
      if (!edgeNaborFaces.IsBound(anEdge)) edgeNaborFaces.Bind(anEdge, empty);
      Standard_Boolean faceIsNew = Standard_True;
      TopTools_ListIteratorOfListOfShape itrF(edgeNaborFaces.Find(anEdge));
      for (; itrF.More(); itrF.Next()) {
        if (itrF.Value().IsSame(aFace)) {
          faceIsNew = Standard_False;
          break;
        }
      }
      if (faceIsNew)
        edgeNaborFaces.ChangeFind(anEdge).Append(aFace);
    }
  }

  TopTools_MapOfShape anEdgesToDelete;
  TopExp_Explorer anEx(aShape,TopAbs_EDGE);
  std::string aName;
  for(;anEx.More();anEx.Next()) {
    Standard_Boolean aC0 = Standard_False;
    TopoDS_Shape anEdge1 = anEx.Current();
    if (edgeNaborFaces.IsBound(anEdge1)) {
      const TopTools_ListOfShape& aList1 = edgeNaborFaces.Find(anEdge1);
      if (aList1.Extent()<2) continue;
      TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itr(edgeNaborFaces);
      for (; itr.More(); itr.Next()) {
        TopoDS_Shape anEdge2 = itr.Key();
        if(anEdgesToDelete.Contains(anEdge2)) continue;
        if (anEdge1.IsSame(anEdge2)) continue;
        const TopTools_ListOfShape& aList2 = itr.Value();
        // compare lists of the neighbour faces of edge1 and edge2
        if (aList1.Extent() == aList2.Extent()) {
          Standard_Integer aMatches = 0;
          for(TopTools_ListIteratorOfListOfShape aLIter1(aList1);aLIter1.More();aLIter1.Next())
            for(TopTools_ListIteratorOfListOfShape aLIter2(aList2);aLIter2.More();aLIter2.Next())
              if (aLIter1.Value().IsSame(aLIter2.Value())) aMatches++;
          if (aMatches == aList1.Extent()) {
            aC0=Standard_True;
            builder(theTag)->Generated(anEdge2);
            anEdgesToDelete.Add(anEdge2);
            TCollection_AsciiString aStr(theTag);
            aName = theName + aStr.ToCString();
            buildName(theTag, aName);
            theTag++;
          }
        }
      }
      TopTools_MapIteratorOfMapOfShape itDelete(anEdgesToDelete);
      for(;itDelete.More();itDelete.Next())
        edgeNaborFaces.UnBind(itDelete.Key());
      edgeNaborFaces.UnBind(anEdge1);
    }
    if (aC0) {
      builder(theTag)->Generated(anEdge1);
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
      theTag++;
    }
  }
}

void Model_BodyBuilder::loadDisconnectedVertexes(std::shared_ptr<GeomAPI_Shape> theShape,
                                                 const std::string& theName, int&  theTag)
{
  if(theShape->isNull()) return;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  TopTools_DataMapOfShapeListOfShape vertexNaborEdges;
  TopTools_ListOfShape empty;
  TopExp_Explorer explF(aShape, TopAbs_EDGE);
  for (; explF.More(); explF.Next()) {
    const TopoDS_Shape& anEdge = explF.Current();
    TopExp_Explorer explV(anEdge, TopAbs_VERTEX);
    for (; explV.More(); explV.Next()) {
      const TopoDS_Shape& aVertex = explV.Current();
      if (!vertexNaborEdges.IsBound(aVertex)) vertexNaborEdges.Bind(aVertex, empty);
      Standard_Boolean faceIsNew = Standard_True;
      TopTools_ListIteratorOfListOfShape itrF(vertexNaborEdges.Find(aVertex));
      for (; itrF.More(); itrF.Next()) {
        if (itrF.Value().IsSame(anEdge)) {
          faceIsNew = Standard_False;
          break;
        }
      }
      if (faceIsNew) {
        vertexNaborEdges.ChangeFind(aVertex).Append(anEdge);
      }
    }
  }
  std::string aName;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itr(vertexNaborEdges);
  for (; itr.More(); itr.Next()) {
    const TopTools_ListOfShape& naborEdges = itr.Value();
    if (naborEdges.Extent() < 2) {
      builder(theTag)->Generated(itr.Key());
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
      theTag++;
    }
  }
}

std::shared_ptr<GeomAPI_Shape> Model_BodyBuilder::shape()
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData && aData->isValid()) {
    TDF_Label aShapeLab = aData->shapeLab();
    Handle(TDF_Reference) aRef;
    if (aShapeLab.FindAttribute(TDF_Reference::GetID(), aRef)) {
      aShapeLab = aRef->Get();
    }
    Handle(TNaming_NamedShape) aName;
    if (aShapeLab.FindAttribute(TNaming_NamedShape::GetID(), aName)) {
      TopoDS_Shape aShape = aName->Get();
      if (!aShape.IsNull()) {
        std::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
        aRes->setImpl(new TopoDS_Shape(aShape));
        return aRes;
      }
    }
  }
  return std::shared_ptr<GeomAPI_Shape>();
}

bool Model_BodyBuilder::isLatestEqual(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  if (theShape.get()) {
    TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
    if (aData) {
      TDF_Label& aShapeLab = aData->shapeLab();
      Handle(TNaming_NamedShape) aName;
      if (aShapeLab.FindAttribute(TNaming_NamedShape::GetID(), aName)) {
        TopoDS_Shape aLatest = TNaming_Tool::CurrentShape(aName);
        if (aLatest.IsNull())
          return false;
        if (aLatest.IsEqual(aShape))
          return true;
        // check sub-shapes for comp-solids:
        for (TopExp_Explorer anExp(aShape, aLatest.ShapeType()); anExp.More(); anExp.Next()) {
          if (aLatest.IsEqual(anExp.Current()))
            return true;
        }
      }
    }
  }
  return false;
}
