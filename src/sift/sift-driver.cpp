// file:        sift-driver.cpp
// author:      Andrea Vedaldi
// description: SIFT command line utility implementation

// AUTORIGTHS

#include "sift.hpp"

#include<string>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<algorithm>

//extern "C" {
//#include<getopt.h>
//#if defined (VL_MAC)
//#include<libgen.h>
//#else
//#include<string.h>
//#endif
//#include<assert.h>
//}
extern "C" {
#include<string.h>
#include<assert.h>
}
#include<memory>

using namespace std ;

size_t const not_found = numeric_limits<size_t>::max() - 1 ;

/** @brief Case insensitive character comparison
 **
 ** This predicate returns @c true if @a a and @a b are equal up to
 ** case.
 **
 ** @return predicate value.
 **/
inline
bool ciIsEqual(char a, char b)
{
  return 
    tolower((char unsigned)a) == 
    tolower((char unsigned)b) ;
}

/** @brief Case insensitive extension removal
 **
 ** The function returns @a name with the suffix $a ext removed.  The
 ** suffix is matched case-insensitve.
 **
 ** @return @a name without @a ext.
 **/
string
removeExtension(string name, string ext)
{
  string::iterator pos = 
    find_end(name.begin(),name.end(),ext.begin(),ext.end(),ciIsEqual) ;

  // make sure the occurence is at the end
  if(pos+ext.size() == name.end()) {
    return name.substr(0, pos-name.begin()) ;
  } else {
    return name ;
  }
}


/** @brief Insert descriptor into stream
 **
 ** The function writes a descriptor in ASCII/binary format
 ** and in integer/floating point format into the stream.
 **
 ** @param os output stream.
 ** @param descr_pt descriptor (floating point)
 ** @param binary write binary descriptor?
 ** @param fp write floating point data?
 **/
std::ostream&
insertDescriptor(std::ostream& os,
                 VL::float_t const * descr_pt,
                 bool binary,
                 bool fp )
{
#define RAW_CONST_PT(x) reinterpret_cast<char const*>(x)
#define RAW_PT(x)       reinterpret_cast<char*>(x)

  if( fp ) {

    /* convert to 32 bits floats (single precision) */
    VL::float32_t fdescr_pt [128] ;
    for(int i = 0 ; i < 128 ; ++i)
      fdescr_pt[i] = VL::float32_t( descr_pt[i]) ;

    if( binary ) {
      /* 
         Test for endianess. Recall: big_endian = the most significant
         byte at lower memory address.
      */
      short int const word = 0x0001 ;
      bool little_endian = RAW_CONST_PT(&word)[0] ;
      
      /* 
         We save in big-endian (network) order. So if this machine is
         little endiand do the appropriate conversion.
      */
      if( little_endian ) {
        for(int i = 0 ; i < 128 ; ++i) {
          VL::float32_t tmp = fdescr_pt[ i ] ;        
          char* pt  = RAW_PT(fdescr_pt + i) ;
          char* spt = RAW_PT(&tmp) ;
          pt[0] = spt[3] ;
          pt[1] = spt[2] ;
          pt[2] = spt[1] ;
          pt[3] = spt[0] ;
        }
      }            
      os.write( RAW_PT(fdescr_pt), 128 * sizeof(VL::float32_t) ) ;

    } else {

      for(int i = 0 ; i < 128 ; ++i) 
        os << ' ' 
           << fdescr_pt[i] ;
    }

  } else {

    VL::uint8_t idescr_pt [128] ;

    for(int i = 0 ; i < 128 ; ++i)
      idescr_pt[i] = VL::uint8_t(512.0 * descr_pt[i]) ;
    
    if( binary ) {

      os.write( RAW_PT(idescr_pt), 128) ;	

    } else { 
      
      for(int i = 0 ; i < 128 ; ++i) 
        os << ' ' 
           << VL::uint32_t( idescr_pt[i] ) ;
    }
  }
  return os ;
}

/* keypoint list */
typedef vector<pair<VL::Sift::Keypoint,VL::float_t> > Keypoints ;

/* predicate used to order keypoints by increasing scale */
bool cmpKeypoints (Keypoints::value_type const&a,
		   Keypoints::value_type const&b) {
  return a.first.sigma < b.first.sigma ;
}

