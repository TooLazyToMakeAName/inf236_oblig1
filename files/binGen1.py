for i,k  in enumerate(range(10,21)):
    file = open("ex1_"+str(i)+".txt", "w")
    file.write(str((2**k)+1+(2**(k-1)))+"\n")
    file.write((2**k)*'0'+'1'+(2**(k-1))*'0')
    file.close()
