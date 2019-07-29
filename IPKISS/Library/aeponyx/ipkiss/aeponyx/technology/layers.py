"""
Process layers, drawing purposes and process-purpose layer pairs for aeponyx.
"""


__all__ = ['TECH']

from ipkiss.technology.technology import ProcessTechnologyTree, TechnologyTree, DelayedInitTechnologyTree
from ipkiss.technology import get_technology
from ipkiss.process.layer import ProcessLayer, PatternPurpose, PPLayer

TECH = get_technology()

# ###############################################################
# TECH.PROCESS : Process Layers
#
# Process layers typically correspond to a single lithographic
# process step in the fabrication of your design
#
# ###############################################################

TECH.PROCESS = ProcessTechnologyTree()
TECH.PROCESS.MSN = ProcessLayer("Unetched MSN", "MSN") # waveguide layer (required)
TECH.PROCESS.MSN_TRENCH = ProcessLayer("Positive Etch MSN Exclusion", "MSN_TRENCH") # Positive tone waveguide layer
TECH.PROCESS.SHALLOW = ProcessLayer("Fiber Couplers and Rib Waveguide (shallow etched MSN)", "SHALLOW")

TECH.PROCESS.CLADDING = ProcessLayer("Unetched Cladding", "CLAD") #waveguide cladding

TECH.PROCESS.HT = ProcessLayer("Heater", "HT")
TECH.PROCESS.M1 = ProcessLayer("Metallization 1", "M1")
TECH.PROCESS.M2 = ProcessLayer("Metallization 2", "M2")
TECH.PROCESS.V12 = ProcessLayer("Via between M1 and M2", "V12")
TECH.PROCESS.SIL = ProcessLayer("Silicide", "SIL")
TECH.PROCESS.N = ProcessLayer("N+ implant", "N")
TECH.PROCESS.P = ProcessLayer("P+ implant", "P")
TECH.PROCESS.GE = ProcessLayer("Germanium", "GE")
TECH.PROCESS.NPLUS = ProcessLayer("N++ implant", "NPLUS")
TECH.PROCESS.PPLUS = ProcessLayer("P++ implant", "PPLUS")
TECH.PROCESS.CON = ProcessLayer("Contact", "CON")

TECH.PROCESS.NONE = ProcessLayer("No specific process", "NONE")


# The following keys are required by ipkiss / picazzo but not used actively by aeponyx
TECH.PROCESS.WG = TECH.PROCESS.MSN                      # Required for ipkiss, picazzo
TECH.PROCESS.RWG = TECH.PROCESS.SHALLOW                 # Required by picazzo
TECH.PROCESS.FC = TECH.PROCESS.SHALLOW                  # Required by picazzo (grating couplers)

# ###############################################################
# TECH.PURPOSE: Layer Purposes
#
# Layer Purposes indicate what the meaning is of a pattern on a
# given process layer. e.g. a Line, a documentation, ...
#
# ###############################################################

TECH.PURPOSE = TechnologyTree()

TECH.PURPOSE.DRAWING = PatternPurpose(name="Drawing", extension="DRW")
TECH.PURPOSE.DRWSUB = PatternPurpose(name="Subtracted drawing", extension="DRWSUB")

TECH.PURPOSE.BBOX = PatternPurpose(name="Bounding Box", extension="BBOX")
TECH.PURPOSE.ERROR = PatternPurpose(name="Error", extension="ERR", doc="Errors")
TECH.PURPOSE.PINREC = PatternPurpose(name="Pin recognition", extension="PIN", doc="Pin marker for extraction")
TECH.PURPOSE.TRACE = PatternPurpose(name="Trace", extension="TRC", doc="Control shape of trace")
TECH.PURPOSE.TEXT = PatternPurpose(name="Text", extension="TXT")

TECH.PURPOSE.DOC = PatternPurpose(name="Documentation", extension="DOC")
TECH.PURPOSE.UNUSED = PatternPurpose(name="Unused", extension="UNUSED")

# Required by ipkiss / picazzo
TECH.PURPOSE.DF_AREA = TECH.PURPOSE.UNUSED                                           # Required by picazzo
TECH.PURPOSE.LF_AREA = PatternPurpose(name="Light-field area", extension="LFAREA")   # Required by ipkiss
TECH.PURPOSE.LF = TechnologyTree()
TECH.PURPOSE.LF.LINE = PatternPurpose(name="Light-field line", extension="LFLINE")   # Required by ipkiss
TECH.PURPOSE.DF = TechnologyTree()
TECH.PURPOSE.DF.LINE = PatternPurpose(name="Dark-field line", extension="DFLINE")    # Required by ipkiss

