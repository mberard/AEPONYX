# PolygonCSV

This is a L-EDIT UPI Macro to import CSV into polygons inside a L-Edit Cell

Imports the desired CSV file

### CSV File

* For a **vertex without curve**:

```
x,y
```
* For a **vertex with curve or any other shape**:

```
x,y,type,cx,cy,r,r2,startAngle,stopAngle,dir
```
x,y: vertex point\
type:\
&nbsp;&nbsp;&nbsp;&nbsp;\- 0: no curve\
&nbsp;&nbsp;&nbsp;&nbsp;\- 1: curved polygon\
&nbsp;&nbsp;&nbsp;&nbsp;\- 2: circle\
&nbsp;&nbsp;&nbsp;&nbsp;\- 3: torus\
&nbsp;&nbsp;&nbsp;&nbsp;\- 4: pie\
cx,cy: coord of the center of the shape\
r: radius of the shape\
r2: second radius of the shape (torus)\
startAngle,stopAngle: first and second angle (torus, pie)\
dir: LArcDirection CW or CCW (for curved polygon)

* For a **port**:

```
x,y,5,x1,y1,text
```
x,y: lower left corner\
x1,y2: lower left corner

* For a **wire**:

```
x,y,6,width,joinType,capType,miterAngle
```
x,y: vertex point\
width: width of the wire (LCoord)\
joinType: LJoinType\
capType: LCapType\
miterAngle: LCoord
## Authors

* **Martin Berard** - *Initial work* - [mberard](https://github.com/mberard)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone who's code was used
* Inspiration
* etc
