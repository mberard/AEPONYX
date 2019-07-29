from aeponyx import technology
import numpy as np
from ipkiss3 import all as i3
from euler90_rounding_algorithm import Euler90Algorithm

from aeponyx.components.waveguides.wire import SWG450
from picazzo3.wg.spirals import FixedLengthSpiralRounded

# defining waveguides
swg = SWG450()

class MyFixedSpiral(FixedLengthSpiralRounded):

    class Layout(FixedLengthSpiralRounded.Layout):

        def _default__dummy_spiral(self):
            lv = self.cell._dummy_spiral.get_default_view(i3.LayoutView)
            lv.set(inner_size=self._dummy_inner_size,
                   spacing=self.spacing,
                   bend_radius=self.bend_radius,
                   stub_direction=self.stub_direction,
                   incoupling_length=self.incoupling_length,
                   rounding_algorithm=self.rounding_algorithm)

            return lv


cell = MyFixedSpiral(total_length=9000, n_o_loops=3, trace_template=swg)

layout = cell.Layout(incoupling_length=10.0,
                     bend_radius=11.0,
                     rounding_algorithm=Euler90Algorithm,
                     spacing=5.,
                     stub_direction="H",  # either H or V
                     growth_direction="H"  # either H or V
                     )


print layout.trace_length()

layout.visualize()

layout.write_gdsii("spiral.gds")

print 'done'
