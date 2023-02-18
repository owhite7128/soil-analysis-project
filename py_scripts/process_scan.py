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
        n = 1

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
                color = (0, 255, 0)
                frame = cv.rectangle(frame, sp, ep, color, 2)

    
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