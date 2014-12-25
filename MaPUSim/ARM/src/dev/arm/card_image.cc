/*
 * card_image.cc
 *
 *  Created on: Mar 25, 2014
 *      Author: wangl
 */

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <fstream>
#include <string>

#include "base/callback.hh"
#include "base/misc.hh"
#include "base/trace.hh"
#include "debug/CardImageRead.hh"
#include "debug/CardImageWrite.hh"
#include "dev/arm/card_image.hh"
#include "sim/byteswap.hh"
#include "sim/sim_exit.hh"

using namespace std;

////////////////////////////////////////////////////////////////////////
//
// Card Disk image
//
CardImage::CardImage(const Params* p) :
  SimObject(p), initialized(false), disk_size(0) {
  open(p->image_file, p->read_only);
}

CardImage::~CardImage() {
  close();
}

void CardImage::open(const string &filename, bool rd_only) {
  if (!filename.empty()) {
    initialized = true;
    readonly = rd_only;
    file = filename;

    ios::openmode mode = ios::in | ios::binary;
    if (!readonly) mode |= ios::out;
    stream.open(file.c_str(), mode);
    if (!stream.is_open()) warn("Error opening %s", filename);
  }
}

void CardImage::close() {
  stream.close();
}

std::streampos CardImage::size() const {
  if (disk_size == 0) {
    if (!stream.is_open()) panic("file not open!\n");
      stream.seekg(0, ios::end);
      disk_size = stream.tellg();
  }

  return disk_size;
}

std::streampos CardImage::read(uint8_t *data, std::streampos offset, uint64_t count) const {
  if (!initialized) panic("CardDiskImage not initialized");

  if (!stream.is_open()) panic("file not open!\n");

  stream.seekg(offset, ios::beg);
  if (!stream.good()) panic("Could not seek to location in file %#x", offset);

  streampos pos = stream.tellg();
  stream.read((char *)data, count);

  DPRINTF(CardImageRead, "read: offset=%d\n", (uint64_t)offset);
  DDUMP(CardImageRead, data, count);

  return stream.tellg() - pos;
}

std::streampos CardImage::write(const uint8_t *data, std::streampos offset, uint64_t count) {
  if (!initialized) panic("CardDiskImage not initialized");

  if (readonly) panic("Cannot write to a read only disk image");

  if (!stream.is_open()) panic("file not open!\n");

  stream.seekp(offset, ios::beg);
  if (!stream.good()) panic("Could not seek to location in file %#x", offset);

  DPRINTF(CardImageWrite, "write: offset=%d, count=%d\n", (uint64_t)offset, count);
  DDUMP(CardImageWrite, data, count);

  streampos pos = stream.tellp();
  stream.write((const char *)data, count);
  return stream.tellp() - pos;
}

CardImage *CardImageParams::create() {
  return new CardImage(this);
}

