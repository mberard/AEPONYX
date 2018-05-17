# PolygonCSV

This is a L-EDIT UPI Macro to import CSV into polygons inside a L-Edit Cell

Imports the desired CSV file

### CSV File

* For a **vertex without curve**:

```
Shape,Geometry,x,y
```
&nbsp;&nbsp;&nbsp;&nbsp;\- Shape: *LShapeType* (LBox, LCircle, LWire...)\
&nbsp;&nbsp;&nbsp;&nbsp;\- Geometry: *LGeomType* (LOrthogonal, LFortyFive,LCurved...)\
&nbsp;&nbsp;&nbsp;&nbsp;\- x: double\
&nbsp;&nbsp;&nbsp;&nbsp;\- y: double

* For a **vertex with curve or any other shape**:

```
Shape,Geometry,x,y,x1,y1,r,r2,startAngle,stopAngle,dir,text,WireWidth,WireJoin,WireCap,WireMiterAngle

```
&nbsp;&nbsp;&nbsp;&nbsp;\- Shape: *LShapeType* (LBox, LCircle, LWire...)\
&nbsp;&nbsp;&nbsp;&nbsp;\- Geometry: *LGeomType* (LOrthogonal, LFortyFive,LCurved...)\
&nbsp;&nbsp;&nbsp;&nbsp;\- x: *double* point\
&nbsp;&nbsp;&nbsp;&nbsp;\- y: *double* point\
&nbsp;&nbsp;&nbsp;&nbsp;\- x1: *double* center of the shape **or** second point\
&nbsp;&nbsp;&nbsp;&nbsp;\- y1: *double* center of the shape **or** second point\
&nbsp;&nbsp;&nbsp;&nbsp;\- r: *double* raduis\
&nbsp;&nbsp;&nbsp;&nbsp;\- r2: *double* second radius (torus)\
&nbsp;&nbsp;&nbsp;&nbsp;\- startAngle: *double* firstangle (torus, pie)\
&nbsp;&nbsp;&nbsp;&nbsp;\- stopAngle: *double* second angle (torus, pie)\
&nbsp;&nbsp;&nbsp;&nbsp;\- dir: *LArcDirection* (CW or CCW)\
&nbsp;&nbsp;&nbsp;&nbsp;\- text: *String* text\
&nbsp;&nbsp;&nbsp;&nbsp;\- WireWidth: *double* width of a wire\
&nbsp;&nbsp;&nbsp;&nbsp;\- WireJoin: *LJoinType* wire join type (miter, round, bevel, layout)\
&nbsp;&nbsp;&nbsp;&nbsp;\- WireCap: *LCapType* wire cap type (butt, round, extend)\
&nbsp;&nbsp;&nbsp;&nbsp;\- WireMiterAngle: *double* wire miter angle


* For a **label**:

```
LLabel,x,y,textSize,textAlignment,text
```
&nbsp;&nbsp;&nbsp;&nbsp;\- x: *double* point\
&nbsp;&nbsp;&nbsp;&nbsp;\- y: *double* point\
&nbsp;&nbsp;&nbsp;&nbsp;\- textSize: *double* text size\
&nbsp;&nbsp;&nbsp;&nbsp;\- textAlignment: *int* text alignment\
&nbsp;&nbsp;&nbsp;&nbsp;\- text: *String* text



## Authors

* **Martin Berard** - *Initial work* - [mberard](https://github.com/mberard)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone who's code was used
* Inspiration
* etc
