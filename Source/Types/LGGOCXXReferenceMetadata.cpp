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

LGGOCXXReferenceMetadata::LGGOCXXReferenceMetadata(const LGGOCXXSharedStoreContext& C, LGGOCXXReference *R)
    : context(C), strongRefCount(0), weakRefCount(0), dirty(false), dusty(false), reference(R) {
  //Weak declare this weakAddr at top of scope to guarantee the addr is not deleted during setup
  LGGOCXXWeakReference weakAddr(this);
  R->setMetadata(weakAddr);
  address = C->getNextFreeAddress();
  C->setReferenceForAddress(LGGOCXXWeakReference(this), address);
  LG_LOG("Create 0x%llx real address\n", getAddressValue());
  assert(reference->getMetadata().getAddressValue() == address);
}

LGGOCXXReferenceMetadata::LGGOCXXReferenceMetadata(const LGGOCXXSharedStoreContext& C, LGGOCXXReference *R, uint64_t A)
    : context(C), strongRefCount(0), weakRefCount(0), dirty(false), dusty(false), reference(R), address(A) {
  //Weak declare this weakAddr at top of scope to guarantee the addr is not deleted during setup
  LGGOCXXWeakReference weakAddr(this);
  R->setMetadata(weakAddr);
  C->setReferenceForAddress(LGGOCXXWeakReference(this), address);
  LG_LOG("Create 0x%llx real address\n", getAddressValue());
  assert(reference->getMetadata().getAddressValue() == address);
  assert(reference->getMetadata().getAddressValue() == A);
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
  
  if (strongRefCount == 0) {
    delete reference;
    
    if (weakRefCount == 0) {
//      delete this;
    }
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
  
  
bool LGGOCXXReferenceMetadata::operator== (const LGGOCXXReferenceMetadata& R) {
  return (address == R.address);
}
  
bool LGGOCXXReferenceMetadata::operator> (const LGGOCXXReferenceMetadata& b) const {
  return address > b.address;
}
  
bool LGGOCXXReferenceMetadata::operator< (const LGGOCXXReferenceMetadata& b) const {
  return address < b.address;
}

LGGOCXXSharedStoreContext LGGOCXXReferenceMetadata::getContext(void) {
  if (context.expired()) {
    return LGGOCXXSharedStoreContext();
  } else {
    return LGGOCXXSharedStoreContext(context);

  }
}

LGGOCXXReference * LGGOCXXReferenceMetadata::getReference (void) {
  return reference;
}

void LGGOCXXReferenceMetadata::setReference (LGGOCXXReference *R) {
  reference = R;
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
  return metadata->getDirty();
}

void LGGOCXXSharedReference::setDirty (bool D) {
  metadata->setDirty(D);
}

bool LGGOCXXSharedReference::getDusty (void) {
  return metadata->getDusty();
}

void LGGOCXXSharedReference::setDusty (bool D) {
  metadata->setDusty(D);
}

#pragma mark -
#pragma mark Strong Pointer Implementation

LGGOCXXSharedReference::LGGOCXXSharedReference(void) : metadata(NULL) {
  //printf("Create NULL strong address\n");
}

LGGOCXXSharedReference::LGGOCXXSharedReference(const LGGOCXXWeakReference& R) : metadata(R.metadata) {
  if (metadata) {
    metadata->incrementStrongCount();
  }
}
  
LGGOCXXSharedReference::LGGOCXXSharedReference(const LGGOCXXSharedReference& R)  : metadata(R.metadata) {
  if (metadata) {
    metadata->incrementStrongCount();
  }
}

LGGOCXXSharedReference::LGGOCXXSharedReference(const LGGOCXXSharedStoreContext& C, uint64_t A) {
  assert(A != 0);
  LGGOCXXSharedReference existingAddress = C->getAddressForAddressValue(A);
  if (existingAddress.isValid()) {
    metadata = existingAddress.metadata;
    metadata->incrementStrongCount();
  } else {
    assert(0);
  }
  assert(metadata->getAddressValue() == A);
  //printf("Create 0x%llx strong address\n", address->getAddressValue());
}

LGGOCXXSharedReference::LGGOCXXSharedReference(const LGGOCXXSharedStoreContext& C, LGGOCXXReference *T) {
  uint64_t tagValue = T->getTagValue();
  
  if (tagValue) {
    LGGOCXXSharedReference existingAddress = C->getAddressForAddressValue(tagValue);
    if (existingAddress.isValid()) {
      metadata = existingAddress.metadata;
    } else {
      metadata = new LGGOCXXReferenceMetadata(C, T, tagValue);
    }
    assert(metadata->getAddressValue() == tagValue);
    //printf("Create 0x%llx strong address\n", address->getAddressValue());
  } else {
    metadata = new LGGOCXXReferenceMetadata(C, T);
  }
  metadata->incrementStrongCount();
  //printf("Create 0x%llx strong address\n", address->getAddressValue());
}


LGGOCXXSharedReference::~LGGOCXXSharedReference(void) {
  if (metadata) {
    metadata->decrementStrongCount();
  }
}

LGGOCXXSharedStoreContext LGGOCXXSharedReference::getContext(void) {
  assert(metadata != NULL);
  return metadata->getContext();
}

uint64_t LGGOCXXSharedReference::getAddressValue(void) {
  return metadata->getAddressValue();
}

LGGOCXXReference *LGGOCXXSharedReference::getReference (void) {
  assert(metadata != NULL);
  return metadata->getReference();
}

void LGGOCXXSharedReference::setReference (LGGOCXXReference *R) {
  assert(metadata != NULL);
  metadata->setReference(R);
}

bool LGGOCXXSharedReference::isValid (void) {
  return (metadata != NULL);
}

LGGOCXXReference *LGGOCXXSharedReference::operator* (void) const {
  assert(metadata != NULL);
  return metadata->getReference();
}

LGGOCXXReference *LGGOCXXSharedReference::operator-> (void) const {
  assert(metadata != NULL);
  return metadata->getReference();
}

LGGOCXXSharedReference& LGGOCXXSharedReference::operator= (const LGGOCXXSharedReference& R) {
  if (metadata) {
    metadata->decrementStrongCount();
  }
  metadata = R.metadata;
  if (metadata) {
    metadata->incrementStrongCount();
  }
  return *this;
}

#pragma mark -
#pragma mark Weak Pointer Implementation

LGGOCXXWeakReference::LGGOCXXWeakReference(void) : metadata(NULL) {
//  printf("Create NULL weak address\n");
}

LGGOCXXWeakReference::LGGOCXXWeakReference(LGGOCXXReferenceMetadata *MD) : metadata(MD) {
//  printf("Create 0x%llx weak address\n", A->getAddressValue());
  if (metadata) {
    metadata->incrementWeakCount();
  }
}


LGGOCXXWeakReference::LGGOCXXWeakReference(const LGGOCXXWeakReference& MD) : metadata(MD.metadata) {
  if (metadata) {
    metadata->incrementWeakCount();
  }
}
  
LGGOCXXWeakReference::LGGOCXXWeakReference(const LGGOCXXSharedReference& MD) : metadata(MD.metadata) {
  if (metadata) {
    metadata->incrementWeakCount();
  }
}

LGGOCXXWeakReference::~LGGOCXXWeakReference(void) {
  if (metadata) {
    metadata->decrementWeakCount();
  }
}

LGGOCXXSharedStoreContext LGGOCXXWeakReference::getContext(void) {
  assert(metadata != NULL);
  return metadata->getContext();
}

uint64_t LGGOCXXWeakReference::getAddressValue(void) {
  return metadata->getAddressValue();
}

bool LGGOCXXWeakReference::isValid (void) {
  return (metadata != NULL);
}

LGGOCXXWeakReference& LGGOCXXWeakReference::operator= (const LGGOCXXWeakReference& R) {
  if (metadata) {
    metadata->decrementWeakCount();
  }
  metadata = R.metadata;
  if (metadata) {
    metadata->incrementWeakCount();
  }
  return *this;
}

bool LGGOCXXWeakReference::getDirty (void) {
  return metadata->getDirty();
}

void LGGOCXXWeakReference::setDirty (bool D) {
  metadata->setDirty(D);
}

bool LGGOCXXWeakReference::getDusty (void) {
  return metadata->getDusty();
}

void LGGOCXXWeakReference::setDusty (bool D) {
  metadata->setDusty(D);
}
