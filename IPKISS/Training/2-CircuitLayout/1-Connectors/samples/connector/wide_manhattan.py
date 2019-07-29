from aeponyx import technology
from aeponyx.all import SWG450
from routing.routing_functions import  RouteManhattan

def wide_manhattan(start_port, end_port, name=None, shape=None, connector_kwargs={}):

    route = [start_port, end_port]

    if start_port is not None:
        trace_template = start_port.trace_template
    elif end_port is not None:
        trace_template = end_port.trace_template
    else:
        trace_template = SWG450()

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
        shape = RouteManhattan(input_port=start_port, output_port=end_port, bend_radius=bend_radius)

    from ipkiss3.pcell.photonics.expanded_waveguide import ExpandedWaveguide

    wav = ExpandedWaveguide(trace_template=trace_template,name=name)
    wav.Layout(shape=shape,
               narrow_width=trace_template.core_width,
               expanded_width=width_wide,
               taper_length=taper_length,
               expanded_layer=trace_template.core_layer
               )

    return wav


if __name__ == "__main__":

    from ipkiss3 import all as i3
    start_port = i3.OpticalPort(position=(0.0, 0.0), angle=0.0)
    end_port = i3.OpticalPort(position=(200.0, 50.0), angle=180.0)
    cell = wide_manhattan(start_port, end_port,name="TestWaveguide")
    lv = cell.get_default_view(i3.LayoutView)
    lv.visualize()
    lv.write_gdsii("expanded.gds")
