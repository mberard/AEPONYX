"""Compact models for aeponyx.
"""
import ipkiss3.all as i3
from numpy import pi, sqrt, exp, log

__all__ = [
    'AbsorberCompactModel',
    'CrossingCompactModel',
    'GratingCouplerCompactModel',
    'MMI1x2CompactModel',
    'MMI2x2CompactModel',
    'PhotoDetectorCompactModel',
    'WGPhaseErrorCompactModel',
    'MZMLumpedCompactModel'
]


class AbsorberCompactModel(i3.CompactModel):
    """Absorber, for which the reflection is defined by the parameter reflection_dB.
    """

    parameters = [
        'reflection_dB',
    ]

    terms = [
        i3.OpticalTerm(name='in'),
    ]

    def calculate_smatrix(parameters, env, S):
        S['in', 'in'] = - 10**(- parameters.reflection_dB / 20.)


class CrossingCompactModel(i3.CompactModel):
    """A simple planar crossing with 4 ports.

    - The crossing loss is defined by the parameter insertion_loss_dB
    - The cross-talk is defined by the parameter crosstalk_dB
    - The phase propagation for the straight section (east to west and north to south)
      is defined by the length of the crossing and the n_eff (the effective index).

    """

    parameters = [
        'length',
        'neff',
        'crosstalk_dB',
        'insertion_loss_dB',
        'reflection_dB',
    ]

    terms = [
        i3.OpticalTerm(name='north'),
        i3.OpticalTerm(name='east'),
        i3.OpticalTerm(name='south'),
        i3.OpticalTerm(name='west'),
    ]

    def calculate_smatrix(parameters, env, S):
        phase = 2*pi / env.wavelength * parameters.length * parameters.neff
        t = 10**(- parameters.insertion_loss_dB / 20.)
        S['south', 'north'] = S['east', 'west'] = S['west', 'east'] = S['north', 'south'] = t * exp(1j * phase)
        reflection = - 10**(- parameters.reflection_dB / 20.)
        S['east', 'east'] = S['west', 'west'] = S['south', 'south'] = S['north', 'north'] = reflection
        crosstalk = 10**(- parameters.crosstalk_dB / 20.) * exp(1j * phase)
        S['east', 'north'] = S['north', 'east'] = crosstalk
        S['east', 'south'] = S['south', 'east'] = crosstalk
        S['west', 'north'] = S['north', 'west'] = crosstalk
        S['west', 'south'] = S['south', 'west'] = crosstalk


class GratingCouplerCompactModel(i3.CompactModel):
    """Grating coupler model based on a gaussian transmission curve.

    The grating coupler transmission is a gaussian curve with a center wavelength defined
    by center_wavelength and with a 1 dB FWHM defined by the parameter bandwidth_1dB.


    """
    parameters = [
        'center_wavelength',
        'bandwidth_1dB',
        'peak_IL_dB',
        'reflection',
        'reflection_vertical_in'
    ]

    terms = [
        i3.OpticalTerm(name='vertical_in'),
        i3.OpticalTerm(name='out'),
    ]

    def calculate_smatrix(parameters, env, S):
        sigma = parameters.bandwidth_1dB/2. * sqrt(10 / (2 * log(10)))  
        # @k dB sigma = BW/2 * sqrt(10 / (2*k * log(10)))
        peak_transmission = 10**(- parameters.peak_IL_dB / 10.)
        power_S = peak_transmission * exp(-(env.wavelength - parameters.center_wavelength)**2 / (2. * (sigma**2)))
        S['vertical_in', 'out'] = S['out', 'vertical_in'] = sqrt(power_S)
        S['out', 'out'] = parameters.reflection
        S['vertical_in', 'vertical_in'] = parameters.reflection_vertical_in


class MMI1x2CompactModel(i3.CompactModel):
    """Compact model for a 1x2 MMI.
    """
    parameters = [
        'excess_loss_dB',
        'power_imbalance_dB',
        'reflection_dB'
    ]

    terms = [
        i3.OpticalTerm(name='in1'),
        i3.OpticalTerm(name='out1'),
        i3.OpticalTerm(name='out2')
    ]

    def calculate_smatrix(parameters, env, S):
        PI = 10**(parameters.power_imbalance_dB / 20.)
        EL = 10**(- parameters.excess_loss_dB / 20.)
        REF = - 10**(- parameters.reflection_dB / 20.)
        S['in1', 'out1'] = S['out1', 'in1'] = PI * EL / sqrt(1 + PI**2)
        S['in1', 'out2'] = S['out2', 'in1'] = EL / sqrt(1 + PI**2)
        S['out1', 'out1'] = S['out2', 'out2'] = S['in1', 'in1'] = REF


class MMI2x2CompactModel(i3.CompactModel):
    """Compact model for a 2x2 MMI.

    There is a pi/2 phase difference between the cross and bar output.
    """

    parameters = [
        'excess_loss_dB',
        'power_imbalance_dB',
        'reflection_dB'
    ]

    terms = [
        i3.OpticalTerm(name='in1'),
        i3.OpticalTerm(name='in2'),
        i3.OpticalTerm(name='out1'),
        i3.OpticalTerm(name='out2')
    ]

    def calculate_smatrix(parameters, env, S):
        PI = 10**(parameters.power_imbalance_dB / 20.)
        EL = 10**(- parameters.excess_loss_dB / 20.)
        REF = - 10**(- parameters.reflection_dB / 20.)
        S['out1', 'in2'] = S['in2', 'out1'] = S['out2', 'in1'] = S['in1', 'out2'] = EL / sqrt(1 + PI**2) * exp(1j * pi)
        S['out1', 'in1'] = S['in1', 'out1'] = S['out2', 'in2'] = S['in2', 'out2'] = PI * EL / sqrt(1 + PI**2) * exp(1j * pi/2.)
        S['in1', 'in1'] = S['in2', 'in2'] = S['out1', 'out1'] = S['out2', 'out2'] = REF


