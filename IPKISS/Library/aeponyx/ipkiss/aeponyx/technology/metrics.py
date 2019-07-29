# Basic Technology Metrics
#
# The technology keys below are required keys for Ipkiss
# You can change the values or refer them to your own technology keys, but their name should not be changed


#__all__ = ['TECH']

from ipkiss.technology import get_technology
from ipkiss.technology.technology import TechnologyTree

TECH = get_technology()


# Required for IPKISS
TECH.METRICS = TechnologyTree()

# Required parameters
TECH.METRICS.GRID = 1E-9          # Default manufacturing grid, in m
TECH.METRICS.UNIT = 1E-6          # Default user unit, in m
TECH.METRICS.ANGLE_STEP = 1.0     # Default angle step used in curve discretisation, in degrees
