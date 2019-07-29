# Creation of a square with 2 properties length and width

from technologies import silicon_photonics
from ipkiss3 import all as i3


### Solution ###

class Square(i3.PCell):
    length = i3.PositiveNumberProperty(doc="length")
    width = i3.PositiveNumberProperty(doc="width")


my_square = Square(length=10.0, width=5.0)
