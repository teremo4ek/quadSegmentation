#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

//#include <stdio.h>
//#include <stdlib.h>
#include <io.h>

#include "quadsegmentation.h"

cv::Mat src;
cv::Mat bw;
cv::Mat contMat;

int g_thresh = 50;
int g_blur = 3;
CvMemStorage* g_storage = NULL;
cv::RNG rng(12345);
 
void on_trackbar(int) {
 
    contMat = bw.clone();

    equalizeHist( contMat, contMat );

    if(g_blur == 0) g_blur = 1;
    //cv::blur(contMat, contMat, cv::Size(g_blur, g_blur));
    cv::GaussianBlur(contMat, contMat, cv::Size(7,7), 1.5, 1.5);
    cv::Canny(contMat, contMat, g_thresh, g_thresh*4, 3);
    cv::Mat cropMat = contMat.clone();

    //cv::threshold( contMat, contMat, g_thresh, 255, CV_THRESH_BINARY );
    std::cout<< "g_thresh = " << g_thresh <<std::endl;

    std::vector<cv::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours( contMat, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );

    contMat.setTo(cv::Scalar::all(0));
    cvtColor(contMat, contMat, CV_GRAY2RGB);

    //cvZero( g_gray );
    if( contours.size() )
    {
        std::cout<< "cv::findContours size = " << contours.size() <<std::endl;

//       cv::drawContours(
//            contMat,
//            contours,
//            -1,
//            cvScalarAll(34)
//        );

        int shiftByY = contMat.rows/contours.size();
        cv::Point textOrg = cv::Point();

        int ind = -1;
        double maxArea = 0;
        for( int i = 0; i< contours.size(); i++ )
        {
            if(contours.at(i).empty())
                    continue;

            //double currArea = cv::contourArea(contours.at(i));
            const cv::RotatedRect rr = cv::minAreaRect(contours.at(i));
            const double currArea = rr.size.area();

            if(maxArea < currArea)
            {
                ind = i;
                maxArea = currArea;
            }
        }

        if(ind >= 0)
        {
            std::cout<< "max area contours ind = " << ind <<std::endl;
            cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            cv::drawContours( contMat, contours, ind, color, 1, CV_AA, cv::noArray(), 0, cv::Point() );

            std::string text= std::to_string(ind) + ": area = " + std::to_string(maxArea);
            int fontFace = CV_FONT_HERSHEY_PLAIN;
            double fontScale = 1;
            //int thickness = 1;
            textOrg = textOrg + cv::Point(0, 30);
            cv::putText(contMat, text, textOrg, fontFace, fontScale, color);
        }
    }

    cv::imshow( "Contours", contMat );

    std::vector< std::vector<cv::Point> > contours1;
    cv::findContours(cropMat, contours1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    // CV_FILLED fills the connected components found
    //cv::drawContours(cropMat, contours1, -1, cv::Scalar(255), CV_FILLED);
    std::vector<cv::RotatedRect> minRect( contours1.size() );
    std::vector<cv::RotatedRect> minEllipse( contours1.size() );

    for( int i = 0; i < contours1.size(); i++ )
     { minRect[i] = cv::minAreaRect( cv::Mat(contours1[i]) );
       if( contours1[i].size() > 5 )
         { minEllipse[i] = fitEllipse( cv::Mat(contours1[i]) ); }
     }

    /// Draw contours1 + rotated rects + ellipses
    cv::Mat drawing = cv::Mat::zeros( cropMat.size(), CV_8UC3 );
     for( int i = 0; i< contours1.size(); i++ )
     {
       cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       // contour
       cv::drawContours( drawing, contours1, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
       // ellipse
       //cv::ellipse( drawing, minEllipse[i], color, 2, 8 );
       // rotated rectangle
       cv::Point2f rect_points[4]; minRect[i].points( rect_points );
       //for( int j = 0; j < 4; j++ )
       //   line( drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );
     }
    cv::imshow( "cropMat", drawing );
}

int main( int argc, char** argv )
{
    struct _finddata_t c_file;
    long hFile;
    char imageDirectory[] = "d:\\project\\openCV\\perspective_correction\\img";
    char imageFileType[] = "jpg";
    char fullImagePath[1000];
    char buffer[1000];
    sprintf(buffer,"%s\\*.%s", imageDirectory, imageFileType);
    hFile = _findfirst( buffer, &c_file );
    /*Check to make sure that there are files in directory*/
    if( hFile == -1L )
        printf( "No %s files in current directory!\n", imageFileType );
    else
    {
        // List all files in directory
        printf( "Listing of files:\n" );
        // Loop through all images of imageFileType
        do
        {
            // Show file name
            printf( "\nOpening File: %s \n", c_file.name);
            sprintf(fullImagePath,"%s\\%s", imageDirectory, c_file.name);

            cv::Mat src = cv::imread(fullImagePath);
            if (src.empty())
                return -1;
            if(src.cols > 1000 || src.rows > 1000)
                cv::resize(src, src, cv::Size(src.cols/5, src.rows/5));
            cv::cvtColor(src, bw, CV_BGR2GRAY);
            cv::imshow("Get_the_edge_map", bw);


            cvNamedWindow( "Contours", 1 );
            cvCreateTrackbar(
                "Threshold",
                "Contours",
                &g_thresh,255,
                on_trackbar
            );
            cvCreateTrackbar(
                "BLUR",
                "Contours",
                &g_blur,9,
                on_trackbar
            );
            on_trackbar(0);

            int key = cv::waitKey(0);
            if(key == 13)
            {
                findPointRect(src, contMat);
                key = cv::waitKey(0);
            }

            if (key == 27) {
                break;
            }

        }while( _findnext( hFile, &c_file ) == 0 );

        // Close file finder object
        _findclose( hFile );
    }

    return 0;
}
