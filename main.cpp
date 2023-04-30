#include <iostream>
#include "main.h"
bool roiAcquired = false;
bool leftMouseDown = false;
Mat originalImage;
Mat integralImage;
Point point1, point2;

int main(int argc, char **argv) {
    string imageFile;
    parseParameters(argc, argv, &imageFile);
    cout << "Selected Image: " << imageFile << endl;

    try {
        originalImage = imread(imageFile, 0);
        if (originalImage.empty())
            throw (string("Cannot open input originalImage ") + imageFile);
        originalImage.convertTo(originalImage, CV_8UC1);
    }
    catch (string &str) {
        cerr << "Error: " << argv[0] << ": " << str << endl;
        return (1);
    }
    catch (Exception &e) {
        cerr << "Error: " << argv[0] << ": " << e.msg << endl;
        return (1);
    }

    integralImage = createIntegralImage(originalImage);
    imshow("Image", originalImage);
    setMouseCallback("Image", mouse_click, 0);
    waitKey();
    return 0;
}

Mat createIntegralImage(Mat image){
    Mat integral(image.rows, image.cols, CV_32FC1, Scalar(0.0));

    // i is y, j is x
    for(int i = 0; i < integral.cols; i++){
        for(int j = 0; j < integral.rows; j++){
            if(i == 0 && j == 0){
                integral.at<float>(i, j) = image.at<uchar>(i, j);
            }
            else if(i == 0){
                integral.at<float>(i, j) = image.at<uchar>(i, j) + integral.at<float>(i, j - 1);
            }
            else if(j == 0){
                integral.at<float>(i, j) = image.at<uchar>(i, j) + integral.at<float>(i - 1, j);
            }
            else{
                integral.at<float>(i, j) = image.at<uchar>(i, j) + integral.at<float>(i - 1, j) + integral.at<float>(i, j - 1) - integral.at<float>(i - 1, j - 1);
            }
        }
    }
    return integral;
}

void mouse_click(int event, int x, int y, int flags, void *param)
{

    switch(event)
    {
        case EVENT_MOUSEMOVE:{
            if(leftMouseDown && !roiAcquired){
                int xStart, yStart, xEnd, yEnd;
                xStart = min(point1.x, x);
                yStart = min(point1.y, y);
                xEnd = max(point1.x, x);
                yEnd = max(point1.y, y);

                Mat clonedImage = originalImage.clone();
                rectangle(clonedImage, Point(xStart, yStart), Point(xEnd, yEnd), Scalar(255,0,0), 2, LINE_8);
                imshow("Image", clonedImage);
            }
            break;
        }
        case EVENT_LBUTTONDOWN:
        {
            cout<<"Mouse Button Pressed"<<endl;
            leftMouseDown = true;
            if(!roiAcquired)
            {
                point1.x = x;
                point1.y = y;
            }
            else
            {
                std::cout<<"ROI Already Acquired"<<std::endl;
            }
            break;
        }
        case EVENT_LBUTTONUP:
        {
            leftMouseDown = false;
            if(!roiAcquired)
            {
                Mat cl;
                cout<<"Mouse Button Released"<<std::endl;

                point2.x = x;
                point2.y = y;

                Mat roi(originalImage, Rect(point1, point2));

                cout << "Point 1: " << point1.x << ", " << point1.y << endl;
                cout << "Point 2: " << point2.x << ", " << point2.y << endl;

                int xStart, xEnd, yStart, yEnd;
                xStart = min(point1.x, point2.x);
                xEnd = max(point1.x, point2.x);
                yStart = min(point1.y, point2.y);
                yEnd = max(point1.y, point2.y);

                // (yEnd, xEnd) - (yStart-1, xEnd) + (yStart-1, xStart-1) - (yEnd, xStart-1)
                float totalROISum = integralImage.at<float>(yEnd, xEnd) - integralImage.at<float>(yStart - 1, xEnd) + integralImage.at<float>(yStart - 1, xStart - 1) - integralImage.at<float>(yEnd, xStart - 1);
                cout << fixed << "Sum of all pixel values inside the ROI: " << int(totalROISum) << endl;
                float averagePixelValue = (totalROISum)/roi.total();
                cout << fixed << "Average pixel value in the ROI: " << averagePixelValue << endl;
                threshold( roi, roi, averagePixelValue, 255, 2 );

                roi.copyTo(originalImage(cv::Rect(xStart,yStart,roi.cols, roi.rows)));

                imshow("Image", originalImage);
                roiAcquired = true;
            }
            else
            {
                cout<<"ROI Already Acquired"<<endl;
            }
            break;
        }

    }

}

static int parseParameters(int argc, char **argv, string *imageFile) {
    String keys =
            {
                    "{help h usage ? |                            | print this message   }"
                    "{@imagefile|| originalImage you want to use	}"
            };

    // Get required parameters.
    // If no image is passed in, or if the user passes in a help param, usage info is printed
    CommandLineParser parser(argc, argv, keys);
    parser.about("II v1.0");

    if (!parser.has("@imagefile") || parser.has("help")) {
        parser.printMessage();
        return (0);
    }

    *imageFile = parser.get<string>("@imagefile");
    return (1);
}
