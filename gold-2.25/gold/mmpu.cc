// mmpu.cc -- mmpu target support for gold.

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
#include "mmpu.h"
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
namespace {

using namespace gold;

//The mmpu only supports 32bit address & little endian
template<int size, bool big_endian>
class Target_mmpu: public Sized_target<size, big_endian> {
public:
  typedef Output_data_reloc<elfcpp::SHT_RELA, true, size, big_endian> Reloc_section;

  //Construction function
  Target_mmpu() :
      Sized_target<size, big_endian>(&mmpu_info) {
  }

  /*************************************************/
  void gc_process_relocs(Symbol_table* symtab, Layout* layout,
      Sized_relobj_file<size, big_endian>* object, unsigned int data_shndx,
      unsigned int sh_type, const unsigned char* prelocs, size_t reloc_count,
      Output_section* output_section, bool needs_special_offset_handling,
      size_t local_symbol_count, const unsigned char* plocal_symbols);

  /*************************************************/
  void scan_relocs(Symbol_table* symtab, Layout* layout,
      Sized_relobj_file<size, big_endian>* object, unsigned int data_shndx,
      unsigned int sh_type, const unsigned char* prelocs, size_t reloc_count,
      Output_section* output_section, bool needs_special_offset_handling,
      size_t local_symbol_count, const unsigned char* plocal_symbols);

  /*************************************************/
  void relocate_section(const Relocate_info<size, big_endian> * relinfo,
      unsigned int sh_type, const unsigned char* prelocs, size_t reloc_count,
      Output_section* output_section, bool needs_special_offset_handling,
      unsigned char* view,
      typename elfcpp::Elf_types<size>::Elf_Addr view_address,
      section_size_type view_size,
      const Reloc_symbol_changes* reloc_symbol_changes);

  /*************************************************/
  void scan_relocatable_relocs(Symbol_table* symtab, Layout* layout,
      Sized_relobj_file<size, big_endian>* object, unsigned int data_shndx,
      unsigned int sh_type, const unsigned char* prelocs, size_t reloc_count,
      Output_section* output_section, bool needs_special_offset_handling,
      size_t local_symbol_count, const unsigned char* plocal_symbols,
      Relocatable_relocs* rr);

  /*************************************************/
  void relocate_relocs(const Relocate_info<size, big_endian>* relinfo,
      unsigned int sh_type, const unsigned char* prelocs, size_t reloc_count,
      Output_section* output_section,
      typename elfcpp::Elf_types<size>::Elf_Off offset_in_output_section,
      const Relocatable_relocs* rr, unsigned char* view,
      typename elfcpp::Elf_types<size>::Elf_Addr view_address,
      section_size_type view_size, unsigned char* reloc_view,
      section_size_type reloc_view_size);
  /*************************************************/
  static Target::Target_info mmpu_info;

private:
  class Relocatable_size_for_reloc {
  public:
    unsigned int get_size_for_reloc(unsigned int, Relobj*) {
      gold_unreachable();
      return 0;
    }
  }; //end of class Relocatable_size_for_reloc

  /*************************************************/
//  Scan Class for Scan_relocs
  /*************************************************/
private:
  class Scan {
  public:
    Scan() :
        issued_non_pic_error_(false) {
    }

    static inline int get_reference_flags(unsigned int r_type);

    //Scan a relocation  for local variable
    inline void local(Symbol_table* symtab, Layout* layout, Target_mmpu* target,
        Sized_relobj_file<size, big_endian>* object, unsigned int data_shndx,
        Output_section* output_section,
        const elfcpp::Rela<size, big_endian>& reloc, unsigned int r_type,
        const elfcpp::Sym<size, big_endian>& lsym, bool is_discarded);

    //Scan a relocation  for global variable
    inline void global(Symbol_table* symtab, Layout* layout,
        Target_mmpu* target, Sized_relobj_file<size, big_endian>* object,
        unsigned int data_shndx, Output_section* output_section,
        const elfcpp::Rela<size, big_endian>& reloc, unsigned int r_type,
        Symbol* gsym);

