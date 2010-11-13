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

#include "LGGOCXXHackArrayRef.h"
#include "LGGOCXXObjectRef.h"

#import "LGGOGraphContext.h"
#import "LGGOMutableArray.h"

@interface LGGOMutableArray () {
  LGGOCXXSharedReference reference;
  LGGOGraphContext *graphContext;
}

@property (nonatomic, readonly) LGGOCXXSharedReference reference;

@end

@implementation LGGOMutableArray

- (id)initWithGraphObject:(const LGGOCXXSharedReference &)graphObject inContext:(LGGOGraphContext *)context_ {
	self = [super init];
  id retval;
  
  if (self) {
    reference = graphObject;
    
    id existingObject = (id)reference->getNativeObject();
    
    if (existingObject) {
      [self release];
      retval = [existingObject retain];
    } else {
      graphContext = [context_ retain];
      reference->setNativeObject(self);
    }
  }
  
  return retval;
}

- (id)initInContext:(LGGOGraphContext *)context_ {
  self = [super init];
  
  if (self) {
    {
      //LLVM/Clang 2.8 compiler bug work around
      LGGOCXXSharedStoreContext context = context_.CXXContext;
      reference = context->createArray();
    }
    graphContext = [context_ retain];
    reference->setNativeObject(self);
  }
  
  return self;
}

- (void)dealloc {
  if (reference.isValid()) {
    reference->setNativeObject(NULL);
  }
  [graphContext release];
    
  [super dealloc];
}

- (NSUInteger)count {
  return dynamic_cast<LGGOCXXHackArrayRef *>(*reference)->getCount();
}

- (id)objectAtIndex:(NSUInteger)index {
  LGGOCXXSharedReference typedObject = dynamic_cast<LGGOCXXHackArrayRef *>(*reference)->getObjectAtIndex(index);
  id retval = (id)typedObject->getNativeObject();
  
  if (!retval) {
    retval = [graphContext transmuteToNativeObject:typedObject];
    typedObject->setNativeObject(retval);
  }
  return retval;
}

- (void)addObject:(id)anObject {
  LGGOCXXSharedReference objectAddress = [graphContext transmuteToGraphObject:anObject];
  
  dynamic_cast<LGGOCXXHackArrayRef *>(*reference)->addObject(objectAddress);
}

- (void)insertObject:(id)anObject atIndex:(NSUInteger)index {
  LGGOCXXSharedReference objectAddress = [graphContext transmuteToGraphObject:anObject];
  
  dynamic_cast<LGGOCXXHackArrayRef *>(*reference)->insertObjectAtIndex(objectAddress, index);
}

- (void)removeLastObject {
  dynamic_cast<LGGOCXXHackArrayRef *>(*reference)->removeLastObject();
}

- (void)removeObjectAtIndex:(NSUInteger)index {
  dynamic_cast<LGGOCXXHackArrayRef *>(*reference)->removeObjectAtIndex(index);
}

- (void)replaceObjectAtIndex:(NSUInteger)index withObject:(id)anObject {
  LGGOCXXSharedReference objectAddress = [graphContext transmuteToGraphObject:anObject];

  dynamic_cast<LGGOCXXHackArrayRef *>(*reference)->replaceObjectAtIndexWithObject(objectAddress, index);
}

@end
