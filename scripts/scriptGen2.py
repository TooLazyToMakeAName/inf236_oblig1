file = open("runScript.sh", "w")
file.write("#!/bin/bash\n")
for i,k  in enumerate(range(7)):
    for p in [1,2,4,8,16,32,64,128,256,512]:
        file.write("echo "+str(k)+" "+str(p)+"\n")
        file.write("mpiexec -n "+str(p)+" ./ob1 test.txt ex2_"+str(i)+".txt 1000 >> out/time"+str(i)+"_"+str(p)+".txt \n")
file.close()
