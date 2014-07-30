// File:        Model_Application.hxx
// Created:     28 Dec 2011
// Author:      Mikhail PONIKAROV
// Copyright:   CEA 2011

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
 * Application supports the formats and document management. It is uses OCAF-lke
 * architecture and just implements specific features of the module.
 */
class Model_Application: public TDocStd_Application
{
public:
  // useful methods inside of the module

  // CASCADE RTTI
  DEFINE_STANDARD_RTTI(Model_Application);

  //! Retuns the application: one per process    
  MODEL_EXPORT static Handle_Model_Application getApplication();
  //! Returns the main document (on first call creates it) by the string identifier
  MODEL_EXPORT const boost::shared_ptr<Model_Document>& getDocument(std::string theDocID);
  //! Returns true if document has been created
  MODEL_EXPORT bool hasDocument(std::string theDocID);
  //! Deletes the document from the application
  MODEL_EXPORT void deleteDocument(std::string theDocID);

  //! Set path for the loaded by demand documents
  void setLoadPath(std::string thePath);
  //! Defines that specified document must be loaded by demand
  void setLoadByDemand(std::string theID);

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
  /// Map from string identifiers to created documents of an application
  std::map<std::string, boost::shared_ptr<Model_Document> > myDocs;
  /// Path for the loaded by demand documents
  std::string myPath;
  /// Path for the loaded by demand documents
  std::set<std::string> myLoadedByDemand;
};

#endif
