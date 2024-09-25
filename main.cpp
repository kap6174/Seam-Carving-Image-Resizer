#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace std;

// Function to load an image from a file
void loadImageFromFile(const string& path, cv::Mat& img) 
{
    img = cv::imread(path, cv::IMREAD_COLOR);
    
    if (img.empty()) 
    {
        cerr << "Error: Unable to load image from " << path << endl;
        exit(EXIT_FAILURE);
    } 
    else 
    {
        cout << "Successfully loaded image: " << path << endl;
    }
}

// Function to compute the energy at a specific pixel
int computePixelEnergy(const cv::Mat& img, int x, int y) 
{
    int gradX = 0, gradY = 0;

    for (int c = 0; c < 3; ++c) 
    {
        int leftPixel = img.at<cv::Vec3b>(y, max(x - 1, 0))[c];
        int rightPixel = img.at<cv::Vec3b>(y, min(x + 1, img.cols - 1))[c];
        int topPixel = img.at<cv::Vec3b>(max(y - 1, 0), x)[c];
        int bottomPixel = img.at<cv::Vec3b>(min(y + 1, img.rows - 1), x)[c];

        gradX += pow(rightPixel - leftPixel, 2);
        gradY += pow(bottomPixel - topPixel, 2);
    }

    return static_cast<int>(sqrt(gradX + gradY));
}

// Function to create an energy matrix from an image
cv::Mat generateEnergyMatrix(const cv::Mat& img) 
{
    cv::Mat energyMat(img.rows, img.cols, CV_32S);

    for (int y = 0; y < img.rows; ++y) 
    {
        for (int x = 0; x < img.cols; ++x) 
        {
            energyMat.at<int>(y, x) = computePixelEnergy(img, x, y);
        }
    }

    return energyMat;
}

// Function to find the vertical seam with the minimum energy
void computeVerticalSeam(const cv::Mat& energyMat, int* seam) 
{
    int rows = energyMat.rows;
    int cols = energyMat.cols;

    vector<vector<int>> cost(rows, vector<int>(cols, 0));

    for (int x = 0; x < cols; ++x) 
    {
        cost[0][x] = energyMat.at<int>(0, x);
    }

    for (int y = 1; y < rows; ++y) 
    {
        for (int x = 0; x < cols; ++x) 
        {
            int minCost = cost[y-1][x];

            if (x > 0) 
            {
                minCost = min(minCost, cost[y-1][x-1]);
            }

            if (x < cols - 1) 
            {
                minCost = min(minCost, cost[y-1][x+1]);
            }

            cost[y][x] = energyMat.at<int>(y, x) + minCost;
        }
    }

    int minCol = 0;
    for (int x = 1; x < cols; ++x) 
    {
        if (cost[rows-1][x] < cost[rows-1][minCol]) 
        {
            minCol = x;
        }
    }

    seam[rows-1] = minCol;
    for (int y = rows - 2; y >= 0; --y) 
    {
        minCol = seam[y+1];

        if (minCol > 0 && cost[y][minCol-1] < cost[y][minCol]) 
        {
            minCol--;
        }

        if (minCol < cols - 1 && cost[y][minCol+1] < cost[y][minCol]) 
        {
            minCol++;
        }

        seam[y] = minCol;
    }
}

// Function to remove a vertical seam from an image
void deleteVerticalSeam(cv::Mat& img, const int* seam) 
{
    int rows = img.rows;
    int cols = img.cols;

    cv::Mat reducedImg(rows, cols - 1, CV_8UC3);

    for (int y = 0; y < rows; ++y) 
    {
        int seamCol = seam[y];
        
        for (int x = 0; x < seamCol; ++x) 
        {
            reducedImg.at<cv::Vec3b>(y, x) = img.at<cv::Vec3b>(y, x);
        }
        for (int x = seamCol; x < cols - 1; ++x) 
        {
            reducedImg.at<cv::Vec3b>(y, x) = img.at<cv::Vec3b>(y, x + 1);
        }
    }

    img = reducedImg;
}

