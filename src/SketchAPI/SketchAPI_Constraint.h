// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : SketchAPI_Constraint.h
// Purpose:
//
// History:
// 08/08/16 - Artem ZHIDKOV - Creation of the file

#ifndef SRC_SKETCHAPI_SKETCHAPI_CONSTRAINT_H_
#define SRC_SKETCHAPI_SKETCHAPI_CONSTRAINT_H_

#include "SketchAPI.h"

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
#include <ModelHighAPI_RefAttr.h>

#include <SketchPlugin_Constraint.h>

/**\class SketchAPI_Constraint
 * \ingroup CPPHighAPI
 * \brief Interface for Constraint feature
 */
class SketchAPI_Constraint : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Constraint(const std::shared_ptr<ModelAPI_Feature> & theFeature);

  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Constraint();

  static std::string ID()
  {
    static const std::string DUMMY;
    return DUMMY;
  }
  virtual std::string getID() { return ID(); }

  SKETCHAPI_EXPORT
  bool initialize();

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

#endif
