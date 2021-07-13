#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;


/*
˼·����дһЩ������ʵ����Ԫ����ŷ���ǵ���ת�����ת����
ʵ����ξ���ķֽ���ϳɣ�
Ȼ�������ݣ��ṩ��calibrateHandeye()ʹ�ã���������۾���

  https://blog.csdn.net/hellohake/article/details/104808149
*/


Mat R_T2HomogeneousMatrix(const Mat& R, const Mat& T);
void HomogeneousMtr2RT(Mat& HomoMtr, Mat& R, Mat& T);
bool isRotatedMatrix(Mat& R);
Mat eulerAngleToRotateMatrix(const Mat& eulerAngle, const std::string& seq);
Mat quaternionToRotatedMatrix(const Vec4d& q);
Mat attitudeVectorToMatrix(const Mat& m, bool useQuaternion, const string& seq);


//����ʹ�õ����е�ֵ
//�����13��궨���λ�ˣ�x,y,z��rx,ry,rz,
Mat_<double> CalPose = (cv::Mat_<double>(13, 6) <<
	-0.072944147641399, -0.06687830562048944, 0.4340418493881254, -0.2207496117519063, 0.0256862005614321, 0.1926014162476009,
	-0.01969337858360518, -0.05095294728651902, 0.3671266719105768, 0.1552099329677287, -0.5763323472739464, 0.09956130526058841,
	0.1358164536530692, -0.1110802522656379, 0.4001396735998251, -0.04486168331242635, -0.004268942058870162, 0.05290073845562016,
	0.1360676260120161, -0.002373036366121294, 0.3951670952829301, -0.4359637938379769, 0.00807193982932386, 0.06162504121755787,
	-0.1047666520352697, -0.01377729010376614, 0.4570029374109721, -0.612072103513551, -0.04939465180949879, -0.1075464055169537,
	0.02866460103085085, -0.1043911269729344, 0.3879127305077527, 0.3137563103168434, -0.02113958397023016, 0.1311397970432597,
	0.1122741829392126, 0.001044006395747612, 0.3686697279333643, 0.1607160803445018, 0.2468677059920437, 0.1035103912091547,
	-0.06079521129779342, -0.02815190820828123, 0.4451740202390909, 0.1280935541917056, -0.2674407142401368, 0.1633865613363686,
	-0.02475533256363622, -0.06950841248698086, 0.2939836207787282, 0.1260629671933584, -0.2637748974005461, 0.1634102148863728,
	0.1128618887222624, 0.00117877722121125, 0.3362496409334229, 0.1049541359309871, -0.2754352318773509, 0.4251492928748009,
	0.1510545750008333, -0.0725019944548204, 0.3369908269102371, 0.2615745097093249, -0.1295598776133405, 0.6974394284203849,
	0.04885313290076512, -0.06488755216394324, 0.2441532410787161, 0.1998243391807502, -0.04919417529483511, -0.05133193756053007,
	0.08816140480523708, -0.05549965109057759, 0.3164905645998022, 0.164693654482863, 0.1153894876338608, 0.01455551646362294);

//��е��ĩ��13��λ��,x,y,z,rx,ry,rz
Mat_<double> ToolPose = (cv::Mat_<double>(13, 6) <<
	-0.3969707, -0.460018, 0.3899877, 90.2261, -168.2015, 89.7748,
	-0.1870185, -0.6207147, 0.2851157, 57.2636, -190.2034, 80.7958,
	-0.1569776, -0.510021, 0.3899923, 90.225, -178.2038, 81.7772,
	-0.1569787, -0.5100215, 0.3299975, 90.2252, -156.205, 81.7762,
	-0.3369613, -0.4100348, 0.3299969, 90.2264, -146.2071, 71.778,
	-0.2869552, -0.6100449, 0.4299998, 90.2271, -199.2048, 86.7806,
	-0.2869478, -0.6600489, 0.4299948, 105.2274, -189.2053, 86.7814,
	-0.286938, -0.6300559, 0.4299997, 75.2279, -189.2056, 86.783,
	-0.2869343, -0.5700635, 0.2800084, 75.2291, -189.2055, 86.7835,
	-0.1669241, -0.5700796, 0.280015, 75.2292, -189.205, 101.7845,
	-0.236909, -0.4700997, 0.3600046, 87.2295, -196.2063, 118.7868,
	-0.2369118, -0.6201035, 0.2600001, 87.2297, -192.2087, 75.7896,
	-0.2468983, -0.620112, 0.359992, 97.2299, -190.2082, 80.7908);


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

