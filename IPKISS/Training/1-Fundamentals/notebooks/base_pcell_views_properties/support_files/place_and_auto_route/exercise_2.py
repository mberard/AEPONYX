# Creation of an S-Bend

from technologies import silicon_photonics
from ipkiss3 import all as i3

### Solution ###

wg_t_2 = WireWaveguideTemplate()
wg_t_2.Layout(core_width=0.3,
              cladding_width=2 * 3.0 + 0.47)

start_point = i3.Coord2((0.0, 0.0))
end_point = i3.Coord2((20.0, 20.0))
offset = i3.Coord2(10.0, 0)
middle_point = (start_point + end_point) * 0.5
bend_radius = (end_point[0] - start_point[0]) / 2.0

shape = [start_point, start_point + offset, middle_point, end_point - offset, end_point]
wg = i3.RoundedWaveguide(trace_template=wg_t_2)
# Assign a shape to the waveguide.
layout = wg.Layout(shape=shape,
                   bend_radius=bend_radius,
                   manhattan=False,
                   draw_control_shape=True)

layout.visualize()
