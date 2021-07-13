

/*
1 首先要拿到标定板的位姿与机器人TCP的位姿；TCP的位姿是由机器人上位机软件或API给出的，因此第一步是先想办法拿到标定板的位姿
2 标定板位姿：直接跑参考文献2的代码即可，我稍作了修改，代码如下：
3 拿到位姿之后，进一步用参考文献二中的方法，标定出手眼变换矩阵

4 记录Opencv手眼标定的过程
  
*/

//
// Created by ch on 2020/11/25.
// https://blog.csdn.net/weixin_41698305/article/details/110133734
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <ui/robot_feedback.h>
#include <ros/ros.h>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include "ClimberKinematics5D.h"
#include <opencv2/core/eigen.hpp>
#include <opencv2/opencv.hpp>   // Include OpenCV API
#include <opencv2/calib3d.hpp>
using namespace std;
using namespace cv;

enum CONTROL_STATUS { RECORD, WAIT, QUIT } CONTROL_FLAG;
bool to_record = false;//交互变量，true->准备记录一组数据
vector<double> ENCODER_DATA(5, 0);//编码器数据，用来计算TCP

void control_thread() {//交互线程，输入1则记录1组数据
	bool to_quit = false;
	while (!to_quit) {
		switch (CONTROL_FLAG) {
		case CONTROL_STATUS::WAIT: {
			int i = 0;
			cout << "Input 1 for record, input 0 for stop recording.\n";
			cin >> i;
			if (i == 0) {
				CONTROL_FLAG = CONTROL_STATUS::QUIT;
				break;
			}
			else if (i == 1) CONTROL_FLAG = CONTROL_STATUS::RECORD;
			break;
		}
		case CONTROL_STATUS::QUIT: to_quit = true; break;
		case CONTROL_STATUS::RECORD: break;
		}
	}
}
void GetEncoderData(const ui::robot_feedbackConstPtr& data) {//ROS回调函数，获取编码器数据
	ENCODER_DATA[0] = data->feedbackPosData[0];
	ENCODER_DATA[1] = data->feedbackPosData[1];
	ENCODER_DATA[2] = data->feedbackPosData[2];
	ENCODER_DATA[3] = data->feedbackPosData[3];
	ENCODER_DATA[4] = data->feedbackPosData[4];
}

void ros_thread() {//ROS线程，如果不另开线程的话,main里面会有两个while(一个是realsense的while， 一个是ros的spin)
	//ros
	ros::NodeHandle n;
	ros::NodeHandle node;
	ros::Subscriber sub = node.subscribe("/low_level/joints_point_feedback", 100, GetEncoderData);
	ros::spin();
}

