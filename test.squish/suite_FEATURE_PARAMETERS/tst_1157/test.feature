Feature: Disable using expressions in double and Point2D attributes of sketch entities.

    Sketch entities should be modified by solver, but using expressions restricts value changing.

    Scenario: Check that Point, Line, Circle, Arc and Fillet does not allow input parameters

        Given Launch application in salome mode
         When Create part
          And Create parameter a=30
          And Create sketch
         Then Check that Point does not allow using expressions
          And Check that Line does not allow using expressions
          And Check that Circle does not allow using expressions
          And Check that Arc does not allow using expressions
          And Check that Fillet does not allow using expressions
