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
  PARTSETPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = "Part"; return MY_KIND;}

  /// Returns to which group in the document must be added feature
  PARTSETPLUGIN_EXPORT virtual const std::string& getGroup() 
  {static std::string MY_GROUP = "Parts"; return MY_GROUP;}

  /// Creates a new part document if needed
  PARTSETPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PARTSETPLUGIN_EXPORT virtual void initAttributes();

  PARTSETPLUGIN_EXPORT virtual boost::shared_ptr<ModelAPI_Document> documentToAdd();

  /// Use plugin manager for features creation
  PartSetPlugin_Part();
};

#endif
