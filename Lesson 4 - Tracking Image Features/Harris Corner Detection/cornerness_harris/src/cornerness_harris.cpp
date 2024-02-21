#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

using namespace std;

// Non Max Suppression
void nms(cv::Mat &src, int minResponse, vector<cv::KeyPoint> &keypoints,
         int radius) {

  double maxOverlap = 0.0;
  for (size_t j = 0; j < src.rows; j++) {
    for (size_t i = 0; i < src.cols; i++) {

      int response = (int)src.at<float>(j, i);

      if (response > minResponse) { // only store points above the threshold
        cv::KeyPoint newKeyPoint;
        newKeyPoint.pt = cv::Point2f(i, j);
        newKeyPoint.size = 2 * radius;
        newKeyPoint.response = response;

        // Perform non-max suppression over local neighbors
        bool detectNonMax = false;
        for (auto pt = keypoints.begin(); pt != keypoints.end();
             ++pt) { // iterate over existing keypoints
          double overlap = cv::KeyPoint::overlap(
              newKeyPoint, *pt); // check if new keypoints match existing
                                 // keypoints by overlapping local regions
          if (overlap > maxOverlap) {
            detectNonMax = true; // found overlapping keypoints
            if (newKeyPoint.response >
                (*pt).response) {  // walk through existing
                                   // keypoints local maximum
              (*pt) = newKeyPoint; // replace existing keypoint with new one
              break;
            }
          }
        }
        if (!detectNonMax) // only store new keypoints if no overlap has
                           // been found
        {
          keypoints.push_back(newKeyPoint); // store the new keypoint
        }
      }
    }
  }
}

void cornernessHarris() {
  // load image from file
  cv::Mat img;
  img = cv::imread("../images/img1.png");
  cv::cvtColor(img, img, cv::COLOR_BGR2GRAY); // convert to grayscale

  // Detector parameters
  int blockSize =
      2; // for every pixel, a blockSize × blockSize neighborhood is considered
  int apertureSize = 3; // aperture parameter for Sobel operator (must be odd)
  int minResponse =
      100; // minimum value for a corner in the 8bit scaled response matrix
  double k = 0.04; // Harris parameter (see equation for details)

  // Detect Harris corners and normalize output
  cv::Mat dst, dst_norm, dst_norm_scaled;
  dst = cv::Mat::zeros(img.size(), CV_32FC1);
  cv::cornerHarris(img, dst, blockSize, apertureSize, k, cv::BORDER_DEFAULT);
  cv::normalize(dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
  cv::convertScaleAbs(dst_norm, dst_norm_scaled);

  // visualize results
  string windowName = "Harris Corner Detector Response Matrix";
  cv::namedWindow(windowName, 4);
  cv::imshow(windowName, dst_norm_scaled);
  cv::waitKey(0);

  // TODO: Your task is to locate local maxima in the Harris response matrix
  // and perform a non-maximum suppression (NMS) in a local neighborhood around
  // each maximum. The resulting coordinates shall be stored in a list of
  // keypoints of the type `vector<cv::KeyPoint>`.
  vector<cv::KeyPoint> keypoints;

  // Non-maximum suppression window
  nms(dst_norm, minResponse, keypoints, apertureSize);

  windowName = "Harris Corner Detection Results";
  cv::namedWindow(windowName, 5);
  cv::Mat visImage = dst_norm_scaled.clone();
  cv::drawKeypoints(dst_norm_scaled, keypoints, visImage, cv::Scalar::all(-1),
                    cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
  cv::imshow(windowName, visImage);
  cv::waitKey(0);
}

int main() { cornernessHarris(); }
