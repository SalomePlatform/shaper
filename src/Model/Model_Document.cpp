// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <Model_Document.h>
#include <Model_Data.h>
#include <Model_Objects.h>
#include <Model_Application.h>
#include <Model_Session.h>
#include <Model_Events.h>
#include <Model_Tools.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultBody.h>
#include <Events_Loop.h>
#include <Events_InfoMessage.h>
#include <GeomAPI_Tools.h>

#include <Locale_Convert.h>

#include <TDataStd_Integer.hxx>
#include <TDataStd_Comment.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_HLabelArray1.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDF_Reference.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Delta.hxx>
#include <TDF_AttributeDelta.hxx>
#include <TDF_AttributeDeltaList.hxx>
#include <TDF_ListIteratorOfAttributeDeltaList.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_Tool.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_SameShapeIterator.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>

#ifdef TINSPECTOR
#include <CDF_Directory.hxx>
#endif

#include <UTL.hxx>

#include <climits>
#ifndef WIN32
#include <sys/stat.h>
#endif

#ifdef WIN32
# define _separator_ '\\'
#else
# define _separator_ '/'
#endif

static const int UNDO_LIMIT = 1000;  // number of possible undo operations (big for sketcher)

static const int TAG_GENERAL = 1;  // general properties tag

// general sub-labels
/// where the reference to the current feature label is located (or no attribute if null feature)
static const int TAG_CURRENT_FEATURE = 1; ///< reference to the current feature
/// integer, index of the transaction + GUID for auto recomputation blocking
static const int TAG_CURRENT_TRANSACTION = 2;
static const int TAG_SELECTION_FEATURE = 3; ///< integer, tag of the selection feature label
static const int TAG_NODES_STATE = 4; ///< array, tag of the Object Browser nodes states
///< naming structures constructions selected from other document
static const int TAG_EXTERNAL_CONSTRUCTIONS = 5;

/// reference to the shape in external document: sting list attribute identifier
static const Standard_GUID kEXTERNAL_SHAPE_REF("9aa5dd14-6d34-4a8d-8786-05842fd7bbbd");

Model_Document::Model_Document(const int theID, const std::string theKind)
    : myID(theID),
      myKind(theKind),
      myDoc(new TDocStd_Document("BinOcaf")),  // binary OCAF format
      myIsActive(false),
      myIsSetCurrentFeature(false)
{
#ifdef TINSPECTOR
  ModelAPI_Session::get()->application()->NewDocument("BinOcaf", myDoc);
#endif
  myObjs = new Model_Objects(myDoc->Main());
  myDoc->SetUndoLimit(UNDO_LIMIT);
  myTransactionSave = 0;
  myExecuteFeatures = true;
  // to have something in the document and avoid empty doc open/save problem
  // in transaction for nesting correct working
  myDoc->NewCommand();
  TDataStd_Integer::Set(myDoc->Main().Father(), 0);
  // this to avoid creation of integer attribute outside the transaction after undo
  transactionID();
  myDoc->CommitCommand();
}

Model_Document::~Model_Document()
{
  if (!myDoc.IsNull())
  {
    myDoc->ClearUndos();
    myDoc->ClearRedos();
  }
}

void Model_Document::setThis(DocumentPtr theDoc)
{
  myObjs->setOwner(theDoc);
}

/// Returns the file name of this document by the name of directory and identifier of a document
static TCollection_ExtendedString DocFileName(const char* theDirName, const std::string& theID)
{
  TCollection_ExtendedString aPath((const Standard_CString) theDirName);
  // remove end-separators
  while(aPath.Length() &&
        (aPath.Value(aPath.Length()) == '\\' || aPath.Value(aPath.Length()) == '/'))
    aPath.Remove(aPath.Length());
  aPath += _separator_;
  aPath += theID.c_str();
  aPath += ".cbf";  // standard binary file extension
  return aPath;
}

bool Model_Document::isRoot() const
{
  return this == Model_Session::get()->moduleDocument().get();
}

// LCOV_EXCL_START
/// Makes all modification and generation naming shapes that have old shapes corresponding to
/// shapes in a root document be equal to this root document
static void updateShapesFromRoot(const TDF_Label theThisAccess, const TDF_Label theRootAccess)
{
  TopTools_DataMapOfShapeShape aCurrentToRoot; // shapes that must be updated: from this to root
  TDF_ChildIDIterator aThisIter(theThisAccess.Root(), kEXTERNAL_SHAPE_REF, true);
  for(; aThisIter.More(); aThisIter.Next()) {
    aCurrentToRoot.Clear();
    Handle(TNaming_NamedShape) aNS;
    if (!aThisIter.Value()->Label().FindAttribute(TNaming_NamedShape::GetID(), aNS))
      continue;
    if (aNS->Evolution() != TNaming_GENERATED && aNS->Evolution() != TNaming_MODIFY)
      continue;
    for (TNaming_Iterator aNSIter(aNS); aNSIter.More(); aNSIter.Next()) {
      const TopoDS_Shape& anOld = aNSIter.OldShape();
      if (anOld.IsNull())
        continue;
      TNaming_OldShapeIterator aNewIter(anOld, theThisAccess);
      for (; aNewIter.More(); aNewIter.Next()) {
        TNaming_Evolution anEvolution = aNewIter.NamedShape()->Evolution();
        if (anEvolution != TNaming_SELECTED && anEvolution != TNaming_DELETE)
          break;
      }
      if (aNewIter.More())
        continue;
      GeomShapePtr anOldShape(new GeomAPI_Shape), aRootShape(new GeomAPI_Shape);
      anOldShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(anOld));
      anOldShape = GeomAPI_Tools::getTypedShape(anOldShape);

      // search the same shape in the root document
      Handle(TDataStd_ExtStringList) anEntries =
        Handle(TDataStd_ExtStringList)::DownCast(aThisIter.Value());
      TDataStd_ListOfExtendedString::Iterator anIter(anEntries->List());
      for (; anIter.More(); anIter.Next()) {
        TDF_Label aRootLab;
        TDF_Tool::Label(theRootAccess.Data(), anIter.Value(), aRootLab);
        if (aRootLab.IsNull())
          continue;
        Handle(TNaming_NamedShape) aRootNS;
        if (!aRootLab.FindAttribute(TNaming_NamedShape::GetID(), aRootNS))
          continue;
        TNaming_Iterator aRootShapes(aRootNS);
        for (; aRootShapes.More(); aRootShapes.Next()) {
          if (aRootShapes.NewShape().IsNull())
            continue;
          aRootShape->setImpl(new TopoDS_Shape(aRootShapes.NewShape()));
          aRootShape = GeomAPI_Tools::getTypedShape(aRootShape);
          if (!anOldShape->isEqual(aRootShape)) // special checking by geometry
            continue;
          // found a good corresponded shape
          if (!anOld.IsEqual(aRootShapes.NewShape()))
            aCurrentToRoot.Bind(anOld, aRootShapes.NewShape());
        }
      }
    }
    if (!aCurrentToRoot.IsEmpty()) { // update the whole named shape content
      TopTools_ListOfShape anOld, aNew;
      TNaming_Evolution anEvol = aNS->Evolution();
      for(TNaming_Iterator aNSIter(aNS); aNSIter.More(); aNSIter.Next()) {
        anOld.Prepend(aCurrentToRoot.IsBound(aNSIter.OldShape()) ?
          aCurrentToRoot.Find(aNSIter.OldShape()) : aNSIter.OldShape());
        aNew.Prepend(aNSIter.NewShape());
      }
      TNaming_Builder aBuilder(aNS->Label());
      TopTools_ListOfShape::Iterator anOldIter(anOld), aNewIter(aNew);
      for(; anOldIter.More(); anOldIter.Next(), aNewIter.Next()) {
        if (anEvol == TNaming_GENERATED) {
          aBuilder.Generated(anOldIter.Value(), aNewIter.Value());
        } else if (anEvol == TNaming_MODIFY) {
          aBuilder.Modify(anOldIter.Value(), aNewIter.Value());
        }
      }
    }
  }
}
// LCOV_EXCL_STOP

static bool loadDocument(Handle(Model_Application) theApp,
                         Handle(TDocStd_Document)& theDoc,
                         const TCollection_ExtendedString& theFilename)
{
  PCDM_ReaderStatus aStatus = (PCDM_ReaderStatus)-1;
  try {
    aStatus = theApp->Open(theFilename, theDoc);
  } catch (Standard_Failure const& anException) {
    Events_InfoMessage("Model_Document",
        "Exception in opening of document: %1").arg(anException.GetMessageString()).send();
    return false;
  }
  bool isOk = aStatus == PCDM_RS_OK;
  if (!isOk) {
    // LCOV_EXCL_START
    switch (aStatus) {
      case PCDM_RS_UnknownDocument:
        Events_InfoMessage("Model_Document", "Can not open document").send();
        break;
      case PCDM_RS_AlreadyRetrieved:
        Events_InfoMessage("Model_Document", "Can not open document: already opened").send();
        break;
      case PCDM_RS_AlreadyRetrievedAndModified:
        Events_InfoMessage("Model_Document",
            "Can not open document: already opened and modified").send();
        break;
      case PCDM_RS_NoDriver:
        Events_InfoMessage("Model_Document",
                           "Can not open document: driver library is not found").send();
        break;
      case PCDM_RS_UnknownFileDriver:
        Events_InfoMessage("Model_Document",
                           "Can not open document: unknown driver for opening").send();
        break;
      case PCDM_RS_OpenError:
        Events_InfoMessage("Model_Document", "Can not open document: file open error").send();
        break;
      case PCDM_RS_NoVersion:
        Events_InfoMessage("Model_Document", "Can not open document: invalid version").send();
        break;
      case PCDM_RS_NoModel:
        Events_InfoMessage("Model_Document", "Can not open document: no data model").send();
        break;
      case PCDM_RS_NoDocument:
        Events_InfoMessage("Model_Document", "Can not open document: no document inside").send();
        break;
      case PCDM_RS_FormatFailure:
        Events_InfoMessage("Model_Document", "Can not open document: format failure").send();
        break;
      case PCDM_RS_TypeNotFoundInSchema:
        Events_InfoMessage("Model_Document", "Can not open document: invalid object").send();
        break;
      case PCDM_RS_UnrecognizedFileFormat:
        Events_InfoMessage("Model_Document",
                           "Can not open document: unrecognized file format").send();
        break;
      case PCDM_RS_MakeFailure:
        Events_InfoMessage("Model_Document", "Can not open document: make failure").send();
        break;
      case PCDM_RS_PermissionDenied:
        Events_InfoMessage("Model_Document", "Can not open document: permission denied").send();
        break;
      case PCDM_RS_DriverFailure:
        Events_InfoMessage("Model_Document", "Can not open document: driver failure").send();
        break;
      default:
        Events_InfoMessage("Model_Document", "Can not open document: unknown error").send();
        break;
    }
    // LCOV_EXCL_STOP
  }
  return isOk;
}

