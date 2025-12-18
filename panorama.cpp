#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <vector>
using namespace cv;
using namespace std;

//SIFT特征检测与匹配
void detectAndMatchSIFT(const Mat& img1, const Mat& img2, vector<Point2f>& pts1, vector<Point2f>& pts2) {
    Ptr<SIFT> sift = SIFT::create();
    vector<KeyPoint> kp1, kp2; Mat desc1, desc2;
    sift->detectAndCompute(img1, noArray(), kp1, desc1);
    sift->detectAndCompute(img2, noArray(), kp2, desc2);
    FlannBasedMatcher matcher;
    vector<vector<DMatch>> knnMatches;
    matcher.knnMatch(desc2, desc1, knnMatches, 2);
    const float ratioThresh = 0.75f;
    for (auto& m : knnMatches) {
        if (m[0].distance < ratioThresh* m[1].distance) {
            pts2.push_back(kp2[m[0].queryIdx].pt);
            pts1.push_back(kp1[m[0].trainIdx].pt);
        }
    }
}

//自动裁剪黑边
Rect boundingBoxNonZero(const Mat& img) {
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    Mat mask = gray > 0;
    vector<Point> pts;
    findNonZero(mask, pts);
    return boundingRect(pts);
}

//简单融合拼接（消除黑线）
Mat stitchPair(const Mat& img1, const Mat& img2) {
    Mat gray1, gray2;
    cvtColor(img1, gray1, COLOR_BGR2GRAY);
    cvtColor(img2, gray2, COLOR_BGR2GRAY);
    vector<Point2f> pts1, pts2;
    detectAndMatchSIFT(gray1, gray2, pts1, pts2);
    Mat H = findHomography(pts2, pts1, RANSAC);

    //创建画布并扭曲图像2
    Mat warped;
    int width = img1.cols + img2.cols;
    warpPerspective(img2, warped, H, Size(width, img1.rows));

    //简单max融合（解决黑线问题）
    Mat result = warped.clone();
    for (int y = 0; y < img1.rows; ++y) {
        for (int x = 0; x < img1.cols; ++x) {
            Vec3b p1 = img1.at<Vec3b>(y, x);
            Vec3b p2 = warped.at<Vec3b>(y, x);
            if (p2 == Vec3b(0, 0, 0)) result.at<Vec3b>(y, x) = p1;
            else for (int c = 0; c < 3; ++c)
                result.at<Vec3b>(y, x)[c] = max(p1[c], p2[c]);
        }
    }

    //裁剪黑边
    Rect crop = boundingBoxNonZero(result);
    return result(crop);
}

int main() {
    //读取 S1.jpg 到 S7.jpg
    vector<string> paths;
    for (int i = 1; i <= 7; i++) paths.push_back("S" + to_string(i) + ".jpg");
   
    //读取 D1.jpg 到 D8.jpg
    //vector<string> paths;
    //for (int i = 1; i <= 8; i++) paths.push_back("D" + to_string(i) + ".jpg");

    //读取 P1.jpg 到 P5.jpg
    //vector<string> paths;
    //for (int i = 1; i <= 5; i++) paths.push_back("P" + to_string(i) + ".jpg");

    vector<Mat> imgs;
    for (auto& p : paths) {
        Mat im = imread(p);
        if (im.empty()) { cerr << "无法加载:" << p << endl; return -1; }
        imgs.push_back(im);
    }
    //连续拼接
    Mat pano = imgs[0];
    for (size_t i = 1; i < imgs.size(); i++) pano = stitchPair(pano, imgs[i]);

    //显示与保存
    namedWindow("Panorama", WINDOW_NORMAL);
    imshow("Panorama", pano);
    imwrite("panorama_cropped.jpg", pano);
    waitKey(0);
    return 0;
}