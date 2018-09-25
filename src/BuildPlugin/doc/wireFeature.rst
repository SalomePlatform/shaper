
Wire
====

The Wire feature creates a wire using already existing edges in other objects.

To create wire in the active part:

#. select in the Main Menu *Build - > Wire* item  or
#. click **Wire** button in the toolbar

.. image:: images/feature_wire.png
  :align: center

.. centered::
  **Wire** button

The following property panel will be opened:

.. image:: images/Wire.png
  :align: center

.. centered::
  Create a wire
  
Select one or several edges in viewer. For automatic selection of closed contour starting from a selected edge it is necessary to press **Add contour** button.

**Apply** button creates wire.

**Cancel** button cancels operation. 

**TUI Command**:  *model.addWire(Part_doc, Shapes)*

**Arguments**:   Part document + list of shapes.

Result
""""""

The result of the operation will be a wire created from selected shapes:

.. image:: images/CreateWire.png
  :align: center

.. centered::
  Result of the operation.

**See Also** a sample TUI Script of a :ref:`tui_create_wire` operation.
