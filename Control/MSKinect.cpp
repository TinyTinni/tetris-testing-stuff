#if KINECT_SDK_VERSION == 1

#include "MSKinect.h"

#include <Windows.h>
#include <NuiApi.h>

#include <boost/scope_exit.hpp>

#include <utility/Logger.h>
#include <utility/Projection.h>

#include "../Data/QuadEntity.h"

const size_t cameraWidth = 640;
const size_t cameraHeight = 480;

struct KinectImpl
{
    INuiSensor* sensor;
    
    HANDLE colorEvent;
    HANDLE colorStreamHandle;

    HANDLE skeletonEvent;
    HANDLE skeletonStreamHandle;
};

KinectControl::KinectControl(Controller& cont)
    :m_controller(cont), m_impl(0)
{   
    connect();
}

bool KinectControl::connect()
{
    int iSensorCount = 0;
    HRESULT hr = NuiGetSensorCount(&iSensorCount);
    if (FAILED(hr))
    {
        util::trace << "Could not find Kinect Device" << std::flush;
        return false;
    }

    util::trace << "Found " << iSensorCount << " Kinect Sensors." << std::flush;

    INuiSensor* pNuiSensor;
    // Look at each Kinect sensor
    for (int i = 0; i < iSensorCount; ++i)
    {
        // Create the sensor so we can check status, if we can't create it, move on to the next
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr))
            continue;

        // Get the status of the sensor, and if connected, then we can initialize it
        hr = pNuiSensor->NuiStatus();
        if (FAILED(hr))
        {
            // This sensor wasn't OK, so release it since we're not using it
            pNuiSensor->Release();
        }
        else
        {
            m_impl = new KinectImpl();
            m_impl->sensor = pNuiSensor;
            break;
        }       
    }

    if (!m_impl || !m_impl->sensor)
        return false;

    // Initialize the Kinect and specify that we'll be using color
    hr = m_impl->sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_SKELETON);
    if (FAILED(hr))
        return false;


    // Create an event that will be signaled when color data is available
    m_impl->colorEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // Open a color image stream to receive color frames
    hr = m_impl->sensor->NuiImageStreamOpen(
        NUI_IMAGE_TYPE_COLOR,
        NUI_IMAGE_RESOLUTION_640x480,
        0,
        2,
        m_impl->colorEvent,
        &m_impl->colorStreamHandle);

    if (FAILED(hr))
        return false;

    m_impl->skeletonEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    hr = m_impl->sensor->NuiSkeletonTrackingEnable(m_impl->skeletonEvent, NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT);

    if (FAILED(hr))
        return false;


    util::trace << "Kinect initalized" << std::endl;

    return true;
}

KinectControl::~KinectControl()
{
    if (!m_impl)
        return;

    if (m_impl->sensor)
    {
        m_impl->sensor->NuiShutdown();
        m_impl->sensor->Release();
    }

    delete m_impl;
}

void KinectControl::pollEvents()
{
    if (!m_impl)
        return;

    // Wait for 0ms, just quickly test if it is time to process a skeleton
    if (WAIT_OBJECT_0 == WaitForSingleObject(m_impl->skeletonEvent, 0))
    {
        updateSkeleton();
    }
}

void KinectControl::updateImages()
{
    
}

void KinectControl::updateSkeleton()
{
    NUI_SKELETON_FRAME skeletonFrame = { 0 };

    HRESULT hr = m_impl->sensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
    if (FAILED(hr))
    {
        return;
    }

    // smooth out the skeleton data
    m_impl->sensor->NuiTransformSmooth(&skeletonFrame, NULL);

    for (int i = 0; i < NUI_SKELETON_COUNT; ++i)
    {
        const NUI_SKELETON_DATA& skeleton = skeletonFrame.SkeletonData[i];
        if (skeleton.eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_HAND_RIGHT] != NUI_SKELETON_NOT_TRACKED)
        {
            long x, y;
            unsigned short d;
            NuiTransformSkeletonToDepthImage(skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT], &x, &y, &d);
            int screenPointX = int(x * globalProjection().width() / 320);
            int screenPointY = int(y * globalProjection().height() / 240);

            static int lastX = screenPointX;
            static int lastY = screenPointY;

            //std::cout << d << std::endl;
            

            float3 sidel;
            float3 origin;
            origin[0] = origin[1] = 0.f;
            sidel[0] = sidel[1] = Quad::sideLength();
            origin[2] = sidel[2] = 2.5f;

            float2 pos2d = globalProjection().project(sidel);
            float2 origin2d = globalProjection().project(origin);
            pos2d -= origin2d;
            if (lastX - screenPointX >= pos2d[0])
            {
                m_controller.translateLeft();
                lastX = screenPointX;
            }

            if (screenPointX - lastX >= pos2d[0])
            {
                m_controller.translateRight();
                lastX = screenPointX;
            }

            //lastX = screenPointX;
            //lastY = screenPointY;
        }

    }
}

void KinectControl::updateColorStream()
{
    HRESULT hr;
    NUI_IMAGE_FRAME imageFrame;

    // Attempt to get the color frame
    hr = m_impl->sensor->NuiImageStreamGetNextFrame(m_impl->colorStreamHandle, 0, &imageFrame);
    if (FAILED(hr))
    {
        return;
    }

    INuiFrameTexture * pTexture = imageFrame.pFrameTexture;
    NUI_LOCKED_RECT LockedRect;

    // Lock the frame data so the Kinect knows not to modify it while we're reading it
    pTexture->LockRect(0, &LockedRect, NULL, 0);

    BOOST_SCOPE_EXIT_ALL(&, this)
    {
        pTexture->UnlockRect(0);
        this->m_impl->sensor->NuiImageStreamReleaseFrame(m_impl->colorStreamHandle, &imageFrame);
    };

    // Make sure we've received valid data
    if (LockedRect.Pitch)
        return;

    //save data
    //// Draw the data with Direct2D
    //m_pDrawColor->Draw(static_cast<BYTE *>(LockedRect.pBits), LockedRect.size);

    //// If the user pressed the screenshot button, save a screenshot
    //if (m_bSaveScreenshot)
    //{
    //    WCHAR statusMessage[cStatusMessageMaxLen];

    //    // Retrieve the path to My Photos
    //    WCHAR screenshotPath[MAX_PATH];
    //    GetScreenshotFileName(screenshotPath, _countof(screenshotPath));

    //    // Write out the bitmap to disk
    //    hr = SaveBitmapToFile(static_cast<BYTE *>(LockedRect.pBits), cColorWidth, cColorHeight, 32, screenshotPath);

    //    if (SUCCEEDED(hr))
    //    {
    //        // Set the status bar to show where the screenshot was saved
    //        StringCchPrintf(statusMessage, cStatusMessageMaxLen, L"Screenshot saved to %s", screenshotPath);
    //    }
    //    else
    //    {
    //        StringCchPrintf(statusMessage, cStatusMessageMaxLen, L"Failed to write screenshot to %s", screenshotPath);
    //    }

    //    SetStatusMessage(statusMessage);

    //    // toggle off so we don't save a screenshot again next frame
    //    m_bSaveScreenshot = false;
    //}
}

void KinectControl::updateDepthStream()
{

}

#endif