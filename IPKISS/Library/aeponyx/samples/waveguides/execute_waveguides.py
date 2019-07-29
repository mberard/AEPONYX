from aeponyx import technology
from aeponyx.all import SWG450, SWG550, SWG1000, RWG450, RWG850, RoundedWireWaveguide, RoundedRibWaveguide
from ipkiss3 import all as i3

shape = [(0, 0), (300, 0), (300, 300), (600, 300), (1000, 0)]


wg1 = RoundedWireWaveguide(trace_template=SWG450())
wg1.Layout(shape=shape)

wg2 = RoundedRibWaveguide(trace_template=RWG850())
wg2.Layout(shape=shape)

lc = i3.LayoutCell()
#lay = lc.Layout(elements=[i3.SRef(wg1, position=(0, 0)),
#                          i3.SRef(wg2, position=(0, 500))])

lay = lc.Layout(elements=[i3.SRef(wg1, position=(0, 0))])

lay.write_gdsii('waveguides.gds')
lay.visualize()
