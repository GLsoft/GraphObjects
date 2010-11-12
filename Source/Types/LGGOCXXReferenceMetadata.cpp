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

#include "LGGOCXXReferenceMetadata.h"

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

LGGOCXXReferenceMetadata::LGGOCXXReferenceMetadata(const LGGOCXXSharedStoreContext& C, LGGOCXXReference *T)
    : context(C), strongRefCount(0), weakRefCount(0), dirty(false), dusty(false), type(T) {
  //Weak declare this weakAddr at top of scope to guarantee the addr is not deleted during setup
  LGGOCXXWeakReference weakAddr(this);
  T->setAddress(weakAddr);
  address = context->getNextFreeAddress();
  context->setAddressForAddressValue(LGGOCXXWeakReference(this), address);
  LG_LOG("Create 0x%llx real address\n", getAddressValue());
  assert(type->getAddress().getAddressValue() == address);
}

LGGOCXXReferenceMetadata::LGGOCXXReferenceMetadata(const LGGOCXXSharedStoreContext& C, LGGOCXXReference *T, uint64_t A)
    : context(C), strongRefCount(0), weakRefCount(0), dirty(false), dusty(false), type(T), address(A) {
  //Weak declare this weakAddr at top of scope to guarantee the addr is not deleted during setup
  LGGOCXXWeakReference weakAddr(this);
  T->setAddress(weakAddr);
  context->setAddressForAddressValue(LGGOCXXWeakReference(this), address);
  LG_LOG("Create 0x%llx real address\n", getAddressValue());
  assert(type->getAddress().getAddressValue() == address);
  assert(type->getAddress().getAddressValue() == A);
}

const uint64_t LGGOCXXReferenceMetadata::getAddressValue (void) {
  return address;
}
  
const LGGOSimpleType LGGOCXXReferenceMetadata::getType (void) const {
  return static_cast<LGGOSimpleType>(address | (uint64_t)0x00ff);
}

const bool LGGOCXXReferenceMetadata::isNative (void) const {
  return ((address | (uint64_t)0x00ff) == kLGGOAddressNativeType);
}

const bool LGGOCXXReferenceMetadata::isValid (void) const {
  return address != 0;
}
  
  
uint64_t LGGOCXXReferenceMetadata::getConcreteAddressValue (void) const {
  return address;
}
  
void LGGOCXXReferenceMetadata::incrementStrongCount (void) {
  LG_LOG("Counts:%u/%u 0x%lx\n", strongRefCount, weakRefCount, (uintptr_t)this);
  LG_BACKTRACE();
  strongRefCount++;
}
  
void LGGOCXXReferenceMetadata::decrementStrongCount (void) {
  LG_LOG("Counts:%u/%u 0x%lx\n", strongRefCount, weakRefCount, (uintptr_t)this);
  LG_BACKTRACE();
  assert(strongRefCount > 0);
  strongRefCount--;
  
  if (strongRefCount == 0 && weakRefCount == 0) {
    delete this;
  }
}
  
void LGGOCXXReferenceMetadata::incrementWeakCount (void) {
  LG_LOG("Counts:%u/%u 0x%lx\n", strongRefCount, weakRefCount, (uintptr_t)this);
  LG_BACKTRACE();
  weakRefCount++;
}
  
void LGGOCXXReferenceMetadata::decrementWeakCount (void) {
  LG_LOG("Counts:%u/%u 0x%lx\n", strongRefCount, weakRefCount, (uintptr_t)this);
  LG_BACKTRACE();
  assert(weakRefCount > 0);
  weakRefCount--;

  if (strongRefCount == 0 && weakRefCount == 0) {
    delete this;
  }
}
  
  
bool LGGOCXXReferenceMetadata::operator== (const LGGOCXXReferenceMetadata& A) {
  return (address == A.address);
}
  
bool LGGOCXXReferenceMetadata::operator> (const LGGOCXXReferenceMetadata& b) const {
  return address > b.address;
}
  
bool LGGOCXXReferenceMetadata::operator< (const LGGOCXXReferenceMetadata& b) const {
  return address < b.address;
}

const LGGOCXXSharedStoreContext& LGGOCXXReferenceMetadata::getContext(void) {
  return context;
}

LGGOCXXReference * LGGOCXXReferenceMetadata::getType (void) {
  return type;
}

void LGGOCXXReferenceMetadata::setType (LGGOCXXReference *T) {
  type = T;
}

bool LGGOCXXReferenceMetadata::getDirty (void) {
  return dirty;
}

void LGGOCXXReferenceMetadata::setDirty (bool D) {
  dirty = D;
}

bool LGGOCXXReferenceMetadata::getDusty (void) {
  return dusty;
}

void LGGOCXXReferenceMetadata::setDusty (bool D) {
  dusty = D;
}

bool LGGOCXXSharedReference::getDirty (void) {
  return address->getDirty();
}

void LGGOCXXSharedReference::setDirty (bool D) {
  address->setDirty(D);
}

bool LGGOCXXSharedReference::getDusty (void) {
  return address->getDusty();
}

