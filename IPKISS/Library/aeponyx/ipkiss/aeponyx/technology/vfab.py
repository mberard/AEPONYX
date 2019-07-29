####################################################################
# VIRTUAL FABRICATION
####################################################################

from ipkiss.technology import get_technology
from ipkiss.technology.technology import TechnologyTree

TECH = get_technology()

# Final mask layers
TECH.PPLAYER.MSN.ALL = (TECH.PPLAYER.SHALLOW_TRENCH ^ TECH.PPLAYER.MSN) & TECH.PPLAYER.MSN
TECH.PPLAYER.MSN.ALL.name = "MSN_ALL"
TECH.PPLAYER.SHALLOW.ALL = TECH.PPLAYER.SHALLOW | (TECH.PPLAYER.SHALLOW_TRENCH & TECH.PPLAYER.MSN)
TECH.PPLAYER.SHALLOW.ALL.name = "SHALLOW_ALL"
TECH.PPLAYER.N.ALL = TECH.PPLAYER.N
TECH.PPLAYER.NPLUS.ALL = TECH.PPLAYER.NPLUS
TECH.PPLAYER.P.ALL = TECH.PPLAYER.P
TECH.PPLAYER.PPLUS.ALL = TECH.PPLAYER.PPLUS
TECH.PPLAYER.CON.ALL = TECH.PPLAYER.CON
TECH.PPLAYER.M1.ALL = TECH.PPLAYER.M1

# Virtual fabrication flow using Materials Stacks defined in technology/materials.py
# INVALID is used for the unsupported layers stacks

from ipkiss.plugins.vfabrication.process_flow import VFabricationProcessFlow

TECH.VFABRICATION = TechnologyTree()