bool Model_Document::load(const char* theDirName, const char* theFileName, DocumentPtr theThis)
{
  Handle(Model_Application) anApp = Model_Application::getApplication();
  if (isRoot()) {
    anApp->setLoadPath(theDirName);
  }
  TCollection_ExtendedString aPath(DocFileName(theDirName, theFileName));
  Handle(TDocStd_Document) aLoaded;
  bool isOk = loadDocument(anApp, aLoaded, aPath);

  std::shared_ptr<Model_Session> aSession =
    std::dynamic_pointer_cast<Model_Session>(Model_Session::get());
  if (isOk) {
    // keep handle to avoid destruction of the document until myObjs works on it
    Handle(TDocStd_Document) anOldDoc = myDoc;
    myDoc = aLoaded;
    myDoc->SetUndoLimit(UNDO_LIMIT);

    // to avoid the problem that feature is created in the current, not this, document
    aSession->setActiveDocument(anApp->document(myID), false);
    aSession->setCheckTransactions(false);
    if (myObjs)
      delete myObjs;
    anOldDoc->ClearRedos();
    anOldDoc->ClearUndos();
    anOldDoc.Nullify();
    myObjs = new Model_Objects(myDoc->Main()); // synchronization is inside
    myObjs->setOwner(theThis);
    // update the current features status
    setCurrentFeature(currentFeature(false), false);
    aSession->setCheckTransactions(true);
    aSession->setActiveDocument(aSession->moduleDocument(), false);
    // this is done in Part result "activate", so no needed here. Causes not-blue active part.
    // aSession->setActiveDocument(anApp->getDocument(myID), true);

    // make sub-parts as loaded by demand
    std::list<ResultPtr> aPartResults;
    myObjs->allResults(ModelAPI_ResultPart::group(), aPartResults);
    std::list<ResultPtr>::iterator aPartRes = aPartResults.begin();
    for(; aPartRes != aPartResults.end(); aPartRes++) {
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aPartRes);
      if (aPart.get())
        anApp->setLoadByDemand(aPart->data()->name(),
          aPart->data()->document(ModelAPI_ResultPart::DOC_REF())->docId());
    }
    if (!isRoot()) {
      updateShapesFromRoot(myDoc->Main(),
        std::dynamic_pointer_cast<Model_Document>(aSession->moduleDocument())->generalLabel());
    }
  } else { // open failed, but new document was created to work with it: inform the model
    aSession->setActiveDocument(Model_Session::get()->moduleDocument(), false);
  }
  return isOk;
}

bool Model_Document::importPart(const char* theFileName,
                                std::list<std::shared_ptr<ModelAPI_Feature> >& theImported,
                                bool theCheckOnly)
{
  Handle(Model_Application) anApp = Model_Application::getApplication();
  TCollection_ExtendedString aFormat;
  if (!anApp->Format(theFileName, aFormat))
    return false;

  Handle(TDocStd_Document) aTempDoc;
  bool isOk = loadDocument(anApp, aTempDoc, theFileName);

  if (isOk && theCheckOnly) {
    // verify all features are applicable for the current document type (e.g. PartSet)
    std::shared_ptr<Model_Session> aSession =
        std::dynamic_pointer_cast<Model_Session>(ModelAPI_Session::get());
    for (TDF_ChildIterator anIt(aTempDoc->Main()); anIt.More() && isOk; anIt.Next()) {
      TDF_Label aCurrentLab = anIt.Value();
      Handle(TDataStd_Comment) aFeatureID;
      TDF_Label aNewFeatuerLab;
      if (aCurrentLab.FindAttribute(TDataStd_Comment::GetID(), aFeatureID)) {
        TCollection_AsciiString anID(aFeatureID->Get());
        std::string aFeatureKind(anID.ToCString());
        if (aSession->myPlugins.find(aFeatureKind) != aSession->myPlugins.end()) {
          std::string& aDocKind = aSession->myPlugins[aFeatureKind].second;
          isOk = aDocKind.empty() || aDocKind == kind();
        }
      }
    }
  }

  if (isOk && !theCheckOnly) {
    // copy features from the temporary document to the current
    Handle(TDF_RelocationTable) aRelocTable = new TDF_RelocationTable();
    // add to relocation table source root label to the destination label because
    // sometimes there could be a reference to root (issue 3267 on import part
    // with sketch with removed features)
    aRelocTable->SetRelocation(aTempDoc->Main().Root(), myDoc->Main().Root());
    TDF_LabelList anAllNewFeatures;
    // Perform the copying twice for correct references:
    // 1. copy labels hierarchy and fill the relocation table
    for (TDF_ChildIterator anIt(aTempDoc->Main()); anIt.More(); anIt.Next()) {
      TDF_Label aCurrentLab = anIt.Value();
      Handle(TDataStd_Comment) aFeatureID;
      TDF_Label aNewFeatuerLab;
      if (aCurrentLab.FindAttribute(TDataStd_Comment::GetID(), aFeatureID)) {
        TCollection_AsciiString anID(aFeatureID->Get());
        FeaturePtr aNewFeature = addFeature(anID.ToCString());
        std::shared_ptr<Model_Data> aData =
            std::dynamic_pointer_cast<Model_Data>(aNewFeature->data());
        aNewFeatuerLab = aData->label().Father();
        Model_Tools::copyLabels(aCurrentLab, aNewFeatuerLab, aRelocTable);
        theImported.push_back(aNewFeature);
      }
      anAllNewFeatures.Append(aNewFeatuerLab);
    }
    // 2. copy attributes
    std::set<TCollection_AsciiString> aCoordinateLabels;
    Model_Tools::labelsOfCoordinates(aCoordinateLabels, aRelocTable);
    TDF_ListIteratorOfLabelList aNewIt(anAllNewFeatures);
    for (TDF_ChildIterator anIt(aTempDoc->Main()); anIt.More(); anIt.Next()) {
      TDF_Label aCurrentLab = anIt.Value();
      TDF_Label aFeatureLab = aNewIt.Value();
      if (aFeatureLab.IsNull())
        anAllNewFeatures.Remove(aNewIt);
      else {
        Model_Tools::copyAttrsAndKeepRefsToCoordinates(
            aCurrentLab, aFeatureLab, aCoordinateLabels, aRelocTable);
        aNewIt.Next();
      }
    }

    myObjs->synchronizeFeatures(anAllNewFeatures, true, false, false, true);
  }

  if (anApp->CanClose(aTempDoc) == CDM_CCS_OK)
    anApp->Close(aTempDoc);
  return isOk;
}

static bool saveDocument(Handle(Model_Application) theApp,
                         Handle(TDocStd_Document) theDoc,
                         const TCollection_ExtendedString& theFilename)
{
  PCDM_StoreStatus aStatus;
  try {
    // create the directory to save the document
    OSD_Path aPathToFile = UTL::Path(theFilename);
    aPathToFile.SetName("");
    aPathToFile.SetExtension("");
    OSD_Directory aBaseDir(aPathToFile);
    if (aPathToFile.TrekLength() != 0 && !aBaseDir.Exists())
      aBaseDir.Build(OSD_Protection());
    // save the document
    aStatus = theApp->SaveAs(theDoc, theFilename);
  }
  catch (Standard_Failure const& anException) {
    Events_InfoMessage("Model_Document",
      "Exception in saving of document: %1").arg(anException.GetMessageString()).send();
    return false;
  }
  bool isDone = aStatus == PCDM_SS_OK || aStatus == PCDM_SS_No_Obj;
  if (!isDone) {
    switch (aStatus) {
    case PCDM_SS_DriverFailure:
      Events_InfoMessage("Model_Document",
        "Can not save document: save driver-library failure").send();
      break;
    case PCDM_SS_WriteFailure:
      Events_InfoMessage("Model_Document", "Can not save document: file writing failure").send();
      break;
    case PCDM_SS_Failure:
    default:
      Events_InfoMessage("Model_Document", "Can not save document").send();
      break;
    }
  }
  return isDone;
}

bool Model_Document::save(
  const char* theDirName, const char* theFileName, std::list<std::string>& theResults)
{
  // if the history line is not in the end, move it to the end before save, otherwise
  // problems with results restore and (the most important) naming problems will appear
  // due to change evolution to SELECTION (problems in NamedShape and Name)
  FeaturePtr aWasCurrent;
  std::shared_ptr<Model_Session> aSession =
    std::dynamic_pointer_cast<Model_Session>(Model_Session::get());
  if (currentFeature(false) != lastFeature()) {
    aSession->setCheckTransactions(false);
    aWasCurrent = currentFeature(false);
    // if last is nested into something else, make this something else as last:
    // otherwise it will look like edition of sub-element, so, the main will be disabled
    FeaturePtr aLast = lastFeature();
    if (aLast.get()) {
      CompositeFeaturePtr aMain = ModelAPI_Tools::compositeOwner(aLast);
      while(aMain.get()) {
        aLast = aMain;
        aMain = ModelAPI_Tools::compositeOwner(aLast);
      }
    }
    setCurrentFeature(aLast, true);
  }
  // create a directory in the root document if it is not yet exist
  Handle(Model_Application) anApp = Model_Application::getApplication();
  if (isRoot()) {
#ifdef WIN32
    size_t aDirLen = strlen(theDirName);
    std::wstring aWStr(aDirLen, L'#');
    mbstowcs(&aWStr[0], theDirName, aDirLen);
    CreateDirectory(aWStr.c_str(), NULL);
#else
    mkdir(theDirName, 0x1ff);
#endif
  }
  // filename in the dir is id of document inside of the given directory
  TCollection_ExtendedString aPath(DocFileName(theDirName, theFileName));
  bool isDone = saveDocument(anApp, myDoc, aPath);

  if (aWasCurrent.get()) { // return the current feature to the initial position
    setCurrentFeature(aWasCurrent, false);
    aSession->setCheckTransactions(true);
  }

  myTransactionSave = int(myTransactions.size());
  if (isDone) {  // save also sub-documents if any
    theResults.push_back(TCollection_AsciiString(aPath).ToCString());
    // iterate all result parts to find all loaded or not yet loaded documents
    std::list<ResultPtr> aPartResults;
    myObjs->allResults(ModelAPI_ResultPart::group(), aPartResults);
    std::list<ResultPtr>::iterator aPartRes = aPartResults.begin();
    for(; aPartRes != aPartResults.end(); aPartRes++) {
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aPartRes);
      if (!aPart->isActivated()) {
        // copy not-activated document that is not in the memory
        std::string aDocName = Locale::Convert::toString(aPart->data()->name());
        if (!aDocName.empty()) {
          // just copy file
          TCollection_AsciiString aSubPath(DocFileName(anApp->loadPath().c_str(), aDocName));
          OSD_Path aCopyPath(aSubPath);
          OSD_File aFile(aCopyPath);
          if (aFile.Exists()) {
            TCollection_AsciiString aDestinationDir(DocFileName(theDirName, aDocName));
            OSD_Path aDestination(aDestinationDir);
            aFile.Copy(aDestination);
            theResults.push_back(aDestinationDir.ToCString());
          } else {
            Events_InfoMessage("Model_Document",
              "Can not open file %1 for saving").arg(aSubPath.ToCString()).send();
          }
        }
      } else { // simply save opened document
        std::string aDocName = Locale::Convert::toString(aPart->data()->name());
        isDone = std::dynamic_pointer_cast<Model_Document>(aPart->partDoc())->
          save(theDirName, aDocName.c_str(), theResults);
      }
    }
  }
  return isDone;
}

bool Model_Document::save(const char* theFilename,
                          const std::list<FeaturePtr>& theExportFeatures) const
{
  Handle(Model_Application) anApp = Model_Application::getApplication();
  TCollection_ExtendedString aFormat;
  if (!anApp->Format(theFilename, aFormat))
    return false;

  Handle(TDocStd_Document) aTempDoc = new TDocStd_Document(aFormat);
  TDF_Label aMain = aTempDoc->Main();

  Handle(TDF_RelocationTable) aRelocTable = new TDF_RelocationTable();
  std::list<FeaturePtr>::const_iterator anIt = theExportFeatures.begin();
  // Perform the copying twice for correct references:
  // 1. copy labels hierarchy and fill the relocation table
  for (; anIt != theExportFeatures.end(); ++anIt) {
    TDF_Label aFeatureLab = aMain.NewChild();
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>((*anIt)->data());
    Model_Tools::copyLabels(aData->label().Father(), aFeatureLab, aRelocTable);
  }
  // 2. copy attributes
  std::set<TCollection_AsciiString> aCoordinateLabels;
  Model_Tools::labelsOfCoordinates(aCoordinateLabels, aRelocTable);
  TDF_ChildIterator aChildIt(aMain);
  for (anIt = theExportFeatures.begin(); anIt != theExportFeatures.end(); ++anIt) {
    TDF_Label aFeatureLab = aChildIt.Value();
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>((*anIt)->data());
    Model_Tools::copyAttrsAndKeepRefsToCoordinates(
        aData->label().Father(), aFeatureLab, aCoordinateLabels, aRelocTable);
    aChildIt.Next();
  }

  bool isDone = saveDocument(anApp, aTempDoc, theFilename);
  if (aTempDoc->CanClose() == CDM_CCS_OK)
    aTempDoc->Close();
  return isDone;
}

