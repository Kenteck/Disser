import matplotlib.pyplot as plt
import subprocess
import os

def getValue(string):
    return string.split(' ')[-1][:-1]

noice = 0.5
interactionDst = 5.0
typeOfDist = 0
mean = 15.0
deviation = 5.0
tryNumber = 0

while noice <= 3.0:
    while interactionDst <= 30.0:
        for typeOfDist in [0, 1, 2]:
            for tryNumber in [0, 1, 2]:
                print("Params: {} {} {} {} {} {}".format(noice, interactionDst, typeOfDist, mean, deviation, tryNumber))
                subprocess.call("Disser.exe {} {} {} {} {} {}".format(noice, interactionDst, typeOfDist, mean, deviation, tryNumber), shell=True)
                f = open("ParticlesPos.txt", "r")
                numberOfParticles = 0
                circles = 0
                tryNum = 0
                outMean = 0
                outDev = 0
                distirbutionType = ""
                particleList = []
                indexList = []
                for x in f:
                    if "Distribution" in x:
                        distirbutionType = getValue(x)
                    elif "particles" in x:
                        numberOfParticles = int(getValue(x))
                    elif "circles" in x:
                        circles = int(getValue(x))
                    elif "Try" in x:
                       tryNum = int(getValue(x))
                    elif "Mean" in x:
                       outMean = getValue(x)
                    elif "SttDev" in x:
                       outDev = getValue(x)
                    else:
                        particleList.append(int(getValue(x)))
                f.close()
                for i in range(circles):
                    indexList.append(i)
                
                fig = plt.figure()
                ax = fig.add_axes([0,0,1,1])
                ax.set(title = "Number of particles histogram for " + distirbutionType + " distribution",
                    xlabel = "Belt",
                    ylabel = "Number of particles")
                ax.bar(indexList, particleList)
                fileName = "data/Noice_{}_distance_{}_type_{}_mean_{}_sttdev_{}_tryNum_{}".format(noice, interactionDst, typeOfDist, outMean, outDev, tryNum)
                
                plt.savefig(fileName + '.png', bbox_inches='tight')
                os.rename("ParticlesPos.txt", fileName + ".txt")
                os.rename("Picture.png", fileName + "_exm.png" )
        interactionDst += 5
    noice += 0.3
