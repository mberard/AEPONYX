# Instantiation of a Picazzo Ring Resonator

from technologies import silicon_photonics
from ipkiss3 import all as i3


### Solution ###
from picazzo3.filters.ring import RingRect180DropFilter
my_ring2 = RingRect180DropFilter()
my_ring2_layout = my_ring2.Layout(bend_radius=10.0,
                                  coupler_spacings=[1.0, 1.0])

my_ring2_layout.visualize()
