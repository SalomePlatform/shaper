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

#include <Model_BodyBuilder.h>

#include <Model_Data.h>
#include <Model_Document.h>
#include <ModelAPI_Session.h>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_SameShapeIterator.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Reference.hxx>
#include <TDF_Tool.hxx>
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
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAlgoAPI_SortListOfShapes.h>
#include <Config_PropManager.h>
// DEB
//#include <TCollection_AsciiString.hxx>
//#define DEB_IMPORT 1

/// reference to the shape in external document: sting list attribute identifier
static const Standard_GUID kEXTERNAL_SHAPE_REF("9aa5dd14-6d34-4a8d-8786-05842fd7bbbd");

static const int INVALID_TAG            = -1;
static const int GENERATED_VERTICES_TAG = 1;
static const int GENERATED_EDGES_TAG    = 2;
static const int GENERATED_FACES_TAG    = 3;
static const int MODIFIED_VERTICES_TAG  = 4;
static const int MODIFIED_EDGES_TAG     = 5;
static const int MODIFIED_FACES_TAG     = 6;
static const int DELETED_TAG            = 7;
static const int PRIMITIVES_START_TAG   = 11;

static int getGenerationTag(const TopoDS_Shape& theShape) {
  TopAbs_ShapeEnum aShapeType = theShape.ShapeType();
  switch (aShapeType) {
    case TopAbs_VERTEX: return GENERATED_VERTICES_TAG;
    case TopAbs_EDGE:   return GENERATED_EDGES_TAG;
    case TopAbs_FACE:   return GENERATED_FACES_TAG;
    default: break; // [to avoid compilation warning]
  }

  return INVALID_TAG;
}

static int getModificationTag(const TopoDS_Shape& theShape) {
  TopAbs_ShapeEnum aShapeType = theShape.ShapeType();
  switch (aShapeType) {
    case TopAbs_VERTEX: return MODIFIED_VERTICES_TAG;
    case TopAbs_EDGE:   return MODIFIED_EDGES_TAG;
    case TopAbs_FACE:   return MODIFIED_FACES_TAG;
    default: break; // [to avoid compilation warning]
  }

  return INVALID_TAG;
}

static bool isAlreadyStored(const TNaming_Builder* theBuilder,
                            const TopoDS_Shape& theOldShape,
                            const TopoDS_Shape& theNewShape)
{
  for (TNaming_Iterator aNamingIt(theBuilder->NamedShape());
    aNamingIt.More();
    aNamingIt.Next())
  {
    if (aNamingIt.NewShape().IsSame(theNewShape)
      && aNamingIt.OldShape().IsSame(theOldShape))
    {
      return true;
    }
  }

  return false;
}

Model_BodyBuilder::Model_BodyBuilder(ModelAPI_Object* theOwner)
: ModelAPI_BodyBuilder(theOwner),
  myFreePrimitiveTag(PRIMITIVES_START_TAG)
{
}

/// Checks that shape is presented in the tree with not-selection evolution
/// In theOriginalLabel it returns label where NS of old sub-shape is stored
static bool isShapeInTree(const TDF_Label& theAccess1, const TDF_Label& theAccess2,
  TopoDS_Shape theShape, TDF_Label& theOriginalLabel)
{
  bool aResult = TNaming_Tool::HasLabel(theAccess1, theShape);
  if (aResult) { //check evolution and a label of this shape
    for(TNaming_SameShapeIterator aShapes(theShape, theAccess1); aShapes.More(); aShapes.Next())
    {
      static Handle(TNaming_NamedShape) aNS;
      if (aShapes.Label().FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
        if (aNS->Evolution() != TNaming_SELECTED) {
          theOriginalLabel = aNS->Label();
          return true;
        }
      }
    }
  }
  if (!theAccess2.IsNull()) {
    static const TDF_Label anEmpty;
    return isShapeInTree(theAccess2, anEmpty, theShape, theOriginalLabel);
  }
  return false;
}

