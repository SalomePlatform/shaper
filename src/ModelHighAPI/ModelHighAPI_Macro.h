// Name   : ModelHighAPI_Macro.h
// Purpose:
//
// History:
// 27/05/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_MACRO_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_MACRO_H_

//--------------------------------------------------------------------------------------
#include <ModelAPI_Feature.h>

//--------------------------------------------------------------------------------------
// See ModelAPI_Feature functions
#define FN_Boolean boolean
#define FN_DocRef document
#define FN_Double real
#define FN_Integer integer
#define FN_RefAttr refattr
#define FN_Reference reference
#define FN_RefList reflist
#define FN_Selection selection
#define FN_SelectionList selectionList
#define FN_String string

//--------------------------------------------------------------------------------------
#define VAR_NAME(NAME) my##NAME

//--------------------------------------------------------------------------------------
// Used in INTERFACE_N for create variable and getter
#define DEFINE_ATTRIBUTE(NAME, TYPE, COMMENT) \
    COMMENT \
    std::shared_ptr<ModelAPI_Attribute##TYPE> NAME() const { return VAR_NAME(NAME); } \
  protected: \
    std::shared_ptr<ModelAPI_Attribute##TYPE> VAR_NAME(NAME); \
  public:

//--------------------------------------------------------------------------------------
// Used in INTERFACE_N inside START_INIT/END_INIT for set variable with attribute received from feature
#define SET_ATTRIBUTE(NAME, TYPE, ATT_NAME) \
  VAR_NAME(NAME) = feature()->FN_##TYPE(ATT_NAME); \
  if (!VAR_NAME(NAME)) \
    return false;

//--------------------------------------------------------------------------------------
#define INTERFACE_COMMON(KIND) \
  static std::string ID() { return KIND; }

//--------------------------------------------------------------------------------------
#define START_INIT() \
  bool initialize() { \
    if (!feature()) { \
      throwException(ID() + " exception: The feature is NULL."); \
      return false; \
    } \
    if (getKind() != ID()) { \
      throwException(ID() + " exception: Wrong feature kind."); \
      return false; \
    }

//--------------------------------------------------------------------------------------
#define END_INIT() \
    return true; \
  }


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
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_MACRO_H_ */