    inline bool local_reloc_may_be_function_pointer(Symbol_table*, Layout*,
        Target_mmpu*, Sized_relobj_file<size, big_endian>*, unsigned int,
        Output_section*, const elfcpp::Rela<size, big_endian>&, unsigned int,
        const elfcpp::Sym<size, big_endian>&) {
      return false;
    }

    inline bool global_reloc_may_be_function_pointer(Symbol_table*, Layout*,
        Target_mmpu*, Sized_relobj_file<size, big_endian>*, unsigned int,
        Output_section*, const elfcpp::Rela<size, big_endian>&, unsigned int,
        Symbol*) {
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
    inline bool relocate(const Relocate_info<size, big_endian>*, Target_mmpu*,
        Output_section*, size_t relnum, const elfcpp::Rela<size, big_endian>&,
        unsigned int r_type, const Sized_symbol<size>*,
        const Symbol_value<size>*, unsigned char*,
        typename elfcpp::Elf_types<size>::Elf_Addr, section_size_type);
  };
  //end of class Relocate

};
//end of class Target_mmpu

//////////////////////////////////////////////////////////////////////
template<>
Target::Target_info Target_mmpu<32, false>::mmpu_info = { 32,			// size
    false,		// is_big_endian
    elfcpp::EM_MMPU,		// machine_code
    false,		// has_make_symbol
    false,		// has_resolve
    false,		// has_code_fill
    true,			// is_default_stack_executable
    false,		// can_icf_inline_merge_sections
    '\0',			// wrap_char
    "",	// dynamic_linker
    0x0,		// default_text_segment_address
    0x100,		// abi_pagesize: 256-byte alignment (overridable by -z max-page-size)
    0x100,// common_pagesize: 256-byte alignment (overridable by -z common-page-size)
    false,                // isolate_execinstr
    0,                    // rosegment_gap
    elfcpp::SHN_UNDEF,	// small_common_shndx
    elfcpp::SHN_UNDEF,	// large_common_shndx
    0,      // small_common_section_flags
    0,      // large_common_section_flags
    ".MMPU.attributes",  // attributes_section
    "mmpuabi",    // attributes_vendor
    NULL    // entry_symbol_name
    };

///////////////////////////////////////////////////////////////////////////////
template<int size, bool big_endian>
class Target_selector_mmpu: public Target_selector {
public:
  Target_selector_mmpu() :
      Target_selector(elfcpp::EM_NONE, size, big_endian, "elf32-mmpu",
          "elf32mmpu") {
  }

  Target* do_recognize(int machine, int, int) {
    if (machine == elfcpp::EM_MMPU)
      return this->instantiate_target();
    else
      return NULL;
  }

