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


#include "LGGOGraphContext.h"


#include "LGGOCXXAddress.h"
#include "LGGOCXXNumber.h"

#import "LGGONumber.h"

@interface LGGONumber () {
  LGGOCXXSharedAddress address;
  LGGOGraphContext *graphContext;
}

@property LGGOCXXSharedAddress address;
@end

@implementation LGGONumber

- (id)initWithGraphObject:(const LGGOCXXSharedAddress &)graphObject inContext:(LGGOGraphContext *)context_ {
	self = [super init];
  id retval;
  
  if (self) {
    address = graphObject;
    
    id existingObject = (id)address->getNativeObject();
    
    if (existingObject) {
      address = LGGOCXXSharedAddress();
      [self release];
      retval = [existingObject retain];
    } else {
      graphContext = [context_ retain];
      address->setNativeObject(self);
    }
  }
  
  return retval;
}


- (id)initWithNumber:(NSNumber *)number_ inContext:(LGGOGraphContext *)context_ {
	self = [super init];
  id retval;
  
  if (self) {
    LGGOCXXSharedType type(new LGGOCXXNumber(number_.longLongValue));
    address = LGGOCXXSharedAddress(context_.CXXContext, type);
    
    id existingObject = (id)address->getNativeObject();
    
    if (existingObject) {
      //We do this here so that the when the ObjC runtime destructs the C++ ivar it has an empty object to chew on
      address = LGGOCXXSharedAddress();
      [self release];
      retval = [existingObject retain];
    } else {
      retval = self;
      graphContext = [context_ retain];
      address->setNativeObject(self);
    }
  }
	
	return retval;
}

- (void) dealloc {
  if (address.isValid()) {
    address->setNativeObject(NULL);
  }
  [graphContext release];
  
  [super dealloc];
}

- (void)getValue:(void *)value_ {
  std::tr1::shared_ptr<LGGOCXXNumber> sharedNumber = std::tr1::dynamic_pointer_cast<LGGOCXXNumber>(*address);
	switch(sharedNumber->getType()) {
		case kLGGOCXX8BitSignedNumberType:
    case kLGGOCXX16BitSignedNumberType:
		case kLGGOCXX32BitSignedNumberType:
      *(int32_t *)value_ = sharedNumber->signedValue();
      break;
    case kLGGOCXX8BitUnsignedNumberType :
    case kLGGOCXX16BitUnsignedNumberType :
    case kLGGOCXX32BitUnsignedNumberType :  
      *(uint32_t *)value_ = sharedNumber->unsignedValue();
      break;
    case kLGGOCXX64BitSignedNumberType :
      *(int64_t *)value_ = sharedNumber->signedValue();;
		case kLGGOCXX64BitUnsignedNumberType :      
      *(uint64_t *)value_ = sharedNumber->unsignedValue();;
      break;      
		default: assert(0); break;
	}
}

#define LGGO_NUMBER_ACCESSOR(datatype, name)                                                                    \
- (datatype)name {                                                                                              \
  std::tr1::shared_ptr<LGGOCXXNumber> sharedNumber = std::tr1::dynamic_pointer_cast<LGGOCXXNumber>(*address);   \
  datatype retval;                                                                                              \
  switch(sharedNumber->getType()) {                                                                             \
    case kLGGOCXX8BitSignedNumberType :                                                                         \
    case kLGGOCXX16BitSignedNumberType :                                                                        \
    case kLGGOCXX32BitSignedNumberType :                                                                        \
    case kLGGOCXX64BitSignedNumberType :  retval = sharedNumber->signedValue(); break;                          \
    case kLGGOCXX8BitUnsignedNumberType :                                                                       \
    case kLGGOCXX16BitUnsignedNumberType :                                                                      \
    case kLGGOCXX32BitUnsignedNumberType :                                                                      \
    case kLGGOCXX64BitUnsignedNumberType :  retval = sharedNumber->unsignedValue(); break;                      \
    default: assert(0); break;                                                                                  \
  }                                                                                                             \
  return retval;                                                                                                \
}

LGGO_NUMBER_ACCESSOR(char, charValue)
LGGO_NUMBER_ACCESSOR(unsigned char, unsignedCharValue)
LGGO_NUMBER_ACCESSOR(short, shortValue)
LGGO_NUMBER_ACCESSOR(unsigned short, unsignedShortValue)
LGGO_NUMBER_ACCESSOR(int, intValue)
LGGO_NUMBER_ACCESSOR(unsigned int, unsignedIntValue)
LGGO_NUMBER_ACCESSOR(long, longValue)
LGGO_NUMBER_ACCESSOR(unsigned long, unsignedLongValue)

LGGO_NUMBER_ACCESSOR(long long, longLongValue)
LGGO_NUMBER_ACCESSOR(unsigned long long, unsignedLongLongValue)
LGGO_NUMBER_ACCESSOR(float, floatValue)
LGGO_NUMBER_ACCESSOR(double, doubleValue)
LGGO_NUMBER_ACCESSOR(BOOL, boolValue)
LGGO_NUMBER_ACCESSOR(NSInteger, integerValue)
LGGO_NUMBER_ACCESSOR(NSUInteger, unsignedIntegerValue)

- (const char *)objCType {
  const char *retval;
  
  switch(std::tr1::dynamic_pointer_cast<LGGOCXXNumber>(*address)->getType()) {                                                       
    case kLGGOCXX8BitSignedNumberType : retval = "c"; break;
    case kLGGOCXX8BitUnsignedNumberType : retval = "C"; break;
    case kLGGOCXX16BitSignedNumberType : retval = "i"; break;
    case kLGGOCXX16BitUnsignedNumberType : retval = "I"; break;
    case kLGGOCXX32BitSignedNumberType : retval = "s"; break;
    case kLGGOCXX32BitUnsignedNumberType : retval = "S"; break;
    case kLGGOCXX64BitSignedNumberType : retval = "q"; break;
    case kLGGOCXX64BitUnsignedNumberType : retval = "Q"; break;
    default: assert(0);
  }
  
  return retval;
}

@end
