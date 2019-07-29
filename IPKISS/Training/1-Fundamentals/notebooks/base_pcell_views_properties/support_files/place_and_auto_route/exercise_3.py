from technologies import silicon_photonics
from ipkiss3 import all as i3
import numpy as np
import pylab as plt

from picazzo3.traces.wire_wg import WireWaveguideTemplate
from picazzo3.fibcoup.curved import FiberCouplerCurvedGrating
from picazzo3.apertures.basic import WireWgAperture
from support_files.mmi import SimpleMMI
from support_files.ring import SimpleRing
from picazzo3.routing.place_route import PlaceAndAutoRoute


# #######################################
# Rings
# #######################################

spacing = (200*1e9 * (1.55*1e-6)**2 / 3e8)*1e6
print("Spacing: {} nm".format(spacing*1e3))
res_w = 1.550 + spacing * np.asarray([0, 1, 2, 3])

rings = []
r_layouts = []
for nr, l in enumerate(res_w):
    ring=SimpleRing(resonance_wavelength=l)
    rings.append(ring)
    rl = ring.Layout()
    r_layouts.append(rl)
    print rl.ports["out1"].position
    
# #######################################
# Grating Coupler
# #######################################    

grating_coupler = FiberCouplerCurvedGrating()
grating_coupler_layout = grating_coupler.Layout()
grating_cm = grating_coupler.CircuitModel(center_wavelength=1.55, bandwidth_3dB=0.06, peak_transmission=0.60**0.5, reflection=0.01**0.5)

# #######################################
# Splitter
# #######################################    

splitter = SimpleMMI()
splitter_layout = splitter.Layout(length=14.1428571429, waveguide_spacing=2.1)
splitter_cm = splitter.CircuitModel(straight_coupling1=0.45**0.5)

# #######################################
# Termination
# #######################################    

wt = WireWaveguideTemplate()
wt.Layout(core_width = 0.9,
          cladding_width = 2 * i3.TECH.WG.WIRE_WIDTH + 0.9)
wwa = WireWgAperture(aperture_trace_template = wt)
wwa_lay = wwa.Layout()
wwa_cm = wwa.CircuitModel(reflection=0.001)


# #######################################
# Simple circuit
# #######################################

cells = {"gc_in"     : grating_coupler,
         "gc_ref"    : grating_coupler,
         "gc_pass"   : grating_coupler,
         "splitter"  : splitter
         }

# Update the cells dict with all rings, grating couplers, and terminations
for cnt, r in enumerate(rings, 1):
    cells["ring{}".format(cnt)] = rings[cnt - 1]
    cells["gc_out{}".format(cnt)] = grating_coupler
    cells["term_out{}".format(cnt)] = wwa
 

links = [# input coupler to splitter
         ("gc_in:out",      "splitter:in"), 
         
         # reference output to splitter output
         ("gc_ref:out",     "splitter:out1"),   
         
         # connecting the rings together
         ("ring1:in1",      "splitter:out2"),
         ("ring1:out1",     "ring2:in1"),
         ("ring2:out1",     "ring3:in1"),
         ("ring3:out1",     "ring4:in1"),
         ("gc_pass:out",    "ring4:out1")]
         
# Update the links: add (ring <--> gc_out) and (ring <--> term_out) links.
for cnt, r in enumerate(rings, 1):
    links.append(("ring{}:out2".format(cnt), "gc_out{}:out".format(cnt)))
    links.append(("ring{}:in2".format(cnt), "term_out{}:in".format(cnt)))
  
         
d = 100.0
transformations = dict()
transformations['gc_in'] = i3.vector_match_transform(grating_coupler_layout.ports["out"], i3.Vector(position=(0,0), angle=180.0))
transformations['splitter'] = i3.vector_match_transform(splitter_layout.ports["in"], i3.Vector(position=(d/2,0), angle=0.0))

for cnt, r in enumerate(rings, 1):
    transformations["ring{}".format(cnt)] = i3.vector_match_transform(r_layouts[cnt-1].ports["in1"], i3.Vector(position=(cnt*d,d/2), angle=0.0))
    transformations["gc_out{}".format(cnt)] = i3.vector_match_transform(grating_coupler_layout.ports["out"], i3.Vector(position=(cnt*d-10,d), angle=90.0))
    transformations["term_out{}".format(cnt)] = i3.Rotation(rotation=90.0,rotation_center=wwa_lay.ports["in"]) + \
        i3.vector_match_transform(wwa_lay.ports["in"],
        r_layouts[cnt-1].ports["in2"].transform_copy(transformations["ring{}".format(cnt)])) + \
        i3.Translation(translation=(10.0,10.0))



transformations['gc_ref'] = i3.vector_match_transform(grating_coupler_layout.ports["out"], i3.Vector(position=(5*d,-d/2), angle=0.0))
transformations['gc_pass'] = i3.vector_match_transform(grating_coupler_layout.ports["out"], i3.Vector(position=(5*d,d/2), angle=0.0))
             

# Create the PCell
my_circuit = PlaceAndAutoRoute(child_cells=cells,
                               links=links)

# Create Layout
layout = my_circuit.Layout(child_transformations=transformations)
layout.visualize(annotate=True)
layout.write_gdsii("full_circuit.gds")

# If the you pick resonance frequencies that lead to a off-grid ports it is safest to flatten your entire design.
top_cell_layout_flat = layout.layout.flat_copy()
top_layout_flat = i3.LayoutCell().Layout(elements=top_cell_layout_flat)
top_layout_flat.write_gdsii("full_circuit_flat.gds")


# Simulate
cm = my_circuit.CircuitModel()
wavelengths =  np.linspace(1.52, 1.58, 4001)
S = cm.get_smatrix(wavelengths=wavelengths)

plt.figure()
plt.plot(wavelengths, 20 * np.log10(np.abs(S['gc_in_vertical_in', 'gc_pass_vertical_in'])), label='pass')
plt.plot(wavelengths, 20 * np.log10(np.abs(S['gc_in_vertical_in', 'gc_out1_vertical_in'])), label='out1')
plt.plot(wavelengths, 20 * np.log10(np.abs(S['gc_in_vertical_in', 'gc_out2_vertical_in'])), label='out2')
plt.plot(wavelengths, 20 * np.log10(np.abs(S['gc_in_vertical_in', 'gc_out3_vertical_in'])), label='out3')
plt.plot(wavelengths, 20 * np.log10(np.abs(S['gc_in_vertical_in', 'gc_out4_vertical_in'])), label='out4')
plt.plot(wavelengths, 20 * np.log10(np.abs(S['gc_in_vertical_in', 'gc_ref_vertical_in'])), label='ref')
plt.title("Circuit transmission (power)")
plt.xlabel("Wavelength ($\mu m$)")
plt.ylabel("Transmission [dB]")
plt.ylim([-80,0])
plt.xlim([1.52,1.60])
for w in res_w:
    plt.axvline(x=w)
plt.legend()
plt.show()


