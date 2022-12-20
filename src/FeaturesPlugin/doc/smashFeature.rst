.. |bool_smash.icon|    image:: images/bool_smash.png
   :height: 16px

.. _featureSmash:

Smash
=====

Smash feature implements a boolean operation for smashing a set of main objects by a set of tool objects.

To create a Smash in the active part:

#. select in the Main Menu *Features - > Smash* item  or
#. click |bool_smash.icon| **Smash** button in the toolbar

The following property panel will be opened:

.. figure:: images/Smash.png
   :align: center

   **Smash operation**

- **Main Objects** contains a list of objects selected in the Object Browser or in the Viewer, which will be smashed by tool objects.
  If a subshape that belongs to a compsolid/compound was selected, other shapes of this compsolid/compound will be cut from
  other objects (to avoid self intersection) and added to the result.
- **Tool Objects** contains a list of objects selected in the Object Browser or in the Viewer, which will be smashed into main objects.
  If a subshape that belongs to a compsolid/compound was selected, other shapes of this compsolid/compound will be ignored.
- **Fuzzy Parameter** defines the additional tolerance value used to eliminate tiny results.
- **See preview** button shows a result of the operation.

Main objects and Tools objects should have the same dimension. There are supported 2-dimensional (face) or 3-dimensional (solid, compsolid) arguments.

**TUI Command**:

.. py:function:: model.addSmash(Part_doc, mainObjects, toolObjects, fuzzy)

    :param part: The current part object.
    :param list: A list of main objects.
    :param list: A list of tool objects.
    :param real: Additional tolerance used to eliminate tiny results (optional).
    :return: Rotated object.

Result
""""""

The Result of the operation will be a shape which is a smash of tool objects from main objects:

.. figure:: images/CreatedSmash.png
   :align: center

   **Smash created**

**See Also** a sample TUI Script of :ref:`tui_create_smash` operation.
