# Creation of a first LayoutView

from technologies import silicon_photonics
from ipkiss3 import all as i3


### Solution ###
class RectangleWithLayout(i3.PCell):

    class Layout(i3.LayoutView):
        length = i3.PositiveNumberProperty(default=5.0, doc="length")
        width = i3.PositiveNumberProperty(doc="width")

        def _default_width(self):
            return self.length

        def _generate_elements(self, elems):

            shape_rect = i3.ShapeRectangle(box_size=(self.length, self.width))
            elems += i3.Boundary(layer=i3.TECH.PPLAYER.WG.CORE, shape=shape_rect)

            return elems


my_rect = RectangleWithLayout()
my_rect_layout = my_rect.Layout(length=10.0, width=5.0)
my_rect_layout.visualize()