void Model_Document::close(const bool theForever)
{
  std::shared_ptr<ModelAPI_Session> aPM = Model_Session::get();
  if (!isRoot() && this == aPM->activeDocument().get()) {
    aPM->setActiveDocument(aPM->moduleDocument());
  } else if (isRoot()) {
    // erase the active document if root is closed
    aPM->setActiveDocument(DocumentPtr());
  }
  // close all subs
  const std::set<int> aSubs = subDocuments();
  std::set<int>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++) {
    std::shared_ptr<Model_Document> aSub = subDoc(*aSubIter);
    if (aSub->myObjs) // if it was not closed before
      aSub->close(theForever);
  }

  // close for this document needs no transaction in this document
  std::static_pointer_cast<Model_Session>(Model_Session::get())->setCheckTransactions(false);

  // close all only if it is really asked, otherwise it can be undone/redone
  if (theForever) {
    // flush everything to avoid messages with bad objects
    delete myObjs;
    myObjs = 0;
    if (myDoc->CanClose() == CDM_CCS_OK)
      myDoc->Close();
    mySelectionFeature.reset();
  } else {
    setCurrentFeature(FeaturePtr(), false); // disables all features
    // update the OB: features are disabled (on remove of Part)
    Events_Loop* aLoop = Events_Loop::loop();
    static Events_ID aDeleteEvent = Events_Loop::eventByName(EVENT_OBJECT_DELETED);
    aLoop->flush(aDeleteEvent);
  }

  std::static_pointer_cast<Model_Session>(Model_Session::get())->setCheckTransactions(true);
}

void Model_Document::startOperation()
{
  incrementTransactionID(); // outside of transaction in order to avoid empty transactions keeping
  if (myDoc->HasOpenCommand()) {  // start of nested command
    if (myDoc->CommitCommand()) {
      // commit the current: it will contain all nested after compactification
      myTransactions.rbegin()->myOCAFNum++; // if has open command, the list is not empty
    }
    myNestedNum.push_back(0); // start of nested operation with zero transactions inside yet
    myDoc->OpenCommand();
  } else {  // start the simple command
    myDoc->NewCommand();
  }
  // starts a new operation
  myTransactions.push_back(Transaction());
  if (!myNestedNum.empty())
    (*myNestedNum.rbegin())++;
  myRedos.clear();
  // new command for all subs
  const std::set<int> aSubs = subDocuments();
  std::set<int>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++)
    subDoc(*aSubIter)->startOperation();
}

void Model_Document::compactNested()
{
  if (!myNestedNum.empty()) {
    int aNumToCompact = *(myNestedNum.rbegin());
    int aSumOfTransaction = 0;
    for(int a = 0; a < aNumToCompact; a++) {
      aSumOfTransaction += myTransactions.rbegin()->myOCAFNum;
      myTransactions.pop_back();
    }
    // the latest transaction is the start of lower-level operation which starts the nested
    myTransactions.rbegin()->myOCAFNum += aSumOfTransaction;
    myNestedNum.pop_back();
  }
}

/// Compares the content of the given attributes, returns true if equal.
/// This method is used to avoid empty transactions when only "current" is changed
/// to some value and then comes back in this transaction, so, it compares only
/// references and Boolean and Integer Arrays for the current moment.
static bool isEqualContent(Handle(TDF_Attribute) theAttr1, Handle(TDF_Attribute) theAttr2)
{
  if (Standard_GUID::IsEqual(theAttr1->ID(), TDF_Reference::GetID())) { // reference
    Handle(TDF_Reference) aRef1 = Handle(TDF_Reference)::DownCast(theAttr1);
    Handle(TDF_Reference) aRef2 = Handle(TDF_Reference)::DownCast(theAttr2);
    if (aRef1.IsNull() && aRef2.IsNull())
      return true;
    if (aRef1.IsNull() || aRef2.IsNull())
      return false;
    return aRef1->Get().IsEqual(aRef2->Get()) == Standard_True;
  } else if (Standard_GUID::IsEqual(theAttr1->ID(), TDataStd_BooleanArray::GetID())) {
    Handle(TDataStd_BooleanArray) anArr1 = Handle(TDataStd_BooleanArray)::DownCast(theAttr1);
    Handle(TDataStd_BooleanArray) anArr2 = Handle(TDataStd_BooleanArray)::DownCast(theAttr2);
    if (anArr1.IsNull() && anArr2.IsNull())
      return true;
    if (anArr1.IsNull() || anArr2.IsNull())
      return false;
    if (anArr1->Lower() == anArr2->Lower() && anArr1->Upper() == anArr2->Upper()) {
      for(int a = anArr1->Lower(); a <= anArr1->Upper(); a++) {
        if (a == 1 && // second is for display
            anArr2->Label().Tag() == 1 && (anArr2->Label().Depth() == 4 ||
            anArr2->Label().Depth() == 6))
          continue;
        if (anArr1->Value(a) != anArr2->Value(a))
          return false;
      }
      return true;
    }
  } else if (Standard_GUID::IsEqual(theAttr1->ID(), TDataStd_IntegerArray::GetID())) {
    Handle(TDataStd_IntegerArray) anArr1 = Handle(TDataStd_IntegerArray)::DownCast(theAttr1);
    Handle(TDataStd_IntegerArray) anArr2 = Handle(TDataStd_IntegerArray)::DownCast(theAttr2);
    if (anArr1.IsNull() && anArr2.IsNull())
      return true;
    if (anArr1.IsNull() || anArr2.IsNull())
      return false;
    if (anArr1->Lower() == anArr2->Lower() && anArr1->Upper() == anArr2->Upper()) {
      for(int a = anArr1->Lower(); a <= anArr1->Upper(); a++)
        if (anArr1->Value(a) != anArr2->Value(a)) {
          // avoid the transaction ID checking
          if (a == 2 && anArr1->Upper() == 2 && anArr2->Label().Tag() == 1 &&
            (anArr2->Label().Depth() == 4 || anArr2->Label().Depth() == 6))
            continue;
          return false;
        }
      return true;
    }
  } else if (Standard_GUID::IsEqual(theAttr1->ID(), TDataStd_ReferenceArray::GetID())) {
    Handle(TDataStd_ReferenceArray) anArr1 = Handle(TDataStd_ReferenceArray)::DownCast(theAttr1);
    Handle(TDataStd_ReferenceArray) anArr2 = Handle(TDataStd_ReferenceArray)::DownCast(theAttr2);
    if (anArr1.IsNull() && anArr2.IsNull())
      return true;
    if (anArr1.IsNull() || anArr2.IsNull())
      return false;
    if (anArr1->Lower() == anArr2->Lower() && anArr1->Upper() == anArr2->Upper()) {
      for(int a = anArr1->Lower(); a <= anArr1->Upper(); a++)
        if (anArr1->Value(a) != anArr2->Value(a)) {
          // avoid the transaction ID checking
          if (a == 2 && anArr1->Upper() == 2 && anArr2->Label().Tag() == 1 &&
            (anArr2->Label().Depth() == 4 || anArr2->Label().Depth() == 6))
            continue;
          return false;
        }
      return true;
    }
  } else if (Standard_GUID::IsEqual(theAttr1->ID(), TDataStd_ReferenceList::GetID())) {
    Handle(TDataStd_ReferenceList) aList1 = Handle(TDataStd_ReferenceList)::DownCast(theAttr1);
    Handle(TDataStd_ReferenceList) aList2= Handle(TDataStd_ReferenceList)::DownCast(theAttr2);
    if (aList1.IsNull() && aList2.IsNull())
      return true;
    if (aList1.IsNull() || aList2.IsNull())
      return false;
    const TDF_LabelList& aLList1 = aList1->List();
    const TDF_LabelList& aLList2 = aList2->List();
    TDF_ListIteratorOfLabelList aLIter1(aLList1);
    TDF_ListIteratorOfLabelList aLIter2(aLList2);
    for(; aLIter1.More() && aLIter2.More(); aLIter1.Next(), aLIter2.Next()) {
      if (aLIter1.Value() != aLIter2.Value())
        return false;
    }
    return !aLIter1.More() && !aLIter2.More(); // both lists are with the same size
  } else if (Standard_GUID::IsEqual(theAttr1->ID(), TDF_TagSource::GetID())) {
    return true; // it just for created and removed feature: nothing is changed
  }
  return false;
}

/// Returns true if the last transaction is actually empty: modification to the same values
/// were performed only
static bool isEmptyTransaction(const Handle(TDocStd_Document)& theDoc) {
  Handle(TDF_Delta) aDelta;
  aDelta = theDoc->GetUndos().Last();
  TDF_LabelList aDeltaList;
  aDelta->Labels(aDeltaList); // it clears list, so, use new one and then append to the result
  if (!aDeltaList.IsEmpty()) {
    return false;
  }
  // add also label of the modified attributes
  const TDF_AttributeDeltaList& anAttrs = aDelta->AttributeDeltas();
  for (TDF_ListIteratorOfAttributeDeltaList anAttr(anAttrs); anAttr.More(); anAttr.Next()) {
    Handle(TDF_AttributeDelta)& anADelta = anAttr.Value();
    Handle(TDF_DeltaOnAddition) anAddition = Handle(TDF_DeltaOnAddition)::DownCast(anADelta);
    if (anAddition.IsNull()) { // if the attribute was added, transaction is not empty
      if (!anADelta->Label().IsNull() && !anADelta->Attribute().IsNull()) {
        Handle(TDF_Attribute) aCurrentAttr;
        if (anADelta->Label().FindAttribute(anADelta->Attribute()->ID(), aCurrentAttr)) {
          if (isEqualContent(anADelta->Attribute(), aCurrentAttr)) {
            continue; // attribute is not changed actually
          }
        } else
          if (Standard_GUID::IsEqual(anADelta->Attribute()->ID(), TDataStd_AsciiString::GetID())) {
            continue; // error message is disappeared
        }
      }
    }
    return false;
  }
  return true;
}

