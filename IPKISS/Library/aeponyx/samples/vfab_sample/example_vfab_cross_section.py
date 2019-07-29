"""
Example to show the virtual fabrication and a  cross-section of a Mach-Zehnder Modulator

See http://docs.lucedaphotonics.com/3.1/guides/technology/index.html?#virtual-fabrication for more information about the Virtual Fabrication
"""

import aeponyx.all as demo

import ipkiss3.all as i3
import numpy as np

n_eff = demo.SWG450().CircuitModel().n_eff
wavelength = 1.55
d_phi = np.pi/2.
delay_length = d_phi * wavelength / (2 * np.pi * n_eff)

mzm = demo.TW_MZM()
lay = mzm.Layout(delta=50., additional_length=150., delay=delay_length)

fig = lay.visualize(annotate=True, show=False)
fig.canvas.set_window_title('Modulator layout')


fig = lay.visualize_2d(show=False)
fig.canvas.set_window_title('Virtual fabrication of the modulator')
ax = fig.get_axes()[0]
ax.vlines(0, -75, 75)
ax.annotate('Cross-section', xy=(0, 0), xytext=(100, 100),
            arrowprops=dict(facecolor='black', shrink=0.05))

xs = lay.cross_section(cross_section_path=i3.Shape([(0., -75.), (0., 75.)]), path_origin=-75.)
fig = xs.visualize(show=False)
fig.canvas.set_window_title('Cross-section from y=-75 --> y=75 um.')

xs = lay.cross_section(cross_section_path=i3.Shape([(0., 36.), (0., 60.)]), path_origin=36.)
fig = xs.visualize(show=False)
fig.canvas.set_window_title('Cross-section from y=36 --> y=60 um.')

from pylab import plt
plt.show()
