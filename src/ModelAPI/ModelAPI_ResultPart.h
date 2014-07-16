// File:        ModelAPI_ResultPart.h
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_ResultPart_HeaderFile
#define ModelAPI_ResultPart_HeaderFile

#include "ModelAPI_Result.h"

/**\class ModelAPI_ResultPart
 * \ingroup DataModel
 * \brief The Part document, result of a creation of new part feature.
 *
 * This result leaves without feature: no parametricity for this element,
 * only add/remove, undo/redo.
 */
class ModelAPI_ResultPart : public ModelAPI_Result
{
public:
  /// Returns the group identifier of this result
  virtual std::string groupName()
    { return group(); }

  /// Returns the group identifier of this result
  static std::string group()
    {static std::string MY_GROUP = "Parts"; return MY_GROUP;}

  /// part document reference attribute
  inline static const std::string& DOC_REF()
  {
    static const std::string MY_DOC_REF("PartDocument");
    return MY_DOC_REF;
  }

  /// Returns the part-document of this result
  virtual boost::shared_ptr<ModelAPI_Document> partDoc() = 0;
};

//! Pointer on feature object
typedef boost::shared_ptr<ModelAPI_ResultPart> ResultPartPtr;

#endif
