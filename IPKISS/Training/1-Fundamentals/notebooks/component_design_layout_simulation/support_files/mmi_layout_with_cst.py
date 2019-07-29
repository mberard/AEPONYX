from ipkiss3 import all as i3
from picazzo3.traces.wire_wg import WireWaveguideTemplate
from luceda_cst.model import CapheModelCST


class SimpleMMI(i3.PCell):

    trace_template = i3.TraceTemplateProperty(doc="Trace template of the access waveguide")

    def _default_trace_template(self):
        return WireWaveguideTemplate()

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

    class Netlist(i3.NetlistFromLayout):
        pass

    class CapheModelCST(CapheModelCST):

        """
        This model encapsulates all the properties of the simulation. This way we consolidate simulation settings.
        """

        def _default_start_wavelength(self):
            return 1.45e-6

        def _default_stop_wavelength(self):
            return 1.55e-6

        def _default_cells_per_wave(self):
            return 6  # 10 for more precise results

        def _default_additional_commands_path(self):
            import os
            return r"{}\additional_commands.mod".format(os.path.dirname(__file__))  # Extra settings set in the CST GUI

        def _default_bounding_box(self):
            si = self.extended_layout.size_info()
            w = self.layout.width
            bounding_box = [[si.west, si.east],  # [startx, stopx], propagation direction
                            [-w / 2.0 - 0.7, +w / 2.0 + 0.7],  # [starty,stopy]
                            [0.15, 0.5 + 0.22 + 0.6]]  # [startz, stopz], layer direction

            return bounding_box
