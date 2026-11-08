/* SPDX-License-Identifier: GPL-3.0-only
 * Native vector operations run synchronously inside the existing locked factory.
 * Never alter firmware code, vector headers or member bytes directly.
 */
#include <cstdint>
#include <cstring>
struct NativeVector { void *begin,*end,*capacity; };
static_assert(sizeof(NativeVector)==24,"Exact native vector ABI");
template<class F> static F fn(uintptr_t p){return reinterpret_cast<F>(p);}
struct OwnedVector {
 NativeVector value{};
 ~OwnedVector(){fn<void(*)(void*)>(0x13e5670)(&value);}
};
extern "C" int role_vector_keep(void *owner,unsigned wanted) {
 if(!owner || (wanted!=7 && wanted!=11))return 1;
 auto vector=reinterpret_cast<NativeVector *>(static_cast<unsigned char *>(owner)+0x398);
 auto begin=reinterpret_cast<uintptr_t>(vector->begin),end=reinterpret_cast<uintptr_t>(vector->end);
 if(!begin || (begin&7) || end<begin || end-begin!=5*0x50)return 2;
 for(unsigned i=0;i<5;i++) {
  auto p=reinterpret_cast<const unsigned char *>(begin+i*0x50);
  if(p[0]!=(i<4?1:2) || p[1]!=(i<4?i+1:33))return 3;
 }
 try {
  OwnedVector temporary;
  for(unsigned i=0;i<5;i++)if(i==4 || (wanted&(1u<<i))) {
   auto p=reinterpret_cast<const void *>(begin+i*0x50);
   /* Native realloc-insert copies the member and retains its JUCE strings.
    * Always insert at end into our own vector; capacity is managed natively. */
   fn<void(*)(void*,void*,const void*)>(0x13e5f50)(&temporary.value,temporary.value.end,p);
  }
  /* Native vector assignment safely replaces the factory's list; our retained
   * source members remain alive until its assignment has completed. */
  fn<void(*)(void*,const void*)>(0x13e5810)(vector,&temporary.value);
  return 0;
 }catch(...){return 4;}
}

/* Native Optional<DeviceMember> return is 0x58 bytes (valid byte + padded
 * 0x50-byte member). Both this trivial ABI carrier and the native type return
 * indirectly in x8. The caller remains the sole owner of embedded strings.
 * Do not construct, edit, destroy or retain native result/member bytes here.
 * Native lookup is called exactly once and constructs directly into the result.
 */
struct LookupResult { alignas(8) unsigned char bytes[0x58]; };
static_assert(sizeof(LookupResult)==0x58,"Exact native lookup return ABI");
extern "C" {
void *native_lookup_original;
unsigned link_reserves_number(void *,unsigned,uintptr_t);
LookupResult allocation_lookup(void *owner,const unsigned char *number) {
 const unsigned char local_identity=0;
 auto original=reinterpret_cast<LookupResult(*)(void*,const unsigned char*)>(native_lookup_original);
 const unsigned char *requested=number;
 if(number && link_reserves_number(owner,*number,(uintptr_t)__builtin_return_address(0)))
  requested=&local_identity;
 return original(owner,requested);
}
}
