
Compound
========

Compound feature creates a compound using shapes already existing in other objects.

To create a compound in the active part:

#. select in the Main Menu *Build - > Compound* item  or
#. click **Compound** button in the toolbar

.. image:: images/feature_compound.png
  :align: center

.. centered::
  **Compound** button

The following property panel will be opened:

.. image:: images/Compound.png
  :align: center

.. centered::
  Create a compound

Select one or several shapes in the viewer. The selected shapes can be of any type (vertex, edge, face, etc.)

**Apply** button creates a compound.

**Cancel** button cancels the operation. 

**TUI Command**:  *model.addCompound(Part_doc, Shapes)*

**Arguments**:   Part document + a list of shapes.

Result
""""""

The result of the operation will be a compound created from the selected shapes:

.. image:: images/CreateCompound.png
  :align: center

.. centered::
  Result of the operation.

**See Also** a sample TUI Script of :ref:`tui_create_compound` operation.
