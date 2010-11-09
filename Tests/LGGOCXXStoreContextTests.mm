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


#include <string.h>

#import "LGGOGraph.h"
#import "LGGOGraphContext.h"
#import "LGGONumber.h"
#import "LGGOString.h"

#include "LGGOCXXStore.h"
#include "LGGOCXXStoreContext.h"

#import "LGGOCXXStoreContextTests.h"

@implementation LGGOCXXStoreContextTests

#if 0
- (void) testContextCreation {
  char path[32];
  strlcpy(path, "/tmp/GraphObjects.XXXXXX", 32);
  (void)mktemp(path);
  LGGOCXXSharedStore store = LGGOCXXSharedStore(new LGGOCXXStore(path));
  LGGOCXXSharedStoreContext context = LGGOCXXSharedStoreContext(store->newContext());
}
#endif

- (void) testObjectUniquing {
  LGGOGraph *graph = [[LGGOGraph alloc] initWithURL:[NSURL URLWithString:@"file:///tmp/"]];
  LGGOGraphContext *context = [graph newContext];
  
  LGGONumber *testNumber1 = [[LGGONumber alloc] initWithNumber:[NSNumber numberWithInt:2] inContext:context];
  LGGONumber *testNumber2 = [[LGGONumber alloc] initWithNumber:[NSNumber numberWithInt:2] inContext:context];
  STAssertTrue(testNumber1 == testNumber2, nil);
  
  LGGOString *testString1 = [[LGGOString alloc] initWithString:@"ABC" inContext:context];
  LGGOString *testString2 = [[LGGOString alloc] initWithString:@"ABC" inContext:context];
  STAssertTrue(testString1 == testString2, nil);
  
  [testNumber1 release];
  [testNumber2 release];
  [testString1 release];
  [testString2 release];
  
  [context release];
  [graph release];
}

@end
