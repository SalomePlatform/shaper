
Split
=====

Split feature implements a Boolean operation splitting of main objects by parts of tool objects.

To perform Split in the active part:

#. select in the Main Menu *Features - > Split* item  or
#. click **Split** button in the toolbar

.. image:: images/bool_split.png
   :align: center

.. centered::
   **Split**  button 

The following property panel will be opened:

.. image:: images/Split.png
  :align: center

.. centered::
   **Split operation**

- **Main Objects** contains a list of objects selected in the Object Browser or in the Viewer, which will be cut and splitted by tool objects.
- **Tool Objects** contains a list of objects selected in the Object Browser or in the Viewer, which will cut and split the main objects.
- **See preview** button shows a result of the operation.

**TUI Command**:  *model.addSplit(Part_doc, mainObjects, toolObjects)*

**Arguments**:   Part + list of main objects + list of tool objects.

Result
""""""

The Result of the operation will be a shape, which is a split of tool objects with main objects:

.. image:: images/CreatedSplit.png
	   :align: center

.. centered::
   **Split created**

**See Also** a sample TUI Script of :ref:`tui_create_bool_split` operation.
