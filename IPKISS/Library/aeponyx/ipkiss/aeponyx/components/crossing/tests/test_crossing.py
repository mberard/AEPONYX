from pdkbuilding.testing import LayoutNoFilterGDSReferenceTest, LayoutXMLReferenceTest, CapheModelTest
import pytest

class TestCrossing(LayoutNoFilterGDSReferenceTest,
                  LayoutXMLReferenceTest,
                  CapheModelTest):

    @pytest.fixture
    def component(self):
        # Create and return a basic crossing
        import ipkiss3
        from aeponyx.all import CROSSING_450
        my_crossing = CROSSING_450()
        my_crossing.Layout()
        return my_crossing


    @pytest.fixture
    def wavelengths(self):
        import numpy as np
        return np.arange(1.3, 1.8, 0.0001)

    # You can specify your own tests in this method:
    @pytest.mark.xfail(reason = "pdkbuilding does not support new models yet")
    def test_Crossing_Smatrix(self, smatrix):
        from pdkbuilding.testing import SMatrixTester
        # Create a tester object
        tester = SMatrixTester(smatrix)

        # Perform general tests
        assert tester.is_reciprocal(), "Component is not reciprocal"
        assert tester.is_passive(), "Component is active"
