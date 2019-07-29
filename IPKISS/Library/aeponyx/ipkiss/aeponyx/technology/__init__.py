# ###############################################################
# aeponyx: Demonstration library for Photonics Design
#
# copyright Luceda Photonics 2016
#
# ###############################################################
# Technology data
# ###############################################################
#
# The following imports load the different parts of the
# Technology tree
#

# INIT file
# Sets the technology name and loads all modules
# Be careful when changing the order of importing modules, since there may be interdependencies and risk for circular imports
# through ipkiss and/or picazzo

from ipkiss.technology import TECHNOLOGY as TECH  # Import the base technology from IPKISS.

__all__ = ["TECH"]

TECH.name = "AEPONYXTECH"

from technologies.base_ipkiss import admin as _           # Admin: auto-name generator, ...
from technologies.base_ipkiss import mask as _            # Constants for mask polarity
from . import metrics as _                                # Metrics: unit, grid, ...
from . import layers as _                                 # Process layers and drawing purposes

from technologies.base_ipkiss import pcells as _          # Initializes the PCELL tree to add PCELLS to the TECH.
from . import ports as _                                  # Ports: Settings about ports sizes in L-EDIT.
from technologies.base_ipkiss import blocks as _          # Blocks: Sets the block spacing for IOColumn.
# from technologies.base_ipkiss import io as _             # Adaptors: Set default adaptors for IOColumn.
from . import rules as _                                  # Constants for various design parameters
from . import metals_via as _                             # Metal_via : Sets the layers and properties to handle metals and via's
from . import traces as _                                 # traces: Sets the rules that handles the drawing of waveguides and traces.
from . import gdsii as _                                  # Gdsii: Sets the GDSII import-export layers.
from . import display as _                                # Display: Display settings for visualisation in IPKISS.flow or L-EDIT.
from . import openaccess as _                             # All OpenAccess-related settings.
from . import picazzo as _                                # Required TECH keys for picazzo
from . import fibcoup as _                                # Input-output: fiber couplers, iocolumns
from . import container as _                              # For terminations
from . import modulators as _                             # Modulator standard dimensions and offsets
from . import pcells as _                                 # All PCell that are in the TECH-tree
from . import materials as _                              # Materials and material stacks

from . import vfab as _                                   # Virtual fabrication

from ..components.waveguides.wire import transition as _  # Load the transitions in the AUTOTRANSITIONDATABASE
from ..components.waveguides.rib import transition as _   # Load the transitions in the AUTOTRANSITIONDATABASE
