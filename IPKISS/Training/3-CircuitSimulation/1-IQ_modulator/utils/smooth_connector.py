import numpy as np
from ipkiss3 import all as i3

from routing.routing_functions import sbend

def get_waveguide_shape(start_port, end_port, way_points=[], distance_straight=10.0):
    original_points = [start_port.position, start_port.move_polar_copy(distance_straight, start_port.angle).position]
    original_points.extend(way_points)
    original_points.extend([end_port.move_polar_copy(distance_straight, end_port.angle).position, end_port.position])
    original_shape = i3.Shape(points=original_points, start_face_angle=start_port.angle,
                              end_face_angle=end_port.angle + 180.0)
    s2 = i3.ShapeFitClampedCubicSpline(original_shape=original_shape, discretisation=1)

    return s2



from routing.routing_functions import get_template
def smooth_wav(start_port, end_port,name=None, way_points=[], distance_straight=10.0, connector_kwargs={}):
    tt = get_template(start_port,end_port)
    shape= get_waveguide_shape(start_port=start_port, end_port=end_port, way_points=way_points,distance_straight=distance_straight)
    cell = i3.Waveguide(name=name, trace_template=tt)
    cell.Layout(shape=shape)
    return cell

from scipy.optimize import minimize

def get_waypoints(start_port, end_port, distance_straight):
    points = [start_port.position.move_polar_copy(distance_straight, start_port.angle),
              end_port.position.move_polar_copy(distance_straight, end_port.angle),
             ]

    return points

def sbend_fixed_length_delta(start_port, end_port,  initial_straight=20.0, length_delta=0.0,
                             max_error=1e-2, connector_kwargs={},name="Waveguide"):
    i_wp = get_waypoints(start_port,end_port,initial_straight)
    length = get_waveguide_shape(start_port=start_port, end_port=end_port, way_points=i_wp,
                                    distance_straight=0.0).length() + length_delta


    def to_minimize(straight):
        way_points = get_waypoints(start_port,end_port,straight)

        shape = get_waveguide_shape(start_port=start_port, end_port=end_port, way_points=way_points,
                                    distance_straight=0.0)

        trace_length = shape.length()
        #print shape.length()

        return np.abs(trace_length - length)


    res = minimize(to_minimize, x0=initial_straight, method='nelder-mead', options={'xtol': max_error, 'disp': False})
    waypoints_final = get_waypoints(start_port,end_port, res.x[0])
    shape_final = get_waveguide_shape(start_port=start_port, end_port=end_port, way_points=waypoints_final,
                  distance_straight=0.0)

    from routing.routing_functions import get_template
    tt =get_template(start_port,end_port)
    wav = i3.Waveguide(name=name, trace_template=tt)
    wav.Layout(shape=shape_final)
    return wav