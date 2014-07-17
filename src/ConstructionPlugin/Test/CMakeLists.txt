INCLUDE(Common)

ENABLE_TESTING()
ADD_TEST(ConstructionPluginTest 
         ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_point_name.py)
