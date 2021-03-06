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

#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include <stdint.h>
#include <string>
#include <chrono>

namespace util
{
  class timer 
  {
    public:

      timer();
      
      void     reset();
      uint32_t elapsed();

    private:
    
      std::chrono::steady_clock::time_point epoch_;
  };
}

#endif
