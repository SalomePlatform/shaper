// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSetPlugin_Duplicate.h
// Created:     20 May 2014
// Author:      Mikhail PONIKAROV

#ifndef PartSetPlugin_Duplicate_H_
#define PartSetPlugin_Duplicate_H_

#include "PartSetPlugin_Part.h"

/**\class PartSetPlugin_Duplicate
 * \ingroup DataModel
 * \brief Duplicates the active part (not root). Creates a new "part" feature.
 */
class PartSetPlugin_Duplicate : public PartSetPlugin_Part
{
 public:
    /// Duplicate kind
  inline static const std::string& ID()
  {
    static const std::string MY_DUPLICATE_KIND("Duplicate");
    return MY_DUPLICATE_KIND;
  }

  /// Makes a new part, copy of active
  PartSetPlugin_Duplicate();

  /// Request for initialization of data model of the feature: adding all attributes
  PARTSETPLUGIN_EXPORT virtual void initAttributes();

  /// doesn't call creation of new document, just does nothing if document was not copied
  PARTSETPLUGIN_EXPORT virtual void execute();
};

#endif
