// File:        Model_Application.hxx
// Created:     28 Dec 2011
// Author:      Mikhail PONIKAROV
// Copyright:   CEA 2011

#ifndef Model_Application_HeaderFile
#define Model_Application_HeaderFile

#include <Model_Document.hxx> 
#include <ModelAPI_Application.hxx> 
#include <TDocStd_Application.hxx>

// Define handle class 
DEFINE_STANDARD_HANDLE(Model_Application,TDocStd_Application)

  /**\class Model_Application
 * \ingroup DataModel
 * \brief Realization of Open CASCADE application abstraction. Class for internal use only.
 * Application supports the formats and document management. It is uses OCAF-lke
 * architecture and just implements specific features of the module.
 */
class Model_Application : public TDocStd_Application, public ModelAPI_Application
{
public: // useful methods inside of the module

  // CASCADE RTTI
  DEFINE_STANDARD_RTTI(Model_Application);

  //! Retuns the application: one per process    
  MODEL_EXPORT static Handle_Model_Application GetApplication();   
  //! Returns the main document (on first call creates it)
  MODEL_EXPORT ModelAPI_Document* GetMainDocument();

public: // Redefined OCAF methods
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

  Handle_Model_Document myMainDoc; ///< main document of an application
};

#endif
