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

#include <math.h>
#include <declination.h>

#include <limits>


// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

#include "igrfdata.inc"

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

static double julday(int day, int month, int year)
{
  int days[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

  int leap_year = ( ((year % 4) == 0) &&
                    (((year % 100) != 0) || ((year % 400) == 0)) );

  double day_in_year = (days[month - 1] + day + (month > 2 ? leap_year : 0));

  return ((double)year + (day_in_year / (365.0 + leap_year)));
}
static double degrees_to_decimal(int degrees,int minutes,int seconds)
{
  double deg;
  double min;
  double sec;
  double decimal;

  deg = degrees;
  min = minutes/60.0;
  sec = seconds/3600.0;

  decimal = fabs(sec) + fabs(min) + fabs(deg);

  if (deg < 0) {
    decimal = -decimal;
  } else if (deg == 0){
    if (min < 0){
      decimal = -decimal;
    } else if (min == 0){
      if (sec<0){
        decimal = -decimal;
      }
    }
  }

  return(decimal);
}
int DeclinationCalc::shval3(int igdgc, double flat, double flon, double elev, int nmax, int gh, int iext, double ext1, double ext2, double ext3)
{
  double earths_radius = 6371.2;
  double dtr = 0.01745329;
  double slat;
  double clat;
  double ratio;
  double aa, bb, cc, dd;
  double sd;
  double cd;
  double r;
  double a2;
  double b2;
  double rr = 0;
  double fm,fn = 0;
  double sl[14];
  double cl[14];
  double p[119];
  double q[119];
  int ii,j,k,l,m,n;
  int npq;
  int ios;
  double argument;
  double power;
  a2 = 40680631.59;            /* WGS84 */
  b2 = 40408299.98;            /* WGS84 */
  ios = 0;
  r = elev;
  argument = flat * dtr;
  slat = sin( argument );
  if ((90.0 - flat) < 0.001)
    {
      aa = 89.999;            /*  300 ft. from North pole  */
    }
  else
    {
      if ((90.0 + flat) < 0.001)
        {
          aa = -89.999;        /*  300 ft. from South pole  */
        }
      else
        {
          aa = flat;
        }
    }
  argument = aa * dtr;
  clat = cos( argument );
  argument = flon * dtr;
  sl[1] = sin( argument );
  cl[1] = cos( argument );
  switch(gh)
    {
    case 3:  x = 0;
      y = 0;
      z = 0;
      break;
    case 4:  xtemp = 0;
      ytemp = 0;
      ztemp = 0;
      break;
    default:
        //printf("\nError in subroutine shval3");
      break;
    }
  sd = 0.0;
  cd = 1.0;
  l = 1;
  n = 0;
  m = 1;
  npq = (nmax * (nmax + 3)) / 2;
  if (igdgc == 1)
    {
      aa = a2 * clat * clat;
      bb = b2 * slat * slat;
      cc = aa + bb;
      argument = cc;
      dd = sqrt( argument );
      argument = elev * (elev + 2.0 * dd) + (a2 * aa + b2 * bb) / cc;
      r = sqrt( argument );
      cd = (elev + dd) / r;
      sd = (a2 - b2) / dd * slat * clat / r;
      aa = slat;
      slat = slat * cd - clat * sd;
      clat = clat * cd + aa * sd;
    }
  ratio = earths_radius / r;
  argument = 3.0;
  aa = sqrt( argument );
  p[1] = 2.0 * slat;
  p[2] = 2.0 * clat;
  p[3] = 4.5 * slat * slat - 1.5;
  p[4] = 3.0 * aa * clat * slat;
  q[1] = -clat;
  q[2] = slat;
  q[3] = -3.0 * clat * slat;
  q[4] = aa * (slat * slat - clat * clat);
  for ( k = 1; k <= npq; ++k)
    {
      if (n < m)
        {
          m = 0;
          n = n + 1;
          argument = ratio;
          power =  n + 2;
          rr = pow(argument,power);
          fn = n;
        }
      fm = m;
      if (k >= 5)
        {
          if (m == n)
            {
              argument = (1.0 - 0.5/fm);
              aa = sqrt( argument );
              j = k - n - 1;
              p[k] = (1.0 + 1.0/fm) * aa * clat * p[j];
              q[k] = aa * (clat * q[j] + slat/fm * p[j]);
              sl[m] = sl[m-1] * cl[1] + cl[m-1] * sl[1];
              cl[m] = cl[m-1] * cl[1] - sl[m-1] * sl[1];
            }
          else
            {
              argument = fn*fn - fm*fm;
              aa = sqrt( argument );
              argument = ((fn - 1.0)*(fn-1.0)) - (fm * fm);
              bb = sqrt( argument )/aa;
              cc = (2.0 * fn - 1.0)/aa;
              ii = k - n;
              j = k - 2 * n + 1;
              p[k] = (fn + 1.0) * (cc * slat/fn * p[ii] - bb/(fn - 1.0) * p[j]);
              q[k] = cc * (slat * q[ii] - clat/fn * p[ii]) - bb * q[j];
            }
        }
      switch(gh)
        {
        case 3:  aa = rr * gha[l];
          break;
        case 4:  aa = rr * ghb[l];
          break;
        default:
            //printf("\nError in subroutine shval3");
          break;
        }
      if (m == 0)
        {
          switch(gh)
            {
            case 3:  x = x + aa * q[k];
              z = z - aa * p[k];
              break;
            case 4:  xtemp = xtemp + aa * q[k];
              ztemp = ztemp - aa * p[k];
              break;
            default:
                //printf("\nError in subroutine shval3");
              break;
            }
          l = l + 1;
        }
      else
        {
          switch(gh)
            {
            case 3:  bb = rr * gha[l+1];
              cc = aa * cl[m] + bb * sl[m];
              x = x + cc * q[k];
              z = z - cc * p[k];
              if (clat > 0)
                {
                  y = y + (aa * sl[m] - bb * cl[m]) *
                    fm * p[k]/((fn + 1.0) * clat);
                }
              else
                {
                  y = y + (aa * sl[m] - bb * cl[m]) * q[k] * slat;
                }
              l = l + 2;
              break;
            case 4:  bb = rr * ghb[l+1];
              cc = aa * cl[m] + bb * sl[m];
              xtemp = xtemp + cc * q[k];
              ztemp = ztemp - cc * p[k];
              if (clat > 0)
                {
                  ytemp = ytemp + (aa * sl[m] - bb * cl[m]) *
                    fm * p[k]/((fn + 1.0) * clat);
                }
              else
                {
                  ytemp = ytemp + (aa * sl[m] - bb * cl[m]) *
                    q[k] * slat;
                }
              l = l + 2;
              break;
            default:
                //printf("\nError in subroutine shval3");
              break;
            }
        }
      m = m + 1;
    }
  if (iext != 0)
    {
      aa = ext2 * cl[1] + ext3 * sl[1];
      switch(gh)
        {
        case 3:   x = x - ext1 * clat + aa * slat;
          y = y + ext2 * sl[1] - ext3 * cl[1];
          z = z + ext1 * slat + aa * clat;
          break;
        case 4:   xtemp = xtemp - ext1 * clat + aa * slat;
          ytemp = ytemp + ext2 * sl[1] - ext3 * cl[1];
          ztemp = ztemp + ext1 * slat + aa * clat;
          break;
        default:
            //printf("\nError in subroutine shval3");
          break;
        }
    }
  switch(gh)
    {
    case 3:   aa = x;
                x = x * cd + z * sd;
                z = z * cd - aa * sd;
                break;
    case 4:   aa = xtemp;
                xtemp = xtemp * cd + ztemp * sd;
                ztemp = ztemp * cd - aa * sd;
                break;
    default:
        //printf("\nError in subroutine shval3");
                break;
    }
  return(ios);
}
int DeclinationCalc::dihf (int gh)
{
  int ios;
  int j;
  double sn;
  double h2;
  double hpx;
  double argument, argument2;

  ios = gh;
  sn = 0.0001;

  switch(gh)
    {
    case 3:   for (j = 1; j <= 1; ++j)
        {
          h2 = x*x + y*y;
          argument = h2;
          h = sqrt(argument);       /* calculate horizontal intensity */
          argument = h2 + z*z;
          f = sqrt(argument);      /* calculate total intensity */
          if (f < sn)
            {
              d = std::numeric_limits<double>::quiet_NaN();        /* If d and i cannot be determined, */
              i = std::numeric_limits<double>::quiet_NaN();        /*       set equal to NaN         */
            }
          else
            {
              argument = z;
              argument2 = h;
              i = atan2(argument,argument2);
              if (h < sn)
                {
                  d = std::numeric_limits<double>::quiet_NaN();
                }
              else
                {
                  hpx = h + x;
                  if (hpx < sn)
                    {
                      d = PI;
                    }
                  else
                    {
                      argument = y;
                      argument2 = hpx;
                      d = 2.0 * atan2(argument,argument2);
                    }
                }
            }
        }
    break;
case 4:   for (j = 1; j <= 1; ++j)
{
h2 = xtemp*xtemp + ytemp*ytemp;
argument = h2;
htemp = sqrt(argument);
argument = h2 + ztemp*ztemp;
ftemp = sqrt(argument);
if (ftemp < sn)
{
  dtemp = std::numeric_limits<double>::quiet_NaN();    /* If d and i cannot be determined, */
  itemp = std::numeric_limits<double>::quiet_NaN();    /*       set equal to 999.0         */
}
else
{
  argument = ztemp;
  argument2 = htemp;
  itemp = atan2(argument,argument2);
  if (htemp < sn)
    {
      dtemp = std::numeric_limits<double>::quiet_NaN();
    }
  else
    {
      hpx = htemp + xtemp;
      if (hpx < sn)
        {
          dtemp = PI;
        }
      else
        {
          argument = ytemp;
          argument2 = hpx;
          dtemp = 2.0 * atan2(argument,argument2);
        }
    }
}
}
    break;
default:
    //printf("\nError in subroutine dihf");
    break;
}
return(ios);
}



CalcResult::CalcResult()
{
  declination = std::numeric_limits<double>::quiet_NaN();
  annualDeclinationDelta = std::numeric_limits<double>::quiet_NaN();
  inclination = std::numeric_limits<double>::quiet_NaN();
  annualInclinationDelta = std::numeric_limits<double>::quiet_NaN();
  horizontalIntensity = std::numeric_limits<double>::quiet_NaN();
  annualHorizontalChange = std::numeric_limits<double>::quiet_NaN();
  xIntensity = std::numeric_limits<double>::quiet_NaN();
  annualChangeOfXIntensity = std::numeric_limits<double>::quiet_NaN();
  yIntensity = std::numeric_limits<double>::quiet_NaN();
  annualChangeOfYIntensity = std::numeric_limits<double>::quiet_NaN();
  zIntensity = std::numeric_limits<double>::quiet_NaN();
  annualChangeOfZIntensity = std::numeric_limits<double>::quiet_NaN();
  totalField = std::numeric_limits<double>::quiet_NaN();
  annualChangeOfTotalField = std::numeric_limits<double>::quiet_NaN();
}
int DeclinationCalc::interpsh(double date, double dte1, int nmax1, double dte2, int nmax2, int gh)
{
  int   nmax;
  int   k, l;
  int   ii;
  double factor;

  factor = (date - dte1) / (dte2 - dte1);
  if (nmax1 == nmax2)
    {
      k =  nmax1 * (nmax1 + 2);
      nmax = nmax1;
    }
  else
    {
      if (nmax1 > nmax2)
        {
          k = nmax2 * (nmax2 + 2);
          l = nmax1 * (nmax1 + 2);
          switch(gh)
            {
            case 3:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  gha[ii] = gh1[ii] + factor * (-gh1[ii]);
                }
              break;
            case 4:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  ghb[ii] = gh1[ii] + factor * (-gh1[ii]);
                }
              break;
            default:
                //printf("\nError in subroutine extrapsh");
              break;
            }
          nmax = nmax1;
        }
      else
        {
          k = nmax1 * (nmax1 + 2);
          l = nmax2 * (nmax2 + 2);
          switch(gh)
            {
            case 3:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  gha[ii] = factor * gh2[ii];
                }
              break;
            case 4:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  ghb[ii] = factor * gh2[ii];
                }
              break;
            default:
                //printf("\nError in subroutine extrapsh");
              break;
            }
          nmax = nmax2;
        }
    }
  switch(gh)
    {
    case 3:  for ( ii = 1; ii <= k; ++ii)
        {
          gha[ii] = gh1[ii] + factor * (gh2[ii] - gh1[ii]);
        }
      break;
    case 4:  for ( ii = 1; ii <= k; ++ii)
        {
          ghb[ii] = gh1[ii] + factor * (gh2[ii] - gh1[ii]);
        }
      break;
    default:
        //printf("\nError in subroutine extrapsh");
      break;
    }
  return(nmax);
}
int DeclinationCalc::getshc(ModelData * modelData, int modelSize, int iflag, int nmax_of_gh, int gh)
{
//  char  inbuff[MAXINBUFF];
  int ii,m,n,mm,nn;
  int ios;
//  int line_num;
  double g,hh;
//  double trash;
  int idx = 0;

  ii = 0;
  ios = 0;

  for (nn = 1; (idx < modelSize) && (nn <= nmax_of_gh); ++nn)
    {
      for (mm = 0; (idx < modelSize) && (mm <= nn); ++mm)
        {
          if (iflag == 1)
            {

//              fgets(inbuff, MAXREAD, stream);
//              sscanf(inbuff, "%d%d%lg%lg%lg%lg%s%d",
//                     &n, &m, &g, &hh, &trash, &trash, irat, &line_num);

              n         = modelData[idx].f1;
              m         = modelData[idx].f2;
              g         = modelData[idx].f3;
              hh        = modelData[idx].f4;
//              line_num  = modelData[idx].f8;

              // printf("getshc stage 1: %f %f %f %f\n", n, m, g, hh);
            }
          else
            {
//              fgets(inbuff, MAXREAD, stream);
//              sscanf(inbuff, "%d%d%lg%lg%lg%lg%s%d",
//                     &n, &m, &trash, &trash, &g, &hh, irat, &line_num);
              n         = modelData[idx].f1;
              m         = modelData[idx].f2;
              g         = modelData[idx].f5;
              hh        = modelData[idx].f6;

              // printf("getshc stage 2: %f %f %f %f\n", n, m, g, hh);
            }

          idx++;

          if ((nn != n) || (mm != m))
            {
              ios = -2;
              //fclose(stream);
              return(ios);
            }
          ii = ii + 1;
          switch(gh)
            {
            case 1:  gh1[ii] = g;
              break;
            case 2:  gh2[ii] = g;
              break;
            default:
                //printf("\nError in subroutine getshc");
              break;
            }
          if (m != 0)
            {
              ii = ii+ 1;
              switch(gh)
                {
                case 1:  gh1[ii] = hh;
                  break;
                case 2:  gh2[ii] = hh;
                  break;
                default:
                    //printf("\nError in subroutine getshc");
                  break;
                }
            }
        }
    }

  return(ios);
}
// ***************************************************************************
// TYPE
//      Constructor
// FUNCTION
//      DeclinationCalc::DeclinationCalc
// PURPOSE
//      Class construction
// PARAMETERS
//          --
// ***************************************************************************
DeclinationCalc::DeclinationCalc()
{
    minyr               = 1000000000;
    maxyr               = -1000000000;
    modelInfosCount     = 0;
    modelInfos          = nil;

    d=0;
    f=0;
    h=0;
    i=0;
    x=0;
    y=0;
    z=0;
}
// ***************************************************************************
// FUNCTION
//      DeclinationCalc::init
// PURPOSE
//
// PARAMETERS
//          --
// RESULT
//      bool --
// ***************************************************************************
bool DeclinationCalc::init()
{
    modelInfosCount = sizeof(mModelInfos) / sizeof(ModelInfo);
    modelInfos      = &mModelInfos[0];

    for (int i = 0; i < modelInfosCount; i++)
    {
        if (modelInfos[i].yrmin < minyr)
          minyr = modelInfos[i].yrmin;

        if (modelInfos[i].yrmax > maxyr)
          maxyr = modelInfos[i].yrmax;
    }

  return true;
} // method-end: init(...)
int DeclinationCalc::extrapsh(double date, double dte1, int nmax1, int nmax2, int gh)
{
  int   nmax;
  int   k, l;
  int   ii;
  double factor;

  factor = date - dte1;
  if (nmax1 == nmax2)
    {
      k =  nmax1 * (nmax1 + 2);
      nmax = nmax1;
    }
  else
    {
      if (nmax1 > nmax2)
        {
          k = nmax2 * (nmax2 + 2);
          l = nmax1 * (nmax1 + 2);
          switch(gh)
            {
            case 3:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  gha[ii] = gh1[ii];
                }
              break;
            case 4:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  ghb[ii] = gh1[ii];
                }
              break;
            default:
                //printf("\nError in subroutine extrapsh");
              break;
            }
          nmax = nmax1;
        }
      else
        {
          k = nmax1 * (nmax1 + 2);
          l = nmax2 * (nmax2 + 2);
          switch(gh)
            {
            case 3:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  gha[ii] = factor * gh2[ii];
                }
              break;
            case 4:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  ghb[ii] = factor * gh2[ii];
                }
              break;
            default:
                //printf("\nError in subroutine extrapsh");
              break;
            }
          nmax = nmax2;
        }
    }
    switch(gh)
    {
    case 3:  for ( ii = 1; ii <= k; ++ii)
        {
          gha[ii] = gh1[ii] + factor * gh2[ii];
        }
      break;
    case 4:  for ( ii = 1; ii <= k; ++ii)
        {
          ghb[ii] = gh1[ii] + factor * gh2[ii];
        }
      break;
    default:
        //printf("\nError in subroutine extrapsh");
      break;
    }
    return(nmax);
}
bool DeclinationCalc::getDeclination(CalcResult& result, double latitude, double longitude, double altitude /*= 0.0*/,
                                 PAXDATE curDate /*= boost::gregorian::day_clock::local_day()*/ )
{
    BOOL            b_result          = false;
    int             rc;

    ENTER(true);

    // получаем дату в форме года (с днем и месяцем закодированным как доля года)
    double sdate = julday(curDate->day, curDate->month, curDate->year);

    //printf("DeclinationCalc::getDeclination sdate = %f\n", sdate);

    // выбираем модель подходящую для заданной даты
    ModelInfo * found       = nil;
    int         foundIdx;

    for (foundIdx = 0; !found && (foundIdx < modelInfosCount); foundIdx++)
    {
        if (sdate < modelInfos[foundIdx].yrmax)
        {
            found = &modelInfos[foundIdx];
        }
    }

    if (found)
    {
        // загружаем данные модели и производим расчеты
        if (found->max2 == 0)
        {
            if ((rc = getshc(found->data, found->size, 1, found->max1, 1)) == 0)
            {
                if (foundIdx < modelInfosCount)
                {
                    ModelInfo * next = found + 1;

                    getshc(next->data, next->size, 1, next->max1, 2);

                    nmax        = interpsh(sdate, found->yrmin, found->max1, next->yrmin, next->max1, 3);
                    nmax        = interpsh(sdate + 1, found->yrmin, found->max1, next->yrmin, next->max1, 4);

                    b_result    = true;
                }
            }
//            else
//                printf("rc 1 = %d\n", rc);

        }
        else
        {
            if ((rc = getshc(found->data, found->size, 1, found->max1, 1)) == 0)
            {
                if ((rc = getshc(found->data, found->size, 0, found->max2, 2)) == 0)
                {
                    nmax        = extrapsh(sdate, found->epoch, found->max1, found->max2, 3);
                    nmax        = extrapsh(sdate+1, found->epoch, found->max1, found->max2, 4);

                    b_result    = true;
                }
//                else
//                    printf("rc 3 = %d\n", rc);
            }
//            else
//                printf("rc 2 = %d\n", rc);
        }

        if (b_result)
        {
            /* Do the first calculations */
            shval3(1 /*geodetic coords*/, latitude, longitude, altitude, nmax, 3, IEXT, EXT_COEFF1, EXT_COEFF2, EXT_COEFF3);
            dihf(3);
            shval3(1 /*geodetic coords*/, latitude, longitude, altitude, nmax, 4, IEXT, EXT_COEFF1, EXT_COEFF2, EXT_COEFF3);
            dihf(4);

            /* Do the annual rates calculations */
            double ddot; // annual rate of change of declination (arc-min/yr)
            double idot; // annual rate of change of inclination (arc-min/yr).
            double hdot;
            double xdot;
            double ydot;
            double zdot;
            double fdot;

            ddot = ((dtemp - d)*RAD2DEG);
            if (ddot > 180.0) ddot -= 360.0;
            if (ddot <= -180.0) ddot += 360.0;
            ddot *= 60.0;

            idot = ((itemp - i)*RAD2DEG)*60;
            d = d*(RAD2DEG);   i = i*(RAD2DEG);

            hdot = htemp - h;   xdot = xtemp - x;
            ydot = ytemp - y;   zdot = ztemp - z;
            fdot = ftemp - f;

            /* deal with geographic and magnetic poles */
            if (h < 100.0) /* at magnetic poles */
            {
                d = std::numeric_limits<double>::quiet_NaN();
                ddot = std::numeric_limits<double>::quiet_NaN();
                /* while rest is ok */
            }

            if (90.0-fabs(latitude) <= 0.001) /* at geographic poles */
            {
                x = std::numeric_limits<double>::quiet_NaN();
                y = std::numeric_limits<double>::quiet_NaN();
                d = std::numeric_limits<double>::quiet_NaN();
                xdot = std::numeric_limits<double>::quiet_NaN();
                ydot = std::numeric_limits<double>::quiet_NaN();
                ddot = std::numeric_limits<double>::quiet_NaN();
                /* while rest is ok */
            }

            // записываем результаты расчетов в результирующую структуру
            result.declination = d;
            result.annualDeclinationDelta = degrees_to_decimal( 0, ddot, 0 ); // ddot измеряется в "угловых минутах в год"; переводим в градусы
            result.inclination = i;
            result.annualInclinationDelta = degrees_to_decimal( 0, idot, 0 ); // idot измеряется в "угловых минутах в год"; переводим в градусы
            result.horizontalIntensity = h;
            result.annualHorizontalChange = hdot;
            result.xIntensity = x;
            result.annualChangeOfXIntensity = xdot;
            result.yIntensity = y;
            result.annualChangeOfYIntensity = ydot;
            result.zIntensity = z;
            result.annualChangeOfZIntensity = zdot;
            result.totalField = f;
            result.annualChangeOfTotalField = fdot;
        }
    }
//    else
//        printf("not found\n");

    RETURN(b_result);
}
