from ipkiss3 import all as i3
### Solution Check ###


def check_1(my_square):

    try:
        if my_square.length == 10.0 and my_square.width == 5.0:
            print "Congratulations! You have made your first PCell."
        else:
            print "Your PCell does not have the correct values. Did you use the correct property names?"
    except:

        print "There is something wrong with your PCell."


### Solution Check ###

def check_2(SquareWithDefaults):

    try:
        square1 = SquareWithDefaults()
        square2 = SquareWithDefaults(length=10.0)
        if square1.length == 5.0 and square1.width == 5.0:
            if square2.length == square2.width:
                print "Congratulations! You mastered properties and defaults."
            else:
                print "Your PCell does not have the correct values."
        else:
            print "Your PCell does not have the correct defaults."

    except:
        print "There is something wrong with your PCell."