void LGGOCXXSharedReference::setDusty (bool D) {
  address->setDusty(D);
}

#pragma mark -
#pragma mark Strong Pointer Implementation

LGGOCXXSharedReference::LGGOCXXSharedReference(void) : address(NULL) {
  //printf("Create NULL strong address\n");
}

LGGOCXXSharedReference::LGGOCXXSharedReference(const LGGOCXXWeakReference& A) : address(A.address) {
  if (address) {
    address->incrementStrongCount();
  }
}
  
LGGOCXXSharedReference::LGGOCXXSharedReference(const LGGOCXXSharedReference& A)  : address(A.address) {
  if (address) {
    address->incrementStrongCount();
  }
}

LGGOCXXSharedReference::LGGOCXXSharedReference(const LGGOCXXSharedStoreContext& C, uint64_t A) {
  assert(A != 0);
  LGGOCXXSharedReference existingAddress = C->getAddressForAddressValue(A);
  if (existingAddress.isValid()) {
    address = existingAddress.address;
    address->incrementStrongCount();
  } else {
    assert(0);
  }
  assert(address->getAddressValue() == A);
  //printf("Create 0x%llx strong address\n", address->getAddressValue());
}

LGGOCXXSharedReference::LGGOCXXSharedReference(const LGGOCXXSharedStoreContext& C, LGGOCXXReference *T) {
  uint64_t tagValue = T->getTagValue();
  
  if (tagValue) {
    LGGOCXXSharedReference existingAddress = C->getAddressForAddressValue(tagValue);
    if (existingAddress.isValid()) {
      address = existingAddress.address;
    } else {
      address = new LGGOCXXReferenceMetadata(C, T, tagValue);
    }
    assert(address->getAddressValue() == tagValue);
    //printf("Create 0x%llx strong address\n", address->getAddressValue());
  } else {
    address = new LGGOCXXReferenceMetadata(C, T);
  }
  address->incrementStrongCount();
  //printf("Create 0x%llx strong address\n", address->getAddressValue());
}


LGGOCXXSharedReference::~LGGOCXXSharedReference(void) {
  if (address) {
    address->decrementStrongCount();
  }
}

const LGGOCXXSharedStoreContext& LGGOCXXSharedReference::getContext(void) {
  assert(address != NULL);
  return address->getContext();
}

uint64_t LGGOCXXSharedReference::getAddressValue(void) {
  return address->getAddressValue();
}

LGGOCXXReference *LGGOCXXSharedReference::getType (void) {
  assert(address != NULL);
  return address->getType();
}

void LGGOCXXSharedReference::setType (LGGOCXXReference *T) {
  assert(address != NULL);
  address->setType(T);
}

bool LGGOCXXSharedReference::isValid (void) {
  return (address != NULL);
}

LGGOCXXReference *LGGOCXXSharedReference::operator* (void) const {
  assert(address != NULL);
  return address->getType();
}

LGGOCXXReference *LGGOCXXSharedReference::operator-> (void) const {
  assert(address != NULL);
  return address->getType();
}

LGGOCXXSharedReference& LGGOCXXSharedReference::operator= (const LGGOCXXSharedReference& A) {
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

LGGOCXXWeakReference::LGGOCXXWeakReference(void) : address(NULL) {
//  printf("Create NULL weak address\n");
}

LGGOCXXWeakReference::LGGOCXXWeakReference(LGGOCXXReferenceMetadata *A) : address(A) {
//  printf("Create 0x%llx weak address\n", A->getAddressValue());
  if (address) {
    address->incrementWeakCount();
  }
}


LGGOCXXWeakReference::LGGOCXXWeakReference(const LGGOCXXWeakReference& A) : address(A.address) {
  if (address) {
    address->incrementWeakCount();
  }
}
  
LGGOCXXWeakReference::LGGOCXXWeakReference(const LGGOCXXSharedReference& A) : address(A.address) {
  if (address) {
    address->incrementWeakCount();
  }
}

LGGOCXXWeakReference::~LGGOCXXWeakReference(void) {
  if (address) {
    address->decrementWeakCount();
  }
}

const LGGOCXXSharedStoreContext& LGGOCXXWeakReference::getContext(void) {
  assert(address != NULL);
  return address->getContext();
}

uint64_t LGGOCXXWeakReference::getAddressValue(void) {
  return address->getAddressValue();
}

bool LGGOCXXWeakReference::isValid (void) {
  return (address != NULL);
}

LGGOCXXWeakReference& LGGOCXXWeakReference::operator= (const LGGOCXXWeakReference& A) {
  if (address) {
    address->decrementWeakCount();
  }
  address = A.address;
  if (address) {
    address->incrementWeakCount();
  }
  return *this;
}

bool LGGOCXXWeakReference::getDirty (void) {
  return address->getDirty();
}

void LGGOCXXWeakReference::setDirty (bool D) {
  address->setDirty(D);
}

bool LGGOCXXWeakReference::getDusty (void) {
  return address->getDusty();
}

void LGGOCXXWeakReference::setDusty (bool D) {
  address->setDusty(D);
}
