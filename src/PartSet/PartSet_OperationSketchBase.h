// File:        PartSet_OperationSketchBase.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationSketchBase_H
#define PartSet_OperationSketchBase_H

#include "PartSet.h"

#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_List.hxx>

#include <ModuleBase_PropPanelOperation.h>
#include <QObject>

class GeomAPI_Shape;

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
  boost::shared_ptr<GeomAPI_Shape> preview() const;

  /// Returns the operation local selection mode
  /// \return the selection mode
  virtual int getSelectionMode() const = 0;

  /// Gives the current selected objects to be processed by the operation
  /// \param theList a list of interactive selected shapes
  virtual void setSelectedShapes(const NCollection_List<TopoDS_Shape>& theList) {};

  /// Processes the mouse release in the point
  /// \param thePoint a point clicked in the viewer
  virtual void mouseReleased(const gp_Pnt& thePoint) {};

  /// Processes the mouse move in the point
  /// \param thePoint a 3D point clicked in the viewer
  virtual void mouseMoved(const gp_Pnt& thePoint) {};

  /// temporary code to provide edition mode
  void setEditMode(const bool isEditMode) { myIsEditMode = isEditMode; };
protected:
  bool isEditMode() const { return myIsEditMode; }
private:
  bool myIsEditMode;
};

#endif
