// mspu.cc -- mspu target support for gold.

// Copyright 2012 CASIA.
// Written by Shawn.Xie <shaolin.xie@ia.ac.cn>

// This file is part of gold.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

#include "gold.h"

#include "elfcpp.h"
#include "parameters.h"
#include "reloc.h"
#include "mspu.h"
#include "object.h"
#include "symtab.h"
#include "layout.h"
#include "output.h"
#include "copy-relocs.h"
#include "target.h"
#include "target-reloc.h"
#include "target-select.h"
#include "errors.h"
#include "gc.h"
#include <cassert>

#pragma GCC diagnostic ignored "-Wunused-parameter"
namespace
{

using namespace gold;

//The mspu only supports 32bit address & little endian
template<int size, bool big_endian>
class Target_mspu : public Sized_target<size, big_endian>{
public:
  typedef Output_data_reloc<elfcpp::SHT_RELA, true, size, big_endian> Reloc_section;

  //Construction function
  Target_mspu()  : Sized_target<size, big_endian>(&mspu_info)  { }
  
/*************************************************/
  void gc_process_relocs(
    Symbol_table*         symtab,
    Layout*               layout,
    Sized_relobj_file<size, big_endian>* object,
    unsigned int          data_shndx,
    unsigned int          sh_type,
    const unsigned char*  prelocs,
    size_t                reloc_count,
    Output_section*       output_section,
    bool                  needs_special_offset_handling,
    size_t                local_symbol_count,
    const unsigned char*  plocal_symbols);

/*************************************************/
  void  scan_relocs(
    Symbol_table*         symtab,
    Layout*               layout,
    Sized_relobj_file<size, big_endian>* object,
    unsigned int          data_shndx,
    unsigned int          sh_type,
    const unsigned char*  prelocs,
    size_t                reloc_count,
    Output_section*       output_section,
    bool                  needs_special_offset_handling,
    size_t                local_symbol_count,
    const unsigned char*  plocal_symbols);
  
 
/*************************************************/
  void   relocate_section(
    const Relocate_info<size, big_endian> * relinfo,
    unsigned int                            sh_type,
    const unsigned char*                    prelocs,
    size_t                                  reloc_count,
    Output_section*                         output_section,
    bool                                    needs_special_offset_handling,
    unsigned char*                          view,
    typename elfcpp::Elf_types<size>::Elf_Addr  view_address,
    section_size_type                           view_size,
    const Reloc_symbol_changes*                 reloc_symbol_changes);

/*************************************************/
  void   scan_relocatable_relocs(
    Symbol_table*                         symtab,
    Layout*                               layout,
    Sized_relobj_file<size, big_endian>*  object,
    unsigned int                          data_shndx,
    unsigned int                          sh_type,
    const unsigned char*                  prelocs,
    size_t                                reloc_count,
    Output_section*                       output_section,
    bool                                  needs_special_offset_handling,
    size_t                                local_symbol_count,
    const unsigned char*                  plocal_symbols,
    Relocatable_relocs*                   rr); 
  

/*************************************************/
  void   relocate_for_relocatable(
    const Relocate_info<size, big_endian>*      relinfo,
    unsigned int                                sh_type,
    const unsigned char*                        prelocs,
    size_t                                      reloc_count,
    Output_section*                             output_section,
    off_t                                       offset_in_output_section,
    const Relocatable_relocs*                   rr,
    unsigned char*                              view,
    typename elfcpp::Elf_types<size>::Elf_Addr  view_address,
    section_size_type                           view_size,
    unsigned char*                              reloc_view,
    section_size_type                           reloc_view_size);
/*************************************************/
  static Target::Target_info mspu_info;

private:
  class Relocatable_size_for_reloc  {
    public: unsigned int   get_size_for_reloc(unsigned int, Relobj*){
      gold_unreachable();
      return 0;
    }
  }; //end of class Relocatable_size_for_reloc

/*************************************************/
//  Scan Class for Scan_relocs
/*************************************************/
private:
  class Scan {
    public: Scan() : issued_non_pic_error_(false){}

    static inline int get_reference_flags(unsigned int r_type);

