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

#include "LGGOCXXReference.h"
#include "LGGOCXXStore.h"
#include "LGGOCXXStoreSegment.h"
#include "LGGOCXXMemoryDescriptor.h"

#include "LGGOCXXStringRef.h"
#include "LGGOCXXNumberRef.h"

#include "LGGOCXXClassRef.h"

#include "LGGOCXXHackArrayRef.h"

#include "LGGOCXXStoreContext.h"

LGGOCXXStoreContext::LGGOCXXStoreContext(const LGGOCXXSharedStore &S)
  : store(S), nextAddressValue(256) {
}

uint64_t LGGOCXXStoreContext::getNextFreeAddress (void) {
  uint64_t retval = nextAddressValue<<4;
  nextAddressValue++;
  return retval;
}

LGGOCXXSharedReference LGGOCXXStoreContext::createArray(void) {
  return LGGOCXXSharedReference(shared_from_this(), new LGGOCXXHackArrayRef());
}

LGGOCXXSharedReference LGGOCXXStoreContext::createString(std::string S) {
  return LGGOCXXSharedReference (shared_from_this(), new LGGOCXXStringRef(S));
}

LGGOCXXSharedReference LGGOCXXStoreContext::createNumber(int64_t N) {
  return LGGOCXXSharedReference (shared_from_this(), new LGGOCXXNumberRef(N));
}


LGGOCXXSharedReference LGGOCXXStoreContext::createClass(std::string C) {
  return LGGOCXXSharedReference (shared_from_this(), new LGGOCXXClassRef(C));
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

LGGOCXXReference * LGGOCXXStoreContext::rootObject(void) {
  LGGOCXXSharedReference address(getReferenceForAddress(1<<4));
  return *address;
}

void LGGOCXXStoreContext::setRootObject(LGGOCXXReference *T) {
  LGGOCXXSharedReference address(shared_from_this(), 1<<4);
  address.setReference(T);
}

LGGOCXXSharedReference LGGOCXXStoreContext::getReferenceForAddress (uint64_t address) {
  std::map<uint64_t,LGGOCXXWeakReference>::iterator i = addresses.find(address);
  
  if (i == addresses.end()) {
    return LGGOCXXSharedReference(i->second);
  } else {
    assert(0);
    LGGOCXXSharedReference retval = LGGOCXXSharedReference(shared_from_this(), address);
    setReferenceForAddress(LGGOCXXWeakReference(retval), address);
    return retval;
  }
};

void LGGOCXXStoreContext::setReferenceForAddress(const LGGOCXXWeakReference& reference, uint64_t address) {
  addresses.insert(std::pair<uint64_t, LGGOCXXWeakReference>(address, reference));
}

LGGOCXXSharedReference LGGOCXXStoreContext::getAddressForAddressValue(uint64_t addressValue) {
  std::map<uint64_t,LGGOCXXWeakReference>::iterator i = addresses.find(addressValue);

  if (i != addresses.end()) {
    return LGGOCXXSharedReference(i->second);
  }
  
  return LGGOCXXSharedReference();
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