bool Model_Document::finishOperation()
{
  bool isNestedClosed = !myDoc->HasOpenCommand() && !myNestedNum.empty();
  static std::shared_ptr<Model_Session> aSession =
    std::static_pointer_cast<Model_Session>(Model_Session::get());

  // open transaction if nested is closed to fit inside
  // all synchronizeBackRefs and flushed consequences
  if (isNestedClosed) {
    myDoc->OpenCommand();
  }
  // do it before flashes to enable and recompute nesting features correctly
  if (myNestedNum.empty() || (isNestedClosed && myNestedNum.size() == 1)) {
    // if all nested operations are closed, make current the higher level objects (to perform
    // it in the python scripts correctly): sketch become current after creation of sub-elements
    FeaturePtr aCurrent = currentFeature(false);
    CompositeFeaturePtr aMain, aNext = ModelAPI_Tools::compositeOwner(aCurrent);
    while(aNext.get()) {
      aMain = aNext;
      aNext = ModelAPI_Tools::compositeOwner(aMain);
    }
    if (aMain.get() && aMain != aCurrent)
      setCurrentFeature(aMain, false);
  }
  myObjs->synchronizeBackRefs();
  Events_Loop* aLoop = Events_Loop::loop();
  static const Events_ID kCreatedEvent = aLoop->eventByName(EVENT_OBJECT_CREATED);
  static const Events_ID kUpdatedEvent = aLoop->eventByName(EVENT_OBJECT_UPDATED);
  static const Events_ID kRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const Events_ID kDeletedEvent = aLoop->eventByName(EVENT_OBJECT_DELETED);
  aLoop->flush(kCreatedEvent);
  aLoop->flush(kUpdatedEvent);
  aLoop->flush(kRedispEvent);
  aLoop->flush(kDeletedEvent);

  if (isNestedClosed) {
    if (myDoc->CommitCommand())
      myTransactions.rbegin()->myOCAFNum++;
  }

  // this must be here just after everything is finished but before real transaction stop
  // to avoid messages about modifications outside of the transaction
  // and to rebuild everything after all updates and creates
  if (isRoot()) { // once for root document
    static std::shared_ptr<Events_Message> aFinishMsg
      (new Events_Message(Events_Loop::eventByName("FinishOperation")));
    Events_Loop::loop()->send(aFinishMsg);
  }

  // for open of document with primitive box inside (finish transaction in initAttributes)
  bool aWasActivatedFlushes = aLoop->activateFlushes(true);
  while(aLoop->hasGrouppedEvent(kCreatedEvent) || aLoop->hasGrouppedEvent(kUpdatedEvent) ||
        aLoop->hasGrouppedEvent(kRedispEvent) || aLoop->hasGrouppedEvent(kDeletedEvent)) {
    aLoop->flush(kCreatedEvent);
    aLoop->flush(kUpdatedEvent);
    aLoop->flush(kRedispEvent);
    aLoop->flush(kDeletedEvent);
  }
  aLoop->activateFlushes(aWasActivatedFlushes);

  // to avoid "updated" message appearance by updater
  //aLoop->clear(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));

  // finish for all subs first: to avoid nested finishing and "isOperation" calls problems inside
  bool aResult = false;
  const std::set<int> aSubs = subDocuments();
  std::set<int>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++)
    if (subDoc(*aSubIter)->finishOperation())
      aResult = true;

  // transaction may be empty if this document was created during this transaction (create part)
  if (!myTransactions.empty() && myDoc->CommitCommand()) {
    // if commit is successful, just increment counters
    if (isEmptyTransaction(myDoc)) { // erase this transaction
      myDoc->Undo();
      myDoc->ClearRedos();
    } else {
      myTransactions.rbegin()->myOCAFNum++;
      aResult = true;
    }
  }

  if (isNestedClosed) {
    compactNested();
  }
  if (!aResult && !myTransactions.empty() /* it can be for just created part document */)
    aResult = myTransactions.rbegin()->myOCAFNum != 0;

  if (!aResult && isRoot()) {
    // nothing inside in all documents, so remove this transaction from the transactions list
    undoInternal(true, false);
  }
  // on finish clear redo in any case (issue 446) and for all subs (issue 408)
  myDoc->ClearRedos();
  myRedos.clear();
  for (aSubIter = aSubs.begin(); aSubIter != aSubs.end(); aSubIter++) {
    subDoc(*aSubIter)->myDoc->ClearRedos();
    subDoc(*aSubIter)->myRedos.clear();
  }

  return aResult;
}

/// Returns in theDelta labels that has been modified in the latest transaction of theDoc
static void modifiedLabels(const Handle(TDocStd_Document)& theDoc, TDF_LabelList& theDelta,
  const bool isRedo = false) {
  Handle(TDF_Delta) aDelta;
  if (isRedo)
    aDelta = theDoc->GetRedos().First();
  else
    aDelta = theDoc->GetUndos().Last();
  TDF_LabelList aDeltaList;
  aDelta->Labels(aDeltaList); // it clears list, so, use new one and then append to the result
  for(TDF_ListIteratorOfLabelList aListIter(aDeltaList); aListIter.More(); aListIter.Next()) {
    theDelta.Append(aListIter.Value());
  }
  // add also label of the modified attributes
  const TDF_AttributeDeltaList& anAttrs = aDelta->AttributeDeltas();
  /// named shape evolution also modifies integer on this label: exclude it
  TDF_LabelMap anExcludedInt;
  for (TDF_ListIteratorOfAttributeDeltaList anAttr(anAttrs); anAttr.More(); anAttr.Next()) {
    if (anAttr.Value()->Attribute()->ID() == TDataStd_BooleanArray::GetID()) {
      // Boolean array is used for feature auxiliary attributes only, feature args are not modified
      continue;
    }
    if (anAttr.Value()->Attribute()->ID() == TNaming_NamedShape::GetID()) {
      anExcludedInt.Add(anAttr.Value()->Label());
      // named shape evolution is changed in history update => skip them,
      // they are not the features arguments
      continue;
    }
    if (anAttr.Value()->Attribute()->ID() == TDataStd_Integer::GetID()) {
      if (anExcludedInt.Contains(anAttr.Value()->Label()))
        continue;
    }
      theDelta.Append(anAttr.Value()->Label());
  }
  TDF_ListIteratorOfLabelList aDeltaIter(theDelta);
  for(; aDeltaIter.More(); aDeltaIter.Next()) {
    if (anExcludedInt.Contains(aDeltaIter.Value())) {
      theDelta.Remove(aDeltaIter);
      if (!aDeltaIter.More())
        break;
    }
  }
}

void Model_Document::abortOperation()
{
  TDF_LabelList aDeltaLabels; // labels that are updated during "abort"
  if (!myNestedNum.empty() && !myDoc->HasOpenCommand()) {  // abort all what was done in nested
    compactNested();
    // store undo-delta here as undo actually does in the method later
    int a, aNumTransactions = myTransactions.rbegin()->myOCAFNum;
    for(a = 0; a < aNumTransactions; a++) {
      modifiedLabels(myDoc, aDeltaLabels);
      myDoc->Undo();
    }
    for(a = 0; a < aNumTransactions; a++) {
      myDoc->Redo();
    }

    undoInternal(false, false);
    myDoc->ClearRedos();
    myRedos.clear();
  } else { // abort the current
    int aNumTransactions = myTransactions.rbegin()->myOCAFNum;
    myTransactions.pop_back();
    if (!myNestedNum.empty())
      (*myNestedNum.rbegin())--;
    // roll back the needed number of transactions
    //myDoc->AbortCommand();
    // instead of abort, do commit and undo: to get the delta of modifications
    if (myDoc->CommitCommand())  {
      modifiedLabels(myDoc, aDeltaLabels);
      myDoc->Undo();
    }
    for(int a = 0; a < aNumTransactions; a++) {
      modifiedLabels(myDoc, aDeltaLabels);
      myDoc->Undo();
    }
    myDoc->ClearRedos();
  }
  // abort for all subs, flushes will be later, in the end of root abort
  const std::set<int> aSubs = subDocuments();
  std::set<int>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++)
    subDoc(*aSubIter)->abortOperation();
  // references may be changed because they are set in attributes on the fly
  myObjs->synchronizeFeatures(aDeltaLabels, true, false, false, isRoot());
}

bool Model_Document::isOperation() const
{
  // operation is opened for all documents: no need to check subs
  return myDoc->HasOpenCommand() == Standard_True ;
}

bool Model_Document::isModified()
{
  // is modified if at least one operation was committed and not undone
  return (int)myTransactions.size() != myTransactionSave || isOperation();
}

bool Model_Document::canUndo()
{
  // issue 406 : if transaction is opened, but nothing to undo behind, can not undo
  int aCurrentNum = isOperation() ? 1 : 0;
  if (myDoc->GetAvailableUndos() > 0 &&
      // there is something to undo in nested
      (myNestedNum.empty() || *myNestedNum.rbegin() - aCurrentNum > 0) &&
      myTransactions.size() - aCurrentNum > 0 /* for omitting the first useless transaction */)
    return true;
  // check other subs contains operation that can be undone
  const std::set<int> aSubs = subDocuments();
  std::set<int>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++) {
    std::shared_ptr<Model_Document> aSub = subDoc(*aSubIter);
    if (aSub->myObjs) {// if it was not closed before
      if (aSub->canUndo())
        return true;
    }
  }

  return false;
}

void Model_Document::undoInternal(const bool theWithSubs, const bool theSynchronize)
{
  if (myTransactions.empty())
    return;
  int aNumTransactions = myTransactions.rbegin()->myOCAFNum;
  myRedos.push_back(*myTransactions.rbegin());
  myTransactions.pop_back();
  if (!myNestedNum.empty())
    (*myNestedNum.rbegin())--;
  // roll back the needed number of transactions
  TDF_LabelList aDeltaLabels;
  for(int a = 0; a < aNumTransactions; a++) {
    if (theSynchronize)
      modifiedLabels(myDoc, aDeltaLabels);
    myDoc->Undo();
  }

  std::set<int> aSubs;
  if (theWithSubs) {
    // undo for all subs
    aSubs = subDocuments();
    std::set<int>::iterator aSubIter = aSubs.begin();
    for (; aSubIter != aSubs.end(); aSubIter++) {
      if (!subDoc(*aSubIter)->myObjs)
        continue;
      subDoc(*aSubIter)->undoInternal(theWithSubs, theSynchronize);
    }
  }
  // after undo of all sub-documents to avoid updates on not-modified data (issue 370)
  if (theSynchronize) {
    myObjs->synchronizeFeatures(aDeltaLabels, true, false, false, isRoot());
    // update the current features status
    setCurrentFeature(currentFeature(false), false);

    if (theWithSubs) {
      // undo for all subs
      const std::set<int> aNewSubs = subDocuments();
      std::set<int>::iterator aNewSubIter = aNewSubs.begin();
      for (; aNewSubIter != aNewSubs.end(); aNewSubIter++) {
        // synchronize only newly appeared documents
        if (!subDoc(*aNewSubIter)->myObjs || aSubs.find(*aNewSubIter) != aSubs.end())
          continue;
        TDF_LabelList anEmptyDeltas;
        subDoc(*aNewSubIter)->myObjs->synchronizeFeatures(anEmptyDeltas, true, false, true, true);
      }
    }
  }
}

void Model_Document::undo()
{
  undoInternal(true, true);
}

bool Model_Document::canRedo()
{
  if (!myRedos.empty())
    return true;
  // check other subs contains operation that can be redone
  const std::set<int> aSubs = subDocuments();
  std::set<int>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++) {
    if (!subDoc(*aSubIter)->myObjs)
      continue;
    if (subDoc(*aSubIter)->canRedo())
      return true;
  }
  return false;
}

void Model_Document::redo()
{
  if (!myNestedNum.empty())
    (*myNestedNum.rbegin())++;
  int aNumRedos = myRedos.rbegin()->myOCAFNum;
  myTransactions.push_back(*myRedos.rbegin());
  myRedos.pop_back();
  TDF_LabelList aDeltaLabels;
  for(int a = 0; a < aNumRedos; a++) {
    modifiedLabels(myDoc, aDeltaLabels, true);
    myDoc->Redo();
  }

  // redo for all subs
  const std::set<int> aSubs = subDocuments();
  std::set<int>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++)
    subDoc(*aSubIter)->redo();

  // after redo of all sub-documents to avoid updates on not-modified data (issue 370)
  myObjs->synchronizeFeatures(aDeltaLabels, true, false, false, isRoot());
  // update the current features status
  setCurrentFeature(currentFeature(false), false);
}
// this is used for creation of undo/redo1-list by GUI
// LCOV_EXCL_START
std::list<std::string> Model_Document::undoList() const
{
  std::list<std::string> aResult;
  // the number of skipped current operations (on undo they will be aborted)
  int aSkipCurrent = isOperation() ? 1 : 0;
  std::list<Transaction>::const_reverse_iterator aTrIter = myTransactions.crbegin();
  int aNumUndo = int(myTransactions.size());
  if (!myNestedNum.empty())
    aNumUndo = *myNestedNum.rbegin();
  for( ; aNumUndo > 0; aTrIter++, aNumUndo--) {
    if (aSkipCurrent == 0) aResult.push_back(aTrIter->myId);
    else aSkipCurrent--;
  }
  return aResult;
}

std::list<std::string> Model_Document::redoList() const
{
  std::list<std::string> aResult;
  std::list<Transaction>::const_reverse_iterator aTrIter = myRedos.crbegin();
  for( ; aTrIter != myRedos.crend(); aTrIter++) {
    aResult.push_back(aTrIter->myId);
  }
  return aResult;
}
// LCOV_EXCL_STOP

