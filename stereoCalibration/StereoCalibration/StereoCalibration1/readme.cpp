
/*
 * 1 ΪʲôҪ������ͷ�궨��
 *   �궨��Ŀ����Ϊ�����������Լ��õ������������
 *
 * 2 ���������ͷ�ı궨��
 * ���裺

��1���������̸񣬴�ӡOpencv�Դ���9*7�����̸�A4ֽ�ϡ����̸񷽸��СΪ27*27mm��
��2����׼����ζ����̸񡣵õ�����ͼ��ע�����̸�ǶȲ�Ҫ���󣬳�ͼ��߽硣����������㡣
��3��ʹ�õõ���ͼ�񣬱���10�ţ�һ�����5�š�ʹ��Opencv2.0�ĳ����������궨����ȡ�ڲκͻ���ϵ������
��4��ʹ���ڲκͻ���ϵ���������ͼ�������


#define CALIB //�򿪴˺���Ϊ�궨������Ϊ����

#ifdef CALIB
    int numBoards = 10; // ͼ�����
    int board_w = 8;   // ���̸�X������Ӹ���-1
    int board_h = 6;   // ���̸�Y������Ӹ���-1

    Code:  https://bbs.csdn.net/topics/391855140

  2 ���������ͷ�ı궨��
    �������ֱ����opencv�����sample����opencv/sources/sample/cpp���棬�и�calibration.cpp���ļ������ǵ�Ŀ�ı궨���ǿ���ֱ�ӱ���ʹ�õģ�����Ҫע�⼸�㣺

       1.����
       ����Ҳ���Ǳ궨����ҪԤ�ȴ�ӡ�õģ����ӡ�����̵���ʽ�����˺����������д������Ҫ��Ҳ���Ǻ��Ͻ����������þ��С�֮������������Ϊ�����ǵ�ܷ��㣬and..��û��ѡ����

       2. ����
       һ������Ϊ������ӣ�-w 6 -h 8 -s 2 -n 10 -o camera.yml -op -oe [<list_of_views.txt>]  �����Ǽ�����Ҫ�����ĺ��壺

                   -w <board_width>         # ͼƬĳһά�����ϵĽ������
                   -h <board_height>        # ͼƬ��һά�ϵĽ������
                   [-n <number_of_frames>]  # �궨�õ�ͼƬ֡��
                   [-s <square_size>]       # square size in some user-defined units (1 by default)
                   [-o <out_camera_params>] # the output filename for intrinsic [and extrinsic] parameters
                   [-op]                    # write detected feature points
                   [-oe]                    # write extrinsic parameters


        ���Է��� -w -h�����̵ĳ��͸ߣ�Ҳ�����м����ڰ׽��㣬-s��ÿ�����ӵĳ��ȣ���λ��cm
        ���͸�һ��Ҫ���ԣ���Ȼ������ʶ��ǵ��ʱ���ʶ�𲻳����ġ�

       #https://www.cnblogs.com/daihengchen/p/5492729.html
*/
