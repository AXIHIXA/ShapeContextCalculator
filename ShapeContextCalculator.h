#pragma once

#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

// used for calculting the distance between pixels and recording
struct Neighbor;

// class used to calculate the velocity of the mask of a frame using the Shape Contexts
// divide the shape context into 12 * 5 blocks, as shown in
// https://www2.eecs.berkeley.edu/Research/Projects/CS/vision/shape/sc_digits.html
class ShapeContextCalculator
{
public:
    // constrcutor recommended
    // @param frame0: the previous frame
    // @param frame1: present frame, the frame whose pixels' vecolity is to be calculated based on frame1
    // @param timeInterval: the actual time interval between the 2 frames, in ms
    // @param sampling number: the total number of sampling pixels on the border of the mask. CAUTION: the actual sampling number may +- 2
    // the frame cv::mat should be CV_8UC1 mat, with pixels on the border set 0 while others set 255
    // when you use this class, you can pass in mats in your prefered formats, and you just need to
    // modify the private border-finding method `bool onBorder(int which, const Point2i &origin)`
    ShapeContextCalculator(const Mat &frame0, const Mat &frame1, double timeInterval, int samplingNumber = 70);

    // default constructor
    // DO NOT USE THIS METHOD TO NEW INSTANCES OF THIS CLASS
    ShapeContextCalculator();

    // default deconstructor
    ~ShapeContextCalculator();

    // calculate the velocity of a point on the mask
    // @param row, column: point (row, column) to be calculated
    // @return velocity, v = (delta_row, delta_column) (pixel / ms)
    Point2d velocity(int row, int column);

private:
    // judge whether a pixel is on the border of the mask
    // DON'T FORGET TO MODIFY THIS METHOD IF YOU PASSED IN CV::MATS IN YOUR OWN FORMAT
    // @param which: first(0), second(1)
    // @param row, column: the cordinate (row, column) of pixel to be judged
    // @return true or false
    bool onBorder(int which, int row, int column);

    // get the starting pixel of the border of the mask
    // @param which: first(0), second(1)
    // @return cordinate (row, column) of the starting pixel
    Point2i getStartingPixel(int which);

    // calculate the number of origin's shape context block point end is in 
    // @param origin: the cordinate (row, column) of origin pixel
    // @param end: the cordinate (row, column) of the end pixel of the arc
    // @return the number of log-polar block, range [0, 60)
    int getBlock(const Point2i &origin, const Point2i &end);

    // calculate the radian (used for judging which block a pixel is in)
    // @param dx: end.x - origin.x
    // @param dy: end.y - origin.y
    // @return value of the radian, range [0, 2 * PI)
    // end, origin as defined in int getBlock(const Point2i &origin, const Point2i &end)
    double radian(int dx, int dy);

    // get the 2 nearest sampling points around given point (row, column)
    // @param which: first(0), second(1)
    // @param row, column: the cordinate (row, column) of pixel to be judged
    // @return the 2 nearest sampling points
    vector<Neighbor> getNeighbor(int which, int row, int column);

    // a debug funtion use cv::imwrite to output a 5 * 12 CV_8UC1 bmp
    // that represents the sampling points on the border of the mask
    // @param which: first(0), second(1)
    // @param fileName: the name of the bmp file, for example "debug0.bmp"
    void writeDebugSamplingPointGraph(int which, const char *fileName);

    // a debug funtion use cv::imwrite to output a 5 * 12 CV_8UC1 bmp
    // that represents the shape context of offset-th sampling point on the mask
    // @param which: first(0), second(1)
    // @param offset: the offset-th point on the mask, counting first in row then in column
    // @param fileName: the name of the bmp file, for example "debug0.bmp"
    void writeDebugShapeContextGraph(int which, int offset, const char *fileName);

    // a debug funtion that use cv::imwrite to output the matching graph
    // @param fileName: the name of the bmp file, for example "debug0.bmp"
    void writeDebugAssignmentGraph(const char *fileName);

    // a debug funtion that shows the sampling pixel matching result on console
    // shows the cordinate matching result
    void showDebugAssignmentResult();

    // get all border bixels and store them in the their order on the border curve
    // used for private method `void preprocessAllFrames()`
    // @param which: first(0), second(1)
    // @param totalAmount: total amount of pixels on the border
    // DO NOT CALL MANUALLY
    void getAllBorderPixels(int which, int totalAmount);

    // preprocess the frame to record which pixels are on the mask
    // used for constructor, need to set mask[0][0] and mask[1][0] before calling
    // DO NOT CALL MANUALLY
    void preprocessAllFrames();

    // get the shape contexts of all pixels on the border of the mask
    // used for constructor
    // DO NOT CALL MANUALLY
    void initAllShapeContexts();

    // calcaulate the matching cost matrix 
    // used for constructor
    // DO NOT CALL MANUALLY
    void calculateCostMatrix();

    // match the cost matrix using Hungrian Algorithm
    // used for constructor
    // DO NOT CALL MANUALLY
    void matchWithHungrianAlgorithm();

    // two cv::Mat represents the two frames
    Mat frame[2];

    // record of all pixels on the border of the mask, of the two frames
    vector<Point2i> mask[2];

    // record of sampling pixels selected from mask
    vector<Point2i> sample[2];

    // shape contexts for the two frames
    vector< vector<int> > shapeContext[2];

    // cost matrix
    vector< vector<double> > cost;

    // assignment of sampling pixels
    vector<int> assignment;
    vector<int> assignmentFrom1to0;

    // the actual time interval between the two frames
    double timeInterval;

    // the amount of sampling pixels
    int samplingNumber;

    // value of PI
    const double PI = 3.1415926;
};

// used for calculting the distance between pixels and recording
struct Neighbor
{
    Neighbor() = default;
    Neighbor(int o, double d) : offset(o), distance(d) {}
    int offset;
    double distance;

    bool operator <(const Neighbor &b)
    {
        return this->distance < b.distance;
    };
};