void Model_Document::operationId(const std::string& theId)
{
  myTransactions.rbegin()->myId = theId;
}

FeaturePtr Model_Document::addFeature(std::string theID, const bool theMakeCurrent)
{
  std::shared_ptr<Model_Session> aSession =
    std::dynamic_pointer_cast<Model_Session>(ModelAPI_Session::get());
  if (!aSession->hasModuleDocument() || !myObjs)
    return FeaturePtr(); // this may be on close of the document
  FeaturePtr aFeature = aSession->createFeature(theID, this);
  if (!aFeature)
    return aFeature;
  aFeature->init();
  Model_Document* aDocToAdd;
  if (!aFeature->documentToAdd().empty()) { // use the customized document to add
    if (aFeature->documentToAdd() != kind()) { // the root document by default
      aDocToAdd = std::dynamic_pointer_cast<Model_Document>(aSession->moduleDocument()).get();
    } else {
      aDocToAdd = this;
    }
  } else { // if customized is not presented, add to "this" document
    aDocToAdd = this;
  }
  if (aFeature) {
    // searching for feature after which must be added the next feature: this is the current feature
    // but also all sub-features of this feature
    FeaturePtr aCurrent = aDocToAdd->currentFeature(false);
    bool isModified = true;
    for(CompositeFeaturePtr aComp = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aCurrent);
        aComp.get() && isModified;
        aComp = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aCurrent)) {
      isModified =  false;
      int aSubs = aComp->numberOfSubs(false);
      for(int a = 0; a < aSubs; a++) {
        FeaturePtr aSub = aComp->subFeature(a, false);
        if (aSub && myObjs->isLater(aSub, aCurrent)) {
          isModified =  true;
          aCurrent = aSub;
        }
      }
    }
    // #2861,3029: if the parameter is added, add it after parameters existing in the list
    if (aCurrent.get() &&
      (aFeature->getKind() == "Parameter" || aFeature->getKind() == "ParametersMgr")) {
      int anIndex = kUNDEFINED_FEATURE_INDEX;
      for(FeaturePtr aNextFeat = myObjs->nextFeature(aCurrent, anIndex);
        aNextFeat.get() && aNextFeat->getKind() == "Parameter";
        aNextFeat = myObjs->nextFeature(aCurrent, anIndex))
        aCurrent = aNextFeat;
    }
    aDocToAdd->myObjs->addFeature(aFeature, aCurrent);
    if (!aFeature->isAction()) {  // do not add action to the data model
      if (theMakeCurrent)  // after all this feature stays in the document, so make it current
        aDocToAdd->setCurrentFeature(aFeature, false);
    } else { // feature must be executed
       // no creation event => updater not working, problem with remove part
      aFeature->execute();
    }
  }
  return aFeature;
}

void Model_Document::refsToFeature(FeaturePtr theFeature,
  std::set<std::shared_ptr<ModelAPI_Feature> >& theRefs, const bool isSendError)
{
  myObjs->refsToFeature(theFeature, theRefs, isSendError);
}

void Model_Document::removeFeature(FeaturePtr theFeature)
{
  myObjs->removeFeature(theFeature);
  // fix for #2723: send signal that part is updated
  if (!isRoot() && isOperation()) {
    std::shared_ptr<Model_Document> aRoot =
      std::dynamic_pointer_cast<Model_Document>(ModelAPI_Session::get()->moduleDocument());
    std::list<ResultPtr> allParts;
    aRoot->objects()->allResults(ModelAPI_ResultPart::group(), allParts);
    std::list<ResultPtr>::iterator aParts = allParts.begin();
    for(; aParts != allParts.end(); aParts++) {
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aParts);
      if (aPart->partDoc().get() == this) {
        static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
        ModelAPI_EventCreator::get()->sendUpdated(aRoot->feature(aPart), anEvent);
        break;
      }
    }
  }
}

// recursive function to check if theSub is a child of theMain composite feature
// through all the hierarchy of parents
static bool isSub(const CompositeFeaturePtr theMain, const FeaturePtr theSub) {
  CompositeFeaturePtr aParent = ModelAPI_Tools::compositeOwner(theSub);
  if (!aParent.get())
    return false;
  if (aParent == theMain)
    return true;
  return isSub(theMain, aParent);
}

void Model_Document::moveFeature(FeaturePtr theMoved, FeaturePtr theAfterThis, const bool theSplit)
{
  bool aCurrentUp = theMoved == currentFeature(false);
  if (aCurrentUp) {
    setCurrentFeatureUp();
  }
  // if user adds after high-level feature with nested,
  // add it after all nested (otherwise the nested will be disabled)
  CompositeFeaturePtr aCompositeAfter =
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theAfterThis);
  FeaturePtr anAfterThisSub = theAfterThis;
  if (aCompositeAfter.get()) {
    FeaturePtr aSub = aCompositeAfter;
    int anIndex = kUNDEFINED_FEATURE_INDEX;
    do {
      FeaturePtr aNext = myObjs->nextFeature(aSub, anIndex);
      if (!isSub(aCompositeAfter, aNext)) {
        anAfterThisSub = aSub;
        break;
      }
      aSub = aNext;
    } while (aSub.get());
  }

  AttributeSelectionListPtr aMovedList;
  if (theMoved->getKind() == "Group") {
    aMovedList = theMoved->selectionList("group_list");
    if (aMovedList.get())
      aMovedList->setMakeCopy(true);
  }
  myObjs->moveFeature(theMoved, anAfterThisSub);

  if (theSplit) { // split the group into sub-features
    theMoved->customAction("split");
  }

  if (aCurrentUp) { // make the moved feature enabled or disabled due to the real status
    setCurrentFeature(currentFeature(false), false);
  } else if (theAfterThis == currentFeature(false) || anAfterThisSub == currentFeature(false)) {
    // must be after move to make enabled all features which are before theMoved
    setCurrentFeature(theMoved, true);
  }
  if (aMovedList.get())
    aMovedList->setMakeCopy(false);
}

void Model_Document::updateHistory(const std::shared_ptr<ModelAPI_Object> theObject)
{
  if (myObjs)
    myObjs->updateHistory(theObject);
}

void Model_Document::updateHistory(const std::string theGroup)
{
  if (myObjs)
    myObjs->updateHistory(theGroup);
}

const std::set<int> Model_Document::subDocuments() const
{
  std::set<int> aResult;
  std::list<ResultPtr> aPartResults;
  myObjs->allResults(ModelAPI_ResultPart::group(), aPartResults);
  std::list<ResultPtr>::iterator aPartRes = aPartResults.begin();
  for(; aPartRes != aPartResults.end(); aPartRes++) {
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aPartRes);
    if (aPart && aPart->isActivated()) {
      aResult.insert(aPart->original()->partDoc()->id());
    }
  }
  return aResult;
}

std::shared_ptr<Model_Document> Model_Document::subDoc(int theDocID)
{
  // just store sub-document identifier here to manage it later
  return std::dynamic_pointer_cast<Model_Document>(
    Model_Application::getApplication()->document(theDocID));
}

ObjectPtr Model_Document::object(const std::string& theGroupID,
                                 const int theIndex,
                                 const bool theAllowFolder)
{
  return myObjs->object(theGroupID, theIndex, theAllowFolder);
}

std::shared_ptr<ModelAPI_Object> Model_Document::objectByName(
    const std::string& theGroupID, const std::wstring& theName)
{
  return myObjs->objectByName(theGroupID, theName);
}

const int Model_Document::index(std::shared_ptr<ModelAPI_Object> theObject,
                                const bool theAllowFolder)
{
  return myObjs->index(theObject, theAllowFolder);
}

int Model_Document::size(const std::string& theGroupID, const bool theAllowFolder)
{
  if (myObjs == 0) // may be on close
    return 0;
  return myObjs->size(theGroupID, theAllowFolder);
}

std::shared_ptr<ModelAPI_Object> Model_Document::parent(
  const std::shared_ptr<ModelAPI_Object> theChild)
{
  if(myObjs == 0) // may be on close
    return ObjectPtr();
  return myObjs->parent(theChild);
}

std::shared_ptr<ModelAPI_Feature> Model_Document::currentFeature(const bool theVisible)
{
  if (!myObjs) // on close document feature destruction it may call this method
    return std::shared_ptr<ModelAPI_Feature>();
  TDF_Label aRefLab = generalLabel().FindChild(TAG_CURRENT_FEATURE);
  Handle(TDF_Reference) aRef;
  if (aRefLab.FindAttribute(TDF_Reference::GetID(), aRef)) {
    TDF_Label aLab = aRef->Get();
    FeaturePtr aResult = myObjs->feature(aLab);
    if (theVisible) { // get nearest visible (in history) going up
      int anIndex = kUNDEFINED_FEATURE_INDEX;
      while(aResult.get() &&  !aResult->isInHistory()) {
        aResult = myObjs->nextFeature(aResult, anIndex, true);
      }
    }
    return aResult;
  }
  return std::shared_ptr<ModelAPI_Feature>(); // null feature means the higher than first
}

