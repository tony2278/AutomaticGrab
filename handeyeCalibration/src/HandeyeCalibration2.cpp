

/*
1 ����Ҫ�õ��궨���λ���������TCP��λ�ˣ�TCP��λ�����ɻ�������λ�������API�����ģ���˵�һ��������취�õ��궨���λ��
2 �궨��λ�ˣ�ֱ���ܲο�����2�Ĵ��뼴�ɣ����������޸ģ��������£�
3 �õ�λ��֮�󣬽�һ���òο����׶��еķ������궨�����۱任����

4 ��¼Opencv���۱궨�Ĺ���
  
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
bool to_record = false;//����������true->׼����¼һ������
vector<double> ENCODER_DATA(5, 0);//���������ݣ���������TCP

void control_thread() {//�����̣߳�����1���¼1������
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
void GetEncoderData(const ui::robot_feedbackConstPtr& data) {//ROS�ص���������ȡ����������
	ENCODER_DATA[0] = data->feedbackPosData[0];
	ENCODER_DATA[1] = data->feedbackPosData[1];
	ENCODER_DATA[2] = data->feedbackPosData[2];
	ENCODER_DATA[3] = data->feedbackPosData[3];
	ENCODER_DATA[4] = data->feedbackPosData[4];
}

void ros_thread() {//ROS�̣߳���������̵߳Ļ�,main�����������while(һ����realsense��while�� һ����ros��spin)
	//ros
	ros::NodeHandle n;
	ros::NodeHandle node;
	ros::Subscriber sub = node.subscribe("/low_level/joints_point_feedback", 100, GetEncoderData);
	ros::spin();
}

void m_calibration(vector<string> &FilesName, const Size& board_size, const Size& square_size, Mat &cameraMatrix, Mat &distCoeffs, vector<Mat> &rvecsMat, vector<Mat> &tvecsMat)
{
	ofstream fout("caliberation_result.txt");                       // ����궨������ļ�

	cout << "��ʼ��ȡ�ǵ㡭����������" << endl;
	int image_count = 0;                                            // ͼ������
	Size image_size;                                                // ͼ��ĳߴ�

	vector<Point2f> image_points;                                   // ����ÿ��ͼ���ϼ�⵽�Ľǵ�
	vector<vector<Point2f>> image_points_seq;                       // �����⵽�����нǵ�

	for (int i = 0; i < FilesName.size(); i++)
	{
		image_count++;

		// ���ڹ۲�������
		cout << "image_count = " << image_count << endl;
		Mat imageInput = imread(FilesName[i]);
		if (image_count == 1)  //�����һ��ͼƬʱ��ȡͼ������Ϣ
		{
			image_size.width = imageInput.cols;
			image_size.height = imageInput.rows;
			cout << "image_size.width = " << image_size.width << endl;
			cout << "image_size.height = " << image_size.height << endl;
		}

		/* ��ȡ�ǵ� */
		bool ok = findChessboardCorners(imageInput, board_size, image_points, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
		if (0 == ok)
		{
			cout << "��" << image_count << "����Ƭ��ȡ�ǵ�ʧ�ܣ���ɾ�������±궨��" << endl; //�Ҳ����ǵ�
			imshow("ʧ����Ƭ", imageInput);
			waitKey(0);
		}
		else
		{
			Mat view_gray;
			cout << "imageInput.channels()=" << imageInput.channels() << endl;
			cvtColor(imageInput, view_gray, COLOR_RGB2GRAY);

			/* �����ؾ�ȷ�� */
			//find4QuadCornerSubpix(view_gray, image_points, Size(5, 5)); //�Դ���ȡ�Ľǵ���о�ȷ��
			cv::cornerSubPix(view_gray, image_points, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 20, 0.01));

			image_points_seq.push_back(image_points);  //���������ؽǵ�

			/* ��ͼ������ʾ�ǵ�λ�� */
			drawChessboardCorners(view_gray, board_size, image_points, true);

			imshow("Camera Calibration", view_gray);//��ʾͼƬ
			waitKey(0);//��ͣ0.1S
		}
	}
	cout << "�ǵ���ȡ��ɣ�����" << endl;


	/*������ά��Ϣ*/
	vector<vector<Point3f>> object_points_seq;                     // ����궨���Ͻǵ����ά����

	for (int t = 0; t < image_count; t++)
	{
		vector<Point3f> object_points;
		for (int i = 0; i < board_size.height; i++)
		{
			for (int j = 0; j < board_size.width; j++)
			{
				Point3f realPoint;
				/* ����궨�������������ϵ��z=0��ƽ���� */
				realPoint.x = i * square_size.width;
				realPoint.y = j * square_size.height;
				realPoint.z = 0;
				object_points.push_back(realPoint);
			}
		}
		object_points_seq.push_back(object_points);
	}

	/* ���б궨���� */
	double err_first = calibrateCamera(object_points_seq, image_points_seq, image_size, cameraMatrix, distCoeffs, rvecsMat, tvecsMat, CALIB_FIX_K3);
	fout << "��ͶӰ���1��" << err_first << "����" << endl << endl;
	cout << "�궨��ɣ�����" << endl;


	cout << "��ʼ���۱궨���������������";
	double total_err = 0.0;            // ����ͼ���ƽ�������ܺ�
	double err = 0.0;                  // ÿ��ͼ���ƽ�����
	double totalErr = 0.0;
	double totalPoints = 0.0;
	vector<Point2f> image_points_pro;     // �������¼���õ���ͶӰ��

	for (int i = 0; i < image_count; i++)
	{

		projectPoints(object_points_seq[i], rvecsMat[i], tvecsMat[i], cameraMatrix, distCoeffs, image_points_pro);   //ͨ���õ������������������Խǵ�Ŀռ���ά�����������ͶӰ����

		err = norm(Mat(image_points_seq[i]), Mat(image_points_pro), NORM_L2);

		totalErr += err * err;
		totalPoints += object_points_seq[i].size();

		err /= object_points_seq[i].size();
		//fout << "��" << i + 1 << "��ͼ���ƽ����" << err << "����" << endl;
		total_err += err;
	}
	fout << "��ͶӰ���2��" << sqrt(totalErr / totalPoints) << "����" << endl << endl;
	fout << "��ͶӰ���3��" << total_err / image_count << "����" << endl << endl;


	//���涨����
	cout << "��ʼ���涨����������������" << endl;
	Mat rotation_matrix = Mat(3, 3, CV_32FC1, Scalar::all(0)); /* ����ÿ��ͼ�����ת���� */
	fout << "����ڲ�������" << endl;
	fout << cameraMatrix << endl << endl;
	fout << "����ϵ����\n";
	fout << distCoeffs << endl << endl << endl;
	for (int i = 0; i < image_count; i++)
	{
		fout << "��" << i + 1 << "��ͼ�����ת������" << endl;
		fout << rvecsMat[i] << endl;

		/* ����ת����ת��Ϊ���Ӧ����ת���� */
		Rodrigues(rvecsMat[i], rotation_matrix);
		fout << "��" << i + 1 << "��ͼ�����ת����" << endl;
		fout << rotation_matrix << endl;
		fout << "��" << i + 1 << "��ͼ���ƽ��������" << endl;
		fout << tvecsMat[i] << endl << endl;
	}
	cout << "��������ɱ��棡����" << endl;
	fout << endl;
}

