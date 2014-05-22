// File:        PartSetPlugin_Duplicate.h
// Created:     20 May 2014
// Author:      Mikhail PONIKAROV

#ifndef PartSetPlugin_Duplicate_HeaderFile
#define PartSetPlugin_Duplicate_HeaderFile

#include "PartSetPlugin_Part.h"

/// the reference to copy: reference to the attribute
const std::string PART_DUPLICATE_ATTR_REF = "Origin";

/**\class PartSetPlugin_Duplicate
 * \ingroup DataModel
 * \brief Duplicates the active part (not root). Creates a new "part" feature.
 */
class PartSetPlugin_Duplicate: public PartSetPlugin_Part
{
public:
  /// Makes a new part, copy of active
  PartSetPlugin_Duplicate();

  /// Request for initialization of data model of the feature: adding all attributes
  PARTSETPLUGIN_EXPORT virtual void initAttributes();
};

#endif
