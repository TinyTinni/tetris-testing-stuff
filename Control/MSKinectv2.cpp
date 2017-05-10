#if KINECT_SDK_VERSION == 2

#include "MSKinect.h"

#include <Kinect.h>

struct KinectImpl
{

};

KinectControl::KinectControl(Controller& cont)
    :m_controller(cont), m_impl(0)
{
    connect();
}

bool KinectControl::connect()
{
    return false;
}

KinectControl::~KinectControl()
{

}

void KinectControl::pollEvents()
{

}

#endif