import numpy as np
from scipy import interpolate
from ipkiss3 import all as i3
import pylab as plt

yvalues = [0.5, 0.5, 0.6, 0.7, 0.9, 1.26, 1.4, 1.4, 1.4, 1.31, 1.2, 1.2]
points = []
dist = 2.0


class SplinedShape(i3.Shape):

    """
    Shape based on a spline on a series of waypoint_shape
    """

    waypoint_shape = i3.ShapeProperty(doc="Regular shape containing the waypoint_shape along which the spline will be done")
    resolution = i3.FractionProperty(default=0.01, doc="Determines the resolution of the discretisation the closer to zero the higher the discretization")
    points = i3.PointsDefinitionProperty()

    def _default_points(self, pts):
        from scipy import interpolate
        x = self.waypoint_shape.x_coords()
        y = self.waypoint_shape.y_coords()
        tck, u = interpolate.splprep([x, y], s=0)
        unew = np.arange(0, 1 + self.resolution, self.resolution)
        out = interpolate.splev(unew, tck)
        points = np.zeros([len(out[0]), 2])
        points[:, 0] = out[0]
        points[:, 1] = out[1]

        points = [i3.Coord2(x, y) for x, y in zip(out[0], out[1])]
        return points

    def __example(self):

        for i, y in enumerate(yvalues):
            x = dist / len(yvalues) * (i + 1)
            points.append((x, y))

        waypoint_shape = i3.Shape(points=points)
        splined_shape = SplinedShape(waypoint_shape=waypoint_shape)

        plt.figure()
        plt.plot(waypoint_shape.x_coords(), waypoint_shape.y_coords(), 'x', splined_shape.x_coords(), splined_shape.y_coords())
        plt.legend(['Orig', 'Interp'])
        plt.title('Spline of parametrically-defined curve')
        plt.show()