void Model_Document::setCurrentFeature(
  std::shared_ptr<ModelAPI_Feature> theCurrent, const bool theVisible)
{
  if (myIsSetCurrentFeature)
    return;
  myIsSetCurrentFeature = true;
  // blocks the flush signals to avoid each objects visualization in the viewer
  // they should not be shown once after all modifications are performed
  Events_Loop* aLoop = Events_Loop::loop();
  bool isActive = aLoop->activateFlushes(false);

  TDF_Label aRefLab = generalLabel().FindChild(TAG_CURRENT_FEATURE);
  CompositeFeaturePtr aMain; // main feature that may nest the new current
  std::set<FeaturePtr> anOwners; // composites that contain theCurrent (with any level of nesting)
  if (theCurrent.get()) {
    aMain = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theCurrent);
    CompositeFeaturePtr anOwner = ModelAPI_Tools::compositeOwner(theCurrent);
    while(anOwner.get()) {
      if (!aMain.get()) {
        aMain = anOwner;
      }
      anOwners.insert(anOwner);
      anOwner = ModelAPI_Tools::compositeOwner(anOwner);
    }
  }

  if (theVisible && !theCurrent.get()) {
    // needed to avoid disabling of PartSet initial constructions
    int anIndex = kUNDEFINED_FEATURE_INDEX;
    FeaturePtr aNext =
      theCurrent.get() ? myObjs->nextFeature(theCurrent, anIndex, false) : myObjs->firstFeature();
    for (; aNext.get(); aNext = myObjs->nextFeature(theCurrent, anIndex, false)) {
      if (aNext->isInHistory()) {
        break; // next in history is not needed
      } else { // next not in history is good for making current
        theCurrent = aNext;
      }
    }
  }
  if (theVisible) { // make RemoveResults feature be active even it is performed after the current
    int anIndex = kUNDEFINED_FEATURE_INDEX;
    FeaturePtr aNext =
      theCurrent.get() ? myObjs->nextFeature(theCurrent, anIndex, false) : myObjs->firstFeature();
    for (; aNext.get(); aNext = myObjs->nextFeature(theCurrent, anIndex, false)) {
      if (aNext->isInHistory()) {
        break; // next in history is not needed
      } else if (aNext->getKind() == "RemoveResults"){
        theCurrent = aNext;
      }
    }
  }
  if (theCurrent.get()) {
    std::shared_ptr<Model_Data> aData = std::static_pointer_cast<Model_Data>(theCurrent->data());
    if (!aData.get() || !aData->isValid()) {
      aLoop->activateFlushes(isActive);
      myIsSetCurrentFeature = false;
      return;
    }
    TDF_Label aFeatureLabel = aData->label().Father();

    Handle(TDF_Reference) aRef;
    if (aRefLab.FindAttribute(TDF_Reference::GetID(), aRef)) {
      aRef->Set(aFeatureLabel);
    } else {
      aRef = TDF_Reference::Set(aRefLab, aFeatureLabel);
    }
  } else { // remove reference for the null feature
    aRefLab.ForgetAttribute(TDF_Reference::GetID());
  }
  // make all features after this feature disabled in reversed order
  // (to remove results without dependencies)
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);

  bool aPassed = false; // flag that the current object is already passed in cycle
  FeaturePtr anIter = myObjs->lastFeature();
  bool aWasChanged = false;
  bool isCurrentParameter = theCurrent.get() && theCurrent->getKind() == "Parameter";
  int anIndex = kUNDEFINED_FEATURE_INDEX;
  for(; anIter.get(); anIter = myObjs->nextFeature(anIter, anIndex, true)) {
    // check this before passed become enabled: the current feature is enabled!
    if (anIter == theCurrent) aPassed = true;

    bool aDisabledFlag = !aPassed;
    if (aMain.get()) {
      if (isSub(aMain, anIter)) // sub-elements of not-disabled feature are not disabled
        aDisabledFlag = false;
      else if (anOwners.find(anIter) != anOwners.end())
        // disable the higher-level feature if the nested is the current
        if (aMain->getKind() != "Import") // exception for the import XAO feature with Group (2430)
          aDisabledFlag = true;
    }

    if (anIter->getKind() == "Parameter") {
      // parameters are always out of the history of features, but not parameters
      // due to the issue 1491 all parameters are kept enabled any time
      //if (!isCurrentParameter)
        aDisabledFlag = false;
    } else if (isCurrentParameter) {
      // if parameter is active, all other features become enabled (issue 1307)
      aDisabledFlag = false;
    }

    if (anIter->setDisabled(aDisabledFlag)) {
      static Events_ID anUpdateEvent = aLoop->eventByName(EVENT_OBJECT_UPDATED);
      // state of feature is changed => so inform that it must be updated if it has such state
      if (!aDisabledFlag &&
          (anIter->data()->execState() == ModelAPI_StateMustBeUpdated ||
           anIter->data()->execState() == ModelAPI_StateInvalidArgument))
        ModelAPI_EventCreator::get()->sendUpdated(anIter, anUpdateEvent);
      // flush is in the end of this method
      ModelAPI_EventCreator::get()->sendUpdated(anIter, aRedispEvent /*, false*/);
      aWasChanged = true;
    }
    // update for everyone concealment flag immediately: on edit feature in the middle of history
    if (aWasChanged) {
      std::list<ResultPtr> aResults;
      ModelAPI_Tools::allResults(anIter, aResults);
      std::list<ResultPtr>::const_iterator aRes = aResults.begin();
      for(; aRes != aResults.end(); aRes++) {
        if ((*aRes).get() && (*aRes)->data()->isValid() && !(*aRes)->isDisabled())
          std::dynamic_pointer_cast<Model_Data>((*aRes)->data())->updateConcealmentFlag();
      }
      // update the concealment status for display in isConcealed of ResultBody
      for(aRes = aResults.begin(); aRes != aResults.end(); aRes++) {
        if ((*aRes).get() && (*aRes)->data()->isValid() && !(*aRes)->isDisabled())
          (*aRes)->isConcealed();
      }
    }
  }
  myIsSetCurrentFeature = false;
  // unblock  the flush signals and up them after this
  aLoop->activateFlushes(isActive);

  static Events_ID kUpdatedSel = aLoop->eventByName(EVENT_UPDATE_SELECTION);
  aLoop->flush(kUpdatedSel);
}

void Model_Document::setCurrentFeatureUp()
{
  // on remove just go up for minimum step: highlight external objects in sketch causes
  // problems if it is true: here and in "setCurrentFeature"
  FeaturePtr aCurrent = currentFeature(false);
  if (aCurrent.get()) { // if not, do nothing because null is the upper
    int anIndex = kUNDEFINED_FEATURE_INDEX;
    FeaturePtr aPrev = myObjs->nextFeature(aCurrent, anIndex, true);
    // make the higher level composite as current (sketch becomes disabled if line is enabled)
    if (aPrev.get()) {
      FeaturePtr aComp = ModelAPI_Tools::compositeOwner(aPrev);
      // without cycle (issue 1555): otherwise extrusion fuse
      // will be enabled and displayed when inside sketch
      if (aComp.get())
          aPrev = aComp;
    }
    // do not flush: it is called only on remove, it will be flushed in the end of transaction
    setCurrentFeature(aPrev, false);
  }
}

TDF_Label Model_Document::generalLabel() const
{
  return myDoc->Main().FindChild(TAG_GENERAL);
}

std::shared_ptr<ModelAPI_ResultConstruction> Model_Document::createConstruction(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  return myObjs->createConstruction(theFeatureData, theIndex);
}

std::shared_ptr<ModelAPI_ResultBody> Model_Document::createBody(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  return myObjs->createBody(theFeatureData, theIndex);
}

std::shared_ptr<ModelAPI_ResultPart> Model_Document::createPart(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  return myObjs->createPart(theFeatureData, theIndex);
}

std::shared_ptr<ModelAPI_ResultPart> Model_Document::copyPart(
      const std::shared_ptr<ModelAPI_ResultPart>& theOrigin,
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  return myObjs->copyPart(theOrigin, theFeatureData, theIndex);
}

std::shared_ptr<ModelAPI_ResultGroup> Model_Document::createGroup(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  return myObjs->createGroup(theFeatureData, theIndex);
}

std::shared_ptr<ModelAPI_ResultField> Model_Document::createField(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  return myObjs->createField(theFeatureData, theIndex);
}

std::shared_ptr<ModelAPI_ResultParameter> Model_Document::createParameter(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  return myObjs->createParameter(theFeatureData, theIndex);
}

std::shared_ptr<ModelAPI_Folder> Model_Document::addFolder(
    std::shared_ptr<ModelAPI_Feature> theAddBefore)
{
  return myObjs->createFolder(theAddBefore);
}

void Model_Document::removeFolder(std::shared_ptr<ModelAPI_Folder> theFolder)
{
  if (theFolder)
    myObjs->removeFolder(theFolder);
}

std::shared_ptr<ModelAPI_Folder> Model_Document::findFolderAbove(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures)
{
  return myObjs->findFolder(theFeatures, false);
}

std::shared_ptr<ModelAPI_Folder> Model_Document::findFolderBelow(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures)
{
  return myObjs->findFolder(theFeatures, true);
}

std::shared_ptr<ModelAPI_Folder> Model_Document::findContainingFolder(
      const std::shared_ptr<ModelAPI_Feature>& theFeature,
      int& theIndexInFolder)
{
  return myObjs->findContainingFolder(theFeature, theIndexInFolder);
}

bool Model_Document::moveToFolder(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
      const std::shared_ptr<ModelAPI_Folder>& theFolder)
{
  return myObjs->moveToFolder(theFeatures, theFolder);
}

bool Model_Document::removeFromFolder(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
      const bool theBefore)
{
  return myObjs->removeFromFolder(theFeatures, theBefore);
}

std::shared_ptr<ModelAPI_Feature> Model_Document::feature(
    const std::shared_ptr<ModelAPI_Result>& theResult)
{
  if (myObjs == 0) // may be on close
    return std::shared_ptr<ModelAPI_Feature>();
  return myObjs->feature(theResult);
}

FeaturePtr Model_Document::featureByLab(const TDF_Label& theLab) {
  TDF_Label aCurrentLab = theLab;
  while(aCurrentLab.Depth() > 3)
    aCurrentLab = aCurrentLab.Father();
  return myObjs->feature(aCurrentLab);
}

ResultPtr Model_Document::resultByLab(const TDF_Label& theLab)
{
  TDF_Label aCurrentLab = theLab;
  while(aCurrentLab.Depth() > 3) {
    ObjectPtr aResultObj = myObjs->object(aCurrentLab);
    if (aResultObj.get()) {
      return std::dynamic_pointer_cast<ModelAPI_Result>(aResultObj); // this may be null if feature
    }
    aCurrentLab = aCurrentLab.Father();
  }
  return ResultPtr(); // not found
}

void Model_Document::addNamingName(const TDF_Label theLabel, std::wstring theName)
{
  std::map<std::wstring, std::list<TDF_Label> >::iterator aFind = myNamingNames.find(theName);

  if (aFind != myNamingNames.end()) { // to avoid duplicate-labels
    // to keep correct order in spite of history line management
    std::list<TDF_Label>::iterator anAddAfterThis = aFind->second.end();
    FeaturePtr anAddedFeature = featureByLab(theLabel);
    std::list<TDF_Label>::iterator aLabIter = aFind->second.begin();
    while(aLabIter != aFind->second.end()) {
      if (theLabel.IsEqual(*aLabIter)) {
        std::list<TDF_Label>::iterator aTmpIter = aLabIter;
        aLabIter++;
        aFind->second.erase(aTmpIter);
      } else {
        FeaturePtr aCurFeature = featureByLab(*aLabIter);
        if (aCurFeature.get() && anAddedFeature.get() &&
            myObjs->isLater(anAddedFeature, aCurFeature))
          anAddAfterThis = aLabIter;

        aLabIter++;
      }
    }
    if (anAddAfterThis != aFind->second.end()) {
      anAddAfterThis++;
      if (anAddAfterThis != aFind->second.end()) {
        myNamingNames[theName].insert(anAddAfterThis, theLabel); // inserts before anAddAfterThis
        return;
      }
    }
  }
  myNamingNames[theName].push_back(theLabel);
}

void Model_Document::changeNamingName(const std::wstring theOldName,
                                      const std::wstring theNewName,
                                      const TDF_Label& theLabel)
{
  std::map<std::wstring, std::list<TDF_Label> >::iterator aFind = myNamingNames.find(theOldName);
  if (aFind != myNamingNames.end()) {
    std::list<TDF_Label>::iterator aLabIter = aFind->second.begin();
    for(; aLabIter != aFind->second.end(); aLabIter++) {
      if (theLabel.IsEqual(*aLabIter)) { // found the label
        myNamingNames[theNewName].push_back(theLabel);
        if (aFind->second.size() == 1) { // only one element, so, just change the name
          myNamingNames.erase(theOldName);
        } else { // remove from the list
          aFind->second.erase(aLabIter);
        }
        // check the sketch vertex name located under renamed sketch line
        TDF_ChildIDIterator aChild(theLabel, TDataStd_Name::GetID());
        for(; aChild.More(); aChild.Next()) {
          Handle(TDataStd_Name) aSubName = Handle(TDataStd_Name)::DownCast(aChild.Value());
          std::wstring aName = Locale::Convert::toWString(aSubName->Get().ToExtString());
          if (aName.find(theOldName) == 0) { // started from parent name
            std::wstring aNewSubName = theNewName + aName.substr(theOldName.size());
            changeNamingName(aName, aNewSubName, aSubName->Label());
            aSubName->Set(aNewSubName.c_str());
          }
        }
        return;
      }
    }
  }
}

// returns true if names consist of the same sub-elements but with different order.
// Sub-elements are separated by "-" symbol. First part must be "Face", second at the same place.
static bool IsExchangedName(const TCollection_ExtendedString& theName1,
                            const TCollection_ExtendedString& theName2)
{
  static const TCollection_ExtendedString aSepStr("-");
  static const Standard_ExtString aSep = aSepStr.ToExtString();
  static const TCollection_ExtendedString aWireTail("_wire");
  if (theName1.Token(aSep, 1) != "Face" || theName2.Token(aSep, 1) != "Face")
    return false;
  if (theName1.Token(aSep, 2) != theName2.Token(aSep, 2))
    return false;
  // Collect Map of the sub-elements of the first name
  NCollection_Map<TCollection_ExtendedString> aSubsMap;
  TCollection_ExtendedString aWireSuffix;
  int a = 3;
  for (; true ; a++) {
    TCollection_ExtendedString aToken = theName1.Token(aSep, a);
    if (aToken.IsEmpty())
      break;
    int aTailPos = aToken.Search(aWireTail);
    if (aTailPos > 0) {
      aWireSuffix = aToken.Split(aTailPos - 1);
    }
    aSubsMap.Add(aToken);
  }
  // check all subs in the second name are in the map
  for (int a2 = 3; true; a2++) {
    TCollection_ExtendedString aToken = theName2.Token(aSep, a2);
    if (aToken.IsEmpty()) {
      if (a2 != a) // number of sub-elements is not equal
        return false;
      break;
    }
    int aTailPos = aToken.Search(aWireTail);
    if (aTailPos > 0) {
      TCollection_ExtendedString aSuffix = aToken.Split(aTailPos - 1);
      if (aWireSuffix != aSuffix)
        return false;
    }
    if (!aSubsMap.Contains(aToken))
      return false;
  }
  return true;
}

