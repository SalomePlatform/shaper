.. |feature_filling.icon|    image:: images/feature_filling.png

Filling
=======

Filling feature creates one or several faces using shapes already existing in other objects.

To create a filling in the active part:

#. select in the Main Menu *Build - > Filling* item  or
#. click |feature_filling.icon| **Filling** button in the toolbar

The following property panel will be opened:

.. image:: images/Filling.png
  :align: center

.. centered::
  Create a filling

Select two or more edges in the viewer.

**Advanced options** manage filling process.

**Apply** button creates a filling.

**Cancel** button cancels the operation.

**TUI Command**:

.. py:function:: model.addFilling(Part_doc, Edges)

    :param part: The current part object.
    :param list: A list of shapes.
    :return: Result object.

Result
""""""

The result of the operation will be a set of faces created from the selected shapes:

.. image:: images/CreateFilling.png
  :align: center

.. centered::
  Result of the operation.

**See Also** a sample TUI Script of :ref:`tui_create_filling` operation.
