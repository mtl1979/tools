/* This file is Copyright 2003 Level Control Systems.  See the included LICENSE.txt file for details. */

#ifndef MuscleTuple_h
#define MuscleTuple_h

#include <math.h>  // here for subclasses to use
#include "support/MuscleSupport.h"

namespace muscle {

/** Templated base class representing a fixed-size set of numeric values that can be operated on in parallel.  */
template <int NumItems, class ItemType> class Tuple
{
public:
   /** Default ctor;  All values are set to their default value. */
   Tuple() {Reset();}

   /** Value constructor.  All items are set to (value) */
   Tuple(const ItemType & value) {*this = value;}

   /** Copy constructor */
   Tuple(const Tuple & copyMe) {*this = copyMe;}

   /** Destructor */
   ~Tuple() {/* empty */}

   /** Assignment operator. */
   Tuple & operator =(const Tuple & rhs) {if (this != &rhs) {for (int i=0; i<NumItems; i++) _items[i] = rhs._items[i];} return *this;}

   /** Assignment operator for copying data in from an appropriately-sized array. */
   Tuple & operator =(const ItemType values[NumItems]) {for (int i=0; i<NumItems; i++) (*this)[i] = values[i]; return *this;}

   /** Assignment operator for setting all values to the same value. */
   Tuple & operator =(const ItemType & value) {for (int i=0; i<NumItems; i++) (*this)[i] = value; return *this;}

   /** Adds all indices in (rhs) to their counterparts in this object. */
   Tuple & operator +=(const Tuple & rhs) {for (int i=0; i<NumItems; i++) _items[i] += rhs._items[i]; return *this;}

   /** Subtracts all indices in (rhs) from their counterparts in this object. */
   Tuple & operator -=(const Tuple & rhs) {for (int i=0; i<NumItems; i++) _items[i] -= rhs._items[i]; return *this;}

   /** Multiplies all indices in this object by their counterparts in this (rhs). */
   Tuple & operator *=(const Tuple & rhs) {for (int i=0; i<NumItems; i++) _items[i] *= rhs._items[i]; return *this;}

   /** Divides all indices in this object by their counterparts in this object (rhs). */
   Tuple & operator /=(const Tuple & rhs) {for (int i=0; i<NumItems; i++) _items[i] /= rhs._items[i]; return *this;}

   /** Adds (value) to all indices in this object */
   Tuple & operator +=(const ItemType & value) {for (int i=0; i<NumItems; i++) _items[i] += value; return *this;}

   /** Subtracts (value) from all indices in this object. */
   Tuple & operator -=(const ItemType & value) {for (int i=0; i<NumItems; i++) _items[i] -= value; return *this;}

   /** Multiplies all indices in this object by (value) */
   Tuple & operator *=(const ItemType & value) {for (int i=0; i<NumItems; i++) _items[i] *= value; return *this;}

   /** Divides all indices in this object by (value) */
   Tuple & operator /=(const ItemType & value) {for (int i=0; i<NumItems; i++) _items[i] /= value; return *this;}

   /** Shifts the values of the indices left (numPlaces) spaces.  Default values (typically zero) are filled in on the right. */
   Tuple & operator <<=(int numPlaces) {ShiftValuesLeft(numPlaces); return *this;}

   /** Shifts the values of the indices right (numPlaces) spaces.  Default values (typically zero) are filled in on the left. */
   Tuple & operator >>=(int numPlaces) {ShiftValuesRight(numPlaces); return *this;}

   /** Returns true iff all indices in this object are equal to their counterparts in (rhs). */
   bool operator ==(const Tuple & rhs) const {if (this != &rhs) {for (int i=0; i<NumItems; i++) if (_items[i] != rhs._items[i]) return false;} return true;}

   /** Returns true iff any indices in this object are not equal to their counterparts in (rhs). */
   bool operator !=(const Tuple & rhs) const {return !(*this == rhs);}

   /** Comparison Operator.  Returns true if this string comes before (rhs) lexically. */
   bool operator < (const Tuple &rhs) const {if (this != &rhs) {for (int i=0; i<NumItems; i++) {if (_items[i] < rhs._items[i]) return true; if (_items[i] > rhs._items[i]) return false;}} return false;}

   /** Comparison Operator.  Returns true if this string comes after (rhs) lexically. */
   bool operator > (const Tuple &rhs) const {if (this != &rhs) {for (int i=0; i<NumItems; i++) {if (_items[i] > rhs._items[i]) return true; if (_items[i] < rhs._items[i]) return false;}} return false;}

   /** Comparison Operator.  Returns true if the two strings are equal, or this string comes before (rhs) lexically. */
   bool operator <=(const Tuple &rhs) const {return !(*this > rhs);}

   /** Comparison Operator.  Returns true if the two strings are equal, or this string comes after (rhs) lexically. */
   bool operator >=(const Tuple &rhs) const {return !(*this < rhs);}

   /** Read-write array operator (not bounds-checked) */
   ItemType & operator [](uint32 i) {return _items[i];}

   /** Read-only array operator (not bounds-checked) */
   const ItemType & operator [](uint32 i) const {return _items[i];}

   /** Returns the dot-product of (this) and (rhs) */
   ItemType DotProduct(const Tuple & rhs) const {ItemType dp = ItemType(); for (int i=0; i<NumItems; i++) dp += (_items[i]*rhs._items[i]); return dp;}

   /** Returns the index of the first value equal to (value), or -1 if not found. */
   int IndexOf(const ItemType & value) const {for (int i=0; i<NumItems; i++) if (_items[i] == value) return i; return -1;}

   /** Returns the index of the last value equal to (value), or -1 if not found. */
   int LastIndexOf(const ItemType & value) const {for (int i=NumItems-1; i>=0; i--) if (_items[i] == value) return i; return -1;}

   /** Works like strcmp(), only for a tuple. */
   int Compare(const Tuple & rhs) const {for (int i=0; i<NumItems; i++) {if (_items[i] < rhs[i]) return -1; if (_items[i] > rhs[i]) return 1;} return 0;}

   /** Returns the minimum value from amongst all the items in the tuple */
   ItemType GetMaximumValue() const {ItemType maxv = _items[0]; for (int i=1; i<NumItems; i++) if (_items[i] > maxv) maxv = _items[i]; return maxv;}

   /** Returns the maximum value from amongst all the items in the tuple */
   ItemType GetMinimumValue() const {ItemType minv = _items[0]; for (int i=1; i<NumItems; i++) if (_items[i] < minv) minv = _items[i]; return minv;}

   /** Multiplies each value by itself, and returns the sum */
   ItemType GetLengthSquared() const {ItemType sum = ItemType(); for (int i=0; i<NumItems; i++) sum += (_items[i]*_items[i]); return sum;}

   /** Returns the number of times (value) appears in this tuple */
   uint32 GetNumInstancesOf(const ItemType & value) const {uint32 count = 0; for (int i=0; i<NumItems; i++) if (_items[i] == value) count++; return count;}

   /** Returns true iff the all index values in the given range match those of the given Tuple 
     * @param matchAgainst The Tuple to do a partial index value match against
     * @param startIndex The first index to match on.  Values that are less than zero will be capped to zero.  Defaults to zero.
     * @param endIndex The last index to match on, plus one.  Values that are greater than the number of items in the tuple will
     *                 be capped to (the number of items in the tuple).  Defaults to ((uint32)-1).
     * @return true iff all indices in the range are equal, else false.
     */
   bool MatchSubrange(const Tuple & rhs, uint32 startItem = 0, uint32 endItem = ((uint32)-1)) const
   {
      if (endItem > NumItems) endItem = NumItems;
      for (uint32 i=startItem; i<endItem; i++) if (_items[i] != rhs._items[i]) return false;
      return true;
   }

   /** Sets all items in the range [startItem, endItem] (inclusive) to (value).
     * @param value The value to set items to.
     * @param startIndex The first index to set.  Values that are less than zero will be capped to zero.  Defaults to zero.
     * @param endIndex The last index to set, plus one.  Values that are greater than the number of items in the tuple will
     *                 be capped to (the number of items in the tuple).  Defaults to ((uint32)-1).
     * @return true iff all indices in the range are equal, else false.
     */
   void FillSubrange(ItemType value, uint32 startItem = 0, uint32 endItem = ((uint32)-1))
   {
      if (endItem > NumItems) endItem = NumItems;
      for (uint32 i=startItem; i<endItem; i++) _items[i] = value;
   }

   /** Sets all items in the range [startItem, endItem] (inclusive) to be equal to their counterparts in (rhs).
     * @param rhs The tuple to copy from.
     * @param startIndex The first index to set.  Values that are less than zero will be capped to zero.  Defaults to zero.
     * @param endIndex The last index to set, plus one.  Values that are greater than the number of items in the tuple will
     *                 be capped to (the number of items in the tuple).  Defaults to ((uint32)-1).
     * @return true iff all indices in the range are equal, else false.
     */
   void CopySubrange(const Tuple & rhs, uint32 startItem = 0, uint32 endItem = ((uint32)-1))
   {
      if (endItem > NumItems) endItem = NumItems;
      for (uint32 i=startItem; i<endItem; i++) _items[i] = rhs[i];
   }

   /** Sets all our items to their zero/default values */
   void Reset() {ItemType def = ItemType(); for (int i=0; i<NumItems; i++) _items[i] = def;}

   /** How many items in this tuple */
   const uint32 GetNumItemsInTuple() const {return NumItems;}

   /** typedef for our item type; used by the binary operators below */
   typedef ItemType TupleItemType;

private:
   /** Shifts the values of the indices left (numPlaces) spaces.  Blanks are filled in on the right. */
   void ShiftValuesLeft(int numPlaces)
   {
      if (numPlaces > 0) 
      {
         ItemType def = ItemType();
         for (int i=0; i<NumItems; i++) _items[i] = (i < NumItems-numPlaces) ? _items[i+numPlaces] : def;
      }
      else if (numPlaces < 0) ShiftValuesRight(-numPlaces);
   }

   /** Shifts the values of the indices right (numPlaces) spaces.  Blanks are filled in on the left. */
   void ShiftValuesRight(int numPlaces) 
   {
      if (numPlaces > 0) 
      {
         ItemType def = ItemType();
         for (int i=NumItems-1; i>=0; i--) _items[i] = (i >= numPlaces) ? _items[i-numPlaces] : def;
      }
      else if (numPlaces < 0) ShiftValuesLeft(-numPlaces);
   }

private:
   ItemType _items[NumItems];
};

template <int N,class T> inline const Tuple<N,T> operator -  (const Tuple<N,T> & lhs)                {Tuple<N,T> ret(lhs); ret  -= lhs+lhs;      return ret;}
template <int N,class T> inline const Tuple<N,T> operator +  (const Tuple<N,T> & lhs, const T & rhs) {Tuple<N,T> ret(lhs); ret  += rhs;          return ret;}
template <int N,class T> inline const Tuple<N,T> operator -  (const Tuple<N,T> & lhs, const T & rhs) {Tuple<N,T> ret(lhs); ret  -= rhs;          return ret;}
template <int N,class T> inline const Tuple<N,T> operator +  (const T & lhs, const Tuple<N,T> & rhs) {Tuple<N,T> ret(lhs); ret  += rhs;          return ret;}
template <int N,class T> inline const Tuple<N,T> operator -  (const T & lhs, const Tuple<N,T> & rhs) {Tuple<N,T> ret(lhs); ret  -= rhs;          return ret;}
template <int N,class T> inline const Tuple<N,T> operator +  (const Tuple<N,T> & lhs, const Tuple<N,T> & rhs) {Tuple<N,T> ret(lhs); ret  += rhs; return ret;}
template <int N,class T> inline const Tuple<N,T> operator -  (const Tuple<N,T> & lhs, const Tuple<N,T> & rhs) {Tuple<N,T> ret(lhs); ret  -= rhs; return ret;}
template <int N,class T> inline const Tuple<N,T> operator *  (const Tuple<N,T> & lhs, const T & rhs) {Tuple<N,T> ret(lhs); ret  *= rhs;          return ret;}
template <int N,class T> inline const Tuple<N,T> operator /  (const Tuple<N,T> & lhs, const T & rhs) {Tuple<N,T> ret(lhs); ret  /= rhs;          return ret;}
template <int N,class T> inline const Tuple<N,T> operator *  (const T & lhs, const Tuple<N,T> & rhs) {Tuple<N,T> ret(lhs); ret  *= rhs;          return ret;}
template <int N,class T> inline const Tuple<N,T> operator /  (const T & lhs, const Tuple<N,T> & rhs) {Tuple<N,T> ret(lhs); ret  /= rhs;          return ret;}
template <int N,class T> inline const Tuple<N,T> operator *  (const Tuple<N,T> & lhs, const Tuple<N,T> & rhs) {Tuple<N,T> ret(lhs); ret  *= rhs; return ret;}
template <int N,class T> inline const Tuple<N,T> operator /  (const Tuple<N,T> & lhs, const Tuple<N,T> & rhs) {Tuple<N,T> ret(lhs); ret  /= rhs; return ret;}

#define DECLARE_ADDITION_TUPLE_OPERATORS(C,I) \
  inline const C operator + (const C & lhs, const I & rhs) {C ret(lhs);                   ret += rhs;     return ret;} \
  inline const C operator + (const I & lhs, const C & rhs) {C ret; ret.FillSubrange(lhs); ret += rhs;     return ret;} \
  inline const C operator + (const C & lhs, const C & rhs) {C ret(lhs);                   ret += rhs;     return ret;} 

#define DECLARE_SUBTRACTION_TUPLE_OPERATORS(C,I) \
  inline const C operator - (const C & lhs)                {C ret(lhs);                   ret -= lhs+lhs; return ret;} \
  inline const C operator - (const C & lhs, const I & rhs) {C ret(lhs);                   ret -= rhs;     return ret;} \
  inline const C operator - (const I & lhs, const C & rhs) {C ret; ret.FillSubrange(lhs); ret -= rhs;     return ret;} \
  inline const C operator - (const C & lhs, const C & rhs) {C ret(lhs);                   ret -= rhs;     return ret;}

#define DECLARE_MULTIPLICATION_TUPLE_OPERATORS(C,I) \
  inline const C operator * (const C & lhs, const I & rhs) {C ret(lhs);                   ret *= rhs;     return ret;} \
  inline const C operator * (const I & lhs, const C & rhs) {C ret; ret.FillSubrange(lhs); ret *= rhs;     return ret;} \
  inline const C operator * (const C & lhs, const C & rhs) {C ret(lhs);                   ret *= rhs;     return ret;}

#define DECLARE_DIVISION_TUPLE_OPERATORS(C,I) \
  inline const C operator / (const C & lhs, const I & rhs) {C ret(lhs);                   ret /= rhs;     return ret;} \
  inline const C operator / (const I & lhs, const C & rhs) {C ret; ret.FillSubrange(lhs); ret /= rhs;     return ret;} \
  inline const C operator / (const C & lhs, const C & rhs) {C ret(lhs);                   ret /= rhs;     return ret;}

#define DECLARE_SHIFT_TUPLE_OPERATORS(C) \
  inline const C operator >> (const C & lhs, int rhs) {C ret(lhs);                        ret >>= rhs;    return ret;} \
  inline const C operator << (const C & lhs, int rhs) {C ret(lhs);                        ret <<= rhs;    return ret;} 

// Classes sublassing a Tuple class can use this macro to get the all the standard operators without having to rewrite them.
// If anyone knows how to accomplish this without resorting to preprocessor macro hacks, I'd love to hear about it...
#define DECLARE_ALL_TUPLE_OPERATORS(C,I) \
        DECLARE_ADDITION_TUPLE_OPERATORS(C,I) \
        DECLARE_SUBTRACTION_TUPLE_OPERATORS(C,I) \
        DECLARE_MULTIPLICATION_TUPLE_OPERATORS(C,I) \
        DECLARE_DIVISION_TUPLE_OPERATORS(C,I)  \
        DECLARE_SHIFT_TUPLE_OPERATORS(C)

};  // end namespace muscle

#endif
