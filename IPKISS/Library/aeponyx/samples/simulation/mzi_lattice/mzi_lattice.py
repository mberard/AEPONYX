from aeponyx import all as aeponyx
import numpy as np
from picazzo3.routing.place_route import PlaceAndAutoRoute
from ipkiss3 import all as i3

from components.directional_coupler import DirectionalCoupler


class MZILattice(PlaceAndAutoRoute):
    """Mach-Zehnder based lattice filter with parameter n_stages.
    """

    directional_couplers = i3.ChildCellListProperty(doc="list of directional couplers", locked=True)
    n_stages = i3.PositiveIntProperty(doc="Number of stages in the lattice.")

    def _default_trace_template(self):
        return aeponyx.SiWireWaveguideTemplate()

    def _default_directional_couplers(self):
        return [
            DirectionalCoupler(name=self.name + "dc_{}".format(cnt),
                               trace_template=self.trace_template)
            for cnt in range(self.n_stages + 1)
        ]

    def _default_child_cells(self):
        return {
            "dc_{}".format(cnt): dc
            for cnt, dc in enumerate(self.directional_couplers)
        }

    def _default_links(self):
        links = []
        for cnt in range(self.n_stages):
            p1 = "dc_{}:out1".format(cnt)
            p2 = "dc_{}:in1".format(cnt+1)
            links.append((p1,p2))
            p1 = "dc_{}:out2".format(cnt)
            p2 = "dc_{}:in2".format(cnt+1)
            links.append((p1, p2))

        return links

    def _default_external_port_names(self):
        return {
            "dc_0:in1": "in1",
            "dc_0:in2": "in2",
            "dc_{}:out1".format(self.n_stages): "out1",
            "dc_{}:out2".format(self.n_stages): "out2",
        }

    class CircuitModel(PlaceAndAutoRoute.CircuitModel):
        power_couplings = i3.ListProperty(doc="List of power couplings")

        def _default_power_couplings(self):
            return [0.5, 0.5]

        def _default_directional_couplers(self):
            dc_modelviews = [
                    dc.get_default_view(self)
                    for dc in self.cell.directional_couplers
            ]

            for p, modelview in zip(self.power_couplings, dc_modelviews):
                modelview.set(power_coupling_factor=p)

            return dc_modelviews

    class Layout(PlaceAndAutoRoute.Layout):
        delay_lengths = i3.ListProperty(doc="List of delay lengths")

        def _default_delay_lengths(self):
            return [100.]

        def _default_child_transformations(self):
            return {
                "dc_{}".format(cnt): i3.Translation((cnt*50, 0))
                for cnt, dc in enumerate(self.directional_couplers)
            }

        def _default_bend_radius(self):
            return 10.0

        def _default_waveguide_shapes(self):
            shapes = []
            min_delta = max(4 * self.bend_radius, -min(self.delay_lengths))
            min_delta = 40
            for cnt, (inst_port1, inst_port2) in enumerate(self.links):
                inst1, port1 = self._resolve_inst_port(inst_port1)
                inst2, port2 = self._resolve_inst_port(inst_port2)

                if port1.angle == 90:
                    dl = self.delay_lengths[cnt / 2] / 2.0

                else:
                    dl = 0

                shape = [
                    port1.position,
                    port1.position.move_polar_copy(angle=port1.angle, distance=min_delta + dl),
                    port2.position.move_polar_copy(angle=port2.angle, distance=min_delta + dl),
                    port2.position
                ]

                shapes.append(shape)


            return shapes


tt = aeponyx.SiWireWaveguideTemplate()
tt_cm = tt.CircuitModel(n_g=4.18920116149,
                        center_wavelength=1.31,
                        n_eff=2.4579358757,
                        phase_error_sigma=np.pi / 200.)

# The filter coefficients below lead to a flat-band response.
fsr = 0.04
power_couplings = [0.5, 0.13, 0.12, 0.5, 0.25]
n_eff = tt_cm.n_eff
n_g = tt_cm.n_g
center_wavelength = tt_cm.center_wavelength
L = center_wavelength ** 2 / n_g / fsr
L_pi = center_wavelength / (2 * n_eff)

delay_lengths = [L, 2 * L, -(2 * L + L_pi), -2 * L]

cell = MZILattice(name="MziLattice", trace_template=tt, n_stages=4)
lv = cell.Layout(delay_lengths=delay_lengths)
lv.visualize(annotate=True)
lv.write_gdsii("mzi_lattice.gds")

cm = cell.CircuitModel(power_couplings=power_couplings)
wavelengths = np.linspace(1.28, 1.35, 1001)
S = cm.get_smatrix(wavelengths=wavelengths)

import pylab as plt
plt.plot(wavelengths, 20 * np.log10(np.abs(S["in1","out1"])), label='Transmission out 1')
plt.plot(wavelengths, 20 * np.log10(np.abs(S["in1","out2"])), label='Transmission out 2')
plt.ylabel("Transmission (dB)")
plt.xlabel("Wavelength ($\mu$ m)")
plt.ylim([-40, 0])
plt.show()