/// Stores entry to the external label in the entries list at this label
static void storeExternalReference(const TDF_Label& theExternal, const TDF_Label theThis)
{
  // store information about the external document reference to restore old shape on open
  if (!theExternal.IsNull() && !theExternal.Root().IsEqual(theThis.Root())) {
    Handle(TDataStd_ExtStringList) anEntries;
    if (!theThis.FindAttribute(kEXTERNAL_SHAPE_REF, anEntries)) {
      anEntries = TDataStd_ExtStringList::Set(theThis, kEXTERNAL_SHAPE_REF);
    }
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(theExternal, anEntry);
    // check it already contains this entry
    TDataStd_ListOfExtendedString::Iterator anIter(anEntries->List());
    for(; anIter.More(); anIter.Next())
      if (anIter.Value() == anEntry)
        break;
    if (!anIter.More()) {
      anEntries->Append(anEntry);
    }
  }
}

void Model_BodyBuilder::store(const GeomShapePtr& theShape,
                              const bool theIsStoreSameShapes)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label aShapeLab = aData->shapeLab();
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
      Handle(TNaming_NamedShape) aNS;
      if (TNaming_Tool::HasLabel(aShapeLab, aShape))
        aNS = TNaming_Tool::NamedShape(aShape, aShapeLab);
      // the last condition is for the issue 2751 : existing shape may be found in compound-NS
      if(!aNS.IsNull() && !aNS->IsEmpty() && aNS->Get().IsSame(aShape)) {
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

void Model_BodyBuilder::storeGenerated(const GeomShapePtr& theFromShape,
  const GeomShapePtr& theToShape, const bool theIsCleanStored)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    // clean builders
    if (theIsCleanStored)
      clean();
    TNaming_Builder* aBuilder = builder(0);
    if (!theFromShape || !theToShape)
      return;  // bad shape
    TopoDS_Shape aShapeBasis = theFromShape->impl<TopoDS_Shape>();
    if (aShapeBasis.IsNull())
      return;  // null shape inside
    TopoDS_Shape aShapeNew = theToShape->impl<TopoDS_Shape>();
    if (aShapeNew.IsNull())
      return;  // null shape inside

    // There is no sense to write history if old shape does not exist in the document.
    TDF_Label anAccess2 = std::dynamic_pointer_cast<Model_Document>(
      ModelAPI_Session::get()->moduleDocument())->generalLabel();
    TDF_Label anOriginalLabel;
    if (!isShapeInTree(aData->shapeLab(), anAccess2, aShapeBasis, anOriginalLabel)) {
      if (aBuilder->NamedShape()->Get().IsNull()) { // store as primitive if alone anyway
        aBuilder->Generated(aShapeNew);
      }
    } else {
      if (aBuilder->NamedShape()->Evolution() == TNaming_PRIMITIVE) { // erase primitive before
        myBuilders.erase(0);
        aBuilder = builder(0);
      }

      aBuilder->Generated(aShapeBasis, aShapeNew);
      // store information about the external document reference to restore old shape on open
      storeExternalReference(anOriginalLabel, aBuilder->NamedShape()->Label());
    }

    // register name
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

void Model_BodyBuilder::storeGenerated(const std::list<GeomShapePtr>& theFromShapes,
  const GeomShapePtr& theToShape, const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape)
{
  bool aStored = false;
  std::list<GeomShapePtr>::const_iterator anOldIter = theFromShapes.cbegin();
  for (; anOldIter != theFromShapes.cend(); anOldIter++) {
    bool aStore = (*anOldIter)->isCompound() || (*anOldIter)->isShell() || (*anOldIter)->isWire();
    if (!aStore) {
      ListOfShape aNews; // check this old really generates theToShape
      theMakeShape->generated(*anOldIter, aNews);
      ListOfShape::iterator aNewIter = aNews.begin();
      for (; aNewIter != aNews.end(); aNewIter++) {
        if (theToShape->isSame(*aNewIter))
          break;
      }
      aStore = aNewIter != aNews.end();
    }
    if (aStore) {
      storeGenerated(*anOldIter, theToShape, !aStored);
      TNaming_Builder* aBuilder = builder(0);
      aStored = !aBuilder->NamedShape()->IsEmpty();
    }
  }
  if (!aStored) { // store as PRIMITIVE, but clean in any way
    store(theToShape);
    return;
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

void Model_BodyBuilder::storeModified(const GeomShapePtr& theOldShape,
                                      const GeomShapePtr& theNewShape,
                                      const bool theIsCleanStored)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    // clean builders
    if (theIsCleanStored) clean();
    TNaming_Builder* aBuilder = builder(0);
    if (!theOldShape || !theNewShape)
      return;  // bad shape
    TopoDS_Shape aShapeOld = theOldShape->impl<TopoDS_Shape>();
    if (aShapeOld.IsNull())
      return;  // null shape inside
    TopoDS_Shape aShapeNew = theNewShape->impl<TopoDS_Shape>();
    if (aShapeNew.IsNull())
      return;  // null shape inside

    // There is no sense to write history if old shape does not exist in the document.
    TDF_Label anAccess2 = std::dynamic_pointer_cast<Model_Document>(
      ModelAPI_Session::get()->moduleDocument())->generalLabel();
    TDF_Label anOriginalLabel;
    if (!isShapeInTree(aData->shapeLab(), anAccess2, aShapeOld, anOriginalLabel)) {
      if (aBuilder->NamedShape()->Get().IsNull()) { // store as primitive if alone anyway
        aBuilder->Generated(aShapeNew);
      }
    } else {
      if (aBuilder->NamedShape()->Evolution() == TNaming_PRIMITIVE) { // erase primitive before
        myBuilders.erase(0);
        aBuilder = builder(0);
      }

      aBuilder->Modify(aShapeOld, aShapeNew);
      // store information about the external document reference to restore old shape on open
      storeExternalReference(anOriginalLabel, aBuilder->NamedShape()->Label());
    }

    if(!aBuilder->NamedShape()->IsEmpty()) {
      Handle(TDataStd_Name) anAttr;
      if(aBuilder->NamedShape()->Label().FindAttribute(TDataStd_Name::GetID(), anAttr)) {
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

void Model_BodyBuilder::storeModified(const std::list<GeomShapePtr>& theOldShapes,
  const GeomShapePtr& theNewShape, const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape)
{
  bool aStored = false;
  std::list<GeomShapePtr>::const_iterator anOldIter = theOldShapes.cbegin();
  for(; anOldIter != theOldShapes.cend(); anOldIter++) {
    // compounds may cause crash if call "modified"
    bool aStore = (*anOldIter)->isCompound() || (*anOldIter)->isShell() || (*anOldIter)->isWire() ||
		(*anOldIter)->isCompSolid();
    if (!aStore) {
      ListOfShape aNews; // check this old really modifies theNewShape
      theMakeShape->modified(*anOldIter, aNews);
      ListOfShape::iterator aNewIter = aNews.begin();
      for(; aNewIter != aNews.end(); aNewIter++) {
        if (theNewShape->isSame(*aNewIter))
          break;
      }
      aStore = aNewIter != aNews.end();
    }
    if (aStore) {
      storeModified(*anOldIter, theNewShape, !aStored);
      TNaming_Builder* aBuilder = builder(0);
      aStored = !aBuilder->NamedShape()->IsEmpty();
    }
  }
  if (!aStored) {
    // check the new shape is already in the tree, so, no need to store primitive, just reference
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
    if (aData.get()) {
      TDF_Label aShapeLab = aData->shapeLab();
      TopoDS_Shape aShapeNew = theNewShape->impl<TopoDS_Shape>();
      Handle(TNaming_NamedShape) aNS;
      if (TNaming_Tool::HasLabel(aShapeLab, aShapeNew))
        aNS = TNaming_Tool::NamedShape(aShapeNew, aShapeLab);
      // the last condition is for the issue 2751 : existing shape may be found in compound-NS
      if (!aNS.IsNull() && !aNS->IsEmpty() && aNS->Get().IsSame(aShapeNew)) {
        // This shape is already in document, store reference instead of shape;
        const TDF_Label aFoundLabel = aNS->Label();
        TDF_Reference::Set(aShapeLab, aFoundLabel);
        myBuilders.erase(0);
        aShapeLab.ForgetAttribute(TNaming_NamedShape::GetID());
        return;
      }
    }
    store(theNewShape); // store as PRIMITIVE, but clean in any way
    return;
  }
}

void Model_BodyBuilder::clean()
{
  TDF_Label aLab = std::dynamic_pointer_cast<Model_Data>(data())->shapeLab();
  if (aLab.IsNull())
    return;
  std::map<int, TNaming_Builder*>::iterator aBuilder = myBuilders.begin();
  for(; aBuilder != myBuilders.end(); aBuilder++) {
    Handle(TNaming_NamedShape) aNS = aBuilder->second->NamedShape();
    delete aBuilder->second;
    if (!aNS.IsNull() && !aNS->Label().IsNull())
      aNS->Label().ForgetAttribute(TNaming_NamedShape::GetID());
  }
  myBuilders.clear();
  myPrimitivesNamesIndexMap.clear();
  // remove the old reference (if any)
  aLab.ForgetAttribute(TDF_Reference::GetID());
  myFreePrimitiveTag = PRIMITIVES_START_TAG;
  TDF_ChildIDIterator anEntriesIter(aLab, kEXTERNAL_SHAPE_REF, true);
  for(; anEntriesIter.More(); anEntriesIter.Next()) {
    anEntriesIter.Value()->Label().ForgetAttribute(kEXTERNAL_SHAPE_REF);
  }
}

void Model_BodyBuilder::cleanCash()
{
  myPrimitivesNamesIndexMap.clear();
}

Model_BodyBuilder::~Model_BodyBuilder()
{
  clean();
}

void Model_BodyBuilder::buildName(const int theTag, const std::string& theName)
{
  std::string aName = theName;
  std::string aPrefix = "";
  switch (theTag) {
    case GENERATED_VERTICES_TAG: aPrefix = aName.empty() ? "Generated_Vertex" : "GV:"; break;
    case GENERATED_EDGES_TAG:    aPrefix = aName.empty() ? "Generated_Edge"   : "GE:"; break;
    case GENERATED_FACES_TAG:    aPrefix = aName.empty() ? "Generated_Face"   : "GF:"; break;
    case MODIFIED_VERTICES_TAG:  aPrefix = aName.empty() ? "Modified_Vertex"  : "MV:"; break;
    case MODIFIED_EDGES_TAG:     aPrefix = aName.empty() ? "Modified_Edge"    : "ME:"; break;
    case MODIFIED_FACES_TAG:     aPrefix = aName.empty() ? "Modified_Face"    : "MF:"; break;
  }
  aName.insert(0, aPrefix);

  TDataStd_Name::Set(builder(theTag)->NamedShape()->Label(), aName.c_str());
}
bool Model_BodyBuilder::generated(const GeomShapePtr& theNewShape,
                                  const std::string& theName,
                                  const bool theCheckIsInResult)
{
  GeomShapePtr aResultShape = shape();
  if (theCheckIsInResult) {
    bool aNewShapeIsNotInResultShape = !aResultShape->isSubShape(theNewShape, false);
    if (aNewShapeIsNotInResultShape) {
      return false;
    }
  }

  TopoDS_Shape aShape = theNewShape->impl<TopoDS_Shape>();
  builder(myFreePrimitiveTag)->Generated(aShape);
  if (!theName.empty()) {
    std::string aName = theName;
    if (myPrimitivesNamesIndexMap.find(theName) != myPrimitivesNamesIndexMap.end()) {
      IndexTags& anIndexTags = myPrimitivesNamesIndexMap.find(theName)->second;
      aName += "_" + std::to_string(++(anIndexTags.index));
      anIndexTags.tags.push_back(myFreePrimitiveTag);
      if (anIndexTags.index == 2) {
        buildName(anIndexTags.tags.front(), theName + "_1");
      }
    }
    else {
      IndexTags anIndexTags;
      anIndexTags.index = 1;
      anIndexTags.tags.push_back(myFreePrimitiveTag);
      myPrimitivesNamesIndexMap[theName] = anIndexTags;
    }

    buildName(myFreePrimitiveTag, aName);
  }
  ++myFreePrimitiveTag;
  return true;
}

void Model_BodyBuilder::generated(const GeomShapePtr& theOldShape,
                                  const GeomShapePtr& theNewShape,
                                  const std::string& theName)
{
  TopoDS_Shape anOldShape = theOldShape->impl<TopoDS_Shape>();
  TopoDS_Shape aNewShape = theNewShape->impl<TopoDS_Shape>();
  TopAbs_ShapeEnum aNewShapeType = aNewShape.ShapeType();
  int aTag;
  if (aNewShapeType == TopAbs_WIRE || aNewShapeType == TopAbs_SHELL) {
    // TODO: This is a workaround. New shape should be only vertex, edge or face.
    TopAbs_ShapeEnum aShapeTypeToExplore = aNewShapeType == TopAbs_WIRE ? TopAbs_EDGE : TopAbs_FACE;
    aTag = TopAbs_WIRE ? GENERATED_EDGES_TAG : GENERATED_FACES_TAG;
    for (TopExp_Explorer anExp(aNewShape, aShapeTypeToExplore); anExp.More(); anExp.Next()) {
      builder(aTag)->Generated(anOldShape, anExp.Current());
    }
    buildName(aTag, theName);
  } else {
    aTag = getGenerationTag(aNewShape);
    if (aTag == INVALID_TAG) return;
    builder(aTag)->Generated(anOldShape, aNewShape);
    buildName(aTag, theName);
  }
}

void Model_BodyBuilder::modified(const GeomShapePtr& theOldShape,
                                 const GeomShapePtr& theNewShape,
                                 const std::string& theName)
{
  TopoDS_Shape anOldShape = theOldShape->impl<TopoDS_Shape>();
  TopoDS_Shape aNewShape = theNewShape->impl<TopoDS_Shape>();
  int aTag = getModificationTag(aNewShape);
  if (aTag == INVALID_TAG) return;
  builder(aTag)->Modify(anOldShape, aNewShape);
  buildName(aTag, theName);
}

void Model_BodyBuilder::loadDeletedShapes(const GeomMakeShapePtr& theAlgo,
                                          const GeomShapePtr& theOldShape,
                                          const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                          const GeomShapePtr& theShapesToExclude)
{
  TopTools_MapOfShape anAlreadyProcessedShapes;
  GeomShapePtr aResultShape = shape();
  for (GeomAPI_ShapeExplorer anExp(theOldShape, theShapeTypeToExplore);
       anExp.more();
       anExp.next())
  {
    GeomShapePtr anOldSubShape = anExp.current();
    const TopoDS_Shape& anOldSubShape_ = anOldSubShape->impl<TopoDS_Shape>();
    if (!anAlreadyProcessedShapes.Add(anOldSubShape_)
        || !theAlgo->isDeleted(anOldSubShape)
        || aResultShape->isSubShape(anOldSubShape, false)
        || (theShapesToExclude.get() && theShapesToExclude->isSubShape(anOldSubShape, false)))
    {
      continue;
    }

    ListOfShape aNewShapes;
    if (BRepTools_History::IsSupportedType(anOldSubShape_)) { // to avoid crash in #2572
      theAlgo->modified(anOldSubShape, aNewShapes);
    }

    if (aNewShapes.size() == 0
        || (aNewShapes.size() == 1 && aNewShapes.front()->isSame(anOldSubShape))) {
      builder(DELETED_TAG)->Delete(anOldSubShape_);
    }
  }
}

// Keep only the shapes with minimal shape type
static void keepTopLevelShapes(ListOfShape& theShapes,
                               const TopoDS_Shape& theRoot,
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

void Model_BodyBuilder::loadModifiedShapes(const GeomMakeShapePtr& theAlgo,
                                           const GeomShapePtr& theOldShape,
                                           const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                           const std::string& theName)
{
  GeomShapePtr aResultShape = shape();
  GeomShapePtr aShapeToExplore = theOldShape;
  if (theAlgo->isNewShapesCollected(theOldShape, theShapeTypeToExplore)) {
    // use optimized set of old shapes for this
    GeomShapePtr aCompound = theAlgo->oldShapesForNew(theOldShape,
                                                      aResultShape,
                                                      theShapeTypeToExplore);
    if (aCompound.get()) aShapeToExplore = aCompound;
  }

  TopTools_MapOfShape anAlreadyProcessedShapes;
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  for (GeomAPI_ShapeExplorer anOldShapeExp(aShapeToExplore, theShapeTypeToExplore);
       anOldShapeExp.more();
       anOldShapeExp.next())
  {
    GeomShapePtr anOldSubShape = anOldShapeExp.current();
    const TopoDS_Shape& anOldSubShape_ = anOldSubShape->impl<TopoDS_Shape>();

    // There is no sense to write history if shape already processed
    // or old shape does not exist in the document.
    bool anOldSubShapeAlreadyProcessed = !anAlreadyProcessedShapes.Add(anOldSubShape_);
    TDF_Label anAccess2 = std::dynamic_pointer_cast<Model_Document>(
      ModelAPI_Session::get()->moduleDocument())->generalLabel();
    TDF_Label anOriginalLabel;
    bool anOldSubShapeNotInTree =
      !isShapeInTree(aData->shapeLab(), anAccess2, anOldSubShape_, anOriginalLabel);
    if (anOldSubShapeAlreadyProcessed || anOldSubShapeNotInTree) {
      continue;
    }

    // Get new shapes.
    ListOfShape aNewShapes;
    theAlgo->modified(anOldSubShape, aNewShapes);

    for (ListOfShape::const_iterator aNewShapesIt = aNewShapes.cbegin();
         aNewShapesIt != aNewShapes.cend();
         ++aNewShapesIt)
    {
      GeomShapePtr aNewShape = *aNewShapesIt;
      const TopoDS_Shape& aNewShape_ = aNewShape->impl<TopoDS_Shape>();
      bool isGenerated = anOldSubShape_.ShapeType() != aNewShape_.ShapeType();

      bool aNewShapeIsSameAsOldShape = anOldSubShape->isSame(aNewShape);
      bool aNewShapeIsNotInResultShape = !aResultShape->isSubShape(aNewShape, false);
      if (aNewShapeIsSameAsOldShape || aNewShapeIsNotInResultShape)
        continue;

      if (aResultShape->isSame(aNewShape))
        continue; // it is stored on the root level (2241 - history propagation issue)

      int aTag = isGenerated ? getGenerationTag(aNewShape_) : getModificationTag(aNewShape_);
      TNaming_Builder*aBuilder = builder(aTag);
      if (isAlreadyStored(aBuilder, anOldSubShape_, aNewShape_))
        continue; // new shape was already stored.

      buildName(aTag, theName);
      isGenerated ? aBuilder->Generated(anOldSubShape_, aNewShape_)
                  : aBuilder->Modify(anOldSubShape_, aNewShape_);
      // store information about the external document reference to restore old shape on open
      storeExternalReference(anOriginalLabel, aBuilder->NamedShape()->Label());
    }
  }
}

void Model_BodyBuilder::loadGeneratedShapes(const GeomMakeShapePtr& theAlgo,
                                            const GeomShapePtr& theOldShape,
                                            const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                            const std::string& theName,
                                            const bool theSaveOldIfNotInTree)
{
  GeomShapePtr aResultShape = shape();
  TopTools_MapOfShape anAlreadyProcessedShapes;
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  for (GeomAPI_ShapeExplorer anOldShapeExp(theOldShape, theShapeTypeToExplore);
       anOldShapeExp.more();
       anOldShapeExp.next())
  {
    GeomShapePtr anOldSubShape = anOldShapeExp.current();
    const TopoDS_Shape& anOldSubShape_ = anOldSubShape->impl<TopoDS_Shape>();

    // There is no sense to write history if shape already processed
    // or old shape does not exist in the document.
    bool anOldSubShapeAlreadyProcessed = !anAlreadyProcessedShapes.Add(anOldSubShape_);
    TDF_Label anAccess2 = std::dynamic_pointer_cast<Model_Document>(
      ModelAPI_Session::get()->moduleDocument())->generalLabel();
    TDF_Label anOriginalLabel;
    bool anOldSubShapeNotInTree =
      !isShapeInTree(aData->shapeLab(), anAccess2, anOldSubShape_, anOriginalLabel);
    if (anOldSubShapeAlreadyProcessed || anOldSubShapeNotInTree) {
      if (theSaveOldIfNotInTree) {
        std::string aSelectionName = theName + "Selected";
        generated(anOldSubShape, aSelectionName, false);
      } else
        continue;
    }

    // Get new shapes.
    ListOfShape aNewShapes;
    theAlgo->generated(anOldSubShape, aNewShapes);

    keepTopLevelShapes(aNewShapes, anOldSubShape_);

    for (ListOfShape::const_iterator aNewShapesIt = aNewShapes.cbegin();
         aNewShapesIt != aNewShapes.cend();
         ++aNewShapesIt)
    {
      GeomShapePtr aNewShape = *aNewShapesIt;
      const TopoDS_Shape& aNewShape_ = aNewShape->impl<TopoDS_Shape>();

      bool aNewShapeIsSameAsOldShape = anOldSubShape->isSame(aNewShape);
      bool aNewShapeIsNotInResultShape = !aResultShape->isSubShape(aNewShape, false);
      if (aNewShapeIsSameAsOldShape || aNewShapeIsNotInResultShape) {
        continue;
      }

      if (aResultShape->isSame(aNewShape))
        continue; // it is stored on the root level

      TopAbs_ShapeEnum aNewShapeType = aNewShape_.ShapeType();
      if (aNewShapeType == TopAbs_WIRE || aNewShapeType == TopAbs_SHELL) {
        // TODO: This is a workaround. New shape should be only edge or face.
        TopAbs_ShapeEnum aShapeTypeToExplore = aNewShapeType == TopAbs_WIRE ? TopAbs_EDGE
                                                                            : TopAbs_FACE;
        int aTag = TopAbs_WIRE ? GENERATED_EDGES_TAG : GENERATED_FACES_TAG;
        for (TopExp_Explorer anExp(aNewShape_, aShapeTypeToExplore); anExp.More(); anExp.Next()) {
          builder(aTag)->Generated(anOldSubShape_, anExp.Current());
          // store information about the external document reference to restore old shape on open
          storeExternalReference(anOriginalLabel, builder(aTag)->NamedShape()->Label());
        }
        buildName(aTag, theName);
      } else {
        int aTag = getGenerationTag(aNewShape_);
        if (aTag == INVALID_TAG) return;
        builder(aTag)->Generated(anOldSubShape_, aNewShape_);
        buildName(aTag, theName);
        // store information about the external document reference to restore old shape on open
        storeExternalReference(anOriginalLabel, builder(aTag)->NamedShape()->Label());
      }
    }
  }
}

// LCOV_EXCL_START
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
// LCOV_EXCL_STOP

//=======================================================================
void Model_BodyBuilder::loadNextLevels(GeomShapePtr theShape,
                                       const std::string& theName)
{
  if(theShape->isNull()) return;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  std::string aName;
  if (aShape.ShapeType() == TopAbs_SOLID) {
    TopExp_Explorer expl(aShape, TopAbs_FACE);
    for (; expl.More(); expl.Next()) {
      builder(myFreePrimitiveTag)->Generated(expl.Current());
      TCollection_AsciiString aStr(myFreePrimitiveTag - PRIMITIVES_START_TAG + 1);
      aName = theName + "_" + aStr.ToCString();
      buildName(myFreePrimitiveTag, aName);
      ++myFreePrimitiveTag;
    }
  }
  else if (aShape.ShapeType() == TopAbs_SHELL || aShape.ShapeType() == TopAbs_FACE) {
    // load faces and all the free edges
    TopTools_IndexedMapOfShape Faces;
    TopExp::MapShapes(aShape, TopAbs_FACE, Faces);
    if (Faces.Extent() > 1 || (aShape.ShapeType() == TopAbs_SHELL && Faces.Extent() == 1)) {
      TopExp_Explorer expl(aShape, TopAbs_FACE);
      for (; expl.More(); expl.Next()) {
        builder(myFreePrimitiveTag)->Generated(expl.Current());
        TCollection_AsciiString aStr(myFreePrimitiveTag - PRIMITIVES_START_TAG + 1);
        aName = theName + "_" + aStr.ToCString();
        buildName(myFreePrimitiveTag, aName);
        ++myFreePrimitiveTag;
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
        builder(myFreePrimitiveTag)->Generated(anEdgeAndNeighbourFaces.FindKey(i));
        TCollection_AsciiString aStr(myFreePrimitiveTag - PRIMITIVES_START_TAG + 1);
        aName = theName + "_" + aStr.ToCString();
        buildName(myFreePrimitiveTag, aName);
        ++myFreePrimitiveTag;
      } else {
        TopTools_ListIteratorOfListOfShape anIter(aLL);
        const TopoDS_Face& aFace = TopoDS::Face(anIter.Value());
        anIter.Next();
        if(aFace.IsEqual(anIter.Value())) {
          builder(myFreePrimitiveTag)->Generated(anEdgeAndNeighbourFaces.FindKey(i));
          TCollection_AsciiString aStr(myFreePrimitiveTag - PRIMITIVES_START_TAG + 1);
          aName = theName + "_" + aStr.ToCString();
          buildName(myFreePrimitiveTag, aName);
          ++myFreePrimitiveTag;
        }
      }
    }
  } else if (aShape.ShapeType() == TopAbs_WIRE) {
    TopTools_IndexedMapOfShape Edges;
    BRepTools::Map3DEdges(aShape, Edges);
    if (Edges.Extent() == 1) {
      builder(myFreePrimitiveTag++)->Generated(Edges.FindKey(1));
      TopExp_Explorer expl(aShape, TopAbs_VERTEX);
      for (; expl.More(); expl.Next()) {
        builder(myFreePrimitiveTag)->Generated(expl.Current());
        TCollection_AsciiString aStr(myFreePrimitiveTag - PRIMITIVES_START_TAG + 1);
        aName = theName + "_" + aStr.ToCString();
        buildName(myFreePrimitiveTag, aName);
        ++myFreePrimitiveTag;
      }
    } else {
      TopExp_Explorer expl(aShape, TopAbs_EDGE);
      for (; expl.More(); expl.Next()) {
        builder(myFreePrimitiveTag)->Generated(expl.Current());
        TCollection_AsciiString aStr(myFreePrimitiveTag - PRIMITIVES_START_TAG + 1);
        aName = theName + "_" + aStr.ToCString();
        buildName(myFreePrimitiveTag, aName);
        ++myFreePrimitiveTag;
      }
      // and load generated vertices.
      TopTools_DataMapOfShapeShape generated;
      if (getDangleShapes(aShape, TopAbs_EDGE, generated))
      {
        TNaming_Builder* pBuilder = builder(myFreePrimitiveTag++);
        loadGeneratedDangleShapes(aShape, TopAbs_EDGE, pBuilder);
      }
    }
  } else if (aShape.ShapeType() == TopAbs_EDGE) {
    TopExp_Explorer expl(aShape, TopAbs_VERTEX);
    for (; expl.More(); expl.Next()) {
      builder(myFreePrimitiveTag)->Generated(expl.Current());
      TCollection_AsciiString aStr(myFreePrimitiveTag - PRIMITIVES_START_TAG + 1);
      aName = theName + "_" + aStr.ToCString();
      buildName(myFreePrimitiveTag, aName);
      ++myFreePrimitiveTag;
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
void Model_BodyBuilder::loadFirstLevel(GeomShapePtr theShape, const std::string& theName)
{
  GeomShapePtr aShapePtr = shape();
  if (theShape->isNull() || !aShapePtr.get())
    return;
  TopoDS_Shape aShape = shape()->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return;
  std::string aName;
  if (aShape.ShapeType() == TopAbs_COMPOUND || aShape.ShapeType() == TopAbs_COMPSOLID) {
    TopoDS_Iterator itr(aShape);
    for (; itr.More(); itr.Next()) {
      builder(myFreePrimitiveTag)->Generated(itr.Value());
      TCollection_AsciiString aStr(myFreePrimitiveTag - PRIMITIVES_START_TAG + 1);
      aName = theName + "_" + aStr.ToCString();
      buildName(myFreePrimitiveTag, aName);
      ++myFreePrimitiveTag;
      if (itr.Value().ShapeType() == TopAbs_COMPOUND ||
        itr.Value().ShapeType() == TopAbs_COMPSOLID)
      {
        GeomShapePtr itrShape(new GeomAPI_Shape());
        itrShape->setImpl(new TopoDS_Shape(itr.Value()));
        loadFirstLevel(itrShape, theName);
      } else {
        GeomShapePtr itrShape(new GeomAPI_Shape());
        itrShape->setImpl(new TopoDS_Shape(itr.Value()));
        loadNextLevels(itrShape, theName);
      }
    }
  } else {
    GeomShapePtr itrShape(new GeomAPI_Shape());
    itrShape->setImpl(new TopoDS_Shape(aShape));
    loadNextLevels(itrShape, theName);
  }
  TopTools_ListOfShape   aList;
  if(findAmbiguities(aShape, aList)) {
    TopTools_ListIteratorOfListOfShape it(aList);
    for (; it.More(); it.Next(), ++myFreePrimitiveTag) {
      builder(myFreePrimitiveTag)->Generated(it.Value());
      TCollection_AsciiString aStr(myFreePrimitiveTag - PRIMITIVES_START_TAG + 1);
      aName = theName + "_" + aStr.ToCString();
      buildName(myFreePrimitiveTag, aName);
    }
  }
}

GeomShapePtr Model_BodyBuilder::shape()
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
        GeomShapePtr aRes(new GeomAPI_Shape);
        aRes->setImpl(new TopoDS_Shape(aShape));
        return aRes;
      }
    }
  }
  return GeomShapePtr();
}