  Target* do_instantiate_target() {
    return new Target_mmpu<size, big_endian>();
  }
};

Target_selector_mmpu<32, false> target_selector_mmpu32le;

} // End anonymous namespace.

template<int size, bool big_endian>
class mmpu_relocate_functions {
public:
  // Do a simple relocation using a symbol value with the addend in
  // the relocation.
  template<int valsize> static inline void rela(unsigned char* view,
      unsigned int right_shift, unsigned int left_shift,
      elfcpp::Elf_Xword dst_mask,
      const Sized_relobj_file<size, big_endian>* object,
      const Symbol_value<size>* psymval,
      typename elfcpp::Swap<valsize, big_endian>::Valtype addend) {

    typedef typename elfcpp::Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<valsize, big_endian>::readval(wv);
    Valtype reloc = (psymval->value(object, addend) >> right_shift);
    assert(reloc % 41 == 0);

    reloc = reloc / 41;        //41 is the instruction size of the microcode.
    reloc = reloc << left_shift;
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
void Target_mmpu<size, big_endian>::gc_process_relocs(Symbol_table* symtab,
    Layout* layout, Sized_relobj_file<size, big_endian>* object,
    unsigned int data_shndx, unsigned int sh_type, const unsigned char* prelocs,
    size_t reloc_count, Output_section* output_section,
    bool needs_special_offset_handling, size_t local_symbol_count,
    const unsigned char* plocal_symbols) {

  typedef Target_mmpu<size, big_endian> Mmpu;
  typedef typename Target_mmpu<size, big_endian>::Scan Scan;

  gold::gc_process_relocs<size, big_endian, Mmpu, elfcpp::SHT_RELA, Scan,
      typename Target_mmpu::Relocatable_size_for_reloc>(symtab, layout, this,
      object, data_shndx, prelocs, reloc_count, output_section,
      needs_special_offset_handling, local_symbol_count, plocal_symbols);
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
void Target_mmpu<size, big_endian>::scan_relocs(Symbol_table* symtab,
    Layout* layout, Sized_relobj_file<size, big_endian>* object,
    unsigned int data_shndx, unsigned int sh_type, const unsigned char* prelocs,
    size_t reloc_count, Output_section* output_section,
    bool needs_special_offset_handling, size_t local_symbol_count,
    const unsigned char* plocal_symbols) {

  typedef Target_mmpu<size, big_endian> Mmpu;
  typedef typename Target_mmpu<size, big_endian>::Scan Scan;

  if (sh_type == elfcpp::SHT_REL) {
    gold_error(_("%s: unsupported REL reloc section"), object->name().c_str());
    return;
  }

  gold::scan_relocs<size, big_endian, Mmpu, elfcpp::SHT_RELA, Scan>(symtab,
      layout, this, object, data_shndx, prelocs, reloc_count, output_section,
      needs_special_offset_handling, local_symbol_count, plocal_symbols);
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
void Target_mmpu<size, big_endian>::relocate_section(
    const Relocate_info<size, big_endian> * relinfo, unsigned int sh_type,
    const unsigned char* prelocs, size_t reloc_count,
    Output_section* output_section, bool needs_special_offset_handling,
    unsigned char* view,
    typename elfcpp::Elf_types<size>::Elf_Addr view_address,
    section_size_type view_size,
    const Reloc_symbol_changes* reloc_symbol_changes) {

  typedef Target_mmpu<size, big_endian> Mmpu;
  typedef typename Target_mmpu<size, big_endian>::Relocate Mmpu_relocate;

  gold_assert(sh_type == elfcpp::SHT_RELA);

  gold::relocate_section<size, big_endian, Mmpu, elfcpp::SHT_RELA,
                         Mmpu_relocate, gold::Default_comdat_behavior>(
    relinfo, this, prelocs, reloc_count, output_section,
    needs_special_offset_handling, view, view_address, view_size,
    reloc_symbol_changes);
}
/****************************************************************/
// Scan the relocs during a relocatable link.  The parameters are
// like scan_relocs, with an additional Relocatable_relocs
// parameter, used to record the disposition of the relocs.
/****************************************************************/

template<int size, bool big_endian>
void Target_mmpu<size, big_endian>::scan_relocatable_relocs(
    Symbol_table* symtab, Layout* layout,
    Sized_relobj_file<size, big_endian>* object, unsigned int data_shndx,
    unsigned int sh_type, const unsigned char* prelocs, size_t reloc_count,
    Output_section* output_section, bool needs_special_offset_handling,
    size_t local_symbol_count, const unsigned char* plocal_symbols,
    Relocatable_relocs* rr) {

  gold_assert(sh_type == elfcpp::SHT_RELA);

  typedef gold::Default_scan_relocatable_relocs<elfcpp::SHT_RELA,
      Relocatable_size_for_reloc> Scan_relocatable_relocs;

  gold::scan_relocatable_relocs<size, big_endian, elfcpp::SHT_RELA,
      Scan_relocatable_relocs>(symtab, layout, object, data_shndx, prelocs,
      reloc_count, output_section, needs_special_offset_handling,
      local_symbol_count, plocal_symbols, rr);
} //end of scan_relocatable_relocs
// Relocate a section during a relocatable link.

/****************************************************************/
// Relocate a section during a relocatable link.  The parameters are
// like relocate_section, with additional parameters for the view of
// the output reloc section.
/****************************************************************/
template<int size, bool big_endian>
void Target_mmpu<size, big_endian>::relocate_relocs(
    const Relocate_info<size, big_endian>* relinfo, unsigned int sh_type,
    const unsigned char* prelocs, size_t reloc_count,
    Output_section* output_section,
    typename elfcpp::Elf_types<size>::Elf_Off offset_in_output_section,
    const Relocatable_relocs* rr, unsigned char* view,
    typename elfcpp::Elf_types<size>::Elf_Addr view_address,
    section_size_type view_size, unsigned char* reloc_view,
    section_size_type reloc_view_size) {

  gold_assert(sh_type == elfcpp::SHT_RELA);

  gold::relocate_relocs<size, big_endian, elfcpp::SHT_RELA>(relinfo,
      prelocs, reloc_count, output_section, offset_in_output_section, rr, view,
      view_address, view_size, reloc_view, reloc_view_size);
}

/**********************************************************************/
//  Functions for Class Scan
/**********************************************************************/

//Get the Reference type according to the "r_type"
//Canditates inclues:
//  0, ABSOLUTE_REF, RELATIVE_REF, FUNCTION_REF, TSL_REF
template<int size, bool big_endian>
int Target_mmpu<size, big_endian>::Scan::get_reference_flags(
    unsigned int r_type) {
  return Symbol::ABSOLUTE_REF;
}

// Scan a relocation for a local symbol, but all code are related to 
// GOT or share libraries in Powerpc.cc
template<int size, bool big_endian>
inline void Target_mmpu<size, big_endian>::Scan::local(Symbol_table* symtab,
    Layout* layout, Target_mmpu* target,
    Sized_relobj_file<size, big_endian>* object, unsigned int data_shndx,
    Output_section* output_section, const elfcpp::Rela<size, big_endian>& reloc,
    unsigned int r_type, const elfcpp::Sym<size, big_endian>& lsym,
    bool is_discarded) {

  switch (r_type) {
  case elfcpp::R_MMPU_11:
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
inline void Target_mmpu<size, big_endian>::Scan::global(Symbol_table* symtab,
    Layout* layout, Target_mmpu* target,
    Sized_relobj_file<size, big_endian>* object, unsigned int data_shndx,
    Output_section* output_section, const elfcpp::Rela<size, big_endian>& reloc,
    unsigned int r_type, Symbol* gsym) {
  switch (r_type) {
  case elfcpp::R_MMPU_11:
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
template<int size, bool big_endian> inline bool Target_mmpu<size, big_endian>::Relocate::relocate(
    const Relocate_info<size, big_endian>* relinfo, Target_mmpu* target,
    Output_section*, size_t relnum, const elfcpp::Rela<size, big_endian>& rela,
    unsigned int r_type, const Sized_symbol<size>* gsym,
    const Symbol_value<size>* psymval, unsigned char* view,
    typename elfcpp::Elf_types<size>::Elf_Addr address,
    section_size_type /* view_size */) {

  typedef mmpu_relocate_functions<size, big_endian> Reloc;

  const Sized_relobj_file<size, big_endian>* object = relinfo->object;
  elfcpp::Elf_Xword addend = rela.get_r_addend();

  switch (r_type) {
  case elfcpp::R_MMPU_11:
    /****************************************************************/
    /* This is only used for Micorcode Jump & Loop Instructions     */
    //  -------------------------------------------------------------
    //MSB|    B40    |    B39    |    B38    |    B37    |    B36    |LSB
    //  -------------------------------------------------------------
    //                 -------17                         0
    //  The View is the address of B37
    //  The Imm11 is locate in 26-17
    /* Refer to the Instruction encoding document                   */
    Reloc::template rela<32>(view, 0, 17, 0x0FFE0000, object, psymval, addend);
    break;
  default:
    break;
  }
  return true;
}