/**************************************************
* @brief	����Ƿ�����ת����
* @note
* @param
* @param
* @param
* @return  true : ����ת���� false : ������ת����
**************************************************/
bool isRotatedMatrix(Mat& R)		//��ת�����ת�þ������������������� * ���� = ��λ����
{
	Mat temp33 = R({ 0,0,3,3 });	//���������Ǽ��׾��󣬾���ȡ�������׾���
	Mat Rt;
	transpose(temp33, Rt);  //ת�þ���
	Mat shouldBeIdentity = Rt * temp33;//����ת������˻�Ϊ��λ����
	Mat I = Mat::eye(3, 3, shouldBeIdentity.type());

	return cv::norm(I, shouldBeIdentity) < 1e-6;
}

/**************************************************
* @brief   ŷ����ת��Ϊ��ת����
* @note
* @param    const std::string& seq  ָ��ŷ���ǵ�����˳�򣻣���е�۵�λ��������xyz,zyx,zyz���֣���Ҫ���֣�
* @param    const Mat& eulerAngle   ŷ���ǣ�1*3����, �Ƕ�ֵ
* @param
* @return   ����3*3��ת����
**************************************************/
Mat eulerAngleToRotateMatrix(const Mat& eulerAngle, const std::string& seq)
{
	CV_Assert(eulerAngle.rows == 1 && eulerAngle.cols == 3);//�������Ƿ���ȷ

	eulerAngle /= (180 / CV_PI);		//��ת����

	Matx13d m(eulerAngle);				//<double, 1, 3>

	auto rx = m(0, 0), ry = m(0, 1), rz = m(0, 2);
	auto rxs = sin(rx), rxc = cos(rx);
	auto rys = sin(ry), ryc = cos(ry);
	auto rzs = sin(rz), rzc = cos(rz);

	//XYZ�������ת����
	Mat RotX = (Mat_<double>(3, 3) << 1, 0, 0,
		0, rxc, -rxs,
		0, rxs, rxc);
	Mat RotY = (Mat_<double>(3, 3) << ryc, 0, rys,
		0, 1, 0,
		-rys, 0, ryc);
	Mat RotZ = (Mat_<double>(3, 3) << rzc, -rzs, 0,
		rzs, rzc, 0,
		0, 0, 1);
	//��˳��ϳɺ����ת����
	cv::Mat rotMat;

	if (seq == "zyx") rotMat = RotX * RotY * RotZ;
	else if (seq == "yzx") rotMat = RotX * RotZ * RotY;
	else if (seq == "zxy") rotMat = RotY * RotX * RotZ;
	else if (seq == "yxz") rotMat = RotZ * RotX * RotY;
	else if (seq == "xyz") rotMat = RotZ * RotY * RotX;
	else if (seq == "xzy") rotMat = RotY * RotZ * RotX;
	else
	{
		cout << "Euler Angle Sequence string is wrong...";
	}
	if (!isRotatedMatrix(rotMat))		//ŷ������������»��������
	{
		cout << "Euler Angle convert to RotatedMatrix failed..." << endl;
		exit(-1);
	}
	return rotMat;
}

