// ***************************************************************************
// TITLE
//
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id$
// HISTORY
//      $Log$
// ***************************************************************************


#ifndef __SNARK_INTADC_H__
#define __SNARK_INTADC_H__

#include <arsenal.h>

#include <axthreads.h>

#include <customboard.h>

#include <singleton_template.hpp>

#include <LowPassFilter2p.hpp>

#ifndef CONF_ADC_CH_NUM
#define CONF_ADC_CH_NUM    4
#endif

// ---------------------------------------------------------------------------
// --------------------------------- CLASSES ---------------------------------
// -----|---------------|-----------------------(|--------------|-------------

class IntAdc: public SingletonTemplate<IntAdc>
{
        friend          void lowlevelAdcUpdater();

#if (TARGET_FAMILY != __AX_standalone__)
        HAXMUTEX        channelsLCK;
#endif
        UINT            channelsEnabled;
        UINT            channelsReady;
        math::LowPassFilter2p * channelFilters  [ CONF_ADC_CH_NUM ];

        INT             channelsValues          [ CONF_ADC_CH_NUM ];

protected:
        void            updateChannel           (UINT           channel,
                                                 INT            value);

public:
                        IntAdc                  ();
                        ~IntAdc                 ();

        BOOL            init                    (UINT           samplerate);

        void            closeChannel            (UINT           channel);

        void            closeAllChannels        ();

        BOOL            openChannel             (UINT           channel,
                                                 BOOL           useLPF      = false);

        BOOL            readChannel             (UINT           channel,
                                                 PINT           value);

        BOOL            readChannelDirect       (UINT           channel,
                                                 PINT           value,
                                                 UINT           TO);

        BOOL            startConversion         (BOOL           burst);

        void            stopConversion          ();
};

#endif // __SNARK_INTADC_H__
