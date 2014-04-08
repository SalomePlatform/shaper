// File:        Model_Iterator.hxx
// Created:     1 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_Iterator.h"
#include "Model_Document.h"
#include "ModelAPI_Feature.h"
#include "Model_Object.h"
#include <TDataStd_Comment.hxx>
#include <TDataStd_Name.hxx>

using namespace std;

void Model_Iterator::next()
{
  return myIter.Next();
}

bool Model_Iterator::more()
{
  return myIter.More();
}

shared_ptr<ModelAPI_Feature> Model_Iterator::current()
{
  TDF_Label aLab = myIter.Value()->Label();
  return myDoc->feature(aLab);
}

string Model_Iterator::currentKind()
{
  return string(TCollection_AsciiString(
    Handle(TDataStd_Comment)::DownCast(myIter.Value())->Get()).ToCString());
}

string Model_Iterator::currentName()
{
  TDF_Label aLab = myIter.Value()->Label();
  Handle(TDataStd_Name) aName;
  if (aLab.FindAttribute(TDataStd_Name::GetID(), aName))
    return string(TCollection_AsciiString(aName->Get()).ToCString());
  return ""; // name is not found
}

int Model_Iterator::numIterationsLeft()
{
  int aResult = 0;
  TDF_ChildIDIterator aTempIter(myIter);
  for(; aTempIter.More(); aTempIter.Next())
    aResult++;
  return aResult;
}

bool Model_Iterator::is(std::shared_ptr<ModelAPI_Feature> theFeature)
{
  return myIter.Value()->Label() == 
    dynamic_pointer_cast<Model_Object>(theFeature->data())->label();

}


Model_Iterator::Model_Iterator(std::shared_ptr<Model_Document> theDoc, TDF_Label theLab)
  : myDoc(theDoc), myIter(theLab, TDataStd_Comment::GetID(), Standard_False)
{}