    //Scan a relocation  for local variable
    inline void       local(
      Symbol_table*                         symtab, 
      Layout*                               layout, 
      Target_mspu*                           target,
	    Sized_relobj_file<size, big_endian>*  object,
	    unsigned int                          data_shndx,
	    Output_section*                       output_section,
	    const elfcpp::Rela<size, big_endian>& reloc, 
      unsigned int                          r_type,
	    const elfcpp::Sym<size, big_endian>&  lsym);

    //Scan a relocation  for global variable
    inline void global(
      Symbol_table*                         symtab, 
      Layout*                               layout, 
      Target_mspu*                           target,
	    Sized_relobj_file<size, big_endian>*  object,
	    unsigned int                          data_shndx,
	    Output_section*                       output_section,
	    const elfcpp::Rela<size, big_endian>& reloc, 
      unsigned int                          r_type,
	    Symbol*                               gsym);

    inline bool local_reloc_may_be_function_pointer(
      Symbol_table*                               , 
      Layout*                                     ,
		  Target_mspu*                                 ,
	    Sized_relobj_file<size, big_endian>*        ,
			unsigned int                                ,
	    Output_section*                             ,
	    const elfcpp::Rela<size, big_endian>&       ,
		  unsigned int                                ,
	    const elfcpp::Sym<size, big_endian>&        ) { 
        return false; 
    }

    inline bool global_reloc_may_be_function_pointer(
      Symbol_table*                               , 
      Layout*                                     ,
		  Target_mspu*                                 ,
		  Sized_relobj_file<size, big_endian>*        ,
		  unsigned int                                ,
		  Output_section*                             ,
		  const elfcpp::Rela<size, big_endian>&       ,
			unsigned int                                , 
      Symbol*                                     ) { 
        return false; 
    }

private:
    // Whether we have issued an error about a non-PIC compilation.
    bool issued_non_pic_error_;
  }; //end of class Scan
/*************************************************/
// Relocate Class for Relocate,which must be a class type which 
// provides a single function, relocate(), which implements the 
// machine  specific part of a relocation.
/*************************************************/
private:
  class Relocate {
   public:
    // Do a relocation.  Return false if the caller should not issue
    // any warnings about this relocation.
    inline bool relocate(
       const Relocate_info<size, big_endian>*           , 
       Target_mspu*                                      ,
	     Output_section*, size_t relnum                   ,
	     const elfcpp::Rela<size, big_endian>&            ,
	     unsigned int r_type, const Sized_symbol<size>*   ,
	     const Symbol_value<size>*                        ,
	     unsigned char*                                   ,
	     typename elfcpp::Elf_types<size>::Elf_Addr       ,
	     section_size_type);
  };//end of class Relocate
  
};//end of class Target_mspu



//////////////////////////////////////////////////////////////////////
template<>
Target::Target_info Target_mspu<32, false>::mspu_info =
{
  32,			// size
  false,		// is_big_endian
  elfcpp::EM_MSPU,// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  false,		// has_code_fill
  true,			// is_default_stack_executable
  false,		// can_icf_inline_merge_sections
  '\0',			// wrap_char
  "",	// dynamic_linker
  0x0,		// default_text_segment_address
  16,		// abi_pagesize: 4-byte alignment (overridable by -z max-page-size)
  4,		// common_pagesize: 4-byte alignment (overridable by -z common-page-size)
  elfcpp::SHN_UNDEF,	// small_common_shndx
  elfcpp::SHN_UNDEF,	// large_common_shndx
  0,			// small_common_section_flags
  0,			// large_common_section_flags
  NULL,			// attributes_section
  NULL			// attributes_vendor
};

///////////////////////////////////////////////////////////////////////////////
template<int size, bool big_endian>
class Target_selector_mspu : public Target_selector {
public:
  Target_selector_mspu()
    : Target_selector(elfcpp::EM_NONE, size, big_endian,
					  "elf32-mspu", "elf32mspu")  { }

  Target* do_recognize(int machine, int, int){
	if( machine == elfcpp::EM_MSPU) 	return this->instantiate_target();
	else 							return NULL;
  }

  Target* do_instantiate_target()  { 
	return new Target_mspu<size, big_endian>(); 
   }
};

Target_selector_mspu<32, false> target_selector_mspu32le;

} // End anonymous namespace.

