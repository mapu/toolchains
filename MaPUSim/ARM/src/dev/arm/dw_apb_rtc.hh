/*
 * dw_apb_rtc.hh
 *
 *  Created on: Mar 20, 2014
 *      Author: wangl
 */

#ifndef DW_APB_RTC_HH_
#define DW_APB_RTC_HH_

#include "dev/arm/amba_device.hh"
#include "params/DWrtc.hh"

/** @file
 * This implements the DesignWare RTC
 */

class DWrtc : public AmbaIntDevice
{
  protected:
    enum {
        DataReg    = 0x00,
        MatchReg   = 0x04,
        LoadReg    = 0x08,
        ControlReg = 0x0C,
        MaskedISR  = 0x10,
        RawISR     = 0x14,
        IntClear   = 0x18,
    };

    /* Seconds since epoch that correspond to time simulation was started at the
     * begining of simulation and is then updated if ever written. */
    uint32_t timeVal;

    /* Time when the timeVal register was written */
    Tick lastWrittenTick;

    /* Previous load value */
    uint32_t loadVal;

    /* RTC Match Value
     * Cause an interrupt when this value hits counter
     */
    uint32_t matchVal;

    /** If timer has caused an interrupt. This is irrespective of
     * interrupt enable */
    bool rawInt;

    /** If the timer interrupt mask that is anded with the raw interrupt to
     * generate a pending interrupt
     */
    bool maskInt;

    /** Allow the timer to send interrupt to the raw interrupt */
    bool enableInt;

    /** If an interrupt is currently pending. Logical and of CTRL.intEnable
     * and rawInt */
    bool pendingInt;

    /** Called when the counter reaches matches */
    void counterMatch();
    EventWrapper<DWrtc, &DWrtc::counterMatch> matchEvent;

    /** Called to update the matchEvent when the load Value or match value are
     * written.
     */
    void resyncMatch();

  public:
    typedef DWrtcParams Params;
    const Params *
    params() const
    {
        return dynamic_cast<const Params *>(_params);
    }
    /**
      * The constructor for RealView just registers itself with the MMU.
      * @param p params structure
      */
    DWrtc(Params *p);

    /**
     * Handle a read to the device
     * @param pkt The memory request.
     * @param data Where to put the data.
     */
    virtual Tick read(PacketPtr pkt);

    /**
     * Handle writes to the device
     * @param pkt The memory request.
     * @param data the data
     */
    virtual Tick write(PacketPtr pkt);


    virtual void serialize(std::ostream &os);
    virtual void unserialize(Checkpoint *cp, const std::string &section);
};


#endif /* DW_APB_RTC_HH_ */
