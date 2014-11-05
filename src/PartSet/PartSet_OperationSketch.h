// File:        PartSet_OperationSketch.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationSketch_H
#define PartSet_OperationSketch_H

#include "PartSet.h"

#include <PartSet_OperationSketchBase.h>

#include <SketchPlugin_Sketch.h>

#include <QObject>
#include <QList>

class Handle_AIS_InteractiveObject;

/*!
 \class PartSet_OperationSketch
 * \brief The operation for the sketch feature creation
 */
class PARTSET_EXPORT PartSet_OperationSketch : public PartSet_OperationSketchBase
{
Q_OBJECT
 public:
  /// Returns the operation type key
  static std::string Type()
  {
    return SketchPlugin_Sketch::ID();
  }

 public:
  /// Constructor
  /// \param theId the feature identifier
  /// \param theParent the operation parent
  PartSet_OperationSketch(const QString& theId, QObject* theParent);
  /// Destructor
  virtual ~PartSet_OperationSketch();

  /// Returns True if the given operation is a Sketcher operation
  virtual bool isGranted(ModuleBase_Operation* theOperation) const;


  /// Returns the operation sketch feature
  /// \returns the sketch instance
  virtual CompositeFeaturePtr sketch() const;

  /// Processes the mouse pressed in the point
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void mousePressed(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer, ModuleBase_ISelection* theSelection);

  /// Processes the mouse release in the point
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void mouseReleased(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer,
                             ModuleBase_ISelection* theSelection);

  /// Gives the current mouse point in the viewer
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  virtual void mouseMoved(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer);

  /// Returns the list of the nested features
  /// \return the list of subfeatures
  virtual std::list<FeaturePtr> subFeatures() const;

  /// Virtual method called when operation stopped - committed or aborted.
  /// Emits a signal to hide the preview of the operation
  virtual void stopOperation();

  /// Returns whether the nested operations are enabled.
  /// The state can depend on the operation current state.
  /// It returns true after the sketch plane is choosen.
  /// \return enabled state
  virtual bool isNestedOperationsEnabled() const;

  /// Returns whether the sketch plane is set
  /// \return the boolean value whether the sketch is set
  bool hasSketchPlane() const;

  /// Set the plane to the current sketch
  /// \param theShape the shape
  /// \return selected direction
  boost::shared_ptr<GeomAPI_Dir> setSketchPlane(const TopoDS_Shape& theShape);
  
  /// Called on selection changed when the operation is active
  virtual void selectionChanged(ModuleBase_ISelection* theSelection);

  /// If operation needs to redisplay its result during operation
  /// then this method has to return True
  virtual bool hasPreview() const { return false; }

signals:
  /// signal about the sketch plane is selected
  /// \param theX the value in the X direction of the plane
  /// \param theX the value in the Y direction value of the plane
  /// \param theX the value in the Z direction of the plane
  void planeSelected(double theX, double theY, double theZ);
  // signal about the viewer fit all perform
  void fitAllView();

  /// Signal to define sketch mode
  void launchSketch();

 protected:
  /// Virtual method called when operation started (see start() method for more description)
  /// Default impl calls corresponding slot and commits immediately.
  virtual void startOperation();

  /// Virtual method called after operation committed (see commit() method for more description)
  virtual void afterCommitOperation();

 private:
  QList<ModuleBase_ViewerPrs> myFeatures;  ///< the features to apply the edit operation
};

#endif
