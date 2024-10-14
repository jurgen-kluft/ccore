#ifndef __CCORE_VERSION_H__
#define __CCORE_VERSION_H__

///////////////////////////////////////////////////////////////////////////////
// Version
//
// We more or less follow a conventional packaging approach to versioning 
// here. A primary distinction here is that minor versions are defined as two
// digit entities (e.g. .03") instead of minimal digit entities ".3"). The logic
// here is that the value is a counter and not a floating point fraction.
// Note that the major version doesn't have leading zeros.
//
// Example version strings:
//      "0.91.00"   // Major version 0, minor version 91, patch version 0. 
//      "1.00.00"   // Major version 1, minor and patch version 0.
//      "3.10.02"   // Major version 3, minor version 10, patch version 02.
//     "12.03.01"   // Major version 12, minor version 03, patch version 
//
// Example usage:
//     printf("CCore version: %s", CCORE_VERSION);
//     printf("CCore version: %d.%d.%d", CCORE_VERSION_N / 10000 % 100, CCORE_VERSION_N / 100 % 100, CCORE_VERSION_N % 100);
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CCORE_VERSION
    #define CCORE_VERSION "2.09.12"
    #define CCORE_VERSION_N 20912
#endif

#endif