/**************************************************
* @brief   ����Ԫ��ת��Ϊ��ת����
* @note
* @param   const Vec4d& q   ��һ������Ԫ��: q = q0 + q1 * i + q2 * j + q3 * k;
* @return  ����3*3��ת����R
**************************************************/
Mat quaternionToRotatedMatrix(const Vec4d& q)
{
	double q0 = q[0], q1 = q[1], q2 = q[2], q3 = q[3];

	double q0q0 = q0 * q0, q1q1 = q1 * q1, q2q2 = q2 * q2, q3q3 = q3 * q3;
	double q0q1 = q0 * q1, q0q2 = q0 * q2, q0q3 = q0 * q3;
	double q1q2 = q1 * q2, q1q3 = q1 * q3;
	double q2q3 = q2 * q3;
	//���ݹ�ʽ����
	Mat RotMtr = (Mat_<double>(3, 3) << (q0q0 + q1q1 - q2q2 - q3q3), 2 * (q1q2 + q0q3), 2 * (q1q3 - q0q2),
		2 * (q1q2 - q0q3), (q0q0 - q1q1 + q2q2 - q3q3), 2 * (q2q3 + q0q1),
		2 * (q1q3 + q0q2), 2 * (q2q3 - q0q1), (q0q0 - q1q1 - q2q2 + q3q3));
	//������ʽ�ȼ�
	/*Mat RotMtr = (Mat_<double>(3, 3) << (1 - 2 * (q2q2 + q3q3)), 2 * (q1q2 - q0q3), 2 * (q1q3 + q0q2),
										 2 * (q1q2 + q0q3), 1 - 2 * (q1q1 + q3q3), 2 * (q2q3 - q0q1),
										 2 * (q1q3 - q0q2), 2 * (q2q3 + q0q1), (1 - 2 * (q1q1 + q2q2)));*/

	return RotMtr;
}

/**************************************************
* @brief      ���ɼ���ԭʼ����ת��Ϊ��ξ��󣨴ӻ����˿������л�õģ�
* @note
* @param	  Mat& m    1*6//1*10���� �� Ԫ��Ϊ�� x,y,z,rx,ry,rz  or x,y,z, q0,q1,q2,q3,rx,ry,rz
* @param	  bool useQuaternion      ԭʼ�����Ƿ�ʹ����Ԫ����ʾ
* @param	  string& seq         ԭʼ����ʹ��ŷ���Ǳ�ʾʱ������ϵ����ת˳��
* @return	  ����ת�������ξ���
**************************************************/
Mat attitudeVectorToMatrix(const Mat& m, bool useQuaternion, const string& seq)
{
	CV_Assert(m.total() == 6 || m.total() == 10);
	//if (m.cols == 1)	//ת�þ���Ϊ�о���
	//	m = m.t();	

	Mat temp = Mat::eye(4, 4, CV_64FC1);

	if (useQuaternion)
	{
		Vec4d quaternionVec = m({ 3,0,4,1 });   //��ȡ�洢����Ԫ��
		quaternionToRotatedMatrix(quaternionVec).copyTo(temp({ 0,0,3,3 }));
	}
	else
	{
		Mat rotVec;
		if (m.total() == 6)
		{
			rotVec = m({ 3,0,3,1 });   //��ȡ�洢��ŷ����
		}
		if (m.total() == 10)
		{
			rotVec = m({ 7,0,3,1 });
		}
		//���seqΪ�գ���ʾ�������3*1��ת���������򣬴������ŷ����
		if (0 == seq.compare(""))
		{
			Rodrigues(rotVec, temp({ 0,0,3,3 }));   //�޵���˹ת��
		}
		else
		{
			eulerAngleToRotateMatrix(rotVec, seq).copyTo(temp({ 0,0,3,3 }));
		}
	}
	//����ƽ�ƾ���
	temp({ 3,0,1,3 }) = m({ 0,0,3,1 }).t();
	return temp;   //����ת����������ξ���
}

