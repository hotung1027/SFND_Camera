#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;

void magnitudeSobel() {
  // load image from file
  cv::Mat img;
  img = cv::imread("../images/img1gray.png");

  float sobel_x[9] = {-1, 0, +1, -2, 0, +2, -1, 0, +1};
  float sobel_y[9] = {-1, +2, +1, 0, 0, 0, -1, +2, +1};

  // convert image to grayscale
  cv::Mat imgGray;
  cv::Mat imgBlur;
  cv::Mat magnitude;
  cv::Mat sobel_x_mat;
  cv::Mat sobel_y_mat;
  cv::Mat sobel_mag;
  cv::Mat sobel_ang;
  cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

  // apply smoothing using the GaussianBlur() function from the OpenCV
  // ToDo : Add your code here
  cv::Size blur_size(5, 5);
  cv::GaussianBlur(imgGray, imgBlur, blur_size, 1);

  // create filter kernels using the cv::Mat datatype both for x and y
  // ToDo : Add your code here
  cv::Mat sobel_x_kernel = cv::Mat(3, 3, CV_32F, sobel_x);
  cv::Mat sobel_y_kernel = cv::Mat(3, 3, CV_32F, sobel_y);

  // apply filter using the OpenCv function filter2D()
  // ToDo : Add your code here
  cv::filter2D(imgBlur, sobel_x_mat, -1, sobel_x_kernel, cv::Point(-1, -1), 0,
               cv::BORDER_DEFAULT);

  cv::filter2D(imgBlur, sobel_y_mat, -1, sobel_y_kernel, cv::Point(-1, -1), 0,
               cv::BORDER_DEFAULT);

  // compute magnitude image based on the equation presented in the lesson
  // ToDo : Add your code here
  cv::convertScaleAbs(sobel_x_mat, sobel_x_mat);
  cv::convertScaleAbs(sobel_y_mat, sobel_y_mat);

  cv::addWeighted(sobel_x_mat, 0.5, sobel_y_mat, 0.5, 0, magnitude);
  string windowName = "Gaussian Blurring";
  cv::namedWindow(windowName, 1); // create window
  cv::imshow(windowName, magnitude);

  sobel_y_mat.convertTo(sobel_y_mat, CV_32F);
  sobel_x_mat.convertTo(sobel_x_mat, CV_32F);

  cv::phase(sobel_x_mat, sobel_y_mat, sobel_ang);

  cv::pow(sobel_x_mat, 2, sobel_x_mat);
  cv::pow(sobel_y_mat, 2, sobel_y_mat);
  cv::sqrt(sobel_x_mat + sobel_y_mat, sobel_mag);

  sobel_mag.convertTo(sobel_mag, CV_8U);
  sobel_ang.convertTo(sobel_ang, CV_8U);

  cv::multiply(sobel_mag, sobel_ang, magnitude, 0.5);

  // cv::cvtColor(sobel_mag, sobel_mag, cv::COLOR_GRAY2BGR);
  // cv::cvtColor(sobel_ang, sobel_ang, cv::COLOR_GRAY2BGR);

  string sobel_mag_win = "Sobel Magnitude";
  cv::namedWindow(sobel_mag_win, 1); // create window
  cv::imshow(sobel_mag_win, sobel_mag * 0.5);

  string sobel_ang_win = "Sobel Angle";
  cv::namedWindow(sobel_ang_win, 1); // create window
  cv::imshow(sobel_ang_win, sobel_ang);

  string sobel = "Sobel";
  cv::namedWindow(sobel, 1); // create window
  cv::imshow(sobel, magnitude);

  // show result
  cv::waitKey(0); // wait for keyboard input before continuing
}

int main() { magnitudeSobel(); }
