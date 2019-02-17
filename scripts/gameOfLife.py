for i in range(2**9):
    binaryString = "{0:b}".format(i).rjust(9,'0')
    negbours = 0
    for i,n in enumerate(binaryString):
        if (i != 4) and (n == '1'):
            negbours += 1
    if (binaryString[4] == '1'):
            #alive
            if (negbours == 2) or (negbours == 3):
                binaryString += ' 1'
            else:
                binaryString += ' 0'
    else:
        #dead
        if negbours == 3 :
            binaryString += ' 1'
        else:
            binaryString += ' 0'
    print(binaryString)