template<int size, bool big_endian>
class mspu_relocate_functions {
  public:
  // Do a simple relocation using a symbol value with the addend in
  // the relocation.
  template<int valsize> static inline void rela(
  unsigned char*                                      view,
  unsigned int                                        right_shift,
  unsigned int                                        left_shift, 
  elfcpp::Elf_Xword                                   dst_mask,
  const Sized_relobj_file<size, big_endian>*          object,
  const Symbol_value<size>*                           psymval,
  typename elfcpp::Swap<valsize, big_endian>::Valtype addend) {

    typedef typename elfcpp::Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<valsize, big_endian>::readval(wv);
    Valtype reloc = (psymval->value(object, addend) >> right_shift);
    
    reloc = reloc << left_shift;
    val &= ~dst_mask;
    reloc &= dst_mask;

    elfcpp::Swap<valsize, big_endian>::writeval(wv, val | reloc);
  }

  // Do a relocation agaist the mmpu object file
  template<int valsize> static inline void mmpu_rela(
    unsigned char*                                      view,
    unsigned int                                        right_shift,
    unsigned int                                        left_shift,
    elfcpp::Elf_Xword                                   dst_mask,
    const Sized_relobj_file<size, big_endian>*          object,
    const Symbol_value<size>*                           psymval,
    typename elfcpp::Swap<valsize, big_endian>::Valtype addend) {

      typedef typename elfcpp::Swap<valsize, big_endian>::Valtype Valtype;
      Valtype* wv = reinterpret_cast<Valtype*>(view);
      Valtype val = elfcpp::Swap<valsize, big_endian>::readval(wv);
      Valtype reloc = (psymval->value(object, addend) >> right_shift);

      assert( reloc%41== 0 );

      reloc = reloc /41 ;        //41 is the instruction size of the microcode.
      reloc = reloc << left_shift;
      val &= ~dst_mask;
      reloc &= dst_mask;

      elfcpp::Swap<valsize, big_endian>::writeval(wv, val | reloc);
    }

  // Do a simple PC relative relocation with a Symbol_value with the
  // addend in the relocation.
  template<int valsize> static inline void pcrela(
  unsigned char*                                    view, 
  unsigned int                                      right_shift,
  unsigned int                                      left_shift,
  elfcpp::Elf_Xword                                 dst_mask,
  const Sized_relobj_file<size, big_endian>*        object,
  const Symbol_value<size>*                         psymval,
  typename elfcpp::Swap<size, big_endian>::Valtype  addend,
  typename elfcpp::Elf_types<size>::Elf_Addr        address) {

    typedef typename elfcpp::Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<valsize, big_endian>::readval(wv);
    Valtype reloc = ((psymval->value(object, addend) - address) >> right_shift);

    reloc = reloc << left_shift;

    val &= ~dst_mask;
    reloc &= dst_mask;

    elfcpp::Swap<valsize, big_endian>::writeval(wv, val | reloc);
  }

  // Do a absolute function  point relocation
  template<int valsize> static inline void func_rela(
  unsigned char*                                    view,
  elfcpp::Elf_Xword                                 dst_mask,
  const Sized_relobj_file<size, big_endian>*        object,
  const Symbol_value<size>*                         psymval,
  typename elfcpp::Swap<size, big_endian>::Valtype  addend) {

    typedef typename elfcpp::Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<valsize, big_endian>::readval(wv);
    Valtype reloc = psymval->value(object, addend);

    val &= ~dst_mask;
    reloc &= dst_mask;

    elfcpp::Swap<valsize, big_endian>::writeval(wv, val | reloc);
  }


