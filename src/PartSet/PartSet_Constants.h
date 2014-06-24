// File:        PartSet_Tools.h
// Created:     04 Jun 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_Constants_H
#define PartSet_Constants_H

#include <ModelAPI_Feature.h>

/// This file contains various constants used in the PartSet module

/// Types of viewer selection in an operation
enum PartSet_SelectionMode
{
  SM_FirstPoint,
  SM_SecondPoint,
  SM_ThirdPoint,
  SM_LastPoint,
  SM_DonePoint
};

////const int CONSTRAINT_TEXT_HEIGHT = 28; /// the text height of the constraint
////const int CONSTRAINT_TEXT_SELECTION_TOLERANCE = 20; /// the text selection tolerance

#endif
