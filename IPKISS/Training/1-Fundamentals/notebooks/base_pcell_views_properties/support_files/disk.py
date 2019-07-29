import ipkiss3.all as i3
import math
# We will define a RingResonator PCell which consists of
# a ring waveguide with evanescent coupling to a bus waveguide.


class DiskResonator(i3.PCell):
    """A Disk resonator with a wrap-around waveguide.
    """
    class Layout(i3.LayoutView):
        radius = i3.PositiveNumberProperty(default=10.0, doc="radius of the disk")
        spacing = i3.PositiveNumberProperty(default=0.6, doc="spacing between centerline of bus waveguide and edge of the disk")
        wg_length = i3.PositiveNumberProperty(doc="length of the coupling waveguide")

        def _generate_elements(self, elems):
            # define the disk as pure geometric elements
            elems += i3.Circle(layer=i3.TECH.PPLAYER.WG.CORE,
                               radius=self.radius
                               )
            elems += i3.Circle(layer=i3.TECH.PPLAYER.WG.CLADDING,
                               radius=self.radius + i3.TECH.WG.TRENCH_WIDTH
                               )
            return elems