  // Do a pc-relative function  point relocation
  template<int valsize> static inline void func_pc_rela(
  unsigned char*                                    view,
  elfcpp::Elf_Xword                                 dst_mask,
  const Sized_relobj_file<size, big_endian>*        object,
  const Symbol_value<size>*                         psymval,
  typename elfcpp::Swap<size, big_endian>::Valtype  addend,
  typename elfcpp::Elf_types<size>::Elf_Addr        address) {

    typedef typename elfcpp::Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<valsize, big_endian>::readval(wv);
    Valtype reloc =psymval->value(object, addend) - address ;

    val &= ~dst_mask;
    reloc &= dst_mask;

    elfcpp::Swap<valsize, big_endian>::writeval(wv, val | reloc);
  }

};
///////////////////////////////////////////////////////////////////////////////
/*************************************************/
// Process the relocs for a section, and record information of the
// mapping from source to destination sections. This mapping is later
// used to determine unreferenced garbage sections. This procedure is
// only called during garbage collection.
template<int size, bool big_endian>
void
Target_mspu<size, big_endian>::gc_process_relocs(
    Symbol_table*                         symtab,
    Layout*                               layout,
    Sized_relobj_file<size, big_endian>*  object,
    unsigned int                          data_shndx,
    unsigned int                          sh_type,
    const unsigned char*                  prelocs,
    size_t                                reloc_count,
    Output_section*                       output_section,
    bool                                  needs_special_offset_handling,
    size_t                                local_symbol_count,
    const unsigned char*                  plocal_symbols){
  
  typedef Target_mspu<size, big_endian> Mspu;
  typedef typename Target_mspu<size, big_endian>::Scan Scan;

  gold::gc_process_relocs<size, big_endian, Mspu, elfcpp::SHT_RELA, Scan,
			  typename Target_mspu::Relocatable_size_for_reloc>(
    symtab,
    layout,
    this,
    object,
    data_shndx,
    prelocs,
    reloc_count,
    output_section,
    needs_special_offset_handling,
    local_symbol_count,
    plocal_symbols);
}

/*************************************************/
// Scan the relocs for a section, and record any information
// required for the symbol.  
//
// SYMTAB --the symbol table.  
// OBJECT --the object in which the section appears. 
// DATA_SHNDX -- the section index that these relocs apply to.  
// SH_TYPE    -- the type of the relocation section, SHT_REL or SHT_RELA.  
// PRELOCS    -- points to  the relocation data.  
// RELOC_COUNT-- is the number of relocs.
// LOCAL_SYMBOL_COUNT  --  the number of local symbols.
// OUTPUT_SECTION      --  the output section.
// NEEDS_SPECIAL_OFFSET_HANDLING  --  true if offsets to the output
// sections are not mapped as usual.  
// PLOCAL_SYMBOLS      -- points to the local symbol data from OBJECT. 
template<int size, bool big_endian>
void
Target_mspu<size, big_endian>::scan_relocs(
    Symbol_table*         symtab,
    Layout*               layout,
    Sized_relobj_file<size, big_endian>* object,
    unsigned int          data_shndx,
    unsigned int          sh_type,
    const unsigned char*  prelocs,
    size_t                reloc_count,
    Output_section*       output_section,
    bool                  needs_special_offset_handling,
    size_t                local_symbol_count,
    const unsigned char*  plocal_symbols){

  typedef Target_mspu<size, big_endian>                Mspu;
  typedef typename Target_mspu<size, big_endian>::Scan Scan;

  if (sh_type == elfcpp::SHT_REL) {
    gold_error(_("%s: unsupported REL reloc section"),
		object->name().c_str());
    return;
  }

  gold::scan_relocs<size, big_endian, Mspu, elfcpp::SHT_RELA, Scan>(
    symtab,
    layout,
    this,
    object,
    data_shndx,
    prelocs,
    reloc_count,
    output_section,
    needs_special_offset_handling,
    local_symbol_count,
    plocal_symbols);
}

