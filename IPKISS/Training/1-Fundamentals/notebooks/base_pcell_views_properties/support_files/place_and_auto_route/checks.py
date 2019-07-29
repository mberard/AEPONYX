from ipkiss3 import all as i3


def check_1(my_ring2):
    try:
        if all([s == 1.0 for s in my_ring2.get_default_view(i3.LayoutView).coupler_spacings]):
            print "Congratulations! Your cell has the correct coupler spacings"
        else:
            print "Your PCell does not have the correct coupler spacings."
    except:

        print "There is something wrong with your PCell"
