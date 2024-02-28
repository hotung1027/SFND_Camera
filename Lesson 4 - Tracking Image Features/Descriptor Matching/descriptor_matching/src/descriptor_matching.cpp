#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "structIO.hpp"

using namespace std;

void matchDescriptors(cv::Mat &imgSource, cv::Mat &imgRef,
                      vector<cv::KeyPoint> &kPtsSource,
                      vector<cv::KeyPoint> &kPtsRef, cv::Mat &descSource,
                      cv::Mat &descRef, vector<cv::DMatch> &matches,
                      string descriptorType, string matcherType,
                      string selectorType) {

  // configure matcher
  bool crossCheck = true;
  cv::Ptr<cv::DescriptorMatcher> matcher;

  // if (matcherType.compare("MAT_BF") == 0) {
  // Brute Force Matcher
  int normType = descriptorType.compare("DES_BINARY") == 0 ? cv::NORM_HAMMING
                                                           : cv::NORM_L2;

  matcher = cv::BFMatcher::create(normType, crossCheck);
  double t = (double)cv::getTickCount();
  matcher->match(descSource, descRef, matches);

  t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
  cout << " (BF) with n=" << matches.size() << " matches in " << 1000 * t / 1.0
       << " ms" << endl;

  cv::Mat BFMatchImg = imgRef.clone();
  cv::drawMatches(imgSource, kPtsSource, imgRef, kPtsRef, matches, BFMatchImg,
                  cv::Scalar::all(-1), cv::Scalar::all(-1), vector<char>(),
                  cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
  string windowName = "Matching keypoints using Brute Force Matcher";
  cv::namedWindow(windowName, 7);
  cv::imshow(windowName, BFMatchImg);
  cv::waitKey(0);

  cout << "BF matching cross-check=" << crossCheck << endl;

  // } else if (matcherType.compare("MAT_FLANN") == 0) {
  // FLANN Based Matcher
  if (descSource.type() != CV_32F) { // OpenCV bug workaround : convert binary
                                     // descriptors to floating point due to a
                                     // bug in current OpenCV implementation
    descSource.convertTo(descSource, CV_32F);
    descRef.convertTo(descRef, CV_32F);
  }

  matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
  t = (double)cv::getTickCount();

  matcher->match(descSource, descRef, matches);

  t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
  cout << " (FLANN) with n=" << matches.size() << " matches in "
       << 1000 * t / 1.0 << " ms" << endl;
  //... TODO : implement FLANN matching
  cout << "FLANN matching" << endl;
  cv::Mat FLANNMatchImg = imgRef.clone();

  cv::drawMatches(imgSource, kPtsSource, imgRef, kPtsRef, matches,
                  FLANNMatchImg, cv::Scalar::all(-1), cv::Scalar::all(-1),
                  vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
  windowName = "Matching keypoints using FLANN Matcher";
  cv::namedWindow(windowName, 7);
  cv::imshow(windowName, FLANNMatchImg);
  cv::waitKey(0);
  // }

  // perform matching task
  if (selectorType.compare("SEL_NN") == 0) { // nearest neighbor (best match)

    //
    // } else if (selectorType.compare("SEL_NN") == 0) {
    // k nearest neighbors (k=2)
    //
    matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);

    vector<vector<cv::DMatch>> matchesFlann;

    double t = (double)cv::getTickCount();
    matcher->knnMatch(descSource, descRef, matchesFlann, 2);
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

    // TODO : implement k-nearest-neighbor matching
    std::vector<std::vector<cv::DMatch>> goodMatches;
    const float RATIO_THRESHOLD = 0.8f;
    // TODO : filter matches using descriptor distance ratio test
    for (auto pair : matchesFlann) {
      if (pair[0].distance < RATIO_THRESHOLD * pair[1].distance) {
        goodMatches.push_back(pair);
      }
    }
    cout << " (KNN) with n=" << goodMatches.size() << " matches in "
         << 1000 * t / 1.0 << " ms" << endl;

    cv::Mat matchImg = imgRef.clone();
    cv::drawMatches(imgSource, kPtsSource, imgRef, kPtsRef, goodMatches,
                    matchImg, cv::Scalar::all(-1), cv::Scalar::all(-1),
                    vector<vector<char>>(),
                    cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    string windowName =
        "Matching keypoints between two camera images (best 50)";
    cv::namedWindow(windowName, 7);
    cv::imshow(windowName, matchImg);
    cv::waitKey(0);
  }
  // cv::Mat matchImg = imgRef.clone();
  // cv::drawMatches(imgSource, kPtsSource, imgRef, kPtsRef, matches, matchImg,
  //                 cv::Scalar::all(-1), cv::Scalar::all(-1), vector<char>(),
  //                 cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
  //
  // string windowName = "Matching keypoints between two camera images (best
  // 50)"; cv::namedWindow(windowName, 7); cv::imshow(windowName, matchImg);
  // cv::waitKey(0);
}

// visualize results

int main() {
  cv::Mat imgSource = cv::imread("../images/img1gray.png");
  cv::Mat imgRef = cv::imread("../images/img2gray.png");

  vector<cv::KeyPoint> kptsSource, kptsRef;
  readKeypoints("../dat/C35A5_KptsSource_BRISK_large.dat", kptsSource);
  readKeypoints("../dat/C35A5_KptsRef_BRISK_large.dat", kptsRef);

  cv::Mat descSource, descRef;
  readDescriptors("../dat/C35A5_DescSource_BRISK_large.dat", descSource);
  readDescriptors("../dat/C35A5_DescRef_BRISK_large.dat", descRef);

  vector<cv::DMatch> matches;
  string matcherType = "MAT_BF";
  string descriptorType = "DES_BINARY";
  string selectorType = "SEL_NN";
  matchDescriptors(imgSource, imgRef, kptsSource, kptsRef, descSource, descRef,
                   matches, descriptorType, matcherType, selectorType);
}
