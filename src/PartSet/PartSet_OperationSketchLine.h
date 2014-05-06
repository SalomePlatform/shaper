// File:        PartSet_OperationSketchLine.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationSketchLine_H
#define PartSet_OperationSketchLine_H

#include "PartSet.h"

#include <PartSet_OperationSketchBase.h>
#include <QObject>

class QMouseEvent;

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
  virtual bool isGranted() const;

  /// Returns the operation local selection mode
  /// \param theFeature the feature object to get the selection mode
  /// \return the selection mode
  virtual std::list<int> getSelectionModes(boost::shared_ptr<ModelAPI_Feature> theFeature) const;

  /// Gives the current selected objects to be processed by the operation
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  virtual void mouseReleased(const gp_Pnt& thePoint, QMouseEvent* theEvent);
  /// Gives the current mouse point in the viewer
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  virtual void mouseMoved(const gp_Pnt& thePoint, QMouseEvent* theEvent);
  /// Processes the key pressed in the view
  /// \param theKey a key value
  virtual void keyReleased(const int theKey);

protected:
  /// \brief Virtual method called when operation is started
  /// Virtual method called when operation started (see start() method for more description)
  /// After the parent operation body perform, set sketch feature to the created line feature
  virtual void startOperation();

  /// \brief Virtual method called when operation is started
  /// Virtual method called when operation stopped - committed or aborted.
  /// After the parent operation body perform, reset selection point mode of the operation
  virtual void stopOperation();

  /// Creates an operation new feature
  /// In addition to the default realization it appends the created line feature to
  /// the sketch feature
  /// \returns the created feature
  virtual boost::shared_ptr<ModelAPI_Feature> createFeature();

protected:
  /// \brief Save the point to the line.
  /// \param thePoint the 3D point in the viewer
  /// \param theAttribute the start or end attribute of the line
  void setLinePoint(const gp_Pnt& thePoint, const std::string& theAttribute);

  /// \brief Set the point to the line by the point of the source line.
  /// \param theSourceFeature the feature, where the point is obtained
  /// \param theSourceAttribute the start or end attribute of the source line
  /// \param theAttribute the start or end attribute of the line
  void setLinePoint(boost::shared_ptr<ModelAPI_Feature> theSourceFeature,
                                               const std::string& theSourceAttribute,
                                               const std::string& theAttribute);

protected:
  ///< Structure to lists the possible types of point selection modes
  enum PointSelectionMode {SM_FirstPoint, SM_SecondPoint, SM_None};

private:
  boost::shared_ptr<ModelAPI_Feature> mySketch; ///< the sketch feature
  PointSelectionMode myPointSelectionMode; ///< point selection mode
};

#endif
