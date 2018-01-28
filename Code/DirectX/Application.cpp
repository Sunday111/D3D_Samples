#include "Application.h"

#define MAXSAMPLES 100
static int tickindex = 0;
static long long ticksum = 0;
static long long ticklist[MAXSAMPLES]{};

/* need to zero out the ticklist array before starting */
/* average will ramp up until the buffer is full */
/* returns average ticks per frame over the MAXSAMPLES last frames */

float CalcAverageTick(long long newtick)
{
    ticksum -= ticklist[tickindex];  /* subtract value falling off */
    ticksum += newtick;              /* add new value */
    ticklist[tickindex] = newtick;   /* save new value so it can be subtracted later */
    if (++tickindex == MAXSAMPLES)    /* inc buffer index */
        tickindex = 0;

    /* return average */
    return((float)ticksum / (float)MAXSAMPLES);
}