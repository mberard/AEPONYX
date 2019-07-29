"""For more information about more complex PN Phase Shifter trace template:
http://docs.lucedaphotonics.com/3.1/picazzo/modulators/phase/ref/picazzo3.modulators.phase.trace.LateralPNPhaseShifterTemplate.html
"""

__all__ = ['PhaseShifterTemplate', 'PhaseShifterWaveguide']

import ipkiss3.all as i3

from picazzo3.modulators.phase.trace import PhaseShifterWaveguideTemplate

from ...compactmodels import DC_Phase_ModModel

class PhaseShifterTemplate(PhaseShifterWaveguideTemplate):
    ''' PN junction trace template with parameters to set up the implants and the contact.
    '''
    trace_template = i3.WaveguideTemplateProperty(locked=True)

    def _default_trace_template(self):
        return SiWireWaveguideTemplate(name="{}_wg_tt".format(self.name))

    class Layout(PhaseShifterWaveguideTemplate.Layout):
        core_width = i3.PositiveNumberProperty(doc="width of the core")
        p_width = i3.PositiveNumberProperty(doc="width of low dose P implant")
        p_offset = i3.NumberProperty(doc="offset from the center line of low dose P implant")
        n_width = i3.PositiveNumberProperty(doc="width of low dose N implant")
        n_offset = i3.NumberProperty(doc="offset from the center line of low dose N implant")
        high_dose_p_width = i3.PositiveNumberProperty(doc="width of high dose P implant")
        high_dose_p_offset = i3.NumberProperty(doc="offset from the center line of high dose P implant")
        high_dose_n_width = i3.PositiveNumberProperty(doc="width of high dose N implant")
        high_dose_n_offset = i3.NumberProperty(doc="offset from the center line of high dose N implant")
        con_width = i3.PositiveNumberProperty(doc="width of contacts etch (same on both side of the center line)")
        con_p_offset = i3.NumberProperty(doc="offset from the center line of contacts etch on P implant")
        con_n_offset = i3.NumberProperty(doc="offset from the center line of contacts etch on N implant")

        def _default_core_width(self):
            return 0.45

        def _default_p_width(self):
            return 3.

        def _default_p_offset(self):
            return self.p_width/2.

        def _default_n_width(self):
            return self.p_width

        def _default_n_offset(self):
            return -self.n_width/2.

        def _default_high_dose_p_width(self):
            return 2.

        def _default_high_dose_p_offset(self):
            return self.p_width + self.high_dose_p_width/2.

        def _default_high_dose_n_width(self):
            return self.high_dose_p_width

        def _default_high_dose_n_offset(self):
            return -self.high_dose_p_offset

        def _default_con_width(self):
            return self.high_dose_p_width/2.

        def _default_con_p_offset(self):
            return self.high_dose_p_offset

        def _default_con_n_offset(self):
            return self.high_dose_n_offset

        def _default_trace_template(self):
            lv = self.cell.trace_template.get_default_view(self)
            lv.set(core_width=self.core_width)
            return lv

        def _default_trace_template_for_ports(self):
            port_tt = SiWireWaveguideTemplate(name="{}_wg_port_tt".format(self.name))
            port_tt_lv = port_tt.Layout(core_width=self.core_width)
            return port_tt_lv

        def _default_implant_windows(self):
            core_half_width = 0.5 * self.core_width
            p_half_width = 0.5 * self.p_width
            n_half_width = 0.5 * self.n_width
            high_dose_p_half_width = 0.5 * self.high_dose_p_width
            high_dose_n_half_width = 0.5 * self.high_dose_n_width

            p_offset = self.p_offset
            n_offset = self.n_offset
            high_dose_p_offset = self.high_dose_p_offset
            high_dose_n_offset = self.high_dose_n_offset

            windows = []
            # core
            windows.append(i3.PathTraceWindow(layer=i3.TECH.PPLAYER.SI,
                                              start_offset=-core_half_width,
                                              end_offset=+core_half_width))
            # low_dose_p
            windows.append(i3.PathTraceWindow(layer=i3.TECH.PPLAYER.P,
                                              start_offset=p_offset - p_half_width,
                                              end_offset=p_offset + p_half_width))
            # low_dose_n
            windows.append(i3.PathTraceWindow(layer=i3.TECH.PPLAYER.N,
                                              start_offset=n_offset - n_half_width,
                                              end_offset=n_offset + n_half_width))
            # high_dose_p
            windows.append(i3.PathTraceWindow(layer=i3.TECH.PPLAYER.PPLUS,
                                              start_offset=high_dose_p_offset - high_dose_p_half_width,
                                              end_offset=high_dose_p_offset + high_dose_p_half_width))
            # high_dose_n
            windows.append(i3.PathTraceWindow(layer=i3.TECH.PPLAYER.NPLUS,
                                              start_offset=high_dose_n_offset - high_dose_n_half_width,
                                              end_offset=high_dose_n_offset + high_dose_n_half_width))
            return windows

        def _default_contact_windows(self):
            con_half_width = 0.5 * self.con_width
            con_p_offset = self.con_p_offset
            con_n_offset = self.con_n_offset

            windows = []
            # Contact on P++
            windows.append(i3.ExtendedPathTraceWindow(layer=i3.TECH.PPLAYER.CON,
                                                      start_offset=con_p_offset - con_half_width,
                                                      end_offset=con_p_offset + con_half_width,
                                                      extension=(-0.5, -0.5)))
            # Contact on N++
            windows.append(i3.ExtendedPathTraceWindow(layer=i3.TECH.PPLAYER.CON,
                                                      start_offset=con_n_offset - con_half_width,
                                                      end_offset=con_n_offset + con_half_width,
                                                      extension=(-0.5, -0.5)))
            return windows

        def _default_metal_windows(self):
            windows = []
            return windows

        def _default_windows(self):
            windows = self.implant_windows + self.contact_windows + self.metal_windows
            return windows

    class CircuitModel(PhaseShifterWaveguideTemplate.CircuitModel):

        VpiLpi = i3.PositiveNumberProperty(default=1.0, doc="Voltage to get pi-shift for 1 cm of phase modulator [V.cm]")

        def _default_loss_dB_m(self):
            return 300.0

        def _default_neff(self):
            return 2.5

        def _default_n_g(self):
            return 4.1




