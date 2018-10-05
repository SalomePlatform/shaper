
Wire
====

Wire feature creates a wire using edges already existing in other objects.

To create a wire in the active part:

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
  
Select one or several edges in the viewer. For automatic selection of a closed contour starting from the selected edge it is necessary to press **Add contour** button.

**Apply** button creates a wire.

**Cancel** button cancels the operation. 

**TUI Command**:  *model.addWire(Part_doc, Shapes)*

**Arguments**:   Part document + list of shapes.

Result
""""""

The result of the operation will be a wire created from the selected shapes:

.. image:: images/CreateWire.png
  :align: center

.. centered::
  Result of the operation.

**See Also** a sample TUI Script of :ref:`tui_create_wire` operation.
