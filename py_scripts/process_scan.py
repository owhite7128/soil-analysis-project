import cv2 as cv
import numpy as np

cap = cv.VideoCapture(0)
cap.set(cv.CAP_PROP_SETTINGS, 1)

# Check if camera opened successfully
if (cap.isOpened()== False): 
  print("Error opening video stream or file")
 
# Read until video is completed
while(cap.isOpened()):
  # Capture frame-by-frame
  ret, frame = cap.read()
  if ret == True:
    blurred = cv.medianBlur(frame, 7)
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
    
    gray = cv.cvtColor(blurred, cv.COLOR_BGR2GRAY)

    #lower_bound = np.array([30, 100, 60])	 
    #upper_bound = np.array([50, 150, 100])

    #mask = cv.inRange(hsv, lower_bound, upper_bound)
    #blur_mask = cv.Laplacian(mask, cv.CV_8U, mask, ksize = 5)
    #canny = cv.Canny(blur_mask, 200, 250)
    #
    canny_gray = cv.Canny(gray, 400, 500)
    contours , hierarchy = cv.findContours(canny_gray, cv.RETR_TREE, cv.CHAIN_APPROX_NONE)

    large_area = 0
    big_contour = 0

    for contour in contours:
        approx = cv.approxPolyDP(contour, 0.01* cv.arcLength(contour, True), True)
        if len(approx) == 4:
            x, y, w, h = cv.boundingRect(approx)
            if w*h > large_area:
                large_area = w*h
                big_contour = contour

    if type(big_contour) == type(np.array([])):
        approx = cv.approxPolyDP(big_contour, 0.01* cv.arcLength(big_contour, True), True)
        cv.drawContours(frame, [approx], 0, (0, 0, 0), 5)
        x, y, w, h = cv.boundingRect(approx)
        n = 4

        rect_pts = [[0 for x in range(n)] for y in range(n)] 
        for i in range(n):
            for j in range(n):
                sp_x = int(x+i*(w/n))
                sp_y = int(y+j*(h/n))
                ep_x = int(x+(i+1)*(w/n))
                ep_y = int(y+(j+1)*(h/n))
                sp = (sp_x, sp_y)
                ep = (ep_x, ep_y)
                rect_pts[i][j] = [sp,ep]

        # This is where the code to prune rects goes. Check every pt on rect, if a single on returns that it is outside of contour, get
        # rid of that rectangle and move to next.
        # u = ((bs.y - as.y) * bd.x - (bs.x - as.x) * bd.y) / (bd.x * ad.y - bd.y * ad.x)
        # v = ((bs.y - as.y) * ad.x - (bs.x - as.x) * ad.y) / (bd.x * ad.y - bd.y * ad.x)

        for i in range(n):
            for j in range(n):
              in_bound = 1
              pt_avg = ((rect_pts[i][j][0][0]+rect_pts[i][j][1][0])/2, (rect_pts[i][j][0][1]+rect_pts[i][j][1][1])/2)
              pt_avg_cross = 0

              for k in range(len(approx)):
                if in_bound:
                  if k+1 < len(approx):
                    r_Base = (approx[k][0][0], approx[k][0][1])
                    r_Ray = (approx[k+1][0][0] - approx[k][0][0], approx[k+1][0][1] - approx[k][0][1])
                  else:
                    #r_Base = (approx[k][0][0], approx[k][0][1])
                    #r_Ray = (approx[0][0][0] - approx[k][0][0], approx[0][0][1] - approx[k][0][1])
                    r_Base = (approx[0][0][0], approx[0][0][1])
                    r_Ray = (approx[k][0][0] - approx[0][0][0], approx[k][0][1] - approx[0][0][1])

                  if ((r_Ray[0]*0 - r_Ray[1]*1) != 0 ):
                    u_avg = (((r_Base[1]-pt_avg[1])*r_Ray[0]-(r_Base[0]-pt_avg[0])*r_Ray[1])/(r_Ray[0]*0 - r_Ray[1]*1))
                    v_avg = (((r_Base[1]-pt_avg[1])*1-(r_Base[0]-pt_avg[0])*0)/(r_Ray[0]*0 - r_Ray[1]*1))
                    if (u_avg >= 0 and v_avg >= 0):
                      pt_avg_cross += 1
              #weirded out...
              if ((pt_avg_cross % 2) != 0):
                in_bound = 0
              else: 
                in_bound = 1
              
              if not in_bound:
                rect_pts[i][j][0] = (-1,-1)
                rect_pts[i][j][1] = (-1,-1)


        color = (0, 255, 0)
        for i in range(n):
            for j in range(n):
              if rect_pts[i][j][0][0] != -1 and rect_pts[i][j][0][1] != -1 and rect_pts[i][j][1][0] != -1 and rect_pts[i][j][1][1] != -1:
                frame = cv.rectangle(frame, rect_pts[i][j][0], rect_pts[i][j][1], color, 2)



    
    #print(rect_pts[1][3])
    # Display the resulting frame
    cv.imshow('frame', frame)
 
    # Press Q on keyboard to  exit
    if cv.waitKey(25) & 0xFF == ord('q'):
      break
 
  # Break the loop
  else: 
    break
 
# When everything done, release the video capture object
cap.release()
 
# Closes all the frames
cv.destroyAllWindows()