class PhaseShifterWaveguide(i3.RoundedWaveguide):
    def _default_trace_template(self):
        return PhaseShifterTemplate(name="{}_tt".format(self.name))

    class Layout(i3.RoundedWaveguide.Layout):
        def _default_bend_radius(self):
            return 25.

        def _generate_ports(self, ports):
            ports = super(PhaseShifterWaveguide.Layout, self)._generate_ports( ports)
            center_pos = 0.5*ports["in"].position + 0.5*ports["out"].position


            ports += i3.ElectricalPort(name='elec1', position=center_pos+(0,self.trace_template.con_n_offset),layer=i3.TECH.PPLAYER.M1)
            ports += i3.ElectricalPort(name='elec2', position=center_pos+(0,self.trace_template.con_p_offset),layer=i3.TECH.PPLAYER.M1)

            return ports


    class CircuitModel(i3.CircuitModelView):
        length = i3.PositiveNumberProperty(doc="Length of the phase shifter", locked=True)

        def _default_length(self):
            return self.cell.get_default_view(i3.LayoutView).trace_length()


        def _generate_model(self):
            return DC_Phase_ModModel(length=self.length,
                                     VpiLpi=self.trace_template.VpiLpi,
                                     n_eff=self.trace_template.n_eff,
                                     center_wavelength=self.trace_template.center_wavelength,
                                     n_g=self.trace_template.n_g,
                                     loss_dB_m=self.trace_template.loss_dB_m)


    class Netlist(i3.NetlistFromLayout):
        pass


