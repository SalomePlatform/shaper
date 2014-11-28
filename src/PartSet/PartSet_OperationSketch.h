// File:        PartSet_OperationSketch.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationSketch_H
#define PartSet_OperationSketch_H

#include "PartSet.h"

#include <ModuleBase_Operation.h>
#include <SketchPlugin_Sketch.h>

/*!
 \class PartSet_OperationSketch
 * \brief The operation for the sketch feature creation
 */
class PARTSET_EXPORT PartSet_OperationSketch : public ModuleBase_Operation
{
Q_OBJECT

 public:
  /// Returns the operation type key
  static std::string Type()
  {
    return SketchPlugin_Sketch::ID();
  }

  /// Constructor
  /// \param theId the feature identifier
  /// \param theParent the operation parent
  PartSet_OperationSketch(const QString& theId, QObject* theParent);
  /// Destructor
  virtual ~PartSet_OperationSketch();

  /// Returns whether the nested operations are enabled.
  /// The state can depend on the operation current state.
  /// It returns true after the sketch plane is choosen.
  /// \return enabled state
  virtual bool isNestedOperationsEnabled() const;

};

#endif
