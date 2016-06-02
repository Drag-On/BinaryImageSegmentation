#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <cv.hpp>
#include "MoGDistribution.h"
#include "GraphCut/BinaryGraphCutOptimizer.h"

using Site = BinaryGraphCutOptimizer::Site;
using Label = BinaryGraphCutOptimizer::Label;
using Energy = BinaryGraphCutOptimizer::Energy;

int main()
{
    auto img = cv::imread("banana3.png", cv::IMREAD_COLOR);
    img.convertTo(img, CV_64FC3);
    img /= 255;

    /*
     * We use mixture-of-gaussian distributions to classify pixels as foreground or background. Here the
     * distributions are read from file, if present, or otherwise computed from the image.
     */

    cv::Rect bbForeground(200, 280, 270, 100);
    cv::Rect bbBackground(200, 50, 270, 100);

    using Distribution = MoGDistribution<5, 3>;
    auto makeDataPoint = [](cv::Vec3d const& d)
    {
        Distribution::Data data;
        data(0) = d[0];
        data(1) = d[1];
        data(2) = d[2];
        return data;
    };

    // Data for foreground
    std::vector<Distribution::Data> fgData(bbForeground.area());
    for (unsigned int y = 0; y < bbForeground.height; y++)
    {
        for (unsigned int x = 0; x < bbForeground.width; x++)
        {
            auto color = img.at<cv::Vec3d>(y + bbForeground.y, x + bbForeground.x);
            fgData[x + y * bbForeground.width] = makeDataPoint(color);
        }
    }
    // Train foreground
    Distribution foreground;
    if (!foreground.load("foreground.gmm"))
    {
        foreground.train(fgData.begin(), fgData.end(), 5e2);
        foreground.save("foreground.gmm");
    }
    std::cout << "=== Foreground ===" << std::endl;
    foreground.print();

    // Data for background
    std::vector<Distribution::Data> bgData(bbBackground.area());
    for (unsigned int y = 0; y < bbBackground.height; y++)
    {
        for (unsigned int x = 0; x < bbBackground.width; x++)
        {
            auto color = img.at<cv::Vec3d>(y + bbBackground.y, x + bbBackground.x);
            bgData[x + y * bbBackground.width] = makeDataPoint(color);
        }
    }
    // Train background
    Distribution background;
    if (!background.load("background.gmm"))
    {
        background.train(bgData.begin(), bgData.end(), 5e2);
        background.save("background.gmm");
    }
    std::cout << "=== Background ===" << std::endl;
    background.print();

    /*
     * Set up a graph, where every pixel is connected to its 4 neighbors. The graph cut then optimizes
     * the labeling based on the confidence values of the MoG distributions and a regularizer.
     */

    // Setup 4-connected graph
    EdgeGraph graph;
    for(unsigned int y = 0; y < img.rows - 1; y++)
    {
        for(unsigned int x = 0; x < img.cols - 1; x++)
        {
            Site s = x + y * img.cols;
            Site sR = (x + 1) + y * img.cols;
            Site sD = x + (y + 1) * img.cols;
            graph.insertEdge(s, sR);
            graph.insertEdge(s, sD);
        }
    }
    // Unary energy
    auto unary = [&](Site s, Label l) -> Energy
    {
        auto dataPoint = makeDataPoint(img.at<cv::Vec3d>(s));
        if(l == 0)
            return -std::log(background.confidence(dataPoint));
        else
            return -std::log(foreground.confidence(dataPoint));
    };
    // Pairwise energy
    auto pairwise = [&](Site s1, Site s2, Label l1, Label l2) -> Energy
    {
        if(l1 == l2)
            return 0;
        else
        {
            // Contrast-sensitive potts model.
            Energy w = 2.2;
            Energy lambda = 0.5;
            cv::Vec3d color1 = img.at<cv::Vec3d>(s1);
            cv::Vec3d color2 = img.at<cv::Vec3d>(s2);
            return w * std::exp(-lambda * cv::norm(color1 - color2, cv::NORM_L2SQR));
        }
    };
    BinaryGraphCutOptimizer optimizer (graph, unary, pairwise);
    optimizer.optimize();

    /*
     * Show the results.
     */

    // Show foreground confidence image
    cv::Mat foregroundConf(img.rows, img.cols, CV_64FC1);
    for (unsigned int y = 0; y < img.rows; y++)
    {
        for (unsigned int x = 0; x < img.cols; x++)
            foregroundConf.at<double>(y, x) = foreground.confidence(makeDataPoint(img.at<cv::Vec3d>(y, x)));
    }
    cv::normalize(foregroundConf, foregroundConf, 0, 255, CV_MINMAX);
    cv::imshow("Foreground confidences", foregroundConf);

    // Show background confidence image
    cv::Mat backgroundConf(img.rows, img.cols, CV_64FC1);
    for (unsigned int y = 0; y < img.rows; y++)
    {
        for (unsigned int x = 0; x < img.cols; x++)
            backgroundConf.at<double>(y, x) = background.confidence(makeDataPoint(img.at<cv::Vec3d>(y, x)));
    }
    cv::normalize(backgroundConf, backgroundConf, 0, 255, CV_MINMAX);
    cv::imshow("Background confidences", backgroundConf);

    // Show foreground-background segmentation
    cv::Mat seg(img.rows, img.cols, CV_8UC3);
    for (unsigned int y = 0; y < img.rows; y++)
    {
        for (unsigned int x = 0; x < img.cols; x++)
        {
            if(backgroundConf.at<double>(y, x) > foregroundConf.at<double>(y, x))
                seg.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
            else
                seg.at<cv::Vec3b>(y, x) = cv::Vec3b(95, 218, 246);
        }
    }
    cv::imshow("Simple Segmentation", seg);

    // Show graph cut segmentation
    cv::Mat graphSeg(img.rows, img.cols, CV_8UC3);
    for (unsigned int y = 0; y < img.rows; y++)
    {
        for (unsigned int x = 0; x < img.cols; x++)
        {
            if(optimizer.whatSegment(x + y * img.cols) == 1)
                graphSeg.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
            else
                graphSeg.at<cv::Vec3b>(y, x) = cv::Vec3b(95, 218, 246);
        }
    }
    cv::imshow("Graphcut Segmentation", graphSeg);

    // Show original image with boxes
    img *= 255;
    cv::rectangle(img, bbForeground, cv::Scalar(0, 0, 0));
    cv::rectangle(img, bbBackground, cv::Scalar(0, 0, 0));
    img.convertTo(img, CV_8UC3);
    cv::imshow("Color Image", img);

    cv::waitKey();

    return 0;
}