class PhotoDetectorCompactModel(i3.CompactModel):
    """Simple photodetector model.

    """

    parameters = [
        'R', 'BW', 'reflection_dB', 'dark_current'
    ]

    terms = [
        i3.OpticalTerm(name='in'),
        i3.ElectricalTerm(name='anode'),
        i3.ElectricalTerm(name='cathode')
    ]

    states = [
        'current'
    ]

    def calculate_smatrix(parameters, env, S):
        S['in', 'in'] = 10.**(- parameters.reflection_dB / 20.)

    def calculate_signals(parameters, env, output_signals, y, t, input_signals):
        output_signals['anode'] = y['current'] + parameters.dark_current * 1e-9
        output_signals['cathode'] = -output_signals['anode']

    def calculate_dydt(parameters, env, dydt, y, t, input_signals):
        dydt['current'] = parameters.BW * 1e9 * (parameters.R * (abs(input_signals['in'])**2) - y['current'])


class WGPhaseErrorCompactModel(i3.CompactModel):
    """Simple first-order dispersive waveguide model. Includes a fixed phase error.
    """

    parameters = [
        'n_eff', 'n_g', 'center_wavelength',
        'length', 'phase_error', 'loss_dB_m'
    ]

    loss_dB_m = 100

    terms = [
        i3.OpticalTerm(name='in'),
        i3.OpticalTerm(name='out')
    ]

    def calculate_smatrix(parameters, env, S):
        dneff = -(parameters.n_g - parameters.n_eff) / parameters.center_wavelength
        n_eff_final = parameters.n_eff + (env.wavelength - parameters.center_wavelength) * dneff
        phase = 2*pi * n_eff_final / env.wavelength * parameters.length + parameters.phase_error * sqrt(parameters.length)
        t = 10**(- parameters.loss_dB_m * parameters.length * 1e-6 / 20.)
        S['in', 'out'] = exp(1j * phase) * t
        S['out', 'in'] = exp(1j * phase) * t
        S['in', 'in'] = 0
        S['out', 'out'] = 0


class DC_Phase_ModModel(i3.CompactModel):
    parameters = ['length','n_eff', 'n_g', 'center_wavelength', 'VpiLpi', 'loss_dB_m'] #VpiLpi = 1.2 V.cm
    states = []
    terms = [
        i3.OpticalTerm(name='in'),
        i3.OpticalTerm(name='out'),
        i3.ElectricalTerm(name='elec1'),
        i3.ElectricalTerm(name='elec2'),
    ]

    def calculate_smatrix(parameters, env, S):
        pass  # reflexion

    def calculate_signals(parameters, env, output_signals, y, t, input_signals):
        v_diff = input_signals['elec2'] - input_signals['elec1']
        dneff = -(parameters.n_g - parameters.n_eff) / parameters.center_wavelength
        n_eff_final = parameters.n_eff + (env.wavelength - parameters.center_wavelength) * dneff
        length_m = parameters.length * 1e-6
        length_cm = parameters.length * 1e-4
        # 1e-4: from um to cm
        phase = 2 * pi * n_eff_final/ env.wavelength * parameters.length + pi * (v_diff * length_cm) / parameters.VpiLpi
        amp = 10**(-parameters.loss_dB_m * length_m/20.)
        output_signals['in'] = amp * exp(1j * phase) * input_signals['out']
        output_signals['out'] = amp * exp(1j * phase) * input_signals['in']


class MZMLumpedCompactModel(i3.CompactModel):
    """Lumped model for a MZM with
    - Insertion Loss
    - VpiLpi

    Only 'elec_left_2' (bottom arm) and 'elect_left_4' (top arm) are taken into account for the electrical signals !
    """

    parameters = [
        'n_eff',
        'delay',
        'length',
        'insertion_loss_dB',
        'VpiLpi',
    ]

    terms = [
        i3.OpticalTerm(name='in'),
        i3.OpticalTerm(name='out'),
    ]
    terms += [i3.ElectricalTerm(name='elec_left_{}'.format(_ + 1)) for _ in range(5)]
    terms += [i3.ElectricalTerm(name='elec_right_{}'.format(_ + 1)) for _ in range(5)]

    def calculate_smatrix(parameters, env, S):
        t = 10**(- parameters.insertion_loss_dB / 20.)
        delay_phase = parameters.n_eff * abs(parameters.delay) / env.wavelength
        S['in', 'out'] = S['out', 'in'] = t * exp(1j * delay_phase)

    def calculate_signals(parameters, env, output_signals, y, t, input_signals):
        t = 10**(- parameters.insertion_loss_dB / 20.)
        delay_phase = parameters.n_eff * abs(parameters.delay) / env.wavelength

        phase_arm1 = (
                2*pi * (delay_phase if (parameters.delay < 0) else 0)
                + pi * (input_signals['elec_left_2'] * parameters.length * 1e-4) / parameters.VpiLpi
        )

        phase_arm2 = (
                2*pi * (delay_phase if (parameters.delay > 0) else 0)
                + pi * (input_signals['elec_left_4'] * parameters.length * 1e-4) / parameters.VpiLpi
        )

        output_signals['out'] = t/2. * input_signals['in'] * (exp(1j * phase_arm1) + exp(1j * phase_arm2))
        output_signals['in'] = t/2. * input_signals['out'] * (exp(1j * phase_arm1) + exp(1j * phase_arm2))
