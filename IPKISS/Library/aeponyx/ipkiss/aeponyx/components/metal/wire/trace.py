from ipkiss3.pcell.wiring.window_trace import ElectricalWindowWireTemplate
from ipkiss3.pcell.trace.window.window import PathTraceWindow
from ipkiss.process import PPLayer, ProcessProperty
from ipkiss.technology import get_technology
from ipcore.properties.predefined import PositiveNumberProperty

__all__ = ["M1WireTemplate"]

TECH = get_technology()


class M1WireTemplate(ElectricalWindowWireTemplate):

    class Layout(ElectricalWindowWireTemplate.Layout):

        def _default_width(self):
            return TECH.METAL.LINE_WIDTH

        def _default_layer(self):
            return PPLayer(process=TECH.PROCESS.M1, purpose=TECH.PURPOSE.DRAWING)

        def _default_windows(self):

            return [PathTraceWindow(layer=self.layer,
                                    start_offset=-self.width / 2.0,
                                    end_offset=self.width / 2.0)]