void m_calibration(vector<string> &FilesName, const Size& board_size, const Size& square_size, Mat &cameraMatrix, Mat &distCoeffs, vector<Mat> &rvecsMat, vector<Mat> &tvecsMat)
{
	ofstream fout("caliberation_result.txt");                       // 保存标定结果的文件

	cout << "开始提取角点………………" << endl;
	int image_count = 0;                                            // 图像数量
	Size image_size;                                                // 图像的尺寸

	vector<Point2f> image_points;                                   // 缓存每幅图像上检测到的角点
	vector<vector<Point2f>> image_points_seq;                       // 保存检测到的所有角点

	for (int i = 0; i < FilesName.size(); i++)
	{
		image_count++;

		// 用于观察检验输出
		cout << "image_count = " << image_count << endl;
		Mat imageInput = imread(FilesName[i]);
		if (image_count == 1)  //读入第一张图片时获取图像宽高信息
		{
			image_size.width = imageInput.cols;
			image_size.height = imageInput.rows;
			cout << "image_size.width = " << image_size.width << endl;
			cout << "image_size.height = " << image_size.height << endl;
		}

		/* 提取角点 */
		bool ok = findChessboardCorners(imageInput, board_size, image_points, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
		if (0 == ok)
		{
			cout << "第" << image_count << "张照片提取角点失败，请删除后，重新标定！" << endl; //找不到角点
			imshow("失败照片", imageInput);
			waitKey(0);
		}
		else
		{
			Mat view_gray;
			cout << "imageInput.channels()=" << imageInput.channels() << endl;
			cvtColor(imageInput, view_gray, COLOR_RGB2GRAY);

			/* 亚像素精确化 */
			//find4QuadCornerSubpix(view_gray, image_points, Size(5, 5)); //对粗提取的角点进行精确化
			cv::cornerSubPix(view_gray, image_points, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 20, 0.01));

			image_points_seq.push_back(image_points);  //保存亚像素角点

			/* 在图像上显示角点位置 */
			drawChessboardCorners(view_gray, board_size, image_points, true);

			imshow("Camera Calibration", view_gray);//显示图片
			waitKey(0);//暂停0.1S
		}
	}
	cout << "角点提取完成！！！" << endl;


	/*棋盘三维信息*/
	vector<vector<Point3f>> object_points_seq;                     // 保存标定板上角点的三维坐标

	for (int t = 0; t < image_count; t++)
	{
		vector<Point3f> object_points;
		for (int i = 0; i < board_size.height; i++)
		{
			for (int j = 0; j < board_size.width; j++)
			{
				Point3f realPoint;
				/* 假设标定板放在世界坐标系中z=0的平面上 */
				realPoint.x = i * square_size.width;
				realPoint.y = j * square_size.height;
				realPoint.z = 0;
				object_points.push_back(realPoint);
			}
		}
		object_points_seq.push_back(object_points);
	}

	/* 运行标定函数 */
	double err_first = calibrateCamera(object_points_seq, image_points_seq, image_size, cameraMatrix, distCoeffs, rvecsMat, tvecsMat, CALIB_FIX_K3);
	fout << "重投影误差1：" << err_first << "像素" << endl << endl;
	cout << "标定完成！！！" << endl;


	cout << "开始评价标定结果………………";
	double total_err = 0.0;            // 所有图像的平均误差的总和
	double err = 0.0;                  // 每幅图像的平均误差
	double totalErr = 0.0;
	double totalPoints = 0.0;
	vector<Point2f> image_points_pro;     // 保存重新计算得到的投影点

	for (int i = 0; i < image_count; i++)
	{

		projectPoints(object_points_seq[i], rvecsMat[i], tvecsMat[i], cameraMatrix, distCoeffs, image_points_pro);   //通过得到的摄像机内外参数，对角点的空间三维坐标进行重新投影计算

		err = norm(Mat(image_points_seq[i]), Mat(image_points_pro), NORM_L2);

		totalErr += err * err;
		totalPoints += object_points_seq[i].size();

		err /= object_points_seq[i].size();
		//fout << "第" << i + 1 << "幅图像的平均误差：" << err << "像素" << endl;
		total_err += err;
	}
	fout << "重投影误差2：" << sqrt(totalErr / totalPoints) << "像素" << endl << endl;
	fout << "重投影误差3：" << total_err / image_count << "像素" << endl << endl;


	//保存定标结果
	cout << "开始保存定标结果………………" << endl;
	Mat rotation_matrix = Mat(3, 3, CV_32FC1, Scalar::all(0)); /* 保存每幅图像的旋转矩阵 */
	fout << "相机内参数矩阵：" << endl;
	fout << cameraMatrix << endl << endl;
	fout << "畸变系数：\n";
	fout << distCoeffs << endl << endl << endl;
	for (int i = 0; i < image_count; i++)
	{
		fout << "第" << i + 1 << "幅图像的旋转向量：" << endl;
		fout << rvecsMat[i] << endl;

		/* 将旋转向量转换为相对应的旋转矩阵 */
		Rodrigues(rvecsMat[i], rotation_matrix);
		fout << "第" << i + 1 << "幅图像的旋转矩阵：" << endl;
		fout << rotation_matrix << endl;
		fout << "第" << i + 1 << "幅图像的平移向量：" << endl;
		fout << tvecsMat[i] << endl << endl;
	}
	cout << "定标结果完成保存！！！" << endl;
	fout << endl;
}

/**************************************************
* @brief   将旋转矩阵与平移向量合成为齐次矩阵
* @note
* @param   Mat& R   3*3旋转矩阵
* @param   Mat& T   3*1平移矩阵
* @return  Mat      4*4齐次矩阵
**************************************************/
Mat R_T2HomogeneousMatrix(const Mat& R, const Mat& T)
{
	Mat HomoMtr;
	Mat_<double> R1 = (Mat_<double>(4, 3) <<
		R.at<double>(0, 0), R.at<double>(0, 1), R.at<double>(0, 2),
		R.at<double>(1, 0), R.at<double>(1, 1), R.at<double>(1, 2),
		R.at<double>(2, 0), R.at<double>(2, 1), R.at<double>(2, 2),
		0, 0, 0);
	Mat_<double> T1 = (Mat_<double>(4, 1) <<
		T.at<double>(0, 0),
		T.at<double>(1, 0),
		T.at<double>(2, 0),
		1);
	cv::hconcat(R1, T1, HomoMtr);		//矩阵拼接
	return HomoMtr;
}

/**************************************************
* @brief    齐次矩阵分解为旋转矩阵与平移矩阵
* @note
* @param	const Mat& HomoMtr  4*4齐次矩阵
* @param	Mat& R              输出旋转矩阵
* @param	Mat& T				输出平移矩阵
* @return
**************************************************/
void HomogeneousMtr2RT(Mat& HomoMtr, Mat& R, Mat& T)
{
	//Mat R_HomoMtr = HomoMtr(Rect(0, 0, 3, 3)); //注意Rect取值
	//Mat T_HomoMtr = HomoMtr(Rect(3, 0, 1, 3));
	//R_HomoMtr.copyTo(R);
	//T_HomoMtr.copyTo(T);
	/*HomoMtr(Rect(0, 0, 3, 3)).copyTo(R);
	HomoMtr(Rect(3, 0, 1, 3)).copyTo(T);*/
	Rect R_rect(0, 0, 3, 3);
	Rect T_rect(3, 0, 1, 3);
	R = HomoMtr(R_rect);
	T = HomoMtr(T_rect);

}

