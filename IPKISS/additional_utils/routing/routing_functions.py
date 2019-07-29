
from ipkiss3 import all as i3
from ipkiss3.all import RouteManhattan
from route_through_control_points import RouteManhattanControlPoints
from ipkiss3 import all as i3
import warnings


def get_template(start_port, end_port):
    if start_port is not None:
        trace_template = start_port.trace_template
    elif end_port is not None:
        trace_template = end_port.trace_template
    else:
        trace_template = i3.TECH.PCELLS.WG.DEFAULT

    cw1 = start_port.trace_template.core_width
    cw2 = end_port.trace_template.core_width
    cw3 = trace_template.core_width

    if cw3 != cw1:
        warnings.warn("A waveguide core may not match at location {}".format(start_port.position))

    if cw3 != cw2:
        warnings.warn("A waveguide core may not match at location {}".format(end_port.position))

    return trace_template



def route_sbend(start_port, end_port, bend_radius=20.0):
    """
    Calculates an sbend between ports, returns the route and the used bend radius
    """
    ports = start_port, end_port
    points = []
    points.append(start_port.position)
    points.append(start_port.position.move_polar_copy(bend_radius, start_port.angle))
    points.append(end_port.position.move_polar_copy(bend_radius, end_port.angle))
    points.append(end_port.position)

    return i3.Shape(points)


def route_manhattan(start_port, end_port, bend_radius=20.0,control_points=[]):
    return RouteManhattanControlPoints(input_port=start_port, output_port=end_port,control_points=control_points, bend_radius=bend_radius)


def sbend(start_port, end_port, name=None, shape=None,  connector_kwargs={}):

    trace_template = get_template(start_port, end_port)

    if "bend_radius" in connector_kwargs:
        bend_radius = connector_kwargs["bend_radius"]
    else:
        bend_radius = 20.0

    if shape == None:
        shape = route_sbend(start_port=start_port, end_port=end_port, bend_radius=bend_radius)

    wav = i3.RoundedWaveguide(trace_template=trace_template,name=name)

    lv = wav.Layout(bend_radius=bend_radius, shape=shape)

    return wav

# Routemanhattan


def manhattan(start_port, end_port, name=None, control_points=[], shape=None, connector_kwargs={}):

    trace_template = get_template(start_port,end_port)

    if "bend_radius" in connector_kwargs:
        bend_radius = connector_kwargs["bend_radius"]
    else:
        bend_radius = 20.

    if shape is None:
        shape = route_manhattan(start_port=start_port, end_port=end_port, bend_radius=bend_radius,control_points=control_points)

    wav = i3.RoundedWaveguide(trace_template=trace_template, name=name)
    wav.Layout(bend_radius=bend_radius, shape=shape)

    return wav




def wide_manhattan(start_port, end_port, name=None,  shape=None,control_points=[],  connector_kwargs={}):

    route = [start_port, end_port]

    trace_template = get_template(start_port, end_port)

    if "bend_radius" in connector_kwargs:
        bend_radius = connector_kwargs["bend_radius"]
    else:
        bend_radius = 20.

    if "taper_length" in connector_kwargs:
        taper_length = connector_kwargs["taper_length"]
    else:
        taper_length = 10.

    if "width_wide" in connector_kwargs:
        width_wide = connector_kwargs["width_wide"]
    else:
        width_wide = 3.

    if shape is None:
        shape = RouteManhattanControlPoints(input_port=start_port, output_port=end_port, bend_radius=bend_radius, control_points=control_points)

    from ipkiss3.pcell.photonics.expanded_waveguide import ExpandedWaveguide

    wav = ExpandedWaveguide(trace_template=trace_template, name=name)
    wav.Layout(shape=shape,
               narrow_width=trace_template.core_width,
               expanded_width=width_wide,
               taper_length=taper_length,
               expanded_layer=trace_template.core_layer
               )

    return wav
