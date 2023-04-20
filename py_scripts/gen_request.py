import cv2 as cv
import numpy as np
import os
import datetime
import json

cap = cv.VideoCapture(0)
cap.set(cv.CAP_PROP_AUTOFOCUS, 1)
cap.set(cv.CAP_PROP_FOCUS, 255)

pts = []

dt = datetime.date.today()

def draw_circle(event,x,y,flags,param):
    global mouseX,mouseY
    if event == cv.EVENT_LBUTTONDOWN:
        print (f"Goat: {x}, {y}")
        if len(pts) == 4:
            pts.pop(0)
        pts.append((x,y))
        mouseX,mouseY = x,y

cv.namedWindow('frame')
cv.setMouseCallback('frame', draw_circle)

# Check if camera opened successfully
if (cap.isOpened()== False): 
    print("Error opening video stream or file")

while(cap.isOpened()):
    ret, frame = cap.read()
    undone = frame
    if ret == True:
        for i in range(len(pts)):
            cv.circle(frame, (pts[i][0], pts[i][1]), 10, (255, 0, 0), -1)

        if len(pts) == 4:
            pt1 = (0,0)
            pt2 = (0,0)
            pt3 = (0,0)
            pt4 = (0,0)

            for i in range(0,4):
                temptuple = (0,0)
                minCoord = 10000000

                for j in pts:
                    if not (j == pt1 or j == pt2 or j == pt3 or j == pt4):
                        if j[0]*j[1] < minCoord:
                            minCoord = j[0]*j[1]
                            temptuple = j
                if pt1 == (0,0):
                    pt1 = temptuple
                elif pt2 == (0,0):
                    pt2 = temptuple
                elif pt3 == (0,0):
                    pt3 = temptuple
                elif pt4 == (0,0):
                    pt4 = temptuple

            cv.line(frame, pt1, pt2, (0, 255, 0), 2)
            cv.line(frame, pt1, pt3, (0, 255, 0), 2)
            cv.line(frame, pt2, pt4, (0, 255, 0), 2)
            cv.line(frame, pt3, pt4, (0, 255, 0), 2)



        cv.imshow('frame', frame)

        if cv.waitKey(25) & 0xFF == ord('c'):
            if len(pts) == 4:
                res = []

                for path in os.listdir(r"/Users/owhite/Documents/cuSp23/GEEN1400/final_proj/soil-analysis-project/electron_app/public/scan_images"):
                    # check if current path is a file
                    if os.path.isfile(os.path.join(r"/Users/owhite/Documents/cuSp23/GEEN1400/final_proj/soil-analysis-project/electron_app/public/scan_images", path)):
                        res.append(path)
                scanId = 0
                tdate = f"{dt.month}_{dt.day}_{int(str(dt.year)[2:4])}"
                for i in res:
                    if i[0:-6] == tdate:
                        scanId += 1

                fname = f"{tdate}_{chr(scanId+97)}"
            
                pt1 = (0,0)
                pt2 = (0,0)
                pt3 = (0,0)
                pt4 = (0,0)

                for i in range(0,4):
                    temptuple = (0,0)
                    minCoord = 10000000

                    for j in pts:
                        if not (j == pt1 or j == pt2 or j == pt3 or j == pt4):
                            if j[0]*j[1] < minCoord:
                                minCoord = j[0]*j[1]
                                temptuple = j
                    if pt1 == (0,0):
                        pt1 = temptuple
                    elif pt2 == (0,0):
                        pt2 = temptuple
                    elif pt3 == (0,0):
                        pt3 = temptuple
                    elif pt4 == (0,0):
                        pt4 = temptuple

                os.chdir(r"/Users/owhite/Documents/cuSp23/GEEN1400/final_proj/soil-analysis-project/electron_app/public/scan_images")
                cX = int((pt1[0] + pt4[0]) /2.0)
                cY = int((pt1[1] + pt4[1]) / 2.0)

                minX = 1000000
                minY = 1000000
                maxX = 0
                maxY = 0
                for i in pts:
                    if i[0] < minX:
                        minX = i[0]
                    if i[0] > maxX:
                        maxX = i[0]
                    if i[1] < minY:
                        minY = i[1]
                    if i[1] > maxY:
                        maxY = i[1]

                tL = (cX-300, cY-300)
                bR = (cX+300, cY+300)

                print(f"({minX}, {minY}), ({maxX}, {maxY})")
                cX = int((minX + maxX) /2.0)
                cY = int((minY + maxY) / 2.0)

                json_d = {"pts":[(pt1[0]-(cX - (abs(minX-cX))), pt1[1]-(cY - (abs(minY-cY)))), (pt2[0]-(cX - (abs(minX-cX))), pt2[1]-(cY - (abs(minY-cY)))), (pt3[0]-(cX - (abs(minX-cX))), pt3[1]-(cY - (abs(minY-cY)))), (pt4[0]-(cX - (abs(minX-cX))), pt4[1]-(cY - 0.5*(abs(minY-cY))))], "img_name":f"{fname}.png"}

                outImg = undone[minY:maxY, minX:maxX]
                
                cv.imwrite(f"{fname}.png", outImg)
                with open("scan.json", "w") as fp:
                    json.dump(json_d, fp)
            
                break




        if cv.waitKey(25) & 0xFF == ord('q'):
            break

    else:
        break

cap.release()

cv.destroyAllWindows()