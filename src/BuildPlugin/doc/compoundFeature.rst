
Compound
========

The Compound feature creates a compound using already existing shapes in other objects.

To create compound in the active part:

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

Select one or several shapes in a viewer. Selected shapes can be of any type (vertex, edge, face and so on).

**Apply** button creates compound.

**Cancel** button cancels operation. 

**TUI Command**:  *model.addCompound(Part_doc, Shapes)*

**Arguments**:   Part document + list of shapes.

Result
""""""

The result of the operation will be a compound created from selected shapes:

.. image:: images/CreateCompound.png
  :align: center

.. centered::
  Result of the operation.

**See Also** a sample TUI Script of a :ref:`tui_create_compound` operation.
