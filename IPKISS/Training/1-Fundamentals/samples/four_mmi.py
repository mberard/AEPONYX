from aeponyx import technology
from aeponyx.components.mmi.cell import  MMI_22
from ipkiss3 import all as i3


class FourMMIs(i3.PCell):

    mmi = i3.ChildCellProperty(doc="MMI that is replicated four times.")

    def _default_mmi(self):
        mmi = MMI_22(name=self.name + "_mmi")
        mmi.Layout(length=30.0)
        return mmi

    class Layout(i3.LayoutView):
        mmi_sep = i3.PositiveNumberProperty(default=20.0)

        def _generate_instances(self, insts):
            for cnt in range(4):
                trans = i3.Translation(translation=(0.0, cnt * self.mmi_sep))




                insts += i3.SRef(name="MMI_{}".format(cnt),
                                 reference=self.mmi,
                                 transformation= trans)

            return insts

        def _generate_ports(self, ports):
            for cnt in range(4):
                mmip = self.instances["MMI_{}".format(cnt)]
                for p in mmip.ports:
                    ports += p.modified_copy(name="{}_{}".format(p.name, cnt))

            return ports


cell = FourMMIs(name="4mmi")
lv = cell.Layout()
lv.visualize(annotate=True)
lv.write_gdsii("fourmmis.gds")