/****************************************************************/
// Relocate section data.  SH_TYPE is the type of the relocation
// section, SHT_REL or SHT_RELA.  PRELOCS points to the relocation
// information.  RELOC_COUNT is the number of relocs.
// OUTPUT_SECTION is the output section.
// NEEDS_SPECIAL_OFFSET_HANDLING is true if offsets must be mapped
// to correspond to the output section.  VIEW is a view into the
// output file holding the section contents, VIEW_ADDRESS is the
// virtual address of the view, and VIEW_SIZE is the size of the
// view.  If NEEDS_SPECIAL_OFFSET_HANDLING is true, the VIEW_xx
// parameters refer to the complete output section data, not just
// the input section data.
/****************************************************************/
template<int size, bool big_endian>
void Target_mspu<size, big_endian>:: relocate_section(
  const Relocate_info<size, big_endian> * relinfo,
  unsigned int                            sh_type,
  const unsigned char*                    prelocs,
  size_t                                  reloc_count,
  Output_section*                         output_section,
  bool                                    needs_special_offset_handling,
  unsigned char*                          view,
  typename elfcpp::Elf_types<size>::Elf_Addr  view_address,
  section_size_type                           view_size,
  const Reloc_symbol_changes*                 reloc_symbol_changes){

  typedef Target_mspu<size, big_endian> Mspu;
  typedef typename Target_mspu<size, big_endian>::Relocate Mspu_relocate;

  gold_assert(sh_type == elfcpp::SHT_RELA);

  gold::relocate_section<size, big_endian, Mspu, elfcpp::SHT_RELA,
    Mspu_relocate>(
    relinfo                       ,
    this                          ,
    prelocs                       ,
    reloc_count                   ,
    output_section                ,
    needs_special_offset_handling ,
    view                          ,
    view_address                  ,
    view_size                     ,
    reloc_symbol_changes          );
}
/****************************************************************/
// Scan the relocs during a relocatable link.  The parameters are
// like scan_relocs, with an additional Relocatable_relocs
// parameter, used to record the disposition of the relocs.
/****************************************************************/

template<int size, bool big_endian>
void Target_mspu<size, big_endian>::scan_relocatable_relocs(
  Symbol_table*                         symtab,
  Layout*                               layout,
  Sized_relobj_file<size, big_endian>*  object,
  unsigned int                          data_shndx,
  unsigned int                          sh_type,
  const unsigned char*                  prelocs,
  size_t                                reloc_count,
  Output_section*                       output_section,
  bool                                  needs_special_offset_handling,
  size_t                                local_symbol_count,
  const unsigned char*                  plocal_symbols,
  Relocatable_relocs*                   rr) {

  gold_assert(sh_type == elfcpp::SHT_RELA);

  typedef gold::Default_scan_relocatable_relocs<elfcpp::SHT_RELA,
    Relocatable_size_for_reloc> Scan_relocatable_relocs;

  gold::scan_relocatable_relocs<size, big_endian, elfcpp::SHT_RELA,
    Scan_relocatable_relocs>(
    symtab,
    layout,
    object,
    data_shndx,
    prelocs,
    reloc_count,
    output_section,
    needs_special_offset_handling,
    local_symbol_count,
    plocal_symbols,
    rr);
} //end of scan_relocatable_relocs
// Relocate a section during a relocatable link.

/****************************************************************/
// Relocate a section during a relocatable link.  The parameters are
// like relocate_section, with additional parameters for the view of
// the output reloc section.
/****************************************************************/
template<int size, bool big_endian>
void Target_mspu<size, big_endian>::relocate_for_relocatable(
  const Relocate_info<size, big_endian>*      relinfo,
  unsigned int                                sh_type,
  const unsigned char*                        prelocs,
  size_t                                      reloc_count,
  Output_section*                             output_section,
  off_t                                       offset_in_output_section,
  const Relocatable_relocs*                   rr,
  unsigned char*                              view,
  typename elfcpp::Elf_types<size>::Elf_Addr  view_address,
  section_size_type                           view_size,
  unsigned char*                              reloc_view,
  section_size_type                           reloc_view_size) {

  gold_assert(sh_type == elfcpp::SHT_RELA);

  gold::relocate_for_relocatable<size, big_endian, elfcpp::SHT_RELA>(
    relinfo,
    prelocs,
    reloc_count,
    output_section,
    offset_in_output_section,
    rr,
    view,
    view_address,
    view_size,
    reloc_view,
    reloc_view_size);
}

/**********************************************************************/
//  Functions for Class Scan
/**********************************************************************/

//Get the Reference type according to the "r_type"
//Canditates inclues:
//  0, ABSOLUTE_REF, RELATIVE_REF, FUNCTION_REF, TSL_REF
template<int size, bool big_endian>
int Target_mspu<size, big_endian>::Scan::get_reference_flags(
    unsigned int r_type){
  if ( r_type == elfcpp::R_MSPU_PC17 ) 	return Symbol::RELATIVE_REF;
  else                         			return Symbol::ABSOLUTE_REF;
} 

