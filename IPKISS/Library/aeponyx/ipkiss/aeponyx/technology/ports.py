# Port settings

# The technology keys below are required keys for Ipkiss
# You can change the values or refer them to your own technology keys, but their name should not be changed

from ipkiss.technology import get_technology
from ipkiss.technology.technology import TechnologyTree

TECH = get_technology()

TECH.PORT = TechnologyTree()
TECH.PORT.DEFAULT_LAYER = TECH.PPLAYER.PINREC  # Default layer for drawing pins
TECH.PORT.DEFAULT_LENGTH = 0.1                 # Default length of a PIN
