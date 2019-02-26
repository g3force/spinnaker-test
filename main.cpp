#include <iostream>
#include <spinnaker/SystemPtr.h>
#include <spinnaker/CameraPtr.h>
#include <spinnaker/ImagePtr.h>

using namespace Spinnaker;

int main(int argc, const char **argv) {
  auto streamBufferHandlingMode = StreamBufferHandlingMode_OldestFirstOverwrite;
  if (argc > 1) {
    if (strcmp(argv[1], "NewestOnly") == 0) {
      streamBufferHandlingMode = StreamBufferHandlingMode_NewestOnly;
    } else if (strcmp(argv[1], "NewestFirst") == 0) {
      streamBufferHandlingMode = StreamBufferHandlingMode_NewestFirst;
    } else {
      fprintf(stderr, "Illegal parameter: %s\n", argv[1]);
      return 1;
    }
  }

  SystemPtr pSystem = System::GetInstance();

  CameraList camList = pSystem->GetCameras();
  printf("Number of cams: %u\n", camList.GetSize());

  CameraPtr pCam = camList.GetByIndex(0);
  pCam->Init();
  camList.Clear();

  printf("Opened %s - %s\n", pCam->DeviceModelName.GetValue().c_str(), pCam->DeviceSerialNumber.GetValue().c_str());

  pCam->TriggerMode.SetValue(Spinnaker::TriggerMode_Off);
  pCam->AcquisitionMode.SetValue(Spinnaker::AcquisitionMode_Continuous);

  pCam->ExposureAuto.SetValue(Spinnaker::ExposureAuto_Off);
  pCam->ExposureTime.SetValue(15000);

  // reference: https://www.ptgrey.com/tan/11174
  pCam->TLStream.StreamBufferHandlingMode.SetValue(streamBufferHandlingMode);
  pCam->TLStream.StreamBufferCountMode.SetValue(Spinnaker::StreamBufferCountMode_Manual);
  pCam->TLStream.StreamBufferCountManual.SetValue(3);

  try {
    pCam->PixelFormat.SetValue(Spinnaker::PixelFormat_BayerRG8);
  }
  catch (Spinnaker::Exception &e) {
    fprintf(stderr, "An error occurred while setting pixel format (error code: %d, '%s')\n", e.GetError(),
            e.GetFullErrorMessage());
  }

  pCam->BeginAcquisition();
  pCam->EndAcquisition();

  try {
    pCam->PixelFormat.SetValue(Spinnaker::PixelFormat_BayerRG8);
  }
  catch (Spinnaker::Exception &e) {
    fprintf(stderr, "An error occurred while setting pixel format (error code: %d, '%s')\n", e.GetError(),
            e.GetFullErrorMessage());
  }

  pCam->BeginAcquisition();

  printf("fps: %.2f\n", pCam->AcquisitionResultingFrameRate.GetValue());

  int sleepMs = 30;
  int durationMS = 300000;
  int iterations = durationMS / sleepMs;
  for (int i = 0; i < iterations; i++) {
    printf("Get %d\n", i);
    ImagePtr pImage = pCam->GetNextImage();
    printf("Received %d\n", i);
    pImage->Release();
    printf("Released %d/%d\n", i, iterations);
    usleep(static_cast<__useconds_t>(sleepMs * 1000));
  }

  pCam->EndAcquisition();
  pCam->DeInit();
  pCam = nullptr;
  pSystem->ReleaseInstance();

  return 0;
}