# ###############################################################
# TECH.PPLAYER: Process-Purpose Layer pairs
#
# These are the predefined layer pairs of a Process and a Purpose
# that are allowed in your technology.
#
# ###############################################################

TECH.PPLAYER = TechnologyTree()  # Tree for PPLayers - required
TECH.PPLAYER.MSN = PPLayer(TECH.PROCESS.MSN, TECH.PURPOSE.DRAWING, name="MSN")
TECH.PPLAYER.MSN_TRENCH = PPLayer(TECH.PROCESS.MSN_TRENCH, TECH.PURPOSE.DRAWING, name="MSN_TRENCH")

TECH.PPLAYER.SHALLOW = PPLayer(TECH.PROCESS.SHALLOW, TECH.PURPOSE.DRAWING, name="SHALLOW")
TECH.PPLAYER.SHALLOW_TRENCH = PPLayer(TECH.PROCESS.SHALLOW, TECH.PURPOSE.DRWSUB, name="SHALLOW_TRENCH")

TECH.PPLAYER.CLADDING = PPLayer(TECH.PROCESS.CLADDING, TECH.PURPOSE.DRAWING, name="CLADDING")

TECH.PPLAYER.GE = PPLayer(TECH.PROCESS.GE, TECH.PURPOSE.DRAWING, name="GE")

TECH.PPLAYER.HT = PPLayer(TECH.PROCESS.HT, TECH.PURPOSE.DRAWING, name="HT")
TECH.PPLAYER.M1 = PPLayer(TECH.PROCESS.M1, TECH.PURPOSE.DRAWING, name="M1")
TECH.PPLAYER.M2 = PPLayer(TECH.PROCESS.M2, TECH.PURPOSE.DRAWING, name="M2")
TECH.PPLAYER.V12 = PPLayer(TECH.PROCESS.V12, TECH.PURPOSE.DRAWING, name="V12")

TECH.PPLAYER.N = PPLayer(TECH.PROCESS.N, TECH.PURPOSE.DRAWING, name="N_DRW")
TECH.PPLAYER.NPLUS = PPLayer(TECH.PROCESS.NPLUS, TECH.PURPOSE.DRAWING, name="NPLUS_DRW")
TECH.PPLAYER.P = PPLayer(TECH.PROCESS.P, TECH.PURPOSE.DRAWING, name="P_DRW")
TECH.PPLAYER.PPLUS = PPLayer(TECH.PROCESS.PPLUS, TECH.PURPOSE.DRAWING, name="PPLUS_DRW")

TECH.PPLAYER.PINREC = PPLayer(TECH.PROCESS.NONE, TECH.PURPOSE.PINREC, name="PINREC")    # Used in ports
TECH.PPLAYER.NONE = PPLayer(TECH.PROCESS.NONE, TECH.PURPOSE.UNUSED, name="NONE")        # TODO: Update aeponyx components to not use this layer

TECH.PPLAYER.ERROR = TechnologyTree()                                                 # Error indication, for example used in PlaceAndAutoRoute to show where waveguides cross unintentionally
TECH.PPLAYER.ERROR.GENERIC = PPLayer(TECH.PROCESS.NONE, TECH.PURPOSE.ERROR, name="ERROR")  # Generic Error indication
TECH.PPLAYER.ERROR.CROSSING = TECH.PPLAYER.ERROR.GENERIC
TECH.PPLAYER.SIL = PPLayer(TECH.PROCESS.SIL, TECH.PURPOSE.DRAWING, name="SIL_DRW")      # Silicide layer
TECH.PPLAYER.CON = PPLayer(TECH.PROCESS.CON, TECH.PURPOSE.DRAWING, name="CON_DRW")      # Contact layer

TECH.PPLAYER.DOC = PPLayer(TECH.PROCESS.NONE, TECH.PURPOSE.DOC, name="DOC")            # Design documentation

# Routing layers (for routing in EDA tools)
TECH.PPLAYER.WIRE_TRACE = PPLayer(TECH.PROCESS.MSN, TECH.PURPOSE.TRACE, name="WIRE_TRC")
TECH.PPLAYER.RIB_TRACE = PPLayer(TECH.PROCESS.SHALLOW, TECH.PURPOSE.TRACE, name="RIB_TRC")

# Required by ipkiss/picazzo
TECH.PPLAYER.M1.LINE = TECH.PPLAYER.M1                                                 # Required by ipkiss
TECH.PPLAYER.M2.LINE = TECH.PPLAYER.M2                                                 # Required by ipkiss
TECH.PPLAYER.V12.PILLAR = TECH.PPLAYER.V12                                             # Required by ipkiss
TECH.PPLAYER.WG = TechnologyTree()
TECH.PPLAYER.WG.TEXT = TECH.PPLAYER.MSN                                                # Required by ipkiss
