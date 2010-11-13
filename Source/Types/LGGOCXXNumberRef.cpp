/* Copyright (c) 2010 GLsoft.mobi
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
 */

#include "LGGOCXXNumberRef.h"

#define LGGO_NUMBER_DATAMASK ((uint64_t)0x07ffffffffffffff)

#define LGGO_MAX_INT59 ((int64_t)0x03ffffffffffffff)
#define LGGO_MIN_INT59 ((int64_t)(-1*LGGO_MAX_INT59))

LGGOCXXNumberRef::LGGOCXXNumberRef(int64_t N) : LGGOCXXReference(), rawValue(N) {
  rawValue = *((uint64_t *)&N);
  
  if (N >= 0) {
    if (N <= LGGO_MAX_INT59) {
      if (N > UINT32_MAX) {
        type = kLGGOCXX64BitUnsignedNumberType;
      } else if (N > UINT16_MAX) {
        type = kLGGOCXX32BitUnsignedNumberType;
      } else if (N > UINT8_MAX) {
        type = kLGGOCXX16BitUnsignedNumberType;
      } else {
        type = kLGGOCXX8BitUnsignedNumberType;
      } 
      
    } else {
      assert(0 && "Deal with oversized ints");
    }      
  } else {
    if (N >= LGGO_MIN_INT59) {
      if (N < INT32_MIN) {
        type = kLGGOCXX64BitSignedNumberType;
      } else if (N < INT16_MIN) {
        type = kLGGOCXX32BitSignedNumberType;
      } else if (N < INT16_MIN) {
        type = kLGGOCXX16BitSignedNumberType;
      } else {
        type = kLGGOCXX8BitSignedNumberType;
      } 
    } else {
      assert(0 && "Deal with oversized ints");
    }
  }
}

LGGOCXXScalarEncodingType LGGOCXXNumberRef::getNumberType(void) {
  return type;
}

uint64_t LGGOCXXNumberRef::getTagValue (void) {
  switch(type) {
    case kLGGOCXX64BitUnsignedNumberType:
    case kLGGOCXX32BitUnsignedNumberType:
    case kLGGOCXX16BitUnsignedNumberType:
    case kLGGOCXX8BitUnsignedNumberType:
      return ((uint64_t)kLGGOAddressNumberType) | ((uint64_t)LGGOCXXIntegerTypeEncoding << 4) | ((uint64_t)rawValue << 5);
    case kLGGOCXX64BitSignedNumberType:
    case kLGGOCXX32BitSignedNumberType:
    case kLGGOCXX16BitSignedNumberType:
    case kLGGOCXX8BitSignedNumberType:
      return ((uint64_t)kLGGOAddressNumberType) | ((uint64_t)LGGOCXXIntegerTypeEncoding << 4) | ((*((int64_t *)&rawValue)) << 5) | (uint64_t)1 << 63;
    default:
      assert(0);
      break;
  }
}

LGGOCXXSharedMemoryDescriptor LGGOCXXNumberRef::getSerializedData(void) {
  return NULL_DESCRIPTOR;
}

int64_t LGGOCXXNumberRef::signedValue (void) {
  return *((int64_t *)&rawValue);
}

uint64_t LGGOCXXNumberRef::unsignedValue (void) {
  return *((uint64_t *)&rawValue);
}

float LGGOCXXNumberRef::floatValue(void) {
  assert(0);
  return 0.0;
}

double LGGOCXXNumberRef::doubleValue(void) {
  assert(0);
  return 0.0;
}