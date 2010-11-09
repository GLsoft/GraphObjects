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



#include <tr1/memory>

#include "LGGOCXXAddress.h"

#include <execinfo.h>
#include <stdio.h>

#if 0

#define LG_LOG(fmt, ...) printf(fmt, __VA_ARGS__)

#define LG_BACKTRACE() {                              \
  void* callstack[128];                               \
  int i, frames = backtrace(callstack, 128);          \
  char** strs = backtrace_symbols(callstack, frames); \
  for (i = 0; i < frames; ++i) {                      \
    printf("%s\n", strs[i]);                          \
  }                                                   \
  free(strs);                                         \
}

#else

#define LG_LOG(fmt, ...) while(0)
#define LG_BACKTRACE() while(0)

#endif


#pragma mark -
#pragma mark Control Block Implementation

LGGOCXXAddress::LGGOCXXAddress(const LGGOCXXSharedStoreContext& C, LGGOCXXType *T)
    : context(C), strongRefCount(0), weakRefCount(0), type(T) {
  //Weak declare this weakAddr at top of scope to guarantee the addr is not deleted during setup
  LGGOCXXWeakAddress weakAddr(this);
  T->setAddress(weakAddr);
  address = context->getNextFreeAddress();
  context->setAddressForAddressValue(LGGOCXXWeakAddress(this), address);
  LG_LOG("Create 0x%llx real address\n", getAddressValue());
  assert(type->getAddress().getAddressValue() == address);
}

LGGOCXXAddress::LGGOCXXAddress(const LGGOCXXSharedStoreContext& C, LGGOCXXType *T, uint64_t A)
    : context(C), strongRefCount(0), weakRefCount(0), type(T), address(A) {
  //Weak declare this weakAddr at top of scope to guarantee the addr is not deleted during setup
  LGGOCXXWeakAddress weakAddr(this);
  T->setAddress(weakAddr);
  context->setAddressForAddressValue(LGGOCXXWeakAddress(this), address);
  LG_LOG("Create 0x%llx real address\n", getAddressValue());
  assert(type->getAddress().getAddressValue() == address);
  assert(type->getAddress().getAddressValue() == A);
}

const uint64_t LGGOCXXAddress::getAddressValue (void) {
  return address;
}
  
const LGGOSimpleType LGGOCXXAddress::getType (void) const {
  return static_cast<LGGOSimpleType>(address | (uint64_t)0x00ff);
}

const bool LGGOCXXAddress::isNative (void) const {
  return ((address | (uint64_t)0x00ff) == kLGGOAddressNativeType);
}

const bool LGGOCXXAddress::isValid (void) const {
  return address != 0;
}
  
  
uint64_t LGGOCXXAddress::getConcreteAddressValue (void) const {
  return address;
}
  
void LGGOCXXAddress::incrementStrongCount (void) {
  LG_LOG("Counts:%u/%u 0x%lx\n", strongRefCount, weakRefCount, (uintptr_t)this);
  LG_BACKTRACE();
  strongRefCount++;
}
  
void LGGOCXXAddress::decrementStrongCount (void) {
  LG_LOG("Counts:%u/%u 0x%lx\n", strongRefCount, weakRefCount, (uintptr_t)this);
  LG_BACKTRACE();
  assert(strongRefCount > 0);
  strongRefCount--;
  
  if (strongRefCount == 0 && weakRefCount == 0) {
    delete this;
  }
}
  
void LGGOCXXAddress::incrementWeakCount (void) {
  LG_LOG("Counts:%u/%u 0x%lx\n", strongRefCount, weakRefCount, (uintptr_t)this);
  LG_BACKTRACE();
  weakRefCount++;
}
  
void LGGOCXXAddress::decrementWeakCount (void) {
  LG_LOG("Counts:%u/%u 0x%lx\n", strongRefCount, weakRefCount, (uintptr_t)this);
  LG_BACKTRACE();
  assert(weakRefCount > 0);
  weakRefCount--;

  if (strongRefCount == 0 && weakRefCount == 0) {
    delete this;
  }
}
  
  
bool LGGOCXXAddress::operator== (const LGGOCXXAddress& A) {
  return (address == A.address);
}
  
bool LGGOCXXAddress::operator> (const LGGOCXXAddress& b) const {
  return address > b.address;
}
  
bool LGGOCXXAddress::operator< (const LGGOCXXAddress& b) const {
  return address < b.address;
}

const LGGOCXXSharedStoreContext& LGGOCXXAddress::getContext(void) {
  return context;
}

#pragma mark -
#pragma mark Strong Pointer Implementation

LGGOCXXSharedAddress::LGGOCXXSharedAddress(void) : address(NULL) {
  //printf("Create NULL strong address\n");
}

