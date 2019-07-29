if __name__  == "__main__":
    from aeponyx import technology
import ipkiss3.all as i3

from routing.apac import APAC
from aeponyx.all import MMI_12
from mzm import ShiftedMZI



class IQModulator(APAC):

    splitter = i3.ChildCellProperty(doc="Splitter used")
    length_phase_shifter = i3.PositiveNumberProperty( doc="Length of the phase shifter")
    length_modulator = i3.PositiveNumberProperty(doc="Length of the modulator")
    modulator = i3.ChildCellProperty(doc="Modulator used")
    spacing_y = i3.PositiveNumberProperty(default=100.0, doc="spacing in y direction")
    center_wavelength = i3.PositiveNumberProperty(default=1.55, doc="center_wavelength of the modulator")

    def _default_length_modulator(self):
        vpipi = 1.5*1e4
        import numpy as np
        lpi = vpipi
        return lpi/2.0

    def _default_length_phase_shifter(self):
        return self.length_modulator/6.0


    def _default_splitter(self):
        cell = MMI_12(name=self.name+"_MMI")
        return cell

    def _default_modulator(self):
        cell = ShiftedMZI(name=self.name + "_shifted_mzi",
                          splitter=self.splitter,
                          length_phase_shifter=self.length_phase_shifter,
                          length_modulator=self.length_modulator,
                          spacing_y=self.spacing_y)
        return cell


    def _default_child_cells(self):
        return {
            'splitter': self.splitter,
            'combiner': self.splitter,
            'modulator1': self.modulator,
            'modulator2': self.modulator,
        }


    def get_delta_pi2(self):
        tt = self.modulator.phaseshifter.trace_template
        tt_cm = tt.get_default_view(i3.CircuitModelView)
        neff = tt_cm.n_eff
        return self.center_wavelength/(4.0 * neff)


    def _default_connectors(self):
        from utils.smooth_connector import sbend_fixed_length_delta
        from functools import partial

        c1 = partial(sbend_fixed_length_delta, length_delta=-self.get_delta_pi2()/4.0)
        c2 = partial(sbend_fixed_length_delta, length_delta=+self.get_delta_pi2()/4.0)

        return [
            ('splitter:out1', 'modulator1:in', c1),
            ('splitter:out2', 'modulator2:in', c2),
            ('combiner:out2', 'modulator2:out', c2),
            ('combiner:out1', 'modulator1:out', c1),
        ]



    def _default_propagated_electrical_ports(self):
        pp = []
        for cnt in range(1,3):
            for pn in ["p", "m"]:
                pp.append( "m{}ps_{}".format(cnt, pn))
                for mod in range(1,3):
                    pp.append( "m{}{}_{}".format(cnt, mod, pn))

        return pp


    def _default_external_port_names(self):
        epn={"splitter:in1": "in",
             "combiner:in1": "out"}

        for cnt in range(1,3):
            for pn in ["p", "m"]:
                epn.update({"modulator{}:ps{}".format(cnt,pn):"m{}ps_{}".format(cnt,pn)})
                for mod in range(1,3):
                    epn.update({"modulator{}:mod{}{}".format(cnt, mod,pn):"m{}{}_{}".format(cnt, mod,pn)})
        return epn


    class Layout(APAC.Layout):

        def _default_child_transformations(self):
            si_m = self.modulator.size_info()
            s_x = si_m.width  + 600.0
            s_y = si_m.height + 50.0
            center = 0.5*(self.modulator.ports["in"].position + self.modulator.ports["out"].position)
            return {
                'splitter': (-(s_x / 2), 0),
                'combiner': i3.HMirror() + i3.Translation(((s_x ) / 2,0.0)),
                'modulator1': i3.VMirror() +  i3.Translation((-center[0] , -s_y / 2 )),
                'modulator2':  i3.Translation((-center[0] , s_y / 2 )),
            }

if __name__  == "__main__":
    cell = IQModulator(name="IQModulator",length_phase_shifter=1000.0)
    lv = cell.Layout()
    lv.visualize(annotate=True)
    lv.write_gdsii("iq_modulator.gds")