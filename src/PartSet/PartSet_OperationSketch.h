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
  /// Returns the operation type key
  static std::string Type() { return "Sketch"; }

public:
  /// Constructor
  /// \param theId the feature identifier
  /// \param theParent the operation parent
  PartSet_OperationSketch(const QString& theId, QObject* theParent);
  /// Destructor
  virtual ~PartSet_OperationSketch();

  /// Returns the operation local selection mode
  /// \param theFeature the feature object to get the selection mode
  /// \return the selection mode
  virtual std::list<int> getSelectionModes(boost::shared_ptr<ModelAPI_Feature> theFeature) const;

  /// Gives the current selected objects to be processed by the operation
  /// \param theFeature the selected feature
  /// \param theShape the selected shape
  virtual void setSelected(boost::shared_ptr<ModelAPI_Feature> theFeature,
                           const TopoDS_Shape& theShape);

signals:
  /// signal about the sketch plane is selected
  /// \param theX the value in the X direction of the plane
  /// \param theX the value in the Y direction value of the plane
  /// \param theX the value in the Z direction of the plane
  void planeSelected(double theX, double theY, double theZ);

protected:
  /// Set the plane to the current sketch
  /// \param theShape the shape
  void setSketchPlane(const TopoDS_Shape& theShape);

private:
  bool myIsEditMode; /// the edit mode of this operation
};

#endif