// Scan a relocation for a local symbol, but all code are related to 
// GOT or share libraries in Powerpc.cc
template<int size, bool big_endian>
inline void
Target_mspu<size, big_endian>::Scan::local(
  Symbol_table*                         symtab, 
  Layout*                               layout, 
  Target_mspu*                           target,
  Sized_relobj_file<size, big_endian>*  object,
  unsigned int                          data_shndx,
  Output_section*                       output_section,
  const elfcpp::Rela<size, big_endian>& reloc, 
  unsigned int                          r_type,
  const elfcpp::Sym<size, big_endian>&  lsym){

  switch( r_type ) {
    case elfcpp::R_MSPU_PC17:
    case elfcpp::R_MSPU_PC28:
    case elfcpp::R_MSPU_11_B5:
    case elfcpp::R_MSPU_11_B10:
    case elfcpp::R_MSPU_17:
    case elfcpp::R_MSPU_28:
    case elfcpp::R_MSPU_PC_8:     // 1 byte, 8 bits
    case elfcpp::R_MSPU_PC_16:    // 2 bytes, 16 bits
    case elfcpp::R_MSPU_PC_32:    // 32 bits
    case elfcpp::R_MSPU_PC_64:    // 64 bits

    // pc irrelative,//Write the R_MSPU_8 to  *R_MSPU_8
    case elfcpp::R_MSPU_8:     // 1 byte, 8 bits
    case elfcpp::R_MSPU_16:    // 2 bytes, 16 bits
    case elfcpp::R_MSPU_32:    // 32 bits
    case elfcpp::R_MSPU_64:    // 64 bits
      break;
    default:
      gold_error(_("%s: unexpected reloc %u in object file"),
		  object->name().c_str(), r_type);
      break;
  }
}

