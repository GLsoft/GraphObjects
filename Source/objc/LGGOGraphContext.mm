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

#include "LGGOCXXStoreContext.h"
#include "LGGOCXXString.h"
#include "LGGOCXXNumber.h"
#include "LGGOCXXHackArray.h"

#import "LGGOGraphContext.h"
#import "LGGOString.h"
#import "LGGONumber.h"
#import "LGGOMutableArray.h"

@protocol LGGOCXXSharedAddress

@property (nonatomic, readonly) LGGOCXXSharedAddress address;

@end

@interface LGGOGraphContext () {
  LGGOCXXSharedStoreContext CXXContext;
}
@end 

@implementation LGGOGraphContext

- (id)initWithCXXContext:(const LGGOCXXSharedStoreContext&)context_  {
  self = [super init];
  
    if (self) {
      CXXContext = context_;
    }
    
    return self;
}

- (id) rootObject {
  return nil;
}

- (void)setRootObject:(id)rootObject {
  
}


// This function returns a graph object from a normal object. It will grab the underlying
// graph object, or make one (without an attached native object) if necessary.

- (id) transmuteToNativeObject:(LGGOCXXSharedAddress)address {
  id retval = (id)(*address)->getNativeObject();
  
  if (!retval) {
    LGGOCXXHackArray *lggoArray = dynamic_cast<LGGOCXXHackArray *>(*address);
    if (lggoArray) {
      return [[[LGGOMutableArray alloc] initWithGraphObject:address inContext:self] autorelease];
    }
    
    LGGOCXXString *lggoString = dynamic_cast<LGGOCXXString *>(*address);
    if (lggoString) {
      return [[[LGGOString alloc] initWithGraphObject:address inContext:self] autorelease];
    }
    
    LGGOCXXNumber *lggoNumber = dynamic_cast<LGGOCXXNumber *>(*address);
    if (lggoNumber) {
      return [[[LGGONumber alloc] initWithGraphObject:address inContext:self] autorelease];
    }
  }
  
  return retval;
}

- (LGGOCXXSharedAddress) transmuteToGraphObject:(id)object {
  LGGOCXXSharedAddress retval;
  
  if ([object respondsToSelector:@selector(address)]) {
    id<LGGOCXXSharedAddress> lggoObject = object;
    retval = [lggoObject address];
    retval->setNativeObject(object);
  } else if ([object isKindOfClass:[NSString class]]) {
    NSString *string = object;
    retval = LGGOCXXSharedAddress(self.CXXContext, new LGGOCXXString(string.UTF8String));
    retval->setNativeObject(object);
  } else if ([object isKindOfClass:[NSNumber class]]) {
    //FIXME this needs improvement for floats
    NSNumber *number = object;
    LGGOCXXSharedAddress temp(self.CXXContext, new LGGOCXXNumber(number.longLongValue));
    retval = temp;
    retval->setNativeObject(object);
  } else if ([object isKindOfClass:[NSArray class]]) {
    NSArray *array = object;
    retval = LGGOCXXSharedAddress(self.CXXContext, new LGGOCXXHackArray());
    LGGOCXXHackArray *lggoArray = dynamic_cast<LGGOCXXHackArray *>(*retval);
    [array enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
      lggoArray->addObject([self transmuteToGraphObject:obj]);
    }];
    retval->setNativeObject(object);
  } else {
    assert(0);
  }
  
  return retval;
}

@end
