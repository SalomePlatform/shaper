Feature: Two parameters with the same name could exist in PartSet and Part

    Each part can have its local parameter with any value.

    Scenario: Parameter in part should be created

        Given Launch application in salome mode
         When Create parameter a=30
          And Create part
          And Create parameter a=a+2
         Then the parameter in the part should exist