TDF_Label Model_Document::findNamingName(std::wstring theName, ResultPtr theContext)
{
  std::map<std::wstring, std::list<TDF_Label> >::iterator aFind = myNamingNames.find(theName);
  if (aFind != myNamingNames.end()) {
      std::list<TDF_Label>::reverse_iterator aLabIter = aFind->second.rbegin();
      for(; aLabIter != aFind->second.rend(); aLabIter++) {
        if (theContext.get()) {
          // context is defined and not like this, so, skip
          if (theContext == myObjs->object(aLabIter->Father()))
            return *aLabIter;
        }
      }
      return *(aFind->second.rbegin()); // no more variants, so, return the last
  }
  // not found exact name, try to find by sub-components
  std::wstring::size_type aSlash = theName.rfind(L'/');
  if (aSlash != std::wstring::npos) {
    std::wstring anObjName = theName.substr(0, aSlash);
    aFind = myNamingNames.find(anObjName);
    if (aFind != myNamingNames.end()) {
      TCollection_ExtendedString aSubName(theName.substr(aSlash + 1).c_str());
      // iterate all possible same-named labels starting from the last one (the recent)
      std::list<TDF_Label>::reverse_iterator aLabIter = aFind->second.rbegin();
      for(; aLabIter != aFind->second.rend(); aLabIter++) {
        if (theContext.get()) {
          // context is defined and not like this, so, skip
          if (theContext != myObjs->object(aLabIter->Father()))
            continue;
        }
        // copy aSubName to avoid incorrect further processing after its suffix cutting
        TCollection_ExtendedString aSubNameCopy(aSubName);
        TDF_Label aFaceLabelWithExchangedSubs; // check also exchanged sub-elements of the name
        // searching sub-labels with this name
        TDF_ChildIDIterator aNamesIter(*aLabIter, TDataStd_Name::GetID(), Standard_True);
        for(; aNamesIter.More(); aNamesIter.Next()) {
          Handle(TDataStd_Name) aName = Handle(TDataStd_Name)::DownCast(aNamesIter.Value());
          if (aName->Get() == aSubNameCopy)
            return aName->Label();
          if (aName->Get().Length() == aSubNameCopy.Length() &&
              IsExchangedName(aName->Get(),  aSubNameCopy))
            aFaceLabelWithExchangedSubs = aName->Label();
        }
        if (!aFaceLabelWithExchangedSubs.IsNull())
          return aFaceLabelWithExchangedSubs;
        // If not found child label with the exact sub-name, then try to find compound with
        // such sub-name without suffix.
        Standard_Integer aSuffixPos = aSubNameCopy.SearchFromEnd('_');
        if (aSuffixPos != -1 && aSuffixPos != aSubNameCopy.Length()) {
          TCollection_ExtendedString anIndexStr = aSubNameCopy.Split(aSuffixPos);
          aSubNameCopy.Remove(aSuffixPos);
          aNamesIter.Initialize(*aLabIter, TDataStd_Name::GetID(), Standard_True);
          for(; aNamesIter.More(); aNamesIter.Next()) {
            Handle(TDataStd_Name) aName = Handle(TDataStd_Name)::DownCast(aNamesIter.Value());
            if (aName->Get() == aSubNameCopy) {
              return aName->Label();
            }
          }
          // check also "this" label
          Handle(TDataStd_Name) aName;
          if (aLabIter->FindAttribute(TDataStd_Name::GetID(), aName)) {
            if (aName->Get() == aSubNameCopy) {
              return aName->Label();
            }
          }
        }
      }
      // verify context's name is same as sub-component's and use context's label
      if (aSubName.IsEqual(anObjName.c_str()))
        return *(aFind->second.rbegin());
    }
  }
  return TDF_Label(); // not found
}

bool Model_Document::isLaterByDep(FeaturePtr theThis, FeaturePtr theOther) {
  // check dependencies first: if theOther depends on theThis, theThis is not later
  std::list<std::pair<std::string, std::list<std::shared_ptr<ModelAPI_Object> > > > aRefs;
  theOther->data()->referencesToObjects(aRefs);
  std::list<std::pair<std::string, std::list<std::shared_ptr<ModelAPI_Object> > > >::iterator
    aRefIt = aRefs.begin();
  for(; aRefIt != aRefs.end(); aRefIt++) {
    std::list<ObjectPtr>::iterator aRefObjIt = aRefIt->second.begin();
    for(; aRefObjIt != aRefIt->second.end(); aRefObjIt++) {
      ObjectPtr aRefObj = *aRefObjIt;
      if (aRefObj.get()) {
        FeaturePtr aRefFeat = std::dynamic_pointer_cast<ModelAPI_Feature>(aRefObj);
        if (!aRefFeat.get()) { // take feature of the result
          aRefFeat = feature(std::dynamic_pointer_cast<ModelAPI_Result>(aRefObj));
        }
        if (aRefFeat.get()) {
          if (aRefFeat == theThis)
            return false; // other references to this, so other later than this
          //if (std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aRefFeat)) {
          //  if (!isLaterByDep(theThis, aRefFeat)) // nested composites: recursion
          //    return false;
          //}
        }
      }
    }
  }
  FeaturePtr aThisOwner = ModelAPI_Tools::compositeOwner(theThis);
  if (aThisOwner.get()) {
    if (aThisOwner == theOther)
      return true; // composite owner is later that its sub
    if (!isLaterByDep(aThisOwner, theOther))
      return false;
  }
  return myObjs->isLater(theThis, theOther);
}

int Model_Document::numberOfNameInHistory(
  const ObjectPtr& theNameObject, const TDF_Label& theStartFrom)
{
  std::map<std::wstring, std::list<TDF_Label> >::iterator aFind =
    myNamingNames.find(theNameObject->data()->name());
  if (aFind == myNamingNames.end() || aFind->second.size() < 2) {
    return 1; // no need to specify the name by additional identifiers
  }
  // get the feature of the object for relative compare
  FeaturePtr aStart = myObjs->feature(theStartFrom);
  if (!aStart.get()) // strange, but can not find feature by the label
    return 1;
  // feature that contain result with this name
  FeaturePtr aNameFeature;
  ResultPtr aNameResult = std::dynamic_pointer_cast<ModelAPI_Result>(theNameObject);
  if (aNameResult)
    aNameFeature = myObjs->feature(aNameResult);
  else
    aNameFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theNameObject);
  // iterate all labels with this name to find the nearest just before or equal relative
  std::list<TDF_Label>::reverse_iterator aLabIter = aFind->second.rbegin();
  for(; aLabIter != aFind->second.rend(); aLabIter++) {
    FeaturePtr aLabFeat = featureByLab(*aLabIter);
    if (!aLabFeat.get())
      continue;
    if (isLaterByDep(aStart, aLabFeat)) // skip also start: its result don't used
      break;
  }
  int aResIndex = 1;
  for(; aLabIter != aFind->second.rend(); aLabIter++) {
    FeaturePtr aLabFeat = featureByLab(*aLabIter);
    if (!aLabFeat.get())
      continue;
    if (aLabFeat == aNameFeature || isLaterByDep(aNameFeature, aLabFeat))
      return aResIndex;
    aResIndex++;
  }
  return aResIndex; // strange
}

ResultPtr Model_Document::findByName(
  std::wstring& theName, std::wstring& theSubShapeName, bool& theUniqueContext)
{
  int aNumInHistory = 0;
  std::wstring aName = theName;
  ResultPtr aRes = myObjs->findByName(aName);
  theUniqueContext = !(aRes.get() && myNamingNames.find(aName) != myNamingNames.end());
  while(!aRes.get() && aName[0] == '_') { // this may be theContext with the history index
    aNumInHistory++;
    aName = aName.substr(1);
    aRes = myObjs->findByName(aName);
  }
  if (aNumInHistory) {
    std::map<std::wstring, std::list<TDF_Label> >::iterator aFind = myNamingNames.find(aName);
    if (aFind != myNamingNames.end() && (int)aFind->second.size() > aNumInHistory) {
      std::list<TDF_Label>::reverse_iterator aLibIt = aFind->second.rbegin();
      for(; aNumInHistory != 0; aNumInHistory--)
        aLibIt++;
      const TDF_Label& aResultLab = *aLibIt;
      aRes = std::dynamic_pointer_cast<ModelAPI_Result>(myObjs->object(aResultLab.Father()));
      if (aRes) { // modify the incoming names
        if (!theSubShapeName.empty())
          theSubShapeName = theSubShapeName.substr(theName.size() - aName.size());
        theName = aName;
      }
    }
  }
  return aRes;
}

std::list<std::shared_ptr<ModelAPI_Feature> > Model_Document::allFeatures()
{
  return myObjs->allFeatures();
}

std::list<std::shared_ptr<ModelAPI_Object> > Model_Document::allObjects()
{
  return myObjs->allObjects();
}

void Model_Document::setActive(const bool theFlag)
{
  if (theFlag != myIsActive) {
    myIsActive = theFlag;
    // redisplay all the objects of this part
    static Events_Loop* aLoop = Events_Loop::loop();
    static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);

    for(int a = size(ModelAPI_Feature::group()) - 1; a >= 0; a--) {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(
        object(ModelAPI_Feature::group(), a));
      if (aFeature.get() && aFeature->data()->isValid()) {
        std::list<ResultPtr> aResults;
        ModelAPI_Tools::allResults(aFeature, aResults);
        for (std::list<ResultPtr>::iterator aRes = aResults.begin();
                                                aRes != aResults.end(); aRes++) {
          ModelAPI_EventCreator::get()->sendUpdated(*aRes, aRedispEvent);
        }
      }
    }
  }
}

bool Model_Document::isActive() const
{
  return myIsActive;
}

int Model_Document::transactionID()
{
  Handle(TDataStd_Integer) anIndex;
  if (!generalLabel().FindChild(TAG_CURRENT_TRANSACTION).
      FindAttribute(TDataStd_Integer::GetID(), anIndex)) {
    anIndex = TDataStd_Integer::Set(generalLabel().FindChild(TAG_CURRENT_TRANSACTION), 1);
  }
  return anIndex->Get();
}

void Model_Document::incrementTransactionID()
{
  int aNewVal = transactionID() + 1;
  TDataStd_Integer::Set(generalLabel().FindChild(TAG_CURRENT_TRANSACTION), aNewVal);
}

TDF_Label Model_Document::extConstructionsLabel() const
{
  return myDoc->Main().FindChild(TAG_EXTERNAL_CONSTRUCTIONS);
}

bool Model_Document::isOpened()
{
  return myObjs && !myDoc.IsNull();
}

int Model_Document::numInternalFeatures()
{
  return myObjs->numInternalFeatures();
}

std::shared_ptr<ModelAPI_Feature> Model_Document::internalFeature(const int theIndex)
{
  return myObjs->internalFeature(theIndex);
}

void Model_Document::synchronizeTransactions()
{
  Model_Document* aRoot =
    std::dynamic_pointer_cast<Model_Document>(ModelAPI_Session::get()->moduleDocument()).get();
  if (aRoot == this)
    return; // don't need to synchronize root with root

  std::shared_ptr<Model_Session> aSession =
    std::dynamic_pointer_cast<Model_Session>(Model_Session::get());
  while(myRedos.size() > aRoot->myRedos.size()) { // remove redo in this
    aSession->setCheckTransactions(false);
    redo();
    aSession->setCheckTransactions(true);
  }
  /* this case can not be reproduced in any known case for the current moment, so, just comment
  while(myRedos.size() < aRoot->myRedos.size()) { // add more redo in this
    undoInternal(false, true);
  }*/
}

