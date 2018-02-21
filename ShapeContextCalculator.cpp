#include "ShapeContextCalculator.h"
#include "Hungarian.h"

ShapeContextCalculator::ShapeContextCalculator()
{

}

ShapeContextCalculator::ShapeContextCalculator(const Mat &frame0, const Mat &frame1, double timeInterval, int samplingNumber)
{
    this->frame[0] = frame0;
    this->frame[1] = frame1;
    this->timeInterval = timeInterval;
    this->samplingNumber = samplingNumber;

    preprocessAllFrames();
    initAllShapeContexts();
    calculateCostMatrix();
    matchWithHungrianAlgorithm();
}

ShapeContextCalculator::~ShapeContextCalculator()
{

}

Point2d ShapeContextCalculator::velocity(int row, int column)
{
    vector<Neighbor> neighbor = getNeighbor(1, row, column);

    //printf("n0 %d\nn1 %d\n", neighbor[0].offset, neighbor[1].offset);///
    //printf("n0 (%d, %d)\nn1(%d, %d)\n", sample[1][neighbor[0].offset].x, sample[1][neighbor[0].offset].y, sample[1][neighbor[1].offset].x, sample[1][neighbor[1].offset].y);///

    Point2d deltaNeighbor0 = sample[1][neighbor[0].offset] - sample[0][assignmentFrom1to0[neighbor[0].offset]];
    Point2d deltaNeighbor1 = sample[1][neighbor[1].offset] - sample[0][assignmentFrom1to0[neighbor[1].offset]];
    
    //printf("d0 (%lf, %lf)\nd1(%lf, %lf)\n", deltaNeighbor0.x, deltaNeighbor0.y, deltaNeighbor1.x, deltaNeighbor1.y);

    double sum = neighbor[0].distance + neighbor[1].distance;
    
    Point2d deltaPoint = deltaNeighbor0 * (neighbor[0].distance / sum) + deltaNeighbor1 * (neighbor[1].distance / sum);

    return Point2d(deltaPoint.x / timeInterval, deltaPoint.y / timeInterval);
}

bool ShapeContextCalculator::onBorder(int which, int row, int column)
{
    // TODO modify this method if you passed in cv::mat in your prefered format
    assert(0 <= row && row < frame[which].rows);
    assert(0 <= column && column < frame[which].cols);
    return frame[which].at<uchar>(row, column) == 0;  // in this case, border has already been marked black
}

Point2i ShapeContextCalculator::getStartingPixel(int which)
{
    int rows = frame[which].rows;
    int cols = frame[which].cols;
    
    if (rows >= cols)  // the frames are vertical
    {
        for (int c = 0; c < cols; c++)
        {
            for (int r = 0; r < rows; r++)
            {
                if (onBorder(which, r, c))
                {
                    return Point2i(r, c);
                }
            }
        }
    }
    else  // the frames are horizontal
    {
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                if (onBorder(which, r, c))
                {
                    return Point2i(r, c);
                }
            }
        }
    }

    // when reaches here the input frame should be a corrupted one
    // for nothing is on the border of the mask
    return Point2i(-1, -1);
}

vector<Neighbor> ShapeContextCalculator::getNeighbor(int which, int row, int column)
{  
    vector<Neighbor> neighbors;

    int size = sample[1].size();

    for (int i = 0; i < size; i++)
    {
        const Point2i &sp = sample[1][i];
        neighbors.emplace_back(Neighbor(i, sqrt((sp.x - row) * (sp.x - row) + (sp.y - column) * (sp.y - column))));
    }

    sort(neighbors.begin(), neighbors.end());

    //for (const Neighbor & n : neighbors)
    //{
    //    printf("sample[1][%d] distance = %lf\n", n.offset, n.distance);
    //}

    vector<Neighbor> result;
    result.emplace_back(neighbors[0]);
    result.emplace_back(neighbors[1]);
    return result;
}

double ShapeContextCalculator::radian(int dx, int dy)
{
    double result;

    if (dx > 0)
    {
        result = atan(dy / dx);
    }
    else if (dx == 0)
    {
        result = (dy > 0) ? (0.5 * PI) : (1.5 * PI);
    }
    else
    {
        result = PI - atan(dy / (-dx));
    }

    if (result < 0)
    {
        return 2 * PI + result;
    }
    else if (result == 2 * PI)
    {
        return 0;
    }
    else
    {
        return result;
    }
}

