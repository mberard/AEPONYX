if __name__ == "__main__":
    from aeponyx import technology

from ipkiss3.pcell.photonics.waveguide import WindowWaveguideTemplate, TemplatedWindowWaveguide
from picazzo3.traces.wire_wg.transitions import LinearWindowWaveguideTransition
from scipy.interpolate import  RectBivariateSpline
from ipkiss.technology import get_technology
from numpy import sqrt, exp, pi, random


from ipkiss3.pcell.layout.view import LayoutView
from ipkiss3.simulation.engines.caphe_circuit_sim.pcell_views.caphemodel import CircuitModelView
from ipkiss3.pcell.model.model import CompactModel
from ipkiss3.pcell.photonics.term import OpticalTerm
from ipkiss3.pcell.trace.window.window import PathTraceWindow
from ipcore.properties.predefined import PositiveNumberProperty, NonNegativeNumberProperty
from ipcore.properties.predefined import NumpyArrayProperty
import numpy as np
from ipkiss3.pcell.cell.template import _ViewTemplate

TECH = get_technology()


class WaveguideModel(CompactModel):
    _model_type = 'python'

    parameters = [
        'n_effs',
        'wavelengths',
        'length',
        'phase_error'
    ]

    terms = [OpticalTerm(name='in'), OpticalTerm(name='out')]

    def calculate_smatrix(parameters, env, S):

        neff_total = np.interp(env.wavelength, parameters.wavelengths, np.real(parameters.n_effs)) +\
                     1j * np.interp(env.wavelength, parameters.wavelengths, np.imag(parameters.n_effs))

        S['in', 'out'] = S['out', 'in'] = exp(1j *((2 * pi / env.wavelength * neff_total * parameters.length) +
                                                    parameters.phase_error *  sqrt(parameters.length)))



class GenericWaveguide(TemplatedWindowWaveguide):

    class CircuitModel(TemplatedWindowWaveguide.CircuitModel):

        def _generate_model(self):
            tt_cm = self.cell.template.get_default_view(CircuitModelView)
            tt_lv = self.cell.template.get_default_view(LayoutView)
            wav_lv = self.cell.get_default_view(LayoutView)

            neffs = np.array([tt_cm._get_n_eff_for_wavelength_and_width(wl,tt_lv.core_width)
                              for wl in tt_cm.wavelengths])
            return WaveguideModel(length=wav_lv.trace_length(),
                                  n_effs=neffs,
                                  wavelengths=tt_cm.wavelengths,
                                  phase_error=tt_cm.get_phase_error())


