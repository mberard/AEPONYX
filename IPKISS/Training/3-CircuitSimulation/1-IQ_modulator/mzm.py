if __name__  == "__main__":
    from aeponyx import technology
import ipkiss3.all as i3
from aeponyx.all import MMI_12, PhaseShifterWaveguide
from routing.apac import APAC
from routing.routing_functions import sbend
from utils.smooth_connector import sbend_fixed_length_delta



class ShiftedMZI(APAC):

    length_phase_shifter = i3.PositiveNumberProperty(default=125.0, doc="Length of the phase shifter")
    length_modulator = i3.PositiveNumberProperty(default=2500.0, doc="Length of the modulator")
    spacing_y = i3.PositiveNumberProperty(default=100.0,doc="spacing in y direction")
    spacing_x = i3.PositiveNumberProperty(doc="spacing in the x direction between splitter and the combiner")
    center_wavelength = i3.PositiveNumberProperty(default=1.55, doc="center wavelength")
    splitter = i3.ChildCellProperty(doc="Splitter used")
    phaseshifter = i3.ChildCellProperty(doc="Phaseshfiter used", locked=True)
    modulator = i3.ChildCellProperty(doc="Modulator used")
    def _default_splitter(self):
        cell = MMI_12(name=self.name + "MMI")
        return cell

    def _default_spacing_x(self):
        splitter_w = self.splitter.get_default_view(i3.LayoutView).size_info().width
        return 300 + self.length_modulator + 2*splitter_w

    def _default_phaseshifter(self):
        shape = [(0.0, 0.0), (self.length_phase_shifter, 0.0)]
        cell = PhaseShifterWaveguide(name=self.name + "_phaseshifter")
        cell.Layout(shape=shape)
        return cell

    def _default_modulator(self):
        shape = [(0.0, 0.0), (self.length_modulator,0.0)]
        cell = PhaseShifterWaveguide(name=self.name + "_modulator")
        lv = cell.Layout(shape=shape)
        return cell

    def _default_child_cells(self):
        return {
            'splitter': self.splitter,
            'combiner': self.splitter,
            'phaseshifter': self.phaseshifter,
            'modulator1': self.modulator,
            'modulator2': self.modulator,
        }

    def _default_connectors(self):

        from utils.smooth_connector import sbend_fixed_length_delta
        from functools import partial
        cm = self.modulator.get_default_view(i3.CircuitModelView).model
        dpi = cm.center_wavelength / (2.0 * cm.n_eff)
        c1 = partial(sbend_fixed_length_delta, length_delta=dpi/4.0)
        c2 = partial(sbend_fixed_length_delta, length_delta=-dpi/4.0)
        return [
            ('splitter:out1', 'modulator1:in', c1),
            ('splitter:out2', 'modulator2:in', c2),
            ('combiner:out2', 'modulator2:out', c2),
            ('combiner:out1', 'modulator1:out', c1),
        ]

    def _default_links(self):
        p1 = "splitter:in1"
        p2 = "phaseshifter:out"

        return [(p1, p2)]


    def _default_propagated_electrical_ports(self):
        return ["mod1p", "mod1m", "mod2p","mod2m", "psm", "psp"]

    def _default_external_port_names(self):
        return {
            "phaseshifter:in": "in",
            "combiner:in1": "out",
            "modulator1:elec1": "mod1m",
            "modulator1:elec2": "mod1p",
            "modulator2:elec1": "mod2m",
            "modulator2:elec2": "mod2p",
            "phaseshifter:elec1": "psm",
            "phaseshifter:elec2": "psp"

        }

    class Layout(APAC.Layout):

        def _default_child_transformations(self):
            return {
                'splitter': (-(self.spacing_x / 2), 0),
                'combiner': i3.HMirror() + i3.Translation((self.spacing_x / 2, 0)),
                'modulator1': i3.VMirror() + i3.Translation(translation=(-self.length_modulator/2.0, -self.spacing_y / 2 )),
                'modulator2':  i3.Translation((-self.length_modulator/2.0, self.spacing_y / 2 )),
            }



if __name__  == "__main__":
    cell = ShiftedMZI(name="ShiftedMZI", length_phase_shifter=300.0)
    lv = cell.Layout()
    lv.visualize(annotate=True)
    lv.write_gdsii("shifted_mzi.gds")