from .technology import TECH
from .components.absorber import ABSORBER
from .components.metal.wire.trace import M1WireTemplate
from .components.metal.contact import CONTACT_STD
from .components.metal.bondpad import BondPad, BONDPAD_5050
from .components.metal.via import VIA_M1_M2
from .components.grating_couplers.cell import FC_TE_1550
from .components.modulator.traces import PhaseShifterTemplate, PhaseShifterWaveguide
from .components.modulator.cell import TW_MZM
from .components.waveguides.wire.trace import MSNWireWaveguideTemplate, SWG450, SWG550, SWG1000, RoundedWireWaveguide
from .components.waveguides.rib.trace import MSNRibWaveguideTemplate, RWG450, RWG850, RoundedRibWaveguide
from .components.waveguides.wire.transition import LinearWireTransition, LinearWireTransitionFromPort
from .components.waveguides.rib.transition import LinearWireRibTransition, LinearWireRibTransitionFromPort
from .components.crossing import WireWgCrossing, CROSSING_450
from .components.mmi import MMI, MMI_12, MMI_22
from .components.photodetector.cell import PhotoDiode
from picazzo3.traces.twoshape import CoreCladdingShapeWaveguide
from picazzo3.wg.connector import RoundedWaveguideConnector

# Dependencies for the grating couplers
from picazzo3.fibcoup.socket import LinearTransitionSocket
from picazzo3.fibcoup.curved.curved_basic import CurvedGrating

# Dependencies for the Modulator
from picazzo3.wg.splitters import WgY90Splitter
from picazzo3.wg.dircoup import StraightDirectionalCoupler
from ipkiss3.pcell.photonics.rounded_waveguide import RoundedWaveguide
from ipkiss3.pcell.photonics.waveguide import Waveguide
from picazzo3.filters.mzi import MZIContainerArm
from picazzo3.modulators.phase.trace import TemplatedPhaseShifterWaveguide, LateralBridge
from ipkiss3.pcell.trace.transitions.auto_transition.auto_trace_transition import AutoTraceTransitionFromPort

# In __all__, we store all accessible components
__all__ = ['ABSORBER',
           'CONTACT_STD',
           'BondPad', 'BONDPAD_5050', 'VIA_M1_M2',
           'FC_TE_1550',
           'PhotoDiode',
           'PhaseShifterTemplate', 'PhaseShifterWaveguide', 'TW_MZM',
           'MSNWireWaveguideTemplate', 'SWG450', 'SWG550', 'SWG1000', 'RoundedWireWaveguide', 'MSNWireWaveguide',
           'MSNRibWaveguideTemplate', 'RWG450', 'RWG850', 'RoundedRibWaveguide',
           'LinearWireTransition', 'LinearWireTransitionFromPort', 'LinearWireRibTransition', 'LinearWireRibTransitionFromPort',
           'CoreCladdingShapeWaveguide',
           'LinearTransitionSocket', 'CurvedGrating',
           'WgY90Splitter', 'StraightDirectionalCoupler', 'RoundedWaveguide', 'Waveguide', 'MZIContainerArm', 'TemplatedPhaseShifterWaveguide', 'LateralBridge',
           'WireWgCrossing', 'CROSSING_450',
           'MMI', 'MMI_12', 'MMI_22',
           'RoundedWaveguideConnector', 'AutoTraceTransitionFromPort',
           ]

templates = ['SWG450', 'SWG550', 'SWG1000', 'RWG450', 'RWG850']