LGGOCXXSharedAddress::LGGOCXXSharedAddress(const LGGOCXXWeakAddress& A) : address(A.address) {
  if (address) {
    address->incrementStrongCount();
  }
}
  
LGGOCXXSharedAddress::LGGOCXXSharedAddress(const LGGOCXXSharedAddress& A)  : address(A.address) {
  if (address) {
    address->incrementStrongCount();
  }
}

LGGOCXXSharedAddress::LGGOCXXSharedAddress(const LGGOCXXSharedStoreContext& C, uint64_t A) {
  assert(A != 0);
  LGGOCXXSharedAddress existingAddress = C->getAddressForAddressValue(A);
  if (existingAddress.isValid()) {
    address = existingAddress.address;
    address->incrementStrongCount();
  } else {
    assert(0);
  }
  assert(address->getAddressValue() == A);
  //printf("Create 0x%llx strong address\n", address->getAddressValue());
}

LGGOCXXSharedAddress::LGGOCXXSharedAddress(const LGGOCXXSharedStoreContext& C, LGGOCXXType *T) {
  uint64_t tagValue = T->getTagValue();
  
  if (tagValue) {
    LGGOCXXSharedAddress existingAddress = C->getAddressForAddressValue(tagValue);
    if (existingAddress.isValid()) {
      address = existingAddress.address;
    } else {
      address = new LGGOCXXAddress(C, T, tagValue);
    }
    assert(address->getAddressValue() == tagValue);
    //printf("Create 0x%llx strong address\n", address->getAddressValue());
  } else {
    address = new LGGOCXXAddress(C, T);
  }
  address->incrementStrongCount();
  //printf("Create 0x%llx strong address\n", address->getAddressValue());
}


LGGOCXXSharedAddress::~LGGOCXXSharedAddress(void) {
  if (address) {
    address->decrementStrongCount();
  }
}

const LGGOCXXSharedStoreContext& LGGOCXXSharedAddress::getContext(void) {
  assert(address != NULL);
  return address->getContext();
}

uint64_t LGGOCXXSharedAddress::getAddressValue(void) {
  return address->getAddressValue();
}

LGGOCXXType *LGGOCXXSharedAddress::getType (void) {
  assert(address != NULL);
  return address->getType();
}

void LGGOCXXSharedAddress::setType (LGGOCXXType *T) {
  assert(address != NULL);
  address->setType(T);
}

bool LGGOCXXSharedAddress::isValid (void) {
  return (address != NULL);
}

LGGOCXXType *LGGOCXXSharedAddress::operator* (void) const {
  assert(address != NULL);
  return address->getType();
}

LGGOCXXType *LGGOCXXSharedAddress::operator-> (void) const {
  assert(address != NULL);
  return address->getType();
}

LGGOCXXSharedAddress& LGGOCXXSharedAddress::operator= (const LGGOCXXSharedAddress& A) {
  if (address) {
    address->decrementStrongCount();
  }
  address = A.address;
  if (address) {
    address->incrementStrongCount();
  }
  return *this;
}

#pragma mark -
#pragma mark Weak Pointer Implementation

LGGOCXXWeakAddress::LGGOCXXWeakAddress(void) : address(NULL) {
//  printf("Create NULL weak address\n");
}

LGGOCXXWeakAddress::LGGOCXXWeakAddress(LGGOCXXAddress *A) : address(A) {
//  printf("Create 0x%llx weak address\n", A->getAddressValue());
  if (address) {
    address->incrementWeakCount();
  }
}


LGGOCXXWeakAddress::LGGOCXXWeakAddress(const LGGOCXXWeakAddress& A) : address(A.address) {
  if (address) {
    address->incrementWeakCount();
  }
}
  
LGGOCXXWeakAddress::LGGOCXXWeakAddress(const LGGOCXXSharedAddress& A) : address(A.address) {
  if (address) {
    address->incrementWeakCount();
  }
}

LGGOCXXWeakAddress::~LGGOCXXWeakAddress(void) {
  if (address) {
    address->decrementWeakCount();
  }
}

const LGGOCXXSharedStoreContext& LGGOCXXWeakAddress::getContext(void) {
  assert(address != NULL);
  return address->getContext();
}

uint64_t LGGOCXXWeakAddress::getAddressValue(void) {
  return address->getAddressValue();
}

bool LGGOCXXWeakAddress::isValid (void) {
  return (address != NULL);
}

LGGOCXXWeakAddress& LGGOCXXWeakAddress::operator= (const LGGOCXXWeakAddress& A) {
  if (address) {
    address->decrementWeakCount();
  }
  address = A.address;
  if (address) {
    address->incrementWeakCount();
  }
  return *this;
}

