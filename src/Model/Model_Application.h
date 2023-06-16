// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#ifndef Model_Application_H_
#define Model_Application_H_

#include <Model_Document.h>
#include <TDocStd_Application.hxx>
#include <map>

// Define handle class
DEFINE_STANDARD_HANDLE(Model_Application, TDocStd_Application)

/**\class Model_Application
 * \ingroup DataModel
 * \brief Realization of Open CASCADE application abstraction. Class for internal use only.
 *
 * Application supports the formats and document management. It is uses OCAF-lke
 * architecture and just implements specific features of the module.
 */
class Model_Application : public TDocStd_Application
{
public:
  // useful methods inside of the module

  // CASCADE RTTI
  /// Definition of "Handles" usage
  /// \param Model_Application is the class name that is covered by the OCCT Handle
  DEFINE_STANDARD_RTTIEXT(Model_Application, TDocStd_Application)

  //! Retuns the application: one per process
  MODEL_EXPORT static Handle(Model_Application) getApplication();
  //! Returns the document by the identifier
  //! \returns false of document is not yet created/not loaded
  MODEL_EXPORT std::shared_ptr<Model_Document> document(const int theDocID);
  //! Returns true if document has been created
  MODEL_EXPORT bool hasDocument(const int theDocID);
  //! Returns true if root document has been created
  MODEL_EXPORT bool hasRoot();
  //! Returns root document, if created (or null otherwise)
  MODEL_EXPORT std::shared_ptr<Model_Document> rootDocument();
  //! Deletes the document from the application
  MODEL_EXPORT void deleteDocument(const int theDocID);
  //! Deletes all documents existing in the application
  MODEL_EXPORT void deleteAllDocuments();
  //! Creates a new document an returns an id of this document
  //! \param theDocID if it is zero, the root document is created
  MODEL_EXPORT void createDocument(const int theDocID);
  //! Loads document by the file name (if it is registered as load by demand)
  //! \param theDocName name of the document file
  //! \param theDocID the identifier of the loaded document (to be created)
  //! \returns true if load is ok
  MODEL_EXPORT bool loadDocument(const std::wstring theDocName, const int theDocID);

  //! Set path for the loaded by demand documents
  void setLoadPath(std::string thePath);
  //! Returns the path for the loaded by demand documents
  const std::string& loadPath() const;
  //! Defines that specified document must be loaded by demand
  void setLoadByDemand(std::wstring theID, const int theDocID);
  //! Returns true if specified document must be loaded by demand
  bool isLoadByDemand(std::wstring theID, const int theDocIndex);

  //! produces new unique identifier of the document
  int generateDocumentId();

 public:
  // Redefined OCAF methods
  //! Return name of resource (i.e. "Standard")
  Standard_CString ResourcesName();
  //! Return format (i.e "MDTV-Standard")
  //! \param theFormats sequence of allowed formats for input/output
  virtual void Formats(TColStd_SequenceOfExtendedString& theFormats);
  //! Constructor
  //! Use method GetInstance() method to obtain
  //! the static instance of the object (or derive your own application)
  Model_Application();

 private:
  /// Map from identifiers to created documents of an application
  std::map<int, std::shared_ptr<Model_Document> > myDocs;
  /// Path for the loaded by demand documents
  std::string myPath;
  /// Path for the loaded by demand documents (and the persistent ID as the value)
  std::map<std::wstring, int> myLoadedByDemand;
};

#endif