int main(int argc, char * argv[]) try
{
	//realsense initialization
	rs2::colorizer color_map;
	rs2::pipeline pipe;
	pipe.start();

	//other initialization
	ClimbotKinematics *kinematics = new ClimberKinematics5D();//运动学
	std::thread control(control_thread);
	std::thread ros_t(ros_thread);
	vector<Eigen::Matrix4d> TCP_poses;
	vector<string> pic_names;
	int count = 0;

	//capture pictures
	const auto window_name = "Display Image";
	namedWindow(window_name, WINDOW_AUTOSIZE);
	bool to_quit = false;
	while (waitKey(1) < 0 && getWindowProperty(window_name, WND_PROP_AUTOSIZE) >= 0 && !to_quit) {
		rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
		rs2::frame depth = data.get_depth_frame().apply_filter(color_map);
		rs2::frame rgb = data.get_color_frame();

		const int w = rgb.as<rs2::video_frame>().get_width();
		const int h = rgb.as<rs2::video_frame>().get_height();

		Mat image(Size(w, h), CV_8UC3, (void*)rgb.get_data(), Mat::AUTO_STEP);
		cvtColor(image, image, COLOR_BGR2RGB);

		//record a picture

		switch (CONTROL_FLAG) {
		case CONTROL_STATUS::RECORD: {
			string num_string = to_string(count);
			cv::imwrite("./pose_" + num_string + ".jpg", image);
			pic_names.emplace_back("./pose_" + num_string + ".jpg");

			auto T = kinematics->GetTransformMatrix(ClimberKinematics5D::G6, ENCODER_DATA);
			TCP_poses.push_back(T);

			ofstream TCP_trasform("./TCP_pose_" + num_string + ".txt", ofstream::out);
			Eigen::Matrix3d R = T.block(0, 0, 3, 3);
			Eigen::Vector3d euler_angles = R.eulerAngles(2, 1, 0);
			TCP_trasform << "Index: " << num_string << endl;
			TCP_trasform << "Fixed X-Y-Z(x/y/z):\n" << euler_angles[0] / PI * 180 << " " << euler_angles[1] / PI * 180 << " " << euler_angles[2] / PI * 180 << endl;
			TCP_trasform << "Position(m):\n" << T(0, 3) / 1000 << " " << T(1, 3) / 1000 << " " << T(2, 3) / 1000 << endl << endl;
			break;
		}
		case WAIT:
			break;
		case QUIT:
			to_quit = true;
			break;
		}

		count++;

		// Update the window with new data
		imshow(window_name, image);
	}
	//camera calibration and get board pose
	Size board_size = Size(11, 8);                         // 标定板上每行、列的角点数
	Size square_size = Size(25, 25);                       // 实际测量得到的标定板上每个棋盘格的物理尺寸，单位mm
	Mat cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));        // 摄像机内参数矩阵
	Mat distCoeffs = Mat(1, 5, CV_32FC1, Scalar::all(0));          // 摄像机的5个畸变系数：k1,k2,p1,p2,k3
	vector<Mat> R_target2cam;                                          // 存放所有图像的旋转向量，每一副图像的旋转向量为一个mat
	vector<Mat> t_target2cam;                                          // 存放所有图像的平移向量，每一副图像的平移向量为一个mat
	m_calibration(pic_names, board_size, square_size, cameraMatrix, distCoeffs, R_target2cam, t_target2cam);
	//至此，第2部分完成

	//开始第3部分:矩阵格式转换
	vector<cv::Mat> R_gripper2base, t_gripper2base;
	for (auto& T : TCP_poses) {
		Mat cv_format_tcp_pose, cv_format_tcp_rvec, cv_format_tcp_tvec;
		eigen2cv(T, cv_format_tcp_pose);
		HomogeneousMtr2RT(cv_format_tcp_pose, cv_format_tcp_rvec, cv_format_tcp_tvec);
		R_gripper2base.emplace_back(cv_format_tcp_rvec);
		t_gripper2base.emplace_back(cv_format_tcp_tvec);
	}
	//至此，第3部分完成

	//开始第4部分：手眼标定程序
	cv::Mat R_cam2gripper, T_cam2gripper;
	calibrateHandEye(R_gripper2base, t_gripper2base, R_target2cam, t_target2cam, R_cam2gripper, T_cam2gripper);

	//第五部分：矩阵格式转换
	auto Homo_cam2gripper = R_T2HomogeneousMatrix(R_cam2gripper, T_cam2gripper);
	cout << "Homo_cam2gripper:\n" << Homo_cam2gripper << endl;
	cout << endl << "Finish.";
	return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
	std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
}


