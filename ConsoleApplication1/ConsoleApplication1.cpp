﻿// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#include <k4a/k4a.hpp>

#include <iostream>
#include <vector>
#include <array>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>


#include "Pixel.h"
#include "DepthPixelColorizer.h"
#include "StaticImageProperties.h"

using namespace std;
using namespace cv;
using namespace sen;

int main()
{
	const uint32_t deviceCount = k4a::device::get_installed_count();
	if (deviceCount == 0)
	{
		cout << "no azure kinect devices detected!" << endl;
	}

	k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
	config.camera_fps = K4A_FRAMES_PER_SECOND_30;
	config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
	config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
	config.color_resolution = K4A_COLOR_RESOLUTION_720P;
	config.synchronized_images_only = true;

	cout << "Started opening K4A device..." << endl;
	k4a::device device = k4a::device::open(K4A_DEVICE_DEFAULT);
	device.start_cameras(&config);
	cout << "Finished opening K4A device." << endl;

	std::vector<Pixel> depthTextureBuffer;
	std::vector<Pixel> irTextureBuffer;
	uint8_t *colorTextureBuffer;

	k4a::capture capture;

	k4a::image depthImage;
	k4a::image colorImage;
	k4a::image irImage;

	cv::Mat depthFrame;
	cv::Mat colorFrame;
	cv::Mat irFrame;
	
	
	while (1)
	{
		if (device.get_capture(&capture, std::chrono::milliseconds(0)))
		{
			{
				depthImage = capture.get_depth_image();
				colorImage = capture.get_color_image();
				irImage = capture.get_ir_image();

				ColorizeDepthImage(depthImage, DepthPixelColorizer::ColorizeBlueToRed, GetDepthModeRange(config.depth_mode), &depthTextureBuffer);
				ColorizeDepthImage(irImage, DepthPixelColorizer::ColorizeGreyscale, GetIrLevels(K4A_DEPTH_MODE_PASSIVE_IR), &irTextureBuffer);
				colorTextureBuffer = colorImage.get_buffer();

				depthFrame = cv::Mat(depthImage.get_height_pixels(), depthImage.get_width_pixels(), CV_8UC4, depthTextureBuffer.data());
				colorFrame = cv::Mat(colorImage.get_height_pixels(), colorImage.get_width_pixels(), CV_8UC4, colorTextureBuffer);
				irFrame = cv::Mat(irImage.get_height_pixels(), irImage.get_width_pixels(), CV_8UC4, irTextureBuffer.data());


				cv::imshow("kinect depth map master", depthFrame);

			}
		}
		if (waitKey(30) == 27 || waitKey(30) == 'q')
		{
			device.close();
			break;
		}
	}
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
