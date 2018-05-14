# PolygonCSV

This is a L-EDIT UPI Macro to import CSV into polygons inside a L-Edit Cell

Imports the desired CSV file

### CSV File

For a vertex without curve:

```
x,y
```
For a vertex with curve or any other shape:

```
x,y,type,cx,cy,r,r2,startAngle,stopAngle,dir
```
x,y: vertex point

type:
* 0: no curve
* 1: curved polygon
* 2: circle
* 3: torus
* 4: pie

cx,cy: coord of the center of the shape

r: radius of the shape

r2: second radius of the shape (torus)

startAngle,stopAngle: first and second angle (torus, pie)

dir: LArcDirection CW or CCW (for curved polygon)

For a port:

```
x,y,5,x1,y1
```

For a wire:

```
x,y,6,width,joinType,capType,miterAngle
```
## Authors

* **Martin Berard** - *Initial work* - [mberard](https://github.com/mberard)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone who's code was used
* Inspiration
* etc
