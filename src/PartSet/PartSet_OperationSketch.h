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

  /// Returns the operation sketch feature
  /// \returns the sketch instance
  virtual boost::shared_ptr<ModelAPI_Feature> sketch() const;

  /// Processes the mouse pressed in the point
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  /// \param theSelected the list of selected presentations
  virtual void mousePressed(QMouseEvent* theEvent, Handle_V3d_View theView,
                            const std::list<XGUI_ViewerPrs>& theSelected);
  /// Processes the mouse release in the point
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  /// \param theSelected the list of selected presentations
  virtual void mouseReleased(QMouseEvent* theEvent, Handle_V3d_View theView,
                             const std::list<XGUI_ViewerPrs>& theSelected);
  /// Gives the current mouse point in the viewer
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  virtual void mouseMoved(QMouseEvent* theEvent, Handle_V3d_View theView);

  /// Returns the map of the operation previews including the nested feature previews
  /// \return the map of feature to the feature preview
  virtual std::map<boost::shared_ptr<ModelAPI_Feature>, boost::shared_ptr<GeomAPI_Shape> >
                                                                           preview() const;
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
  std::list<XGUI_ViewerPrs> myFeatures; ///< the features to apply the edit operation
};

#endif
