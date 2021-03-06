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

#ifndef __USBLINK_H__
#define __USBLINK_H__

#include "link.h"
#include "utils/timer.hpp"
#include <libusb.h>
#include <stdexcept>

class USBLink : public Link
{
public:
    USBLink();
    ~USBLink();

    int open(uint index);
    virtual int send(const uint8_t *data, uint32_t len, uint16_t timeout_ms);
    virtual int receive(uint8_t *data, uint32_t len, uint16_t timeout_ms);
    virtual void setTimer();
    virtual uint32_t getTimer();

private:
    libusb_context *m_context;
    libusb_device **m_devices;
    libusb_device_handle *m_handle;

    ssize_t get_device_list(libusb_device ***list);
    libusb_device *get_pixy_using_index(libusb_device **list, ssize_t size, uint index);

    util::timer timer_;
};

#endif

