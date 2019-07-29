# Creation of a square with 2 properties length and width that use default values

from technologies import silicon_photonics
from ipkiss3 import all as i3


### Solution ###
class SquareWithDefaults(i3.PCell):
    length = i3.PositiveNumberProperty(default=5.0, doc="length")
    width = i3.PositiveNumberProperty(doc="width")

    def _default_width(self):
        return self.length
