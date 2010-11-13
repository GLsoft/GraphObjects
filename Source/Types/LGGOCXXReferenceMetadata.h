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

class LGGOCXXSharedReference;
class LGGOCXXWeakReference;

#include "LGGOCXXReference.h"
#include "LGGOCXXStoreContext.h"

#ifndef LGGOREFERENCEMETADATA_H
#define LGGOREFERENCEMETADATA_H

#define LGGOCXXReferenceMetadata_SERIALIZED_SIZE (8)

typedef enum {
  kLGGOAddressNativeType = 0,
  kLGGOAddressConstantType = 1,
  kLGGOAddressNumberType = 3,
  kLGGOAddressStringType = 4,
  kLGGOAddressDateType = 5,
  kLGGOAddressArrayType = 6,
  kLGGOAddressComplexType = 15,
  kLGGOAddressMutableArray = (kLGGOAddressComplexType | 1<<4),
  kLGGOAddressMutableSet = (kLGGOAddressComplexType | 2<<4),
  kLGGOAddressMutableDictionary = (kLGGOAddressComplexType | 3<<4)
} LGGOSimpleType;

#include <stdint.h>

//FIXME we can be stupid clever here and half the size by using a union, but it is tricky

class LGGOCXXReferenceMetadata {
private:
  uint64_t address;
  LGGOCXXWeakStoreContext context;
  LGGOCXXReference *reference;
  uint32_t strongRefCount;
  uint32_t weakRefCount;
  bool dirty:1;
  bool dusty:1;
  
public:
  explicit LGGOCXXReferenceMetadata(const LGGOCXXSharedStoreContext& C, LGGOCXXReference *R, uint64_t A);
  explicit LGGOCXXReferenceMetadata(const LGGOCXXSharedStoreContext& C, LGGOCXXReference *R);
  const uint64_t getAddressValue (void);
  
  const LGGOSimpleType getType (void) const;
  const bool isNative (void) const;
  const bool isValid (void) const;
  
  uint64_t getConcreteAddressValue (void) const;
  
  void incrementStrongCount (void);
  void decrementStrongCount (void);
  
  void incrementWeakCount (void);
  void decrementWeakCount (void);
  
  LGGOCXXSharedStoreContext getContext(void);

  bool operator== (const LGGOCXXReferenceMetadata& A);
  bool operator> (const LGGOCXXReferenceMetadata& b) const;
  bool operator< (const LGGOCXXReferenceMetadata& b) const;
  
  LGGOCXXReference * getReference (void);
  void setReference (LGGOCXXReference *R);
  
  bool getDirty (void);
  void setDirty (bool D);
  bool getDusty (void);
  void setDusty (bool D);
  
  friend class LGGOCXXSharedReference;
  friend class LGGOCXXWeakReference;
};

class LGGOCXXSharedReference {
private:
  LGGOCXXReferenceMetadata *metadata;
public:
  explicit LGGOCXXSharedReference(const LGGOCXXWeakReference& A);
  LGGOCXXSharedReference(const LGGOCXXSharedReference& A);
  LGGOCXXSharedReference(const LGGOCXXSharedStoreContext& C, uint64_t A);
  LGGOCXXSharedReference(const LGGOCXXSharedStoreContext& C, LGGOCXXReference *T);
  LGGOCXXSharedReference(void);
  ~LGGOCXXSharedReference(void);
  
  bool getDirty (void);
  void setDirty (bool D);
  bool getDusty (void);
  void setDusty (bool D);
  
  
  LGGOCXXSharedStoreContext getContext(void);
  uint64_t getAddressValue(void);
  
  LGGOCXXReference * getReference (void);
  
  void setReference (LGGOCXXReference *T);
  bool isValid (void);
  
  LGGOCXXReference * operator* (void) const;
  LGGOCXXReference * operator-> (void) const;
  
  LGGOCXXSharedReference& operator= (const LGGOCXXSharedReference& A);
  
  friend class LGGOCXXWeakReference;
};

class LGGOCXXWeakReference {
private:
  LGGOCXXReferenceMetadata *metadata;
public:
  LGGOCXXWeakReference(void);
  explicit LGGOCXXWeakReference(const LGGOCXXSharedReference& A);
  explicit LGGOCXXWeakReference(LGGOCXXReferenceMetadata *A);
  LGGOCXXWeakReference(const LGGOCXXWeakReference&A);
  ~LGGOCXXWeakReference(void);
  
  bool getDirty (void);
  void setDirty (bool D);
  bool getDusty (void);
  void setDusty (bool D);
    
  LGGOCXXSharedStoreContext getContext(void);
  uint64_t getAddressValue(void);
  bool isValid (void);
  
  LGGOCXXWeakReference& operator= (const LGGOCXXWeakReference& A);
  
  friend class LGGOCXXSharedReference;
};




#endif