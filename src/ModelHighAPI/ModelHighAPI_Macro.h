// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_MACRO_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_MACRO_H_

//--------------------------------------------------------------------------------------
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
//--------------------------------------------------------------------------------------
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Feature.h>

//--------------------------------------------------------------------------------------
#define VAR_NAME(NAME) my##NAME

//--------------------------------------------------------------------------------------
#define TO_STRING_(NAME) #NAME
#define TO_STRING(NAME) TO_STRING_(NAME)

//--------------------------------------------------------------------------------------
// Used in INTERFACE_N for create variable and getter
#define DEFINE_ATTRIBUTE(NAME, TYPE, COMMENT) \
    COMMENT \
    std::shared_ptr<TYPE> NAME() const { return VAR_NAME(NAME); } \
  protected: \
    std::shared_ptr<TYPE> VAR_NAME(NAME); \
  public:

//--------------------------------------------------------------------------------------
// Used in INTERFACE_N inside START_INIT/END_INIT for
// set variable with attribute received from feature
#define SET_ATTRIBUTE(NAME, TYPE, ATT_NAME) \
  VAR_NAME(NAME) = std::dynamic_pointer_cast<TYPE>(feature()->attribute(ATT_NAME)); \
  if (!VAR_NAME(NAME)) \
    return false; \
  myAttrGetter[ATT_NAME] = TO_STRING(NAME);

//--------------------------------------------------------------------------------------
#define INTERFACE_COMMON(KIND) \
  static std::string ID() { return KIND; }\
  virtual std::string getID() { return ID(); }

//--------------------------------------------------------------------------------------
#define START_INIT() \
  bool initialize() { \
    if (!feature()) { \
      throwException(ID() + " exception: The feature is NULL."); \
      return false; \
    } \
    if (getKind() != getID()) { \
      throwException(ID() + " exception: Wrong feature kind."); \
      return false; \
    }

//--------------------------------------------------------------------------------------
#define END_INIT() \
    return true; \
  }

