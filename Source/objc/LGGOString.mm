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


#include "LGGOCXXString.h"

#import "LGGOGraphContext.h"
#import "LGGOString.h"


@interface LGGOString () {
  LGGOCXXSharedAddress address;
  LGGOGraphContext *graphContext;
}

@property LGGOCXXSharedAddress address;

@end

@implementation LGGOString

- (id)initWithGraphObject:(const LGGOCXXSharedAddress &)graphObject inContext:(LGGOGraphContext *)context_ {
	self = [super init];
  id retval;
  
  if (self) {
    address = graphObject;
    
    id existingObject = (id)address->getNativeObject();
    
    if (existingObject) {
      [self release];
      retval = [existingObject retain];
    } else {
      graphContext = [context_ retain];
      address->setNativeObject(self);
    }
  }
  
  return retval;
}

- (id)initWithString:(NSString *)string_ inContext:(LGGOGraphContext *)context_ {
	self = [super init];
  id retval;
  
  if (self) {
    address = LGGOCXXSharedAddress(context_.CXXContext, new LGGOCXXString(std::string(string_.UTF8String)));
    
    id existingObject = (id)address->getNativeObject();
    
    if (existingObject) {
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

- (NSUInteger)length {  
  return dynamic_cast<LGGOCXXString *>(*address)->getLength();
}

- (unichar)characterAtIndex:(NSUInteger)index {
  return dynamic_cast<LGGOCXXString *>(*address)->getCharacterAtIndex(index);
}

#if 0
- (void)getCharacters:(unichar *)buffer range:(NSRange)aRange {
  std::tr1::dynamic_pointer_cast<LGGOCXXString>(sharedType)->getCharsInRage(buffer, aRange.location, aRange.length);
}
#endif

@end
