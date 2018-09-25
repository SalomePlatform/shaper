
Measurement
===========

The feature **Measurement** calculates dimensions of sub-elements of a geometrical object (shape).

The measured dimension is displayed in the property panel and shown in 3D OCC viewer.

The  properties of dimension text and line, such as font, color, arrow size, etc. can be defined in the Preferences  :ref:`visualization_preferences`.

**Apply** button does not generate any result and has the same effect as **Cancel** for this feature.  

To start Measurement in the active part:

#. select in the Main Menu *Part - > Measurement* item  or
#. click **Measurement** button in the toolbar

.. image:: images/measurement.png      
   :align: center

.. centered::
   **Measurement** button 

The following dimensions can be calculated:

  .. image:: images/meas_length_32x32.png      
    :align: left
  edge length 

  .. image:: images/meas_distance_32x32.png    
    :align: left
  distance between objects

  .. image:: images/meas_radius_32x32.png    
    :align: left
  radius

  .. image:: images/meas_angle_32x32.png    
    :align: left
  angle between edges  

  .. image:: images/meas_angle3p_32x32.png    
    :align: left
  angle by 3 points  

Edge length
-----------

The length can be measured for straight or curved edge built on two points. 

.. image:: images/Measurement1.png
  :align: center

.. centered::
  Edge length

Input fields:

- **Edge**  contains  edge to be measured selected in 3D OCC viewer or object browser. 

Note, that for curved edges length is displayed only in the property panel.

**TUI Command**:  *length =model.measureLength(Part_doc, edge)*
 
**Arguments**: part + edge in format *model.selection("EDGE", edge)*. 


Distance between objects
------------------------

**Distance between objects** calculates minimal distance for any pair of shapes: vertex, edge, face, solid.

 .. image:: images/Measurement2.png
  :align: center

.. centered::
  Distance between objects

Input fields:

- **From**, **To**  contain shapes between which distance is  measured. Shapes are selected in 3D OCC viewer or object browser. 

**TUI Command**:  *length =model.measureDistance(Part_doc, shape1, shape2)*
 
**Arguments**: part +  2 shapes in format *model.selection("TYPE", shape)*.

Radius
------

**Radius** calculates radius for circular edge or cylindrical face. 

.. image:: images/Measurement3.png
  :align: center

.. centered::
  Radius

Input fields:

- **Object**  contains shape to be measured selected in 3D OCC viewer or object browser.  


**TUI Command**:  *radius = model.measureRadius(Part_doc, shape)*
 
**Arguments**: part +  shape in format *model.selection("TYPE", shape)*.

Angle between edges
-------------------

**Angle between edges** calculates angle between the selected pair of edges in the point of intersection. If there are several points of intersection, then angles are calculated in all points.

.. image:: images/Measurement4.png
  :align: center

.. centered::
  Angle between edges

Input fields:

- **First Edge**, **Second edge**  contain  edges between which angle is measured. Edges are selected in 3D OCC viewer or object browser. 

**TUI Command**:  *angle = model.measureAngle(Part_doc, edge1, edge2)*
 
**Arguments**: part +  2 edges in format *model.selection("EDGE", edge)*.  

Angle by 3 points
-----------------

**Angle by 3 points** calculates angle between the selected three points. The second point is in the corner of the angle.

.. image:: images/Measurement5.png
  :align: center

.. centered::
  Angle by 3 points

Input fields:

- **First point**, **Second point**, **Third point**  contain  point between which angle is measured. Points are selected in 3D OCC viewer or object browser. 

**TUI Command**:  *angle = model.measureAngle(Part_doc, vertex1, vertex2, vertex3)*
  
**Arguments**: part +  3 vertices in format *model.selection("VERTEX", vertex)*.  
 