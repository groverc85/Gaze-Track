//
//  main.cpp
//  eye_like
//
//  Created by Grover Chen on 4/9/15.
//  Copyright (c) 2015 Grover Chen. All rights reserved.
//

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <queue>
#include <stdio.h>
#include <math.h>
#include "mainwindow.h"
#include <QApplication>
#include <unistd.h>
#include "constants.h"
#include "drawGazeCycles.h"
#include "findEyeCenter.h"
#include "findEyeCorner.h"

int leftcenterXposition, leftcenterYposition, rightcenterXposition, rightcenterYposition;

extern int gazeXposition, gazeYposition;

/** Function Headers */
void detectAndDisplay( cv::Mat frame );

/** Global variables */
//-- Note, either copy these two files from opencv/data/haarscascades to your current folder, or change these locations
cv::String face_cascade_name = "/Users/chenjiongjian/Desktop/SRTP-FINAL/gazetrackfinal/haarcascade_frontalface_alt.xml";
cv::CascadeClassifier face_cascade;
std::string main_window_name = "Capture - Face detection";
std::string face_window_name = "Capture - Face";
cv::RNG rng(12345);
cv::Mat debugImage;
cv::Mat skinCrCbHist = cv::Mat::zeros(cv::Size(256, 256), CV_8UC1);

/**
 * @function main
 */
int main(int argc, char *argv[]){

    QApplication a(argc, argv);
    MainWindow w;
    CvCapture* capture;
    cv::Mat frame;
    w.showFullScreen();
    
    // Load the cascades
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade, please change face_cascade_name in source code.\n"); return -1; };
    /*
    cv::namedWindow(main_window_name,CV_WINDOW_NORMAL);
    cv::moveWindow(main_window_name, 400, 100);
    cv::namedWindow(face_window_name,CV_WINDOW_NORMAL);
    cv::moveWindow(face_window_name, 10, 100);
    cv::namedWindow("Right Eye",CV_WINDOW_NORMAL);
    cv::moveWindow("Right Eye", 10, 600);
    cv::namedWindow("Left Eye",CV_WINDOW_NORMAL);
    cv::moveWindow("Left Eye", 10, 800);
    */
    createCornerKernels();
    ellipse(skinCrCbHist, cv::Point(113, 155.6), cv::Size(23.4, 15.2),
            43.0, 0.0, 360.0, cv::Scalar(255, 255, 255), -1);
    
    // Read the video stream
    capture = cvCaptureFromCAM( -1 );
    if( capture ) {
        while( true ) {
            frame = cvQueryFrame( capture );
            // mirror it
            cv::flip(frame, frame, 1);
            frame.copyTo(debugImage);
            
            // Apply the classifier to the frame
            if( !frame.empty() ) {
                detectAndDisplay( frame );
            }
            else {
                printf(" --(!) No captured frame -- Break!");
                break;
            }
            
//            imshow(main_window_name,debugImage);
            
            int c = cv::waitKey(10);
            if( (char)c == 'c' ) { break; }
            if( (char)c == 'f' ) {
                imwrite("frame.png",frame);
            }
            w.changexy(gazeXposition,gazeYposition);
        }
    }
    
    releaseCornerKernels();



    return a.exec();
    return 0;
}

