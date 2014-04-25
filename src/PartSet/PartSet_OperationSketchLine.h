// File:        PartSet_OperationSketchLine.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationSketchLine_H
#define PartSet_OperationSketchLine_H

#include "PartSet.h"

#include <PartSet_OperationSketchBase.h>
#include <QObject>

/*!
 \class PartSet_OperationSketchLine
 * \brief The operation for the sketch feature creation
*/
class PARTSET_EXPORT PartSet_OperationSketchLine : public PartSet_OperationSketchBase
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theId the feature identifier
  /// \param theParent the operation parent
  /// \param theFeature the parent feature
  PartSet_OperationSketchLine(const QString& theId, QObject* theParent,
                              boost::shared_ptr<ModelAPI_Feature> theSketchFeature);
  /// Destructor
  virtual ~PartSet_OperationSketchLine();

  /// The sketch can not be created immediately, firstly a plane should be set
  virtual bool isPerformedImmediately() const;

  /// Returns the operation local selection mode
  /// \return the selection mode
  virtual int getSelectionMode() const;

  /// Gives the current selected objects to be processed by the operation
  /// \param theList a list of interactive selected shapes
  virtual void setSelectedShapes(const NCollection_List<TopoDS_Shape>& theList);

protected:
  /// \brief Virtual method called when operation is started
  /// Virtual method called when operation started (see start() method for more description)
  /// After the parent operation body perform, set sketch feature to the created line feature
  virtual void startOperation();

private:
  boost::shared_ptr<ModelAPI_Feature> mySketch; ///< the sketch feature
};

#endif
