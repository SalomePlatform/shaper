// File:        PartSet_OperationSketchBase.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationSketchBase_H
#define PartSet_OperationSketchBase_H

#include "PartSet.h"

#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

#include <ModuleBase_PropPanelOperation.h>
#include <QObject>

/*!
  \class PartSet_OperationSketchBase
  * \brief The base operation for the sketch features.
  *  Base class for all sketch operations. It provides an access to the feature preview
*/
class PARTSET_EXPORT PartSet_OperationSketchBase : public ModuleBase_PropPanelOperation
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theId an feature index
  /// \param theParent the object parent
  PartSet_OperationSketchBase(const QString& theId, QObject* theParent);
  /// Destructor
  virtual ~PartSet_OperationSketchBase();

  /// Returns the feature preview shape
  const TopoDS_Shape& preview() const;

  /// Returns the operation local selection mode
  /// \return the selection mode
  virtual int getSelectionMode() const = 0;

  /// Gives the current selected objects to be processed by the operation
  /// \param a list of interactive selected objects
  virtual void setSelectedShapes(const NCollection_List<TopoDS_Shape>& theList) = 0;

signals:
  void viewerProjectionChange(double theX, double theY, double theZ);
};

#endif
