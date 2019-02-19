import random
for c, k in enumerate([1024,2048,4096,8192,16384,32768,65536]):
    file = open("ex3_"+str(c)+".txt", "w")
    table = open("tableNew.txt", "r")
    file.write(str(k)+"\n")
    file.write(str(k)+"\n")
    file.write(str(60)+"\n")
    file.write(str(1)+"\n")
    for line in table.readlines():
        file.write(line)
    file.write("1\n")
    for bu in range(k):
        b = random.randint(0,1)
        i1  = random.randint(0,59)
        i2  = random.randint(0,59)
        file.write("{} {} {}".format(b,i1,i2)+"\n")
    file.close()
