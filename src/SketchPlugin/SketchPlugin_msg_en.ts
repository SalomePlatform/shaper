<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0" language="en_US">
  <context>
    <name>Sketch</name>
    <message>
      <source>Model_FeatureValidator: Attribute "DirX" is not initialized.</source>
      <translation>Select a plane for the sketch</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "Features" is not initialized.</source>
      <translation>Sketch objects are not defined</translation>
    </message>
    
    <message>
      <source>External - GeomValidators_Face: The attribute with the %1 type is not processed</source>
      <translation>Only attribute selection can be used for the sketch face, not %1</translation>
    </message>
    <message>
      <source>External - GeomValidators_Face: The shape is not a face.</source>
      <translation>Sketch can be created only on a face</translation>
    </message>
    <message>
      <source>External - GeomValidators_Face: The shape is not a plane.</source>
      <translation>Sketch can be created only on a planar face</translation>
    </message>
    <message>
      <source>External - GeomValidators_Face: The shape is not a cylinder.</source>
      <translation>Sketch can be created only on a cylindrical face</translation>
    </message>
    <message>
      <source>External - GeomValidators_Face: The shape is not an available face.</source>
      <translation>Sketch cannot be created on the selected face</translation>
    </message>
  </context>
  
  <context>
    <name>SketchCircle</name>
    <message>
      <source>CircleRadius - GeomValidators_Positive: Double is not positive.</source>
      <translation>Radius value is not positive</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "CircleCenter" is not initialized.</source>
      <translation>A center point is not selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "FirstPoint" is not initialized.</source>
      <translation>A first point is not selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "SecondPoint" is not initialized.</source>
      <translation>A second point is not selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ThirdPoint" is not initialized.</source>
      <translation>A third point is not selected</translation>
    </message>
    <message>
      <source>Attribute "CircleCenter" is locked by modification value in the viewer.</source>
      <translation>Select a center point</translation>
    </message>
    <message>
      <source>Attribute "CircleRadius" is locked by modification value in the viewer.</source>
      <translation>Select a radius distance</translation>
    </message>
    
    <message>
      <source>CircleRadius - GeomValidators_Positive: Integer is not initialized.</source>
      <translation>Integer radius is not initialized</translation>
    </message>
    <message>
      <source>CircleRadius - GeomValidators_Positive: Integer is not positive.</source>
      <translation>Integer radius must be set</translation>
    </message>
  </context>
  
  <context>
    <name>SketchConstraintHorizontal</name>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityA" is not initialized.</source>
      <translation>Line is not selected</translation>
    </message>
    <message>
      <source>ModelAPI_StateInvalidArgument</source>
      <translation>Wrong object is selected</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The object is empty</source>
      <translation>Line is not selected</translation>
    </message>
    
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: It does not contain element with acceptable shape type. The type should be one of the next: %1</source>
      <translation>The constraint argument does not contain element with acceptable shape type. The type should be one of the next: %1</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: It has reference to an empty attribute</source>
      <translation>The constraint argument refers to nothing</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: Shape type is "%1", it should be "%2"</source>
      <translation>The constraint argument refers to %1 but must be %2</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The attribute with the %1 type is not processed</source>
      <translation>The constraint argument of type %1 is not supported</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The result is empty</source>
      <translation>The constraint argument refers to not existing item</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The shape is empty</source>
      <translation>The constraint argument refers to the empty shape</translation>
    </message>
  </context>
  
  <context>
    <name>SketchConstraintPerpendicular</name>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityA" is not initialized.</source>
      <translation>First line is not selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityB" is not initialized.</source>
      <translation>Second line is not selected</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The object is empty</source>
      <translation>First line is not selected</translation>
    </message>
    <message>
      <source>ConstraintEntityB - GeomValidators_ShapeType: The object is empty</source>
      <translation>Second line is not selected</translation>
    </message>
    
    <message>
      <source>ConstraintEntityB - GeomValidators_ShapeType: It does not contain element with acceptable shape type. The type should be one of the next: %1</source>
      <translation>The second line references to not acceptable shape type. The type should be: %1</translation>
    </message>
    <message>
      <source>ConstraintEntityB - GeomValidators_ShapeType: It has reference to an empty attribute</source>
      <translation>The second line refers to nothing</translation>
    </message>
    <message>
      <source>ConstraintEntityB - GeomValidators_ShapeType: Shape type is "%1", it should be "%2"</source>
      <translation>The second line refers to %1 but must be %2</translation>
    </message>
    <message>
      <source>ConstraintEntityB - GeomValidators_ShapeType: The attribute with the %1 type is not processed</source>
      <translation>The second line of type %1 is not supported</translation>
    </message>
    <message>
      <source>ConstraintEntityB - GeomValidators_ShapeType: The result is empty</source>
      <translation>The second line refers to not existing item</translation>
    </message>
    <message>
      <source>ConstraintEntityB - GeomValidators_ShapeType: The shape is empty</source>
      <translation>The second line refers to the empty shape</translation>
    </message>    
  </context>
  
  <context>
    <name>SketchConstraintRadius</name>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityA" is not initialized.</source>
      <translation>Circle is not selected</translation>
    </message>
    <message>
      <source>Attribute "ConstraintFlyoutValuePnt" is locked by modification value in the viewer.</source>
      <translation>Flyout value point is not clicked in the viewer</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The object is empty</source>
      <translation>Object is not selected</translation>
    </message>
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Double is not initialized.</source>
      <translation>Radius value is not positive</translation>
    </message>
    
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Double is not positive.</source>
      <translation>Radius must be positive</translation>
    </message>
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Integer is not initialized.</source>
      <translation>Integer radius must be set</translation>
    </message>
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Integer is not positive.</source>
      <translation>Integer radius must be positive</translation>
    </message>
  </context>
  
  <context>
    <name>SketchConstraintVertical</name>
    <message>
      <source>ModelAPI_StateInvalidArgument</source>
      <translation>Wrong object is selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityA" is not initialized.</source>
      <translation>Line is not selected</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The object is empty</source>
      <translation>Line is not selected</translation>
    </message>
    
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: It does not contain element with acceptable shape type. The type should be one of the next: %1</source>
      <translation>The first line references to not acceptable shape type. The type should be: %1</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: It has reference to an empty attribute</source>
      <translation>The first line refers to nothing</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: Shape type is "%1", it should be "%2"</source>
      <translation>The first line refers to %1 but must be %2</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The attribute with the %1 type is not processed</source>
      <translation>The first line of type %1 is not supported</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The result is empty</source>
      <translation>The first line refers to not existing item</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The shape is empty</source>
      <translation>The first line refers to the empty shape</translation>
    </message>
  </context>
  
  <context>
    <name>SketchLine</name>
    <message>
      <source>GeomValidators_Different: Attributes StartPoint and EndPoint are equal.</source>
      <translation>Start point and end point are the same</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "EndPoint" is not initialized.</source>
      <translation>End point is not selected</translation>
    </message>
    <message>
      <source>Attribute "EndPoint" is locked by modification value in the viewer.</source>
      <translation>Select an end point in the viewer</translation>
    </message>
    <message>
      <source>Attribute "StartPoint" is locked by modification value in the viewer.</source>
      <translation>Select a start point in the viewer</translation>
    </message>
    <message>
      <source>Attribute "StartPoint" is not initialized.</source>
      <translation>Start point is not selected</translation>
    </message>
    <message>
      <source>Attribute "EndPoint" is not initialized.</source>
      <translation>End point is not selected</translation>
    </message>
  </context>
  
  <context>
    <name>SketchPoint</name>
    <message>
      <source>Model_FeatureValidator: Attribute "PointCoordindates" is not initialized.</source>
      <translation>Point is not defined</translation>
    </message>
    <message>
      <source>Attribute "PointCoordindates" is locked by modification value in the viewer.</source>
      <translation>Select a point</translation>
    </message>
  </context>
  
  <context>
    <name>SketchRectangle</name>
    <message>
      <source>GeomValidators_Different: Attributes RectStartPoint and RectEndPoint are equal.</source>
      <translation>Start point and end point must be different</translation>
    </message>
    <message>
      <source>Attribute "RectEndPoint" is locked by modification value in the viewer.</source>
      <translation>Select an end point of the rectangle</translation>
    </message>
    <message>
      <source>Attribute "RectStartPoint" is locked by modification value in the viewer.</source>
      <translation>Select a first point of the rectangle</translation>
    </message>
    
    <message>
      <source>Model_FeatureValidator: Attribute "RectStartPoint" is not initialized.</source>
      <translation>Select a first point of the rectangle</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "RectEndPoint" is not initialized.</source>
      <translation>Select an end point of the rectangle</translation>
    </message>
  </context>

  <context>
    <name>SketchArc</name>
    <message>
      <source>ArcRadius - GeomValidators_Positive: Double is not initialized.</source>
      <translation>Radius value is not positive</translation>
    </message>
    <message>
      <source>ArcTangentPoint - SketchPlugin_ArcTangentPoint: The attribute ArcTangentPoint should be a point</source>
      <translation>Arc tangent point is not selected</translation>
    </message>
    <message>
      <source>Attribute "ArcCenter" is locked by modification value in the viewer.</source>
      <translation>Select a center point</translation>
    </message>
    <message>
      <source>Attribute "ArcCenter" is not initialized.</source>
      <translation>Center point is not defined</translation>
    </message>
    <message>
      <source>Attribute "ArcEndPoint" is locked by modification value in the viewer.</source>
      <translation>Select an end point</translation>
    </message>
    <message>
      <source>Attribute "ArcPassedPoint" is locked by modification value in the viewer.</source>
      <translation>Select an intermediate point</translation>
    </message>
    <message>
      <source>Attribute "ArcStartPoint" is locked by modification value in the viewer.</source>
      <translation>Select a start point</translation>
    </message>
    <message>
      <source>GeomValidators_Different: Attributes ArcCenter and ArcStartPoint are equal.</source>
      <translation>Center point and end point must be different</translation>
    </message>
    <message>
      <source>GeomValidators_Different: Attributes ArcStartPoint and ArcEndPoint are equal.</source>
      <translation>Start point and end point must be different</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ArcEndPoint" is not initialized.</source>
      <translation>End point is not defined</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ArcPassedPoint" is not initialized.</source>
      <translation>Intermediate point is not defined</translation>
    </message>
    
    <message>
      <source>ArcRadius - GeomValidators_Positive: Double is not positive.</source>
      <translation>Acr radius must e positive</translation>
    </message>
    <message>
      <source>ArcRadius - GeomValidators_Positive: Integer is not initialized.</source>
      <translation>Integer arc radius must be set</translation>
    </message>
    <message>
      <source>ArcRadius - GeomValidators_Positive: Integer is not positive.</source>
      <translation>Integer arc radius must be positive</translation>
    </message>    
  </context>

  <context>
    <name>SketchConstraintMirror</name>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The object is empty</source>
      <translation>Line is not selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityA" is not initialized.</source>
      <translation>Mirror line is not selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintMirrorList" is not initialized.</source>
      <translation>Objects for mirror are not selected</translation>
    </message>
    
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: It does not contain element with acceptable shape type. The type should be one of the next: %1</source>
      <translation>The mirror line references to not acceptable shape type. The type should be: %1</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: It has reference to an empty attribute</source>
      <translation>The mirror line refers to nothing</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: Shape type is "%1", it should be "%2"</source>
      <translation>The mirror line refers to %1 but must be %2</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The attribute with the %1 type is not processed</source>
      <translation>The mirror line of type %1 is not supported</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The result is empty</source>
      <translation>The mirror line refers to not existing item</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The shape is empty</source>
      <translation>The mirror line refers to the empty shape</translation>
    </message>
  </context>

  <context>
    <name>SketchMultiRotation</name>
    <message>
      <source>Model_FeatureValidator: Attribute "AngleType" is not initialized.</source>
      <translation>Type of angle is not defined</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "MultiRotationCenter" is not initialized.</source>
      <translation>Rotation center is not selected</translation>
    </message>
    <message>
      <source>MultiRotationCenter - GeomValidators_ShapeType: The object is empty</source>
      <translation>Rotation center is not selected</translation>
    </message>
    
    <message>
      <source>MultiRotationObjects - GeomValidators_Positive: Double is not initialized.</source>
      <translation>The total number of rotated objects is not set</translation>
    </message>
    <message>
      <source>MultiRotationObjects - GeomValidators_Positive: Double is not positive.</source>
      <translation>The total number of rotated objects must be positive</translation>
    </message>
    <message>
      <source>MultiRotationObjects - GeomValidators_Positive: Integer is not initialized.</source>
      <translation>The total number of rotated objects is not set</translation>
    </message>
    <message>
      <source>MultiRotationObjects - GeomValidators_Positive: Integer is not positive.</source>
      <translation>The total number of rotated objects must be positive</translation>
    </message>
  </context>
  
  <context>
    <name>SketchMultiTranslation</name>
    <message>
      <source>Model_FeatureValidator: Attribute "MultiTranslationEndPoint" is not initialized.</source>
      <translation>End point of translation vector is not defined</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "MultiTranslationList" is not initialized.</source>
      <translation>Objects for translation are not selected</translation>
    </message>
    <message>
      <source>MultiTranslationEndPoint - GeomValidators_ShapeType: The object is empty</source>
      <translation>End point of translation vector is not selected</translation>
    </message>
    <message>
      <source>MultiTranslationStartPoint - GeomValidators_ShapeType: The object is empty</source>
      <translation>Start point of translation vector is not selected</translation>
    </message>
    
    <message>
      <source>MultiTranslationObjects - GeomValidators_Positive: Double is not initialized.</source>
      <translation>The total number of translated objects is not set</translation>
    </message>
    <message>
      <source>MultiTranslationObjects - GeomValidators_Positive: Double is not positive.</source>
      <translation>The total number of translated objects must be positive</translation>
    </message>
    <message>
      <source>MultiTranslationObjects - GeomValidators_Positive: Integer is not initialized.</source>
      <translation>The total number of translated objects is not set</translation>
    </message>
    <message>
      <source>MultiTranslationObjects - GeomValidators_Positive: Integer is not positive.</source>
      <translation>The total number of translated objects must be positive</translation>
    </message>
  </context>

  <context>
    <name>SketchConstraintAngle</name>
    <message>
      <source>Attribute "ConstraintFlyoutValuePnt" is locked by modification value in the viewer.</source>
      <translation>Flyout value point is not clicked in the viewer</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The object is empty</source>
      <translation>First object is not selected</translation>
    </message>
    <message>
      <source>ConstraintEntityB - GeomValidators_ShapeType: The object is empty</source>
      <translation>Second object is not selected</translation>
    </message>
    
    <message>
      <source>ConstraintEntityB - SketchPlugin_ExternalValidator: Both features, attribute and attribute in parameter, are external.</source>
      <translation>An angle can not be created between two external objects</translation>
    </message>
  </context>

  <context>
    <name>SketchConstraintCoincidence</name>
    <message>
      <source>ConstraintEntityA - PartSet_DifferentObjects: The feature uses one  object in ConstraintEntityA and ConstraintEntityB attributes.</source>
      <translation>Diferent objects should be selected</translation>
    </message>
    <message>
      <source>ConstraintEntityB - PartSet_DifferentObjects: The feature uses one  object in ConstraintEntityB and ConstraintEntityA attributes.</source>
      <translation>Diferent objects should be selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityA" is not initialized.</source>
      <translation>First object is not selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityB" is not initialized.</source>
      <translation>Second object is not selected</translation>
    </message>
    
    <message>
      <source>ConstraintEntityB - SketchPlugin_ExternalValidator: Both features, attribute and attribute in parameter, are external.</source>
      <translation>A coincidence can not be created between two external objects</translation>
    </message>    
  </context>

  <context>
    <name>SketchConstraintDistance</name>
    <message>
      <source>Attribute "ConstraintFlyoutValuePnt" is locked by modification value in the viewer.</source>
      <translation>Flyout value point is not clicked in the viewer</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The object is empty</source>
      <translation>First object is not selected</translation>
    </message>
    <message>
      <source>ConstraintEntityB - GeomValidators_ShapeType: The object is empty</source>
      <translation>Second object is not selected</translation>
    </message>
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Double is not initialized.</source>
      <translation>Distance value must be set</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityA" is not initialized.</source>
      <translation>First object is not selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityB" is not initialized.</source>
      <translation>Second object is not selected</translation>
    </message>
    
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Double is not positive.</source>
      <translation>Distance value must be positive</translation>
    </message>
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Integer is not initialized.</source>
      <translation>Integer distance value must be set</translation>
    </message>
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Integer is not positive.</source>
      <translation>Integer distance value must be positive</translation>
    </message>    
  </context>

  <context>
    <name>SketchConstraintEqual</name>
    <message>
      <source>ConstraintEntityA - PartSet_DifferentObjects: The feature uses one  object in ConstraintEntityA and ConstraintEntityB attributes.</source>
      <translation>Diferent objects should be selected</translation>
    </message>
    <message>
      <source>ConstraintEntityB - PartSet_DifferentObjects: The feature uses one  object in ConstraintEntityB and ConstraintEntityA attributes.</source>
      <translation>Diferent objects should be selected</translation>
    </message>
    <message>
      <source>ConstraintEntityB - SketchPlugin_EqualAttr: An empty object is used.</source>
      <translation>An empty object is used</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityA" is not initialized.</source>
      <translation>First object is not selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityB" is not initialized.</source>
      <translation>Second object is not selected</translation>
    </message>
    
    <message>
      <source>ConstraintEntityB - SketchPlugin_ExternalValidator: Both features, attribute and attribute in parameter, are external.</source>
      <translation>An equality can not be set between two external objects</translation>
    </message>    
  </context>

  <context>
    <name>SketchConstraintLength</name>
    <message>
      <source>Attribute "ConstraintFlyoutValuePnt" is locked by modification value in the viewer.</source>
      <translation>Flyout value point is not clicked in the viewer</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The object is empty</source>
      <translation>Object is not selected</translation>
    </message>
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Double is not initialized.</source>
      <translation>Set the length value</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityA" is not initialized.</source>
      <translation>Line is not selected</translation>
    </message>
    
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Double is not positive.</source>
      <translation>Length value is not positive</translation>
    </message>
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Integer is not initialized.</source>
      <translation>Set the integer length value</translation>
    </message>
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Integer is not positive.</source>
      <translation>Integer length value is not positive</translation>
    </message>    
  </context>

  <context>
    <name>SketchConstraintMiddle</name>
    <message>
      <source>ConstraintEntityA - PartSet_DifferentObjects: The feature uses one  object in ConstraintEntityA and ConstraintEntityB attributes.</source>
      <translation>Diferent objects should be selected</translation>
    </message>
    <message>
      <source>ConstraintEntityB - PartSet_DifferentObjects: The feature uses one  object in ConstraintEntityB and ConstraintEntityA attributes.</source>
      <translation>Diferent objects should be selected</translation>
    </message>
    <message>
      <source>ConstraintEntityB - SketchPlugin_MiddlePointAttr: Middle point constraint allows points and lines only</source>
      <translation>Not point or line is selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityA" is not initialized.</source>
      <translation>First object is not selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityB" is not initialized.</source>
      <translation>Second object is not selected</translation>
    </message>
    
    <message>
      <source>ConstraintEntityB - SketchPlugin_ExternalValidator: Both features, attribute and attribute in parameter, are external.</source>
      <translation>An middle can not be set for two external objects</translation>
    </message>
  </context>

  <context>
    <name>SketchConstraintParallel</name>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The object is empty</source>
      <translation>First object is not selected</translation>
    </message>
    <message>
      <source>ConstraintEntityB - GeomValidators_ShapeType: The object is empty</source>
      <translation>Second object is not selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityA" is not initialized.</source>
      <translation>First line is not selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityB" is not initialized.</source>
      <translation>Second line is not selected</translation>
    </message>
    
    <message>
      <source>ConstraintEntityB - SketchPlugin_ExternalValidator: Both features, attribute and attribute in parameter, are external.</source>
      <translation>An parallel can not be set between two external objects</translation>
    </message>
  </context>

  <context>
    <name>SketchConstraintRigid</name>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The object is empty</source>
      <translation>Object is not selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityA" is not initialized.</source>
      <translation>Object is not selected</translation>
    </message>
    
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: It does not contain element with acceptable shape type. The type should be one of the next: %1</source>
      <translation>The fixed constraint references to not acceptable shape type. The type should be: %1</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: It has reference to an empty attribute</source>
      <translation>The fixed constraint refers to nothing</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: Shape type is "%1", it should be "%2"</source>
      <translation>The fixed constraint refers to %1 but must be %2</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The attribute with the %1 type is not processed</source>
      <translation>The fixed constraint of type %1 is not supported</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The result is empty</source>
      <translation>The fixed constraint refers to not existing item</translation>
    </message>
    <message>
      <source>ConstraintEntityA - GeomValidators_ShapeType: The shape is empty</source>
      <translation>The fixed constraint refers to the empty shape</translation>
    </message>
  </context>

  <context>
    <name>SketchConstraintTangent</name>
    <message>
      <source>ConstraintEntityA - PartSet_DifferentObjects: The feature uses one  object in ConstraintEntityA and ConstraintEntityB attributes.</source>
      <translation>Diferent objects should be selected</translation>
    </message>
    <message>
      <source>ConstraintEntityB - PartSet_DifferentObjects: The feature uses one  object in ConstraintEntityB and ConstraintEntityA attributes.</source>
      <translation>Diferent objects should be selected</translation>
    </message>
    <message>
      <source>ConstraintEntityB - SketchPlugin_TangentAttr: It uses an empty object</source>
      <translation>Selection is not valid</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityA" is not initialized.</source>
      <translation>First object is not selected</translation>
    </message>
    <message>
      <source>Model_FeatureValidator: Attribute "ConstraintEntityB" is not initialized.</source>
      <translation>Second object is not selected</translation>
    </message>
  </context>
  
  <context>
    <name>SketchConstraintFillet</name>
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Double is not initialized.</source>
      <translation>Set the fillet radius</translation>
    </message>
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Double is not positive.</source>
      <translation>The fillet radius must be positive</translation>
    </message>
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Integer is not initialized.</source>
      <translation>Set the integer fillet radius</translation>
    </message>
    <message>
      <source>ConstraintValue - GeomValidators_Positive: Integer is not positive.</source>
      <translation>The integer fillet radius must be positive</translation>
    </message>
  </context>
  
  <context>
    <name>SketchProjection</name>
    <message>
      <source>ExternalFeature - SketchPlugin_ProjectionValidator: The attribute with the %1 type is not processed</source>
      <translation>An argument of type %1 of the projection feature is not supported</translation>
    </message>
    <message>
      <source>ExternalFeature - SketchPlugin_ProjectionValidator: The attribute %1 should be an edge</source>
      <translation>The projected item must be an edge</translation>
    </message>
    <message>
      <source>ExternalFeature - SketchPlugin_ProjectionValidator: There is no sketch referring to the current feature</source>
      <translation>The projection feature has no sketch</translation>
    </message>
  </context>
</TS>
