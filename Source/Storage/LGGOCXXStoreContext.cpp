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

#include <unistd.h>
#include <fcntl.h>
#include <vector>

#include <sstream>

#include "LGGOCXXType.h"
#include "LGGOCXXStore.h"
#include "LGGOCXXStoreSegment.h"
#include "LGGOCXXMemoryDescriptor.h"

#include "LGGOCXXStoreContext.h"

LGGOCXXStoreContext::LGGOCXXStoreContext(const LGGOCXXSharedStore &S)
  : store(S), nextAddressValue(256) {
}

uint64_t LGGOCXXStoreContext::getNextFreeAddress (void) {
  uint64_t retval = nextAddressValue<<4;
  nextAddressValue++;
  return retval;
}

//FIXME implement transient save here

void LGGOCXXStoreContext::writeSegment (void) {
#if 0
  std::ostringstream path;
  
  LGGOCXXWritableStoreSegment writableSegment;
  
  int fd = open(store->getStorePath().c_str(), O_WRONLY | O_CREAT);/  LGGOCXXSharedMemoryDescriptor descriptor = writableSegment.serializeToMemory();
  path << store->getStorePath() << "/" << descriptor->getHash()->getHexValue();
  
  
  if (fd > 0) {
    write(fd, descriptor->getData(), descriptor->getSize());
    LGGOCXXSharedMemoryDescriptor newDescriptor = LGGOCXXSharedMemoryDescriptor(new LGGOCXXMappedMemoryDescriptor(path.str()));
    LGGOCXXSharedStoreSegment newSegment = LGGOCXXSharedStoreSegment(new LGGOCXXStoreSegment(newDescriptor));
    segments.push_back(newSegment);
  } else {
    assert(0);
  }
#endif
  assert(0);
}

void LGGOCXXStoreContext::commit (void) {
  //FIXME we need to use a locking protocol here
}

LGGOCXXType * LGGOCXXStoreContext::rootObject(void) {
  LGGOCXXSharedAddress address(getAddress(1<<4));
  return *address;
}

void LGGOCXXStoreContext::setRootObject(LGGOCXXType *T) {
  LGGOCXXSharedAddress address(shared_from_this(), 1<<4);
  address.setType(T);
}

LGGOCXXSharedAddress LGGOCXXStoreContext::getAddress (uint64_t address) {
  std::map<uint64_t,LGGOCXXWeakAddress>::iterator i = addresses.find(address);
  
  if (i == addresses.end()) {
    return LGGOCXXSharedAddress(i->second);
  } else {
    assert(0);
    LGGOCXXSharedAddress retval = LGGOCXXSharedAddress(shared_from_this(), address);
    setAddressForAddressValue(LGGOCXXWeakAddress(retval), address);
    return retval;
  }
};

void LGGOCXXStoreContext::setAddressForAddressValue(const LGGOCXXWeakAddress& address, uint64_t addressValue) {
//   addresses[addressValue] = address;
  addresses.insert(std::pair<uint64_t, LGGOCXXWeakAddress>(addressValue, address));
}

LGGOCXXSharedAddress LGGOCXXStoreContext::getAddressForAddressValue(uint64_t addressValue) {
  std::map<uint64_t,LGGOCXXWeakAddress>::iterator i = addresses.find(addressValue);

  if (i != addresses.end()) {
    return LGGOCXXSharedAddress(i->second);
  }
  
  return LGGOCXXSharedAddress();
}

LGGOCXXSharedMemoryDescriptor LGGOCXXStoreContext::getDescriptorForAddress (uint64_t address) {
  LGGOCXXSharedMemoryDescriptor retval;
    
  std::vector<LGGOCXXSharedStoreSegment>::iterator i;
  for (i = segments.begin(); i != segments.end(); ++i) {
    retval = (*i)->getDescriptorForAddress(address);
    
    if (retval.get() != NULL) {
      break;
    }
  }
  
  return retval;
}