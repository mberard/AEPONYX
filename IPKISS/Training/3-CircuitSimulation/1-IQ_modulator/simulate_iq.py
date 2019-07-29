# In this example, we will simulate in time domain the circuit designed in example_circuit_layout
if __name__ == "__main__":
    from aeponyx import technology

import ipkiss3.all as i3

import ipkiss3.all as i3
from picazzo3.routing.place_route.cell import ConnectComponents
import numpy as np
import pylab as plt


def create_prbs_function(bitrate, t_end, amplitude, noise_amplitude=0.0, seed=None):
    """Create a Pseudo Random Binary Sequence (PRBS) function f(t) with a given bitrate,
    end time, amplitude.
    """
    from numpy import random, ceil
    if seed is not None:
        np.random.seed(seed)
    size = int(ceil(t_end * bitrate))
    data = (random.randint(0, 2, size) - 0.5) * 2

    def f_prbs(t):
        idx = int(t * bitrate)
        if idx >= size:
            idx = size - 1

        return amplitude * data[idx] + np.random.rand() * noise_amplitude

    return f_prbs




def create_constant_function(amplitude=1, noise_amplitude=0.0, t_start=0.):
    def f_on(t):
        return amplitude * (t >= t_start) + np.random.rand() * noise_amplitude

    return f_on



def simulate_mod(cell, src_bitrate=20e9, mod_amp=1.0,ps_1=0.0, ps_2=0.0,  optical_noise_amp=0.0, dc_noise_amp=0.0, rf_noise_amp=0.0,
                 plot_signal=False, plot_constellation=True ):
    # Set time, time step and frequency
    t0 = 0
    dt = 1e-12
    t1 = 2e-9
# Define the function to create the sources' functions


    source_dict = dict()
    source_dict["in"] = create_constant_function(t_start=0., amplitude=mod_amp,noise_amplitude=optical_noise_amp) #1W
    source_dict["m1ps_p"] =create_constant_function(t_start=0., amplitude=ps_1, noise_amplitude=dc_noise_amp)
    source_dict["m2ps_p"] =create_constant_function(t_start=0., amplitude=ps_2,noise_amplitude=dc_noise_amp)
    source_dict["m11_p"] = create_prbs_function(bitrate=2*src_bitrate, t_end=t1,noise_amplitude=rf_noise_amp, amplitude=0.5*mod_amp, seed=45)
    source_dict["m12_p"] = create_prbs_function(bitrate=2*src_bitrate, t_end=t1,noise_amplitude=rf_noise_amp, amplitude=-0.5*mod_amp, seed=45)
    source_dict["m21_p"] = create_prbs_function(bitrate=2*src_bitrate, t_end=t1,noise_amplitude=rf_noise_amp, amplitude=0.5*mod_amp, seed=46)
    source_dict["m22_p"] = create_prbs_function(bitrate=2*src_bitrate, t_end=t1,noise_amplitude=rf_noise_amp, amplitude=-0.5*mod_amp, seed=46)

    probe_dict= dict()
    probe_dict["out"] = i3.Probe(port_domain=i3.OpticalDomain)


    childs = {"iq":cell}
    links =[]
    for key, val in source_dict.iteritems():
        if key == "in":
            source = i3.FunctionExcitation(port_domain=i3.OpticalDomain, excitation_function=val)
        else:
            source = i3.FunctionExcitation(port_domain=i3.ElectricalDomain, excitation_function=val)

        childs["src_{}".format(key)] = source
        p1 = "src_{}:out".format(key)
        p2 = "iq:{}".format(key)
        links.append((p1,p2))


    childs["prb_out"] = i3.Probe(port_domain=i3.OpticalDomain)
    p1 = "prb_out:in"
    p2 = "iq:out"
    links.append((p1, p2))




    # Connection of sources and probes to the circuit
    circuit = ConnectComponents(
        child_cells=childs,
        links=links
    )

    # Simulation
    cm = circuit.CircuitModel()
    print "simulating..."
    result = cm.get_time_response(t0=t0, t1=t1, dt=dt, center_wavelength=1.55)
    print "...done"
    # Retrieve data useful for the plot
    times = result.timesteps



    if plot_signal:
        plt.figure()

        plt.plot(times * 1e9, np.real(result["src_m11_p"]), 'k', label='input')
        plt.plot(times * 1e9, np.real(result["src_m21_p"]), 'b', label='quadrature_input')

        plt.legend()
        plt.ylabel("Voltage (V)", fontsize=14)
        plt.xlabel("Time (ns)", fontsize=14)
        plt.show()

    if plot_constellation:
        plt.figure()
        plt.scatter(np.real(result["prb_out"]), np.imag(result["prb_out"]), marker='+', alpha=0.1)
        plt.grid('on')
        plt.xlabel('real', fontsize=14)
        plt.ylabel('imag', fontsize=14)
        plt.title("Constellation diagram", fontsize=20)
        #plt.axis("equal")
        plt.xlim([-1.0, 1.0])
        plt.ylim([-1.0, 1.0])
        plt.show()
if __name__ == "__main__":
    from iq import IQModulator

    iq_cell = IQModulator()
    iq_lv = iq_cell.Layout()
    iq_lv.visualize(annotate=True)

    simulate_mod(cell=iq_cell, dc_noise_amp=0.05, rf_noise_amp=0.05, plot_signal=True)