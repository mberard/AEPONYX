__all__ = ["Contact", "CONTACT_STD"]

import ipkiss3.all as i3


class Contact(i3.PCell):

    class Layout(i3.LayoutView):
        diameter = i3.PositiveNumberProperty(default=i3.TECH.CON.WIDTH)

        def _generate_elements(self, elems):
            elems += i3.RegularPolygon(layer=i3.TECH.PPLAYER.SIL, radius=0.5 * self.diameter, n_o_sides=12)
            elems += i3.RegularPolygon(layer=i3.TECH.PPLAYER.CON, radius=0.5 * self.diameter, n_o_sides=12)
            return elems


@i3.lock_properties()
class CONTACT_STD(Contact):

    def _default_name(self):
        return "CONTACT_STD"

    class Layout(Contact.Layout):

        def _default_diameter(self):
            return i3.TECH.CON.WIDTH
