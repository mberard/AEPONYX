from ipkiss3 import all as i3
from picazzo3.filters.ring import RingRect180DropFilter
import numpy as np

class SimpleRing(RingRect180DropFilter):
    
    resonance_wavelength = i3.PositiveNumberProperty(default=1.55)
    length = i3.PositiveNumberProperty()
    order = i3.PositiveIntProperty(default=200)
    
    def _get_neff_at_res(self):
        tt=self.coupler_trace_templates[0]
        cm = tt.get_default_view(i3.CircuitModelView)
        # Get the n_eff and n_g from the circuit model
        n_eff = cm.n_eff
        n_g = cm.n_g
        cw = cm.center_wavelength
        dneff = -(n_g - n_eff) / cw
        dl = self.resonance_wavelength - cw
        n_res = n_eff + dneff*(dl) 
        return n_res
    
    def _default_length(self):
        length = self.order * self.resonance_wavelength / self._get_neff_at_res() 
        return length
  
    
    class Layout(RingRect180DropFilter.Layout):
        
        def _default_straights(self):
            return [0.0, 0.0]
        
        
        def _default_bend_radius(self):
            return self.length / np.pi / 2       
        
    class CircuitModel(RingRect180DropFilter.CircuitModel):
        
        def _default_coupler_parameters(self):
            tau = 0.9**0.5
            kappa = 1j * (1 - tau**2)**0.5
            cp = dict(cross_coupling1=kappa,
                      straight_coupling1=tau)
            
            return [cp,cp]
        
        def _default_ring_length(self):
            
            return self.length
        
        
    