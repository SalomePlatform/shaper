// File:        PartSet_OperationSketch.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationSketch_H
#define PartSet_OperationSketch_H

#include "PartSet.h"

#include <PartSet_OperationSketchBase.h>
#include <QObject>

/*!
 \class PartSet_OperationSketch
 * \brief The operation for the sketch feature creation
*/
class PARTSET_EXPORT PartSet_OperationSketch : public PartSet_OperationSketchBase
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theId the feature identifier
  /// \param theParent the operation parent
  PartSet_OperationSketch(const QString& theId, QObject* theParent);
  /// Destructor
  virtual ~PartSet_OperationSketch();

  /// Returns the operation local selection mode
  /// \return the selection mode
  virtual int getSelectionMode() const;

  /// Gives the current selected objects to be processed by the operation
  /// \param theList a list of interactive selected shapes
  /// \param theSelectedPoint a point clidked in the viewer
  virtual void setSelectedShapes(const NCollection_List<TopoDS_Shape>& theList,
                                 const gp_Pnt& theSelectedPoint);
};

#endif