void ShapeContextCalculator::writeDebugSamplingPointGraph(int which, const char *fileName)
{
    Mat sampleGraph;
    cvtColor(frame[which], sampleGraph, CV_GRAY2BGR, 0);
    for (const Point2i &sp : sample[which])
    {
        for (int r = sp.x - 1; r <= sp.x + 1; r++)
        {
            for (int c = sp.y - 1; c <= sp.y + 1; c++)
            {
                if (0 <= r && r < frame[which].rows && 0 <= c && c < frame[which].cols)
                {
                    sampleGraph.at<Vec3b>(r, c) = Vec3b(0, 0, 255);
                }
            }
        }       
    }
    imwrite(fileName, sampleGraph);
    sampleGraph.release();
}

void ShapeContextCalculator::writeDebugShapeContextGraph(int which, int offset, const char *fileName)
{
    uchar buf[5 * 12];
    for (int kk = 0; kk < 5 * 12; kk++)
    {
        buf[kk] = 255 - (double)shapeContext[which][offset][kk] / (double)sample[which].size() * 255;
    }
    Mat image = Mat(5, 12, CV_8UC1, buf);
    Mat image40 = Mat(200, 480, CV_8UC1);

    for (int r = 0; r < 5; r++)
    {
        for (int c = 0; c < 12; c++)
        {
            for (int rr = r * 40; rr < r * 40 + 40; rr++)
            {
                for (int cc = c * 40; cc < c * 40 + 40; cc++)
                {
                    image40.at<uchar>(rr, cc) = image.at<uchar>(r, c);
                }
            }     
        }
    }

    imwrite(fileName, image40);
    image.release();
    image40.release();
}

void ShapeContextCalculator::writeDebugAssignmentGraph(const char *fileName)
{
    Mat sampleGraph = Mat(frame[0].size(), CV_8UC3, Scalar(255, 255, 255));

    for (int which = 0; which < 2; which++)
    {
        for (const Point2i &sp : sample[which])
        {
            for (int r = sp.x - 2; r <= sp.x + 2; r++)
            {
                for (int c = sp.y - 2; c <= sp.y + 2; c++)
                {
                    if (0 <= r && r < frame[which].rows && 0 <= c && c < frame[which].cols)
                    {
                        sampleGraph.at<Vec3b>(r, c) = which ? Vec3b(0, 0, 255) : Vec3b(255, 0, 0);
                    }
                }
            }       
        }
    }

    for (int i = 0; i < assignment.size(); i++)
    {
        arrowedLine(sampleGraph, Point2i(sample[0][i].y, sample[0][i].x), Point2i(sample[1][assignment[i]].y, sample[1][assignment[i]].x), Scalar(0, 0, 0));
    }

    imwrite(fileName, sampleGraph);
    sampleGraph.release();
}

void ShapeContextCalculator::showDebugAssignmentResult()
{
    for (int i = 0; i < assignment.size(); i++)
    {
        printf("[0](% 5d, % 5d) -> [1](% 5d, % 5d)\n", sample[0][i].x, sample[0][i].y, sample[1][assignment[i]].x, sample[1][assignment[i]].y);
    }
}

int ShapeContextCalculator::getBlock(const Point2i &origin, const Point2i &end)
{
    double dx = end.x - origin.x;
    double dy = end.y - origin.y;

    double radius = log2(sqrt(dx * dx + dy * dy));
    double theta = radian(dx, dy);

    int angularOffset = theta / PI * 12;
    int radialOffset = 0;

    // (polar) radial offset
    // radius  [0, 0.5)  [0.5, 1.5)  [1.5, 3.5)  [3.5, 7.5)  [7.5, 15.5)  [15.5, ...)
    // offset   0         1           2           3           4            5
    
    if (radius < 0.5)
        radialOffset = 0;
    else if (0.5 <= radius && radius < 1.5)
        radialOffset = 1;
    else if (1.5 <= radius && radius < 3.5)
        radialOffset = 2;
    else if (3.5 <= radius && radius < 7.5)
        radialOffset = 3;
    else if (7.5 <= radius && radius < 15.5)
        radialOffset = 4;
    else
        radialOffset = 5;

    return radialOffset * 12 + angularOffset;
}

