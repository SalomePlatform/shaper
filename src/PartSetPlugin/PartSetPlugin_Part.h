// File:        PartSetPlugin_Part.h
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef PartSetPlugin_Part_HeaderFile
#define PartSetPlugin_Part_HeaderFile

#include "PartSetPlugin.h"
#include <ModelAPI_Feature.h>

/// part reference attribute
const std::string PART_ATTR_DOC_REF = "PartDocument";

/**\class PartSetPlugin_Part
 * \ingroup DataModel
 * \brief Feature for creation of the new part in PartSet.
 */
class PartSetPlugin_Part: public ModelAPI_Feature
{
public:
  /// Returns the kind of a feature
  PARTSETPLUGIN_EXPORT virtual std::string getKind() {return "Part";}

  /// Creates a new part document if needed
  PARTSETPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PARTSETPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  PartSetPlugin_Part();
};

#endif