// Scan a relocation for a global symbol, but all code are related to 
// GOT or share libraries in Powerpc.cc
template<int size, bool big_endian>
inline void
Target_mspu<size, big_endian>::Scan::global(
  Symbol_table*                         symtab, 
  Layout*                               layout, 
  Target_mspu*                           target,
  Sized_relobj_file<size, big_endian>*  object,
  unsigned int                          data_shndx,
  Output_section*                       output_section,
  const elfcpp::Rela<size, big_endian>& reloc, 
  unsigned int                          r_type,
  Symbol*                               gsym){
  switch( r_type ) {
    case elfcpp::R_MSPU_PC17:
    case elfcpp::R_MSPU_PC28:
    case elfcpp::R_MSPU_11_B5:
    case elfcpp::R_MSPU_11_B10:
    case elfcpp::R_MSPU_17:
    case elfcpp::R_MSPU_28:
    case elfcpp::R_MSPU_PC_8:     // 1 byte, 8 bits
    case elfcpp::R_MSPU_PC_16:    // 2 bytes, 16 bits
    case elfcpp::R_MSPU_PC_32:    // 32 bits
    case elfcpp::R_MSPU_PC_64:    // 64 bits

    // pc irrelative,//Write the R_MSPU_8 to  *R_MSPU_8
    case elfcpp::R_MSPU_8:     // 1 byte, 8 bits
    case elfcpp::R_MSPU_16:    // 2 bytes, 16 bits
    case elfcpp::R_MSPU_32:    // 32 bits
    case elfcpp::R_MSPU_64:    // 64 bits
      break;
    default:
      gold_error(_("%s: unexpected reloc %u in object file"),
		  object->name().c_str(), r_type);
      break;
  }
}
/**********************************************************************/
//  Functions for Class Relocation
/**********************************************************************/
template<int size, bool big_endian> inline bool
Target_mspu<size, big_endian>::Relocate::relocate(
  const Relocate_info<size, big_endian>*      relinfo,
  Target_mspu*                             	  target,
  Output_section*                                   ,
  size_t                                      relnum,
  const elfcpp::Rela<size, big_endian>&       rela,
  unsigned int                                r_type,
  const Sized_symbol<size>*                   gsym,
  const Symbol_value<size>*                   psymval,
  unsigned char*                              view,
  typename elfcpp::Elf_types<size>::Elf_Addr  address,
  section_size_type /* view_size */) {

  typedef mspu_relocate_functions<size, big_endian> Reloc;
  typedef typename elfcpp::Swap<size, big_endian>::Valtype Valtype;
  
  const Sized_relobj_file<size, big_endian>* object = relinfo->object;
  elfcpp::Elf_Xword                          addend = rela.get_r_addend();
  //////////////////////////Check if Is Immextended.
  bool  bImmExt=false;
  Valtype* wv = reinterpret_cast<Valtype*>(view + 4);   //get the next instruction code.
  Valtype val = elfcpp::Swap<size, big_endian>::readval(wv);
  if ( ( (val >> 28) & 0x7 ) == 0x5 ) bImmExt= true;
  else                                bImmExt= false;

  switch( r_type ){
    case elfcpp::R_MSPU_11_B5   ://This is the ImmAssign Instruction
    	if( bImmExt )  Reloc::template rela<32>(view, 0, 5, 0x000001E0, object, psymval, addend);
    	else           Reloc::template rela<32>(view, 0, 5, 0x0000FFE0, object, psymval, addend);
        break;
    case elfcpp::R_MSPU_11_B10   : //This is the AGUImm    Instruction
    	if( bImmExt )  Reloc::template rela<32>(view, 0, 10, 0x0003C000, object, psymval, addend);
    	else           Reloc::template rela<32>(view, 0, 10, 0x01FFC000, object, psymval, addend);
        break;
    case elfcpp::R_MSPU_PC17 :    //This is the Jump/loop  Instruction,
         if( bImmExt ) Reloc::template pcrela<32>(view, 0,5, 0x000001E0, object, psymval, addend, address);
         else          Reloc::template pcrela<32>(view, 0,5, 0x003FFFE0, object, psymval, addend, address);
         break;
    case elfcpp::R_MSPU_17    :   //This is the CallM instruction
    	   Reloc::template mmpu_rela<32>(view, 0, 5, ( (1<<17)-1 )<< 5, object, psymval, addend); 
         break;
    case elfcpp::R_MSPU_28   :    //This is the Imm Extenion Instruction except the jump/seq
         Reloc::template rela<32>(view, 4, 0, 0x0FFFFFFF, object, psymval, addend); 
         break; 
    case elfcpp::R_MSPU_PC28   :  //This is the Imm Extenion Instruction for jump/seq
                                  //The address must sub 4 , which points to the origial instruction.
         Reloc::template pcrela<32>(view, 4, 0, 0x0FFFFFFF, object, psymval, addend, address -4); 
         break; 
    //the absolute funciton point relocation.
    case elfcpp::R_MSPU_8     :
    	 Reloc::template func_rela<32>(view,  0xFF, object, psymval, addend); break;
    case elfcpp::R_MSPU_16     :
    	 Reloc::template func_rela<32>(view,  0xFFFF, object, psymval, addend); break;
    case elfcpp::R_MSPU_32     :
    	 Reloc::template func_rela<32>(view,  0xFFFFFFFF, object, psymval, addend); break;
    case elfcpp::R_MSPU_64     :
    	 Reloc::template func_rela<64>(view,  0xFFFFFFFFFFFFFFFFL, object, psymval, addend); break;

    //the relative funciton point relocation.
    case elfcpp::R_MSPU_PC_8     :
    	 Reloc::template func_pc_rela<32>(view,  0xFF, object, psymval, addend,address); break;
    case elfcpp::R_MSPU_PC_16     :
    	 Reloc::template func_pc_rela<32>(view,  0xFFFF, object, psymval, addend,address); break;
    case elfcpp::R_MSPU_PC_32     :
    	 Reloc::template func_pc_rela<32>(view,  0xFFFFFFFF, object, psymval, addend,address); break;
    case elfcpp::R_MSPU_PC_64     :
    	 Reloc::template func_pc_rela<64>(view,  0xFFFFFFFFFFFFFFFFL, object, psymval, addend,address); break;
    default:
      break;
    }
  return true;
}
