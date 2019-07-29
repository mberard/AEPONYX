# aeponyx models

The aeponyx contains the following circuit models:

- waveguide parabolic crossing
- periodic curved fiber grating coupler
- multi mode interferometer
- photodetector
- absorber

## Waveguide parabolic crossing

**Model name:** CROSSING\_450

**Description:** Highly efficient crossing for high-contrast silicon photonics. The crossing consists of double-etched parabolic transitions at right angles, collimating the optical field over a short distance to a parallel beam at the center of the crossing. This allows for a minimum of crosstalk. 

**Parameters:**

- The crossing loss is defined by the parameter *insertion\_loss\_dB*
- The cross-talk is defined by the parameter *crosstalk\_dB*
- The phase propagation for the straight section (east to west and  north to south) is defined by the *length* the crossing and the parameter *n_eff* (the effective index).

**References:** The details of this crossing are described in detail in "W. Bogaerts et al., Optics Letters, 32(19), p.2801-2803 (2007)"

## Curved fiber grating coupler

**Model name:** FC\_TE\_1550

**Description:** Periodic curved fiber grating coupler
**Parameters:** The grating coupler transmission is a gaussian curve with a center wavelength defined by the parameter *center\_wavelength* and with a 1 dB FWHM defined by the parameter *bandwidth\_1dB*

## Multi Mode Interferometer(s)

**Model name(s):**  MMI\_12, MMI\_22

**Description:** A multimode interferometer with variable width and length of the taper.

**Parameters:** 

- The excess loss is defined by the parameter *excess\_loss\_dB*
- The power imbalance is defined by the parameter *power\_imbalance\_dB*
- The reflection is defined by the parameter *reflection\_dB*

- **Model MMI\_12**
Interferometer with one input and two outputs.
The splitting ratio is by default 50-50, but can be modified by adjusting the parameter *power\_imbalance\_dB*

- **Model MMI\_22**
Interferometer with two inputs and two outputs. There is a pi/2 phase difference between the cross and bar output.

**References**:
[MMI model](https://era.library.ualberta.ca/files/z316q3680/NQ23053.pdf)

## Photodetector

**Model name:** PhotoDiode

**Description:** Basic photodetector

**Parameters:**

- The default responsitivity value  (parameter *R*) is 0.8 A/W
- The default 3 dB bandwidth (parameter *BW*) value of the photodetector is 60 GHz
- The default reflection value (parameter *reflection\_dB*) is 100.0 dB
- The default dark current value (parameter *dark\_current*) is 50 nA

## Absorber

**Model name:** ABSORBER

**Description:** Sample absorber, based on a GDS file.
**Parameters:** The absorber reflection is defined by the parameter *reflection*