// Function to find the horizontal seam with the minimum energy
void computeHorizontalSeam(const cv::Mat& energyMat, int* seam) 
{
    int rows = energyMat.rows;
    int cols = energyMat.cols;

    vector<vector<int>> cost(rows, vector<int>(cols, 0));

    for (int y = 0; y < rows; ++y) 
    {
        cost[y][0] = energyMat.at<int>(y, 0);
    }

    for (int x = 1; x < cols; ++x) 
    {
        for (int y = 0; y < rows; ++y) 
        {
            int minCost = cost[y][x-1];

            if (y > 0) 
            {
                minCost = min(minCost, cost[y-1][x-1]);
            }

            if (y < rows - 1) 
            {
                minCost = min(minCost, cost[y+1][x-1]);
            }

            cost[y][x] = energyMat.at<int>(y, x) + minCost;
        }
    }

    int minRow = 0;
    for (int y = 1; y < rows; ++y) 
    {
        if (cost[y][cols-1] < cost[minRow][cols-1]) 
        {
            minRow = y;
        }
    }

    seam[cols-1] = minRow;
    for (int x = cols - 2; x >= 0; --x) 
    {
        minRow = seam[x+1];

        if (minRow > 0 && cost[minRow-1][x] < cost[minRow][x]) 
        {
            minRow--;
        }

        if (minRow < rows - 1 && cost[minRow+1][x] < cost[minRow][x]) 
        {
            minRow++;
        }

        seam[x] = minRow;
    }
}

// Function to remove a horizontal seam from an image
void deleteHorizontalSeam(cv::Mat& img, const int* seam) 
{
    int rows = img.rows;
    int cols = img.cols;

    cv::Mat reducedImg(rows - 1, cols, CV_8UC3);

    for (int x = 0; x < cols; ++x) 
    {
        int seamRow = seam[x];
        
        for (int y = 0; y < seamRow; ++y) 
        {
            reducedImg.at<cv::Vec3b>(y, x) = img.at<cv::Vec3b>(y, x);
        }
        for (int y = seamRow; y < rows - 1; ++y) 
        {
            reducedImg.at<cv::Vec3b>(y, x) = img.at<cv::Vec3b>(y + 1, x);
        }
    }

    img = reducedImg;
}

int main() 
{
    string imagePath;
    cout << "Enter the image file path: ";
    cin >> imagePath;
    cv::Mat img;
    loadImageFromFile(imagePath, img);

    int targetWidth, targetHeight;
    cout << "Enter the target width: ";
    cin >> targetWidth;
    cout << "Enter the target height: ";
    cin >> targetHeight;

    // Remove vertical seams until the width matches targetWidth
    while (img.cols > targetWidth) 
    {
        cv::Mat energyMat = generateEnergyMatrix(img);

        int* verticalSeam = new int[img.rows];
        computeVerticalSeam(energyMat, verticalSeam);

        cv::Mat animatedImage = img.clone();
        for (int y = 0; y < img.rows; ++y) 
        {
            int seamCol = verticalSeam[y];
            cv::circle(animatedImage, cv::Point(seamCol, y), 1, cv::Scalar(0, 255, 0), -1); // Green color
        }
        
        cv::imshow("Seam Carving Animation", animatedImage);
        cv::waitKey(50); // Pause for 50 milliseconds

        deleteVerticalSeam(img, verticalSeam);
        delete[] verticalSeam;
    }

    // Remove horizontal seams until the height matches targetHeight
    while (img.rows > targetHeight) 
    {
        cv::Mat energyMat = generateEnergyMatrix(img);

        int* horizontalSeam = new int[img.cols];
        computeHorizontalSeam(energyMat, horizontalSeam);

        cv::Mat animatedImage = img.clone();
        for (int x = 0; x < img.cols; ++x) 
        {
            int seamRow = horizontalSeam[x];
            cv::circle(animatedImage, cv::Point(x, seamRow), 1, cv::Scalar(0, 255, 0), -1); // Green color
        }

        cv::imshow("Seam Carving Animation", animatedImage);
        cv::waitKey(50); // Pause for 50 milliseconds

        deleteHorizontalSeam(img, horizontalSeam);
        delete[] horizontalSeam;
    }

    cv::imwrite("output_resized_image.jpg", img);
    cout << "Resized image saved as output_resized_image.jpg" << endl;

    return 0;
}
