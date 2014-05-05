// File:        PartSet_OperationSketchBase.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationSketchBase_H
#define PartSet_OperationSketchBase_H

#include "PartSet.h"

#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_List.hxx>

#include <ModuleBase_Operation.h>
#include <ModuleBase_Operation.h>
#include <QObject>

class GeomAPI_Shape;

/*!
  \class PartSet_OperationSketchBase
  * \brief The base operation for the sketch features.
  *  Base class for all sketch operations. It provides an access to the feature preview
*/
class PARTSET_EXPORT PartSet_OperationSketchBase : public ModuleBase_Operation
{
  Q_OBJECT
public:
  enum FeatureActionMode { FM_Activation, FM_Deactivation, FM_Abort };

public:
  /// Constructor
  /// \param theId an feature index
  /// \param theParent the object parent
  PartSet_OperationSketchBase(const QString& theId, QObject* theParent);
  /// Destructor
  virtual ~PartSet_OperationSketchBase();

  /// Returns the feature preview shape
  /// \param theFeature the feature object to obtain the preview
  boost::shared_ptr<GeomAPI_Shape> preview(boost::shared_ptr<ModelAPI_Feature> theFeature) const;

  /// Returns the operation local selection mode
  /// \param theFeature the feature object to get the selection mode
  /// \return the selection mode
  virtual std::list<int> getSelectionModes(boost::shared_ptr<ModelAPI_Feature> theFeature) const = 0;

  /// Gives the current selected objects to be processed by the operation
  /// \param theList a list of interactive selected shapes
  virtual void setSelectedShapes(const NCollection_List<TopoDS_Shape>& theList) {};

  /// Processes the mouse release in the point
  /// \param thePoint a point clicked in the viewer
  virtual void mouseReleased(const gp_Pnt& thePoint) {};

  /// Processes the mouse move in the point
  /// \param thePoint a 3D point clicked in the viewer
  virtual void mouseMoved(const gp_Pnt& thePoint) {};

  /// Processes the key pressed in the view
  /// \param theKey a key value
  virtual void keyReleased(const int theKey) {};

signals:
  /// Signal about the feature construing is finished
  /// \param theFeature the result feature
  /// \param theMode the mode of the feature modification
  void featureConstructed(boost::shared_ptr<ModelAPI_Feature> theFeature,
                          int theMode);

public:
  /// temporary code to provide edition mode
  void setEditMode(const bool isEditMode) { myIsEditMode = isEditMode; };
protected:
  bool isEditMode() const { return myIsEditMode; }
private:
  bool myIsEditMode;
};

#endif
