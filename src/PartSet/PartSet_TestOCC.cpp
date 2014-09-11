// File:        PartSet_TestOCC.h
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_TestOCC.h>

#include <XGUI_Workshop.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_Displayer.h>
#include <XGUI_ViewerProxy.h>
#include <PartSet_Tools.h>
#include <PartSet_OperationSketchBase.h>

#include <ModuleBase_ViewerPrs.h>
#include <ModelAPI_Feature.h>

#include <SketchPlugin_Line.h>
#include <ModelAPI_Document.h>

static double myTestDelta;
static ResultPtr myTestObject;

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <V3d_View.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>

void PartSet_TestOCC::testSelection(XGUI_Workshop* theWorkshop)
{
  if (!myTestObject) {
    PartSet_TestOCC::createTestLine(theWorkshop);
    PartSet_TestOCC::moveMouse(theWorkshop->viewer()->AISContext(),
                               theWorkshop->viewer()->activeView());
    PartSet_TestOCC::changeTestLine(theWorkshop);
  }
  boost::shared_ptr<GeomAPI_AISObject> anIO = theWorkshop->displayer()->getAISObject(myTestObject);
  if (!anIO->empty()) {
    theWorkshop->viewer()->AISContext()->MoveTo(0, 0, theWorkshop->viewer()->activeView());
    theWorkshop->viewer()->AISContext()->Select(0, 0, 2500, 2500,
                                                theWorkshop->viewer()->activeView());
  }
}

void PartSet_TestOCC::local_selection_change_shape(Handle_AIS_InteractiveContext theContext,
                                                   Handle_V3d_View theView)
{
  // 1. Create shape
  gp_Pnt aPnt1(100, 100, 0);
  gp_Pnt aPnt2(150, 300, 0);
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aPnt1, aPnt2).Edge();

  // 2. Display shape and activate it in the local context
  Handle(AIS_Shape) anAIS = new AIS_Shape(anEdge);
  if (!theContext->HasOpenedContext()) {
    theContext->ClearCurrents(false);
    theContext->OpenLocalContext(false/*use displayed objects*/, true/*allow shape decomposition*/);
  }
  theContext->Display(anAIS, false);
  theContext->Load(anAIS, -1, true/*allow decomposition*/);
  theContext->Activate(anAIS, AIS_Shape::SelectionMode(TopAbs_VERTEX));
  theContext->Activate(anAIS, AIS_Shape::SelectionMode(TopAbs_EDGE));

  // 3. Change selection in the viewer
  // emulate rectangle selection in the viewer
  theContext->Select(0, 0, 2500, 2500, theView);

  // 3. Change the shape of AIS presentation
  gp_Pnt aPnt3(-100, -100, 0);
  gp_Pnt aPnt4(200, 200, 0);
  TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(aPnt3, aPnt4).Edge();
  anAIS->Set(anEdge2);
  anAIS->Redisplay(Standard_True);
  theContext->RecomputeSelectionOnly(anAIS);

  // 4. Check the current viewer selection
  Handle(AIS_InteractiveContext) aContext = theContext;
  bool isFirstLinePoint = false;
  bool isShapeSelected = false;
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
    TopoDS_Shape aShape = aContext->SelectedShape();
    if (aShape.ShapeType() == TopAbs_VERTEX) {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShape);
      if (!aVertex.IsNull()) {
        gp_Pnt aPoint = BRep_Tool::Pnt(aVertex);
        double aD_1 = aPoint.Distance(aPnt1);
        double aD_2 = aPoint.Distance(aPnt2);
        isFirstLinePoint = aD_1 < Precision::Confusion() || aD_2 < Precision::Confusion();
      }
    }
  }
  if (isFirstLinePoint)
    std::cout << "Error: The point of the first line should not be selected." << std::endl;
}

void PartSet_TestOCC::local_selection_erase(Handle_AIS_InteractiveContext theContext,
                                            Handle_V3d_View theView)
{
  // 1. Create shape
  gp_Pnt aPnt1(100, 100, 0);
  gp_Pnt aPnt2(150, 300, 0);
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aPnt1, aPnt2).Edge();

  // 2. Display shape and activate it in the local context
  Handle(AIS_Shape) anAIS = new AIS_Shape(anEdge);
  if (!theContext->HasOpenedContext()) {
    theContext->ClearCurrents(false);
    theContext->OpenLocalContext(false/*use displayed objects*/, true/*allow shape decomposition*/);
  }
  theContext->Display(anAIS, false);
  theContext->Load(anAIS, -1, true/*allow decomposition*/);
  theContext->Activate(anAIS, AIS_Shape::SelectionMode(TopAbs_VERTEX));
  theContext->Activate(anAIS, AIS_Shape::SelectionMode(TopAbs_EDGE));

  // 3. Change selection in the viewer
  // emulate rectangle selection in the viewer
  theContext->Select(0, 0, 2500, 2500, theView);

  theContext->Erase(anAIS);
}

