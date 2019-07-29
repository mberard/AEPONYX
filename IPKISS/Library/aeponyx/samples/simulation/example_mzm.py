"""
In this example, we will simulate a Mach-Zehnder Modulator in time-domain.
The MZM model has no time dependency, no bandwidth parameter.
"""

import aeponyx.all as demo

import ipkiss3.all as i3
from picazzo3.routing.place_route.cell import ConnectComponents
import numpy as np
import pylab as plt


cell = demo.TW_MZM(modulator_length=1000.)

n_eff = demo.SWG450().CircuitModel().n_eff
wavelength = 1.55
d_phi = np.pi # phase difference between the two arms
delay_length = d_phi * wavelength / (2 * np.pi * n_eff) # converted in delay length


lay = cell.Layout(delta=50., additional_length=150., delay=delay_length)
lay.visualize(annotate=True)

# Set time, time step and frequency
t0 = 0
dt = 0.02e-11
t1 = 0.5e-7
src_bitrate = 2e8
V = 1.5 # default VpiLpi of TW_MZM is 1.5 V.cm

# Define the function to create the sources' functions
def create_prbs_function(bitrate, t_end, amplitude, seed=None):
    """Create a Pseudo Random Binary Sequence (PRBS) function f(t) with a given bitrate,
    end time, amplitude.
    """
    from numpy import random, ceil
    if seed is not None:
        np.random.seed(seed)
    size = int(ceil(t_end * bitrate))
    data = random.randint(0, 2, size)

    def f_prbs(t):
        idx = int(t * bitrate)
        if idx >= size:
            idx = size - 1

        return amplitude * data[idx]

    return f_prbs


def create_constant_function(amplitude=1., t_start=0.):
    def f_on(t):
        return amplitude if t >= t_start else 0

    return f_on


# Sources' function
f_opt = create_constant_function(t_start=0., amplitude=np.sqrt(10e-3)) #10mW
f_volt_arm1 = create_prbs_function(bitrate=2*src_bitrate, t_end=t1, amplitude=-V, seed=45)
f_volt_arm2 = create_prbs_function(bitrate=2*src_bitrate, t_end=t1, amplitude=V, seed=45)

# Sources (Optical and Electrical)
src_opt = i3.FunctionExcitation(port_domain=i3.OpticalDomain, excitation_function=f_opt)
src_volt_arm1 = i3.FunctionExcitation(port_domain=i3.ElectricalDomain, excitation_function=f_volt_arm2)
src_volt_arm2 = i3.FunctionExcitation(port_domain=i3.ElectricalDomain, excitation_function=f_volt_arm1)

# Connection of sources and probes to the circuit
circuit = ConnectComponents(
    child_cells={
        'mzi': cell,
        'src_opt': src_opt,
        'src_arm1': src_volt_arm1,
        'src_arm2': src_volt_arm2,
        'opt_out': i3.Probe(port_domain=i3.OpticalDomain),
    },
    links=[
        ('mzi:in', 'src_opt:out'),
        ('mzi:elec_left_2', 'src_arm1:out'),
        ('mzi:elec_left_4', 'src_arm2:out'),
        ('mzi:out', 'opt_out:in'),
    ]
)

# Simulation
cm = circuit.CircuitModel()
result = cm.get_time_response(t0=t0, t1=t1, dt=dt, center_wavelength=wavelength)

# Retrieve data useful for the plot
times = result.timesteps

# Plot
plt.plot(times * 1e9, np.abs(result['opt_out'])**2*1e3, label='Optical output power [mW]')
plt.xlabel("Time (ns)")
plt.title('Transient response for a {} GHz PRBS ideal signal'.format(src_bitrate/1e9))
plt.legend()
plt.xlim([times[0]*1e9, times[-1]*1e9])
plt.show()
