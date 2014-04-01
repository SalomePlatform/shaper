// File:        PartSetPlugin_NewPart.hxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef PartSetPlugin_NewPart_HeaderFile
#define PartSetPlugin_NewPart_HeaderFile

#include "PartSetPlugin.h"
#include <ModelAPI_Feature.h>

/**\class PartSetPlugin_NewPart
 * \ingroup DataModel
 * \brief Feature for creation of the new part in PartSet.
 */

class PartSetPlugin_NewPart: public ModelAPI_Feature
{
public:
  /// Returns the kind of a feature
  PARTSETPLUGIN_EXPORT virtual std::string GetKind() {return "new_part";}

  /// Use plugin manager for features creation
  PartSetPlugin_NewPart();
};

#endif
