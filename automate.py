import matplotlib.pyplot as plt
import subprocess
import os

def getValue(string):
    return string.split(' ')[-1][:-1]

startNoice = 0.5
stepNoice = 0.3
finishNoice = 3.0
currentNoice = startNoice

startInteractionDst = 3.0
stepInteractionDst = 3.0
finishInteractionDst = 20.0
currentInteractionDst = startInteractionDst

typeOfDist = 0
listOfDist = [0, 1, 2]

mean = 15.0
deviation = 5.0
tryNumber = 0

while currentNoice <= finishNoice:
    currentInteractionDst = startInteractionDst
    while currentInteractionDst <= finishInteractionDst:
        for typeOfDist in listOfDist:
            for tryNumber in [0, 1, 2]:
                print("Params: {} {} {} {} {} {}".format(currentNoice, currentInteractionDst, typeOfDist, mean, deviation, tryNumber))
                subprocess.call("Disser.exe {} {} {} {} {} {}".format(currentNoice, currentInteractionDst, typeOfDist, mean, deviation, tryNumber), shell=True)
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
                fileName = "data/Noice_{}_distance_{}_type_{}_mean_{}_sttdev_{}_tryNum_{}".format(currentNoice, currentInteractionDst, typeOfDist, outMean, outDev, tryNum)
                
                plt.savefig(fileName + '.png', bbox_inches='tight')
                os.rename("ParticlesPos.txt", fileName + ".txt")
                os.rename("Picture.png", fileName + "_exm.png" )
        currentInteractionDst += stepInteractionDst
    currentNoice += stepNoice
