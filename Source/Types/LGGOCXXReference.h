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

virtual class LGGOCXXReference;

#ifndef LGGOCXXREFERENCE_H
#define LGGOCXXREFERENCE_H

#include <string>

#include "LGGOCXXStoreContext.h"
#include "LGGOCXXMemoryDescriptor.h"

#include "LGGOCXXReferenceMetadata.h"

//FIXME should we support _Bool/c++ bool encoding?

//This is the base type for all the C++ graph object types
//Includes books for mapping to native objects (handled in the native objects context)

typedef enum {
  kLGGOCXX8BitSignedNumberType = 0,
  kLGGOCXX8BitUnsignedNumberType = 1,
  kLGGOCXX16BitSignedNumberType = 2,
  kLGGOCXX16BitUnsignedNumberType = 3,
  kLGGOCXX32BitSignedNumberType = 4,
  kLGGOCXX32BitUnsignedNumberType = 5,
  kLGGOCXX64BitSignedNumberType = 6,
  kLGGOCXX64BitUnsignedNumberType = 7,
  kLGGOCXXFloatNumberType = 8,
  kLGGOCXXDoubleNumberType = 9,
} LGGOCXXScalarEncodingType;

typedef enum {
  kLGGOCXXClassRefReferenceType = 0,
  kLGGOCXXClassRefArrayType = 1,
  kLGGOCXXClassRefSetType = 2,
  kLGGOCXXClassRefDictionaryType = 3
} LGGOCXXRelationEncodingType;

typedef enum {
  kLGGOCXXClassRefByValueType = 0,
  kLGGOCXXClassRefByReferenceType = 1
} LGGOCXXRelationSemanticsType;

class LGGOCXXReference {
private:
  LGGOCXXWeakReference metadata;
  void *clientData;
public:
  explicit LGGOCXXReference(void);
  
  LGGOCXXWeakReference getMetadata(void);
  void setMetadata (const LGGOCXXWeakReference &R);
  
  const LGGOCXXSharedStoreContext& getContext(void);

  void *getNativeObject(void);
  void setNativeObject(void *D);
  
  virtual LGGOCXXSharedMemoryDescriptor getSerializedData (void) = 0;
  virtual uint64_t getTagValue (void) = 0;
};

#endif