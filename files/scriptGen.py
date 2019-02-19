file = open("runScript.sh", "w")
file.write("#!/bin/bash\n")
for i,k  in enumerate(range(10,21)):
    for p in [1,2,4,8,16,32,64,128,256,512]:
        file.write("mpiexec -n "+str(p)+" ./ob1 test.txt ex1_"+str(i)+".txt 1000000 >> out/time"+str(k)+"_"+str(p)+".txt \n")
file.close()
