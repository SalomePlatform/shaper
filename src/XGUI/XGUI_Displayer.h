#ifndef XGUI_Displayer_H
#define XGUI_Displayer_H

#include "XGUI.h"

#include <QString>
#include <boost/shared_ptr.hpp>

#include <TopoDS_Shape.hxx>

class XGUI_Viewer;
class ModelAPI_Feature;

/**\class XGUI_Displayer
 * \ingroup GUI
 * \brief Displayer. Provides mechanizm of displa/erase of objects in viewer
 */
class XGUI_EXPORT XGUI_Displayer
{
public:
  XGUI_Displayer(XGUI_Viewer* theViewer);
  virtual ~XGUI_Displayer();

  void Display(boost::shared_ptr<ModelAPI_Feature> theFeature);

  void Display(boost::shared_ptr<ModelAPI_Feature> theFeature, const TopoDS_Shape& theShape);

protected:
  XGUI_Viewer* myViewer; ///< the viewer
};

#endif
