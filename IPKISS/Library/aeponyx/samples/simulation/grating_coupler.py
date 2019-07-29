# shows the transmission of a single grating coupler
import aeponyx.all as demo
import ipkiss3.all as i3
import numpy as np
import pylab as plt

fc = demo.FC_TE_1550()
fc_lo = fc.Layout()
fc_cm = fc.CircuitModel()

fc_lo.visualize()

wavelengths = np.linspace(1.54, 1.56, 1000)
S = fc_cm.get_smatrix(wavelengths=wavelengths)
plt.figure()
plt.subplot(121)
plt.plot(wavelengths, 10 * np.log10(np.abs(S["vertical_in", "out"])**2), 'rx-')
plt.xlabel("Wavelengths ($\mu$m)")
plt.ylabel("Transmission (dB)")
plt.title('power transmission')
plt.xlim([wavelengths[0], wavelengths[-1]])
plt.subplot(122)
plt.plot(wavelengths, np.angle(S["vertical_in", "out"]), 'bx-')
plt.xlabel("Wavelengths ($\mu$m)")
plt.ylabel("Phase")
plt.title('phase transmission')
plt.xlim([wavelengths[0], wavelengths[-1]])
plt.show()
