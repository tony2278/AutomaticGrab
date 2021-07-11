
/*
 * 1 为什么要做摄像头标定？
 *   标定的目的是为了消除畸变以及得到内外参数矩阵
 *
 * 2 如何做摄像头的标定？
 * 步骤：

（1）制作棋盘格，打印Opencv自带的9*7的棋盘格到A4纸上。棋盘格方格大小为27*27mm。
（2）对准相机晃动棋盘格。得到多张图像。注意棋盘格角度不要过大，出图像边界。距离相机近点。
（3）使用得到的图像，比如10张，一般大于5张。使用Opencv2.0的程序进行相机标定。获取内参和畸变系数矩阵。
（4）使用内参和畸变系数矩阵进行图像矫正。


#define CALIB //打开此宏则为标定，否则为矫正

#ifdef CALIB
    int numBoards = 10; // 图像个数
    int board_w = 8;   // 棋盘格X方向格子个数-1
    int board_h = 6;   // 棋盘格Y方向格子个数-1

    Code:  https://bbs.csdn.net/topics/391855140

  2 如何做摄像头的标定？
    这里可以直接用opencv里面的sample，在opencv/sources/sample/cpp里面，有个calibration.cpp的文件，这是单目的标定，是可以直接编译使用的，这里要注意几点：

       1.棋盘
       棋盘也就是标定板是要预先打印好的，你打印的棋盘的样式决定了后面参数的填写，具体要求也不是很严谨，清晰能用就行。之所用棋盘是因为他检测角点很方便，and..你没得选。。

       2. 参数
       一般设置为这个样子：-w 6 -h 8 -s 2 -n 10 -o camera.yml -op -oe [<list_of_views.txt>]  ，这是几个重要参数的含义：

                   -w <board_width>         # 图片某一维方向上的交点个数
                   -h <board_height>        # 图片另一维上的交点个数
                   [-n <number_of_frames>]  # 标定用的图片帧数
                   [-s <square_size>]       # square size in some user-defined units (1 by default)
                   [-o <out_camera_params>] # the output filename for intrinsic [and extrinsic] parameters
                   [-op]                    # write detected feature points
                   [-oe]                    # write extrinsic parameters


        可以发现 -w -h是棋盘的长和高，也就是有几个黑白交点，-s是每个格子的长度，单位是cm
        长和高一定要数对，不然程序在识别角点的时候会识别不出来的。

       #https://www.cnblogs.com/daihengchen/p/5492729.html
*/
