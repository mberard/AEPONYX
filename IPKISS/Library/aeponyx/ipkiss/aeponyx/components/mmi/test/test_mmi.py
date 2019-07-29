from pdkbuilding.testing import LayoutNoFilterGDSReferenceTest, LayoutXMLReferenceTest, CapheModelTest
import pytest

class TestMMI12(LayoutNoFilterGDSReferenceTest,
                  LayoutXMLReferenceTest,
                  CapheModelTest):

    @pytest.fixture
    def component(self):
        # Create and return a basic MMI_12
        import ipkiss3
        from aeponyx.all import MMI_12
        my_mmi = MMI_12()
        my_mmi.Layout()
        return my_mmi

class TestMMI22(LayoutNoFilterGDSReferenceTest,
                    LayoutXMLReferenceTest,
                    CapheModelTest):

    @pytest.fixture
    def component(self):
        # Create and return a basic MMI_22
        from aeponyx.all import MMI_22
        my_mmi = MMI_22()
        my_mmi.Layout()
        return my_mmi


    @pytest.fixture
    def wavelengths(self):
        import numpy as np
        return np.arange(1.3, 1.8, 0.0001)

    # You can specify your own tests in this method:
    @pytest.mark.xfail(reason = "pdkbuilding does not support new models yet")
    def test_MMI_12_Smatrix(self, smatrix):
        from pdkbuilding.testing import SMatrixTester
        # Create a tester object
        tester = SMatrixTester(smatrix)

        # Perform general tests
        assert tester.is_reciprocal(), "Component is not reciprocal"
        assert tester.is_passive(), "Component is active"


    @pytest.fixture
    def wavelengths(self):
        import numpy as np
        return np.arange(1.3, 1.8, 0.0001)

    # You can specify your own tests in this method:
    @pytest.mark.xfail(reason = "pdkbuilding does not support new models yet")
    def test_MMI_22_Smatrix(self, smatrix):
        from pdkbuilding.testing import SMatrixTester
        # Create a tester object
        tester = SMatrixTester(smatrix)

        # Perform general tests
        assert tester.is_reciprocal(), "Component is not reciprocal"
        assert tester.is_passive(), "Component is active"
