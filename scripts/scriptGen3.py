import random
file = open("runScript.sh", "w")
file.write("#!/bin/bash\n")
for i,k  in enumerate(range(10,17)):
    num = random.randint(0, (2**(k)))
    extra = random.randint(0,1)
    binString = "{0:b}".format(num).rjust(2**(k),'0')
    binString += str(extra)
    for p in [1,2,4,8,16,32,64,128,256,512]:
        file.write("echo "+str(k)+" "+str(p)+"\n")
        file.write("mpiexec -n "+str(p)+" ./ob1 ex3_"+str(i)+".txt "+binString+" > out/time"+str(p)+"_"+str(i)+"0 \n")
file.close()
