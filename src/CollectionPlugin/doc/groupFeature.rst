
Group
=====

A groups is a set of geometrical entities created by selection.
To create a Group in the active part:

#. select in the Main Menu *Features - > Group* item  or
#. click **Group** button in Shaper toolbar:

.. image:: images/shape_group.png
  :align: center

.. centered::
   Group button

The following property panel appears. 

.. image:: images/group_property_panel.png
  :align: center

.. centered::
  Create a group

Input fields:

- **Name** defines the name of the group, by default, it is **Group_n**.
- **Type** buttons select geometrical entities to be included in group:
  
    .. image:: images/group_property_panel_vertice.png
         :align: left
    Vertices;
  
    .. image:: images/group_property_panel_edge.png
         :align: left
    Edges;

    .. image:: images/group_property_panel_face.png
         :align: left
    Faces;

    .. image:: images/group_property_panel_solid.png
         :align: left	       
    Solids;
 
- The list of selected entities of the given type.  Multiple selection can be done manually in OCC 3D Viewer by mouse click with Shift button pressed or by rectangle selection.

To delete entities from the list, select them and call pop-up menu *Delete* item.


**TUI Command**: *model.addGroup(Part_1_doc, [model.selection("EDGE", "Cone_1_1/Face_1&Cone_1_1/Face_2"), model.selection("EDGE", "Cone_1_1/Face_1")])*

**Arguments**:  1 part + list of selected entities with type


Result
""""""

Created Group appears in the view.

.. image:: images/group_res.png
	   :align: center

.. centered::
   Created group

**See Also** a sample TUI Script of :ref:`tui_create_group` operation.
