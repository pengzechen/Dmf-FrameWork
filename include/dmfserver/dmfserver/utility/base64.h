/* 
    *  Copyright 2023 Ajax
    *
    *  Licensed under the Apache License, Version 2.0 (the "License");
    *  you may not use this file except in compliance with the License.
    *
    *  You may obtain a copy of the License at
    *
    *    http://www.apache.org/licenses/LICENSE-2.0
    *    
    *  Unless required by applicable law or agreed to in writing, software
    *  distributed under the License is distributed on an "AS IS" BASIS,
    *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    *  See the License for the specific language governing permissions and
    *  limitations under the License. 
    *
    */

#ifndef __DM_BASE64_INCLUDE__ 
#define __DM_BASE64_INCLUDE__  
  
#include <stdlib.h>  
#include <string.h>  
  
unsigned char *base64_encode(unsigned char *str);  
  
unsigned char *base64_decode(unsigned char *code);  
  
#endif  // __DM_BASE64_INCLUDE__