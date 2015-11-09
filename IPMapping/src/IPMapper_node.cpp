//
//  IPMapper.cpp
//  TestExtendedDetector
//
/**
Copyright (c)
Audi Autonomous Driving Cup. Team MomenTUM . All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software must display the following acknowledgement: “This product includes software developed by the Audi AG and its contributors for Audi Autonomous Driving Cup.”
4. Neither the name of Audi nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AUDI AG AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AUDI AG OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 **/

/***************************************************************************
 * $Author:: Zahra Boroujeni $  $Date:: 2015-11-05  $*
 ***************************************************************************/
#include <iostream>

#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;
#include "IPMapper.h"
#include <image_transport/image_transport.h>
#define PROJECTED_IMAGE_HEIGTH 250
#define PAINT_OUTPUT

class online_IPM
{
  private:
    // the node handle
    ros::NodeHandle nh_;
    // Node handle in the private namespace
    ros::NodeHandle priv_nh_;
    // subscribers
    ros::Subscriber read_images_;

    // publishers
    image_transport::ImageTransport it;
    image_transport::Publisher pub_mapped_images_;

    IPMapper ipMapper;

  public:

    // 
 
    // constructor
    online_IPM(ros::NodeHandle nh) : nh_(nh), priv_nh_("~"),ipMapper(640,480),it(nh_)
    {
        read_images_= nh_.subscribe(nh_.resolveName("/camera/rgb/image_rect_color"), 1000,&online_IPM::publish_remapper,this);
        pub_mapped_images_= it.advertise("camera/ground_image", 1);
        //ipMapper.initialize(200,PROJECTED_IMAGE_HEIGTH, fu, fv, cx, cy, pitch);
    }

    // callback functions
    void publish_remapper(const sensor_msgs::Image::ConstPtr& msg);


    //! Empty stub
    ~online_IPM() {}

};

void online_IPM::publish_remapper(const sensor_msgs::Image::ConstPtr& msg)
{
    // VideoInput
    std::cout << "Hey, listen!" << std::endl;
  
    //ROS_INFO("CERTAINTY:");
    try
    {
        cv_bridge::CvImagePtr cv_ptr;
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::MONO8);
        Mat gray;
        gray = cv_ptr->image.clone();
        Mat remapped = gray.clone();
        remapped=ipMapper.remap(gray);
        #ifdef PAINT_OUTPUT
            cv::imshow("foo", remapped);
            cv::imshow("foo1", gray);
            cv::waitKey(1);
        #endif
        sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(),sensor_msgs::image_encodings::MONO8, remapped).toImageMsg();
        pub_mapped_images_.publish(msg);
    } 
    catch (const cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "online_IPM");
    ros::NodeHandle nh;
    online_IPM node(nh);

    while(ros::ok())
    {
        ros::spinOnce();

    }

    return 0;
}