/// Feature that is used for selection in the Part document by the external request
class Model_SelectionInPartFeature : public ModelAPI_Feature {
public:
  /// Nothing to do in constructor
  Model_SelectionInPartFeature() : ModelAPI_Feature() {}

  /// Returns the unique kind of a feature
  virtual const std::string& getKind() {
    static std::string MY_KIND("InternalSelectionInPartFeature");
    return MY_KIND;
  }
  /// Request for initialization of data model of the object: adding all attributes
  virtual void initAttributes() {
    data()->addAttribute("selection", ModelAPI_AttributeSelectionList::typeId());
  }
  /// Nothing to do in the execution function
  virtual void execute() {}

};

//! Returns the feature that is used for calculation of selection externally from the document
AttributeSelectionListPtr Model_Document::selectionInPartFeature()
{
  // return already created, otherwise create
  if (!mySelectionFeature.get() || !mySelectionFeature->data()->isValid()) {
    // create a new one
    mySelectionFeature = FeaturePtr(new Model_SelectionInPartFeature);

    TDF_Label aFeatureLab = generalLabel().FindChild(TAG_SELECTION_FEATURE);
    std::shared_ptr<Model_Data> aData(new Model_Data);
    aData->setLabel(aFeatureLab.FindChild(1));
    aData->setObject(mySelectionFeature);
    mySelectionFeature->setDoc(myObjs->owner());
    mySelectionFeature->setData(aData);
    std::wstring aName = id() + L"_Part";
    mySelectionFeature->data()->setName(aName);
    mySelectionFeature->setDoc(myObjs->owner());
    mySelectionFeature->initAttributes();
    mySelectionFeature->init(); // to make it enabled and Update correctly
    // this update may cause recomputation of the part after selection on it, that is not needed
    mySelectionFeature->data()->blockSendAttributeUpdated(true);
  }
  return mySelectionFeature->selectionList("selection");
}

FeaturePtr Model_Document::lastFeature()
{
  if (myObjs)
    return myObjs->lastFeature();
  return FeaturePtr();
}

static Handle(TNaming_NamedShape) searchForOriginalShape(TopoDS_Shape theShape, TDF_Label aMain) {
  Handle(TNaming_NamedShape) aResult;
  while(!theShape.IsNull()) { // searching for the very initial shape that produces this one
    TopoDS_Shape aShape = theShape;
    theShape.Nullify();
    // to avoid crash of TNaming_SameShapeIterator if pure shape does not exists
    if (!TNaming_Tool::HasLabel(aMain, aShape))
      break;
    for(TNaming_SameShapeIterator anIter(aShape, aMain); anIter.More(); anIter.Next()) {
      TDF_Label aNSLab = anIter.Label();
      Handle(TNaming_NamedShape) aNS;
      if (aNSLab.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
        for(TNaming_Iterator aShapesIter(aNS); aShapesIter.More(); aShapesIter.Next()) {
          if (aShapesIter.Evolution() == TNaming_SELECTED ||
              aShapesIter.Evolution() == TNaming_DELETE)
            continue; // don't use the selection evolution
          if (aShapesIter.NewShape().IsSame(aShape)) { // found the original shape
            aResult = aNS;
            if (aResult->Evolution() == TNaming_MODIFY)
              theShape = aShapesIter.OldShape();
            // otherwise may me searching for another item of this shape with longer history
            if (!theShape.IsNull())
              break;
          }
        }
      }
    }
  }
  return aResult;
}

std::shared_ptr<ModelAPI_Feature> Model_Document::producedByFeature(
    std::shared_ptr<ModelAPI_Result> theResult,
    const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
  if (!aBody.get()) {
    return feature(theResult); // for not-body just returns the feature that produced this result
  }
  // otherwise get the shape and search the very initial label for it
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return FeaturePtr();

  // for compsolids and compounds all the naming is located in the main object, so, try to use
  // it first
  ResultBodyPtr aMain = ModelAPI_Tools::bodyOwner(theResult);
  while (aMain.get()) { // get the top-most main
    ResultBodyPtr aNextMain = ModelAPI_Tools::bodyOwner(aMain);
    if (aNextMain.get())
      aMain = aNextMain;
    else break;
  }
  if (aMain.get()) {
    FeaturePtr aMainRes = producedByFeature(aMain, theShape);
    if (aMainRes)
      return aMainRes;
  }

  std::shared_ptr<Model_Data> aBodyData = std::dynamic_pointer_cast<Model_Data>(theResult->data());
  if (!aBodyData.get() || !aBodyData->isValid())
    return FeaturePtr();

  TopoDS_Shape anOldShape; // old shape in the pair old shape->theShape in the named shape
  TopoDS_Shape aShapeContainer; // old shape of the shape that contains aShape as sub-element
  Handle(TNaming_NamedShape) aCandidatInThis, aCandidatContainer;
  TDF_Label aBodyLab = aBodyData->shapeLab();
  // use child and this label (the lowest priority)
  TDF_ChildIDIterator aNSIter(aBodyLab, TNaming_NamedShape::GetID(), Standard_True);
  bool aUseThis = !aNSIter.More();
  while(anOldShape.IsNull() && (aNSIter.More() || aUseThis)) {
    Handle(TNaming_NamedShape) aNS;
    if (aUseThis) {
      if (!aBodyLab.FindAttribute(TNaming_NamedShape::GetID(), aNS))
        break;
    } else {
      aNS = Handle(TNaming_NamedShape)::DownCast(aNSIter.Value());
    }
    for(TNaming_Iterator aShapesIter(aNS); aShapesIter.More(); aShapesIter.Next()) {
      if (aShapesIter.Evolution() == TNaming_SELECTED || aShapesIter.Evolution() == TNaming_DELETE)
        continue; // don't use the selection evolution
      if (aShapesIter.NewShape().IsSame(aShape)) { // found the original shape
        aCandidatInThis = aNS;
        if (aCandidatInThis->Evolution() == TNaming_MODIFY)
          anOldShape = aShapesIter.OldShape();
        // otherwise may me searching for another item of this shape with longer history
        if (!anOldShape.IsNull())
          break;
      }
      // check that the shape contains aShape as sub-shape to fill container
      if (aShapesIter.NewShape().ShapeType() < aShape.ShapeType() && aCandidatContainer.IsNull()) {
        TopExp_Explorer anExp(aShapesIter.NewShape(), aShape.ShapeType());
        for(; anExp.More(); anExp.Next()) {
          if (aShape.IsSame(anExp.Current())) {
            aCandidatContainer = aNS;
            aShapeContainer = aShapesIter.NewShape();
          }
        }
      }
    }
    // iterate to the next label or to the body label in the end
    if (!aUseThis)
      aNSIter.Next();
    if (!aNSIter.More()) {
      if (aUseThis)
        break;
      aUseThis = true;
    }
  }
  if (aCandidatInThis.IsNull()) {
    // to fix 1512: searching for original shape of this shape
    // if modification of it is not in this result
    aCandidatInThis = searchForOriginalShape(aShape, myDoc->Main());
    if (aCandidatInThis.IsNull()) {
      if (aCandidatContainer.IsNull())
        return FeaturePtr();
      // with the lower priority use the higher level shape that contains aShape
      aCandidatInThis = aCandidatContainer;
      anOldShape = aShapeContainer;
    } else {
      // to stop the searching by the following searchForOriginalShape
      anOldShape.Nullify();
    }
  }

  Handle(TNaming_NamedShape) aNS = searchForOriginalShape(anOldShape, myDoc->Main());
  if (!aNS.IsNull())
    aCandidatInThis = aNS;

  FeaturePtr aResult;
  TDF_Label aResultLab = aCandidatInThis->Label();
  while(aResultLab.Depth() > 3)
    aResultLab = aResultLab.Father();
  FeaturePtr aFeature = myObjs->feature(aResultLab);
  if (aFeature.get()) {
    if (!aResult.get() || myObjs->isLater(aResult, aFeature)) {
      aResult = aFeature;
    }
  }
  return aResult;
}

bool Model_Document::isLater(FeaturePtr theLater, FeaturePtr theCurrent) const
{
  return myObjs->isLater(theLater, theCurrent);
}

// Object Browser nodes states
// LCOV_EXCL_START
void Model_Document::storeNodesState(const std::list<bool>& theStates)
{
  TDF_Label aLab = generalLabel().FindChild(TAG_NODES_STATE);
  aLab.ForgetAllAttributes();
  if (!theStates.empty()) {
    Handle(TDataStd_BooleanArray) anArray =
      TDataStd_BooleanArray::Set(aLab, 0, int(theStates.size()) - 1);
    std::list<bool>::const_iterator aState = theStates.begin();
    for(int anIndex = 0; aState != theStates.end(); aState++, anIndex++) {
      anArray->SetValue(anIndex, *aState);
    }
  }
}

void Model_Document::restoreNodesState(std::list<bool>& theStates) const
{
  TDF_Label aLab = generalLabel().FindChild(TAG_NODES_STATE);
  Handle(TDataStd_BooleanArray) anArray;
  if (aLab.FindAttribute(TDataStd_BooleanArray::GetID(), anArray)) {
    int anUpper = anArray->Upper();
    for(int anIndex = 0; anIndex <= anUpper; anIndex++) {
      theStates.push_back(anArray->Value(anIndex) == Standard_True);
    }
  }
}
// LCOV_EXCL_STOP

void Model_Document::eraseAllFeatures()
{
  if (myObjs)
    myObjs->eraseAllFeatures();
}

std::shared_ptr<ModelAPI_Feature> Model_Document::nextFeature(
  std::shared_ptr<ModelAPI_Feature> theCurrent, const bool theReverse) const
{
  if (theCurrent.get() && myObjs) {
    int anIndex = kUNDEFINED_FEATURE_INDEX;
    return myObjs->nextFeature(theCurrent, anIndex, theReverse);
  }
  return FeaturePtr(); // nothing by default
}

void Model_Document::setExecuteFeatures(const bool theFlag)
{
  myExecuteFeatures = theFlag;
  const std::set<int> aSubs = subDocuments();
  std::set<int>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++) {
    if (!subDoc(*aSubIter)->myObjs)
      continue;
    subDoc(*aSubIter)->setExecuteFeatures(theFlag);
  }
}

void Model_Document::appendTransactionToPrevious()
{
  Transaction anAppended =  myTransactions.back();
  myTransactions.pop_back();
  if (!myNestedNum.empty())
    (*myNestedNum.rbegin())--;
  if (!myTransactions.empty()) { // if it is empty, just forget the appended
    myTransactions.back().myOCAFNum += anAppended.myOCAFNum;
  }
  // propagate the same action to sub-documents
  const std::set<int> aSubs = subDocuments();
  for (std::set<int>::iterator aSubIter = aSubs.begin(); aSubIter != aSubs.end(); aSubIter++) {
    subDoc(*aSubIter)->appendTransactionToPrevious();
  }
}

/// GUID for keeping information about the auto-recomputation state
static const Standard_GUID kAutoRecomputationID("8493fb74-0674-4912-a100-1cf46c7cfab3");

void Model_Document::setAutoRecomutationState(const bool theState)
{
  if (theState)
    generalLabel().FindChild(TAG_CURRENT_TRANSACTION).ForgetAttribute(kAutoRecomputationID);
  else
    TDataStd_UAttribute::Set(
      generalLabel().FindChild(TAG_CURRENT_TRANSACTION), kAutoRecomputationID);
}

bool Model_Document::autoRecomutationState() const
{
  return !generalLabel().FindChild(TAG_CURRENT_TRANSACTION).IsAttribute(kAutoRecomputationID);
}