int main(int argc, char** argv)
{
	//��������
	vector<Mat> R_gripper2base;
	vector<Mat> T_gripper2base;
	vector<Mat> R_target2cam;
	vector<Mat> T_target2cam;
	Mat R_cam2gripper = Mat(3, 3, CV_64FC1);				//������е��ĩ������ϵ����ת������ƽ�ƾ���
	Mat T_cam2gripper = Mat(3, 1, CV_64FC1);
	Mat Homo_cam2gripper = Mat(4, 4, CV_64FC1);

	vector<Mat> Homo_target2cam;
	vector<Mat> Homo_gripper2base;
	Mat tempR, tempT, temp;

	for (int i = 0; i < CalPose.rows; i++)				//����궨������������ξ�����ת������ƽ��������
	{
		temp = attitudeVectorToMatrix(CalPose.row(i), false, "");	//ע��seqΪ�գ������궨����Ϊ��ת����
		Homo_target2cam.push_back(temp);
		HomogeneousMtr2RT(temp, tempR, tempT);
		/*cout << i << "::" << temp << endl;
		cout << i << "::" << tempR << endl;
		cout << i << "::" << tempT << endl;*/
		R_target2cam.push_back(tempR);
		T_target2cam.push_back(tempT);
	}
	for (int j = 0; j < ToolPose.rows; j++)				//�����е��ĩ������ϵ������˻�����ϵ֮�����ξ���
	{
		temp = attitudeVectorToMatrix(ToolPose.row(j), false, "xyz");  //ע��seq���ǿգ���е��ĩ������ϵ������˻�����ϵ֮���Ϊŷ����
		Homo_gripper2base.push_back(temp);
		HomogeneousMtr2RT(temp, tempR, tempT);
		/*cout << j << "::" << temp << endl;
		cout << j << "::" << tempR << endl;
		cout << j << "::" << tempT << endl;*/
		R_gripper2base.push_back(tempR);
		T_gripper2base.push_back(tempT);
	}
	//TSAI�����ٶ����
	calibrateHandEye(R_gripper2base, T_gripper2base, R_target2cam, T_target2cam, R_cam2gripper, T_cam2gripper, CALIB_HAND_EYE_TSAI);

	Homo_cam2gripper = R_T2HomogeneousMatrix(R_cam2gripper, T_cam2gripper);
	cout << Homo_cam2gripper << endl;
	cout << "Homo_cam2gripper �Ƿ������ת����" << isRotatedMatrix(Homo_cam2gripper) << endl;

	///

		/**************************************************
		* @note   ����ϵͳ���Ȳ��ԣ�ԭ���Ǳ궨���ڻ����˻�����ϵ��λ�˹̶����䣬
		*		  ���Ը�����һ���������
		**************************************************/
		//ʹ��1,2��������֤  �궨���ڻ����˻�����ϵ��λ�˹̶�����
	cout << "1 : " << Homo_gripper2base[0] * Homo_cam2gripper * Homo_target2cam[0] << endl;
	cout << "2 : " << Homo_gripper2base[1] * Homo_cam2gripper * Homo_target2cam[1] << endl;
	//�궨��������е�λ��
	cout << "3 : " << Homo_target2cam[1] << endl;
	cout << "4 : " << Homo_cam2gripper.inv() * Homo_gripper2base[1].inv() * Homo_gripper2base[0] * Homo_cam2gripper * Homo_target2cam[0] << endl;

	cout << "----����ϵͳ����-----" << endl;
	cout << "��е���±궨��XYZΪ��" << endl;
	for (int i = 0; i < Homo_target2cam.size(); i++)
	{
		Mat chessPos{ 0.0,0.0,0.0,1.0 };  //4*1���󣬵������е������ϵ�£��궨��XYZ
		Mat worldPos = Homo_gripper2base[i] * Homo_cam2gripper * Homo_target2cam[i] * chessPos;
		cout << i << ": " << worldPos.t() << endl;
	}
	waitKey(0);

	return 0;
}




