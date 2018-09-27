
Pipe
====

The feature **Pipe** makes an extruded Pipe shape.

To create Pipe in the active part:

#. select in the Main Menu *Features - > Pipe* item  or
#. click **Pipe** button in the toolbar

.. image:: images/pipe.png        
   :align: center

.. centered::
   **Pipe** button 

Pipe can be created in three different ways:

  .. image:: images/pipe_simple_32x32.png    
    :align: left
  by object and path 

  .. image:: images/pipe_binormal_32x32.png      
    :align: left
  by object, path and Bi-normal  

  .. image:: images/pipe_locations_32x32.png    
    :align: left
  by object, path and locations 

Pipe by object and path
-----------------------

Base Objects are extruded along the Path Object so that the angle between the normal vector to the base shape and the tangent to the path remain constant at any point of the given path. 

.. image:: images/Pipe1.png
  :align: center

.. centered::
  Pipe by object and path property panel

Input fields:

- **Base objects** panel contains shapes to be extruded. Several Base Objects generate several pipes. Shapes (edges, faces, shells) are selected in 3D OCC viewer or object browser;
- **Path object**  defines path along which the Base Object will be extruded.  **Path object** (edge or wire) is selected in 3D OCC viewer or object browser;

**TUI Command**:  *model.addPipe(Part_doc, [shape], path)*
 
**Arguments**: part + list of shapes in format *model.selection(TYPE, shape)* + path in format *model.selection(TYPE, shape)*.

Result
""""""

Result of extrusion of arc along another arc.

.. image:: images/simplePipe.png
   :align: center

.. centered::
   Pipe by object and path

**See Also** a sample TUI Script of a :ref:`tui_simple_pipe` operation.

Pipe by object, path and Bi-normal
----------------------------------

The pipe is generated to preserve the constant angle between the normal vector to the base shape and the BiNormal vector at any point of the given path. 

.. image:: images/Pipe2.png
  :align: center

.. centered::
  Pipe by object, path and Bi-normal property panel

Input fields:

- **Base objects** panel contains shapes to be extruded. Several Base Objects generate several pipes. Shapes (edges, faces, shells) are selected in 3D OCC viewer or object browser;
- **Path object**  defines path along which the Base Object will be extruded.  **Path object** (edge or wire) is selected in 3D OCC viewer or object browser;
- **Bi-Normal** defines the BiNormal Vector. **Bi-Normal** (edge or wire) is selected in 3D OCC viewer or object browser.

**TUI Command**:  *model.addPipe(Part_doc, [shape], path, binormal)*
 
**Arguments**: part + list of shapes in format *model.selection(TYPE, shape)* + path in format *model.selection(TYPE, shape)* + binormal in format *model.selection(TYPE, shape)*.

Result
""""""

Result of extrusion of rectangular face along  arc.

.. image:: images/binormalPipe.png
   :align: center

.. centered::
   Pipe by object, path and Bi-normal

**See Also** a sample TUI Script of a :ref:`tui_binormal_pipe` operation.

Pipe by object, path and locations
----------------------------------

The pipe is generated to preserve selected profiles at the specified locations along path.

.. image:: images/Pipe3.png
  :align: center

.. centered::
  Pipe by object, path and locations property panel

Input fields:

- **Base objects** panel contains shapes to be extruded.  Shapes (edges, faces, shells) are selected in 3D OCC viewer or object browser;
- **Path object**  defines path along which the Base Object will be extruded.  **Path object** (edge or wire) is selected in 3D OCC viewer or object browser;
- **Locations** define list of vertices that specify the locations of extruded Base Objects on the resulting Path Object. The number of Base Objects should be equal to the number of Locations.

**TUI Command**:  *model.addPipe(Part_doc, [shape], path, locations)*
 
**Arguments**: part + list of shapes in format *model.selection(TYPE, shape)* + path in format *model.selection(TYPE, shape)* + list of locations in format *model.selection(TYPE, shape)*.

Result
""""""

Result of extrusion of 3 circles along arc with specified locations.

.. image:: images/locationPipe.png
   :align: center

.. centered::
   Pipe by object, path and locations

**See Also** a sample TUI Script of a :ref:`tui_locations_pipe` operation.