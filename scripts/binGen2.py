import random
for c, k in enumerate([1024,2048,4096,8192,16384,32768,65536]):
    file = open("ex2_"+str(c)+".txt", "w")
    file.write(str(k)+"\n")
    for bu in range(k):
        i = random.randint(0, (2**(k)))
        binaryString = "{0:b}".format(i).rjust(k,'0')
        file.write(binaryString+"\n")
    file.close()

