/*
 * card_image.hh
 *
 *  Created on: Mar 25, 2014
 *      Author: wangl
 */

#ifndef CARD_IMAGE_HH_
#define CARD_IMAGE_HH_



#include <fstream>

#include "base/hashmap.hh"
#include "params/CardImage.hh"
#include "sim/sim_object.hh"

/**
 * Basic interface for accessing a disk image.
 */
class CardImage : public SimObject
{
  protected:
    bool initialized;
    mutable std::fstream stream;
    std::string file;
    bool readonly;
    mutable std::streampos disk_size;

  public:
    typedef CardImageParams Params;
    CardImage(const Params *p);
    virtual ~CardImage();

    void close();
    void open(const std::string &filename, bool rd_only = false);

    virtual std::streampos size() const;

    virtual std::streampos read(uint8_t *data, std::streampos offset, uint64_t count) const;
    virtual std::streampos write(const uint8_t *data, std::streampos offset, uint64_t count);
};


#endif /* CARD_IMAGE_HH_ */