void ShapeContextCalculator::getAllBorderPixels(int which, int totalAmount)
{
    // (x1, y1): cordiante of the point whose succssor is to be found
    int x1 = mask[which][0].x;
    int y1 = mask[which][0].y;

    // record whether a pixel is already in the border,
    // to avoid repeated pixels in sample set
    int rows = frame[which].rows;
    int cols = frame[which].cols;
    bool *known = new bool[rows * cols];
    memset(known, 0, sizeof(bool) * rows * cols);
    known[x1 * cols + y1] = true;

    while (totalAmount--)
    {
        // given that we divide the mask into two parts: 
        // one part whose condidence > 0.5 while the other <= 0.5,
        // we will get a continuous seam line with width 1.
        // then it's not hard to find the succssor-on-border of a pixel, i.e.
        // given a pixel, the succ shoule be in 
        // { 
        //     (x1 - 1, y1 + 1), (x1, y1 + 1), (x1 - 1, y1 + 1),
        //     (x1 - 1, y1),                 , (x1 + 1, y1),
        //     (x1 - 1, y1 - 1), (x1, y1 - 1), (x1 - 1, y1 - 1)
        // } - (x0, y0)
            
        bool succFoundFlag = false;

        for (int r = x1 - 1; r <= x1 + 1; r++)
        {
            for (int c = y1 - 1; c <= y1 + 1; c++)
            {  
                if (!(0 <= r && r < rows && 0 <= c && c < cols))
                {
                    continue;
                }
                
                if (known[r * cols + c])
                {                   
                    continue;
                }   

                if (onBorder(which, r, c))
                {                  
                    x1 = r, y1 = c;
                    succFoundFlag = true;
                    break;
                }
            }

            if (succFoundFlag)
                break;
        }

        mask[which].emplace_back(Point2i(x1, y1));
        known[x1 * cols + y1] = true;
    }

    delete [] known;
}

void ShapeContextCalculator::preprocessAllFrames()
{
    for (int which = 0; which < 2; which++)
    {
        int rows = frame[which].rows;
        int cols = frame[which].cols;

        // get the total amount of pixels on the border
        // and the interval of sampling points
        int totalBorderPixelAmount = 0;

        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                if (onBorder(which, r, c))
                {
                    totalBorderPixelAmount++;
                }
            }
        }

        int interval = totalBorderPixelAmount / samplingNumber;

        // get the starting pixel
        mask[which].emplace_back(getStartingPixel(which));

        // get all other pixels on the border
        getAllBorderPixels(which, totalBorderPixelAmount);

        // get all sampling pixels
        for (int offset = 0; offset < totalBorderPixelAmount; offset += interval)
        {
            sample[which].emplace_back(mask[which][offset]);
        }
    }
}

void ShapeContextCalculator::initAllShapeContexts()
{ 
    for (int which = 0; which < 2; which++)
    {
        int size = sample[which].size();

        for (int i = 0; i < size; i++)
        {
            const Point2i &origin = sample[which][i];

            // consider all other pixels and judge their location 
            // in the shape context circle of pixel origin
            vector<int> _sc_;
            _sc_.resize(5 * 12, 0);
            
            for (int j = 0; j < size; j++)
            {
                if (j == i)
                    continue;
                
                const Point2i &end = sample[which][j];

                int blockOffset = getBlock(origin, end);

                // add to shape context only when end lies inside the context circle of origin
                if (blockOffset < 60)
                {
                    _sc_[blockOffset]++;
                }
            }

            shapeContext[which].emplace_back(_sc_);
        }
    }
}

void ShapeContextCalculator::calculateCostMatrix()
{
    int rows = sample[0].size();
    int cols = sample[1].size();

    // init the cost mat
    cost.resize(rows);

    for (int r = 0; r < rows; r++)
    {
        cost[r].resize(cols, 0);
    }
    
    // calculate all elements
    for (int i = 0; i < rows; i++)
    {   
        for (int j = 0; j < cols; j++)
        {
            for (int k = 0; k < 60; k++)
            {
                double numerator = (shapeContext[0][i][k] - shapeContext[1][j][k]) * (shapeContext[0][i][k] - shapeContext[1][j][k]);
                double denominator = shapeContext[0][i][k] + shapeContext[1][j][k];
                if (denominator == 0)  denominator = 1;

                cost[i][j] += numerator / denominator;
            }

            cost[i][j] *= 0.5;
        }
    }
}

void ShapeContextCalculator::matchWithHungrianAlgorithm()
{
    HungarianAlgorithm ha = HungarianAlgorithm();
    vector< vector<double> > distMatrix = cost;
    ha.Solve(distMatrix, assignment);

    int size = assignment.size();
    assignmentFrom1to0.resize(size);

    for (int i = 0; i < size; i++)
    {
        assignmentFrom1to0[assignment[i]] = i;
    }
}
