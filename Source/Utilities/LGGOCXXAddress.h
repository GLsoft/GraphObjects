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

class LGGOCXXSharedAddress;
class LGGOCXXWeakAddress;

#include "LGGOCXXType.h"
#include "LGGOCXXStoreContext.h"

#ifndef LGGOADDRESS_H
#define LGGOADDRESS_H

#define LGGOCXXADDRESS_SERIALIZED_SIZE (8)

typedef enum {
  kLGGOAddressNativeType = 0,
  kLGGOAddressConstantType = 1,
  kLGGOAddressNumberType = 3,
  kLGGOAddressStringType = 4,
  kLGGOAddressDateType = 5,
  kLGGOAddressArrayType = 6,
  kLGGOAddressComplexType = 15
} LGGOSimpleType;

#include <stdint.h>

//FIXME we can be stupid clever here and half the size by using a union, but it is tricky

class LGGOCXXAddress {
private:
  uint64_t address;
  LGGOCXXSharedStoreContext context;
  LGGOCXXType *type;
  uint32_t strongRefCount;
  uint32_t weakRefCount;
  bool dirty:1;
  bool dusty:1;
public:
  explicit LGGOCXXAddress(const LGGOCXXSharedStoreContext& C, LGGOCXXType *T, uint64_t A);
  explicit LGGOCXXAddress(const LGGOCXXSharedStoreContext& C, LGGOCXXType *T);
  const uint64_t getAddressValue (void);
  
  const LGGOSimpleType getType (void) const;
  const bool isNative (void) const;
  const bool isValid (void) const;
  
  uint64_t getConcreteAddressValue (void) const;
  
  void incrementStrongCount (void);
  void decrementStrongCount (void);
  
  void incrementWeakCount (void);
  void decrementWeakCount (void);
  
  const LGGOCXXSharedStoreContext& getContext(void);

  bool operator== (const LGGOCXXAddress& A);
  bool operator> (const LGGOCXXAddress& b) const;
  bool operator< (const LGGOCXXAddress& b) const;
  
  LGGOCXXType * getType (void);
  void setType (LGGOCXXType *T);
  
  bool getDirty (void);
  void setDirty (bool D);
  bool getDusty (void);
  void setDusty (bool D);
  
  friend class LGGOCXXSharedAddress;
  friend class LGGOCXXWeakAddress;
};
class LGGOCXXSharedAddress {
private:
  LGGOCXXAddress *address;
public:
  explicit LGGOCXXSharedAddress(const LGGOCXXWeakAddress& A);
  LGGOCXXSharedAddress(const LGGOCXXSharedAddress& A);
  LGGOCXXSharedAddress(const LGGOCXXSharedStoreContext& C, uint64_t A);
  LGGOCXXSharedAddress(const LGGOCXXSharedStoreContext& C, LGGOCXXType *T);
  LGGOCXXSharedAddress(void);
  ~LGGOCXXSharedAddress(void);
  
  bool getDirty (void);
  void setDirty (bool D);
  bool getDusty (void);
  void setDusty (bool D);
  
  
  const LGGOCXXSharedStoreContext& getContext(void);
  uint64_t getAddressValue(void);
  
  LGGOCXXType * getType (void);
  
  void setType (LGGOCXXType *T);
  bool isValid (void);
  
  LGGOCXXType * operator* (void) const;
  LGGOCXXType * operator-> (void) const;
  
  LGGOCXXSharedAddress& operator= (const LGGOCXXSharedAddress& A);
  
  friend class LGGOCXXWeakAddress;
};

class LGGOCXXWeakAddress {
private:
  LGGOCXXAddress *address;
public:
  LGGOCXXWeakAddress(void);
  explicit LGGOCXXWeakAddress(const LGGOCXXSharedAddress& A);
  explicit LGGOCXXWeakAddress(LGGOCXXAddress *A);
  LGGOCXXWeakAddress(const LGGOCXXWeakAddress&A);
  ~LGGOCXXWeakAddress(void);
  
  bool getDirty (void);
  void setDirty (bool D);
  bool getDusty (void);
  void setDusty (bool D);
    
  const LGGOCXXSharedStoreContext& getContext(void);
  uint64_t getAddressValue(void);
  bool isValid (void);
  
  LGGOCXXWeakAddress& operator= (const LGGOCXXWeakAddress& A);
  
  friend class LGGOCXXSharedAddress;
};




#endif