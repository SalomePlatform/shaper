#include "XGUI_Displayer.h"
#include "XGUI_Tools.h"
#include "XGUI_Viewer.h"

#include <ModelAPI_Document.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>

/*!
 \brief Constructor
 */
XGUI_Displayer::XGUI_Displayer(XGUI_Viewer* theViewer)
: myViewer(theViewer)
{
}

/*!
 \brief Destructor
 */
XGUI_Displayer::~XGUI_Displayer()
{
}

/*!
 * Display the feature
 * \param theFeature a feature instance
 */
void XGUI_Displayer::Display(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
}

/*!
 * Display the feature and a shape. This shape would be associated to the given feature
 * \param theFeature a feature instance
 * \param theFeature a shape
 */
void XGUI_Displayer::Display(boost::shared_ptr<ModelAPI_Feature> theFeature,
                             const TopoDS_Shape& theShape)
{
  Handle(AIS_InteractiveContext) aContext = myViewer->AISContext();

  Handle(AIS_Shape) anAIS = new AIS_Shape(theShape);
  aContext->Display(anAIS, Standard_False);

  aContext->UpdateCurrentViewer();
}