/**************************************************
* @brief   ����ת������ƽ�������ϳ�Ϊ��ξ���
* @note
* @param   Mat& R   3*3��ת����
* @param   Mat& T   3*1ƽ�ƾ���
* @return  Mat      4*4��ξ���
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
	cv::hconcat(R1, T1, HomoMtr);		//����ƴ��
	return HomoMtr;
}

/**************************************************
* @brief    ��ξ���ֽ�Ϊ��ת������ƽ�ƾ���
* @note
* @param	const Mat& HomoMtr  4*4��ξ���
* @param	Mat& R              �����ת����
* @param	Mat& T				���ƽ�ƾ���
* @return
**************************************************/
void HomogeneousMtr2RT(Mat& HomoMtr, Mat& R, Mat& T)
{
	//Mat R_HomoMtr = HomoMtr(Rect(0, 0, 3, 3)); //ע��Rectȡֵ
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
	ClimbotKinematics *kinematics = new ClimberKinematics5D();//�˶�ѧ
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
	Size board_size = Size(11, 8);                         // �궨����ÿ�С��еĽǵ���
	Size square_size = Size(25, 25);                       // ʵ�ʲ����õ��ı궨����ÿ�����̸������ߴ磬��λmm
	Mat cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));        // ������ڲ�������
	Mat distCoeffs = Mat(1, 5, CV_32FC1, Scalar::all(0));          // �������5������ϵ����k1,k2,p1,p2,k3
	vector<Mat> R_target2cam;                                          // �������ͼ�����ת������ÿһ��ͼ�����ת����Ϊһ��mat
	vector<Mat> t_target2cam;                                          // �������ͼ���ƽ��������ÿһ��ͼ���ƽ������Ϊһ��mat
	m_calibration(pic_names, board_size, square_size, cameraMatrix, distCoeffs, R_target2cam, t_target2cam);
	//���ˣ���2�������

	//��ʼ��3����:�����ʽת��
	vector<cv::Mat> R_gripper2base, t_gripper2base;
	for (auto& T : TCP_poses) {
		Mat cv_format_tcp_pose, cv_format_tcp_rvec, cv_format_tcp_tvec;
		eigen2cv(T, cv_format_tcp_pose);
		HomogeneousMtr2RT(cv_format_tcp_pose, cv_format_tcp_rvec, cv_format_tcp_tvec);
		R_gripper2base.emplace_back(cv_format_tcp_rvec);
		t_gripper2base.emplace_back(cv_format_tcp_tvec);
	}
	//���ˣ���3�������

	//��ʼ��4���֣����۱궨����
	cv::Mat R_cam2gripper, T_cam2gripper;
	calibrateHandEye(R_gripper2base, t_gripper2base, R_target2cam, t_target2cam, R_cam2gripper, T_cam2gripper);

	//���岿�֣������ʽת��
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


