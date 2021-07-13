

/*
  OpenCV����궨ȫ����
  https://blog.csdn.net/czhzasui/article/details/83273380

*/


//1 ����궨
cv::imwrite("../blizzard/res/calibration/cb_source.png", cb_source);

printf("#Start scan corner\n");
cv::Mat img;
std::vector<cv::Point2f> image_points;
std::vector<std::vector<cv::Point2f>> image_points_seq; /* �����⵽�����нǵ� */
if (cv::findChessboardCorners(cb_source, cv::Size(aqXnum, aqYnum), image_points, 0) == 0) {
	printf("#Error: Corners not find ");
	return 0;
}
else {
	cvtColor(cb_source, img, CV_RGBA2GRAY);
	cv::imwrite("../blizzard/res/calibration/cb_gray.png", img);
	//find4QuadCornerSubpix(img, image_points, cv::Size(5, 5));

	cv::cornerSubPix(img, image_points, cv::Size(11, 11), cv::Size(-1, -1),
		cv::TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 30, 0.01));

	image_points_seq.push_back(image_points);

	cv::Mat cb_corner;
	cb_corner = cb_source.clone();
	drawChessboardCorners(cb_corner, cv::Size(aqXnum, aqYnum), image_points, true);
	cv::imwrite("../blizzard/res/calibration/cb_corner.png", cb_corner);
}

printf("#Start calibrate\n");
cv::Size square_size = cv::Size(14.2222, 12);
std::vector<std::vector<cv::Point3f>> object_points; /* ����궨���Ͻǵ����ά���� */
cv::Mat cameraMatrix = cv::Mat(3, 3, CV_32FC1, cv::Scalar::all(0)); /* ������ڲ������� */
cv::Mat distCoeffs = cv::Mat(1, 5, CV_32FC1, cv::Scalar::all(0)); /* �������5������ϵ����k1,k2,p1,p2,k3 */
std::vector<cv::Mat> tvecsMat;  /* ÿ��ͼ�����ת���� */
std::vector<cv::Mat> rvecsMat;  /* ÿ��ͼ���ƽ������ */

std::vector<cv::Point3f> realPoint;
for (int i = 0; i < aqYnum; i++) {
	for (int j = 0; j < aqXnum; j++) {
		cv::Point3f tempPoint;
		/* ����궨�������������ϵ��z=0��ƽ���� */
		tempPoint.x = i * square_size.width;
		tempPoint.y = j * square_size.height;
		tempPoint.z = 0;
		realPoint.push_back(tempPoint);
	}
}
object_points.push_back(realPoint);

printf("#objectPoints�� %ld\n", sizeof(object_points[0]));
std::cout << object_points[0] << std::endl;

printf("#image_points�� %ld\n", sizeof(image_points_seq[0]));
std::cout << image_points << std::endl;

printf("#image size\n");
std::cout << SCREEN_WIDTH << "*" << SCREEN_HEIGHT << std::endl;

cv::calibrateCamera(object_points, image_points_seq, cb_source.size(), cameraMatrix, distCoeffs, rvecsMat, tvecsMat,
	CV_CALIB_FIX_K3);

std::cout << "tvecsMat:\n" << tvecsMat[0] << std::endl;
std::cout << "rvecsMat:\n" << rvecsMat[0] << std::endl;

std::cout << "#cameraMatrix:\n" << cameraMatrix << std::endl;
std::cout << "#distCoeffs:\n" << distCoeffs << std::endl;



//2 ��ͼƬ����У��
cv::Mat cb_final;

cv::Mat mapx = cv::Mat(cb_source.size(), CV_32FC1);
cv::Mat mapy = cv::Mat(cb_source.size(), CV_32FC1);
cv::Mat R = cv::Mat::eye(3, 3, CV_32F);
//initUndistortRectifyMap(cameraMatrix, distCoeffs, R, cv::Mat(), cb_source.size(), CV_32FC1,
//                        mapx, mapy);
//cv::remap(cb_source, cb_final, mapx, mapy, cv::INTER_LINEAR);

undistort(cb_source, cb_final, cameraMatrix, distCoeffs);

cv::imwrite("../blizzard/res/calibration/cb_final.png", cb_final);