//--------------------------------------------------------------------------------------
#define INTERFACE_0(KIND) \
  public: \
    INTERFACE_COMMON(KIND) \
  protected: \
    START_INIT() \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_1(KIND, \
                    N_0, AN_0, T_0, C_0) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_2(KIND, \
                    N_0, AN_0, T_0, C_0, \
                    N_1, AN_1, T_1, C_1) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_3(KIND, \
                    N_0, AN_0, T_0, C_0, \
                    N_1, AN_1, T_1, C_1, \
                    N_2, AN_2, T_2, C_2) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_4(KIND, \
                    N_0, AN_0, T_0, C_0, \
                    N_1, AN_1, T_1, C_1, \
                    N_2, AN_2, T_2, C_2, \
                    N_3, AN_3, T_3, C_3) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_5(KIND, \
                    N_0, AN_0, T_0, C_0, \
                    N_1, AN_1, T_1, C_1, \
                    N_2, AN_2, T_2, C_2, \
                    N_3, AN_3, T_3, C_3, \
                    N_4, AN_4, T_4, C_4) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_6(KIND, \
                    N_0, AN_0, T_0, C_0, \
                    N_1, AN_1, T_1, C_1, \
                    N_2, AN_2, T_2, C_2, \
                    N_3, AN_3, T_3, C_3, \
                    N_4, AN_4, T_4, C_4, \
                    N_5, AN_5, T_5, C_5) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
    DEFINE_ATTRIBUTE(N_5, T_5, C_5) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
      SET_ATTRIBUTE(N_5, T_5, AN_5) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_7(KIND, \
                    N_0, AN_0, T_0, C_0, \
                    N_1, AN_1, T_1, C_1, \
                    N_2, AN_2, T_2, C_2, \
                    N_3, AN_3, T_3, C_3, \
                    N_4, AN_4, T_4, C_4, \
                    N_5, AN_5, T_5, C_5, \
                    N_6, AN_6, T_6, C_6) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
    DEFINE_ATTRIBUTE(N_5, T_5, C_5) \
    DEFINE_ATTRIBUTE(N_6, T_6, C_6) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
      SET_ATTRIBUTE(N_5, T_5, AN_5) \
      SET_ATTRIBUTE(N_6, T_6, AN_6) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_8(KIND, \
                    N_0, AN_0, T_0, C_0, \
                    N_1, AN_1, T_1, C_1, \
                    N_2, AN_2, T_2, C_2, \
                    N_3, AN_3, T_3, C_3, \
                    N_4, AN_4, T_4, C_4, \
                    N_5, AN_5, T_5, C_5, \
                    N_6, AN_6, T_6, C_6, \
                    N_7, AN_7, T_7, C_7) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
    DEFINE_ATTRIBUTE(N_5, T_5, C_5) \
    DEFINE_ATTRIBUTE(N_6, T_6, C_6) \
    DEFINE_ATTRIBUTE(N_7, T_7, C_7) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
      SET_ATTRIBUTE(N_5, T_5, AN_5) \
      SET_ATTRIBUTE(N_6, T_6, AN_6) \
      SET_ATTRIBUTE(N_7, T_7, AN_7) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_9(KIND, \
                    N_0, AN_0, T_0, C_0, \
                    N_1, AN_1, T_1, C_1, \
                    N_2, AN_2, T_2, C_2, \
                    N_3, AN_3, T_3, C_3, \
                    N_4, AN_4, T_4, C_4, \
                    N_5, AN_5, T_5, C_5, \
                    N_6, AN_6, T_6, C_6, \
                    N_7, AN_7, T_7, C_7, \
                    N_8, AN_8, T_8, C_8) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
    DEFINE_ATTRIBUTE(N_5, T_5, C_5) \
    DEFINE_ATTRIBUTE(N_6, T_6, C_6) \
    DEFINE_ATTRIBUTE(N_7, T_7, C_7) \
    DEFINE_ATTRIBUTE(N_8, T_8, C_8) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
      SET_ATTRIBUTE(N_5, T_5, AN_5) \
      SET_ATTRIBUTE(N_6, T_6, AN_6) \
      SET_ATTRIBUTE(N_7, T_7, AN_7) \
      SET_ATTRIBUTE(N_8, T_8, AN_8) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_10(KIND, \
                     N_0, AN_0, T_0, C_0, \
                     N_1, AN_1, T_1, C_1, \
                     N_2, AN_2, T_2, C_2, \
                     N_3, AN_3, T_3, C_3, \
                     N_4, AN_4, T_4, C_4, \
                     N_5, AN_5, T_5, C_5, \
                     N_6, AN_6, T_6, C_6, \
                     N_7, AN_7, T_7, C_7, \
                     N_8, AN_8, T_8, C_8, \
                     N_9, AN_9, T_9, C_9) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
    DEFINE_ATTRIBUTE(N_5, T_5, C_5) \
    DEFINE_ATTRIBUTE(N_6, T_6, C_6) \
    DEFINE_ATTRIBUTE(N_7, T_7, C_7) \
    DEFINE_ATTRIBUTE(N_8, T_8, C_8) \
    DEFINE_ATTRIBUTE(N_9, T_9, C_9) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
      SET_ATTRIBUTE(N_5, T_5, AN_5) \
      SET_ATTRIBUTE(N_6, T_6, AN_6) \
      SET_ATTRIBUTE(N_7, T_7, AN_7) \
      SET_ATTRIBUTE(N_8, T_8, AN_8) \
      SET_ATTRIBUTE(N_9, T_9, AN_9) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_11(KIND, \
                     N_0, AN_0, T_0, C_0, \
                     N_1, AN_1, T_1, C_1, \
                     N_2, AN_2, T_2, C_2, \
                     N_3, AN_3, T_3, C_3, \
                     N_4, AN_4, T_4, C_4, \
                     N_5, AN_5, T_5, C_5, \
                     N_6, AN_6, T_6, C_6, \
                     N_7, AN_7, T_7, C_7, \
                     N_8, AN_8, T_8, C_8, \
                     N_9, AN_9, T_9, C_9, \
                     N_10, AN_10, T_10, C_10) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
    DEFINE_ATTRIBUTE(N_5, T_5, C_5) \
    DEFINE_ATTRIBUTE(N_6, T_6, C_6) \
    DEFINE_ATTRIBUTE(N_7, T_7, C_7) \
    DEFINE_ATTRIBUTE(N_8, T_8, C_8) \
    DEFINE_ATTRIBUTE(N_9, T_9, C_9) \
    DEFINE_ATTRIBUTE(N_10, T_10, C_10) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
      SET_ATTRIBUTE(N_5, T_5, AN_5) \
      SET_ATTRIBUTE(N_6, T_6, AN_6) \
      SET_ATTRIBUTE(N_7, T_7, AN_7) \
      SET_ATTRIBUTE(N_8, T_8, AN_8) \
      SET_ATTRIBUTE(N_9, T_9, AN_9) \
      SET_ATTRIBUTE(N_10, T_10, AN_10) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_12(KIND, \
                     N_0, AN_0, T_0, C_0, \
                     N_1, AN_1, T_1, C_1, \
                     N_2, AN_2, T_2, C_2, \
                     N_3, AN_3, T_3, C_3, \
                     N_4, AN_4, T_4, C_4, \
                     N_5, AN_5, T_5, C_5, \
                     N_6, AN_6, T_6, C_6, \
                     N_7, AN_7, T_7, C_7, \
                     N_8, AN_8, T_8, C_8, \
                     N_9, AN_9, T_9, C_9, \
                     N_10, AN_10, T_10, C_10, \
                     N_11, AN_11, T_11, C_11) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
    DEFINE_ATTRIBUTE(N_5, T_5, C_5) \
    DEFINE_ATTRIBUTE(N_6, T_6, C_6) \
    DEFINE_ATTRIBUTE(N_7, T_7, C_7) \
    DEFINE_ATTRIBUTE(N_8, T_8, C_8) \
    DEFINE_ATTRIBUTE(N_9, T_9, C_9) \
    DEFINE_ATTRIBUTE(N_10, T_10, C_10) \
    DEFINE_ATTRIBUTE(N_11, T_11, C_11) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
      SET_ATTRIBUTE(N_5, T_5, AN_5) \
      SET_ATTRIBUTE(N_6, T_6, AN_6) \
      SET_ATTRIBUTE(N_7, T_7, AN_7) \
      SET_ATTRIBUTE(N_8, T_8, AN_8) \
      SET_ATTRIBUTE(N_9, T_9, AN_9) \
      SET_ATTRIBUTE(N_10, T_10, AN_10) \
      SET_ATTRIBUTE(N_11, T_11, AN_11) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_14(KIND, \
                     N_0, AN_0, T_0, C_0, \
                     N_1, AN_1, T_1, C_1, \
                     N_2, AN_2, T_2, C_2, \
                     N_3, AN_3, T_3, C_3, \
                     N_4, AN_4, T_4, C_4, \
                     N_5, AN_5, T_5, C_5, \
                     N_6, AN_6, T_6, C_6, \
                     N_7, AN_7, T_7, C_7, \
                     N_8, AN_8, T_8, C_8, \
                     N_9, AN_9, T_9, C_9, \
                     N_10, AN_10, T_10, C_10, \
                     N_11, AN_11, T_11, C_11, \
                     N_12, AN_12, T_12, C_12, \
                     N_13, AN_13, T_13, C_13) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
    DEFINE_ATTRIBUTE(N_5, T_5, C_5) \
    DEFINE_ATTRIBUTE(N_6, T_6, C_6) \
    DEFINE_ATTRIBUTE(N_7, T_7, C_7) \
    DEFINE_ATTRIBUTE(N_8, T_8, C_8) \
    DEFINE_ATTRIBUTE(N_9, T_9, C_9) \
    DEFINE_ATTRIBUTE(N_10, T_10, C_10) \
    DEFINE_ATTRIBUTE(N_11, T_11, C_11) \
    DEFINE_ATTRIBUTE(N_12, T_12, C_12) \
    DEFINE_ATTRIBUTE(N_13, T_13, C_13) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
      SET_ATTRIBUTE(N_5, T_5, AN_5) \
      SET_ATTRIBUTE(N_6, T_6, AN_6) \
      SET_ATTRIBUTE(N_7, T_7, AN_7) \
      SET_ATTRIBUTE(N_8, T_8, AN_8) \
      SET_ATTRIBUTE(N_9, T_9, AN_9) \
      SET_ATTRIBUTE(N_10, T_10, AN_10) \
      SET_ATTRIBUTE(N_11, T_11, AN_11) \
      SET_ATTRIBUTE(N_12, T_12, AN_12) \
      SET_ATTRIBUTE(N_13, T_13, AN_13) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_16(KIND, \
                     N_0, AN_0, T_0, C_0, \
                     N_1, AN_1, T_1, C_1, \
                     N_2, AN_2, T_2, C_2, \
                     N_3, AN_3, T_3, C_3, \
                     N_4, AN_4, T_4, C_4, \
                     N_5, AN_5, T_5, C_5, \
                     N_6, AN_6, T_6, C_6, \
                     N_7, AN_7, T_7, C_7, \
                     N_8, AN_8, T_8, C_8, \
                     N_9, AN_9, T_9, C_9, \
                     N_10, AN_10, T_10, C_10, \
                     N_11, AN_11, T_11, C_11, \
                     N_12, AN_12, T_12, C_12, \
                     N_13, AN_13, T_13, C_13, \
                     N_14, AN_14, T_14, C_14, \
                     N_15, AN_15, T_15, C_15) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
    DEFINE_ATTRIBUTE(N_5, T_5, C_5) \
    DEFINE_ATTRIBUTE(N_6, T_6, C_6) \
    DEFINE_ATTRIBUTE(N_7, T_7, C_7) \
    DEFINE_ATTRIBUTE(N_8, T_8, C_8) \
    DEFINE_ATTRIBUTE(N_9, T_9, C_9) \
    DEFINE_ATTRIBUTE(N_10, T_10, C_10) \
    DEFINE_ATTRIBUTE(N_11, T_11, C_11) \
    DEFINE_ATTRIBUTE(N_12, T_12, C_12) \
    DEFINE_ATTRIBUTE(N_13, T_13, C_13) \
    DEFINE_ATTRIBUTE(N_14, T_14, C_14) \
    DEFINE_ATTRIBUTE(N_15, T_15, C_15) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
      SET_ATTRIBUTE(N_5, T_5, AN_5) \
      SET_ATTRIBUTE(N_6, T_6, AN_6) \
      SET_ATTRIBUTE(N_7, T_7, AN_7) \
      SET_ATTRIBUTE(N_8, T_8, AN_8) \
      SET_ATTRIBUTE(N_9, T_9, AN_9) \
      SET_ATTRIBUTE(N_10, T_10, AN_10) \
      SET_ATTRIBUTE(N_11, T_11, AN_11) \
      SET_ATTRIBUTE(N_12, T_12, AN_12) \
      SET_ATTRIBUTE(N_13, T_13, AN_13) \
      SET_ATTRIBUTE(N_14, T_14, AN_14) \
      SET_ATTRIBUTE(N_15, T_15, AN_15) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_18(KIND, \
                     N_0, AN_0, T_0, C_0, \
                     N_1, AN_1, T_1, C_1, \
                     N_2, AN_2, T_2, C_2, \
                     N_3, AN_3, T_3, C_3, \
                     N_4, AN_4, T_4, C_4, \
                     N_5, AN_5, T_5, C_5, \
                     N_6, AN_6, T_6, C_6, \
                     N_7, AN_7, T_7, C_7, \
                     N_8, AN_8, T_8, C_8, \
                     N_9, AN_9, T_9, C_9, \
                     N_10, AN_10, T_10, C_10, \
                     N_11, AN_11, T_11, C_11, \
                     N_12, AN_12, T_12, C_12, \
                     N_13, AN_13, T_13, C_13, \
                     N_14, AN_14, T_14, C_14, \
                     N_15, AN_15, T_15, C_15, \
                     N_16, AN_16, T_16, C_16, \
                     N_17, AN_17, T_17, C_17) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
    DEFINE_ATTRIBUTE(N_5, T_5, C_5) \
    DEFINE_ATTRIBUTE(N_6, T_6, C_6) \
    DEFINE_ATTRIBUTE(N_7, T_7, C_7) \
    DEFINE_ATTRIBUTE(N_8, T_8, C_8) \
    DEFINE_ATTRIBUTE(N_9, T_9, C_9) \
    DEFINE_ATTRIBUTE(N_10, T_10, C_10) \
    DEFINE_ATTRIBUTE(N_11, T_11, C_11) \
    DEFINE_ATTRIBUTE(N_12, T_12, C_12) \
    DEFINE_ATTRIBUTE(N_13, T_13, C_13) \
    DEFINE_ATTRIBUTE(N_14, T_14, C_14) \
    DEFINE_ATTRIBUTE(N_15, T_15, C_15) \
    DEFINE_ATTRIBUTE(N_16, T_16, C_16) \
    DEFINE_ATTRIBUTE(N_17, T_17, C_17) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
      SET_ATTRIBUTE(N_5, T_5, AN_5) \
      SET_ATTRIBUTE(N_6, T_6, AN_6) \
      SET_ATTRIBUTE(N_7, T_7, AN_7) \
      SET_ATTRIBUTE(N_8, T_8, AN_8) \
      SET_ATTRIBUTE(N_9, T_9, AN_9) \
      SET_ATTRIBUTE(N_10, T_10, AN_10) \
      SET_ATTRIBUTE(N_11, T_11, AN_11) \
      SET_ATTRIBUTE(N_12, T_12, AN_12) \
      SET_ATTRIBUTE(N_13, T_13, AN_13) \
      SET_ATTRIBUTE(N_14, T_14, AN_14) \
      SET_ATTRIBUTE(N_15, T_15, AN_15) \
      SET_ATTRIBUTE(N_16, T_16, AN_16) \
      SET_ATTRIBUTE(N_17, T_17, AN_17) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_20(KIND, \
                     N_0, AN_0, T_0, C_0, \
                     N_1, AN_1, T_1, C_1, \
                     N_2, AN_2, T_2, C_2, \
                     N_3, AN_3, T_3, C_3, \
                     N_4, AN_4, T_4, C_4, \
                     N_5, AN_5, T_5, C_5, \
                     N_6, AN_6, T_6, C_6, \
                     N_7, AN_7, T_7, C_7, \
                     N_8, AN_8, T_8, C_8, \
                     N_9, AN_9, T_9, C_9, \
                     N_10, AN_10, T_10, C_10, \
                     N_11, AN_11, T_11, C_11, \
                     N_12, AN_12, T_12, C_12, \
                     N_13, AN_13, T_13, C_13, \
                     N_14, AN_14, T_14, C_14, \
                     N_15, AN_15, T_15, C_15, \
                     N_16, AN_16, T_16, C_16, \
                     N_17, AN_17, T_17, C_17, \
                     N_18, AN_18, T_18, C_18, \
                     N_19, AN_19, T_19, C_19) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
    DEFINE_ATTRIBUTE(N_5, T_5, C_5) \
    DEFINE_ATTRIBUTE(N_6, T_6, C_6) \
    DEFINE_ATTRIBUTE(N_7, T_7, C_7) \
    DEFINE_ATTRIBUTE(N_8, T_8, C_8) \
    DEFINE_ATTRIBUTE(N_9, T_9, C_9) \
    DEFINE_ATTRIBUTE(N_10, T_10, C_10) \
    DEFINE_ATTRIBUTE(N_11, T_11, C_11) \
    DEFINE_ATTRIBUTE(N_12, T_12, C_12) \
    DEFINE_ATTRIBUTE(N_13, T_13, C_13) \
    DEFINE_ATTRIBUTE(N_14, T_14, C_14) \
    DEFINE_ATTRIBUTE(N_15, T_15, C_15) \
    DEFINE_ATTRIBUTE(N_16, T_16, C_16) \
    DEFINE_ATTRIBUTE(N_17, T_17, C_17) \
    DEFINE_ATTRIBUTE(N_18, T_18, C_18) \
    DEFINE_ATTRIBUTE(N_19, T_19, C_19) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
      SET_ATTRIBUTE(N_5, T_5, AN_5) \
      SET_ATTRIBUTE(N_6, T_6, AN_6) \
      SET_ATTRIBUTE(N_7, T_7, AN_7) \
      SET_ATTRIBUTE(N_8, T_8, AN_8) \
      SET_ATTRIBUTE(N_9, T_9, AN_9) \
      SET_ATTRIBUTE(N_10, T_10, AN_10) \
      SET_ATTRIBUTE(N_11, T_11, AN_11) \
      SET_ATTRIBUTE(N_12, T_12, AN_12) \
      SET_ATTRIBUTE(N_13, T_13, AN_13) \
      SET_ATTRIBUTE(N_14, T_14, AN_14) \
      SET_ATTRIBUTE(N_15, T_15, AN_15) \
      SET_ATTRIBUTE(N_16, T_16, AN_16) \
      SET_ATTRIBUTE(N_17, T_17, AN_17) \
      SET_ATTRIBUTE(N_18, T_18, AN_18) \
      SET_ATTRIBUTE(N_19, T_19, AN_19) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_21(KIND, \
                     N_0, AN_0, T_0, C_0, \
                     N_1, AN_1, T_1, C_1, \
                     N_2, AN_2, T_2, C_2, \
                     N_3, AN_3, T_3, C_3, \
                     N_4, AN_4, T_4, C_4, \
                     N_5, AN_5, T_5, C_5, \
                     N_6, AN_6, T_6, C_6, \
                     N_7, AN_7, T_7, C_7, \
                     N_8, AN_8, T_8, C_8, \
                     N_9, AN_9, T_9, C_9, \
                     N_10, AN_10, T_10, C_10, \
                     N_11, AN_11, T_11, C_11, \
                     N_12, AN_12, T_12, C_12, \
                     N_13, AN_13, T_13, C_13, \
                     N_14, AN_14, T_14, C_14, \
                     N_15, AN_15, T_15, C_15, \
                     N_16, AN_16, T_16, C_16, \
                     N_17, AN_17, T_17, C_17, \
                     N_18, AN_18, T_18, C_18, \
                     N_19, AN_19, T_19, C_19, \
                     N_20, AN_20, T_20, C_20) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
    DEFINE_ATTRIBUTE(N_5, T_5, C_5) \
    DEFINE_ATTRIBUTE(N_6, T_6, C_6) \
    DEFINE_ATTRIBUTE(N_7, T_7, C_7) \
    DEFINE_ATTRIBUTE(N_8, T_8, C_8) \
    DEFINE_ATTRIBUTE(N_9, T_9, C_9) \
    DEFINE_ATTRIBUTE(N_10, T_10, C_10) \
    DEFINE_ATTRIBUTE(N_11, T_11, C_11) \
    DEFINE_ATTRIBUTE(N_12, T_12, C_12) \
    DEFINE_ATTRIBUTE(N_13, T_13, C_13) \
    DEFINE_ATTRIBUTE(N_14, T_14, C_14) \
    DEFINE_ATTRIBUTE(N_15, T_15, C_15) \
    DEFINE_ATTRIBUTE(N_16, T_16, C_16) \
    DEFINE_ATTRIBUTE(N_17, T_17, C_17) \
    DEFINE_ATTRIBUTE(N_18, T_18, C_18) \
    DEFINE_ATTRIBUTE(N_19, T_19, C_19) \
    DEFINE_ATTRIBUTE(N_20, T_20, C_20) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
      SET_ATTRIBUTE(N_5, T_5, AN_5) \
      SET_ATTRIBUTE(N_6, T_6, AN_6) \
      SET_ATTRIBUTE(N_7, T_7, AN_7) \
      SET_ATTRIBUTE(N_8, T_8, AN_8) \
      SET_ATTRIBUTE(N_9, T_9, AN_9) \
      SET_ATTRIBUTE(N_10, T_10, AN_10) \
      SET_ATTRIBUTE(N_11, T_11, AN_11) \
      SET_ATTRIBUTE(N_12, T_12, AN_12) \
      SET_ATTRIBUTE(N_13, T_13, AN_13) \
      SET_ATTRIBUTE(N_14, T_14, AN_14) \
      SET_ATTRIBUTE(N_15, T_15, AN_15) \
      SET_ATTRIBUTE(N_16, T_16, AN_16) \
      SET_ATTRIBUTE(N_17, T_17, AN_17) \
      SET_ATTRIBUTE(N_18, T_18, AN_18) \
      SET_ATTRIBUTE(N_19, T_19, AN_19) \
      SET_ATTRIBUTE(N_20, T_20, AN_20) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_24(KIND, \
                     N_0, AN_0, T_0, C_0, \
                     N_1, AN_1, T_1, C_1, \
                     N_2, AN_2, T_2, C_2, \
                     N_3, AN_3, T_3, C_3, \
                     N_4, AN_4, T_4, C_4, \
                     N_5, AN_5, T_5, C_5, \
                     N_6, AN_6, T_6, C_6, \
                     N_7, AN_7, T_7, C_7, \
                     N_8, AN_8, T_8, C_8, \
                     N_9, AN_9, T_9, C_9, \
                     N_10, AN_10, T_10, C_10, \
                     N_11, AN_11, T_11, C_11, \
                     N_12, AN_12, T_12, C_12, \
                     N_13, AN_13, T_13, C_13, \
                     N_14, AN_14, T_14, C_14, \
                     N_15, AN_15, T_15, C_15, \
                     N_16, AN_16, T_16, C_16, \
                     N_17, AN_17, T_17, C_17, \
                     N_18, AN_18, T_18, C_18, \
                     N_19, AN_19, T_19, C_19, \
                     N_20, AN_20, T_20, C_20, \
                     N_21, AN_21, T_21, C_21, \
                     N_22, AN_22, T_22, C_22, \
                     N_23, AN_23, T_23, C_23) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
    DEFINE_ATTRIBUTE(N_5, T_5, C_5) \
    DEFINE_ATTRIBUTE(N_6, T_6, C_6) \
    DEFINE_ATTRIBUTE(N_7, T_7, C_7) \
    DEFINE_ATTRIBUTE(N_8, T_8, C_8) \
    DEFINE_ATTRIBUTE(N_9, T_9, C_9) \
    DEFINE_ATTRIBUTE(N_10, T_10, C_10) \
    DEFINE_ATTRIBUTE(N_11, T_11, C_11) \
    DEFINE_ATTRIBUTE(N_12, T_12, C_12) \
    DEFINE_ATTRIBUTE(N_13, T_13, C_13) \
    DEFINE_ATTRIBUTE(N_14, T_14, C_14) \
    DEFINE_ATTRIBUTE(N_15, T_15, C_15) \
    DEFINE_ATTRIBUTE(N_16, T_16, C_16) \
    DEFINE_ATTRIBUTE(N_17, T_17, C_17) \
    DEFINE_ATTRIBUTE(N_18, T_18, C_18) \
    DEFINE_ATTRIBUTE(N_19, T_19, C_19) \
    DEFINE_ATTRIBUTE(N_20, T_20, C_20) \
    DEFINE_ATTRIBUTE(N_21, T_21, C_21) \
    DEFINE_ATTRIBUTE(N_22, T_22, C_22) \
    DEFINE_ATTRIBUTE(N_23, T_23, C_23) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
      SET_ATTRIBUTE(N_5, T_5, AN_5) \
      SET_ATTRIBUTE(N_6, T_6, AN_6) \
      SET_ATTRIBUTE(N_7, T_7, AN_7) \
      SET_ATTRIBUTE(N_8, T_8, AN_8) \
      SET_ATTRIBUTE(N_9, T_9, AN_9) \
      SET_ATTRIBUTE(N_10, T_10, AN_10) \
      SET_ATTRIBUTE(N_11, T_11, AN_11) \
      SET_ATTRIBUTE(N_12, T_12, AN_12) \
      SET_ATTRIBUTE(N_13, T_13, AN_13) \
      SET_ATTRIBUTE(N_14, T_14, AN_14) \
      SET_ATTRIBUTE(N_15, T_15, AN_15) \
      SET_ATTRIBUTE(N_16, T_16, AN_16) \
      SET_ATTRIBUTE(N_17, T_17, AN_17) \
      SET_ATTRIBUTE(N_18, T_18, AN_18) \
      SET_ATTRIBUTE(N_19, T_19, AN_19) \
      SET_ATTRIBUTE(N_20, T_20, AN_20) \
      SET_ATTRIBUTE(N_21, T_21, AN_21) \
      SET_ATTRIBUTE(N_22, T_22, AN_22) \
      SET_ATTRIBUTE(N_23, T_23, AN_23) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#define INTERFACE_27(KIND, \
                     N_0, AN_0, T_0, C_0, \
                     N_1, AN_1, T_1, C_1, \
                     N_2, AN_2, T_2, C_2, \
                     N_3, AN_3, T_3, C_3, \
                     N_4, AN_4, T_4, C_4, \
                     N_5, AN_5, T_5, C_5, \
                     N_6, AN_6, T_6, C_6, \
                     N_7, AN_7, T_7, C_7, \
                     N_8, AN_8, T_8, C_8, \
                     N_9, AN_9, T_9, C_9, \
                     N_10, AN_10, T_10, C_10, \
                     N_11, AN_11, T_11, C_11, \
                     N_12, AN_12, T_12, C_12, \
                     N_13, AN_13, T_13, C_13, \
                     N_14, AN_14, T_14, C_14, \
                     N_15, AN_15, T_15, C_15, \
                     N_16, AN_16, T_16, C_16, \
                     N_17, AN_17, T_17, C_17, \
                     N_18, AN_18, T_18, C_18, \
                     N_19, AN_19, T_19, C_19, \
                     N_20, AN_20, T_20, C_20, \
                     N_21, AN_21, T_21, C_21, \
                     N_22, AN_22, T_22, C_22, \
                     N_23, AN_23, T_23, C_23, \
                     N_24, AN_24, T_24, C_24, \
                     N_25, AN_25, T_25, C_25, \
                     N_26, AN_26, T_26, C_26) \
  public: \
    INTERFACE_COMMON(KIND) \
    DEFINE_ATTRIBUTE(N_0, T_0, C_0) \
    DEFINE_ATTRIBUTE(N_1, T_1, C_1) \
    DEFINE_ATTRIBUTE(N_2, T_2, C_2) \
    DEFINE_ATTRIBUTE(N_3, T_3, C_3) \
    DEFINE_ATTRIBUTE(N_4, T_4, C_4) \
    DEFINE_ATTRIBUTE(N_5, T_5, C_5) \
    DEFINE_ATTRIBUTE(N_6, T_6, C_6) \
    DEFINE_ATTRIBUTE(N_7, T_7, C_7) \
    DEFINE_ATTRIBUTE(N_8, T_8, C_8) \
    DEFINE_ATTRIBUTE(N_9, T_9, C_9) \
    DEFINE_ATTRIBUTE(N_10, T_10, C_10) \
    DEFINE_ATTRIBUTE(N_11, T_11, C_11) \
    DEFINE_ATTRIBUTE(N_12, T_12, C_12) \
    DEFINE_ATTRIBUTE(N_13, T_13, C_13) \
    DEFINE_ATTRIBUTE(N_14, T_14, C_14) \
    DEFINE_ATTRIBUTE(N_15, T_15, C_15) \
    DEFINE_ATTRIBUTE(N_16, T_16, C_16) \
    DEFINE_ATTRIBUTE(N_17, T_17, C_17) \
    DEFINE_ATTRIBUTE(N_18, T_18, C_18) \
    DEFINE_ATTRIBUTE(N_19, T_19, C_19) \
    DEFINE_ATTRIBUTE(N_20, T_20, C_20) \
    DEFINE_ATTRIBUTE(N_21, T_21, C_21) \
    DEFINE_ATTRIBUTE(N_22, T_22, C_22) \
    DEFINE_ATTRIBUTE(N_23, T_23, C_23) \
    DEFINE_ATTRIBUTE(N_24, T_24, C_24) \
    DEFINE_ATTRIBUTE(N_25, T_25, C_25) \
    DEFINE_ATTRIBUTE(N_26, T_26, C_26) \
  protected: \
    START_INIT() \
      SET_ATTRIBUTE(N_0, T_0, AN_0) \
      SET_ATTRIBUTE(N_1, T_1, AN_1) \
      SET_ATTRIBUTE(N_2, T_2, AN_2) \
      SET_ATTRIBUTE(N_3, T_3, AN_3) \
      SET_ATTRIBUTE(N_4, T_4, AN_4) \
      SET_ATTRIBUTE(N_5, T_5, AN_5) \
      SET_ATTRIBUTE(N_6, T_6, AN_6) \
      SET_ATTRIBUTE(N_7, T_7, AN_7) \
      SET_ATTRIBUTE(N_8, T_8, AN_8) \
      SET_ATTRIBUTE(N_9, T_9, AN_9) \
      SET_ATTRIBUTE(N_10, T_10, AN_10) \
      SET_ATTRIBUTE(N_11, T_11, AN_11) \
      SET_ATTRIBUTE(N_12, T_12, AN_12) \
      SET_ATTRIBUTE(N_13, T_13, AN_13) \
      SET_ATTRIBUTE(N_14, T_14, AN_14) \
      SET_ATTRIBUTE(N_15, T_15, AN_15) \
      SET_ATTRIBUTE(N_16, T_16, AN_16) \
      SET_ATTRIBUTE(N_17, T_17, AN_17) \
      SET_ATTRIBUTE(N_18, T_18, AN_18) \
      SET_ATTRIBUTE(N_19, T_19, AN_19) \
      SET_ATTRIBUTE(N_20, T_20, AN_20) \
      SET_ATTRIBUTE(N_21, T_21, AN_21) \
      SET_ATTRIBUTE(N_22, T_22, AN_22) \
      SET_ATTRIBUTE(N_23, T_23, AN_23) \
      SET_ATTRIBUTE(N_24, T_24, AN_24) \
      SET_ATTRIBUTE(N_25, T_25, AN_25) \
      SET_ATTRIBUTE(N_26, T_26, AN_26) \
    END_INIT() \
  public:

//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_MACRO_H_ */
