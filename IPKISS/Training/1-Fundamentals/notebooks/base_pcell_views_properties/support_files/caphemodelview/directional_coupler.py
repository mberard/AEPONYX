from ipkiss3 import all as i3
import numpy as np

class DirectionalCouplerModel(i3.CompactModel):
    
    parameters = ['n_eff_1', 'n_eff_2', 'coupler_length']
    
    terms = [i3.OpticalTerm(name='in1'), 
             i3.OpticalTerm(name='out1'),
            i3.OpticalTerm(name='in2'),
            i3.OpticalTerm(name='out2')]
    

    def calculate_smatrix(parameters, env, S):

        delta_beta = 2 * pi / env.wavelength * (parameters.n_eff_1 - parameters.n_eff_2)
        beta_1 = 2 * pi / env.wavelength * (parameters.n_eff_1)
        straight_trans = 0.5 * np.exp(1j * parameters.coupler_length * beta_1) * (1.0 + np.exp(1j * delta_beta * parameters.coupler_length))
        cross_trans = 0.5 * np.exp(1j * parameters.coupler_length * beta_1) * (1.0 - np.exp(1j * delta_beta * parameters.coupler_length))
        
        
        S["in1", "out1"] =  S["out1", "in1"] =  S["in2", "out2"] =  S["out2", "in2"] = straight_trans
        S["in1", "out2"] =  S["out2", "in1"] =  S["in2", "out1"] =  S["out1", "in2"] = cross_trans
        S["in1", "in1"] =  S["out2", "out2"] =  S["in2", "in2"] =  S["out1", "out1"] = 0
        


class DirectionalCoupler(i3.PCell):
    """A directional coupler. We are only interested in simulation behavior 
    here, so we only define a Netlist and CapheModel.
    """
    # We define our netlist it is made of four ports.  
    class Netlist(i3.NetlistView):
        """
        Netlist view of the directional coupler.
        """
        def _generate_terms(self, terms):
            terms += i3.OpticalTerm(name="in1")
            terms += i3.OpticalTerm(name="in2")
            terms += i3.OpticalTerm(name="out1")
            terms += i3.OpticalTerm(name="out2")
            return terms

    class CircuitModel(i3.CircuitModelView):
        """
        CircuitModelView of the directional coupler.
        """
        n_eff_1 = i3.PositiveNumberProperty(default=3.0, doc="Effective index of the even supermode of the directional coupler.")
        n_eff_2 = i3.PositiveNumberProperty(default=2.99, doc="Effective index of the odd supermode of the directional coupler.")
        coupler_length = i3.PositiveNumberProperty(default=5.0, doc="length of the directional coupler")
        
        def _generate_model(self):
            
            return DirectionalCouplerModel(n_eff_1=self.n_eff_1,
                                            n_eff_2=self.n_eff_2,
                                            coupler_length=self.coupler_length)