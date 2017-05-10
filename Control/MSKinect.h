#pragma once

#include <boost/noncopyable.hpp>
#include "Controller.h"

struct KinectImpl;

#ifdef USE_KINECTSDK
#define ADD_EMPTY ;
#else
#define ADD_EMPTY {}
#endif

#ifdef USE_KINECTSDK
#define CONSTRUCTOR ;
#else
#define CONSTRUCTOR :m_controller(cont) {}
#endif

class KinectControl : boost::noncopyable
{
    KinectImpl* m_impl;
    Controller& m_controller;

    /// updates kinect streams
    void updateImages() ADD_EMPTY
    /// updates color stream
    void updateColorStream() ADD_EMPTY
    /// updates color stream
    void updateDepthStream() ADD_EMPTY
    /// updates skeleton positions
    void updateSkeleton() ADD_EMPTY

public:
    /// tries to connect to the kinect device. returns true, if connection was successful, otherwise false
    bool connect() ADD_EMPTY
    KinectControl(Controller& cont) CONSTRUCTOR
    void pollEvents() ADD_EMPTY
    ~KinectControl() ADD_EMPTY
    bool connected() { return !m_impl; }
};

#ifndef USE_KINECTSDK
struct KinectImpl
{
};
#endif

#undef ADD_EMPTY
#undef CONSTRUCTOR
