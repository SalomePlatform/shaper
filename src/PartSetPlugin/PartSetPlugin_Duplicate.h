// File:        PartSetPlugin_Duplicate.h
// Created:     20 May 2014
// Author:      Mikhail PONIKAROV

#ifndef PartSetPlugin_Duplicate_HeaderFile
#define PartSetPlugin_Duplicate_HeaderFile

#include "PartSetPlugin_Part.h"

/**\class PartSetPlugin_Duplicate
 * \ingroup DataModel
 * \brief Duplicates the active part (not root). Creates a new "part" feature.
 */
class PartSetPlugin_Duplicate: public PartSetPlugin_Part
{
public:
  /// the reference to copy: reference to the attribute
  inline static const std::string& ORIGIN_REF()
  {
    static const std::string MY_DUPLICATE_ID("Origin");
    return MY_DUPLICATE_ID;
  }
  /// Makes a new part, copy of active
  PartSetPlugin_Duplicate();

  /// Request for initialization of data model of the feature: adding all attributes
  PARTSETPLUGIN_EXPORT virtual void initAttributes();

  /// doesn't call creation of new document, just does nothing if document was not copied
  PARTSETPLUGIN_EXPORT virtual void execute();
};

#endif
