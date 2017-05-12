# Quick3D
Qt based 3D engine with geo-referenced entities

This used to be a very simple 3D engine that could show a 3D terrain and objects.
But it has been pushed much further and is now a general purpose engine.

## Features

* Global terrain with automatic level of detail. Can be generated from real earth elevation data or procedurally and can be populated with vegetation.
* Geo-referenced objects, with parent-child relationship
* Q3D file format, with a python script to export from Blender
* Scenes are setup in a XML file

## Things to work on

### Terrain
* Tile seams : for now, seams between terrain tiles are visible
* Z-fighting : should use an adaptive z-buffer depth to avoid this
* Loading : sometimes tiles simply don't show up
* Vegetation rendering : vegetation objects (and all alpha containing objects) should be rendered last to avoid alpha problems
* Vegetation spawning : still some bug in this algorithm

### Camera
* Frustum : sometimes objects disappear because they not considered inside the viewing frustum (although we know they are)

### Materials
* PBR rendering : materials must be refactored in order to allow for more physically based rendering