void findEyes(cv::Mat frame_gray, cv::Rect face) {
    cv::Mat faceROI = frame_gray(face);
    cv::Mat debugFace = faceROI;
    int eyeXposition, eyeYposition;


    
    if (kSmoothFaceImage) {
        double sigma = kSmoothFaceFactor * face.width;
        GaussianBlur( faceROI, faceROI, cv::Size( 0, 0 ), sigma);
    }
    //-- Find eye regions and draw them
    int eye_region_width = face.width * (kEyePercentWidth/100.0);
    int eye_region_height = face.width * (kEyePercentHeight/100.0);
    int eye_region_top = face.height * (kEyePercentTop/100.0);
    cv::Rect leftEyeRegion(face.width*(kEyePercentSide/100.0),
                           eye_region_top,eye_region_width,eye_region_height);
    cv::Rect rightEyeRegion(face.width - eye_region_width - face.width*(kEyePercentSide/100.0),
                            eye_region_top,eye_region_width,eye_region_height);
    
    //-- Find Eye Centers
    cv::Point leftPupil = findEyeCenter(faceROI,leftEyeRegion,"Left Eye");
    cv::Point rightPupil = findEyeCenter(faceROI,rightEyeRegion,"Right Eye");
    // get corner regions
    cv::Rect leftRightCornerRegion(leftEyeRegion);
    leftRightCornerRegion.width -= leftPupil.x;
    leftRightCornerRegion.x += leftPupil.x;
    leftRightCornerRegion.height /= 2;
    leftRightCornerRegion.y += leftRightCornerRegion.height / 2;
    cv::Rect leftLeftCornerRegion(leftEyeRegion);
    leftLeftCornerRegion.width = leftPupil.x;
    leftLeftCornerRegion.height /= 2;
    leftLeftCornerRegion.y += leftLeftCornerRegion.height / 2;
    cv::Rect rightLeftCornerRegion(rightEyeRegion);
    rightLeftCornerRegion.width = rightPupil.x;
    rightLeftCornerRegion.height /= 2;
    rightLeftCornerRegion.y += rightLeftCornerRegion.height / 2;
    cv::Rect rightRightCornerRegion(rightEyeRegion);
    rightRightCornerRegion.width -= rightPupil.x;
    rightRightCornerRegion.x += rightPupil.x;
    rightRightCornerRegion.height /= 2;
    rightRightCornerRegion.y += rightRightCornerRegion.height / 2;
    rectangle(debugFace,leftRightCornerRegion,200);
    rectangle(debugFace,leftLeftCornerRegion,200);
    rectangle(debugFace,rightLeftCornerRegion,200);
    rectangle(debugFace,rightRightCornerRegion,200);
    // change eye centers to face coordinates
    rightPupil.x += rightEyeRegion.x;
    rightPupil.y += rightEyeRegion.y;
    leftPupil.x += leftEyeRegion.x;
    leftPupil.y += leftEyeRegion.y;
    
    rightPupil.x += face.x;
    rightPupil.y += face.y;
    leftPupil.x += face.x;
    leftPupil.y += face.y;//correct
    
    
//    printf("rightPupil(x,y): %d,%d\n",rightPupil.x,rightPupil.y);
//    printf("leftPupil(x,y): %d,%d\n",leftPupil.x,rightPupil.y);
    
    //rightcenterXposition:上一帧的右眼X坐标，leftcenterXposition:上一帧左眼X坐标
    //eyeXposition:摄像头内眼睛的偏移量
    //EyeOffsetWidthMax:摄像头内眼睛最大偏移量
    eyeXposition = ((rightPupil.x - rightcenterXposition) + (leftPupil.x - leftcenterXposition)) / 2.0;
    eyeYposition = ((rightPupil.y - rightcenterYposition) + (leftPupil.y - leftcenterYposition)) / 2.0;
    
//    printf("eyeXposition: %d\n",eyeXposition);
//    printf("eyeYposition: %d\n",eyeYposition);
    
    
    if ((eyeXposition < EyeOffsetWidthMax) && (eyeYposition < EyeOffsetHeightMax)){
        gazecycles(eyeXposition,eyeYposition);
    }
    


    // draw eye centers
    circle(debugFace, rightPupil, 3, 1234);
    circle(debugFace, leftPupil, 3, 1234);
    
//    imshow(face_window_name, faceROI);
    
    leftcenterXposition = leftPupil.x;
    leftcenterYposition = leftPupil.y;
    rightcenterXposition = rightPupil.x;
    rightcenterYposition = rightPupil.y;
    
}
/**
 * @function detectAndDisplay
 */
void detectAndDisplay( cv::Mat frame ) {
    std::vector<cv::Rect> faces;
    //cv::Mat frame_gray;
    
    std::vector<cv::Mat> rgbChannels(3);
    cv::split(frame, rgbChannels);
    cv::Mat frame_gray = rgbChannels[2];
    
    //cvtColor( frame, frame_gray, CV_BGR2GRAY );
    //equalizeHist( frame_gray, frame_gray );
    //cv::pow(frame_gray, CV_64F, frame_gray);
    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150) );
    //  findSkin(debugImage);
    
    for( int i = 0; i < faces.size(); i++ )
    {
        rectangle(debugImage, faces[i], 1234);
    }
    //-- Show what you got
    if (faces.size() > 0) {
        findEyes(frame_gray, faces[0]);
    }
}