# Process in the Silicon layer
TECH.VFABRICATION.PROCESS_FLOW_FEOL = VFabricationProcessFlow(active_processes = [ TECH.PROCESS.MSN, TECH.PROCESS.SHALLOW,
                                                                                   TECH.PROCESS.N, TECH.PROCESS.NPLUS,
                                                                                   TECH.PROCESS.P, TECH.PROCESS.PPLUS,
                                                                                   TECH.PROCESS.CON,
                                                                                   ], # DO NOT CHANGE THE SEQUENCE OF THE ELEMENTS ! IT MUST MATCH THE SEQUENCE OF THE COLUMNS IN VFABRICATION PROPERTY process_to_material_stack_map
                                                              process_to_material_stack_map =
                                                              #SI_ALL, SHALLOW_ALL, N, NPLUS, P, PPLUS, CON
                                                              [((0, 0, 0, 0, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_OX),
                                                               ((1, 0, 0, 0, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_MSN),
                                                               ((0, 1, 0, 0, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_RIB),
                                                               ((1, 1, 0, 0, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_MSN),
                                                               ((0, 0, 1, 0, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_RIB_N),
                                                               ((1, 0, 1, 0, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_N),
                                                               ((0, 1, 1, 0, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_RIB_N),
                                                               ((1, 1, 1, 0, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_N),
                                                               ((0, 0, 0, 1, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_NPLUS),
                                                               ((1, 0, 0, 1, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 0, 1, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_NPLUS),
                                                               ((1, 1, 0, 1, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 1, 1, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_NPLUS),
                                                               ((1, 0, 1, 1, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 1, 1, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_NPLUS),
                                                               ((1, 1, 1, 1, 0, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 0, 0, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_RIB_P),
                                                               ((1, 0, 0, 0, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_P),
                                                               ((0, 1, 0, 0, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_RIB_P),
                                                               ((1, 1, 0, 0, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_P),
                                                               ((0, 0, 1, 0, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_RIB_P),
                                                               ((1, 0, 1, 0, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_P),
                                                               ((0, 1, 1, 0, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_RIB_P),
                                                               ((1, 1, 1, 0, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_P),
                                                               ((0, 0, 0, 1, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 0, 1, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 0, 1, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 0, 1, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 1, 1, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 1, 1, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 1, 1, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 1, 1, 1, 0, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 0, 0, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_PPLUS),
                                                               ((1, 0, 0, 0, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 0, 0, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_PPLUS),
                                                               ((1, 1, 0, 0, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 1, 0, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 1, 0, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 1, 0, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 1, 0, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 0, 1, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 0, 1, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 0, 1, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 0, 1, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 1, 1, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 1, 1, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 1, 1, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 1, 1, 0, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 0, 0, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_PPLUS),
                                                               ((1, 0, 0, 0, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 0, 0, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_PPLUS),
                                                               ((1, 1, 0, 0, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 1, 0, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 1, 0, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 1, 0, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 1, 0, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 0, 1, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 0, 1, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 0, 1, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 0, 1, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 1, 1, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 1, 1, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 1, 1, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 1, 1, 1, 1, 0), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 0, 0, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 0, 0, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 0, 0, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 0, 0, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 1, 0, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 1, 0, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 1, 0, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 1, 0, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 0, 1, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_NPLUS_CON),
                                                               ((1, 0, 0, 1, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 0, 1, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_NPLUS_CON),
                                                               ((1, 1, 0, 1, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 1, 1, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_NPLUS_CON),
                                                               ((1, 0, 1, 1, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 1, 1, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_NPLUS_CON),
                                                               ((1, 1, 1, 1, 0, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 0, 0, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 0, 0, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 0, 0, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 0, 0, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 1, 0, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 1, 0, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 1, 0, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 1, 0, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 0, 1, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 0, 1, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 0, 1, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 0, 1, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 1, 1, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 1, 1, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 1, 1, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 1, 1, 1, 0, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 0, 0, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_PPLUS_CON),
                                                               ((1, 0, 0, 0, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 0, 0, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_PPLUS_CON),
                                                               ((1, 1, 0, 0, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 1, 0, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 1, 0, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 1, 0, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 1, 0, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 0, 1, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 0, 1, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 0, 1, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 0, 1, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 1, 1, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 1, 1, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 1, 1, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 1, 1, 0, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 0, 0, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_PPLUS_CON),
                                                               ((1, 0, 0, 0, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 0, 0, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_PPLUS_CON),
                                                               ((1, 1, 0, 0, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 1, 0, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_PPLUS_CON),
                                                               ((1, 0, 1, 0, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 1, 0, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_PPLUS_CON),
                                                               ((1, 1, 1, 0, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 0, 1, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 0, 1, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 0, 1, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 0, 1, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 0, 1, 1, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 0, 1, 1, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((0, 1, 1, 1, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ((1, 1, 1, 1, 1, 1, 1), TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID),
                                                               ],
                                                              is_lf_fabrication = {TECH.PROCESS.MSN      : False,
                                                                                   TECH.PROCESS.SHALLOW : False,
                                                                                   TECH.PROCESS.N       : False,
                                                                                   TECH.PROCESS.NPLUS   : False,
                                                                                   TECH.PROCESS.P       : False,
                                                                                   TECH.PROCESS.PPLUS   : False,
                                                                                   TECH.PROCESS.CON     : False,
                                                                                   }
                                                              )

# Process in metal layer (only M1)
TECH.VFABRICATION.PROCESS_FLOW_BEOL = VFabricationProcessFlow(active_processes = [ TECH.PROCESS.CON, TECH.PROCESS.M1], # DO NOT CHANGE THE SEQUENCE OF THE ELEMENTS ! IT MUST MATCH THE SEQUENCE OF THE COLUMNS IN VFABRICATION PROPERTY process_to_material_stack_map
                                                              process_to_material_stack_map =
                                                              #CON, M1
                                                              [((0, 0), TECH.MATERIAL_STACKS.MSTACK_METAL_DIEL),
                                                               ((1, 0), TECH.MATERIAL_STACKS.MSTACK_METAL_INVALID),
                                                               ((0, 1), TECH.MATERIAL_STACKS.MSTACK_METAL_M1),
                                                               ((1, 1), TECH.MATERIAL_STACKS.MSTACK_METAL_M1_CON),
                                                               ],
                                                              is_lf_fabrication = {TECH.PROCESS.M1 : False,
                                                                                   TECH.PROCESS.CON: False,
                                                                                   }
                                                              )


TECH.VFABRICATION.PROCESS_FLOW = TECH.VFABRICATION.PROCESS_FLOW_FEOL + TECH.VFABRICATION.PROCESS_FLOW_BEOL

TECH.PURPOSE.DOC.ignore_vfabrication = True
TECH.PURPOSE.BBOX.ignore_vfabrication = True
#TECH.PURPOSE.LABEL.ignore_vfabrication = True
TECH.PURPOSE.PINREC.ignore_vfabrication = True
#TECH.PURPOSE.DEVREC.ignore_vfabrication = True
TECH.PURPOSE.TEXT.ignore_vfabrication = True
TECH.PURPOSE.TRACE.ignore_vfabrication = True
