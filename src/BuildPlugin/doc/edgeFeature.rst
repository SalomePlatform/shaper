
Edge
====

Edge feature creates one or several edges using shapes already existing in other objects.

To create edges in the active part:

#. select in the Main Menu *Build - > Edge* item  or
#. click **Edge** button in the toolbar

.. image:: images/feature_edge.png
  :align: center

.. centered::
  **Edge** button
  
The following property panel will be opened:

.. image:: images/Edge.png
  :align: center

.. centered::
  Create edges

Select one or several edges in the viewer.

**Apply** button creates edges.

**Cancel** button cancels the operation. 

**TUI Command**:  *model.addEdge(Part_doc, Shapes)*

**Arguments**:   Part document + list of shapes.

Result
""""""

The result of the operation will be a set of edges created from the selected shapes:

.. image:: images/CreateEdge.png
  :align: center

.. centered::
  Result of the operation.

**See Also** a sample TUI Script of :ref:`tui_create_edge` operation.
