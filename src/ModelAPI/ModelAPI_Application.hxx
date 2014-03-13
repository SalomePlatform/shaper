// File:        Model_Application.hxx
// Created:     28 Dec 2011
// Author:      Mikhail PONIKAROV
// Copyright:   CEA 2011

#ifndef ModelAPI_Application_HeaderFile
#define ModelAPI_Application_HeaderFile

#include "ModelAPI.hxx"
class ModelAPI_Document;

/**\class Model_Application
 * \ingroup DataModel
 * \brief Realization of Open CASCADE application abstraction. Class for internal use only.
 * Application supports the formats and document management. It is uses OCAF-lke
 * architecture and just implements specific features of the module.
 */
class ModelAPI_Application
{
public: // useful methods inside of the module
  //! Retuns the application: one per process    
  MODELAPI_EXPORT virtual ModelAPI_Application GetApplication() = 0;

  MODELAPI_EXPORT virtual ModelAPI_Document* GetMainDocument() = 0;
};

#endif
