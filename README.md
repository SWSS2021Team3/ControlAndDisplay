# ControlAndDisplay
Control &amp; Display of Studio Project

# Usecase (draft)
![](https://yuml.me/diagram/scruffy/usecase/[Student]-(Login),[Student]-(View%20FaceImage),[Student]-(Add%20FaceImage),[Student]-(Remove%20FaceImage),[Student]-(Logout),[Admin]-(Login),[Admin]-(Start%20Attendance),[Admin]-(End%20Attendance),[Admin]-(Clear%20Attendance),(Start%20Attendance)%3E(View%20Attendance),(Play%20Record)%3E(View%20Attendance),(Play%20Record)%3E(View%20Record%20List),[Admin]-(Play%20Record),[Admin]-(Logout),(Start%20Attendance)%3E(View%20Video),(Play%20Record)%3E(View%20Video),(Start%20Attendance)%3E(Start%20Record),(End%20Attendance)%3E(End%20Record) "usecase")

# Class diagram (draft)
![](http://yuml.me/diagram/scruffy/class/[<<Receiver>>|onRecv()],[AttendanceController]-.-^[<<Receiver>>],[LoginManager]-.-^[<<Receiver>>],[FaceManager]-.-^[<<Receiver>>],[VideoDisplay]<-<>[AttendanceReceiver],[StudentList]<-<>[AttendanceReceiver],[AttendanceController||start();end();clear()],[LiveAttendanceController]-^[AttendanceController],[ReplayAttendanceController]-^[AttendanceController],[AttendanceController]<>->[AttendanceReceiver],[AttendanceController]uses-.->[Connection],[FaceManager||add();remove()]<>->[FaceDisplay],[LoginManager||login();logout()]uses-.->[Connection],[FaceManager]uses-.->[Connection],[Connection||connect();disconnect();send()] "class diagram")

# Setup
 - Install opencv
 - Set environment variable OPENCV_DIR (eg. c:\tools\opencv\build\x64\vc15)
