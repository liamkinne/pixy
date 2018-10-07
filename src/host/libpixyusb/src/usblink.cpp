//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//

#include <unistd.h>
#include <stdio.h>
#include "usblink.h"
#include "pixy.h"
#include "utils/timer.hpp"
#include "debuglog.h"

#define __LINUX__

USBLink::USBLink()
{
  m_handle = 0;
  m_context = 0;
  m_blockSize = 64;
  m_flags = LINK_FLAG_ERROR_CORRECTED;
}

USBLink::~USBLink()
{
  if (m_handle)
    libusb_close(m_handle);
  if (m_context)
    libusb_exit(m_context);
}

int USBLink::open(uint index)
{
  int return_value;
#ifdef __MACOS__
  const unsigned int MILLISECONDS_TO_SLEEP = 100;
#endif

  log("pixydebug: USBLink::open()\n");

  return_value = libusb_init(&m_context);
  log("pixydebug:  libusb_init() = %d\n", return_value);

  if (return_value < 0) {
    throw std::runtime_error("Error during libusb_init");
  }

  ssize_t device_count = get_device_list(&m_devices);

  libusb_device* m_device = get_pixy_using_index(m_devices, device_count, index);

  return_value = libusb_open(m_device, &m_handle);
  log("pixydebug:  libusb_open() = %d\n", return_value);

  if (m_handle == NULL) {
    throw std::runtime_error("Error usb device not found");
  }

#ifdef __MACOS__
  return_value = libusb_reset_device(m_handle);
  log("pixydebug:  libusb_reset_device() = %d\n", return_value);
  usleep(MILLISECONDS_TO_SLEEP * 1000);
#endif

  return_value = libusb_set_configuration(m_handle, 1);
  log("pixydebug:  libusb_set_configuration() = %d\n", return_value);

  if (return_value < 0) {
    libusb_close(m_handle);
    throw std::runtime_error("failed to set configuration");
  }

  return_value = libusb_claim_interface(m_handle, 1);
  log("pixydebug:  libusb_claim_interface() = %d\n", return_value);

  if (return_value < 0) {
    libusb_close(m_handle);
    throw std::runtime_error("failed to claim interface");
  }

#ifdef __LINUX__
  return_value = libusb_reset_device(m_handle);
  log("pixydebug:  libusb_reset_device() = %d\n", return_value);
#endif

  /* Success */
  return_value = 0;
  log("pixydebug: USBLink::open() returned %d\n", return_value);

  return return_value;
}



int USBLink::send(const uint8_t *data, uint32_t len, uint16_t timeout_ms)
{
    int response, transferred;
    
    log("pixydebug: USBLink::send()\n");

    if (timeout_ms==0) // 0 equals infinity
        timeout_ms = 10;

    response = libusb_bulk_transfer(m_handle, 0x02, (unsigned char *)data, len, &transferred, timeout_ms);

    if (response < 0)
    {
        log("pixydebug:  libusb_bulk_transfer() = %d\n", response);
#ifdef __MACOS__
        libusb_clear_halt(m_handle, 0x02);
#endif
        log("pixydebug: USBLink::send() returned %d\n", response);
        return response;
    }
    
    log("pixydebug: USBLink::send() returned %d\n", transferred);
    return transferred;
}

int USBLink::receive(uint8_t *data, uint32_t len, uint16_t timeout_ms)
{
    int response, transferred;

    log("pixydebug: USBLink::receive()\n");

    if (timeout_ms==0) // 0 equals infinity
        timeout_ms = 50;

    response = libusb_bulk_transfer(m_handle, 0x82, (unsigned char *)data, len, &transferred, timeout_ms);

    if (response < 0)
    {
        log("pixydebug:  libusb_bulk_transfer() = %d\n", response);
#ifdef __MACOS__
        libusb_clear_halt(m_handle, 0x82);
#endif
        return response;
    }
    
    log("pixydebug:  libusb_bulk_transfer(%d bytes) = %d\n", len, response);
    log("pixydebug: USBLink::receive() returned %d (bytes transferred)\n", transferred);
    return transferred;
}

void USBLink::setTimer()
{
  timer_.reset();
}

uint32_t USBLink::getTimer()
{
  return timer_.elapsed();
}

ssize_t USBLink::get_device_list(libusb_device ***list) {
  ssize_t device_count = libusb_get_device_list(m_context, list);

  if (device_count < 0) {
    throw std::runtime_error("Get device error");
  }
  else if (device_count == 0) {
    throw std::runtime_error("No devices found");
  }

  log("pixydebug:  found %d devices\n", device_count);

  return device_count;
}

libusb_device* USBLink::get_pixy_using_index(libusb_device **list, ssize_t size, uint index) {
  int error_code;

  for (ssize_t i = 0; i < size; i++) {
    libusb_device_descriptor descriptor;
    error_code = libusb_get_device_descriptor(list[i], &descriptor);

    if (error_code < 0) {
      throw std::runtime_error("failed to get device descriptor");
    }

    if (descriptor.idVendor == PIXY_VID and descriptor.idProduct == PIXY_PID) {
      if (index == 0) {
        return list[i];
      }
      else {
        index--;
      }
    }
  }

  throw std::runtime_error("failed to find a Pixy device with the given index");
}
