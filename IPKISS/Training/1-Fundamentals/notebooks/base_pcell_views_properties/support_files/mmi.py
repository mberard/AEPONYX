from ipkiss3 import all as i3
from picazzo3.traces.wire_wg import WireWaveguideTemplate
from picazzo3.logical.coupler import Coupler1x2


class SimpleMMI(Coupler1x2):

    trace_template = i3.TraceTemplateProperty(doc="Trace template of the access waveguide")

    def _default_trace_template(self):
        return i3.TECH.PCELLS.WG.DEFAULT

    class Layout(i3.LayoutView):
        # properties
        width = i3.PositiveNumberProperty(default=4.0, doc="Width of the MMI section.")
        length = i3.PositiveNumberProperty(default=20.0, doc="Length of the MMI secion.")
        taper_width = i3.PositiveNumberProperty(default=1.0, doc="Width of the taper.")
        taper_length = i3.PositiveNumberProperty(default=2.0, doc="Length of the taper")
        waveguide_spacing = i3.PositiveNumberProperty(default=2.0, doc="Spacing between the waveguides.")

        # methods
        def _generate_elements(self, elems):
            elems += i3.Rectangle(layer=i3.TECH.PPLAYER.WG.CORE,
                                  center=(0.5 * self.length, 0.0),
                                  box_size=(self.length, self.width))
            elems += i3.Wedge(layer=i3.TECH.PPLAYER.WG.CORE,
                              begin_coord=(-self.taper_length, 0.0),
                              end_coord=(0.0, 0.0),
                              begin_width=self.trace_template.core_width,
                              end_width=self.taper_width
                              )
            elems += i3.Wedge(layer=i3.TECH.PPLAYER.WG.CORE,
                              begin_coord=(self.length, 0.5 * self.waveguide_spacing),
                              end_coord=(self.length + self.taper_length, 0.5 * self.waveguide_spacing),
                              begin_width=self.taper_width,
                              end_width=self.trace_template.core_width
                              )
            elems += i3.Wedge(layer=i3.TECH.PPLAYER.WG.CORE,
                              begin_coord=(self.length, -0.5 * self.waveguide_spacing),
                              end_coord=(self.length + self.taper_length, -0.5 * self.waveguide_spacing),
                              begin_width=self.taper_width,
                              end_width=self.trace_template.core_width
                              )

            elems += i3.Rectangle(layer=i3.TECH.PPLAYER.WG.CLADDING,
                                  center=(0.5 * self.length, 0.0),
                                  box_size=(self.length + 2 * self.taper_length,  self.width + 2.0)
                                  )
            return elems

        def _generate_ports(self, ports):

            ports += i3.OpticalPort(name="in", position=(-self.taper_length, 0.0), angle=180.0, trace_template=self.trace_template)
            ports += i3.OpticalPort(name="out1", position=(self.length + self.taper_length, -0.5 * self.waveguide_spacing), angle=0.0,
                                    trace_template=self.trace_template)
            ports += i3.OpticalPort(name="out2", position=(self.length + self.taper_length, +0.5 * self.waveguide_spacing), angle=0.0,
                                    trace_template=self.trace_template)

            return ports
        
        
    class CircuitModel(Coupler1x2.CircuitModel):
        pass