void PartSet_TestOCC::createTestLine(XGUI_Workshop* theWorkshop)
{
  myTestDelta = 80;

  ModuleBase_Operation* anOperation = theWorkshop->operationMgr()->currentOperation();
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  FeaturePtr aSketch;

  if (aPreviewOp) {
    // create a line
    boost::shared_ptr<ModelAPI_Document> aDoc = ModelAPI_Session::get()->rootDocument();
    FeaturePtr aFeature = aDoc->addFeature(SketchPlugin_Line::ID());
    if (aFeature)  // TODO: generate an error if feature was not created
      aFeature->execute();

    boost::shared_ptr<SketchPlugin_Feature> aSketch = boost::dynamic_pointer_cast<
        SketchPlugin_Feature>(aPreviewOp->sketch());
    aSketch->addSub(aFeature);

    PartSet_Tools::setFeaturePoint(aFeature, 100, 100, SketchPlugin_Line::START_ID());
    PartSet_Tools::setFeaturePoint(aFeature, 150, 300, SketchPlugin_Line::END_ID());

    boost::shared_ptr<GeomAPI_Shape> aPreview = PartSet_OperationSketchBase::preview(aFeature);

    XGUI_Displayer* aDisplayer = theWorkshop->displayer();

    boost::shared_ptr<GeomAPI_AISObject> aPrevAIS;
    boost::shared_ptr<SketchPlugin_Feature> aSPFeature = boost::dynamic_pointer_cast<
        SketchPlugin_Feature>(aFeature);
    //boost::shared_ptr<GeomAPI_AISObject> anAIS = aSPFeature->getAISObject(aPrevAIS);
    //if (!anAIS->empty())
    aDisplayer->display(aFeature, false);
    //aDisplayer->redisplay(aFeature->firstResult(), anAIS, false);

    std::list<int> aModes;
    aModes.push_back(TopAbs_VERTEX);
    aModes.push_back(TopAbs_EDGE);
    aDisplayer->activateInLocalContext(aFeature->firstResult(), aModes, true);

    // change the line
    /*double aDelta = -200;
     for (int i = 0; i < 20; i++) {
     aDelta = aDelta - i*2;
     PartSet_Tools::setFeaturePoint(aFeature, 100+aDelta, 200+aDelta, SketchPlugin_Line::START_ID());
     PartSet_Tools::setFeaturePoint(aFeature, 300+aDelta, 500+aDelta, SketchPlugin_Line::END_ID());

     boost::shared_ptr<GeomAPI_Shape> aPreview = PartSet_OperationSketchBase::preview(aFeature);
     Handle(AIS_InteractiveObject) anAIS = PartSet_Presentation::createPresentation(
     aFeature, aSketch,
     aPreview ? aPreview->impl<TopoDS_Shape>() : TopoDS_Shape(), NULL);
     if (!anAIS.IsNull())
     aDisplayer->redisplay(aFeature, anAIS, true);

     int aVal = 90;
     for (int j = 0; j < 10000000; j++)
     aVal = aVal/aVal+aVal*2;
     }*/
    //std::list<int> aModes;
    //aModes.clear();
    //aModes.push_back(TopAbs_VERTEX);
    //aModes.push_back(TopAbs_EDGE);
    //aDisplayer->activateInLocalContext(aFeature, aModes, true);
    myTestObject = aFeature->firstResult();

    QList<ObjectPtr> aFeatureList;
    aFeatureList.append(myTestObject);
    aDisplayer->setSelected(aFeatureList, true);
  }
}

void PartSet_TestOCC::changeTestLine(XGUI_Workshop* theWorkshop)
{
  // change the line
  if (!myTestObject)
    return;
  ResultPtr aFeature = myTestObject;

  myTestDelta = myTestDelta - 50;
  double aDelta = myTestDelta;
  // TODO
  //PartSet_Tools::setFeaturePoint(aFeature, -100/*aDelta*/, -100/*aDelta*/, LINE_ATTR_START);
  //PartSet_Tools::setFeaturePoint(aFeature, 200/*aDelta*2*/, 200/*aDelta*2*/, LINE_ATTR_END);
  //boost::shared_ptr<GeomAPI_Shape> aPreview = PartSet_OperationSketchBase::preview(aFeature);

  boost::shared_ptr<GeomAPI_AISObject> aPrevAIS;
  boost::shared_ptr<SketchPlugin_Feature> aSPFeature = boost::dynamic_pointer_cast<
      SketchPlugin_Feature>(aFeature);
  //boost::shared_ptr<GeomAPI_AISObject> anAIS = aSPFeature->getAISObject(aPrevAIS);
  //if (!anAIS->empty())
  theWorkshop->displayer()->display(aFeature, true);
  //  theWorkshop->displayer()->redisplay(aFeature, anAIS, true);
  //std::list<int> aModes;
  //aModes.clear();
  //aModes.push_back(TopAbs_VERTEX);
  //aModes.push_back(TopAbs_EDGE);
  //aDisplayer->activateInLocalContext(aFeature, aModes, true);

  /*QFeatureList aFeatureList;
   aFeatureList.append(myTestObject);
   theWorkshop->displayer()->setSelected(aFeatureList, true);*/

  theWorkshop->displayer()->updateViewer();
}

void PartSet_TestOCC::moveMouse(Handle(AIS_InteractiveContext) theContext, Handle(V3d_View) theView)
{
  theContext->MoveTo(10, 10, theView);
  theContext->Select();
}