class GenericWaveGuideTemplate(WindowWaveguideTemplate):

    _templated_class =  GenericWaveguide

    class Layout(WindowWaveguideTemplate.Layout):

        def _default_core_layer(self):
            return TECH.PPLAYER.SI

        def _default_core_width(self):
            return 0.5

        def _default_windows(self):
            windows = [PathTraceWindow(layer=self.core_layer,
                                          start_offset=-0.5*self.core_width,
                                          end_offset=0.5*self.core_width)]
            return windows

    class CircuitModel(CircuitModelView, _ViewTemplate):

        phase_error_correlation_length = NonNegativeNumberProperty(doc="Correlation length (in um) of the sidewall roughness")
        phase_error_width_deviation = NonNegativeNumberProperty(doc="Standard deviation (amplitude) of the sidewall roughness")
        wavelengths = NumpyArrayProperty(doc="Wavelength points for interpolation.")
        widths = NumpyArrayProperty(doc="Widths points for the interpolation.")
        n_eff_values = NumpyArrayProperty(doc="Complex refractive index as a function of wavelength and width.")
        center_wavelength = PositiveNumberProperty(doc="Center wavelength of the waveguide  which is used when for calculations of n_g, loss_dbm or n_eff when no wavelength is specified")

        def _default_phase_error_width_deviation(self):
            return 0.000

        def _default_phase_error_correlation_length(self):
            return 0.0

        def _default_center_wavelength(self):
            return (max(self.wavelengths) + min(self.wavelengths))/2.0


        def _default_n_eff_values(self):

            n_eff_values= np.array([[2.24378704, 2.53735656, 2.66001948, 2.71562179, 2.75466422, 2.77430224],
                                   [2.2091054, 2.51351447, 2.64091618, 2.69873319, 2.7392829, 2.7597774],
                                   [2.17442376, 2.48967238, 2.62181287, 2.68184458, 2.72390157,2.74525256],
                                   [2.13974212, 2.46583029, 2.60270956, 2.66495597, 2.70852025,2.73072772],
                                   [2.10506048, 2.44198821, 2.58360625, 2.64806737, 2.69313892, 2.71620288]])

            n_eff_values = n_eff_values + 0.001*1j # Adding some loss. Here non disperisve material loss.

            return n_eff_values

        def _default_wavelengths(self):
            return np.linspace(1.5, 1.6, 5)

        def _default_widths(self):
            return np.linspace(0.4, 1.2, 6)


        def _get_n_eff_interps(self):
            r =  RectBivariateSpline(self.wavelengths,
                                     self.widths,
                                     np.real(self.n_eff_values),
                                     bbox=[self.wavelengths[0]*0.9, self.wavelengths[-1]*1.1, self.widths[0]*0.9, self.widths[-1]*1.1])

            c = RectBivariateSpline(self.wavelengths,
                                    self.widths,
                                    np.imag(self.n_eff_values),
                                    bbox=[self.wavelengths[0]*0.9, self.wavelengths[-1]*1.1, self.widths[0]*0.9, self.widths[-1]*1.1])

            return r, c

        def _get_n_eff_for_wavelength_and_width(self, wavelength, width):
            r,c = self._get_n_eff_interps()
            return float(r(wavelength, width)) + 1j*float(c(wavelength,width))

        def _get_dndw(self):
            """ The change in index as function of width for the center wavelength """
            wl = self.center_wavelength
            width = self.cell.get_default_view(LayoutView).core_width
            w1 = width-0.005
            w2 = width+0.005
            n1 = np.real(self._get_n_eff_for_wavelength_and_width(wl, w1))
            n2 = np.real(self._get_n_eff_for_wavelength_and_width(wl, w2))
            return (n2-n1)/(w2-w1)


        def get_phase_error(self):
            """returns sigma(phase error)/sqrt(um), sigma_dw given in nm at the center wavelength"""
            if self.phase_error_width_deviation>0 and self.phase_error_correlation_length>0:
                dbeta_dw = 2 * pi * self._get_dndw() / self.center_wavelength
                sigma_pe = np.sqrt(2 * self.phase_error_correlation_length * (dbeta_dw ** 2) * (self.phase_error_width_deviation ** 2))
                return random.normal(0.0, sigma_pe)
            else:
                return 0.0
        def get_n_g(self, environment=None):
            """ Returns the actual group index of the waeguide cross section for all the parameters in the Environment
            (e.g. wavelength, temperature). Use this method to retrieve the group index, rather than the property 'n_g'
            """
            if environment is None:
                wavelength = self.center_wavelength
            else:
                wavelength = environment.wavelength

            dwav = 0.001
            w1 = wavelength+dwav/2.0
            w2 = wavelength-dwav/2.0
            width = self.cell.get_default_view(LayoutView).core_width
            n1 = np.real(self._get_n_eff_for_wavelength_and_width(w1, width))
            n2 = np.real(self._get_n_eff_for_wavelength_and_width(w2, width))

            ng = (w2*n1-w1*n2)/(w2-w1)

            return ng

        def get_loss_dB_m(self, environment=None):
            """ Returns the propagation loss of the waeguide cross section for all the parameters in the Environment
            (e.g. wavelength, temperature). Use this method to retrieve the loss, rather than the property 'loss_dB_m'
            """

            if environment is None:
                wavelength = self.center_wavelength
            else:
                wavelength = environment.wavelength

            width = self.cell.get_default_view(LayoutView).core_width
            n1 = self._get_n_eff_for_wavelength_and_width(wavelength, width)

            trans_1m = np.exp(2*np.pi*-np.imag(n1)*1e6/wavelength)
            db_1m = 20*np.log10(trans_1m)



            return db_1m


        def get_n_eff(self, environment=None):

            if environment is None:
                wavelength = self.center_wavelength
            else:
                wavelength = environment.wavelength

            width = self.cell.get_default_view(LayoutView).core_width
            n1 = self._get_n_eff_for_wavelength_and_width(wavelength, width)
            return n1

class GenericTransition(LinearWindowWaveguideTransition):
    pass


if __name__ == "__main__":
    from ipkiss3 import all as i3

    tt = GenericWaveGuideTemplate()
    wav = i3.RoundedWaveguide(trace_template=tt)
    lv = wav.Layout(shape=[(0.0, 0.0), (10.0, 0.0), (10.0, 10.0)], manhattan=True)
    lv.visualize(annotate=True)
    import numpy as np
    wavs = np.linspace(1.5, 1.6, 100)
    cm = wav.CircuitModel()
    S = cm.get_smatrix(wavelengths=wavs)
    import pylab as plt
    plt.plot(wavs, np.abs(S["in","out"]))
    plt.show()
    plt.plot(wavs, np.unwrap(np.angle(S["in", "out"])))
    plt.show()