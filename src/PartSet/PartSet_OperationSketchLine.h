// File:        PartSet_OperationSketchLine.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationSketchLine_H
#define PartSet_OperationSketchLine_H

#include "PartSet.h"

#include <PartSet_OperationSketchBase.h>
#include <QObject>

class GeomDataAPI_Point2D;
class QMouseEvent;
class QKeyEvent;

/*!
 \class PartSet_OperationSketchLine
 * \brief The operation for the sketch feature creation
*/
class PARTSET_EXPORT PartSet_OperationSketchLine : public PartSet_OperationSketchBase
{
  Q_OBJECT

public:
  /// Returns the operation type key
  static std::string Type() { return "SketchLine"; }

public:
  /// Constructor
  /// \param theId the feature identifier
  /// \param theParent the operation parent
  /// \param theFeature the parent feature
  PartSet_OperationSketchLine(const QString& theId, QObject* theParent,
                              boost::shared_ptr<ModelAPI_Feature> theSketchFeature);
  /// Destructor
  virtual ~PartSet_OperationSketchLine();

   /// Returns that this operator can be started above already running one.
   /// The runned operation should be the sketch feature modified operation
  /// \param theOperation the previous running operation
  virtual bool isGranted(ModuleBase_IOperation* theOperation) const;

  /// Returns the operation local selection mode
  /// \param theFeature the feature object to get the selection mode
  /// \return the selection mode
  virtual std::list<int> getSelectionModes(boost::shared_ptr<ModelAPI_Feature> theFeature) const;

  /// Initializes some fields accorging to the feature
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void init(boost::shared_ptr<ModelAPI_Feature> theFeature,
                    const std::list<XGUI_ViewerPrs>& theSelected,
                    const std::list<XGUI_ViewerPrs>& theHighlighted);

  /// Returns the operation sketch feature
  /// \returns the sketch instance
  virtual boost::shared_ptr<ModelAPI_Feature> sketch() const;

  /// Gives the current selected objects to be processed by the operation
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
 virtual void mouseReleased(QMouseEvent* theEvent, Handle_V3d_View theView,
                            const std::list<XGUI_ViewerPrs>& theSelected,
                            const std::list<XGUI_ViewerPrs>& theHighlighted);
  /// Gives the current mouse point in the viewer
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  virtual void mouseMoved(QMouseEvent* theEvent, Handle_V3d_View theView);
  /// Processes the key pressed in the view
  /// \param theKey a key value
  virtual void keyReleased(const int theKey);

  virtual void keyReleased(std::string theName, QKeyEvent* theEvent);

protected:
  /// \brief Virtual method called when operation is started
  /// Virtual method called when operation started (see start() method for more description)
  /// After the parent operation body perform, set sketch feature to the created line feature
  virtual void startOperation();

  /// Virtual method called when operation aborted (see abort() method for more description)
  /// Before the feature is aborted, it should be hidden from the viewer
  virtual void abortOperation();

  /// Virtual method called when operation stopped - committed or aborted.
  /// Restore the multi selection state
  virtual void stopOperation();

  /// Creates an operation new feature
  /// In addition to the default realization it appends the created line feature to
  /// the sketch feature
  /// \param theFlushMessage the flag whether the create message should be flushed
  /// \returns the created feature
  virtual boost::shared_ptr<ModelAPI_Feature> createFeature(const bool theFlushMessage = true);

  /// Creates a constraint on two points
  /// \param thePoint1 the first point
  /// \param thePoint1 the second point
  void createConstraint(boost::shared_ptr<GeomDataAPI_Point2D> thePoint1,
                        boost::shared_ptr<GeomDataAPI_Point2D> thePoint2);

  /// Creates constrains of the current 
  /// \param theX the horizontal coordnate of the point
  /// \param theY the vertical coordnate of the point
  void setConstraints(double theX, double theY);
protected:
  /// \brief Get the line point 2d coordinates.
  /// \param theFeature the line feature
  /// \param theAttribute the start or end attribute of the line
  /// \param theX the horizontal coordinate
  /// \param theY the vertical coordinate
  void getLinePoint(boost::shared_ptr<ModelAPI_Feature> theFeature, const std::string& theAttribute,
                    double& theX, double& theY);
  /// Find a point in the line with given coordinates
  /// \param theFeature the line feature
  /// \param theX the horizontal point coordinate
  /// \param theY the vertical point coordinate
  boost::shared_ptr<GeomDataAPI_Point2D> findLinePoint(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                                       double theX, double theY);

  /// \brief Save the point to the line.
  /// \param theFeature the line feature
  /// \param theX the horizontal coordinate
  /// \param theY the vertical coordinate
  /// \param theAttribute the start or end attribute of the line
  void setLinePoint(boost::shared_ptr<ModelAPI_Feature>, double theX, double theY,
                    const std::string& theAttribute);
  /// \brief Save the point to the line.
  /// \param thePoint the 3D point in the viewer
  /// \param theAttribute the start or end attribute of the line
  void setLinePoint(const gp_Pnt& thePoint, Handle(V3d_View) theView, const std::string& theAttribute);

protected:
  ///< Structure to lists the possible types of point selection modes
  enum PointSelectionMode {SM_FirstPoint, SM_SecondPoint, SM_DonePoint};

  ///< Set the point selection mode. Emit signal about focus change if necessary.
  /// \param theMode a new selection mode
  /// \param isToEmitSignal the neccessity to emit signal
  void setPointSelectionMode(const PointSelectionMode& theMode, const bool isToEmitSignal = true);

private:
  boost::shared_ptr<ModelAPI_Feature> mySketch; ///< the sketch feature
  boost::shared_ptr<GeomDataAPI_Point2D> myInitPoint; ///< the first line point
  PointSelectionMode myPointSelectionMode; ///< point selection mode
};

#endif
