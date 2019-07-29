from pdkbuilding.testing import LayoutNoFilterGDSReferenceTest, LayoutXMLReferenceTest
import pytest

class TestPhotoDetector(LayoutNoFilterGDSReferenceTest,
                  LayoutXMLReferenceTest):

    @pytest.fixture
    def component(self):
        # Create and return a basic photodetector
        import ipkiss3
        from aeponyx.all import PhotoDiode
        my_photodiode = PhotoDiode()
        my_photodiode.Layout()
        return my_photodiode

    @pytest.fixture
    def wavelengths(selfself):
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