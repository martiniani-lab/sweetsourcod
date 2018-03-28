////////////////////////////////////////////////////////////////////////////////
// SA_streamer.cpp
//   Implements a simple class that allows streaming a sequence of integers
//   from file.
////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Juha Karkkainen, Dominik Kempa and Simon J. Puglisi
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#ifndef __SA_STREAMER_H
#define __SA_STREAMER_H

#include <cstdio>
#include <cstdlib>

#include <string>

struct SA_streamer {
  SA_streamer(std::string fname) {
    f = fopen(fname.c_str(), "r");
    if (!f) { perror(fname.c_str()); exit(1); }
    buf = new int[bufsize + 10];
    pos = 0;
    left = fread(buf, sizeof(int), bufsize, f);
  }
  
  inline int read() {
    if (!left) {
      pos = 0;
      left = fread(buf, sizeof(int), bufsize, f);
    }
    --left;
    return buf[pos++];
  }
  
  ~SA_streamer() {
    delete[] buf;
    fclose(f);
  }

  static const int bufsize = 1 << 15;
  int *buf, left, pos;

  FILE *f;
};

#endif // __SA_STREAMER_H
