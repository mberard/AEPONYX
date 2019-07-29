__all__ = ["VIA_M1_M2"]

import ipkiss3.all as i3
from ipkiss3.pcell.via.cell import ElectricalVia


@i3.lock_properties()
class VIA_M1_M2(ElectricalVia):

    def _default_name(self):
        return "VIA_M1_M2"

    class Layout(ElectricalVia.Layout):

        def _default_top_shape(self):
            return i3.TECH.V12.TOP_SHAPE

        def _default_bottom_shape(self):
            return i3.TECH.V12.BOTTOM_SHAPE

        def _default_via_shape(self):
            return i3.TECH.V12.VIA_SHAPE

        def _default_top_layer(self):
            return i3.TECH.PPLAYER.M2.LINE

        def _default_bottom_layer(self):
            return i3.TECH.PPLAYER.M1.LINE

        def _default_via_layer(self):
            return i3.TECH.PPLAYER.V12.PILLAR
