#include "gpu_getrates.H"

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

static AMREX_GPU_DEVICE_MANAGED double molecular_masses[116] = {1.00797, 2.01594, 15.9994, 31.9988, 
  17.00737, 18.01534, 33.00677, 34.01474, 28.01055, 44.00995, 30.02649, 
  29.01852, 46.02589, 32.04243, 48.04183, 47.03386, 16.04303, 15.03506, 
  30.07012, 29.06215, 28.05418, 27.04621, 26.03824, 44.05358, 43.04561, 
  42.03764, 41.02967, 75.04441, 62.06892000000001, 61.06095000000001, 58.08067, 
  89.0715, 90.07947, 56.06473, 55.05676, 58.08067, 42.08127, 41.0733, 40.06533, 
  40.06533, 39.05736, 75.08804000000001, 75.08804000000001, 56.06473, 56.10836, 
  57.11633, 55.10039, 54.09242, 89.11513000000001, 71.09979, 72.10776, 56.10836, 
  55.10039, 89.11513000000001, 89.11513000000001, 72.10776, 71.09979, 121.11393, 
  104.10656, 71.09979, 70.09182, 69.08385, 88.10716000000001, 103.09859, 
  70.13545000000001, 69.12748000000001, 69.12748000000001, 117.16931, 86.13485, 
  100.20557, 98.18963000000001, 98.18963000000001, 98.18963000000001, 
  97.18166000000001, 131.1964, 131.1964, 131.1964, 131.1964, 163.1952, 163.1952, 
  163.1952, 163.1952, 163.1952, 114.18903, 114.18903, 114.18903, 146.18783, 
  88.10716000000001, 57.0727, 55.10039, 98.18963000000001, 98.18963000000001, 
  97.18166000000001, 97.18166000000001, 131.1964, 82.14660000000001, 
  81.13863000000001, 81.13863000000001, 71.14342000000001, 86.13485, 114.23266, 
  112.21672, 112.21672, 145.22349, 145.22349, 128.21612, 128.21612, 160.21492, 
  177.22229, 177.22229, 145.22349, 160.21492, 163.1952, 163.1952, 146.18783, 
  28.0134}; 


static AMREX_GPU_DEVICE_MANAGED double recip_molecular_masses[116] = {0.9920930186414277, 
  0.4960465093207139, 0.06250234383789392, 0.03125117191894696, 
  0.05879803873262004, 0.05550825019122593, 0.03029681486555637, 
  0.02939901936631002, 0.03570083414998991, 0.02272213442641948, 
  0.03330392596670473, 0.03446075127194632, 0.02172690196756651, 
  0.03120861932131863, 0.02081519375927187, 0.021261278576753, 
  0.06233236489615739, 0.06651120780362699, 0.03325560390181349, 
  0.0344090165386938, 0.03564531203549703, 0.0369737571363973, 
  0.0384050534905585, 0.02269963076780593, 0.02323117270262868, 
  0.02378820504671527, 0.02437260645771706, 0.01332544289441412, 
  0.01611112292593459, 0.01637707896781822, 0.01721743223692151, 
  0.01122693566404518, 0.01110130865556824, 0.01783652574443861, 
  0.01816307388956415, 0.01721743223692151, 0.02376354135699802, 
  0.02434671672351625, 0.02495923532889907, 0.02495923532889907, 
  0.02560336899370566, 0.01331770012907515, 0.01331770012907515, 
  0.01783652574443861, 0.01782265601774851, 0.01750812771058645, 
  0.01814869186951308, 0.01848687856819865, 0.01122143905305418, 
  0.01406473915042506, 0.01386813291662368, 0.01782265601774851, 
  0.01814869186951308, 0.01122143905305418, 0.01122143905305418, 
  0.01386813291662368, 0.01406473915042506, 8.256688557625038e-03, 
  9.605542628629742e-03, 0.01406473915042506, 0.01426700005792402, 
  0.01447516315318269, 0.01134981538390296, 9.699453697669386e-03, 
  0.01425812481419881, 0.01446602711396394, 0.01446602711396394, 
  8.534658094342281e-03, 0.01160970269292859, 9.979485172331238e-03, 
  0.01018437486728486, 0.01018437486728486, 0.01018437486728486, 
  0.01029000739439931, 7.622160364156333e-03, 7.622160364156333e-03, 
  7.622160364156333e-03, 7.622160364156333e-03, 6.127631204839358e-03, 
  6.127631204839358e-03, 6.127631204839358e-03, 6.127631204839358e-03, 
  6.127631204839358e-03, 8.757408658257278e-03, 8.757408658257278e-03, 
  8.757408658257278e-03, 6.840514699479429e-03, 0.01134981538390296, 
  0.01752151203640269, 0.01814869186951308, 0.01018437486728486, 
  0.01018437486728486, 0.01029000739439931, 0.01029000739439931, 
  7.622160364156333e-03, 0.01217335836175812, 0.01232458571213243, 
  0.01232458571213243, 0.01405611369259448, 0.01160970269292859, 
  8.754063855293223e-03, 8.911328008874257e-03, 8.911328008874257e-03, 
  6.885938356115805e-03, 6.885938356115805e-03, 7.799331316530245e-03, 
  7.799331316530245e-03, 6.241615949376e-03, 5.642631070843289e-03, 
  5.642631070843289e-03, 6.885938356115805e-03, 6.241615949376e-03, 
  6.127631204839358e-03, 6.127631204839358e-03, 6.840514699479429e-03, 
  0.03569720205330306}; 


AMREX_GPU_GLOBAL void
gpu_getrates(const double *temperature_array, const double *pressure_array, 
  const double *avmolwt_array, const double *mass_frac_array, const double 
  *diffusion_array, const double dt, const int spec_stride/*NX*NY*NZ in number of doubles*/,  
  double *wdot_array) 
{
  
  const double PA = 1.013250e+06;
  const double R0 = 8.314510e+07;
  const double R0c = 1.9872155832;
  const double DLn10 = 2.3025850929940459e0;
  
  {
    const int offset = (blockIdx.x*blockDim.x + threadIdx.x);
    temperature_array += offset;
    pressure_array += offset;
    avmolwt_array += offset;
    mass_frac_array += offset;
    diffusion_array += offset;
    wdot_array += offset;
  }
  double temperature;
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(temperature) : 
    "l"(temperature_array) : "memory"); 
  const double otc     = 1.0 / temperature;
  const double ortc    = 1.0 / (temperature * R0c);
  const double vlntemp = log(temperature);
  const double prt     = PA / (R0 * temperature);
  const double oprt    = 1.0 / prt;
  
  double mass_frac[116];
  double avmolwt;
  double pressure;
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[0]) : 
    "l"(mass_frac_array+0*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[1]) : 
    "l"(mass_frac_array+1*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[2]) : 
    "l"(mass_frac_array+2*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[3]) : 
    "l"(mass_frac_array+3*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[4]) : 
    "l"(mass_frac_array+4*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[5]) : 
    "l"(mass_frac_array+5*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[6]) : 
    "l"(mass_frac_array+6*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[7]) : 
    "l"(mass_frac_array+7*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[8]) : 
    "l"(mass_frac_array+8*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[9]) : 
    "l"(mass_frac_array+9*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[10]) : 
    "l"(mass_frac_array+10*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[11]) : 
    "l"(mass_frac_array+11*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[12]) : 
    "l"(mass_frac_array+12*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[13]) : 
    "l"(mass_frac_array+13*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[14]) : 
    "l"(mass_frac_array+14*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[15]) : 
    "l"(mass_frac_array+15*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[16]) : 
    "l"(mass_frac_array+16*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[17]) : 
    "l"(mass_frac_array+17*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[18]) : 
    "l"(mass_frac_array+18*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[19]) : 
    "l"(mass_frac_array+19*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[20]) : 
    "l"(mass_frac_array+20*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[21]) : 
    "l"(mass_frac_array+21*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[22]) : 
    "l"(mass_frac_array+22*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[23]) : 
    "l"(mass_frac_array+23*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[24]) : 
    "l"(mass_frac_array+24*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[25]) : 
    "l"(mass_frac_array+25*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[26]) : 
    "l"(mass_frac_array+26*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[27]) : 
    "l"(mass_frac_array+27*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[28]) : 
    "l"(mass_frac_array+28*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[29]) : 
    "l"(mass_frac_array+29*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[30]) : 
    "l"(mass_frac_array+30*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[31]) : 
    "l"(mass_frac_array+31*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[32]) : 
    "l"(mass_frac_array+32*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[33]) : 
    "l"(mass_frac_array+33*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[34]) : 
    "l"(mass_frac_array+34*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[35]) : 
    "l"(mass_frac_array+35*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[36]) : 
    "l"(mass_frac_array+36*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[37]) : 
    "l"(mass_frac_array+37*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[38]) : 
    "l"(mass_frac_array+38*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[39]) : 
    "l"(mass_frac_array+39*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[40]) : 
    "l"(mass_frac_array+40*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[41]) : 
    "l"(mass_frac_array+41*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[42]) : 
    "l"(mass_frac_array+42*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[43]) : 
    "l"(mass_frac_array+43*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[44]) : 
    "l"(mass_frac_array+44*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[45]) : 
    "l"(mass_frac_array+45*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[46]) : 
    "l"(mass_frac_array+46*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[47]) : 
    "l"(mass_frac_array+47*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[48]) : 
    "l"(mass_frac_array+48*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[49]) : 
    "l"(mass_frac_array+49*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[50]) : 
    "l"(mass_frac_array+50*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[51]) : 
    "l"(mass_frac_array+51*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[52]) : 
    "l"(mass_frac_array+52*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[53]) : 
    "l"(mass_frac_array+53*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[54]) : 
    "l"(mass_frac_array+54*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[55]) : 
    "l"(mass_frac_array+55*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[56]) : 
    "l"(mass_frac_array+56*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[57]) : 
    "l"(mass_frac_array+57*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[58]) : 
    "l"(mass_frac_array+58*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[59]) : 
    "l"(mass_frac_array+59*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[60]) : 
    "l"(mass_frac_array+60*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[61]) : 
    "l"(mass_frac_array+61*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[62]) : 
    "l"(mass_frac_array+62*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[63]) : 
    "l"(mass_frac_array+63*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[64]) : 
    "l"(mass_frac_array+64*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[65]) : 
    "l"(mass_frac_array+65*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[66]) : 
    "l"(mass_frac_array+66*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[67]) : 
    "l"(mass_frac_array+67*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[68]) : 
    "l"(mass_frac_array+68*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[69]) : 
    "l"(mass_frac_array+69*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[70]) : 
    "l"(mass_frac_array+70*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[71]) : 
    "l"(mass_frac_array+71*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[72]) : 
    "l"(mass_frac_array+72*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[73]) : 
    "l"(mass_frac_array+73*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[74]) : 
    "l"(mass_frac_array+74*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[75]) : 
    "l"(mass_frac_array+75*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[76]) : 
    "l"(mass_frac_array+76*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[77]) : 
    "l"(mass_frac_array+77*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[78]) : 
    "l"(mass_frac_array+78*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[79]) : 
    "l"(mass_frac_array+79*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[80]) : 
    "l"(mass_frac_array+80*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[81]) : 
    "l"(mass_frac_array+81*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[82]) : 
    "l"(mass_frac_array+82*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[83]) : 
    "l"(mass_frac_array+83*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[84]) : 
    "l"(mass_frac_array+84*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[85]) : 
    "l"(mass_frac_array+85*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[86]) : 
    "l"(mass_frac_array+86*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[87]) : 
    "l"(mass_frac_array+87*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[88]) : 
    "l"(mass_frac_array+88*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[89]) : 
    "l"(mass_frac_array+89*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[90]) : 
    "l"(mass_frac_array+90*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[91]) : 
    "l"(mass_frac_array+91*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[92]) : 
    "l"(mass_frac_array+92*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[93]) : 
    "l"(mass_frac_array+93*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[94]) : 
    "l"(mass_frac_array+94*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[95]) : 
    "l"(mass_frac_array+95*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[96]) : 
    "l"(mass_frac_array+96*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[97]) : 
    "l"(mass_frac_array+97*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[98]) : 
    "l"(mass_frac_array+98*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[99]) : 
    "l"(mass_frac_array+99*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[100]) : 
    "l"(mass_frac_array+100*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[101]) : 
    "l"(mass_frac_array+101*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[102]) : 
    "l"(mass_frac_array+102*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[103]) : 
    "l"(mass_frac_array+103*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[104]) : 
    "l"(mass_frac_array+104*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[105]) : 
    "l"(mass_frac_array+105*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[106]) : 
    "l"(mass_frac_array+106*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[107]) : 
    "l"(mass_frac_array+107*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[108]) : 
    "l"(mass_frac_array+108*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[109]) : 
    "l"(mass_frac_array+109*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[110]) : 
    "l"(mass_frac_array+110*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[111]) : 
    "l"(mass_frac_array+111*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[112]) : 
    "l"(mass_frac_array+112*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[113]) : 
    "l"(mass_frac_array+113*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[114]) : 
    "l"(mass_frac_array+114*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(mass_frac[115]) : 
    "l"(mass_frac_array+115*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(avmolwt) : "l"(avmolwt_array) 
    : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(pressure) : 
    "l"(pressure_array) : "memory"); 
  double cgspl[171];
  // Gibbs computation
  {
    const double &tk1 = temperature;
    double tklog = log(tk1);
    double tk2 = tk1 * tk1;
    double tk3 = tk1 * tk2;
    double tk4 = tk1 * tk3;
    double tk5 = tk1 * tk4;
    
    // Species H
    {
      if (tk1 > 1.0e+03)
      {
        cgspl[0] = 2.5*tk1*(1-tklog) + -0.0*tk2 + -0.0*tk3 + -0.0*tk4 + -0.0*tk5 
          + (2.547163e+04 - tk1*-0.4601176); 
      }
      else
      {
        cgspl[0] = 2.5*tk1*(1-tklog) + -0.0*tk2 + -0.0*tk3 + -0.0*tk4 + -0.0*tk5 
          + (2.547163e+04 - tk1*-0.4601176); 
      }
    }
    // Species H2
    {
      if (tk1 > 1.0e+03)
      {
        cgspl[1] = 2.991423*tk1*(1-tklog) + -3.500322e-04*tk2 + 9.389715e-09*tk3 
          + 7.692981666666665e-13*tk4 + -7.913759999999998e-17*tk5 + (-835.034 - 
          tk1*-1.35511); 
      }
      else
      {
        cgspl[1] = 3.298124*tk1*(1-tklog) + -4.124721e-04*tk2 + 
          1.357169166666667e-07*tk3 + 7.896194999999997e-12*tk4 + 
          -2.067436e-14*tk5 + (-1.012521e+03 - tk1*-3.294094); 
      }
    }
    // Species O
    {
      if (tk1 > 1.0e+03)
      {
        cgspl[2] = 2.54206*tk1*(1-tklog) + 1.377531e-05*tk2 + 
          5.171338333333334e-10*tk3 + -3.792555833333332e-13*tk4 + 
          2.184026e-17*tk5 + (2.92308e+04 - tk1*4.920308); 
      }
      else
      {
        cgspl[2] = 2.946429*tk1*(1-tklog) + 8.19083e-04*tk2 + 
          -4.035053333333334e-07*tk3 + 1.3357025e-10*tk4 + -1.945348e-14*tk5 + 
          (2.914764e+04 - tk1*2.963995); 
      }
    }
    // Species O2
    {
      if (tk1 > 1.0e+03)
      {
        cgspl[3] = 3.697578*tk1*(1-tklog) + -3.0675985e-04*tk2 + 2.09807e-08*tk3 
          + -1.479400833333333e-12*tk4 + 5.682175e-17*tk5 + (-1.23393e+03 - 
          tk1*3.189166); 
      }
      else
      {
        cgspl[3] = 3.212936*tk1*(1-tklog) + -5.63743e-04*tk2 + 
          9.593583333333335e-08*tk3 + -1.0948975e-10*tk4 + 
          4.384276999999999e-14*tk5 + (-1.005249e+03 - tk1*6.034738); 
      }
    }
    // Species OH
    {
      if (tk1 > 1.71e+03)
      {
        cgspl[4] = 2.8537604*tk1*(1-tklog) + -5.1497167e-04*tk2 + 
          3.877774616666667e-08*tk3 + -1.6145892e-12*tk4 + 
          1.578799234999999e-17*tk5 + (3.6994972e+03 - tk1*5.78756825); 
      }
      else
      {
        cgspl[4] = 3.41896226*tk1*(1-tklog) + -1.596279005e-04*tk2 + 
          5.138211950000001e-08*tk3 + -3.036729116666666e-11*tk4 + 
          5.009773949999999e-15*tk5 + (3.45264448e+03 - tk1*2.54433372); 
      }
    }
    // Species H2O
    {
      if (tk1 > 1.0e+03)
      {
        cgspl[5] = 2.672146*tk1*(1-tklog) + -1.5281465e-03*tk2 + 
          1.455043333333333e-07*tk3 + -1.00083e-11*tk4 + 
          3.195808999999999e-16*tk5 + (-2.989921e+04 - tk1*6.862817); 
      }
      else
      {
        cgspl[5] = 3.386842*tk1*(1-tklog) + -1.737491e-03*tk2 + 1.059116e-06*tk3 
          + -5.807150833333332e-10*tk4 + 1.253294e-13*tk5 + (-3.020811e+04 - 
          tk1*2.590233); 
      }
    }
    // Species HO2
    {
      if (tk1 > 1.0e+03)
      {
        cgspl[6] = 4.0172109*tk1*(1-tklog) + -1.119910065e-03*tk2 + 
          1.056096916666667e-07*tk3 + -9.520530833333331e-12*tk4 + 
          5.395426749999999e-16*tk5 + (111.856713 - tk1*3.78510215); 
      }
      else
      {
        cgspl[6] = 4.30179801*tk1*(1-tklog) + 2.374560255e-03*tk2 + 
          -3.526381516666667e-06*tk3 + 2.02303245e-09*tk4 + -4.64612562e-13*tk5 
          + (294.80804 - tk1*3.71666245); 
      }
    }
    // Species H2O2
    {
      if (tk1 > 1.0e+03)
      {
        cgspl[7] = 4.573167*tk1*(1-tklog) + -2.168068e-03*tk2 + 2.457815e-07*tk3 
          + -1.957419999999999e-11*tk4 + 7.158269999999999e-16*tk5 + 
          (-1.800696e+04 - tk1*0.5011370000000001); 
      }
      else
      {
        cgspl[7] = 3.388754*tk1*(1-tklog) + -3.284613e-03*tk2 + 
          2.475021666666667e-08*tk3 + 3.854838333333332e-10*tk4 + 
          -1.2357575e-13*tk5 + (-1.766315e+04 - tk1*6.785363); 
      }
    }
    // Species CO
    {
      if (tk1 > 1.429e+03)
      {
        cgspl[8] = 3.1121689*tk1*(1-tklog) + -5.79741415e-04*tk2 + 
          5.641339366666667e-08*tk3 + -3.678359149999999e-12*tk4 + 
          1.06431114e-16*tk5 + (-1.42718539e+04 - tk1*5.71725177); 
      }
      else
      {
        cgspl[8] = 3.19036352*tk1*(1-tklog) + -4.47209986e-04*tk2 + 
          5.415459383333334e-09*tk3 + 8.716663916666665e-12*tk4 + 
          -1.209828465e-15*tk5 + (-1.42869054e+04 - tk1*5.33277914); 
      }
    }
    // Species CO2
    {
      if (tk1 > 1.38e+03)
      {
        cgspl[9] = 5.18953018*tk1*(1-tklog) + -1.03003238e-03*tk2 + 
          1.22262554e-07*tk3 + -9.750364499999997e-12*tk4 + 
          3.458646074999999e-16*tk5 + (-4.93178953e+04 - tk1*-5.18289303); 
      }
      else
      {
        cgspl[9] = 2.5793049*tk1*(1-tklog) + -4.123424935e-03*tk2 + 
          1.071193411666667e-06*tk3 + -2.1219752e-10*tk4 + 
          2.060152214999999e-14*tk5 + (-4.8416283e+04 - tk1*8.811410410000001); 
      }
    }
    // Species CH2O
    {
      if (tk1 > 1.486e+03)
      {
        cgspl[10] = 4.02068394*tk1*(1-tklog) + -2.549517085e-03*tk2 + 
          2.940508000000001e-07*tk3 + -2.300215658333333e-11*tk4 + 
          8.049902099999998e-16*tk5 + (-1.49287258e+04 - tk1*1.06525547); 
      }
      else
      {
        cgspl[10] = 3.00754197*tk1*(1-tklog) + -1.52364748e-03*tk2 + 
          -8.751820766666667e-07*tk3 + 4.266827341666666e-10*tk4 + 
          -6.356689749999998e-14*tk5 + (-1.41188397e+04 - tk1*8.10120233); 
      }
    }
    // Species HCO
    {
      if (tk1 > 1.69e+03)
      {
        cgspl[11] = 3.44148164*tk1*(1-tklog) + -1.760788595e-03*tk2 + 
          2.0689353e-07*tk3 + -1.644405366666666e-11*tk4 + 
          5.826930799999998e-16*tk5 + (3.97409684e+03 - tk1*6.24593456); 
      }
      else
      {
        cgspl[11] = 3.81049965*tk1*(1-tklog) + -4.066349125e-04*tk2 + 
          -5.219411683333334e-07*tk3 + 1.995652233333333e-10*tk4 + 
          -2.53447277e-14*tk5 + (4.03859901e+03 - tk1*4.94843165); 
      }
    }
    // Species HOCHO
    {
      if (tk1 > 1.376e+03)
      {
        cgspl[12] = 6.68733013*tk1*(1-tklog) + -2.57144684e-03*tk2 + 
          3.03730855e-07*tk3 + -2.414326358333333e-11*tk4 + 
          8.544609949999999e-16*tk5 + (-4.839954e+04 - tk1*-11.3104798); 
      }
      else
      {
        cgspl[12] = 1.43548185*tk1*(1-tklog) + -8.168150799999999e-03*tk2 + 
          1.770957016666667e-06*tk3 + -2.767774808333333e-10*tk4 + 
          2.010880514999999e-14*tk5 + (-4.64616504e+04 - tk1*17.2885798); 
      }
    }
    // Species HOCH2O
    {
      if (tk1 > 1.452e+03)
      {
        cgspl[13] = 6.39521515*tk1*(1-tklog) + -3.718365215e-03*tk2 + 
          4.173705900000001e-07*tk3 + -3.207330933333333e-11*tk4 + 
          1.108893445e-15*tk5 + (-2.25557758e+04 - tk1*-6.63865583); 
      }
      else
      {
        cgspl[13] = 4.11183145*tk1*(1-tklog) + -3.769253485e-03*tk2 + 
          -6.288956166666667e-07*tk3 + 4.489550041666666e-10*tk4 + 
          -7.280794349999999e-14*tk5 + (-2.12471918e+04 - tk1*7.46807254); 
      }
    }
    // Species CH3OH
    {
      if (tk1 > 1.985e+03)
      {
        cgspl[14] = 3.92867513*tk1*(1-tklog) + -4.90813037e-03*tk2 + 
          5.676924600000001e-07*tk3 + -4.462584974999999e-11*tk4 + 
          1.568880145e-15*tk5 + (-2.61454175e+04 - tk1*2.69464483); 
      }
      else
      {
        cgspl[14] = 1.82176844*tk1*(1-tklog) + -6.166321e-03*tk2 + 
          4.562369616666667e-07*tk3 + 8.948943166666665e-11*tk4 + 
          -2.118224184999999e-14*tk5 + (-2.52222207e+04 - tk1*14.81043); 
      }
    }
    // Species CH2OH
    {
      if (tk1 > 1.399e+03)
      {
        cgspl[15] = 5.41875913*tk1*(1-tklog) + -2.83092622e-03*tk2 + 
          3.124518933333334e-07*tk3 + -2.370349483333333e-11*tk4 + 
          8.114989899999999e-16*tk5 + (-3.6147554e+03 - tk1*-3.49277963); 
      }
      else
      {
        cgspl[15] = 3.05674228*tk1*(1-tklog) + -5.9667817e-03*tk2 + 
          1.454168838333334e-06*tk3 + -3.189842116666666e-10*tk4 + 
          3.614439754999999e-14*tk5 + (-2.8314019e+03 - tk1*8.98878133); 
      }
    }
    // Species CH3O
    {
      if (tk1 > 1.509e+03)
      {
        cgspl[16] = 4.64787019*tk1*(1-tklog) + -3.454153415e-03*tk2 + 
          3.906746266666667e-07*tk3 + -3.016621416666666e-11*tk4 + 
          1.046267705e-15*tk5 + (-299.208881 - tk1*-1.57740193); 
      }
      else
      {
        cgspl[16] = 2.23058023*tk1*(1-tklog) + -4.26589293e-03*tk2 + 
          -1.702777066666667e-07*tk3 + 2.842057633333333e-10*tk4 + 
          -4.973455189999999e-14*tk5 + (945.939708 - tk1*12.8377569); 
      }
    }
    // Species CH3O2H
    {
      if (tk1 > 1.367e+03)
      {
        cgspl[17] = 8.80409289*tk1*(1-tklog) + -4.04713609e-03*tk2 + 
          4.764054566666668e-07*tk3 + -3.778081283333333e-11*tk4 + 
          1.334903535e-15*tk5 + (-1.98512174e+04 - tk1*-21.7000591); 
      }
      else
      {
        cgspl[17] = 2.83880024*tk1*(1-tklog) + -9.30481245e-03*tk2 + 
          1.41360902e-06*tk3 + -8.365620916666665e-11*tk4 + 
          -8.580621449999998e-15*tk5 + (-1.74033753e+04 - tk1*11.6092433); 
      }
    }
    // Species CH3O2
    {
      if (tk1 > 1.365e+03)
      {
        cgspl[18] = 6.34718801*tk1*(1-tklog) + -3.96044679e-03*tk2 + 
          4.610031883333334e-07*tk3 + -3.628005258333333e-11*tk4 + 
          1.27492381e-15*tk5 + (-1.83436055e+03 - tk1*-7.42552545); 
      }
      else
      {
        cgspl[18] = 3.8049759*tk1*(1-tklog) + -4.9039233e-03*tk2 + 
          6.515677066666668e-08*tk3 + 1.858938349999999e-10*tk4 + 
          -3.216554099999999e-14*tk5 + (-455.625796 - tk1*7.817891); 
      }
    }
    // Species CH4
    {
      if (tk1 > 1.462e+03)
      {
        cgspl[19] = 4.09617653*tk1*(1-tklog) + -3.721654225e-03*tk2 + 
          4.397865000000001e-07*tk3 + -3.496480033333333e-11*tk4 + 
          1.23754025e-15*tk5 + (-1.13835704e+04 - tk1*-4.67561383); 
      }
      else
      {
        cgspl[19] = 3.7211302*tk1*(1-tklog) + 1.251466445e-03*tk2 + 
          -3.170775566666667e-06*tk3 + 1.223927108333333e-09*tk4 + 
          -1.71895576e-13*tk5 + (-1.01424099e+04 - tk1*1.22776596); 
      }
    }
    // Species CH3
    {
      if (tk1 > 1.389e+03)
      {
        cgspl[20] = 3.51281376*tk1*(1-tklog) + -2.557063065e-03*tk2 + 
          2.7938675e-07*tk3 + -2.10412645e-11*tk4 + 7.165146149999999e-16*tk5 + 
          (1.61238027e+04 - tk1*1.62436112); 
      }
      else
      {
        cgspl[20] = 3.43858162*tk1*(1-tklog) + -2.03876332e-03*tk2 + 
          -5.330516566666667e-08*tk3 + 7.897244916666666e-11*tk4 + 
          -1.10914083e-14*tk5 + (1.63164018e+04 - tk1*2.52807406); 
      }
    }
    // Species CH2
    {
      if (tk1 > 1.0e+03)
      {
        cgspl[21] = 3.636408*tk1*(1-tklog) + -9.665285e-04*tk2 + 
          2.811693333333334e-08*tk3 + 8.415824999999999e-12*tk4 + 
          -9.041279999999997e-16*tk5 + (4.534134e+04 - tk1*2.156561); 
      }
      else
      {
        cgspl[21] = 3.762237*tk1*(1-tklog) + -5.799095e-04*tk2 + 
          -4.149308333333334e-08*tk3 + -7.334029999999998e-11*tk4 + 
          3.666217499999999e-14*tk5 + (4.536791e+04 - tk1*1.712578); 
      }
    }
    // Species CH2(S)
    {
      if (tk1 > 1.0e+03)
      {
        cgspl[22] = 3.552889*tk1*(1-tklog) + -1.033394e-03*tk2 + 
          3.190193333333334e-08*tk3 + 9.205608333333331e-12*tk4 + 
          -1.010675e-15*tk5 + (4.984975e+04 - tk1*1.68657); 
      }
      else
      {
        cgspl[22] = 3.971265*tk1*(1-tklog) + 8.495445e-05*tk2 + 
          -1.708948333333334e-07*tk3 + -2.077125833333333e-10*tk4 + 
          9.906329999999997e-14*tk5 + (4.989368e+04 - tk1*0.05753207); 
      }
    }
    // Species C2H6
    {
      if (tk1 > 1.383e+03)
      {
        cgspl[23] = 6.0597263*tk1*(1-tklog) + -6.51914185e-03*tk2 + 
          7.468399033333334e-07*tk3 + -5.814684124999999e-11*tk4 + 
          2.028031765e-15*tk5 + (-1.35751226e+04 - tk1*-12.8608001); 
      }
      else
      {
        cgspl[23] = 0.0478623203*tk1*(1-tklog) + -0.01202845635*tk2 + 
          1.9192652e-06*tk3 + -2.07221865e-10*tk4 + 8.917197199999998e-15*tk5 + 
          (-1.10923014e+04 - tk1*20.6544071); 
      }
    }
    // Species C2H5
    {
      if (tk1 > 1.387e+03)
      {
        cgspl[24] = 5.8878439*tk1*(1-tklog) + -5.15383965e-03*tk2 + 
          5.780739933333334e-07*tk3 + -4.437493808333332e-11*tk4 + 
          1.532563254999999e-15*tk5 + (1.15065499e+04 - tk1*-8.496517709999999); 
      }
      else
      {
        cgspl[24] = 1.32730217*tk1*(1-tklog) + -8.83283765e-03*tk2 + 
          1.024877596666667e-06*tk3 + 2.509528883333333e-11*tk4 + 
          -2.193088874999999e-14*tk5 + (1.34284028e+04 - tk1*17.1789216); 
      }
    }
    // Species C2H4
    {
      if (tk1 > 1.395e+03)
      {
        cgspl[25] = 5.22176372*tk1*(1-tklog) + -4.480686515e-03*tk2 + 
          5.081148100000001e-07*tk3 + -3.928879366666666e-11*tk4 + 
          1.36369796e-15*tk5 + (3.60389679e+03 - tk1*-7.47789234); 
      }
      else
      {
        cgspl[25] = 0.233879687*tk1*(1-tklog) + -9.816732349999999e-03*tk2 + 
          1.947220233333333e-06*tk3 + -3.035387108333332e-10*tk4 + 
          2.387213575e-14*tk5 + (5.46489338e+03 - tk1*19.7084228); 
      }
    }
    // Species C2H3
    {
      if (tk1 > 1.395e+03)
      {
        cgspl[26] = 5.07331248*tk1*(1-tklog) + -3.29158139e-03*tk2 + 
          3.729382066666667e-07*tk3 + -2.881694824999999e-11*tk4 + 
          9.997024499999996e-16*tk5 + (3.37234748e+04 - tk1*-3.39792712); 
      }
      else
      {
        cgspl[26] = 1.25329724*tk1*(1-tklog) + -7.8129185e-03*tk2 + 
          1.796731316666667e-06*tk3 + -3.483788616666666e-10*tk4 + 
          3.506801809999999e-14*tk5 + (3.50734773e+04 - tk1*17.1341661); 
      }
    }
    // Species C2H2
    {
      if (tk1 > 1.407e+03)
      {
        cgspl[27] = 4.98265164*tk1*(1-tklog) + -2.12996465e-03*tk2 + 
          2.29139205e-07*tk3 + -1.705983033333333e-11*tk4 + 
          5.759586999999998e-16*tk5 + (2.52697118e+04 - tk1*-5.81321385); 
      }
      else
      {
        cgspl[27] = 2.06742667*tk1*(1-tklog) + -7.3284253e-03*tk2 + 
          2.549117716666667e-06*tk3 + -6.924714674999999e-10*tk4 + 
          8.646608749999997e-14*tk5 + (2.59578589e+04 - tk1*8.62758672); 
      }
    }
    // Species CH3CHO
    {
      if (tk1 > 1.377e+03)
      {
        cgspl[28] = 6.98518866*tk1*(1-tklog) + -4.839488935e-03*tk2 + 
          5.530699233333334e-07*tk3 + -4.300215841666666e-11*tk4 + 
          1.498629515e-15*tk5 + (-2.39807279e+04 - tk1*-12.7484852); 
      }
      else
      {
        cgspl[28] = 1.77060035*tk1*(1-tklog) + -9.223758049999999e-03*tk2 + 
          1.206896936666667e-06*tk3 + -1.953038008333333e-11*tk4 + 
          -1.677719455e-14*tk5 + (-2.1807885e+04 - tk1*16.5023437); 
      }
    }
    // Species CH3CO
    {
      if (tk1 > 1.371e+03)
      {
        cgspl[29] = 6.56682466*tk1*(1-tklog) + -3.776543345e-03*tk2 + 
          4.332779733333334e-07*tk3 + -3.377791616666666e-11*tk4 + 
          1.17938082e-15*tk5 + (-4.76690401e+03 - tk1*-8.833019650000001); 
      }
      else
      {
        cgspl[29] = 2.5288415*tk1*(1-tklog) + -6.85760865e-03*tk2 + 
          7.143457933333335e-07*tk3 + 6.430702316666666e-11*tk4 + 
          -2.419181899999999e-14*tk5 + (-3.02546532e+03 - tk1*14.0340315); 
      }
    }
    // Species CH2CHO
    {
      if (tk1 > 1.388e+03)
      {
        cgspl[30] = 7.5414579*tk1*(1-tklog) + -3.411485625e-03*tk2 + 
          3.948976783333333e-07*tk3 + -3.096945975e-11*tk4 + 1.08580024e-15*tk5 
          + (-2.61437239e+03 - tk1*-16.2602766); 
      }
      else
      {
        cgspl[30] = 1.47616956*tk1*(1-tklog) + -0.0104487093*tk2 + 
          2.5020592e-06*tk3 + -4.691394849999999e-10*tk4 + 
          4.383122349999999e-14*tk5 + (-482.05051 - tk1*16.3756465); 
      }
    }
    // Species CH2CO
    {
      if (tk1 > 1.0e+03)
      {
        cgspl[31] = 6.038817*tk1*(1-tklog) + -2.90242e-03*tk2 + 3.20159e-07*tk3 
          + -2.328737499999999e-11*tk4 + 7.294339999999998e-16*tk5 + 
          (-8.583402e+03 - tk1*-7.657581); 
      }
      else
      {
        cgspl[31] = 2.974971*tk1*(1-tklog) + -6.059355e-03*tk2 + 
          3.908410000000001e-07*tk3 + 5.388904166666665e-10*tk4 + 
          -1.9528245e-13*tk5 + (-7.632637e+03 - tk1*8.673553); 
      }
    }
    // Species HCCO
    {
      if (tk1 > 1.0e+03)
      {
        cgspl[32] = 6.758073*tk1*(1-tklog) + -1.0002e-03*tk2 + 3.379345e-08*tk3 
          + 8.676099999999999e-12*tk4 + -9.825824999999998e-16*tk5 + 
          (1.901513e+04 - tk1*-9.071262000000001); 
      }
      else
      {
        cgspl[32] = 5.047965*tk1*(1-tklog) + -2.226739e-03*tk2 + 
          -3.780471666666667e-08*tk3 + 1.235079166666666e-10*tk4 + 
          -1.125371e-14*tk5 + (1.965892e+04 - tk1*0.4818439); 
      }
    }
    // Species CH3CO3
    {
      if (tk1 > 1.391e+03)
      {
        cgspl[33] = 11.2522498*tk1*(1-tklog) + -4.16826336e-03*tk2 + 
          4.816908833333335e-07*tk3 + -3.773181116666666e-11*tk4 + 
          1.32177228e-15*tk5 + (-2.60238584e+04 - tk1*-29.6370457); 
      }
      else
      {
        cgspl[33] = 3.60373432*tk1*(1-tklog) + -0.01350401705*tk2 + 
          3.4715573e-06*tk3 + -7.087842533333332e-10*tk4 + 
          7.192305499999998e-14*tk5 + (-2.34205171e+04 - tk1*11.2014914); 
      }
    }
    // Species C2H5O
    {
      if (tk1 > 1.393e+03)
      {
        cgspl[34] = 8.23717244*tk1*(1-tklog) + -5.54429395e-03*tk2 + 
          6.313472866666667e-07*tk3 + -4.896778941666665e-11*tk4 + 
          1.703564445e-15*tk5 + (-6.22948597e+03 - tk1*-19.3190543); 
      }
      else
      {
        cgspl[34] = 0.287429022*tk1*(1-tklog) + -0.0143250459*tk2 + 
          3.064283466666667e-06*tk3 + -5.025801491666666e-10*tk4 + 
          4.022813214999999e-14*tk5 + (-3.35717377e+03 - tk1*23.7513898); 
      }
    }
    // Species C2H5O2H
    {
      if (tk1 > 1.391e+03)
      {
        cgspl[35] = 11.2305737*tk1*(1-tklog) + -6.024106e-03*tk2 + 
          6.612170016666668e-07*tk3 + -5.006288599999999e-11*tk4 + 
          1.713289015e-15*tk5 + (-2.47977531e+04 - tk1*-32.5607232); 
      }
      else
      {
        cgspl[35] = 1.57329011*tk1*(1-tklog) + -0.0176189998*tk2 + 
          4.22006655e-06*tk3 + -7.973353966666665e-10*tk4 + 
          7.408368749999998e-14*tk5 + (-2.15278368e+04 - tk1*19.0472032); 
      }
    }
    // Species C2H5O2
    {
      if (tk1 > 1.387e+03)
      {
        cgspl[36] = 8.776413290000001*tk1*(1-tklog) + -5.93835815e-03*tk2 + 
          6.461384166666668e-07*tk3 + -4.860089616666666e-11*tk4 + 
          1.65503362e-15*tk5 + (-6.78748703e+03 - tk1*-18.3119972); 
      }
      else
      {
        cgspl[36] = 2.58630333*tk1*(1-tklog) + -0.0130918181*tk2 + 
          2.805103216666667e-06*tk3 + -5.072910791666666e-10*tk4 + 
          4.786510199999999e-14*tk5 + (-4.58588992e+03 - tk1*15.0486289); 
      }
    }
    // Species C2H3O1-2
    {
      if (tk1 > 1.492e+03)
      {
        cgspl[37] = 6.88486471*tk1*(1-tklog) + -3.473602505e-03*tk2 + 
          3.720244966666667e-07*tk3 + -2.768256391666666e-11*tk4 + 
          9.356227749999997e-16*tk5 + (1.264422e+04 - tk1*-12.384257); 
      }
      else
      {
        cgspl[37] = -1.62965122*tk1*(1-tklog) + -0.0146727743*tk2 + 
          4.0622925e-06*tk3 + -8.376860416666664e-10*tk4 + 
          8.062951799999998e-14*tk5 + (1.52459425e+04 - tk1*32.2782741); 
      }
    }
    // Species CH3COCH3
    {
      if (tk1 > 1.382e+03)
      {
        cgspl[38] = 9.626743790000001*tk1*(1-tklog) + -7.27596225e-03*tk2 + 
          8.295824283333334e-07*tk3 + -6.439954924999998e-11*tk4 + 
          2.241835825e-15*tk5 + (-3.11862263e+04 - tk1*-26.1613449); 
      }
      else
      {
        cgspl[38] = 1.24527408*tk1*(1-tklog) + -0.01498801275*tk2 + 
          2.333777683333333e-06*tk3 + -1.803779266666666e-10*tk4 + 
          -6.381864749999998e-15*tk5 + (-2.78348727e+04 - tk1*20.3682615); 
      }
    }
    // Species CH3COCH2
    {
      if (tk1 > 1.388e+03)
      {
        cgspl[39] = 10.8892477*tk1*(1-tklog) + -5.57703375e-03*tk2 + 
          6.42527975e-07*tk3 + -5.023617066666666e-11*tk4 + 1.757667245e-15*tk5 
          + (-1.00741464e+04 - tk1*-31.8043322); 
      }
      else
      {
        cgspl[39] = 1.22337251*tk1*(1-tklog) + -0.0162273371*tk2 + 
          3.559041966666667e-06*tk3 + -5.806481124999999e-10*tk4 + 
          4.495801494999999e-14*tk5 + (-6.59419324e+03 - tk1*20.5537233); 
      }
    }
    // Species CH3COCH2O2
    {
      if (tk1 > 1.379e+03)
      {
        cgspl[40] = 12.7690342*tk1*(1-tklog) + -7.1277414e-03*tk2 + 
          8.213691016666667e-07*tk3 + -6.420407674999998e-11*tk4 + 
          2.245552669999999e-15*tk5 + (-2.34798669e+04 - tk1*-32.7155799); 
      }
      else
      {
        cgspl[40] = 5.95535468*tk1*(1-tklog) + -0.01351276025*tk2 + 
          2.289750516666667e-06*tk3 + -2.947798758333333e-10*tk4 + 
          2.019612785e-14*tk5 + (-2.06679464e+04 - tk1*5.21436049); 
      }
    }
    // Species CH3COCH2O2H
    {
      if (tk1 > 1.381e+03)
      {
        cgspl[41] = 15.237281*tk1*(1-tklog) + -7.20573255e-03*tk2 + 
          8.354833483333334e-07*tk3 + -6.558926908333332e-11*tk4 + 
          2.301128919999999e-15*tk5 + (-4.27564444e+04 - tk1*-47.7383784); 
      }
      else
      {
        cgspl[41] = 4.94789761*tk1*(1-tklog) + -0.0180237216*tk2 + 
          3.695332216666667e-06*tk3 + -5.819140616666665e-10*tk4 + 
          4.606346299999999e-14*tk5 + (-3.88687178e+04 - tk1*8.499261300000001); 
      }
    }
    // Species CH3COCH2O
    {
      if (tk1 > 1.37e+03)
      {
        cgspl[42] = 11.4637586*tk1*(1-tklog) + -6.6062171e-03*tk2 + 
          7.609674916666667e-07*tk3 + -5.949146149999999e-11*tk4 + 
          2.081405234999999e-15*tk5 + (-2.23833017e+04 - tk1*-31.5127868); 
      }
      else
      {
        cgspl[42] = 3.72927689*tk1*(1-tklog) + -0.01319718485*tk2 + 
          1.829941433333334e-06*tk3 + -7.151544191666665e-11*tk4 + 
          -1.69737295e-14*tk5 + (-1.91551565e+04 - tk1*11.8505335); 
      }
    }
    // Species C2H3CHO
    {
      if (tk1 > 1.393e+03)
      {
        cgspl[43] = 10.4184959*tk1*(1-tklog) + -4.744816605e-03*tk2 + 
          5.488508816666668e-07*tk3 + -4.302326691666665e-11*tk4 + 
          1.507936455e-15*tk5 + (-1.49630281e+04 - tk1*-30.7235061); 
      }
      else
      {
        cgspl[43] = 0.292355162*tk1*(1-tklog) + -0.01771607085*tk2 + 
          4.9156054e-06*tk3 + -1.067501033333333e-09*tk4 + 1.13072054e-13*tk5 + 
          (-1.16521584e+04 - tk1*22.887828); 
      }
    }
    // Species C2H3CO
    {
      if (tk1 > 1.402e+03)
      {
        cgspl[44] = 9.37467676*tk1*(1-tklog) + -3.9564845e-03*tk2 + 
          4.453304666666667e-07*tk3 + -3.425961916666666e-11*tk4 + 
          1.184894905e-15*tk5 + (1.92969514e+03 - tk1*-24.0892696); 
      }
      else
      {
        cgspl[44] = 1.36242013*tk1*(1-tklog) + -0.0157636986*tk2 + 
          5.003648916666667e-06*tk3 + -1.234725933333333e-09*tk4 + 
          1.43985765e-13*tk5 + (4.25770215e+03 - tk1*17.2626546); 
      }
    }
    // Species C2H5CHO
    {
      if (tk1 > 1.375e+03)
      {
        cgspl[45] = 8.872162230000001*tk1*(1-tklog) + -0.0100855381*tk2 + 
          1.344146078333333e-06*tk3 + -1.146814866666666e-10*tk4 + 
          4.244611624999999e-15*tk5 + (-2.83848527e+04 - tk1*-23.5069163); 
      }
      else
      {
        cgspl[45] = 5.9310753*tk1*(1-tklog) + -4.439719275e-03*tk2 + 
          -3.396072083333334e-06*tk3 + 1.501244566666666e-09*tk4 + 
          -2.070428275e-13*tk5 + (-2.51377751e+04 - tk1*-0.5870199740000001); 
      }
    }
    // Species C2H5CO
    {
      if (tk1 > 1.362e+03)
      {
        cgspl[46] = 10.0147418*tk1*(1-tklog) + -8.076398049999999e-03*tk2 + 
          1.104960203333333e-06*tk3 + -9.60755558333333e-11*tk4 + 
          3.606317289999999e-15*tk5 + (-1.00430305e+04 - tk1*-28.8570933); 
      }
      else
      {
        cgspl[46] = 8.353522460000001*tk1*(1-tklog) + 2.043698635e-03*tk2 + 
          -6.070299233333333e-06*tk3 + 2.270480941666666e-09*tk4 + 
          -3.03220133e-13*tk5 + (-6.58577307e+03 - tk1*-10.5948346); 
      }
    }
    // Species IC3H7
    {
      if (tk1 > 1.373e+03)
      {
        cgspl[47] = 8.14705217*tk1*(1-tklog) + -7.9363553e-03*tk2 + 
          9.076859016666668e-07*tk3 + -7.060064074999998e-11*tk4 + 
          2.460893759999999e-15*tk5 + (6.18073367e+03 - tk1*-19.198085); 
      }
      else
      {
        cgspl[47] = 1.63417589*tk1*(1-tklog) + -0.0120085686*tk2 + 
          7.880134450000001e-07*tk3 + 2.702955025e-10*tk4 + 
          -6.176952199999998e-14*tk5 + (9.20752889e+03 - tk1*18.3848082); 
      }
    }
    // Species NC3H7
    {
      if (tk1 > 1.386e+03)
      {
        cgspl[48] = 8.88635047*tk1*(1-tklog) + -7.61366395e-03*tk2 + 
          8.698431000000002e-07*tk3 + -6.760493399999998e-11*tk4 + 
          2.355140294999999e-15*tk5 + (7.34671584e+03 - tk1*-23.0727791); 
      }
      else
      {
        cgspl[48] = 0.474365329*tk1*(1-tklog) + -0.01595951835*tk2 + 
          2.865163e-06*tk3 + -3.716065724999999e-10*tk4 + 
          2.183376989999999e-14*tk5 + (1.06314565e+04 - tk1*23.2670929); 
      }
    }
    // Species C3H6
    {
      if (tk1 > 1.388e+03)
      {
        cgspl[49] = 8.015959580000001*tk1*(1-tklog) + -6.8511817e-03*tk2 + 
          7.770828883333334e-07*tk3 + -6.010453349999999e-11*tk4 + 
          2.086850629999999e-15*tk5 + (-1.76749303e+03 - tk1*-20.0160668); 
      }
      else
      {
        cgspl[49] = 0.394615444*tk1*(1-tklog) + -0.0144553831*tk2 + 
          2.5814468e-06*tk3 + -3.240118408333333e-10*tk4 + 1.68945176e-14*tk5 + 
          (1.17760132e+03 - tk1*21.9003736); 
      }
    }
    // Species C3H5-A
    {
      if (tk1 > 1.397e+03)
      {
        cgspl[50] = 8.458839579999999*tk1*(1-tklog) + -5.63477415e-03*tk2 + 
          6.396547733333333e-07*tk3 + -4.950492658333332e-11*tk4 + 
          1.71959015e-15*tk5 + (1.64683289e+04 - tk1*-23.2704266); 
      }
      else
      {
        cgspl[50] = -0.529131958*tk1*(1-tklog) + -0.016727955*tk2 + 
          4.223350450000001e-06*tk3 + -8.572146166666664e-10*tk4 + 
          8.662916999999998e-14*tk5 + (1.94941423e+04 - tk1*24.6172315); 
      }
    }
    // Species C3H5-S
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[51] = 7.88765879*tk1*(1-tklog) + -5.65062955e-03*tk2 + 
          6.403552166666667e-07*tk3 + -4.949855641666665e-11*tk4 + 
          1.717835874999999e-15*tk5 + (2.83484913e+04 - tk1*-17.4291589); 
      }
      else
      {
        cgspl[51] = 1.32807335*tk1*(1-tklog) + -0.0126553957*tk2 + 
          2.525507316666667e-06*tk3 + -3.952879708333333e-10*tk4 + 
          3.12333042e-14*tk5 + (3.079811e+04 - tk1*18.3328787); 
      }
    }
    // Species C3H5-T
    {
      if (tk1 > 1.382e+03)
      {
        cgspl[52] = 7.37492443*tk1*(1-tklog) + -5.87550305e-03*tk2 + 
          6.667021383333334e-07*tk3 + -5.157894958333332e-11*tk4 + 
          1.79107509e-15*tk5 + (2.73982108e+04 - tk1*-14.3478655); 
      }
      else
      {
        cgspl[52] = 2.17916644*tk1*(1-tklog) + -0.01019133115*tk2 + 
          1.319023056666667e-06*tk3 + -3.974218224999999e-11*tk4 + 
          -1.35199268e-14*tk5 + (2.96002535e+04 - tk1*14.8785684); 
      }
    }
    // Species C3H4-P
    {
      if (tk1 > 1.4e+03)
      {
        cgspl[53] = 9.768102000000001*tk1*(1-tklog) + -2.6095755e-03*tk2 + 
          6.255233333333335e-08*tk3 + 2.493492499999999e-11*tk4 + 
          -2.553939e-15*tk5 + (1.860277e+04 - tk1*-30.20678); 
      }
      else
      {
        cgspl[53] = 3.02973*tk1*(1-tklog) + -7.494805e-03*tk2 + 
          2.330833333333334e-07*tk3 + 3.308015833333333e-10*tk4 + 
          -6.941084999999999e-14*tk5 + (2.148408e+04 - tk1*8.004594000000001); 
      }
    }
    // Species C3H4-A
    {
      if (tk1 > 1.4e+03)
      {
        cgspl[54] = 9.776256*tk1*(1-tklog) + -2.651069e-03*tk2 + 6.16853e-08*tk3 
          + 2.521988333333333e-11*tk4 + -2.5447905e-15*tk5 + (1.954972e+04 - 
          tk1*-30.77061); 
      }
      else
      {
        cgspl[54] = 2.539831*tk1*(1-tklog) + -8.167185e-03*tk2 + 
          2.941583333333334e-07*tk3 + 3.872804166666666e-10*tk4 + 
          -8.645654999999998e-14*tk5 + (2.251243e+04 - tk1*9.935701999999999); 
      }
    }
    // Species C3H3
    {
      if (tk1 > 1.0e+03)
      {
        cgspl[55] = 8.831047*tk1*(1-tklog) + -2.1785975e-03*tk2 + 
          6.848445000000001e-08*tk3 + 1.973935833333333e-11*tk4 + 
          -2.188259999999999e-15*tk5 + (3.84742e+04 - tk1*-21.77919); 
      }
      else
      {
        cgspl[55] = 4.7542*tk1*(1-tklog) + -5.54014e-03*tk2 + 
          -4.655538333333333e-08*tk3 + 4.566009999999999e-10*tk4 + 
          -9.748144999999997e-14*tk5 + (3.988883e+04 - tk1*0.5854549); 
      }
    }
    // Species C3H2
    {
      if (tk1 > 1.0e+03)
      {
        cgspl[56] = 7.670981*tk1*(1-tklog) + -1.3743745e-03*tk2 + 
          7.284905e-08*tk3 + 5.379665833333333e-12*tk4 + 
          -8.319434999999998e-16*tk5 + (6.259722e+04 - tk1*-12.3689); 
      }
      else
      {
        cgspl[56] = 3.166714*tk1*(1-tklog) + -0.01241286*tk2 + 
          7.652728333333333e-06*tk3 + -3.556682499999999e-09*tk4 + 
          7.410759999999998e-13*tk5 + (6.350421e+04 - tk1*8.869446); 
      }
    }
    // Species C3H5O
    {
      if (tk1 > 1.38e+03)
      {
        cgspl[57] = 10.2551752*tk1*(1-tklog) + -5.749186e-03*tk2 + 
          6.410760983333334e-07*tk3 + -4.907586216666666e-11*tk4 + 
          1.692789615e-15*tk5 + (6.2656081e+03 - tk1*-27.7655042); 
      }
      else
      {
        cgspl[57] = 1.19822582*tk1*(1-tklog) + -0.01527899185*tk2 + 
          3.0105046e-06*tk3 + -4.051250274999999e-10*tk4 + 2.09927281e-14*tk5 + 
          (9.58217784e+03 - tk1*21.5566221); 
      }
    }
    // Species NC3H7O2
    {
      if (tk1 > 1.384e+03)
      {
        cgspl[58] = 12.6327059*tk1*(1-tklog) + -8.4955363e-03*tk2 + 
          9.814447883333334e-07*tk3 + -7.684961633333331e-11*tk4 + 
          2.691153374999999e-15*tk5 + (-1.19194652e+04 - tk1*-38.5349297); 
      }
      else
      {
        cgspl[58] = 2.10731492*tk1*(1-tklog) + -0.0198082493*tk2 + 
          4.158193316666668e-06*tk3 + -7.162085833333332e-10*tk4 + 
          6.562016499999999e-14*tk5 + (-7.93745567e+03 - tk1*18.9082748); 
      }
    }
    // Species IC3H7O2
    {
      if (tk1 > 1.388e+03)
      {
        cgspl[59] = 13.2493493*tk1*(1-tklog) + -8.204109500000001e-03*tk2 + 
          9.457201033333334e-07*tk3 + -7.394469499999998e-11*tk4 + 
          2.586807674999999e-15*tk5 + (-1.44109855e+04 - tk1*-42.9066213); 
      }
      else
      {
        cgspl[59] = 1.49941639*tk1*(1-tklog) + -0.02215406025*tk2 + 
          5.373574266666667e-06*tk3 + -1.080726133333333e-09*tk4 + 
          1.116852845e-13*tk5 + (-1.0258798e+04 - tk1*20.233649); 
      }
    }
    // Species CH3CHCO
    {
      if (tk1 > 1.4e+03)
      {
        cgspl[60] = 10.0219123*tk1*(1-tklog) + -4.7848315e-03*tk2 + 
          5.437027400000001e-07*tk3 + -4.210264216666666e-11*tk4 + 
          1.462966285e-15*tk5 + (-1.42482738e+04 - tk1*-27.7829973); 
      }
      else
      {
        cgspl[60] = 1.48380119*tk1*(1-tklog) + -0.01611015065*tk2 + 
          4.504167216666667e-06*tk3 + -1.0041597e-09*tk4 + 1.091829655e-13*tk5 + 
          (-1.1527654e+04 - tk1*17.1552068); 
      }
    }
    // Species C4H8-1
    {
      if (tk1 > 1.391e+03)
      {
        cgspl[61] = 11.3456574*tk1*(1-tklog) + -9.0421714e-03*tk2 + 
          1.02879419e-06*tk3 + -7.974381791666665e-11*tk4 + 
          2.772931059999999e-15*tk5 + (-5.8865945e+03 - tk1*-36.4627206); 
      }
      else
      {
        cgspl[61] = -0.833377133*tk1*(1-tklog) + -0.02270705615*tk2 + 
          4.960150500000001e-06*tk3 + -8.626529499999997e-10*tk4 + 
          7.598786649999999e-14*tk5 + (-1.49161811e+03 - tk1*29.4327905); 
      }
    }
    // Species PC4H9
    {
      if (tk1 > 1.391e+03)
      {
        cgspl[62] = 12.0779744*tk1*(1-tklog) + -9.8132389e-03*tk2 + 
          1.118836998333333e-06*tk3 + -8.683868666666665e-11*tk4 + 
          3.022346409999999e-15*tk5 + (3.22550473e+03 - tk1*-38.7719384); 
      }
      else
      {
        cgspl[62] = 0.320730933*tk1*(1-tklog) + -0.0217327227*tk2 + 
          4.0097495e-06*tk3 + -5.235377566666665e-10*tk4 + 2.90056583e-14*tk5 + 
          (7.71490893e+03 - tk1*25.7301085); 
      }
    }
    // Species SC4H9
    {
      if (tk1 > 1.381e+03)
      {
        cgspl[63] = 11.6934304*tk1*(1-tklog) + -9.820114350000001e-03*tk2 + 
          1.108844195e-06*tk3 + -8.552657916666664e-11*tk4 + 
          2.964131469999999e-15*tk5 + (1.96382429e+03 - tk1*-36.1626672); 
      }
      else
      {
        cgspl[63] = 0.849159986*tk1*(1-tklog) + -0.019104266*tk2 + 
          2.49377995e-06*tk3 + -1.704160091666666e-11*tk4 + 
          -4.121272184999999e-14*tk5 + (6.38832956e+03 - tk1*24.4466606); 
      }
    }
    // Species C4H71-3
    {
      if (tk1 > 1.392e+03)
      {
        cgspl[64] = 10.947017*tk1*(1-tklog) + -8.186219349999999e-03*tk2 + 
          9.331603333333335e-07*tk3 + -7.242897441666665e-11*tk4 + 
          2.521071944999999e-15*tk5 + (1.11181741e+04 - tk1*-35.49084); 
      }
      else
      {
        cgspl[64] = -1.12475112*tk1*(1-tklog) + -0.0217441427*tk2 + 
          4.7958409e-06*tk3 + -8.233724758333332e-10*tk4 + 
          7.006017049999999e-14*tk5 + (1.5442016e+04 - tk1*29.7651156); 
      }
    }
    // Species C4H6
    {
      if (tk1 > 1.398e+03)
      {
        cgspl[65] = 11.1633789*tk1*(1-tklog) + -6.85819825e-03*tk2 + 
          7.828596383333335e-07*tk3 + -6.080781966666665e-11*tk4 + 
          2.117431015e-15*tk5 + (7.7903977e+03 - tk1*-36.9847949); 
      }
      else
      {
        cgspl[65] = -1.43095121*tk1*(1-tklog) + -0.0239353031*tk2 + 
          6.924113333333334e-06*tk3 + -1.596246266666666e-09*tk4 + 
          1.785792534999999e-13*tk5 + (1.17551314e+04 - tk1*29.0825833); 
      }
    }
    // Species PC4H9O2
    {
      if (tk1 > 1.385e+03)
      {
        cgspl[66] = 15.7845448*tk1*(1-tklog) + -0.0107605455*tk2 + 
          1.241515028333333e-06*tk3 + -9.713172583333332e-11*tk4 + 
          3.399428044999999e-15*tk5 + (-1.60146054e+04 - tk1*-54.0388525); 
      }
      else
      {
        cgspl[66] = 1.9436365*tk1*(1-tklog) + -0.02577565815*tk2 + 
          5.471406666666668e-06*tk3 + -9.422071666666664e-10*tk4 + 
          8.505930299999999e-14*tk5 + (-1.08358103e+04 - tk1*21.3503149); 
      }
    }
    // Species C4H7O
    {
      if (tk1 > 1.395e+03)
      {
        cgspl[67] = 15.313778*tk1*(1-tklog) + -7.17135085e-03*tk2 + 
          8.027091950000001e-07*tk3 + -6.163123658333331e-11*tk4 + 
          2.130704069999999e-15*tk5 + (-729.342884 - tk1*-55.2937859); 
      }
      else
      {
        cgspl[67] = -1.60619192*tk1*(1-tklog) + -0.0279281341*tk2 + 
          7.25992945e-06*tk3 + -1.421577325e-09*tk4 + 1.3281759e-13*tk5 + 
          (4.85090326e+03 - tk1*34.7112559); 
      }
    }
    // Species C2H5COCH2
    {
      if (tk1 > 1.383e+03)
      {
        cgspl[68] = 14.2098738*tk1*(1-tklog) + -7.89332295e-03*tk2 + 
          9.175486383333334e-07*tk3 + -7.215587833333331e-11*tk4 + 
          2.534566645e-15*tk5 + (-1.41284951e+04 - tk1*-48.7132911); 
      }
      else
      {
        cgspl[68] = 1.54013856*tk1*(1-tklog) + -0.0219743129*tk2 + 
          4.950040350000001e-06*tk3 + -8.791276083333331e-10*tk4 + 
          7.929938449999998e-14*tk5 + (-9.507965050000001e+03 - tk1*19.9706641); 
      }
    }
    // Species NC3H7CHO
    {
      if (tk1 > 1.378e+03)
      {
        cgspl[69] = 13.5988068*tk1*(1-tklog) + -9.0826237e-03*tk2 + 
          1.029740763333334e-06*tk3 + -7.966501733333332e-11*tk4 + 
          2.767214789999999e-15*tk5 + (-3.15845348e+04 - tk1*-45.1790228); 
      }
      else
      {
        cgspl[69] = 1.87415959*tk1*(1-tklog) + -0.02096201575*tk2 + 
          3.919146316666667e-06*tk3 + -5.224280608333333e-10*tk4 + 
          3.047219539999999e-14*tk5 + (-2.71032194e+04 - tk1*19.1568574); 
      }
    }
    // Species NC3H7CO
    {
      if (tk1 > 1.38e+03)
      {
        cgspl[70] = 13.0026331*tk1*(1-tklog) + -8.15524385e-03*tk2 + 
          9.294048316666668e-07*tk3 + -7.213921908333332e-11*tk4 + 
          2.511278334999999e-15*tk5 + (-1.25523385e+04 - tk1*-40.2608515); 
      }
      else
      {
        cgspl[70] = 2.67256826*tk1*(1-tklog) + -0.01855994125*tk2 + 
          3.447714316666667e-06*tk3 + -4.573949066666666e-10*tk4 + 
          2.679320915e-14*tk5 + (-8.580508879999999e+03 - tk1*16.484895); 
      }
    }
    // Species IC4H9
    {
      if (tk1 > 1.386e+03)
      {
        cgspl[71] = 12.127693*tk1*(1-tklog) + -9.934474699999999e-03*tk2 + 
          1.14322834e-06*tk3 + -8.928460666666665e-11*tk4 + 
          3.120923044999999e-15*tk5 + (2.11952051e+03 - tk1*-40.8727278); 
      }
      else
      {
        cgspl[71] = -0.221457835*tk1*(1-tklog) + -0.0231878162*tk2 + 
          4.804715333333334e-06*tk3 + -8.001672049999998e-10*tk4 + 
          6.951051699999997e-14*tk5 + (6.76153637e+03 - tk1*26.480122); 
      }
    }
    // Species TC4H9
    {
      if (tk1 > 1.372e+03)
      {
        cgspl[72] = 10.5855083*tk1*(1-tklog) + -0.01059460275*tk2 + 
          1.219447266666667e-06*tk3 + -9.524681083333331e-11*tk4 + 
          3.329494174999999e-15*tk5 + (318.311189 - tk1*-32.0671258); 
      }
      else
      {
        cgspl[72] = 3.04300181*tk1*(1-tklog) + -0.01456381295*tk2 + 
          6.689779766666667e-07*tk3 + 4.085616449999999e-10*tk4 + 
          -8.200744549999999e-14*tk5 + (4.0478098e+03 - tk1*12.1127351); 
      }
    }
    // Species IC4H8
    {
      if (tk1 > 1.388e+03)
      {
        cgspl[73] = 11.225833*tk1*(1-tklog) + -9.089789900000001e-03*tk2 + 
          1.03391432e-06*tk3 + -8.012037149999999e-11*tk4 + 
          2.785440284999999e-15*tk5 + (-7.90617899e+03 - tk1*-36.6411888); 
      }
      else
      {
        cgspl[73] = 0.938433173*tk1*(1-tklog) + -0.01952736435*tk2 + 
          3.6072858e-06*tk3 + -4.893892308333332e-10*tk4 + 3.072177395e-14*tk5 + 
          (-3.95452013e+03 - tk1*19.8337802); 
      }
    }
    // Species IC4H7
    {
      if (tk1 > 1.393e+03)
      {
        cgspl[74] = 11.6382753*tk1*(1-tklog) + -7.88406495e-03*tk2 + 
          8.975647633333334e-07*tk3 + -6.959774391666665e-11*tk4 + 
          2.420705414999999e-15*tk5 + (1.0340823e+04 - tk1*-39.025989); 
      }
      else
      {
        cgspl[74] = -7.20881697e-04*tk1*(1-tklog) + -0.0218247865*tk2 + 
          5.27309795e-06*tk3 + -1.033208191666666e-09*tk4 + 1.0218918e-13*tk5 + 
          (1.43654373e+04 - tk1*23.323434); 
      }
    }
    // Species TC4H9O2
    {
      if (tk1 > 1.388e+03)
      {
        cgspl[75] = 16.7061556*tk1*(1-tklog) + -0.010366389*tk2 + 
          1.195993615e-06*tk3 + -9.356872749999999e-11*tk4 + 
          3.274706929999999e-15*tk5 + (-2.04046924e+04 - tk1*-63.5558608); 
      }
      else
      {
        cgspl[75] = 1.08742583*tk1*(1-tklog) + -0.02913903545*tk2 + 
          7.221544950000001e-06*tk3 + -1.474104458333333e-09*tk4 + 
          1.53384803e-13*tk5 + (-1.4945664e+04 - tk1*20.1871963); 
      }
    }
    // Species IC4H9O2
    {
      if (tk1 > 1.387e+03)
      {
        cgspl[76] = 15.9741221*tk1*(1-tklog) + -0.010676737*tk2 + 
          1.231668508333333e-06*tk3 + -9.635367583333332e-11*tk4 + 
          3.372040229999999e-15*tk5 + (-1.72329304e+04 - tk1*-56.5302409); 
      }
      else
      {
        cgspl[76] = 1.21434293*tk1*(1-tklog) + -0.02726941555*tk2 + 
          6.116693216666668e-06*tk3 + -1.117758683333333e-09*tk4 + 
          1.058708965e-13*tk5 + (-1.1848245e+04 - tk1*23.4153048); 
      }
    }
    // Species IC4H8O2H-I
    {
      if (tk1 > 1.388e+03)
      {
        cgspl[77] = 18.0246456*tk1*(1-tklog) + -9.683413199999999e-03*tk2 + 
          1.124460826666667e-06*tk3 + -8.836677416666664e-11*tk4 + 
          3.102532225e-15*tk5 + (-1.00858977e+04 - tk1*-65.76296929999999); 
      }
      else
      {
        cgspl[77] = 0.994784793*tk1*(1-tklog) + -0.029460612*tk2 + 
          7.086703750000001e-06*tk3 + -1.344754783333333e-09*tk4 + 
          1.27952451e-13*tk5 + (-4.08029057e+03 - tk1*25.895088); 
      }
    }
    // Species TC4H9O
    {
      if (tk1 > 1.397e+03)
      {
        cgspl[78] = 15.0819361*tk1*(1-tklog) + -9.722706399999999e-03*tk2 + 
          1.102222683333334e-06*tk3 + -8.523263333333331e-11*tk4 + 
          2.958984639999999e-15*tk5 + (-1.88111456e+04 - tk1*-57.1658947); 
      }
      else
      {
        cgspl[78] = -0.532084074*tk1*(1-tklog) + -0.02858067345*tk2 + 
          7.002953800000002e-06*tk3 + -1.364545208333333e-09*tk4 + 
          1.3206235e-13*tk5 + (-1.34963439e+04 - tk1*26.2776957); 
      }
    }
    // Species IC4H7O
    {
      if (tk1 > 1.386e+03)
      {
        cgspl[79] = 13.3457615*tk1*(1-tklog) + -8.0609294e-03*tk2 + 
          9.07294005e-07*tk3 + -6.984994783333332e-11*tk4 + 
          2.418041399999999e-15*tk5 + (611.4436439999999 - tk1*-43.6818838); 
      }
      else
      {
        cgspl[79] = 1.74700687*tk1*(1-tklog) + -0.0203891718*tk2 + 
          4.079170716666667e-06*tk3 + -5.887524649999999e-10*tk4 + 
          3.757852944999999e-14*tk5 + (4.86979233e+03 - tk1*19.4535999); 
      }
    }
    // Species IC3H7CHO
    {
      if (tk1 > 1.391e+03)
      {
        cgspl[80] = 13.7501656*tk1*(1-tklog) + -9.156336100000001e-03*tk2 + 
          1.047621048333334e-06*tk3 + -8.152089633333331e-11*tk4 + 
          2.842693264999999e-15*tk5 + (-3.26936771e+04 - tk1*-47.7270548); 
      }
      else
      {
        cgspl[80] = -0.273021382*tk1*(1-tklog) + -0.02448481535*tk2 + 
          5.212834150000001e-06*tk3 + -8.337745416666665e-10*tk4 + 
          6.375603699999999e-14*tk5 + (-2.76054737e+04 - tk1*28.3451139); 
      }
    }
    // Species TC3H6CHO
    {
      if (tk1 > 1.389e+03)
      {
        cgspl[81] = 13.1013047*tk1*(1-tklog) + -8.31959325e-03*tk2 + 
          9.474293716666668e-07*tk3 + -7.348402924999998e-11*tk4 + 
          2.556450804999999e-15*tk5 + (-1.30638647e+04 - tk1*-44.2705813); 
      }
      else
      {
        cgspl[81] = 1.87052762*tk1*(1-tklog) + -0.02074348385*tk2 + 
          4.44692835e-06*tk3 + -7.512763416666665e-10*tk4 + 
          6.393531649999998e-14*tk5 + (-8.97730744e+03 - tk1*16.6174178); 
      }
    }
    // Species IC4H8OOH-IO2
    {
      if (tk1 > 1.385e+03)
      {
        cgspl[82] = 21.8969581*tk1*(1-tklog) + -0.0104818937*tk2 + 
          1.2244415e-06*tk3 + -9.660549499999998e-11*tk4 + 
          3.401127064999999e-15*tk5 + (-2.92664889e+04 - tk1*-82.0540807); 
      }
      else
      {
        cgspl[82] = 2.39424426*tk1*(1-tklog) + -0.03382862745*tk2 + 
          8.618061366666668e-06*tk3 + -1.756633674999999e-09*tk4 + 
          1.799801865e-13*tk5 + (-2.24787495e+04 - tk1*22.5029839); 
      }
    }
    // Species IC4KETII
    {
      if (tk1 > 1.387e+03)
      {
        cgspl[83] = 19.5143059*tk1*(1-tklog) + -9.11886975e-03*tk2 + 
          1.064847676666667e-06*tk3 + -8.400130916666664e-11*tk4 + 
          2.957201749999999e-15*tk5 + (-4.46884836e+04 - tk1*-71.7167584); 
      }
      else
      {
        cgspl[83] = 1.15501614*tk1*(1-tklog) + -0.03053111725*tk2 + 
          7.495188716666668e-06*tk3 + -1.42095545e-09*tk4 + 1.32974301e-13*tk5 + 
          (-3.82747956e+04 - tk1*26.9612235); 
      }
    }
    // Species IC4H6OH
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[84] = 14.0310926*tk1*(1-tklog) + -7.76587705e-03*tk2 + 
          8.879249350000001e-07*tk3 + -6.906549183333331e-11*tk4 + 
          2.407726284999999e-15*tk5 + (-7.69378228e+03 - tk1*-47.6555306); 
      }
      else
      {
        cgspl[84] = 0.863371227*tk1*(1-tklog) + -0.0234355641*tk2 + 
          5.726338983333334e-06*tk3 + -1.1085921e-09*tk4 + 1.069574875e-13*tk5 + 
          (-3.14948305e+03 - tk1*22.9075523); 
      }
    }
    // Species IC3H5CHO
    {
      if (tk1 > 1.396e+03)
      {
        cgspl[85] = 13.6203958*tk1*(1-tklog) + -6.8958596e-03*tk2 + 
          7.889501966666667e-07*tk3 + -6.138793549999998e-11*tk4 + 
          2.10048987e-15*tk5 + (-2.00025274e+04 - tk1*-47.3184531); 
      }
      else
      {
        cgspl[85] = 0.627183793*tk1*(1-tklog) + -0.0233390127*tk2 + 
          6.240510516666667e-06*tk3 + -1.319421183333333e-09*tk4 + 
          1.369760775e-13*tk5 + (-1.57203117e+04 - tk1*21.6034294); 
      }
    }
    // Species IC3H5CO
    {
      if (tk1 > 1.397e+03)
      {
        cgspl[86] = 13.0667437*tk1*(1-tklog) + -5.8352122e-03*tk2 + 
          6.651775383333335e-07*tk3 + -5.162484566666666e-11*tk4 + 
          1.796741244999999e-15*tk5 + (-3.36519344e+03 - tk1*-43.580309); 
      }
      else
      {
        cgspl[86] = 1.85097069*tk1*(1-tklog) + -0.0209427923*tk2 + 
          6.042562183333335e-06*tk3 + -1.380755491666666e-09*tk4 + 
          1.52925423e-13*tk5 + (170.381441 - tk1*15.3014433); 
      }
    }
    // Species TC3H6OCHO
    {
      if (tk1 > 1.394e+03)
      {
        cgspl[87] = 17.0371287*tk1*(1-tklog) + -7.72003225e-03*tk2 + 
          8.805548100000001e-07*tk3 + -6.842377891666666e-11*tk4 + 
          2.384492144999999e-15*tk5 + (-2.75871941e+04 - tk1*-63.727123); 
      }
      else
      {
        cgspl[87] = 0.370830259*tk1*(1-tklog) + -0.02692378305*tk2 + 
          6.374626083333334e-06*tk3 + -1.107351975e-09*tk4 + 
          8.961436499999999e-14*tk5 + (-2.18391262e+04 - tk1*25.8142112); 
      }
    }
    // Species IC4H7OOH
    {
      if (tk1 > 1.392e+03)
      {
        cgspl[88] = 16.9234564*tk1*(1-tklog) + -8.91983845e-03*tk2 + 
          1.023788798333334e-06*tk3 + -7.982458566666666e-11*tk4 + 
          2.78719152e-15*tk5 + (-2.00040686e+04 - tk1*-59.474607); 
      }
      else
      {
        cgspl[88] = 2.99117402*tk1*(1-tklog) + -0.0251674639*tk2 + 
          5.938001016666667e-06*tk3 + -1.11626795e-09*tk4 + 1.055267045e-13*tk5 
          + (-1.51095046e+04 - tk1*15.4537413); 
      }
    }
    // Species TC3H6O2CHO
    {
      if (tk1 > 1.386e+03)
      {
        cgspl[89] = 18.5534443*tk1*(1-tklog) + -8.43871945e-03*tk2 + 
          9.845882750000002e-07*tk3 + -7.762650708333332e-11*tk4 + 
          2.731725934999999e-15*tk5 + (-2.85447191e+04 - tk1*-68.2486667); 
      }
      else
      {
        cgspl[89] = 2.17883383*tk1*(1-tklog) + -0.0270797916*tk2 + 
          6.390598100000001e-06*tk3 + -1.152567533333333e-09*tk4 + 
          1.020950735e-13*tk5 + (-2.27394154e+04 - tk1*20.0751264); 
      }
    }
    // Species CH2CCH2OH
    {
      if (tk1 > 1.372e+03)
      {
        cgspl[90] = 9.707020269999999*tk1*(1-tklog) + -5.698633e-03*tk2 + 
          6.299899366666667e-07*tk3 + -4.793410641666666e-11*tk4 + 
          1.646145625e-15*tk5 + (9.132128839999999e+03 - tk1*-22.5012933); 
      }
      else
      {
        cgspl[90] = 2.88422544*tk1*(1-tklog) + -0.01212140355*tk2 + 
          1.9025378e-06*tk3 + -1.431461116666666e-10*tk4 + 
          -7.108872699999999e-15*tk5 + (1.17935615e+04 - tk1*15.2102335); 
      }
    }
    // Species C5H10-1
    {
      if (tk1 > 1.392e+03)
      {
        cgspl[91] = 14.5851539*tk1*(1-tklog) + -0.01120362355*tk2 + 
          1.272246708333333e-06*tk3 + -9.849080499999998e-11*tk4 + 
          3.421925694999999e-15*tk5 + (-1.00898205e+04 - tk1*-52.3683936); 
      }
      else
      {
        cgspl[91] = -1.06223481*tk1*(1-tklog) + -0.0287109147*tk2 + 
          6.241448166666668e-06*tk3 + -1.061374908333333e-09*tk4 + 
          8.980489449999998e-14*tk5 + (-4.46546666e+03 - tk1*32.273979); 
      }
    }
    // Species C5H91-3
    {
      if (tk1 > 1.392e+03)
      {
        cgspl[92] = 14.1860454*tk1*(1-tklog) + -0.01035644495*tk2 + 
          1.178267695e-06*tk3 + -9.133927749999998e-11*tk4 + 
          3.176611039999999e-15*tk5 + (7.00496135e+03 - tk1*-51.4501773); 
      }
      else
      {
        cgspl[92] = -1.3801395*tk1*(1-tklog) + -0.02788042435*tk2 + 
          6.169065466666667e-06*tk3 + -1.057365841666666e-09*tk4 + 
          8.926941749999998e-14*tk5 + (1.25589824e+04 - tk1*32.6441304); 
      }
    }
    // Species C5H91-4
    {
      if (tk1 > 1.379e+03)
      {
        cgspl[93] = 13.9904314*tk1*(1-tklog) + -9.9981281e-03*tk2 + 
          1.121843228333333e-06*tk3 + -8.620633166666665e-11*tk4 + 
          2.980744915e-15*tk5 + (1.36075123e+04 - tk1*-45.8753046); 
      }
      else
      {
        cgspl[93] = 0.207302224*tk1*(1-tklog) + -0.0237661286*tk2 + 
          4.253871666666667e-06*tk3 + -4.754756733333332e-10*tk4 + 
          1.26963301e-14*tk5 + (1.883001e+04 - tk1*29.752789); 
      }
    }
    // Species C6H13-1
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[94] = 18.538547*tk1*(1-tklog) + -0.0141553981*tk2 + 
          1.60884541e-06*tk3 + -1.246229875e-10*tk4 + 4.331680319999999e-15*tk5 
          + (-5.09299041e+03 - tk1*-70.4490943); 
      }
      else
      {
        cgspl[94] = -0.204871465*tk1*(1-tklog) + -0.0341900636*tk2 + 
          6.9074652e-06*tk3 + -1.05129835e-09*tk4 + 7.656002899999998e-14*tk5 + 
          (1.83280393e+03 - tk1*31.6075093); 
      }
    }
    // Species C6H13O2-1
    {
      if (tk1 > 1.387e+03)
      {
        cgspl[95] = 22.122482*tk1*(1-tklog) + -0.0152804855*tk2 + 
          1.7608359e-06*tk3 + -1.37650005e-10*tk4 + 4.814816729999999e-15*tk5 + 
          (-2.42219722e+04 - tk1*-85.2560175); 
      }
      else
      {
        cgspl[95] = 1.24283154*tk1*(1-tklog) + -0.0385674662*tk2 + 
          8.525823166666668e-06*tk3 + -1.527016933333333e-09*tk4 + 
          1.415293225e-13*tk5 + (-1.65776531e+04 - tk1*27.9518801); 
      }
    }
    // Species NC4H9CHO
    {
      if (tk1 > 1.381e+03)
      {
        cgspl[96] = 16.7965163*tk1*(1-tklog) + -0.01128422595*tk2 + 
          1.27938598e-06*tk3 + -9.897447416666664e-11*tk4 + 3.43772777e-15*tk5 + 
          (-3.5682608e+04 - tk1*-60.9063312); 
      }
      else
      {
        cgspl[96] = 1.59663472*tk1*(1-tklog) + -0.0271770708*tk2 + 
          5.350344183333334e-06*tk3 + -7.798112991666665e-10*tk4 + 
          5.334446599999999e-14*tk5 + (-2.99841025e+04 - tk1*22.1281498); 
      }
    }
    // Species NC4H9CO
    {
      if (tk1 > 1.382e+03)
      {
        cgspl[97] = 16.1782939*tk1*(1-tklog) + -0.010399596*tk2 + 
          1.186313153333333e-06*tk3 + -9.213331249999997e-11*tk4 + 
          3.208485934999999e-15*tk5 + (-1.66537752e+04 - tk1*-55.8943579); 
      }
      else
      {
        cgspl[97] = 2.43530238*tk1*(1-tklog) + -0.0246841188*tk2 + 
          4.831384700000001e-06*tk3 + -7.023865099999999e-10*tk4 + 
          4.841435859999999e-14*tk5 + (-1.14675541e+04 - tk1*19.2695879); 
      }
    }
    // Species NC7H16
    {
      if (tk1 > 1.391e+03)
      {
        cgspl[98] = 22.2148969*tk1*(1-tklog) + -0.0173837875*tk2 + 
          1.973452150000001e-06*tk3 + -1.527487316666666e-10*tk4 + 
          5.306513299999999e-15*tk5 + (-3.42760081e+04 - tk1*-92.3040196); 
      }
      else
      {
        cgspl[98] = -1.26836187*tk1*(1-tklog) + -0.042717791*tk2 + 
          8.755779766666667e-06*tk3 + -1.357881008333333e-09*tk4 + 
          1.011974625e-13*tk5 + (-2.56586565e+04 - tk1*35.3732912); 
      }
    }
    // Species C7H15-1
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[99] = 21.7940709*tk1*(1-tklog) + -0.01631401215*tk2 + 
          1.852304066666667e-06*tk3 + -1.4338929e-10*tk4 + 
          4.981834994999999e-15*tk5 + (-9.20938221e+03 - tk1*-86.4954311); 
      }
      else
      {
        cgspl[99] = -0.499570406*tk1*(1-tklog) + -0.04044132335*tk2 + 
          8.342212566666669e-06*tk3 + -1.304577566666666e-09*tk4 + 
          9.830811349999998e-14*tk5 + (-1.04590223e+03 - tk1*34.6564011); 
      }
    }
    // Species C7H15-2
    {
      if (tk1 > 1.382e+03)
      {
        cgspl[100] = 21.6368842*tk1*(1-tklog) + -0.0161662402*tk2 + 
          1.821230116666667e-06*tk3 + -1.4029755e-10*tk4 + 
          4.858870454999999e-15*tk5 + (-1.05873616e+04 - tk1*-85.2209653); 
      }
      else
      {
        cgspl[100] = -0.0379155767*tk1*(1-tklog) + -0.0378363285*tk2 + 
          6.791227233333334e-06*tk3 + -7.772324524999998e-10*tk4 + 
          2.461803725e-14*tk5 + (-2.35605303e+03 - tk1*33.7321506); 
      }
    }
    // Species C7H15-3
    {
      if (tk1 > 1.382e+03)
      {
        cgspl[101] = 21.6368842*tk1*(1-tklog) + -0.0161662402*tk2 + 
          1.821230116666667e-06*tk3 + -1.4029755e-10*tk4 + 
          4.858870454999999e-15*tk5 + (-1.05873616e+04 - tk1*-85.2209653); 
      }
      else
      {
        cgspl[101] = -0.0379155767*tk1*(1-tklog) + -0.0378363285*tk2 + 
          6.791227233333334e-06*tk3 + -7.772324524999998e-10*tk4 + 
          2.461803725e-14*tk5 + (-2.35605303e+03 - tk1*33.7321506); 
      }
    }
    // Species C7H15-4
    {
      if (tk1 > 1.382e+03)
      {
        cgspl[102] = 21.6368842*tk1*(1-tklog) + -0.0161662402*tk2 + 
          1.821230116666667e-06*tk3 + -1.4029755e-10*tk4 + 
          4.858870454999999e-15*tk5 + (-1.05873616e+04 - tk1*-85.910447); 
      }
      else
      {
        cgspl[102] = -0.0379155767*tk1*(1-tklog) + -0.0378363285*tk2 + 
          6.791227233333334e-06*tk3 + -7.772324524999998e-10*tk4 + 
          2.461803725e-14*tk5 + (-2.35605303e+03 - tk1*33.042669); 
      }
    }
    // Species C7H14-1
    {
      if (tk1 > 1.392e+03)
      {
        cgspl[103] = 21.0898039*tk1*(1-tklog) + -0.0155303939*tk2 + 
          1.76074655e-06*tk3 + -1.361714833333333e-10*tk4 + 
          4.727991094999999e-15*tk5 + (-1.83260065e+04 - 
          tk1*-84.43911079999999); 
      }
      else
      {
        cgspl[103] = -1.67720549*tk1*(1-tklog) + -0.04123058005*tk2 + 
          9.108401800000001e-06*tk3 + -1.565519191666666e-09*tk4 + 
          1.328689915e-13*tk5 + (-1.02168601e+04 - tk1*38.5068032); 
      }
    }
    // Species C7H14-2
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[104] = 20.6192047*tk1*(1-tklog) + -0.01574264955*tk2 + 
          1.786034283333334e-06*tk3 + -1.381897183333333e-10*tk4 + 
          4.799558924999999e-15*tk5 + (-1.96713162e+04 - tk1*-82.2519387); 
      }
      else
      {
        cgspl[104] = -1.16533279*tk1*(1-tklog) + -0.0395219903*tk2 + 
          8.2683611e-06*tk3 + -1.321408408333333e-09*tk4 + 1.026732165e-13*tk5 + 
          (-1.17362359e+04 - tk1*35.987107); 
      }
    }
    // Species C7H14-3
    {
      if (tk1 > 1.392e+03)
      {
        cgspl[105] = 20.682275*tk1*(1-tklog) + -0.01576943145*tk2 + 
          1.792853583333333e-06*tk3 + -1.3890855e-10*tk4 + 
          4.829051964999999e-15*tk5 + (-1.96450604e+04 - tk1*-82.5235002); 
      }
      else
      {
        cgspl[105] = -2.03026994*tk1*(1-tklog) + -0.04131621885*tk2 + 
          9.091907850000001e-06*tk3 + -1.564215183333333e-09*tk4 + 
          1.3378561e-13*tk5 + (-1.15141029e+04 - tk1*40.2316266); 
      }
    }
    // Species C7H132-4
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[106] = 20.2283723*tk1*(1-tklog) + -0.01489091965*tk2 + 
          1.691470016666667e-06*tk3 + -1.309898441666666e-10*tk4 + 
          4.552500564999999e-15*tk5 + (-2.71119859e+03 - tk1*-81.3824175); 
      }
      else
      {
        cgspl[106] = -1.50555526*tk1*(1-tklog) + -0.038737947*tk2 + 
          8.216375600000001e-06*tk3 + -1.323015741666666e-09*tk4 + 
          1.02812411e-13*tk5 + (5.16084218e+03 - tk1*36.4615905); 
      }
    }
    // Species C7H13O2-4
    {
      if (tk1 > 1.389e+03)
      {
        cgspl[107] = 24.8888884*tk1*(1-tklog) + -0.0135864094*tk2 + 
          1.517257481666667e-06*tk3 + -1.1629615e-10*tk4 + 
          4.015515944999999e-15*tk5 + (-1.45377934e+04 - tk1*-102.121928); 
      }
      else
      {
        cgspl[107] = -2.38155227*tk1*(1-tklog) + -0.0456238028*tk2 + 
          1.097854048333333e-05*tk3 + -1.993510358333333e-09*tk4 + 
          1.729095839999999e-13*tk5 + (-5.23001862e+03 - tk1*43.9988302); 
      }
    }
    // Species C7H15O2-1
    {
      if (tk1 > 1.388e+03)
      {
        cgspl[108] = 25.2656556*tk1*(1-tklog) + -0.01754017935*tk2 + 
          2.019646683333333e-06*tk3 + -1.578006716666666e-10*tk4 + 
          5.517706249999999e-15*tk5 + (-2.81113392e+04 - tk1*-100.704794); 
      }
      else
      {
        cgspl[108] = 1.20888552*tk1*(1-tklog) + -0.0442030853*tk2 + 
          9.664354333333334e-06*tk3 + -1.696455741666666e-09*tk4 + 
          1.53230168e-13*tk5 + (-1.92918765e+04 - tk1*29.8117097); 
      }
    }
    // Species C7H15O2-2
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[109] = 26.053564*tk1*(1-tklog) + -0.0171915844*tk2 + 
          1.978554116666667e-06*tk3 + -1.545418675e-10*tk4 + 
          5.402619949999999e-15*tk5 + (-3.06842095e+04 - tk1*-105.407563); 
      }
      else
      {
        cgspl[109] = 0.355252917*tk1*(1-tklog) + -0.04711905035*tk2 + 
          1.111591843333333e-05*tk3 + -2.123296525e-09*tk4 + 2.066055525e-13*tk5 
          + (-2.15795254e+04 - tk1*32.9433435); 
      }
    }
    // Species C7H15O2-3
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[110] = 26.053564*tk1*(1-tklog) + -0.0171915844*tk2 + 
          1.978554116666667e-06*tk3 + -1.545418675e-10*tk4 + 
          5.402619949999999e-15*tk5 + (-3.06842095e+04 - tk1*-105.407563); 
      }
      else
      {
        cgspl[110] = 0.355252917*tk1*(1-tklog) + -0.04711905035*tk2 + 
          1.111591843333333e-05*tk3 + -2.123296525e-09*tk4 + 2.066055525e-13*tk5 
          + (-2.15795254e+04 - tk1*32.9433435); 
      }
    }
    // Species C7H15O2-4
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[111] = 26.053564*tk1*(1-tklog) + -0.0171915844*tk2 + 
          1.978554116666667e-06*tk3 + -1.545418675e-10*tk4 + 
          5.402619949999999e-15*tk5 + (-3.06842095e+04 - tk1*-106.097045); 
      }
      else
      {
        cgspl[111] = 0.355252917*tk1*(1-tklog) + -0.04711905035*tk2 + 
          1.111591843333333e-05*tk3 + -2.123296525e-09*tk4 + 2.066055525e-13*tk5 
          + (-2.15795254e+04 - tk1*32.2538619); 
      }
    }
    // Species C7H14OOH1-2
    {
      if (tk1 > 1.383e+03)
      {
        cgspl[112] = 27.2290222*tk1*(1-tklog) + -0.01624609505*tk2 + 
          1.8543293e-06*tk3 + -1.441073983333333e-10*tk4 + 
          5.021416299999999e-15*tk5 + (-2.2497707e+04 - tk1*-108.901784); 
      }
      else
      {
        cgspl[112] = 1.11146206*tk1*(1-tklog) + -0.0445797696*tk2 + 
          9.455873033333335e-06*tk3 + -1.490750783333333e-09*tk4 + 
          1.11130103e-13*tk5 + (-1.29868314e+04 - tk1*32.9200052); 
      }
    }
    // Species C7H14OOH1-3
    {
      if (tk1 > 1.383e+03)
      {
        cgspl[113] = 27.2290222*tk1*(1-tklog) + -0.01624609505*tk2 + 
          1.8543293e-06*tk3 + -1.441073983333333e-10*tk4 + 
          5.021416299999999e-15*tk5 + (-2.2497707e+04 - tk1*-108.901784); 
      }
      else
      {
        cgspl[113] = 1.11146206*tk1*(1-tklog) + -0.0445797696*tk2 + 
          9.455873033333335e-06*tk3 + -1.490750783333333e-09*tk4 + 
          1.11130103e-13*tk5 + (-1.29868314e+04 - tk1*32.9200052); 
      }
    }
    // Species C7H14OOH2-3
    {
      if (tk1 > 1.389e+03)
      {
        cgspl[114] = 27.4034778*tk1*(1-tklog) + -0.01637639465*tk2 + 
          1.883196166666667e-06*tk3 + -1.470372075e-10*tk4 + 
          5.139347249999999e-15*tk5 + (-2.48636651e+04 - tk1*-110.201978); 
      }
      else
      {
        cgspl[114] = 0.849786005*tk1*(1-tklog) + -0.0462553598*tk2 + 
          1.04226639e-05*tk3 + -1.815669916666666e-09*tk4 + 1.56360199e-13*tk5 + 
          (-1.53640679e+04 - tk1*33.2997487); 
      }
    }
    // Species C7H14OOH2-4
    {
      if (tk1 > 1.389e+03)
      {
        cgspl[115] = 27.4034778*tk1*(1-tklog) + -0.01637639465*tk2 + 
          1.883196166666667e-06*tk3 + -1.470372075e-10*tk4 + 
          5.139347249999999e-15*tk5 + (-2.48636651e+04 - tk1*-110.201978); 
      }
      else
      {
        cgspl[115] = 0.849786005*tk1*(1-tklog) + -0.0462553598*tk2 + 
          1.04226639e-05*tk3 + -1.815669916666666e-09*tk4 + 1.56360199e-13*tk5 + 
          (-1.53640679e+04 - tk1*33.2997487); 
      }
    }
    // Species C7H14OOH3-5
    {
      if (tk1 > 1.389e+03)
      {
        cgspl[116] = 27.4034778*tk1*(1-tklog) + -0.01637639465*tk2 + 
          1.883196166666667e-06*tk3 + -1.470372075e-10*tk4 + 
          5.139347249999999e-15*tk5 + (-2.48636651e+04 - tk1*-110.201978); 
      }
      else
      {
        cgspl[116] = 0.849786005*tk1*(1-tklog) + -0.0462553598*tk2 + 
          1.04226639e-05*tk3 + -1.815669916666666e-09*tk4 + 1.56360199e-13*tk5 + 
          (-1.53640679e+04 - tk1*33.2997487); 
      }
    }
    // Species C7H14OOH4-2
    {
      if (tk1 > 1.389e+03)
      {
        cgspl[117] = 27.4034778*tk1*(1-tklog) + -0.01637639465*tk2 + 
          1.883196166666667e-06*tk3 + -1.470372075e-10*tk4 + 
          5.139347249999999e-15*tk5 + (-2.48636651e+04 - tk1*-110.89146); 
      }
      else
      {
        cgspl[117] = 0.849786005*tk1*(1-tklog) + -0.0462553598*tk2 + 
          1.04226639e-05*tk3 + -1.815669916666666e-09*tk4 + 1.56360199e-13*tk5 + 
          (-1.53640679e+04 - tk1*32.6102671); 
      }
    }
    // Species C7H14OOH4-3
    {
      if (tk1 > 1.389e+03)
      {
        cgspl[118] = 27.4034778*tk1*(1-tklog) + -0.01637639465*tk2 + 
          1.883196166666667e-06*tk3 + -1.470372075e-10*tk4 + 
          5.139347249999999e-15*tk5 + (-2.48636651e+04 - tk1*-110.89146); 
      }
      else
      {
        cgspl[118] = 0.849786005*tk1*(1-tklog) + -0.0462553598*tk2 + 
          1.04226639e-05*tk3 + -1.815669916666666e-09*tk4 + 1.56360199e-13*tk5 + 
          (-1.53640679e+04 - tk1*32.6102671); 
      }
    }
    // Species C7H14OOH1-3O2
    {
      if (tk1 > 1.388e+03)
      {
        cgspl[119] = 32.0144349*tk1*(1-tklog) + -0.0170027998*tk2 + 
          1.9735976e-06*tk3 + -1.550502891666666e-10*tk4 + 
          5.442550599999999e-15*tk5 + (-4.27171859e+04 - tk1*-131.118243); 
      }
      else
      {
        cgspl[119] = 1.89718004*tk1*(1-tklog) + -0.053114898*tk2 + 
          1.337950363333334e-05*tk3 + -2.697075774999999e-09*tk4 + 
          2.72925236e-13*tk5 + (-3.22790229e+04 - tk1*30.2474612); 
      }
    }
    // Species C7H14OOH2-3O2
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[120] = 32.8358296*tk1*(1-tklog) + -0.01663836425*tk2 + 
          1.93055475e-06*tk3 + -1.516321983333333e-10*tk4 + 
          5.321720949999999e-15*tk5 + (-4.52873003e+04 - tk1*-135.97571); 
      }
      else
      {
        cgspl[120] = 1.40912479*tk1*(1-tklog) + -0.055360417*tk2 + 
          1.451963458333333e-05*tk3 + -3.030522908333333e-09*tk4 + 
          3.141753444999999e-13*tk5 + (-3.46304788e+04 - tk1*31.6223383); 
      }
    }
    // Species C7H14OOH2-4O2
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[121] = 32.8358296*tk1*(1-tklog) + -0.01663836425*tk2 + 
          1.93055475e-06*tk3 + -1.516321983333333e-10*tk4 + 
          5.321720949999999e-15*tk5 + (-4.52873003e+04 - tk1*-135.97571); 
      }
      else
      {
        cgspl[121] = 1.40912479*tk1*(1-tklog) + -0.055360417*tk2 + 
          1.451963458333333e-05*tk3 + -3.030522908333333e-09*tk4 + 
          3.141753444999999e-13*tk5 + (-3.46304788e+04 - tk1*31.6223383); 
      }
    }
    // Species C7H14OOH4-2O2
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[122] = 32.8358296*tk1*(1-tklog) + -0.01663836425*tk2 + 
          1.93055475e-06*tk3 + -1.516321983333333e-10*tk4 + 
          5.321720949999999e-15*tk5 + (-4.52873003e+04 - tk1*-135.97571); 
      }
      else
      {
        cgspl[122] = 1.40912479*tk1*(1-tklog) + -0.055360417*tk2 + 
          1.451963458333333e-05*tk3 + -3.030522908333333e-09*tk4 + 
          3.141753444999999e-13*tk5 + (-3.46304788e+04 - tk1*31.6223383); 
      }
    }
    // Species C7H14OOH4-3O2
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[123] = 32.8358296*tk1*(1-tklog) + -0.01663836425*tk2 + 
          1.93055475e-06*tk3 + -1.516321983333333e-10*tk4 + 
          5.321720949999999e-15*tk5 + (-4.52873003e+04 - tk1*-135.97571); 
      }
      else
      {
        cgspl[123] = 1.40912479*tk1*(1-tklog) + -0.055360417*tk2 + 
          1.451963458333333e-05*tk3 + -3.030522908333333e-09*tk4 + 
          3.141753444999999e-13*tk5 + (-3.46304788e+04 - tk1*31.6223383); 
      }
    }
    // Species C7H14O1-3
    {
      if (tk1 > 1.395e+03)
      {
        cgspl[124] = 22.6918916*tk1*(1-tklog) + -0.0166255236*tk2 + 
          1.907629416666667e-06*tk3 + -1.487129458333333e-10*tk4 + 
          5.192055599999999e-15*tk5 + (-3.41591419e+04 - tk1*-94.6346563); 
      }
      else
      {
        cgspl[124] = -5.5003645*tk1*(1-tklog) + -0.050077544*tk2 + 
          1.216559761666667e-05*tk3 + -2.327868916666666e-09*tk4 + 
          2.211228479999999e-13*tk5 + (-2.44292287e+04 - tk1*56.4768482); 
      }
    }
    // Species C7H14O2-4
    {
      if (tk1 > 1.399e+03)
      {
        cgspl[125] = 23.2692731*tk1*(1-tklog) + -0.01627926955*tk2 + 
          1.8604151e-06*tk3 + -1.44644715e-10*tk4 + 5.040660749999998e-15*tk5 + 
          (-3.64349664e+04 - tk1*-97.9457278); 
      }
      else
      {
        cgspl[125] = -5.39477206*tk1*(1-tklog) + -0.0509232885*tk2 + 
          1.266743726666667e-05*tk3 + -2.471150166666666e-09*tk4 + 
          2.374271849999999e-13*tk5 + (-2.67118295e+04 - tk1*55.1731319); 
      }
    }
    // Species C7H14O3-5
    {
      if (tk1 > 1.399e+03)
      {
        cgspl[126] = 23.2692731*tk1*(1-tklog) + -0.01627926955*tk2 + 
          1.8604151e-06*tk3 + -1.44644715e-10*tk4 + 5.040660749999998e-15*tk5 + 
          (-3.64349664e+04 - tk1*-97.9457278); 
      }
      else
      {
        cgspl[126] = -5.39477206*tk1*(1-tklog) + -0.0509232885*tk2 + 
          1.266743726666667e-05*tk3 + -2.471150166666666e-09*tk4 + 
          2.374271849999999e-13*tk5 + (-2.67118295e+04 - tk1*55.1731319); 
      }
    }
    // Species NC7KET13
    {
      if (tk1 > 1.388e+03)
      {
        cgspl[127] = 29.0744507*tk1*(1-tklog) + -0.0158588729*tk2 + 
          1.837710766666667e-06*tk3 + -1.4421134e-10*tk4 + 
          5.058084949999999e-15*tk5 + (-5.82345477e+04 - tk1*-118.064324); 
      }
      else
      {
        cgspl[127] = 2.21629328*tk1*(1-tklog) + -0.0473686881*tk2 + 
          1.150789418333333e-05*tk3 + -2.231836666666666e-09*tk4 + 
          2.181960905e-13*tk5 + (-4.8796551e+04 - tk1*26.3157133); 
      }
    }
    // Species C4H7OOH1-4
    {
      if (tk1 > 1.393e+03)
      {
        cgspl[128] = 16.6978343*tk1*(1-tklog) + -9.02891995e-03*tk2 + 
          1.037461398333333e-06*tk3 + -8.095661949999998e-11*tk4 + 
          2.828432754999999e-15*tk5 + (-1.8534472e+04 - tk1*-57.6609547); 
      }
      else
      {
        cgspl[128] = 1.7980467*tk1*(1-tklog) + -0.02616512755*tk2 + 
          6.0773041e-06*tk3 + -1.099765733333333e-09*tk4 + 
          9.858299649999997e-14*tk5 + (-1.32846748e+04 - tk1*22.5862038); 
      }
    }
    // Species C4H7O1-4
    {
      if (tk1 > 1.393e+03)
      {
        cgspl[129] = 13.1417783*tk1*(1-tklog) + -8.208302549999999e-03*tk2 + 
          9.282486650000001e-07*tk3 + -7.167623074999998e-11*tk4 + 
          2.486150029999999e-15*tk5 + (1.67967537e+03 - tk1*-41.6160087); 
      }
      else
      {
        cgspl[129] = -0.292647947*tk1*(1-tklog) + -0.02415745595*tk2 + 
          5.802220333333334e-06*tk3 + -1.099345566666666e-09*tk4 + 
          1.032427765e-13*tk5 + (6.29978883e+03 - tk1*30.3629978); 
      }
    }
    // Species CH3CHCHO
    {
      if (tk1 > 1.253e+03)
      {
        cgspl[130] = 8.2777209*tk1*(1-tklog) + -9.7843594e-03*tk2 + 
          1.412625148333334e-06*tk3 + -1.277081066666666e-10*tk4 + 
          4.932207709999999e-15*tk5 + (-8.34456062e+03 - tk1*-21.7652483); 
      }
      else
      {
        cgspl[130] = -2.72811212*tk1*(1-tklog) + -0.01901772415*tk2 + 
          3.2797022e-06*tk3 + -4.246288083333332e-10*tk4 + 
          3.643945459999999e-14*tk5 + (-3.51122739e+03 - tk1*40.4496677); 
      }
    }
    // Species IC4H7-I1
    {
      if (tk1 > 1.389e+03)
      {
        cgspl[131] = 11.09576*tk1*(1-tklog) + -7.89050245e-03*tk2 + 
          8.973660250000001e-07*tk3 + -6.953001008333331e-11*tk4 + 
          2.417007404999999e-15*tk5 + (2.24175827e+04 - tk1*-34.0426822); 
      }
      else
      {
        cgspl[131] = 1.87632434*tk1*(1-tklog) + -0.01772430535*tk2 + 
          3.55174915e-06*tk3 + -5.611670766666665e-10*tk4 + 
          4.515831694999999e-14*tk5 + (2.58712914e+04 - tk1*16.2429161); 
      }
    }
    // Species YC7H15
    {
      if (tk1 > 1.384e+03)
      {
        cgspl[132] = 20.4581471*tk1*(1-tklog) + -0.01715381805*tk2 + 
          1.96837725e-06*tk3 + -1.534413425e-10*tk4 + 5.356690299999999e-15*tk5 
          + (-1.34978774e+04 - tk1*-81.8212264); 
      }
      else
      {
        cgspl[132] = 1.30897106*tk1*(1-tklog) + -0.0348068221*tk2 + 
          5.519167616666667e-06*tk3 + -4.857402133333333e-10*tk4 + 
          -1.772136569999999e-15*tk5 + (-5.78512513e+03 - tk1*24.5658235); 
      }
    }
    // Species XC7H14
    {
      if (tk1 > 1.392e+03)
      {
        cgspl[133] = 21.016403*tk1*(1-tklog) + -0.01576072985*tk2 + 
          1.80123245e-06*tk3 + -1.400334925e-10*tk4 + 4.879461564999999e-15*tk5 
          + (-2.1211705e+04 - tk1*-86.3818785); 
      }
      else
      {
        cgspl[133] = -1.33081497*tk1*(1-tklog) + -0.0410541176*tk2 + 
          9.152055050000001e-06*tk3 + -1.619041233333333e-09*tk4 + 
          1.44443271e-13*tk5 + (-1.32021384e+04 - tk1*34.3549746); 
      }
    }
    // Species YC7H14
    {
      if (tk1 > 1.392e+03)
      {
        cgspl[134] = 20.5074323*tk1*(1-tklog) + -0.01598220835*tk2 + 
          1.826979633333334e-06*tk3 + -1.420531133333333e-10*tk4 + 
          4.950180599999999e-15*tk5 + (-2.23945609e+04 - tk1*-83.9266561); 
      }
      else
      {
        cgspl[134] = -0.8422326490000001*tk1*(1-tklog) + -0.03948991485*tk2 + 
          8.409574583333334e-06*tk3 + -1.407783733333333e-09*tk4 + 
          1.186009935e-13*tk5 + (-1.45971538e+04 - tk1*31.9096189); 
      }
    }
    // Species XC7H13-Z
    {
      if (tk1 > 1.392e+03)
      {
        cgspl[135] = 20.6194069*tk1*(1-tklog) + -0.0149139895*tk2 + 
          1.707394733333334e-06*tk3 + -1.328974391666666e-10*tk4 + 
          4.634790189999999e-15*tk5 + (-4.13779765e+03 - 
          tk1*-85.47591009999999); 
      }
      else
      {
        cgspl[135] = -1.64635315*tk1*(1-tklog) + -0.04022549745*tk2 + 
          9.081524216666667e-06*tk3 + -1.61583975e-09*tk4 + 1.44041191e-13*tk5 + 
          (3.80138821e+03 - tk1*34.7109409); 
      }
    }
    // Species YC7H13-Y2
    {
      if (tk1 > 1.389e+03)
      {
        cgspl[136] = 19.6153468*tk1*(1-tklog) + -0.01535370525*tk2 + 
          1.758841133333334e-06*tk3 + -1.369549658333333e-10*tk4 + 
          4.777498639999999e-15*tk5 + (-6.29650914e+03 - tk1*-80.9254914); 
      }
      else
      {
        cgspl[136] = -0.306783292*tk1*(1-tklog) + -0.03603455725*tk2 + 
          7.003898600000001e-06*tk3 + -1.0004174e-09*tk4 + 
          6.630260699999998e-14*tk5 + (1.21727364e+03 - tk1*28.0339431); 
      }
    }
    // Species XC7H13O-Z
    {
      if (tk1 > 1.393e+03)
      {
        cgspl[137] = 24.4947643*tk1*(1-tklog) + -0.01381658035*tk2 + 
          1.546473843333334e-06*tk3 + -1.186487133333333e-10*tk4 + 
          4.098174679999999e-15*tk5 + (-1.57122284e+04 - tk1*-101.342872); 
      }
      else
      {
        cgspl[137] = -2.29755615*tk1*(1-tklog) + -0.047103339*tk2 + 
          1.221469965e-05*tk3 + -2.471858424999999e-09*tk4 + 
          2.440194824999999e-13*tk5 + (-6.85663941e+03 - tk1*41.0526275); 
      }
    }
    // Species YC7H13O-Y2
    {
      if (tk1 > 1.391e+03)
      {
        cgspl[138] = 24.489557*tk1*(1-tklog) + -0.0136249639*tk2 + 
          1.51146668e-06*tk3 + -1.152898975e-10*tk4 + 3.966495684999999e-15*tk5 
          + (-1.87690928e+04 - tk1*-102.028362); 
      }
      else
      {
        cgspl[138] = -0.727101657*tk1*(1-tklog) + -0.0443568192*tk2 + 
          1.110806368333333e-05*tk3 + -2.167884383333333e-09*tk4 + 
          2.064706284999999e-13*tk5 + (-1.03333355e+04 - tk1*32.3860863); 
      }
    }
    // Species YC7H15O2
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[139] = 26.3777368*tk1*(1-tklog) + -0.0170497967*tk2 + 
          1.9619758e-06*tk3 + -1.532348333333333e-10*tk4 + 
          5.356660249999999e-15*tk5 + (-3.39133584e+04 - tk1*-111.594572); 
      }
      else
      {
        cgspl[139] = -0.176194227*tk1*(1-tklog) + -0.0486283817*tk2 + 
          1.184726306666667e-05*tk3 + -2.337391383333333e-09*tk4 + 
          2.334389515e-13*tk5 + (-2.46466146e+04 - tk1*30.8813371); 
      }
    }
    // Species ACC6H10
    {
      if (tk1 > 1.395e+03)
      {
        cgspl[140] = 17.1179863*tk1*(1-tklog) + -0.01144488185*tk2 + 
          1.302817396666667e-06*tk3 + -1.010111041666666e-10*tk4 + 
          3.512993704999999e-15*tk5 + (-3.26888037e+03 - tk1*-66.6240298); 
      }
      else
      {
        cgspl[140] = -0.762523956*tk1*(1-tklog) + -0.0338082029*tk2 + 
          8.694858716666668e-06*tk3 + -1.816558908333333e-09*tk4 + 
          1.89530755e-13*tk5 + (2.72970447e+03 - tk1*28.4975422); 
      }
    }
    // Species ACC6H9-A
    {
      if (tk1 > 1.398e+03)
      {
        cgspl[141] = 17.5340261*tk1*(1-tklog) + -0.0102380972*tk2 + 
          1.166393333333334e-06*tk3 + -9.048562499999998e-11*tk4 + 
          3.148240374999999e-15*tk5 + (1.49755134e+04 - tk1*-69.03166589999999); 
      }
      else
      {
        cgspl[141] = -1.7084984*tk1*(1-tklog) + -0.0361216337*tk2 + 
          1.03691751e-05*tk3 + -2.363327158333333e-09*tk4 + 2.61435174e-13*tk5 + 
          (2.10506573e+04 - tk1*32.0184824); 
      }
    }
    // Species ACC6H9-D
    {
      if (tk1 > 1.398e+03)
      {
        cgspl[142] = 17.5340261*tk1*(1-tklog) + -0.0102380972*tk2 + 
          1.166393333333334e-06*tk3 + -9.048562499999998e-11*tk4 + 
          3.148240374999999e-15*tk5 + (1.49755134e+04 - tk1*-69.03166589999999); 
      }
      else
      {
        cgspl[142] = -1.7084984*tk1*(1-tklog) + -0.0361216337*tk2 + 
          1.03691751e-05*tk3 + -2.363327158333333e-09*tk4 + 2.61435174e-13*tk5 + 
          (2.10506573e+04 - tk1*32.0184824); 
      }
    }
    // Species NEOC5H11
    {
      if (tk1 > 1.449e+03)
      {
        cgspl[143] = 20.3101659*tk1*(1-tklog) + -0.01073233485*tk2 + 
          1.286876271666667e-06*tk3 + -1.033189016666666e-10*tk4 + 
          3.68203782e-15*tk5 + (-6.45138607e+03 - tk1*-90.1108572); 
      }
      else
      {
        cgspl[143] = 3.39222761*tk1*(1-tklog) + -0.01770345445*tk2 + 
          -2.000334666666667e-06*tk3 + 1.950938624999999e-09*tk4 + 
          -3.399979685e-13*tk5 + (1.81088321e+03 - tk1*9.54432517); 
      }
    }
    // Species TC4H9CHO
    {
      if (tk1 > 1.396e+03)
      {
        cgspl[144] = 18.4056359*tk1*(1-tklog) + -0.01076412115*tk2 + 
          1.23109255e-06*tk3 + -9.578499916666663e-11*tk4 + 
          3.340018724999999e-15*tk5 + (-3.84092887e+04 - tk1*-74.9886911); 
      }
      else
      {
        cgspl[144] = -1.75588233*tk1*(1-tklog) + -0.0344370608*tk2 + 
          8.286017850000002e-06*tk3 + -1.524347083333333e-09*tk4 + 
          1.36028467e-13*tk5 + (-3.14740629e+04 - tk1*33.133448); 
      }
    }
    // Species TC4H9CO
    {
      if (tk1 > 1.397e+03)
      {
        cgspl[145] = 17.9864364*tk1*(1-tklog) + -9.7007182e-03*tk2 + 
          1.111193116666667e-06*tk3 + -8.654970499999999e-11*tk4 + 
          3.020322859999999e-15*tk5 + (-1.94463168e+04 - tk1*-71.0669249); 
      }
      else
      {
        cgspl[145] = -0.977833363*tk1*(1-tklog) + -0.0320286742*tk2 + 
          7.773481133333334e-06*tk3 + -1.43452235e-09*tk4 + 1.278406525e-13*tk5 
          + (-1.29463546e+04 - tk1*30.5727393); 
      }
    }
    // Species PC7H15
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[146] = 23.3667406*tk1*(1-tklog) + -0.01536597775*tk2 + 
          1.725135116666667e-06*tk3 + -1.326145991666667e-10*tk4 + 
          4.586552814999999e-15*tk5 + (-1.37695377e+04 - tk1*-99.6212012); 
      }
      else
      {
        cgspl[146] = -2.19681832*tk1*(1-tklog) + -0.04365670395*tk2 + 
          9.394845050000001e-06*tk3 + -1.480296266666666e-09*tk4 + 
          1.06449676e-13*tk5 + (-4.67937082e+03 - tk1*38.6153467); 
      }
    }
    // Species IC8H18
    {
      if (tk1 > 1.396e+03)
      {
        cgspl[147] = 27.137359*tk1*(1-tklog) + -0.0189502445*tk2 + 
          2.1572893e-06*tk3 + -1.6730031e-10*tk4 + 5.820028999999999e-15*tk5 + 
          (-4.07958177e+04 - tk1*-123.277495); 
      }
      else
      {
        cgspl[147] = -4.20868893*tk1*(1-tklog) + -0.0557202905*tk2 + 
          1.31891097e-05*tk3 + -2.436718683333333e-09*tk4 + 
          2.218715954999999e-13*tk5 + (-2.99446875e+04 - tk1*44.9521701); 
      }
    }
    // Species CC8H17
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[148] = 25.1497158*tk1*(1-tklog) + -0.01855484225*tk2 + 
          2.114241383333334e-06*tk3 + -1.640611908333333e-10*tk4 + 
          5.709694699999999e-15*tk5 + (-1.8276179e+04 - tk1*-109.056834); 
      }
      else
      {
        cgspl[148] = -0.0973159697*tk1*(1-tklog) + -0.0446326862*tk2 + 
          8.547896900000002e-06*tk3 + -1.1470044e-09*tk4 + 
          6.389419799999998e-14*tk5 + (-8.811473019999999e+03 - tk1*28.9791898); 
      }
    }
    // Species IC8H16
    {
      if (tk1 > 1.394e+03)
      {
        cgspl[149] = 25.6756746*tk1*(1-tklog) + -0.0170900999*tk2 + 
          1.933382533333333e-06*tk3 + -1.49329565e-10*tk4 + 
          5.180650099999999e-15*tk5 + (-2.62458324e+04 - tk1*-113.928273); 
      }
      else
      {
        cgspl[149] = -2.79610447*tk1*(1-tklog) + -0.050418086*tk2 + 
          1.187084418333333e-05*tk3 + -2.1721652e-09*tk4 + 
          1.950159069999999e-13*tk5 + (-1.64002496e+04 - tk1*38.8854068); 
      }
    }
    // Species JC8H16
    {
      if (tk1 > 1.397e+03)
      {
        cgspl[150] = 26.0101527*tk1*(1-tklog) + -0.01698516215*tk2 + 
          1.923707516666667e-06*tk3 + -1.486856783333333e-10*tk4 + 
          5.160609999999999e-15*tk5 + (-2.65174535e+04 - tk1*-115.359195); 
      }
      else
      {
        cgspl[150] = -3.31862122*tk1*(1-tklog) + -0.0521576525*tk2 + 
          1.274605021666667e-05*tk3 + -2.434743491666666e-09*tk4 + 
          2.284569309999999e-13*tk5 + (-1.6544825e+04 - tk1*41.4548253); 
      }
    }
    // Species BC8H17O2
    {
      if (tk1 > 1.394e+03)
      {
        cgspl[151] = 30.9351615*tk1*(1-tklog) + -0.0187051282*tk2 + 
          2.151182833333334e-06*tk3 + -1.679540416666666e-10*tk4 + 
          5.869945549999999e-15*tk5 + (-3.77457753e+04 - tk1*-136.7308); 
      }
      else
      {
        cgspl[151] = -3.07002356*tk1*(1-tklog) + -0.061320219*tk2 + 
          1.62033794e-05*tk3 + -3.409396875e-09*tk4 + 3.545512734999999e-13*tk5 
          + (-2.64014308e+04 - tk1*44.0345691); 
      }
    }
    // Species CC8H17O2
    {
      if (tk1 > 1.394e+03)
      {
        cgspl[152] = 30.9721695*tk1*(1-tklog) + -0.0186683041*tk2 + 
          2.145402983333333e-06*tk3 + -1.674228866666666e-10*tk4 + 
          5.849453599999999e-15*tk5 + (-3.79648855e+04 - tk1*-138.456446); 
      }
      else
      {
        cgspl[152] = -1.80143766*tk1*(1-tklog) + -0.0590152555*tk2 + 
          1.517718276666667e-05*tk3 + -3.112336341666666e-09*tk4 + 
          3.169996535e-13*tk5 + (-2.68892049e+04 - tk1*36.2716425); 
      }
    }
    // Species AC8H16OOH-A
    {
      if (tk1 > 1.394e+03)
      {
        cgspl[153] = 32.2733666*tk1*(1-tklog) + -0.0180017957*tk2 + 
          2.076410716666667e-06*tk3 + -1.6245188e-10*tk4 + 
          5.686120949999998e-15*tk5 + (-2.80443391e+04 - tk1*-140.571052); 
      }
      else
      {
        cgspl[153] = -1.84900727*tk1*(1-tklog) + -0.059235188*tk2 + 
          1.499348545e-05*tk3 + -2.969030616666666e-09*tk4 + 
          2.898715754999999e-13*tk5 + (-1.64331259e+04 - tk1*41.7717295); 
      }
    }
    // Species BC8H16OOH-C
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[154] = 31.7641522*tk1*(1-tklog) + -0.01824962635*tk2 + 
          2.107307383333334e-06*tk3 + -1.649910375e-10*tk4 + 
          5.777983599999999e-15*tk5 + (-3.25683115e+04 - tk1*-138.935214); 
      }
      else
      {
        cgspl[154] = 0.450202986*tk1*(1-tklog) + -0.053569686*tk2 + 
          1.219663148333333e-05*tk3 + -2.126896258333333e-09*tk4 + 
          1.818006005e-13*tk5 + (-2.14096422e+04 - tk1*30.1864197); 
      }
    }
    // Species DC8H16OOH-C
    {
      if (tk1 > 1.389e+03)
      {
        cgspl[155] = 30.7262781*tk1*(1-tklog) + -0.0186639326*tk2 + 
          2.1528514e-06*tk3 + -1.684307683333333e-10*tk4 + 
          5.895257149999998e-15*tk5 + (-2.94421033e+04 - tk1*-131.98669); 
      }
      else
      {
        cgspl[155] = 1.37822561*tk1*(1-tklog) + -0.050681773*tk2 + 
          1.089171655e-05*tk3 + -1.770723458333333e-09*tk4 + 1.397263245e-13*tk5 
          + (-1.87378447e+04 - tk1*27.3357589); 
      }
    }
    // Species IC8ETERAB
    {
      if (tk1 > 1.403e+03)
      {
        cgspl[156] = 27.6798014*tk1*(1-tklog) + -0.0179661503*tk2 + 
          2.05121095e-06*tk3 + -1.593910066666666e-10*tk4 + 
          5.552712699999998e-15*tk5 + (-4.05912134e+04 - tk1*-125.295695); 
      }
      else
      {
        cgspl[156] = -7.80049041*tk1*(1-tklog) + -0.0617904385*tk2 + 
          1.597702548333334e-05*tk3 + -3.196210516666666e-09*tk4 + 
          3.108807139999999e-13*tk5 + (-2.88197649e+04 - tk1*63.4400053); 
      }
    }
    // Species IC8ETERBD
    {
      if (tk1 > 1.403e+03)
      {
        cgspl[157] = 27.6798014*tk1*(1-tklog) + -0.0179661503*tk2 + 
          2.05121095e-06*tk3 + -1.593910066666666e-10*tk4 + 
          5.552712699999998e-15*tk5 + (-4.05912134e+04 - tk1*-126.392826); 
      }
      else
      {
        cgspl[157] = -7.80049041*tk1*(1-tklog) + -0.0617904385*tk2 + 
          1.597702548333334e-05*tk3 + -3.196210516666666e-09*tk4 + 
          3.108807139999999e-13*tk5 + (-2.88197649e+04 - tk1*62.3428739); 
      }
    }
    // Species IC8KETDB
    {
      if (tk1 > 1.395e+03)
      {
        cgspl[158] = 34.7542216*tk1*(1-tklog) + -0.01705678565*tk2 + 
          1.976160216666667e-06*tk3 + -1.550818116666666e-10*tk4 + 
          5.439971849999999e-15*tk5 + (-6.61213882e+04 - tk1*-153.639919); 
      }
      else
      {
        cgspl[158] = -2.64616572*tk1*(1-tklog) + -0.0631770405*tk2 + 
          1.669284683333333e-05*tk3 + -3.398669783333332e-09*tk4 + 
          3.369093945e-13*tk5 + (-5.36421698e+04 - tk1*45.4596528); 
      }
    }
    // Species ISO002
    {
      if (tk1 > 1.393e+03)
      {
        cgspl[159] = 37.57858*tk1*(1-tklog) + -0.01822568575*tk2 + 
          2.112720266666667e-06*tk3 + -1.658420033333333e-10*tk4 + 
          5.818177799999999e-15*tk5 + (-5.25653329e+04 - tk1*-168.169424); 
      }
      else
      {
        cgspl[159] = -1.49381512*tk1*(1-tklog) + -0.06838076899999999*tk2 + 
          1.90581035e-05*tk3 + -4.159154458333332e-09*tk4 + 
          4.426866759999999e-13*tk5 + (-3.97954365e+04 - tk1*38.6453145); 
      }
    }
    // Species ISO003
    {
      if (tk1 > 1.393e+03)
      {
        cgspl[160] = 36.8372436*tk1*(1-tklog) + -0.0185370962*tk2 + 
          2.148300533333334e-06*tk3 + -1.686081249999999e-10*tk4 + 
          5.914572799999999e-15*tk5 + (-4.93845688e+04 - tk1*-161.072392); 
      }
      else
      {
        cgspl[160] = -1.29316549*tk1*(1-tklog) + -0.0663812575*tk2 + 
          1.790060466666667e-05*tk3 + -3.788707399999999e-09*tk4 + 
          3.934884399999999e-13*tk5 + (-3.67109267e+04 - tk1*41.5232002); 
      }
    }
    // Species ISO004
    {
      if (tk1 > 1.393e+03)
      {
        cgspl[161] = 30.2815958*tk1*(1-tklog) + -0.0189364536*tk2 + 
          2.17447305e-06*tk3 + -1.696035108333333e-10*tk4 + 
          5.923563349999999e-15*tk5 + (-3.48032462e+04 - tk1*-131.647036); 
      }
      else
      {
        cgspl[161] = -1.78614072*tk1*(1-tklog) + -0.0573774515*tk2 + 
          1.417674531666667e-05*tk3 + -2.783674458333333e-09*tk4 + 
          2.72586614e-13*tk5 + (-2.37742056e+04 - tk1*40.0275514); 
      }
    }
    // Species ISO005
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[162] = 32.5510943*tk1*(1-tklog) + -0.0173993366*tk2 + 
          1.9740765e-06*tk3 + -1.52846565e-10*tk4 + 5.313162749999998e-15*tk5 + 
          (-2.9511989e+04 - tk1*-141.56826); 
      }
      else
      {
        cgspl[162] = -2.0691273*tk1*(1-tklog) + -0.0580710695*tk2 + 
          1.406711211666667e-05*tk3 + -2.589078324999999e-09*tk4 + 
          2.299401515e-13*tk5 + (-1.76383455e+04 - tk1*44.0292848); 
      }
    }
    // Species ISO006
    {
      if (tk1 > 1.395e+03)
      {
        cgspl[163] = 32.9649795*tk1*(1-tklog) + -0.0177260763*tk2 + 
          2.046011616666667e-06*tk3 + -1.601463349999999e-10*tk4 + 
          5.607183449999999e-15*tk5 + (-3.05966248e+04 - tk1*-144.760131); 
      }
      else
      {
        cgspl[163] = -3.22207871*tk1*(1-tklog) + -0.0633178175*tk2 + 
          1.707111816666667e-05*tk3 + -3.604647616666666e-09*tk4 + 
          3.724950484999999e-13*tk5 + (-1.86411004e+04 - tk1*47.3182493); 
      }
    }
    // Species ISO007
    {
      if (tk1 > 1.396e+03)
      {
        cgspl[164] = 26.7069782*tk1*(1-tklog) + -0.01788303005*tk2 + 
          2.036311733333334e-06*tk3 + -1.579471916666666e-10*tk4 + 
          5.495392149999998e-15*tk5 + (-1.57229692e+04 - tk1*-117.001113); 
      }
      else
      {
        cgspl[164] = -3.41944741*tk1*(1-tklog) + -0.0534015945*tk2 + 
          1.275685938333333e-05*tk3 + -2.37784735e-09*tk4 + 
          2.182393244999999e-13*tk5 + (-5.33514196e+03 - tk1*44.5471727); 
      }
    }
    // Species ISO008
    {
      if (tk1 > 1.394e+03)
      {
        cgspl[165] = 34.7134*tk1*(1-tklog) + -0.01709726125*tk2 + 
          1.982545316666667e-06*tk3 + -1.556712466666667e-10*tk4 + 
          5.462793899999999e-15*tk5 + (-6.57745821e+04 - tk1*-153.03586); 
      }
      else
      {
        cgspl[165] = -2.37786754*tk1*(1-tklog) + -0.062499637*tk2 + 
          1.635080563333333e-05*tk3 + -3.296444024999999e-09*tk4 + 
          3.240487264999999e-13*tk5 + (-5.33254847e+04 - tk1*44.6667297); 
      }
    }
    // Species ISO010
    {
      if (tk1 > 1.39e+03)
      {
        cgspl[166] = 32.8358296*tk1*(1-tklog) + -0.01663836425*tk2 + 
          1.93055475e-06*tk3 + -1.516321983333333e-10*tk4 + 
          5.321720949999999e-15*tk5 + (-4.52873003e+04 - tk1*-135.97571); 
      }
      else
      {
        cgspl[166] = 1.40912479*tk1*(1-tklog) + -0.055360417*tk2 + 
          1.451963458333333e-05*tk3 + -3.030522908333333e-09*tk4 + 
          3.141753444999999e-13*tk5 + (-3.46304788e+04 - tk1*31.6223383); 
      }
    }
    // Species ISO011
    {
      if (tk1 > 1.388e+03)
      {
        cgspl[167] = 32.0144349*tk1*(1-tklog) + -0.0170027998*tk2 + 
          1.9735976e-06*tk3 + -1.550502891666666e-10*tk4 + 
          5.442550599999999e-15*tk5 + (-4.27171859e+04 - tk1*-131.118243); 
      }
      else
      {
        cgspl[167] = 1.89718004*tk1*(1-tklog) + -0.053114898*tk2 + 
          1.337950363333334e-05*tk3 + -2.697075774999999e-09*tk4 + 
          2.72925236e-13*tk5 + (-3.22790229e+04 - tk1*30.2474612); 
      }
    }
    // Species ISO012
    {
      if (tk1 > 1.389e+03)
      {
        cgspl[168] = 27.4034778*tk1*(1-tklog) + -0.01637639465*tk2 + 
          1.883196166666667e-06*tk3 + -1.470372075e-10*tk4 + 
          5.139347249999999e-15*tk5 + (-2.48636651e+04 - tk1*-110.201978); 
      }
      else
      {
        cgspl[168] = 0.849786005*tk1*(1-tklog) + -0.0462553598*tk2 + 
          1.04226639e-05*tk3 + -1.815669916666666e-09*tk4 + 1.56360199e-13*tk5 + 
          (-1.53640679e+04 - tk1*33.2997487); 
      }
    }
    // Species ISO014
    {
      if (tk1 > 1.388e+03)
      {
        cgspl[169] = 30.5981614*tk1*(1-tklog) + -0.01486294275*tk2 + 
          1.699542966666667e-06*tk3 + -1.322770091666666e-10*tk4 + 
          4.614712084999998e-15*tk5 + (-6.0963892e+04 - tk1*-127.837876); 
      }
      else
      {
        cgspl[169] = -1.45570671*tk1*(1-tklog) + -0.054386192*tk2 + 
          1.432929043333333e-05*tk3 + -2.925794291666666e-09*tk4 + 
          2.917182494999999e-13*tk5 + (-5.0260203e+04 - tk1*42.8185431); 
      }
    }
    // Species N2
    {
      if (tk1 > 1.0e+03)
      {
        cgspl[170] = 2.92664*tk1*(1-tklog) + -7.439885e-04*tk2 + 
          9.474601666666666e-08*tk3 + -8.414199999999998e-12*tk4 + 
          3.376675499999999e-16*tk5 + (-922.7977 - tk1*5.980528); 
      }
      else
      {
        cgspl[170] = 3.298677*tk1*(1-tklog) + -7.0412e-04*tk2 + 6.60537e-07*tk3 
          + -4.7012625e-10*tk4 + 1.2224275e-13*tk5 + (-1.0209e+03 - 
          tk1*3.950372); 
      }
    }
  }
  
  double mole_frac[116];
  // Compute mole fractions
  {
    double sumyow = temperature * avmolwt * R0;
    sumyow = pressure/sumyow;
    mole_frac[0] = mass_frac[0] * recip_molecular_masses[0];
    mole_frac[0] = (mole_frac[0] > 1e-200) ? mole_frac[0] : 1e-200;
    mole_frac[0] *= sumyow;
    mole_frac[1] = mass_frac[1] * recip_molecular_masses[1];
    mole_frac[1] = (mole_frac[1] > 1e-200) ? mole_frac[1] : 1e-200;
    mole_frac[1] *= sumyow;
    mole_frac[2] = mass_frac[2] * recip_molecular_masses[2];
    mole_frac[2] = (mole_frac[2] > 1e-200) ? mole_frac[2] : 1e-200;
    mole_frac[2] *= sumyow;
    mole_frac[3] = mass_frac[3] * recip_molecular_masses[3];
    mole_frac[3] = (mole_frac[3] > 1e-200) ? mole_frac[3] : 1e-200;
    mole_frac[3] *= sumyow;
    mole_frac[4] = mass_frac[4] * recip_molecular_masses[4];
    mole_frac[4] = (mole_frac[4] > 1e-200) ? mole_frac[4] : 1e-200;
    mole_frac[4] *= sumyow;
    mole_frac[5] = mass_frac[5] * recip_molecular_masses[5];
    mole_frac[5] = (mole_frac[5] > 1e-200) ? mole_frac[5] : 1e-200;
    mole_frac[5] *= sumyow;
    mole_frac[6] = mass_frac[6] * recip_molecular_masses[6];
    mole_frac[6] = (mole_frac[6] > 1e-200) ? mole_frac[6] : 1e-200;
    mole_frac[6] *= sumyow;
    mole_frac[7] = mass_frac[7] * recip_molecular_masses[7];
    mole_frac[7] = (mole_frac[7] > 1e-200) ? mole_frac[7] : 1e-200;
    mole_frac[7] *= sumyow;
    mole_frac[8] = mass_frac[8] * recip_molecular_masses[8];
    mole_frac[8] = (mole_frac[8] > 1e-200) ? mole_frac[8] : 1e-200;
    mole_frac[8] *= sumyow;
    mole_frac[9] = mass_frac[9] * recip_molecular_masses[9];
    mole_frac[9] = (mole_frac[9] > 1e-200) ? mole_frac[9] : 1e-200;
    mole_frac[9] *= sumyow;
    mole_frac[10] = mass_frac[10] * recip_molecular_masses[10];
    mole_frac[10] = (mole_frac[10] > 1e-200) ? mole_frac[10] : 1e-200;
    mole_frac[10] *= sumyow;
    mole_frac[11] = mass_frac[11] * recip_molecular_masses[11];
    mole_frac[11] = (mole_frac[11] > 1e-200) ? mole_frac[11] : 1e-200;
    mole_frac[11] *= sumyow;
    mole_frac[12] = mass_frac[12] * recip_molecular_masses[12];
    mole_frac[12] = (mole_frac[12] > 1e-200) ? mole_frac[12] : 1e-200;
    mole_frac[12] *= sumyow;
    mole_frac[13] = mass_frac[13] * recip_molecular_masses[13];
    mole_frac[13] = (mole_frac[13] > 1e-200) ? mole_frac[13] : 1e-200;
    mole_frac[13] *= sumyow;
    mole_frac[14] = mass_frac[14] * recip_molecular_masses[14];
    mole_frac[14] = (mole_frac[14] > 1e-200) ? mole_frac[14] : 1e-200;
    mole_frac[14] *= sumyow;
    mole_frac[15] = mass_frac[15] * recip_molecular_masses[15];
    mole_frac[15] = (mole_frac[15] > 1e-200) ? mole_frac[15] : 1e-200;
    mole_frac[15] *= sumyow;
    mole_frac[16] = mass_frac[16] * recip_molecular_masses[16];
    mole_frac[16] = (mole_frac[16] > 1e-200) ? mole_frac[16] : 1e-200;
    mole_frac[16] *= sumyow;
    mole_frac[17] = mass_frac[17] * recip_molecular_masses[17];
    mole_frac[17] = (mole_frac[17] > 1e-200) ? mole_frac[17] : 1e-200;
    mole_frac[17] *= sumyow;
    mole_frac[18] = mass_frac[18] * recip_molecular_masses[18];
    mole_frac[18] = (mole_frac[18] > 1e-200) ? mole_frac[18] : 1e-200;
    mole_frac[18] *= sumyow;
    mole_frac[19] = mass_frac[19] * recip_molecular_masses[19];
    mole_frac[19] = (mole_frac[19] > 1e-200) ? mole_frac[19] : 1e-200;
    mole_frac[19] *= sumyow;
    mole_frac[20] = mass_frac[20] * recip_molecular_masses[20];
    mole_frac[20] = (mole_frac[20] > 1e-200) ? mole_frac[20] : 1e-200;
    mole_frac[20] *= sumyow;
    mole_frac[21] = mass_frac[21] * recip_molecular_masses[21];
    mole_frac[21] = (mole_frac[21] > 1e-200) ? mole_frac[21] : 1e-200;
    mole_frac[21] *= sumyow;
    mole_frac[22] = mass_frac[22] * recip_molecular_masses[22];
    mole_frac[22] = (mole_frac[22] > 1e-200) ? mole_frac[22] : 1e-200;
    mole_frac[22] *= sumyow;
    mole_frac[23] = mass_frac[23] * recip_molecular_masses[23];
    mole_frac[23] = (mole_frac[23] > 1e-200) ? mole_frac[23] : 1e-200;
    mole_frac[23] *= sumyow;
    mole_frac[24] = mass_frac[24] * recip_molecular_masses[24];
    mole_frac[24] = (mole_frac[24] > 1e-200) ? mole_frac[24] : 1e-200;
    mole_frac[24] *= sumyow;
    mole_frac[25] = mass_frac[25] * recip_molecular_masses[25];
    mole_frac[25] = (mole_frac[25] > 1e-200) ? mole_frac[25] : 1e-200;
    mole_frac[25] *= sumyow;
    mole_frac[26] = mass_frac[26] * recip_molecular_masses[26];
    mole_frac[26] = (mole_frac[26] > 1e-200) ? mole_frac[26] : 1e-200;
    mole_frac[26] *= sumyow;
    mole_frac[27] = mass_frac[27] * recip_molecular_masses[27];
    mole_frac[27] = (mole_frac[27] > 1e-200) ? mole_frac[27] : 1e-200;
    mole_frac[27] *= sumyow;
    mole_frac[28] = mass_frac[28] * recip_molecular_masses[28];
    mole_frac[28] = (mole_frac[28] > 1e-200) ? mole_frac[28] : 1e-200;
    mole_frac[28] *= sumyow;
    mole_frac[29] = mass_frac[29] * recip_molecular_masses[29];
    mole_frac[29] = (mole_frac[29] > 1e-200) ? mole_frac[29] : 1e-200;
    mole_frac[29] *= sumyow;
    mole_frac[30] = mass_frac[30] * recip_molecular_masses[30];
    mole_frac[30] = (mole_frac[30] > 1e-200) ? mole_frac[30] : 1e-200;
    mole_frac[30] *= sumyow;
    mole_frac[31] = mass_frac[31] * recip_molecular_masses[31];
    mole_frac[31] = (mole_frac[31] > 1e-200) ? mole_frac[31] : 1e-200;
    mole_frac[31] *= sumyow;
    mole_frac[32] = mass_frac[32] * recip_molecular_masses[32];
    mole_frac[32] = (mole_frac[32] > 1e-200) ? mole_frac[32] : 1e-200;
    mole_frac[32] *= sumyow;
    mole_frac[33] = mass_frac[33] * recip_molecular_masses[33];
    mole_frac[33] = (mole_frac[33] > 1e-200) ? mole_frac[33] : 1e-200;
    mole_frac[33] *= sumyow;
    mole_frac[34] = mass_frac[34] * recip_molecular_masses[34];
    mole_frac[34] = (mole_frac[34] > 1e-200) ? mole_frac[34] : 1e-200;
    mole_frac[34] *= sumyow;
    mole_frac[35] = mass_frac[35] * recip_molecular_masses[35];
    mole_frac[35] = (mole_frac[35] > 1e-200) ? mole_frac[35] : 1e-200;
    mole_frac[35] *= sumyow;
    mole_frac[36] = mass_frac[36] * recip_molecular_masses[36];
    mole_frac[36] = (mole_frac[36] > 1e-200) ? mole_frac[36] : 1e-200;
    mole_frac[36] *= sumyow;
    mole_frac[37] = mass_frac[37] * recip_molecular_masses[37];
    mole_frac[37] = (mole_frac[37] > 1e-200) ? mole_frac[37] : 1e-200;
    mole_frac[37] *= sumyow;
    mole_frac[38] = mass_frac[38] * recip_molecular_masses[38];
    mole_frac[38] = (mole_frac[38] > 1e-200) ? mole_frac[38] : 1e-200;
    mole_frac[38] *= sumyow;
    mole_frac[39] = mass_frac[39] * recip_molecular_masses[39];
    mole_frac[39] = (mole_frac[39] > 1e-200) ? mole_frac[39] : 1e-200;
    mole_frac[39] *= sumyow;
    mole_frac[40] = mass_frac[40] * recip_molecular_masses[40];
    mole_frac[40] = (mole_frac[40] > 1e-200) ? mole_frac[40] : 1e-200;
    mole_frac[40] *= sumyow;
    mole_frac[41] = mass_frac[41] * recip_molecular_masses[41];
    mole_frac[41] = (mole_frac[41] > 1e-200) ? mole_frac[41] : 1e-200;
    mole_frac[41] *= sumyow;
    mole_frac[42] = mass_frac[42] * recip_molecular_masses[42];
    mole_frac[42] = (mole_frac[42] > 1e-200) ? mole_frac[42] : 1e-200;
    mole_frac[42] *= sumyow;
    mole_frac[43] = mass_frac[43] * recip_molecular_masses[43];
    mole_frac[43] = (mole_frac[43] > 1e-200) ? mole_frac[43] : 1e-200;
    mole_frac[43] *= sumyow;
    mole_frac[44] = mass_frac[44] * recip_molecular_masses[44];
    mole_frac[44] = (mole_frac[44] > 1e-200) ? mole_frac[44] : 1e-200;
    mole_frac[44] *= sumyow;
    mole_frac[45] = mass_frac[45] * recip_molecular_masses[45];
    mole_frac[45] = (mole_frac[45] > 1e-200) ? mole_frac[45] : 1e-200;
    mole_frac[45] *= sumyow;
    mole_frac[46] = mass_frac[46] * recip_molecular_masses[46];
    mole_frac[46] = (mole_frac[46] > 1e-200) ? mole_frac[46] : 1e-200;
    mole_frac[46] *= sumyow;
    mole_frac[47] = mass_frac[47] * recip_molecular_masses[47];
    mole_frac[47] = (mole_frac[47] > 1e-200) ? mole_frac[47] : 1e-200;
    mole_frac[47] *= sumyow;
    mole_frac[48] = mass_frac[48] * recip_molecular_masses[48];
    mole_frac[48] = (mole_frac[48] > 1e-200) ? mole_frac[48] : 1e-200;
    mole_frac[48] *= sumyow;
    mole_frac[49] = mass_frac[49] * recip_molecular_masses[49];
    mole_frac[49] = (mole_frac[49] > 1e-200) ? mole_frac[49] : 1e-200;
    mole_frac[49] *= sumyow;
    mole_frac[50] = mass_frac[50] * recip_molecular_masses[50];
    mole_frac[50] = (mole_frac[50] > 1e-200) ? mole_frac[50] : 1e-200;
    mole_frac[50] *= sumyow;
    mole_frac[51] = mass_frac[51] * recip_molecular_masses[51];
    mole_frac[51] = (mole_frac[51] > 1e-200) ? mole_frac[51] : 1e-200;
    mole_frac[51] *= sumyow;
    mole_frac[52] = mass_frac[52] * recip_molecular_masses[52];
    mole_frac[52] = (mole_frac[52] > 1e-200) ? mole_frac[52] : 1e-200;
    mole_frac[52] *= sumyow;
    mole_frac[53] = mass_frac[53] * recip_molecular_masses[53];
    mole_frac[53] = (mole_frac[53] > 1e-200) ? mole_frac[53] : 1e-200;
    mole_frac[53] *= sumyow;
    mole_frac[54] = mass_frac[54] * recip_molecular_masses[54];
    mole_frac[54] = (mole_frac[54] > 1e-200) ? mole_frac[54] : 1e-200;
    mole_frac[54] *= sumyow;
    mole_frac[55] = mass_frac[55] * recip_molecular_masses[55];
    mole_frac[55] = (mole_frac[55] > 1e-200) ? mole_frac[55] : 1e-200;
    mole_frac[55] *= sumyow;
    mole_frac[56] = mass_frac[56] * recip_molecular_masses[56];
    mole_frac[56] = (mole_frac[56] > 1e-200) ? mole_frac[56] : 1e-200;
    mole_frac[56] *= sumyow;
    mole_frac[57] = mass_frac[57] * recip_molecular_masses[57];
    mole_frac[57] = (mole_frac[57] > 1e-200) ? mole_frac[57] : 1e-200;
    mole_frac[57] *= sumyow;
    mole_frac[58] = mass_frac[58] * recip_molecular_masses[58];
    mole_frac[58] = (mole_frac[58] > 1e-200) ? mole_frac[58] : 1e-200;
    mole_frac[58] *= sumyow;
    mole_frac[59] = mass_frac[59] * recip_molecular_masses[59];
    mole_frac[59] = (mole_frac[59] > 1e-200) ? mole_frac[59] : 1e-200;
    mole_frac[59] *= sumyow;
    mole_frac[60] = mass_frac[60] * recip_molecular_masses[60];
    mole_frac[60] = (mole_frac[60] > 1e-200) ? mole_frac[60] : 1e-200;
    mole_frac[60] *= sumyow;
    mole_frac[61] = mass_frac[61] * recip_molecular_masses[61];
    mole_frac[61] = (mole_frac[61] > 1e-200) ? mole_frac[61] : 1e-200;
    mole_frac[61] *= sumyow;
    mole_frac[62] = mass_frac[62] * recip_molecular_masses[62];
    mole_frac[62] = (mole_frac[62] > 1e-200) ? mole_frac[62] : 1e-200;
    mole_frac[62] *= sumyow;
    mole_frac[63] = mass_frac[63] * recip_molecular_masses[63];
    mole_frac[63] = (mole_frac[63] > 1e-200) ? mole_frac[63] : 1e-200;
    mole_frac[63] *= sumyow;
    mole_frac[64] = mass_frac[64] * recip_molecular_masses[64];
    mole_frac[64] = (mole_frac[64] > 1e-200) ? mole_frac[64] : 1e-200;
    mole_frac[64] *= sumyow;
    mole_frac[65] = mass_frac[65] * recip_molecular_masses[65];
    mole_frac[65] = (mole_frac[65] > 1e-200) ? mole_frac[65] : 1e-200;
    mole_frac[65] *= sumyow;
    mole_frac[66] = mass_frac[66] * recip_molecular_masses[66];
    mole_frac[66] = (mole_frac[66] > 1e-200) ? mole_frac[66] : 1e-200;
    mole_frac[66] *= sumyow;
    mole_frac[67] = mass_frac[67] * recip_molecular_masses[67];
    mole_frac[67] = (mole_frac[67] > 1e-200) ? mole_frac[67] : 1e-200;
    mole_frac[67] *= sumyow;
    mole_frac[68] = mass_frac[68] * recip_molecular_masses[68];
    mole_frac[68] = (mole_frac[68] > 1e-200) ? mole_frac[68] : 1e-200;
    mole_frac[68] *= sumyow;
    mole_frac[69] = mass_frac[69] * recip_molecular_masses[69];
    mole_frac[69] = (mole_frac[69] > 1e-200) ? mole_frac[69] : 1e-200;
    mole_frac[69] *= sumyow;
    mole_frac[70] = mass_frac[70] * recip_molecular_masses[70];
    mole_frac[70] = (mole_frac[70] > 1e-200) ? mole_frac[70] : 1e-200;
    mole_frac[70] *= sumyow;
    mole_frac[71] = mass_frac[71] * recip_molecular_masses[71];
    mole_frac[71] = (mole_frac[71] > 1e-200) ? mole_frac[71] : 1e-200;
    mole_frac[71] *= sumyow;
    mole_frac[72] = mass_frac[72] * recip_molecular_masses[72];
    mole_frac[72] = (mole_frac[72] > 1e-200) ? mole_frac[72] : 1e-200;
    mole_frac[72] *= sumyow;
    mole_frac[73] = mass_frac[73] * recip_molecular_masses[73];
    mole_frac[73] = (mole_frac[73] > 1e-200) ? mole_frac[73] : 1e-200;
    mole_frac[73] *= sumyow;
    mole_frac[74] = mass_frac[74] * recip_molecular_masses[74];
    mole_frac[74] = (mole_frac[74] > 1e-200) ? mole_frac[74] : 1e-200;
    mole_frac[74] *= sumyow;
    mole_frac[75] = mass_frac[75] * recip_molecular_masses[75];
    mole_frac[75] = (mole_frac[75] > 1e-200) ? mole_frac[75] : 1e-200;
    mole_frac[75] *= sumyow;
    mole_frac[76] = mass_frac[76] * recip_molecular_masses[76];
    mole_frac[76] = (mole_frac[76] > 1e-200) ? mole_frac[76] : 1e-200;
    mole_frac[76] *= sumyow;
    mole_frac[77] = mass_frac[77] * recip_molecular_masses[77];
    mole_frac[77] = (mole_frac[77] > 1e-200) ? mole_frac[77] : 1e-200;
    mole_frac[77] *= sumyow;
    mole_frac[78] = mass_frac[78] * recip_molecular_masses[78];
    mole_frac[78] = (mole_frac[78] > 1e-200) ? mole_frac[78] : 1e-200;
    mole_frac[78] *= sumyow;
    mole_frac[79] = mass_frac[79] * recip_molecular_masses[79];
    mole_frac[79] = (mole_frac[79] > 1e-200) ? mole_frac[79] : 1e-200;
    mole_frac[79] *= sumyow;
    mole_frac[80] = mass_frac[80] * recip_molecular_masses[80];
    mole_frac[80] = (mole_frac[80] > 1e-200) ? mole_frac[80] : 1e-200;
    mole_frac[80] *= sumyow;
    mole_frac[81] = mass_frac[81] * recip_molecular_masses[81];
    mole_frac[81] = (mole_frac[81] > 1e-200) ? mole_frac[81] : 1e-200;
    mole_frac[81] *= sumyow;
    mole_frac[82] = mass_frac[82] * recip_molecular_masses[82];
    mole_frac[82] = (mole_frac[82] > 1e-200) ? mole_frac[82] : 1e-200;
    mole_frac[82] *= sumyow;
    mole_frac[83] = mass_frac[83] * recip_molecular_masses[83];
    mole_frac[83] = (mole_frac[83] > 1e-200) ? mole_frac[83] : 1e-200;
    mole_frac[83] *= sumyow;
    mole_frac[84] = mass_frac[84] * recip_molecular_masses[84];
    mole_frac[84] = (mole_frac[84] > 1e-200) ? mole_frac[84] : 1e-200;
    mole_frac[84] *= sumyow;
    mole_frac[85] = mass_frac[85] * recip_molecular_masses[85];
    mole_frac[85] = (mole_frac[85] > 1e-200) ? mole_frac[85] : 1e-200;
    mole_frac[85] *= sumyow;
    mole_frac[86] = mass_frac[86] * recip_molecular_masses[86];
    mole_frac[86] = (mole_frac[86] > 1e-200) ? mole_frac[86] : 1e-200;
    mole_frac[86] *= sumyow;
    mole_frac[87] = mass_frac[87] * recip_molecular_masses[87];
    mole_frac[87] = (mole_frac[87] > 1e-200) ? mole_frac[87] : 1e-200;
    mole_frac[87] *= sumyow;
    mole_frac[88] = mass_frac[88] * recip_molecular_masses[88];
    mole_frac[88] = (mole_frac[88] > 1e-200) ? mole_frac[88] : 1e-200;
    mole_frac[88] *= sumyow;
    mole_frac[89] = mass_frac[89] * recip_molecular_masses[89];
    mole_frac[89] = (mole_frac[89] > 1e-200) ? mole_frac[89] : 1e-200;
    mole_frac[89] *= sumyow;
    mole_frac[90] = mass_frac[90] * recip_molecular_masses[90];
    mole_frac[90] = (mole_frac[90] > 1e-200) ? mole_frac[90] : 1e-200;
    mole_frac[90] *= sumyow;
    mole_frac[91] = mass_frac[91] * recip_molecular_masses[91];
    mole_frac[91] = (mole_frac[91] > 1e-200) ? mole_frac[91] : 1e-200;
    mole_frac[91] *= sumyow;
    mole_frac[92] = mass_frac[92] * recip_molecular_masses[92];
    mole_frac[92] = (mole_frac[92] > 1e-200) ? mole_frac[92] : 1e-200;
    mole_frac[92] *= sumyow;
    mole_frac[93] = mass_frac[93] * recip_molecular_masses[93];
    mole_frac[93] = (mole_frac[93] > 1e-200) ? mole_frac[93] : 1e-200;
    mole_frac[93] *= sumyow;
    mole_frac[94] = mass_frac[94] * recip_molecular_masses[94];
    mole_frac[94] = (mole_frac[94] > 1e-200) ? mole_frac[94] : 1e-200;
    mole_frac[94] *= sumyow;
    mole_frac[95] = mass_frac[95] * recip_molecular_masses[95];
    mole_frac[95] = (mole_frac[95] > 1e-200) ? mole_frac[95] : 1e-200;
    mole_frac[95] *= sumyow;
    mole_frac[96] = mass_frac[96] * recip_molecular_masses[96];
    mole_frac[96] = (mole_frac[96] > 1e-200) ? mole_frac[96] : 1e-200;
    mole_frac[96] *= sumyow;
    mole_frac[97] = mass_frac[97] * recip_molecular_masses[97];
    mole_frac[97] = (mole_frac[97] > 1e-200) ? mole_frac[97] : 1e-200;
    mole_frac[97] *= sumyow;
    mole_frac[98] = mass_frac[98] * recip_molecular_masses[98];
    mole_frac[98] = (mole_frac[98] > 1e-200) ? mole_frac[98] : 1e-200;
    mole_frac[98] *= sumyow;
    mole_frac[99] = mass_frac[99] * recip_molecular_masses[99];
    mole_frac[99] = (mole_frac[99] > 1e-200) ? mole_frac[99] : 1e-200;
    mole_frac[99] *= sumyow;
    mole_frac[100] = mass_frac[100] * recip_molecular_masses[100];
    mole_frac[100] = (mole_frac[100] > 1e-200) ? mole_frac[100] : 1e-200;
    mole_frac[100] *= sumyow;
    mole_frac[101] = mass_frac[101] * recip_molecular_masses[101];
    mole_frac[101] = (mole_frac[101] > 1e-200) ? mole_frac[101] : 1e-200;
    mole_frac[101] *= sumyow;
    mole_frac[102] = mass_frac[102] * recip_molecular_masses[102];
    mole_frac[102] = (mole_frac[102] > 1e-200) ? mole_frac[102] : 1e-200;
    mole_frac[102] *= sumyow;
    mole_frac[103] = mass_frac[103] * recip_molecular_masses[103];
    mole_frac[103] = (mole_frac[103] > 1e-200) ? mole_frac[103] : 1e-200;
    mole_frac[103] *= sumyow;
    mole_frac[104] = mass_frac[104] * recip_molecular_masses[104];
    mole_frac[104] = (mole_frac[104] > 1e-200) ? mole_frac[104] : 1e-200;
    mole_frac[104] *= sumyow;
    mole_frac[105] = mass_frac[105] * recip_molecular_masses[105];
    mole_frac[105] = (mole_frac[105] > 1e-200) ? mole_frac[105] : 1e-200;
    mole_frac[105] *= sumyow;
    mole_frac[106] = mass_frac[106] * recip_molecular_masses[106];
    mole_frac[106] = (mole_frac[106] > 1e-200) ? mole_frac[106] : 1e-200;
    mole_frac[106] *= sumyow;
    mole_frac[107] = mass_frac[107] * recip_molecular_masses[107];
    mole_frac[107] = (mole_frac[107] > 1e-200) ? mole_frac[107] : 1e-200;
    mole_frac[107] *= sumyow;
    mole_frac[108] = mass_frac[108] * recip_molecular_masses[108];
    mole_frac[108] = (mole_frac[108] > 1e-200) ? mole_frac[108] : 1e-200;
    mole_frac[108] *= sumyow;
    mole_frac[109] = mass_frac[109] * recip_molecular_masses[109];
    mole_frac[109] = (mole_frac[109] > 1e-200) ? mole_frac[109] : 1e-200;
    mole_frac[109] *= sumyow;
    mole_frac[110] = mass_frac[110] * recip_molecular_masses[110];
    mole_frac[110] = (mole_frac[110] > 1e-200) ? mole_frac[110] : 1e-200;
    mole_frac[110] *= sumyow;
    mole_frac[111] = mass_frac[111] * recip_molecular_masses[111];
    mole_frac[111] = (mole_frac[111] > 1e-200) ? mole_frac[111] : 1e-200;
    mole_frac[111] *= sumyow;
    mole_frac[112] = mass_frac[112] * recip_molecular_masses[112];
    mole_frac[112] = (mole_frac[112] > 1e-200) ? mole_frac[112] : 1e-200;
    mole_frac[112] *= sumyow;
    mole_frac[113] = mass_frac[113] * recip_molecular_masses[113];
    mole_frac[113] = (mole_frac[113] > 1e-200) ? mole_frac[113] : 1e-200;
    mole_frac[113] *= sumyow;
    mole_frac[114] = mass_frac[114] * recip_molecular_masses[114];
    mole_frac[114] = (mole_frac[114] > 1e-200) ? mole_frac[114] : 1e-200;
    mole_frac[114] *= sumyow;
    mole_frac[115] = mass_frac[115] * recip_molecular_masses[115];
    mole_frac[115] = (mole_frac[115] > 1e-200) ? mole_frac[115] : 1e-200;
    mole_frac[115] *= sumyow;
  }
  
  double thbctemp[9];
  // Computing third body values
  {
    double ctot = 0.0;
    ctot += mole_frac[0];
    ctot += mole_frac[1];
    ctot += mole_frac[2];
    ctot += mole_frac[3];
    ctot += mole_frac[4];
    ctot += mole_frac[5];
    ctot += mole_frac[6];
    ctot += mole_frac[7];
    ctot += mole_frac[8];
    ctot += mole_frac[9];
    ctot += mole_frac[10];
    ctot += mole_frac[11];
    ctot += mole_frac[12];
    ctot += mole_frac[13];
    ctot += mole_frac[14];
    ctot += mole_frac[15];
    ctot += mole_frac[16];
    ctot += mole_frac[17];
    ctot += mole_frac[18];
    ctot += mole_frac[19];
    ctot += mole_frac[20];
    ctot += mole_frac[21];
    ctot += mole_frac[22];
    ctot += mole_frac[23];
    ctot += mole_frac[24];
    ctot += mole_frac[25];
    ctot += mole_frac[26];
    ctot += mole_frac[27];
    ctot += mole_frac[28];
    ctot += mole_frac[29];
    ctot += mole_frac[30];
    ctot += mole_frac[31];
    ctot += mole_frac[32];
    ctot += mole_frac[33];
    ctot += mole_frac[34];
    ctot += mole_frac[35];
    ctot += mole_frac[36];
    ctot += mole_frac[37];
    ctot += mole_frac[38];
    ctot += mole_frac[39];
    ctot += mole_frac[40];
    ctot += mole_frac[41];
    ctot += mole_frac[42];
    ctot += mole_frac[43];
    ctot += mole_frac[44];
    ctot += mole_frac[45];
    ctot += mole_frac[46];
    ctot += mole_frac[47];
    ctot += mole_frac[48];
    ctot += mole_frac[49];
    ctot += mole_frac[50];
    ctot += mole_frac[51];
    ctot += mole_frac[52];
    ctot += mole_frac[53];
    ctot += mole_frac[54];
    ctot += mole_frac[55];
    ctot += mole_frac[56];
    ctot += mole_frac[57];
    ctot += mole_frac[58];
    ctot += mole_frac[59];
    ctot += mole_frac[60];
    ctot += mole_frac[61];
    ctot += mole_frac[62];
    ctot += mole_frac[63];
    ctot += mole_frac[64];
    ctot += mole_frac[65];
    ctot += mole_frac[66];
    ctot += mole_frac[67];
    ctot += mole_frac[68];
    ctot += mole_frac[69];
    ctot += mole_frac[70];
    ctot += mole_frac[71];
    ctot += mole_frac[72];
    ctot += mole_frac[73];
    ctot += mole_frac[74];
    ctot += mole_frac[75];
    ctot += mole_frac[76];
    ctot += mole_frac[77];
    ctot += mole_frac[78];
    ctot += mole_frac[79];
    ctot += mole_frac[80];
    ctot += mole_frac[81];
    ctot += mole_frac[82];
    ctot += mole_frac[83];
    ctot += mole_frac[84];
    ctot += mole_frac[85];
    ctot += mole_frac[86];
    ctot += mole_frac[87];
    ctot += mole_frac[88];
    ctot += mole_frac[89];
    ctot += mole_frac[90];
    ctot += mole_frac[91];
    ctot += mole_frac[92];
    ctot += mole_frac[93];
    ctot += mole_frac[94];
    ctot += mole_frac[95];
    ctot += mole_frac[96];
    ctot += mole_frac[97];
    ctot += mole_frac[98];
    ctot += mole_frac[99];
    ctot += mole_frac[100];
    ctot += mole_frac[101];
    ctot += mole_frac[102];
    ctot += mole_frac[103];
    ctot += mole_frac[104];
    ctot += mole_frac[105];
    ctot += mole_frac[106];
    ctot += mole_frac[107];
    ctot += mole_frac[108];
    ctot += mole_frac[109];
    ctot += mole_frac[110];
    ctot += mole_frac[111];
    ctot += mole_frac[112];
    ctot += mole_frac[113];
    ctot += mole_frac[114];
    ctot += mole_frac[115];
    thbctemp[0] = ctot;
    thbctemp[1] = ctot + 1.5*mole_frac[1] + 11.0*mole_frac[5] + 
      0.8999999999999999*mole_frac[8] + 2.8*mole_frac[9]; 
    thbctemp[2] = ctot + 1.5*mole_frac[1] + 11.0*mole_frac[5] + 
      0.8999999999999999*mole_frac[8] + 2.8*mole_frac[9] + mole_frac[16] + 
      2.0*mole_frac[18]; 
    thbctemp[3] = ctot + 1.5*mole_frac[1] + 11.0*mole_frac[5] + 0.5*mole_frac[8] 
      + mole_frac[9] + mole_frac[16] + 2.0*mole_frac[18]; 
    thbctemp[4] = ctot - 0.27*mole_frac[1] + 11.0*mole_frac[5] + mole_frac[16] + 
      2.0*mole_frac[18]; 
    thbctemp[5] = ctot + 0.3*mole_frac[1] + 13.0*mole_frac[5] + 
      0.8999999999999999*mole_frac[8] + 2.8*mole_frac[9] + mole_frac[16] + 
      2.0*mole_frac[18]; 
    thbctemp[6] = ctot + mole_frac[1] + 5.0*mole_frac[3] + 5.0*mole_frac[5] + 
      0.5*mole_frac[8] + 2.5*mole_frac[9] + mole_frac[16] + 2.0*mole_frac[18]; 
    thbctemp[7] = ctot + mole_frac[1] + 11.0*mole_frac[5] + 0.5*mole_frac[8] + 
      mole_frac[9] + mole_frac[16] + 2.0*mole_frac[18]; 
    thbctemp[8] = ctot + mole_frac[1] + 5.0*mole_frac[5] + 0.5*mole_frac[8] + 
      mole_frac[9] + mole_frac[16] + 2.0*mole_frac[18]; 
  }
  
  double rr_f[861];
  double rr_r[861];
  //   0)  H + O2 <=> O + OH
  {
    double forward = 3.547e+15 * exp(-0.406*vlntemp - 1.66e+04*ortc);
    double reverse = 1.027e+13 * exp(-0.015*vlntemp + 133.0*ortc);
    rr_f[0] = forward * mole_frac[0] * mole_frac[3];
    rr_r[0] = reverse * mole_frac[2] * mole_frac[4];
  }
  //   1)  H2 + O <=> H + OH
  {
    double forward = 5.08e+04 * exp(2.67*vlntemp - 6.292e+03*ortc);
    double reverse = 2.637e+04 * exp(2.651*vlntemp - 4.88e+03*ortc);
    rr_f[1] = forward * mole_frac[1] * mole_frac[2];
    rr_r[1] = reverse * mole_frac[0] * mole_frac[4];
  }
  //   2)  H2 + OH <=> H + H2O
  {
    double forward = 2.16e+08 * exp(1.51*vlntemp - 3.43e+03*ortc);
    double reverse = 2.29e+09 * exp(1.404*vlntemp - 1.832e+04*ortc);
    rr_f[2] = forward * mole_frac[1] * mole_frac[4];
    rr_r[2] = reverse * mole_frac[0] * mole_frac[5];
  }
  //   3)  O + H2O <=> 2 OH
  {
    double forward = 2.97e+06 * exp(2.02*vlntemp - 1.34e+04*ortc);
    double reverse = 1.454e+05 * exp(2.107*vlntemp + 2.904e+03*ortc);
    rr_f[3] = forward * mole_frac[2] * mole_frac[5];
    rr_r[3] = reverse * mole_frac[4] * mole_frac[4];
  }
  //   4)  H2 + M <=> 2 H + M
  {
    double forward = 4.577e+19 * exp(-1.4*vlntemp - 1.044e+05*ortc);
    double reverse = 1.145e+20 * exp(-1.676*vlntemp - 820.0*ortc);
    rr_f[4] = forward * mole_frac[1];
    rr_r[4] = reverse * mole_frac[0] * mole_frac[0];
    rr_f[4] *= thbctemp[1];
    rr_r[4] *= thbctemp[1];
  }
  //   5)  O2 + M <=> 2 O + M
  {
    double forward = 4.42e+17 * exp(-0.634*vlntemp - 1.189e+05*ortc);
    double reverse = 6.165e+15 * exp(-0.5 * vlntemp);
    rr_f[5] = forward * mole_frac[3];
    rr_r[5] = reverse * mole_frac[2] * mole_frac[2];
    rr_f[5] *= thbctemp[2];
    rr_r[5] *= thbctemp[2];
  }
  //   6)  OH + M <=> H + O + M
  {
    double forward = 9.78e+17 * exp(-0.743*vlntemp - 1.021e+05*ortc);
    double reverse = 4.714e+18 * otc;
    rr_f[6] = forward * mole_frac[4];
    rr_r[6] = reverse * mole_frac[0] * mole_frac[2];
    rr_f[6] *= thbctemp[3];
    rr_r[6] *= thbctemp[3];
  }
  //   7)  H2O + M <=> H + OH + M
  {
    double forward = 1.907e+23 * exp(-1.83*vlntemp - 1.185e+05*ortc);
    double reverse = 4.5e+22 * otc * otc;
    rr_f[7] = forward * mole_frac[5];
    rr_r[7] = reverse * mole_frac[0] * mole_frac[4];
    rr_f[7] *= thbctemp[4];
    rr_r[7] *= thbctemp[4];
  }
  //   8)  H + O2 (+M) <=> HO2 (+M)
  {
    double rr_k0 = 3.482e+16 * exp(-0.411*vlntemp + 1.115e+03*ortc);
    double rr_kinf = 1.475e+12 * exp(0.6 * vlntemp);
    double pr = rr_k0 / rr_kinf * thbctemp[5];
    double fcent = log10(MAX(0.5 * exp(-9.999999999999999e+29 * temperature) + 
      0.5 * exp(-9.999999999999999e-31 * temperature) + exp(-1.0e+10 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = -cgspl[0] - cgspl[3] + cgspl[6];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[8] = forward * mole_frac[0] * mole_frac[3];
    rr_r[8] = reverse * mole_frac[6];
  }
  //   9)  H + HO2 <=> H2 + O2
  {
    double forward = 1.66e+13 * exp(-823.0*ortc);
    double reverse = 3.166e+12 * exp(0.348*vlntemp - 5.551e+04*ortc);
    rr_f[9] = forward * mole_frac[0] * mole_frac[6];
    rr_r[9] = reverse * mole_frac[1] * mole_frac[3];
  }
  //  10)  H + HO2 <=> 2 OH
  {
    double forward = 7.079e+13 * exp(-295.0*ortc);
    double reverse = 2.028e+10 * exp(0.72*vlntemp - 3.684e+04*ortc);
    rr_f[10] = forward * mole_frac[0] * mole_frac[6];
    rr_r[10] = reverse * mole_frac[4] * mole_frac[4];
  }
  //  11)  O + HO2 <=> O2 + OH
  {
    double forward = 3.25e+13;
    double reverse = 3.217e+12 * exp(0.329*vlntemp - 5.328e+04*ortc);
    rr_f[11] = forward * mole_frac[2] * mole_frac[6];
    rr_r[11] = reverse * mole_frac[3] * mole_frac[4];
  }
  //  12)  OH + HO2 <=> O2 + H2O
  {
    double forward = 1.973e+10 * exp(0.962*vlntemp + 328.4*ortc);
    double reverse = 3.989e+10 * exp(1.204*vlntemp - 6.925e+04*ortc);
    rr_f[12] = forward * mole_frac[4] * mole_frac[6];
    rr_r[12] = reverse * mole_frac[3] * mole_frac[5];
  }
  //  13)  O2 + H2O2 <=> 2 HO2
  {
    double forward = 1.136e+16 * exp(-0.347*vlntemp - 4.973e+04*ortc);
    double reverse = 1.03e+14 * exp(-1.104e+04*ortc);
    rr_f[13] = forward * mole_frac[3] * mole_frac[7];
    rr_r[13] = reverse * mole_frac[6] * mole_frac[6];
  }
  //  14)  O2 + H2O2 <=> 2 HO2
  {
    double forward = 2.141e+13 * exp(-0.347*vlntemp - 3.728e+04*ortc);
    double reverse = 1.94e+11 * exp(1.409e+03*ortc);
    rr_f[14] = forward * mole_frac[3] * mole_frac[7];
    rr_r[14] = reverse * mole_frac[6] * mole_frac[6];
  }
  //  15)  H2O2 (+M) <=> 2 OH (+M)
  {
    double rr_k0 = 1.202e+17 * exp(-4.55e+04*ortc);
    double rr_kinf = 2.951e+14 * exp(-4.843e+04*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[2];
    double fcent = log10(MAX(0.5 * exp(-9.999999999999999e+29 * temperature) + 
      0.5 * exp(-9.999999999999999e-31 * temperature) + exp(-1.0e+10 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = 2.0 * cgspl[4] - cgspl[7];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[15] = forward * mole_frac[7];
    rr_r[15] = reverse * mole_frac[4] * mole_frac[4];
  }
  //  16)  H + H2O2 <=> OH + H2O
  {
    double forward = 2.41e+13 * exp(-3.97e+03*ortc);
    double reverse = 1.265e+08 * exp(1.31*vlntemp - 7.141e+04*ortc);
    rr_f[16] = forward * mole_frac[0] * mole_frac[7];
    rr_r[16] = reverse * mole_frac[4] * mole_frac[5];
  }
  //  17)  H + H2O2 <=> H2 + HO2
  {
    double forward = 2.15e+10 * temperature * exp(-6.0e+03*ortc);
    double reverse = 3.716e+07 * exp(1.695*vlntemp - 2.2e+04*ortc);
    rr_f[17] = forward * mole_frac[0] * mole_frac[7];
    rr_r[17] = reverse * mole_frac[1] * mole_frac[6];
  }
  //  18)  O + H2O2 <=> OH + HO2
  {
    double forward = 9.55e+06 * temperature * temperature * exp(-3.97e+03*ortc);
    double reverse = 8.568e+03 * exp(2.676*vlntemp - 1.856e+04*ortc);
    rr_f[18] = forward * mole_frac[2] * mole_frac[7];
    rr_r[18] = reverse * mole_frac[4] * mole_frac[6];
  }
  //  19)  OH + H2O2 <=> H2O + HO2
  {
    double forward = 2.0e+12 * exp(-427.2*ortc);
    double reverse = 3.665e+10 * exp(0.589*vlntemp - 3.132e+04*ortc);
    rr_f[19] = forward * mole_frac[4] * mole_frac[7];
    rr_r[19] = reverse * mole_frac[5] * mole_frac[6];
  }
  //  20)  OH + H2O2 <=> H2O + HO2
  {
    double forward = 1.7e+18 * exp(-2.941e+04*ortc);
    double reverse = 3.115e+16 * exp(0.589*vlntemp - 6.03e+04*ortc);
    rr_f[20] = forward * mole_frac[4] * mole_frac[7];
    rr_r[20] = reverse * mole_frac[5] * mole_frac[6];
  }
  //  21)  O + CO (+M) <=> CO2 (+M)
  {
    double rr_k0 = 1.35e+24 * exp(-2.788*vlntemp - 4.191e+03*ortc);
    double rr_kinf = 1.8e+10 * exp(-2.384e+03*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[6];
    double forward = rr_kinf * pr/(1.0 + pr);
    double xik = -cgspl[2] - cgspl[8] + cgspl[9];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[21] = forward * mole_frac[2] * mole_frac[8];
    rr_r[21] = reverse * mole_frac[9];
  }
  //  22)  O2 + CO <=> O + CO2
  {
    double forward = 1.05e+12 * exp(-4.254e+04*ortc);
    double reverse = 8.035e+15 * exp(-0.8*vlntemp - 5.123e+04*ortc);
    rr_f[22] = forward * mole_frac[3] * mole_frac[8];
    rr_r[22] = reverse * mole_frac[2] * mole_frac[9];
  }
  //  23)  OH + CO <=> H + CO2
  {
    double forward = 2.23e+05 * exp(1.89*vlntemp + 1.158e+03*ortc);
    double reverse = 5.896e+11 * exp(0.699*vlntemp - 2.426e+04*ortc);
    rr_f[23] = forward * mole_frac[4] * mole_frac[8];
    rr_r[23] = reverse * mole_frac[0] * mole_frac[9];
  }
  //  24)  HO2 + CO <=> OH + CO2
  {
    double forward = 3.01e+13 * exp(-2.3e+04*ortc);
    double reverse = 2.28e+16 * exp(-0.47*vlntemp - 8.497e+04*ortc);
    rr_f[24] = forward * mole_frac[6] * mole_frac[8];
    rr_r[24] = reverse * mole_frac[4] * mole_frac[9];
  }
  //  25)  HCO + M <=> H + CO + M
  {
    double forward = 4.75e+11 * exp(0.66*vlntemp - 1.487e+04*ortc);
    double reverse = 3.582e+10 * exp(1.041*vlntemp + 457.3*ortc);
    rr_f[25] = forward * mole_frac[11];
    rr_r[25] = reverse * mole_frac[0] * mole_frac[8];
    rr_f[25] *= thbctemp[7];
    rr_r[25] *= thbctemp[7];
  }
  //  26)  O2 + HCO <=> HO2 + CO
  {
    double forward = 7.58e+12 * exp(-410.0*ortc);
    double reverse = 1.198e+12 * exp(0.309*vlntemp - 3.395e+04*ortc);
    rr_f[26] = forward * mole_frac[3] * mole_frac[11];
    rr_r[26] = reverse * mole_frac[6] * mole_frac[8];
  }
  //  27)  H + HCO <=> H2 + CO
  {
    double forward = 7.34e+13;
    double reverse = 2.212e+12 * exp(0.656*vlntemp - 8.823e+04*ortc);
    rr_f[27] = forward * mole_frac[0] * mole_frac[11];
    rr_r[27] = reverse * mole_frac[1] * mole_frac[8];
  }
  //  28)  O + HCO <=> OH + CO
  {
    double forward = 3.02e+13;
    double reverse = 4.725e+11 * exp(0.638*vlntemp - 8.682e+04*ortc);
    rr_f[28] = forward * mole_frac[2] * mole_frac[11];
    rr_r[28] = reverse * mole_frac[4] * mole_frac[8];
  }
  //  29)  O + HCO <=> H + CO2
  {
    double forward = 3.0e+13;
    double reverse = 1.241e+18 * exp(-0.553*vlntemp - 1.122e+05*ortc);
    rr_f[29] = forward * mole_frac[2] * mole_frac[11];
    rr_r[29] = reverse * mole_frac[0] * mole_frac[9];
  }
  //  30)  OH + HCO <=> H2O + CO
  {
    double forward = 1.02e+14;
    double reverse = 3.259e+13 * exp(0.551*vlntemp - 1.031e+05*ortc);
    rr_f[30] = forward * mole_frac[4] * mole_frac[11];
    rr_r[30] = reverse * mole_frac[5] * mole_frac[8];
  }
  //  31)  HCO + CH3 <=> CO + CH4
  {
    double forward = 2.65e+13;
    double reverse = 7.286e+14 * exp(0.211*vlntemp - 8.977e+04*ortc);
    rr_f[31] = forward * mole_frac[11] * mole_frac[17];
    rr_r[31] = reverse * mole_frac[8] * mole_frac[16];
  }
  //  32)  HO2 + HCO <=> O2 + CH2O
  {
    double forward = 2.499e+14 * exp(-0.061*vlntemp - 1.392e+04*ortc);
    double reverse = 8.07e+15 * exp(-5.342e+04*ortc);
    rr_f[32] = forward * mole_frac[6] * mole_frac[11];
    rr_r[32] = reverse * mole_frac[3] * mole_frac[10];
  }
  //  33)  HO2 + HCO <=> H + OH + CO2
  {
    double forward = 3.0e+13;
    double reverse = 0.0;
    rr_f[33] = forward * mole_frac[6] * mole_frac[11];
    rr_r[33] = reverse * mole_frac[0] * mole_frac[4] * mole_frac[9];
  }
  //  34)  CO + CH2O <=> 2 HCO
  {
    double forward = 9.186e+13 * exp(0.37*vlntemp - 7.304e+04*ortc);
    double reverse = 1.8e+13;
    rr_f[34] = forward * mole_frac[8] * mole_frac[10];
    rr_r[34] = reverse * mole_frac[11] * mole_frac[11];
  }
  //  35)  2 HCO <=> H2 + 2 CO
  {
    double forward = 3.0e+12;
    double reverse = 0.0;
    rr_f[35] = forward * mole_frac[11] * mole_frac[11];
    rr_r[35] = reverse * mole_frac[1] * mole_frac[8] * mole_frac[8];
  }
  //  36)  H + HCO (+M) <=> CH2O (+M)
  {
    double rr_k0 = 1.35e+24 * exp(-2.57*vlntemp - 1.425e+03*ortc);
    double rr_kinf = 1.09e+12 * exp(0.48*vlntemp + 260.0*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[8];
    double fcent = log10(MAX(0.2176 * exp(-3.690036900369004e-03 * temperature) 
      + 0.7824 * exp(-3.629764065335753e-04 * temperature) + exp(-6.57e+03 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = -cgspl[0] + cgspl[10] - cgspl[11];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[36] = forward * mole_frac[0] * mole_frac[11];
    rr_r[36] = reverse * mole_frac[10];
  }
  //  37)  H2 + CO (+M) <=> CH2O (+M)
  {
    double rr_k0 = 5.07e+27 * exp(-3.42*vlntemp - 8.4348e+04*ortc);
    double rr_kinf = 4.3e+07 * exp(1.5*vlntemp - 7.96e+04*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[8];
    double fcent = log10(MAX(0.06799999999999995 * exp(-5.076142131979695e-03 * 
      temperature) + 0.9320000000000001 * exp(-6.493506493506494e-04 * 
      temperature) + exp(-1.03e+04 * otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = -cgspl[1] - cgspl[8] + cgspl[10];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[37] = forward * mole_frac[1] * mole_frac[8];
    rr_r[37] = reverse * mole_frac[10];
  }
  //  38)  OH + CH2O <=> H2O + HCO
  {
    double forward = 7.82e+07 * exp(1.63*vlntemp + 1.055e+03*ortc);
    double reverse = 4.896e+06 * exp(1.811*vlntemp - 2.903e+04*ortc);
    rr_f[38] = forward * mole_frac[4] * mole_frac[10];
    rr_r[38] = reverse * mole_frac[5] * mole_frac[11];
  }
  //  39)  H + CH2O <=> H2 + HCO
  {
    double forward = 5.74e+07 * exp(1.9*vlntemp - 2.74e+03*ortc);
    double reverse = 3.39e+05 * exp(2.187*vlntemp - 1.793e+04*ortc);
    rr_f[39] = forward * mole_frac[0] * mole_frac[10];
    rr_r[39] = reverse * mole_frac[1] * mole_frac[11];
  }
  //  40)  O + CH2O <=> OH + HCO
  {
    double forward = 6.26e+09 * exp(1.15*vlntemp - 2.26e+03*ortc);
    double reverse = 1.919e+07 * exp(1.418*vlntemp - 1.604e+04*ortc);
    rr_f[40] = forward * mole_frac[2] * mole_frac[10];
    rr_r[40] = reverse * mole_frac[4] * mole_frac[11];
  }
  //  41)  CH2O + CH3 <=> HCO + CH4
  {
    double forward = 38.3 * exp(3.36*vlntemp - 4.312e+03*ortc);
    double reverse = 206.3 * exp(3.201*vlntemp - 2.104e+04*ortc);
    rr_f[41] = forward * mole_frac[10] * mole_frac[17];
    rr_r[41] = reverse * mole_frac[11] * mole_frac[16];
  }
  //  42)  HO2 + CH2O <=> H2O2 + HCO
  {
    double forward = 7.1e-03 * exp(4.517*vlntemp - 6.58e+03*ortc);
    double reverse = 0.02426 * exp(4.108*vlntemp - 5.769e+03*ortc);
    rr_f[42] = forward * mole_frac[6] * mole_frac[10];
    rr_r[42] = reverse * mole_frac[7] * mole_frac[11];
  }
  //  43)  HOCH2O <=> OH + CH2O
  {
    double forward = 2.056e+21 * exp(-2.336*vlntemp - 2.573e+04*ortc);
    double reverse = 4.5e+15 * exp(-1.1 * vlntemp);
    rr_f[43] = forward;
    rr_r[43] = reverse * mole_frac[4] * mole_frac[10];
  }
  //  44)  HOCH2O <=> H + HOCHO
  {
    double forward = 1.0e+14 * exp(-1.49e+04*ortc);
    double reverse = 1.123e+15 * exp(-0.295*vlntemp - 1.15e+04*ortc);
    rr_f[44] = forward;
    rr_r[44] = reverse * mole_frac[0] * mole_frac[12];
  }
  //  45)  HOCHO <=> H2O + CO
  {
    double forward = 2.45e+12 * exp(-6.047e+04*ortc);
    double reverse = 2.255e+03 * exp(2.093*vlntemp - 5.289e+04*ortc);
    rr_f[45] = forward * mole_frac[12];
    rr_r[45] = reverse * mole_frac[5] * mole_frac[8];
  }
  //  46)  HOCHO <=> H2 + CO2
  {
    double forward = 2.95e+09 * exp(-4.852e+04*ortc);
    double reverse = 6.772e+05 * exp(1.008*vlntemp - 5.147e+04*ortc);
    rr_f[46] = forward * mole_frac[12];
    rr_r[46] = reverse * mole_frac[1] * mole_frac[9];
  }
  //  47)  HOCHO <=> OH + HCO
  {
    double forward = 3.471e+22 * exp(-1.542*vlntemp - 1.107e+05*ortc);
    double reverse = 1.0e+14;
    rr_f[47] = forward * mole_frac[12];
    rr_r[47] = reverse * mole_frac[4] * mole_frac[11];
  }
  //  48)  OH + HOCHO <=> H + H2O + CO2
  {
    double forward = 2.62e+06 * exp(2.06*vlntemp - 916.0*ortc);
    double reverse = 0.0;
    rr_f[48] = forward * mole_frac[4] * mole_frac[12];
    rr_r[48] = reverse * mole_frac[0] * mole_frac[5] * mole_frac[9];
  }
  //  49)  OH + HOCHO <=> OH + H2O + CO
  {
    double forward = 1.85e+07 * exp(1.51*vlntemp + 962.0*ortc);
    double reverse = 0.0;
    rr_f[49] = forward * mole_frac[4] * mole_frac[12];
    rr_r[49] = reverse * mole_frac[4] * mole_frac[5] * mole_frac[8];
  }
  //  50)  H + HOCHO => H + H2 + CO2
  {
    double forward = 4.24e+06 * exp(2.1*vlntemp - 4.868e+03*ortc);
    rr_f[50] = forward * mole_frac[0] * mole_frac[12];
    rr_r[50] = 0.0;
  }
  //  51)  H + HOCHO <=> H2 + OH + CO
  {
    double forward = 6.03e+13 * exp(-0.35*vlntemp - 2.988e+03*ortc);
    double reverse = 0.0;
    rr_f[51] = forward * mole_frac[0] * mole_frac[12];
    rr_r[51] = reverse * mole_frac[1] * mole_frac[4] * mole_frac[8];
  }
  //  52)  HOCHO + CH3 <=> OH + CO + CH4
  {
    double forward = 3.9e-07 * exp(5.8*vlntemp - 2.2e+03*ortc);
    double reverse = 0.0;
    rr_f[52] = forward * mole_frac[12] * mole_frac[17];
    rr_r[52] = reverse * mole_frac[4] * mole_frac[8] * mole_frac[16];
  }
  //  53)  HO2 + HOCHO => OH + H2O2 + CO
  {
    double forward = 1.0e+12 * exp(-1.192e+04*ortc);
    rr_f[53] = forward * mole_frac[6] * mole_frac[12];
    rr_r[53] = 0.0;
  }
  //  54)  O + HOCHO => 2 OH + CO
  {
    double forward = 1.77e+18 * exp(-1.9*vlntemp - 2.975e+03*ortc);
    rr_f[54] = forward * mole_frac[2] * mole_frac[12];
    rr_r[54] = 0.0;
  }
  //  55)  CH3O (+M) <=> H + CH2O (+M)
  {
    double rr_k0 = 1.867e+25 * exp(-3.0*vlntemp - 2.4307e+04*ortc);
    double rr_kinf = 6.8e+13 * exp(-2.617e+04*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[8];
    double fcent = log10(MAX(0.09999999999999998 * exp(-4.0e-04 * temperature) + 
      0.9 * exp(-7.692307692307692e-04 * temperature) + exp(-1.0e+99 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = cgspl[0] + cgspl[10] - cgspl[16];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[55] = forward;
    rr_r[55] = reverse * mole_frac[0] * mole_frac[10];
  }
  //  56)  O2 + CH3O <=> HO2 + CH2O
  {
    double forward = 4.38e-19 * exp(9.5*vlntemp + 5.501e+03*ortc);
    double reverse = 1.416e-20 * exp(9.816000000000001*vlntemp - 
      2.108e+04*ortc); 
    rr_f[56] = forward * mole_frac[3];
    rr_r[56] = reverse * mole_frac[6] * mole_frac[10];
  }
  //  57)  CH2O + CH3O <=> HCO + CH3OH
  {
    double forward = 6.62e+11 * exp(-2.294e+03*ortc);
    double reverse = 8.393e+10 * exp(0.074*vlntemp - 1.771e+04*ortc);
    rr_f[57] = forward * mole_frac[10];
    rr_r[57] = reverse * mole_frac[11] * mole_frac[13];
  }
  //  58)  CH3O + CH4 <=> CH3OH + CH3
  {
    double forward = 611.9 * exp(2.867*vlntemp - 8.248e+03*ortc);
    double reverse = 14.4 * exp(3.1*vlntemp - 6.935e+03*ortc);
    rr_f[58] = forward * mole_frac[16];
    rr_r[58] = reverse * mole_frac[13] * mole_frac[17];
  }
  //  59)  CH3O + CH3 <=> CH2O + CH4
  {
    double forward = 1.2e+13;
    double reverse = 6.749e+13 * exp(0.218*vlntemp - 8.281e+04*ortc);
    rr_f[59] = forward * mole_frac[17];
    rr_r[59] = reverse * mole_frac[10] * mole_frac[16];
  }
  //  60)  H + CH3O <=> H2 + CH2O
  {
    double forward = 2.0e+13;
    double reverse = 1.233e+11 * exp(0.664*vlntemp - 8.127e+04*ortc);
    rr_f[60] = forward * mole_frac[0];
    rr_r[60] = reverse * mole_frac[1] * mole_frac[10];
  }
  //  61)  HO2 + CH3O <=> H2O2 + CH2O
  {
    double forward = 3.01e+11;
    double reverse = 1.074e+12 * exp(-0.031*vlntemp - 6.527e+04*ortc);
    rr_f[61] = forward * mole_frac[6];
    rr_r[61] = reverse * mole_frac[7] * mole_frac[10];
  }
  //  62)  H + CH2O (+M) <=> CH2OH (+M)
  {
    double rr_k0 = 1.27e+32 * exp(-4.82*vlntemp - 6.53e+03*ortc);
    double rr_kinf = 5.4e+11 * exp(0.454*vlntemp - 3.6e+03*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[8];
    double fcent = log10(MAX(0.2813 * exp(-9.708737864077669e-03 * temperature) 
      + 0.7187 * exp(-7.74593338497289e-04 * temperature) + exp(-4.16e+03 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = -cgspl[0] - cgspl[10] + cgspl[15];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[62] = forward * mole_frac[0] * mole_frac[10];
    rr_r[62] = reverse;
  }
  //  63)  O2 + CH2OH <=> HO2 + CH2O
  {
    double forward = 1.51e+15 * otc;
    double reverse = 1.975e+14 * exp(-0.58*vlntemp - 2.006e+04*ortc);
    rr_f[63] = forward * mole_frac[3];
    rr_r[63] = reverse * mole_frac[6] * mole_frac[10];
  }
  //  64)  O2 + CH2OH <=> HO2 + CH2O
  {
    double forward = 2.41e+14 * exp(-5.017e+03*ortc);
    double reverse = 3.152e+13 * exp(0.42*vlntemp - 2.508e+04*ortc);
    rr_f[64] = forward * mole_frac[3];
    rr_r[64] = reverse * mole_frac[6] * mole_frac[10];
  }
  //  65)  H + CH2OH <=> H2 + CH2O
  {
    double forward = 6.0e+12;
    double reverse = 1.497e+11 * exp(0.768*vlntemp - 7.475e+04*ortc);
    rr_f[65] = forward * mole_frac[0];
    rr_r[65] = reverse * mole_frac[1] * mole_frac[10];
  }
  //  66)  HO2 + CH2OH <=> H2O2 + CH2O
  {
    double forward = 1.2e+13;
    double reverse = 1.732e+14 * exp(0.073*vlntemp - 5.875e+04*ortc);
    rr_f[66] = forward * mole_frac[6];
    rr_r[66] = reverse * mole_frac[7] * mole_frac[10];
  }
  //  67)  HCO + CH2OH <=> 2 CH2O
  {
    double forward = 1.8e+14;
    double reverse = 7.602e+14 * exp(0.481*vlntemp - 5.956e+04*ortc);
    rr_f[67] = forward * mole_frac[11];
    rr_r[67] = reverse * mole_frac[10] * mole_frac[10];
  }
  //  68)  CH2OH + CH3O <=> CH2O + CH3OH
  {
    double forward = 2.4e+13;
    double reverse = 1.285e+13 * exp(0.555*vlntemp - 7.498e+04*ortc);
    rr_f[68] = forward;
    rr_r[68] = reverse * mole_frac[10] * mole_frac[13];
  }
  //  69)  CH2O + CH2OH <=> HCO + CH3OH
  {
    double forward = 1.878e+04 * exp(2.722*vlntemp - 4.208e+03*ortc);
    double reverse = 9.63e+03 * exp(2.9*vlntemp - 1.311e+04*ortc);
    rr_f[69] = forward * mole_frac[10];
    rr_r[69] = reverse * mole_frac[11] * mole_frac[13];
  }
  //  70)  OH + CH2OH <=> H2O + CH2O
  {
    double forward = 2.4e+13;
    double reverse = 6.347e+12 * exp(0.662*vlntemp - 8.964e+04*ortc);
    rr_f[70] = forward * mole_frac[4];
    rr_r[70] = reverse * mole_frac[5] * mole_frac[10];
  }
  //  71)  O + CH2OH <=> OH + CH2O
  {
    double forward = 4.2e+13;
    double reverse = 5.438e+11 * exp(0.749*vlntemp - 7.334e+04*ortc);
    rr_f[71] = forward * mole_frac[2];
    rr_r[71] = reverse * mole_frac[4] * mole_frac[10];
  }
  //  72)  CH2O + CH3OH <=> 2 CH2OH
  {
    double forward = 6.498e+12 * exp(0.659*vlntemp - 6.846e+04*ortc);
    double reverse = 3.0e+12;
    rr_f[72] = forward * mole_frac[10] * mole_frac[13];
    rr_r[72] = reverse;
  }
  //  73)  HO2 + CH2OH <=> OH + HOCH2O
  {
    double forward = 1.0e+13;
    double reverse = 8.169e+13 * exp(-0.024*vlntemp - 3.347e+04*ortc);
    rr_f[73] = forward * mole_frac[6];
    rr_r[73] = reverse * mole_frac[4];
  }
  //  74)  CH3OH (+M) <=> OH + CH3 (+M)
  {
    double rr_k0 = 2.95e+44 * exp(-7.35*vlntemp - 9.546e+04*ortc);
    double rr_kinf = 1.9e+16 * exp(-9.173e+04*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[0];
    double fcent = log10(MAX(0.5860000000000001 * exp(-3.584229390681004e-03 * 
      temperature) + 0.414 * exp(-1.831837332844843e-04 * temperature) + 
      exp(-1.0e+10 * otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = cgspl[4] - cgspl[14] + cgspl[20];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[74] = forward * mole_frac[13];
    rr_r[74] = reverse * mole_frac[4] * mole_frac[17];
  }
  //  75)  CH3OH (+M) <=> H + CH2OH (+M)
  {
    double rr_k0 = 2.34e+40 * exp(-6.33*vlntemp - 1.031e+05*ortc);
    double rr_kinf = 2.69e+16 * exp(-0.08*vlntemp - 9.894e+04*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[0];
    double fcent = log10(MAX(0.227 * exp(-1.443001443001443e-03 * temperature) + 
      0.773 * exp(-1.875117194824677e-04 * temperature) + exp(-1.0e+10 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = cgspl[0] - cgspl[14] + cgspl[15];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[75] = forward * mole_frac[13];
    rr_r[75] = reverse * mole_frac[0];
  }
  //  76)  H + CH3OH <=> H2 + CH3O
  {
    double forward = 3.6e+12 * exp(-6.095e+03*ortc);
    double reverse = 1.677e+11 * exp(0.212*vlntemp - 5.868e+03*ortc);
    rr_f[76] = forward * mole_frac[0] * mole_frac[13];
    rr_r[76] = reverse * mole_frac[1];
  }
  //  77)  H + CH3OH <=> H2 + CH2OH
  {
    double forward = 1.204e+06 * exp(2.4*vlntemp - 2.583e+03*ortc);
    double reverse = 1.386e+04 * exp(2.509*vlntemp - 8.871e+03*ortc);
    rr_f[77] = forward * mole_frac[0] * mole_frac[13];
    rr_r[77] = reverse * mole_frac[1];
  }
  //  78)  O + CH3OH <=> OH + CH2OH
  {
    double forward = 3.88e+05 * exp(2.5*vlntemp - 3.08e+03*ortc);
    double reverse = 2.319e+03 * exp(2.59*vlntemp - 7.956e+03*ortc);
    rr_f[78] = forward * mole_frac[2] * mole_frac[13];
    rr_r[78] = reverse * mole_frac[4];
  }
  //  79)  OH + CH3OH <=> H2O + CH3O
  {
    double forward = 5.13e+05 * exp(2.13*vlntemp - 2.45e+03*ortc);
    double reverse = 2.534e+05 * exp(2.237*vlntemp - 1.712e+04*ortc);
    rr_f[79] = forward * mole_frac[4] * mole_frac[13];
    rr_r[79] = reverse * mole_frac[5];
  }
  //  80)  OH + CH3OH <=> H2O + CH2OH
  {
    double forward = 1.44e+06 * temperature * temperature * exp(839.0*ortc);
    double reverse = 1.758e+05 * exp(2.003*vlntemp - 2.034e+04*ortc);
    rr_f[80] = forward * mole_frac[4] * mole_frac[13];
    rr_r[80] = reverse * mole_frac[5];
  }
  //  81)  O2 + CH3OH <=> HO2 + CH2OH
  {
    double forward = 2.05e+13 * exp(-4.49e+04*ortc);
    double reverse = 1.238e+12 * exp(-0.239*vlntemp + 3.501e+03*ortc);
    rr_f[81] = forward * mole_frac[3] * mole_frac[13];
    rr_r[81] = reverse * mole_frac[6];
  }
  //  82)  HO2 + CH3OH <=> H2O2 + CH2OH
  {
    double forward = 1.08e+04 * exp(2.55*vlntemp - 1.053e+04*ortc);
    double reverse = 7.195e+04 * exp(1.963*vlntemp - 819.0*ortc);
    rr_f[82] = forward * mole_frac[6] * mole_frac[13];
    rr_r[82] = reverse * mole_frac[7];
  }
  //  83)  CH3OH + CH3 <=> CH2OH + CH4
  {
    double forward = 31.9 * exp(3.17*vlntemp - 7.172e+03*ortc);
    double reverse = 335.1 * exp(2.833*vlntemp - 1.5e+04*ortc);
    rr_f[83] = forward * mole_frac[13] * mole_frac[17];
    rr_r[83] = reverse * mole_frac[16];
  }
  //  84)  CH3OH + CH3O <=> CH3OH + CH2OH
  {
    double forward = 3.0e+11 * exp(-4.074e+03*ortc);
    double reverse = 7.416e+10 * exp(-0.104*vlntemp - 1.059e+04*ortc);
    rr_f[84] = forward * mole_frac[13];
    rr_r[84] = reverse * mole_frac[13];
  }
  //  85)  CH2O + CH3OH <=> 2 CH3O
  {
    double forward = 7.981e+12 * exp(0.452*vlntemp - 8.149e+04*ortc);
    double reverse = 6.03e+13;
    rr_f[85] = forward * mole_frac[10] * mole_frac[13];
    rr_r[85] = reverse;
  }
  //  86)  H + CH3 (+M) <=> CH4 (+M)
  {
    double rr_k0 = 2.477e+33 * exp(-4.76*vlntemp - 2.444e+03*ortc);
    double rr_kinf = 1.27e+16 * exp(-0.6*vlntemp - 383.0*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[8];
    double fcent = log10(MAX(0.217 * exp(-0.01351351351351351 * temperature) + 
      0.783 * exp(-3.401360544217687e-04 * temperature) + exp(-6.96e+03 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = -cgspl[0] + cgspl[19] - cgspl[20];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[86] = forward * mole_frac[0] * mole_frac[17];
    rr_r[86] = reverse * mole_frac[16];
  }
  //  87)  H + CH4 <=> H2 + CH3
  {
    double forward = 6.14e+05 * exp(2.5*vlntemp - 9.587e+03*ortc);
    double reverse = 673.0 * exp(2.946*vlntemp - 8.047e+03*ortc);
    rr_f[87] = forward * mole_frac[0] * mole_frac[16];
    rr_r[87] = reverse * mole_frac[1] * mole_frac[17];
  }
  //  88)  OH + CH4 <=> H2O + CH3
  {
    double forward = 5.83e+04 * exp(2.6*vlntemp - 2.19e+03*ortc);
    double reverse = 677.6 * exp(2.94*vlntemp - 1.554e+04*ortc);
    rr_f[88] = forward * mole_frac[4] * mole_frac[16];
    rr_r[88] = reverse * mole_frac[5] * mole_frac[17];
  }
  //  89)  O + CH4 <=> OH + CH3
  {
    double forward = 1.02e+09 * exp(1.5*vlntemp - 8.6e+03*ortc);
    double reverse = 5.804e+05 * exp(1.927*vlntemp - 5.648e+03*ortc);
    rr_f[89] = forward * mole_frac[2] * mole_frac[16];
    rr_r[89] = reverse * mole_frac[4] * mole_frac[17];
  }
  //  90)  HO2 + CH4 <=> H2O2 + CH3
  {
    double forward = 11.3 * exp(3.74*vlntemp - 2.101e+04*ortc);
    double reverse = 7.166 * exp(3.491*vlntemp - 3.468e+03*ortc);
    rr_f[90] = forward * mole_frac[6] * mole_frac[16];
    rr_r[90] = reverse * mole_frac[7] * mole_frac[17];
  }
  //  91)  CH4 + CH2 <=> 2 CH3
  {
    double forward = 2.46e+06 * temperature * temperature * exp(-8.27e+03*ortc);
    double reverse = 1.736e+06 * exp(1.868*vlntemp - 1.298e+04*ortc);
    rr_f[91] = forward * mole_frac[16];
    rr_r[91] = reverse * mole_frac[17] * mole_frac[17];
  }
  //  92)  OH + CH3 <=> H2 + CH2O
  {
    double forward = 8.0e+09 * exp(0.5*vlntemp + 1.755e+03*ortc);
    double reverse = 1.066e+12 * exp(0.322*vlntemp - 6.821e+04*ortc);
    rr_f[92] = forward * mole_frac[4] * mole_frac[17];
    rr_r[92] = reverse * mole_frac[1] * mole_frac[10];
  }
  //  93)  OH + CH3 <=> H2O + CH2(S)
  {
    double forward = 4.508e+17 * exp(-1.34*vlntemp - 1.417e+03*ortc);
    double reverse = 1.654e+16 * exp(-0.855*vlntemp - 1.039e+03*ortc);
    rr_f[93] = forward * mole_frac[4] * mole_frac[17];
    rr_r[93] = reverse * mole_frac[5];
  }
  //  94)  OH + CH3 <=> H + CH3O
  {
    double forward = 6.943e+07 * exp(1.343*vlntemp - 1.12e+04*ortc);
    double reverse = 1.5e+12 * exp(0.5*vlntemp + 110.0*ortc);
    rr_f[94] = forward * mole_frac[4] * mole_frac[17];
    rr_r[94] = reverse * mole_frac[0];
  }
  //  95)  OH + CH3 <=> H + CH2OH
  {
    double forward = 3.09e+07 * exp(1.596*vlntemp - 4.506e+03*ortc);
    double reverse = 1.65e+11 * exp(0.65*vlntemp + 284.0*ortc);
    rr_f[95] = forward * mole_frac[4] * mole_frac[17];
    rr_r[95] = reverse * mole_frac[0];
  }
  //  96)  OH + CH3 <=> H2O + CH2
  {
    double forward = 5.6e+07 * exp(1.6*vlntemp - 5.42e+03*ortc);
    double reverse = 9.224e+05 * exp(2.072*vlntemp - 1.406e+04*ortc);
    rr_f[96] = forward * mole_frac[4] * mole_frac[17];
    rr_r[96] = reverse * mole_frac[5];
  }
  //  97)  HO2 + CH3 <=> OH + CH3O
  {
    double forward = 1.0e+12 * exp(0.269*vlntemp + 687.5*ortc);
    double reverse = 6.19e+12 * exp(0.147*vlntemp - 2.455e+04*ortc);
    rr_f[97] = forward * mole_frac[6] * mole_frac[17];
    rr_r[97] = reverse * mole_frac[4];
  }
  //  98)  HO2 + CH3 <=> O2 + CH4
  {
    double forward = 1.16e+05 * exp(2.23*vlntemp + 3.022e+03*ortc);
    double reverse = 2.018e+07 * exp(2.132*vlntemp - 5.321e+04*ortc);
    rr_f[98] = forward * mole_frac[6] * mole_frac[17];
    rr_r[98] = reverse * mole_frac[3] * mole_frac[16];
  }
  //  99)  O + CH3 <=> H + CH2O
  {
    double forward = 5.54e+13 * exp(0.05*vlntemp + 136.0*ortc);
    double reverse = 3.83e+15 * exp(-0.147*vlntemp - 6.841e+04*ortc);
    rr_f[99] = forward * mole_frac[2] * mole_frac[17];
    rr_r[99] = reverse * mole_frac[0] * mole_frac[10];
  }
  // 100)  O2 + CH3 <=> O + CH3O
  {
    double forward = 7.546e+12 * exp(-2.832e+04*ortc);
    double reverse = 4.718e+14 * exp(-0.451*vlntemp - 288.0*ortc);
    rr_f[100] = forward * mole_frac[3] * mole_frac[17];
    rr_r[100] = reverse * mole_frac[2];
  }
  // 101)  O2 + CH3 <=> OH + CH2O
  {
    double forward = 2.641 * exp(3.283*vlntemp - 8.105e+03*ortc);
    double reverse = 0.5285 * exp(3.477*vlntemp - 5.992e+04*ortc);
    rr_f[101] = forward * mole_frac[3] * mole_frac[17];
    rr_r[101] = reverse * mole_frac[4] * mole_frac[10];
  }
  // 102)  O2 + CH3 (+M) <=> CH3O2 (+M)
  {
    double rr_k0 = 6.85e+24 * exp(-3.0 * vlntemp);
    double rr_kinf = 7.812e+09 * exp(0.9 * vlntemp);
    double pr = rr_k0 / rr_kinf * thbctemp[0];
    double fcent = log10(MAX(0.4 * exp(-1.0e-03 * temperature) + 0.6 * 
      exp(-0.01428571428571429 * temperature) + exp(-1.7e+03 * otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = -cgspl[3] + cgspl[18] - cgspl[20];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[102] = forward * mole_frac[3] * mole_frac[17];
    rr_r[102] = reverse * mole_frac[15];
  }
  // 103)  CH2O + CH3O2 <=> HCO + CH3O2H
  {
    double forward = 1.99e+12 * exp(-1.166e+04*ortc);
    double reverse = 1.323e+14 * exp(-0.853*vlntemp - 9.259e+03*ortc);
    rr_f[103] = forward * mole_frac[10] * mole_frac[15];
    rr_r[103] = reverse * mole_frac[11] * mole_frac[14];
  }
  // 104)  CH3O2 + CH4 <=> CH3O2H + CH3
  {
    double forward = 1.81e+11 * exp(-1.848e+04*ortc);
    double reverse = 2.233e+12 * exp(-0.694*vlntemp + 655.0*ortc);
    rr_f[104] = forward * mole_frac[15] * mole_frac[16];
    rr_r[104] = reverse * mole_frac[14] * mole_frac[17];
  }
  // 105)  CH3OH + CH3O2 <=> CH2OH + CH3O2H
  {
    double forward = 1.81e+12 * exp(-1.371e+04*ortc);
    double reverse = 2.346e+14 * exp(-1.031*vlntemp - 2.404e+03*ortc);
    rr_f[105] = forward * mole_frac[13] * mole_frac[15];
    rr_r[105] = reverse * mole_frac[14];
  }
  // 106)  CH3O2 + CH3 <=> 2 CH3O
  {
    double forward = 5.08e+12 * exp(1.411e+03*ortc);
    double reverse = 1.967e+12 * exp(0.176*vlntemp - 2.807e+04*ortc);
    rr_f[106] = forward * mole_frac[15] * mole_frac[17];
    rr_r[106] = reverse;
  }
  // 107)  HO2 + CH3O2 <=> O2 + CH3O2H
  {
    double forward = 2.47e+11 * exp(1.57e+03*ortc);
    double reverse = 5.302e+14 * exp(-0.792*vlntemp - 3.552e+04*ortc);
    rr_f[107] = forward * mole_frac[6] * mole_frac[15];
    rr_r[107] = reverse * mole_frac[3] * mole_frac[14];
  }
  // 108)  2 CH3O2 <=> O2 + CH2O + CH3OH
  {
    double forward = 3.11e+14 * exp(-1.61*vlntemp + 1.051e+03*ortc);
    double reverse = 0.0;
    rr_f[108] = forward * mole_frac[15] * mole_frac[15];
    rr_r[108] = reverse * mole_frac[3] * mole_frac[10] * mole_frac[13];
  }
  // 109)  2 CH3O2 <=> O2 + 2 CH3O
  {
    double forward = 1.4e+16 * exp(-1.61*vlntemp - 1.86e+03*ortc);
    double reverse = 0.0;
    rr_f[109] = forward * mole_frac[15] * mole_frac[15];
    rr_r[109] = reverse * mole_frac[3];
  }
  // 110)  H + CH3O2 <=> OH + CH3O
  {
    double forward = 9.6e+13;
    double reverse = 1.72e+09 * exp(1.019*vlntemp - 4.078e+04*ortc);
    rr_f[110] = forward * mole_frac[0] * mole_frac[15];
    rr_r[110] = reverse * mole_frac[4];
  }
  // 111)  O + CH3O2 <=> O2 + CH3O
  {
    double forward = 3.6e+13;
    double reverse = 2.229e+11 * exp(0.628*vlntemp - 5.752e+04*ortc);
    rr_f[111] = forward * mole_frac[2] * mole_frac[15];
    rr_r[111] = reverse * mole_frac[3];
  }
  // 112)  OH + CH3O2 <=> O2 + CH3OH
  {
    double forward = 6.0e+13;
    double reverse = 1.536e+13 * exp(0.434*vlntemp - 5.916e+04*ortc);
    rr_f[112] = forward * mole_frac[4] * mole_frac[15];
    rr_r[112] = reverse * mole_frac[3] * mole_frac[13];
  }
  // 113)  CH3O2H <=> OH + CH3O
  {
    double forward = 6.31e+14 * exp(-4.23e+04*ortc);
    double reverse = 2.514e+06 * exp(1.883*vlntemp + 2.875e+03*ortc);
    rr_f[113] = forward * mole_frac[14];
    rr_r[113] = reverse * mole_frac[4];
  }
  // 114)  CH2(S) <=> CH2
  {
    double forward = 1.0e+13;
    double reverse = 4.488e+12 * exp(-0.013*vlntemp - 9.02e+03*ortc);
    rr_f[114] = forward;
    rr_r[114] = reverse;
  }
  // 115)  CH4 + CH2(S) <=> 2 CH3
  {
    double forward = 1.6e+13 * exp(570.0*ortc);
    double reverse = 5.067e+12 * exp(-0.145*vlntemp - 1.316e+04*ortc);
    rr_f[115] = forward * mole_frac[16];
    rr_r[115] = reverse * mole_frac[17] * mole_frac[17];
  }
  // 116)  O2 + CH2(S) <=> H + OH + CO
  {
    double forward = 7.0e+13;
    double reverse = 0.0;
    rr_f[116] = forward * mole_frac[3];
    rr_r[116] = reverse * mole_frac[0] * mole_frac[4] * mole_frac[8];
  }
  // 117)  H2 + CH2(S) <=> H + CH3
  {
    double forward = 7.0e+13;
    double reverse = 2.022e+16 * exp(-0.591*vlntemp - 1.527e+04*ortc);
    rr_f[117] = forward * mole_frac[1];
    rr_r[117] = reverse * mole_frac[0] * mole_frac[17];
  }
  // 118)  H + CH2(S) <=> H + CH2
  {
    double forward = 3.0e+13;
    double reverse = 1.346e+13 * exp(-0.013*vlntemp - 9.02e+03*ortc);
    rr_f[118] = forward * mole_frac[0];
    rr_r[118] = reverse * mole_frac[0];
  }
  // 119)  O + CH2(S) <=> 2 H + CO
  {
    double forward = 3.0e+13;
    double reverse = 0.0;
    rr_f[119] = forward * mole_frac[2];
    rr_r[119] = reverse * mole_frac[0] * mole_frac[0] * mole_frac[8];
  }
  // 120)  OH + CH2(S) <=> H + CH2O
  {
    double forward = 3.0e+13;
    double reverse = 1.154e+18 * exp(-0.77*vlntemp - 8.523e+04*ortc);
    rr_f[120] = forward * mole_frac[4];
    rr_r[120] = reverse * mole_frac[0] * mole_frac[10];
  }
  // 121)  CO2 + CH2(S) <=> CO + CH2O
  {
    double forward = 3.0e+12;
    double reverse = 4.366e+10 * exp(0.421*vlntemp - 5.981e+04*ortc);
    rr_f[121] = forward * mole_frac[9];
    rr_r[121] = reverse * mole_frac[8] * mole_frac[10];
  }
  // 122)  H + CH2 (+M) <=> CH3 (+M)
  {
    double rr_k0 = 3.2e+27 * exp(-3.14*vlntemp - 1.23e+03*ortc);
    double rr_kinf = 2.5e+16 * exp(-0.8 * vlntemp);
    double pr = rr_k0 / rr_kinf * thbctemp[8];
    double fcent = log10(MAX(0.32 * exp(-0.01282051282051282 * temperature) + 
      0.68 * exp(-5.012531328320802e-04 * temperature) + exp(-5.59e+03 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = -cgspl[0] + cgspl[20] - cgspl[21];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[122] = forward * mole_frac[0];
    rr_r[122] = reverse * mole_frac[17];
  }
  // 123)  O2 + CH2 <=> O + CH2O
  {
    double forward = 2.4e+12 * exp(-1.5e+03*ortc);
    double reverse = 5.955e+14 * exp(-0.365*vlntemp - 6.098e+04*ortc);
    rr_f[123] = forward * mole_frac[3];
    rr_r[123] = reverse * mole_frac[2] * mole_frac[10];
  }
  // 124)  O2 + CH2 <=> 2 H + CO2
  {
    double forward = 5.8e+12 * exp(-1.5e+03*ortc);
    double reverse = 0.0;
    rr_f[124] = forward * mole_frac[3];
    rr_r[124] = reverse * mole_frac[0] * mole_frac[0] * mole_frac[9];
  }
  // 125)  O2 + CH2 <=> H + OH + CO
  {
    double forward = 5.0e+12 * exp(-1.5e+03*ortc);
    double reverse = 0.0;
    rr_f[125] = forward * mole_frac[3];
    rr_r[125] = reverse * mole_frac[0] * mole_frac[4] * mole_frac[8];
  }
  // 126)  O + CH2 <=> 2 H + CO
  {
    double forward = 5.0e+13;
    double reverse = 0.0;
    rr_f[126] = forward * mole_frac[2];
    rr_r[126] = reverse * mole_frac[0] * mole_frac[0] * mole_frac[8];
  }
  // 127)  2 CH3 (+M) <=> C2H6 (+M)
  {
    double rr_k0 = 1.135e+36 * exp(-5.246*vlntemp - 1.705e+03*ortc);
    double rr_kinf = 9.214e+16 * exp(-1.17*vlntemp - 635.8*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[8];
    double fcent = log10(MAX(0.595 * exp(-8.928571428571428e-04 * temperature) + 
      0.405 * exp(-0.01436781609195402 * temperature) + exp(-1.0e+10 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = -2.0 * cgspl[20] + cgspl[23];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[127] = forward * mole_frac[17] * mole_frac[17];
    rr_r[127] = reverse * mole_frac[18];
  }
  // 128)  H + C2H5 (+M) <=> C2H6 (+M)
  {
    double rr_k0 = 1.99e+41 * exp(-7.08*vlntemp - 6.685e+03*ortc);
    double rr_kinf = 5.21e+17 * exp(-0.99*vlntemp - 1.58e+03*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[8];
    double fcent = log10(MAX(0.158 * exp(-8.0e-03 * temperature) + 0.842 * 
      exp(-4.506534474988734e-04 * temperature) + exp(-6.882e+03 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = -cgspl[0] + cgspl[23] - cgspl[24];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[128] = forward * mole_frac[0] * mole_frac[19];
    rr_r[128] = reverse * mole_frac[18];
  }
  // 129)  H + C2H6 <=> H2 + C2H5
  {
    double forward = 1.15e+08 * exp(1.9*vlntemp - 7.53e+03*ortc);
    double reverse = 1.062e+04 * exp(2.582*vlntemp - 9.76e+03*ortc);
    rr_f[129] = forward * mole_frac[0] * mole_frac[18];
    rr_r[129] = reverse * mole_frac[1] * mole_frac[19];
  }
  // 130)  O + C2H6 <=> OH + C2H5
  {
    double forward = 3.55e+06 * exp(2.4*vlntemp - 5.83e+03*ortc);
    double reverse = 170.2 * exp(3.063*vlntemp - 6.648e+03*ortc);
    rr_f[130] = forward * mole_frac[2] * mole_frac[18];
    rr_r[130] = reverse * mole_frac[4] * mole_frac[19];
  }
  // 131)  OH + C2H6 <=> H2O + C2H5
  {
    double forward = 1.48e+07 * exp(1.9*vlntemp - 950.0*ortc);
    double reverse = 1.45e+04 * exp(2.476*vlntemp - 1.807e+04*ortc);
    rr_f[131] = forward * mole_frac[4] * mole_frac[18];
    rr_r[131] = reverse * mole_frac[5] * mole_frac[19];
  }
  // 132)  O2 + C2H6 <=> HO2 + C2H5
  {
    double forward = 6.03e+13 * exp(-5.187e+04*ortc);
    double reverse = 2.921e+10 * exp(0.334*vlntemp + 593.0*ortc);
    rr_f[132] = forward * mole_frac[3] * mole_frac[18];
    rr_r[132] = reverse * mole_frac[6] * mole_frac[19];
  }
  // 133)  CH3 + C2H6 <=> CH4 + C2H5
  {
    double forward = 1.51e-07 * exp(6.0*vlntemp - 6.047e+03*ortc);
    double reverse = 1.273e-08 * exp(6.236*vlntemp - 9.817e+03*ortc);
    rr_f[133] = forward * mole_frac[17] * mole_frac[18];
    rr_r[133] = reverse * mole_frac[16] * mole_frac[19];
  }
  // 134)  HO2 + C2H6 <=> H2O2 + C2H5
  {
    double forward = 34.6 * exp(3.61*vlntemp - 1.692e+04*ortc);
    double reverse = 1.849 * exp(3.597*vlntemp - 3.151e+03*ortc);
    rr_f[134] = forward * mole_frac[6] * mole_frac[18];
    rr_r[134] = reverse * mole_frac[7] * mole_frac[19];
  }
  // 135)  CH3O2 + C2H6 <=> CH3O2H + C2H5
  {
    double forward = 19.4 * exp(3.64*vlntemp - 1.71e+04*ortc);
    double reverse = 20.17 * exp(3.182*vlntemp - 1.734e+03*ortc);
    rr_f[135] = forward * mole_frac[15] * mole_frac[18];
    rr_r[135] = reverse * mole_frac[14] * mole_frac[19];
  }
  // 136)  CH3O + C2H6 <=> CH3OH + C2H5
  {
    double forward = 2.41e+11 * exp(-7.09e+03*ortc);
    double reverse = 4.779e+08 * exp(0.469*vlntemp - 9.547e+03*ortc);
    rr_f[136] = forward * mole_frac[18];
    rr_r[136] = reverse * mole_frac[13] * mole_frac[19];
  }
  // 137)  CH2(S) + C2H6 <=> CH3 + C2H5
  {
    double forward = 1.2e+14;
    double reverse = 3.203e+12 * exp(0.091*vlntemp - 1.75e+04*ortc);
    rr_f[137] = forward * mole_frac[18];
    rr_r[137] = reverse * mole_frac[17] * mole_frac[19];
  }
  // 138)  H + C2H4 (+M) <=> C2H5 (+M)
  {
    double rr_k0 = 1.2e+42 * exp(-7.62*vlntemp - 6.97e+03*ortc);
    double rr_kinf = 1.081e+12 * exp(0.454*vlntemp - 1.822e+03*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[8];
    double fcent = log10(MAX(0.02500000000000002 * exp(-4.761904761904762e-03 * 
      temperature) + 0.975 * exp(-1.016260162601626e-03 * temperature) + 
      exp(-4.374e+03 * otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = -cgspl[0] + cgspl[24] - cgspl[25];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[138] = forward * mole_frac[0] * mole_frac[20];
    rr_r[138] = reverse * mole_frac[19];
  }
  // 139)  H2 + CH3O2 <=> H + CH3O2H
  {
    double forward = 1.5e+14 * exp(-2.603e+04*ortc);
    double reverse = 1.688e+18 * exp(-1.14*vlntemp - 8.434e+03*ortc);
    rr_f[139] = forward * mole_frac[1] * mole_frac[15];
    rr_r[139] = reverse * mole_frac[0] * mole_frac[14];
  }
  // 140)  H2 + C2H5O2 <=> H + C2H5O2H
  {
    double forward = 1.5e+14 * exp(-2.603e+04*ortc);
    double reverse = 1.691e+18 * exp(-1.14*vlntemp - 8.438e+03*ortc);
    rr_f[140] = forward * mole_frac[1] * mole_frac[29];
    rr_r[140] = reverse * mole_frac[0] * mole_frac[28];
  }
  // 141)  C2H5 + C2H3 <=> 2 C2H4
  {
    double forward = 6.859e+11 * exp(0.11*vlntemp + 4.3e+03*ortc);
    double reverse = 4.82e+14 * exp(-7.153e+04*ortc);
    rr_f[141] = forward * mole_frac[19] * mole_frac[21];
    rr_r[141] = reverse * mole_frac[20] * mole_frac[20];
  }
  // 142)  CH3 + C2H5 <=> CH4 + C2H4
  {
    double forward = 1.18e+04 * exp(2.45*vlntemp + 2.921e+03*ortc);
    double reverse = 2.39e+06 * exp(2.4*vlntemp - 6.669e+04*ortc);
    rr_f[142] = forward * mole_frac[17] * mole_frac[19];
    rr_r[142] = reverse * mole_frac[16] * mole_frac[20];
  }
  // 143)  H + C2H5 <=> 2 CH3
  {
    double forward = 9.69e+13 * exp(-220.0*ortc);
    double reverse = 2.029e+09 * exp(1.028*vlntemp - 1.051e+04*ortc);
    rr_f[143] = forward * mole_frac[0] * mole_frac[19];
    rr_r[143] = reverse * mole_frac[17] * mole_frac[17];
  }
  // 144)  H + C2H5 <=> H2 + C2H4
  {
    double forward = 2.0e+12;
    double reverse = 4.44e+11 * exp(0.396*vlntemp - 6.807e+04*ortc);
    rr_f[144] = forward * mole_frac[0] * mole_frac[19];
    rr_r[144] = reverse * mole_frac[1] * mole_frac[20];
  }
  // 145)  O + C2H5 <=> H + CH3CHO
  {
    double forward = 1.1e+14;
    double reverse = 1.033e+17 * exp(-0.5*vlntemp - 7.742e+04*ortc);
    rr_f[145] = forward * mole_frac[2] * mole_frac[19];
    rr_r[145] = reverse * mole_frac[0] * mole_frac[23];
  }
  // 146)  HO2 + C2H5 <=> OH + C2H5O
  {
    double forward = 1.1e+13;
    double reverse = 9.68e+15 * exp(-0.723*vlntemp - 2.765e+04*ortc);
    rr_f[146] = forward * mole_frac[6] * mole_frac[19];
    rr_r[146] = reverse * mole_frac[4];
  }
  // 147)  CH3O2 + C2H5 <=> CH3O + C2H5O
  {
    double forward = 8.0e+12 * exp(1.0e+03*ortc);
    double reverse = 4.404e+14 * exp(-0.425*vlntemp - 3.089e+04*ortc);
    rr_f[147] = forward * mole_frac[15] * mole_frac[19];
    rr_r[147] = reverse;
  }
  // 148)  O2 + C2H5O <=> HO2 + CH3CHO
  {
    double forward = 4.28e+10 * exp(-1.097e+03*ortc);
    double reverse = 1.322e+08 * exp(0.615*vlntemp - 3.413e+04*ortc);
    rr_f[148] = forward * mole_frac[3];
    rr_r[148] = reverse * mole_frac[6] * mole_frac[23];
  }
  // 149)  C2H5O <=> CH2O + CH3
  {
    double forward = 1.321e+20 * exp(-2.018*vlntemp - 2.075e+04*ortc);
    double reverse = 3.0e+11 * exp(-6.336e+03*ortc);
    rr_f[149] = forward;
    rr_r[149] = reverse * mole_frac[10] * mole_frac[17];
  }
  // 150)  C2H5O <=> H + CH3CHO
  {
    double forward = 5.428e+15 * exp(-0.6870000000000001*vlntemp - 
      2.223e+04*ortc); 
    double reverse = 8.0e+12 * exp(-6.4e+03*ortc);
    rr_f[150] = forward;
    rr_r[150] = reverse * mole_frac[0] * mole_frac[23];
  }
  // 151)  C2H5O2 <=> O2 + C2H5
  {
    double forward = 1.312e+62 * exp(-14.784*vlntemp - 4.918e+04*ortc);
    double reverse = 2.876e+56 * exp(-13.82*vlntemp - 1.462e+04*ortc);
    rr_f[151] = forward * mole_frac[29];
    rr_r[151] = reverse * mole_frac[3] * mole_frac[19];
  }
  // 152)  CH2O + C2H5O2 <=> HCO + C2H5O2H
  {
    double forward = 1.99e+12 * exp(-1.166e+04*ortc);
    double reverse = 1.325e+14 * exp(-0.853*vlntemp - 9.263e+03*ortc);
    rr_f[152] = forward * mole_frac[10] * mole_frac[29];
    rr_r[152] = reverse * mole_frac[11] * mole_frac[28];
  }
  // 153)  CH4 + C2H5O2 <=> CH3 + C2H5O2H
  {
    double forward = 1.81e+11 * exp(-1.848e+04*ortc);
    double reverse = 2.237e+12 * exp(-0.694*vlntemp + 651.0*ortc);
    rr_f[153] = forward * mole_frac[16] * mole_frac[29];
    rr_r[153] = reverse * mole_frac[17] * mole_frac[28];
  }
  // 154)  CH3OH + C2H5O2 <=> CH2OH + C2H5O2H
  {
    double forward = 1.81e+12 * exp(-1.371e+04*ortc);
    double reverse = 2.35e+14 * exp(-1.031*vlntemp - 2.408e+03*ortc);
    rr_f[154] = forward * mole_frac[13] * mole_frac[29];
    rr_r[154] = reverse * mole_frac[28];
  }
  // 155)  HO2 + C2H5O2 <=> O2 + C2H5O2H
  {
    double forward = 1.75e+10 * exp(3.275e+03*ortc);
    double reverse = 3.763e+13 * exp(-0.792*vlntemp - 3.382e+04*ortc);
    rr_f[155] = forward * mole_frac[6] * mole_frac[29];
    rr_r[155] = reverse * mole_frac[3] * mole_frac[28];
  }
  // 156)  C2H6 + C2H5O2 <=> C2H5 + C2H5O2H
  {
    double forward = 8.6 * exp(3.76*vlntemp - 1.72e+04*ortc);
    double reverse = 8.957000000000001 * exp(3.302*vlntemp - 1.838e+03*ortc);
    rr_f[156] = forward * mole_frac[18] * mole_frac[29];
    rr_r[156] = reverse * mole_frac[19] * mole_frac[28];
  }
  // 157)  C2H5O2H <=> OH + C2H5O
  {
    double forward = 6.31e+14 * exp(-4.23e+04*ortc);
    double reverse = 5.661e+08 * exp(1.033*vlntemp + 1.705e+03*ortc);
    rr_f[157] = forward * mole_frac[28];
    rr_r[157] = reverse * mole_frac[4];
  }
  // 158)  O2 + C2H5 <=> HO2 + C2H4
  {
    double forward = 7.561e+14 * exp(-1.01*vlntemp - 4.749e+03*ortc);
    double reverse = 8.802e+14 * exp(-0.962*vlntemp - 1.813e+04*ortc);
    rr_f[158] = forward * mole_frac[3] * mole_frac[19];
    rr_r[158] = reverse * mole_frac[6] * mole_frac[20];
  }
  // 159)  O2 + C2H5 <=> HO2 + C2H4
  {
    double forward = 0.4 * exp(3.88*vlntemp - 1.362e+04*ortc);
    double reverse = 0.4656 * exp(3.928*vlntemp - 2.7e+04*ortc);
    rr_f[159] = forward * mole_frac[3] * mole_frac[19];
    rr_r[159] = reverse * mole_frac[6] * mole_frac[20];
  }
  // 160)  O2 + C2H5 <=> OH + CH3CHO
  {
    double forward = 826.5 * exp(2.41*vlntemp - 5.285e+03*ortc);
    double reverse = 2.247e+03 * exp(2.301*vlntemp - 6.597e+04*ortc);
    rr_f[160] = forward * mole_frac[3] * mole_frac[19];
    rr_r[160] = reverse * mole_frac[4] * mole_frac[23];
  }
  // 161)  C2H5O2 <=> OH + CH3CHO
  {
    double forward = 2.52e+41 * exp(-10.2*vlntemp - 4.371e+04*ortc);
    double reverse = 1.502e+36 * exp(-9.345000000000001*vlntemp - 
      6.984e+04*ortc); 
    rr_f[161] = forward * mole_frac[29];
    rr_r[161] = reverse * mole_frac[4] * mole_frac[23];
  }
  // 162)  C2H5O2 <=> HO2 + C2H4
  {
    double forward = 1.815e+38 * exp(-8.449999999999999*vlntemp - 
      3.789e+04*ortc); 
    double reverse = 4.632e+32 * exp(-7.438*vlntemp - 1.67e+04*ortc);
    rr_f[162] = forward * mole_frac[29];
    rr_r[162] = reverse * mole_frac[6] * mole_frac[20];
  }
  // 163)  C2H3O1-2 <=> CH3CO
  {
    double forward = 8.5e+14 * exp(-1.4e+04*ortc);
    double reverse = 1.002e+14 * exp(0.041*vlntemp - 4.871e+04*ortc);
    rr_f[163] = forward;
    rr_r[163] = reverse;
  }
  // 164)  C2H3O1-2 <=> CH2CHO
  {
    double forward = 1.0e+14 * exp(-1.4e+04*ortc);
    double reverse = 1.245e+15 * exp(-0.375*vlntemp - 4.401e+04*ortc);
    rr_f[164] = forward;
    rr_r[164] = reverse * mole_frac[24];
  }
  // 165)  CH3CHO <=> HCO + CH3
  {
    double forward = 7.687e+20 * exp(-1.342*vlntemp - 8.695e+04*ortc);
    double reverse = 1.75e+13;
    rr_f[165] = forward * mole_frac[23];
    rr_r[165] = reverse * mole_frac[11] * mole_frac[17];
  }
  // 166)  H + CH3CHO <=> H2 + CH3CO
  {
    double forward = 2.37e+13 * exp(-3.642e+03*ortc);
    double reverse = 1.639e+10 * exp(0.633*vlntemp - 1.76e+04*ortc);
    rr_f[166] = forward * mole_frac[0] * mole_frac[23];
    rr_r[166] = reverse * mole_frac[1];
  }
  // 167)  O + CH3CHO <=> OH + CH3CO
  {
    double forward = 5.94e+12 * exp(-1.868e+03*ortc);
    double reverse = 2.133e+09 * exp(0.614*vlntemp - 1.441e+04*ortc);
    rr_f[167] = forward * mole_frac[2] * mole_frac[23];
    rr_r[167] = reverse * mole_frac[4];
  }
  // 168)  OH + CH3CHO <=> H2O + CH3CO
  {
    double forward = 3.37e+12 * exp(619.0*ortc);
    double reverse = 2.472e+10 * exp(0.527*vlntemp - 2.823e+04*ortc);
    rr_f[168] = forward * mole_frac[4] * mole_frac[23];
    rr_r[168] = reverse * mole_frac[5];
  }
  // 169)  O2 + CH3CHO <=> HO2 + CH3CO
  {
    double forward = 3.01e+13 * exp(-3.915e+04*ortc);
    double reverse = 1.092e+11 * exp(0.285*vlntemp + 1.588e+03*ortc);
    rr_f[169] = forward * mole_frac[3] * mole_frac[23];
    rr_r[169] = reverse * mole_frac[6];
  }
  // 170)  CH3 + CH3CHO <=> CH4 + CH3CO
  {
    double forward = 7.08e-04 * exp(4.58*vlntemp - 1.966e+03*ortc);
    double reverse = 4.468e-04 * exp(4.767*vlntemp - 1.746e+04*ortc);
    rr_f[170] = forward * mole_frac[17] * mole_frac[23];
    rr_r[170] = reverse * mole_frac[16];
  }
  // 171)  HO2 + CH3CHO <=> H2O2 + CH3CO
  {
    double forward = 3.01e+12 * exp(-1.192e+04*ortc);
    double reverse = 1.205e+12 * exp(-0.062*vlntemp - 9.877e+03*ortc);
    rr_f[171] = forward * mole_frac[6] * mole_frac[23];
    rr_r[171] = reverse * mole_frac[7];
  }
  // 172)  CH3O2 + CH3CHO <=> CH3O2H + CH3CO
  {
    double forward = 3.01e+12 * exp(-1.192e+04*ortc);
    double reverse = 2.344e+13 * exp(-0.507*vlntemp - 8.282e+03*ortc);
    rr_f[172] = forward * mole_frac[15] * mole_frac[23];
    rr_r[172] = reverse * mole_frac[14];
  }
  // 173)  OH + CH3CHO <=> HOCHO + CH3
  {
    double forward = 3.0e+15 * exp(-1.076 * vlntemp);
    double reverse = 2.371e+16 * exp(-1.277*vlntemp - 2.375e+04*ortc);
    rr_f[173] = forward * mole_frac[4] * mole_frac[23];
    rr_r[173] = reverse * mole_frac[12] * mole_frac[17];
  }
  // 174)  OH + CH3CHO <=> H2O + CH2CHO
  {
    double forward = 1.72e+05 * exp(2.4*vlntemp - 815.0*ortc);
    double reverse = 1.332e+05 * exp(2.511*vlntemp - 2.495e+04*ortc);
    rr_f[174] = forward * mole_frac[4] * mole_frac[23];
    rr_r[174] = reverse * mole_frac[5] * mole_frac[24];
  }
  // 175)  CH3CO (+M) <=> CO + CH3 (+M)
  {
    double rr_k0 = 1.2e+15 * exp(-1.2518e+04*ortc);
    double rr_kinf = 3.0e+12 * exp(-1.672e+04*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[0];
    double forward = rr_kinf * pr/(1.0 + pr);
    double xik = cgspl[8] + cgspl[20] - cgspl[29];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[175] = forward;
    rr_r[175] = reverse * mole_frac[8] * mole_frac[17];
  }
  // 176)  H + CH3CO <=> H2 + CH2CO
  {
    double forward = 2.0e+13;
    double reverse = 1.037e+13 * exp(0.201*vlntemp - 6.056e+04*ortc);
    rr_f[176] = forward * mole_frac[0];
    rr_r[176] = reverse * mole_frac[1] * mole_frac[25];
  }
  // 177)  O + CH3CO <=> OH + CH2CO
  {
    double forward = 2.0e+13;
    double reverse = 5.381e+12 * exp(0.182*vlntemp - 5.914e+04*ortc);
    rr_f[177] = forward * mole_frac[2];
    rr_r[177] = reverse * mole_frac[4] * mole_frac[25];
  }
  // 178)  CH3 + CH3CO <=> CH4 + CH2CO
  {
    double forward = 5.0e+13;
    double reverse = 2.364e+16 * exp(-0.245*vlntemp - 6.21e+04*ortc);
    rr_f[178] = forward * mole_frac[17];
    rr_r[178] = reverse * mole_frac[16] * mole_frac[25];
  }
  // 179)  CH3CO3 <=> O2 + CH3CO
  {
    double forward = 6.863e+19 * exp(-1.949*vlntemp - 3.853e+04*ortc);
    double reverse = 1.2e+11 * exp(1.1e+03*ortc);
    rr_f[179] = forward * mole_frac[27];
    rr_r[179] = reverse * mole_frac[3];
  }
  // 180)  CH2CHO <=> H + CH2CO
  {
    double forward = 4.071e+15 * exp(-0.342*vlntemp - 5.06e+04*ortc);
    double reverse = 5.0e+13 * exp(-1.23e+04*ortc);
    rr_f[180] = forward * mole_frac[24];
    rr_r[180] = reverse * mole_frac[0] * mole_frac[25];
  }
  // 181)  O2 + CH2CHO <=> OH + CO + CH2O
  {
    double forward = 8.95e+13 * exp(-0.6*vlntemp - 1.012e+04*ortc);
    double reverse = 0.0;
    rr_f[181] = forward * mole_frac[3] * mole_frac[24];
    rr_r[181] = reverse * mole_frac[4] * mole_frac[8] * mole_frac[10];
  }
  // 182)  CO + CH2 (+M) <=> CH2CO (+M)
  {
    double rr_k0 = 2.69e+33 * exp(-5.11*vlntemp - 7.095e+03*ortc);
    double rr_kinf = 8.1e+11;
    double pr = rr_k0 / rr_kinf * thbctemp[8];
    double fcent = log10(MAX(0.4093 * exp(-3.636363636363636e-03 * temperature) 
      + 0.5907 * exp(-8.156606851549756e-04 * temperature) + exp(-5.185e+03 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = -cgspl[8] - cgspl[21] + cgspl[31];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[182] = forward * mole_frac[8];
    rr_r[182] = reverse * mole_frac[25];
  }
  // 183)  H + CH2CO <=> CO + CH3
  {
    double forward = 1.1e+13 * exp(-3.4e+03*ortc);
    double reverse = 2.4e+12 * exp(-4.02e+04*ortc);
    rr_f[183] = forward * mole_frac[0] * mole_frac[25];
    rr_r[183] = reverse * mole_frac[8] * mole_frac[17];
  }
  // 184)  H + CH2CO <=> H2 + HCCO
  {
    double forward = 2.0e+14 * exp(-8.0e+03*ortc);
    double reverse = 1.434e+11 * exp(0.47*vlntemp - 4.52e+03*ortc);
    rr_f[184] = forward * mole_frac[0] * mole_frac[25];
    rr_r[184] = reverse * mole_frac[1] * mole_frac[26];
  }
  // 185)  O + CH2CO <=> CO2 + CH2
  {
    double forward = 1.75e+12 * exp(-1.35e+03*ortc);
    double reverse = 2.854e+09 * exp(0.8090000000000001*vlntemp - 
      4.944e+04*ortc); 
    rr_f[185] = forward * mole_frac[2] * mole_frac[25];
    rr_r[185] = reverse * mole_frac[9];
  }
  // 186)  O + CH2CO <=> OH + HCCO
  {
    double forward = 1.0e+13 * exp(-8.0e+03*ortc);
    double reverse = 3.723e+09 * exp(0.452*vlntemp - 3.108e+03*ortc);
    rr_f[186] = forward * mole_frac[2] * mole_frac[25];
    rr_r[186] = reverse * mole_frac[4] * mole_frac[26];
  }
  // 187)  OH + CH2CO <=> H2O + HCCO
  {
    double forward = 1.0e+13 * exp(-2.0e+03*ortc);
    double reverse = 7.604e+10 * exp(0.365*vlntemp - 1.341e+04*ortc);
    rr_f[187] = forward * mole_frac[4] * mole_frac[25];
    rr_r[187] = reverse * mole_frac[5] * mole_frac[26];
  }
  // 188)  OH + CH2CO <=> CO + CH2OH
  {
    double forward = 2.0e+12 * exp(1.01e+03*ortc);
    double reverse = 8.17e+09 * exp(0.494*vlntemp - 2.453e+04*ortc);
    rr_f[188] = forward * mole_frac[4] * mole_frac[25];
    rr_r[188] = reverse * mole_frac[8];
  }
  // 189)  CH2(S) + CH2CO <=> CO + C2H4
  {
    double forward = 1.6e+14;
    double reverse = 3.75e+14 * exp(0.217*vlntemp - 1.034e+05*ortc);
    rr_f[189] = forward * mole_frac[25];
    rr_r[189] = reverse * mole_frac[8] * mole_frac[20];
  }
  // 190)  OH + HCCO <=> H2 + 2 CO
  {
    double forward = 1.0e+14;
    double reverse = 0.0;
    rr_f[190] = forward * mole_frac[4] * mole_frac[26];
    rr_r[190] = reverse * mole_frac[1] * mole_frac[8] * mole_frac[8];
  }
  // 191)  H + HCCO <=> CO + CH2(S)
  {
    double forward = 1.1e+13;
    double reverse = 4.061e+07 * exp(1.561*vlntemp - 1.854e+04*ortc);
    rr_f[191] = forward * mole_frac[0] * mole_frac[26];
    rr_r[191] = reverse * mole_frac[8];
  }
  // 192)  O + HCCO <=> H + 2 CO
  {
    double forward = 8.0e+13;
    double reverse = 0.0;
    rr_f[192] = forward * mole_frac[2] * mole_frac[26];
    rr_r[192] = reverse * mole_frac[0] * mole_frac[8] * mole_frac[8];
  }
  // 193)  O2 + HCCO <=> OH + 2 CO
  {
    double forward = 4.2e+10 * exp(-850.0*ortc);
    double reverse = 0.0;
    rr_f[193] = forward * mole_frac[3] * mole_frac[26];
    rr_r[193] = reverse * mole_frac[4] * mole_frac[8] * mole_frac[8];
  }
  // 194)  H + C2H3 (+M) <=> C2H4 (+M)
  {
    double rr_k0 = 1.4e+30 * exp(-3.86*vlntemp - 3.32e+03*ortc);
    double rr_kinf = 1.36e+14 * exp(0.173*vlntemp - 660.0*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[8];
    double fcent = log10(MAX(0.218 * exp(-4.819277108433735e-03 * temperature) + 
      0.782 * exp(-3.755163349605708e-04 * temperature) + exp(-6.095e+03 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = -cgspl[0] + cgspl[25] - cgspl[26];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[194] = forward * mole_frac[0] * mole_frac[21];
    rr_r[194] = reverse * mole_frac[20];
  }
  // 195)  C2H4 (+M) <=> H2 + C2H2 (+M)
  {
    double rr_k0 = 1.58e+51 * exp(-9.300000000000001*vlntemp - 9.78e+04*ortc);
    double rr_kinf = 8.0e+12 * exp(0.44*vlntemp - 8.877e+04*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[8];
    double fcent = log10(MAX(0.265 * exp(-5.555555555555556e-03 * temperature) + 
      0.735 * exp(-9.66183574879227e-04 * temperature) + exp(-5.417e+03 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = cgspl[1] - cgspl[25] + cgspl[27];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[195] = forward * mole_frac[20];
    rr_r[195] = reverse * mole_frac[1] * mole_frac[22];
  }
  // 196)  H + C2H4 <=> H2 + C2H3
  {
    double forward = 5.07e+07 * exp(1.93*vlntemp - 1.295e+04*ortc);
    double reverse = 1.602e+04 * exp(2.436*vlntemp - 5.19e+03*ortc);
    rr_f[196] = forward * mole_frac[0] * mole_frac[20];
    rr_r[196] = reverse * mole_frac[1] * mole_frac[21];
  }
  // 197)  O + C2H4 <=> HCO + CH3
  {
    double forward = 8.564e+06 * exp(1.88*vlntemp - 183.0*ortc);
    double reverse = 329.7 * exp(2.602*vlntemp - 2.614e+04*ortc);
    rr_f[197] = forward * mole_frac[2] * mole_frac[20];
    rr_r[197] = reverse * mole_frac[11] * mole_frac[17];
  }
  // 198)  O + C2H4 <=> H + CH2CHO
  {
    double forward = 4.986e+06 * exp(1.88*vlntemp - 183.0*ortc);
    double reverse = 1.541e+09 * exp(1.201*vlntemp - 1.878e+04*ortc);
    rr_f[198] = forward * mole_frac[2] * mole_frac[20];
    rr_r[198] = reverse * mole_frac[0] * mole_frac[24];
  }
  // 199)  OH + C2H4 <=> H2O + C2H3
  {
    double forward = 1.8e+06 * temperature * temperature * exp(-2.5e+03*ortc);
    double reverse = 6.029e+03 * exp(2.4*vlntemp - 9.632e+03*ortc);
    rr_f[199] = forward * mole_frac[4] * mole_frac[20];
    rr_r[199] = reverse * mole_frac[5] * mole_frac[21];
  }
  // 200)  CH3 + C2H4 <=> CH4 + C2H3
  {
    double forward = 6.62 * exp(3.7*vlntemp - 9.5e+03*ortc);
    double reverse = 1.908 * exp(3.76*vlntemp - 3.28e+03*ortc);
    rr_f[200] = forward * mole_frac[17] * mole_frac[20];
    rr_r[200] = reverse * mole_frac[16] * mole_frac[21];
  }
  // 201)  O2 + C2H4 <=> HO2 + C2H3
  {
    double forward = 4.0e+13 * exp(-5.82e+04*ortc);
    double reverse = 2.0e+10 * exp(0.158*vlntemp + 4.249e+03*ortc);
    rr_f[201] = forward * mole_frac[3] * mole_frac[20];
    rr_r[201] = reverse * mole_frac[6] * mole_frac[21];
  }
  // 202)  CH3O + C2H4 <=> CH3OH + C2H3
  {
    double forward = 1.2e+11 * exp(-6.75e+03*ortc);
    double xik = cgspl[14] - cgspl[16] - cgspl[25] + cgspl[26];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[202] = forward * mole_frac[20];
    rr_r[202] = reverse * mole_frac[13] * mole_frac[21];
  }
  // 203)  CH3O2 + C2H4 <=> CH3O2H + C2H3
  {
    double forward = 8.59 * exp(3.754*vlntemp - 2.7132e+04*ortc);
    double xik = cgspl[17] - cgspl[18] - cgspl[25] + cgspl[26];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[203] = forward * mole_frac[15] * mole_frac[20];
    rr_r[203] = reverse * mole_frac[14] * mole_frac[21];
  }
  // 204)  C2H4 + C2H5O2 <=> C2H3 + C2H5O2H
  {
    double forward = 8.59 * exp(3.754*vlntemp - 2.7132e+04*ortc);
    double xik = -cgspl[25] + cgspl[26] + cgspl[35] - cgspl[36];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[204] = forward * mole_frac[20] * mole_frac[29];
    rr_r[204] = reverse * mole_frac[21] * mole_frac[28];
  }
  // 205)  CH3 + CH2(S) <=> H + C2H4
  {
    double forward = 2.0e+13;
    double reverse = 6.128e+19 * exp(-1.223*vlntemp - 7.305e+04*ortc);
    rr_f[205] = forward * mole_frac[17];
    rr_r[205] = reverse * mole_frac[0] * mole_frac[20];
  }
  // 206)  H + C2H2 (+M) <=> C2H3 (+M)
  {
    double rr_k0 = 3.8e+40 * exp(-7.27*vlntemp - 7.22e+03*ortc);
    double rr_kinf = 5.6e+12 * exp(-2.4e+03*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[8];
    double fcent = log10(MAX(0.249 * exp(-0.01015228426395939 * temperature) + 
      0.751 * exp(-7.680491551459293e-04 * temperature) + exp(-4.167e+03 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = -cgspl[0] + cgspl[26] - cgspl[27];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[206] = forward * mole_frac[0] * mole_frac[22];
    rr_r[206] = reverse * mole_frac[21];
  }
  // 207)  O2 + C2H3 <=> HO2 + C2H2
  {
    double forward = 2.12e-06 * exp(6.0*vlntemp - 9.484e+03*ortc);
    double reverse = 1.087e-05 * exp(5.905*vlntemp - 2.403e+04*ortc);
    rr_f[207] = forward * mole_frac[3] * mole_frac[21];
    rr_r[207] = reverse * mole_frac[6] * mole_frac[22];
  }
  // 208)  O2 + C2H3 <=> CH2O + HCO
  {
    double forward = 8.5e+28 * exp(-5.312*vlntemp - 6.5e+03*ortc);
    double reverse = 3.994e+27 * exp(-4.883*vlntemp - 9.345e+04*ortc);
    rr_f[208] = forward * mole_frac[3] * mole_frac[21];
    rr_r[208] = reverse * mole_frac[10] * mole_frac[11];
  }
  // 209)  O2 + C2H3 <=> O + CH2CHO
  {
    double forward = 5.5e+14 * exp(-0.611*vlntemp - 5.26e+03*ortc);
    double reverse = 3.0e+18 * exp(-1.386*vlntemp - 1.63e+04*ortc);
    rr_f[209] = forward * mole_frac[3] * mole_frac[21];
    rr_r[209] = reverse * mole_frac[2] * mole_frac[24];
  }
  // 210)  CH3 + C2H3 <=> CH4 + C2H2
  {
    double forward = 3.92e+11;
    double reverse = 3.497e+14 * exp(-0.193*vlntemp - 7.078e+04*ortc);
    rr_f[210] = forward * mole_frac[17] * mole_frac[21];
    rr_r[210] = reverse * mole_frac[16] * mole_frac[22];
  }
  // 211)  H + C2H3 <=> H2 + C2H2
  {
    double forward = 9.64e+13;
    double reverse = 9.427e+13 * exp(0.253*vlntemp - 6.924e+04*ortc);
    rr_f[211] = forward * mole_frac[0] * mole_frac[21];
    rr_r[211] = reverse * mole_frac[1] * mole_frac[22];
  }
  // 212)  OH + C2H3 <=> H2O + C2H2
  {
    double forward = 5.0e+12;
    double reverse = 5.184e+13 * exp(0.147*vlntemp - 8.413e+04*ortc);
    rr_f[212] = forward * mole_frac[4] * mole_frac[21];
    rr_r[212] = reverse * mole_frac[5] * mole_frac[22];
  }
  // 213)  O2 + C2H2 <=> OH + HCCO
  {
    double forward = 2.0e+08 * exp(1.5*vlntemp - 3.01e+04*ortc);
    double reverse = 2.039e+06 * exp(1.541*vlntemp - 3.227e+04*ortc);
    rr_f[213] = forward * mole_frac[3] * mole_frac[22];
    rr_r[213] = reverse * mole_frac[4] * mole_frac[26];
  }
  // 214)  O + C2H2 <=> CO + CH2
  {
    double forward = 6.94e+06 * temperature * temperature * exp(-1.9e+03*ortc);
    double reverse = 40.5 * exp(3.198*vlntemp - 4.836e+04*ortc);
    rr_f[214] = forward * mole_frac[2] * mole_frac[22];
    rr_r[214] = reverse * mole_frac[8];
  }
  // 215)  O + C2H2 <=> H + HCCO
  {
    double forward = 1.35e+07 * temperature * temperature * exp(-1.9e+03*ortc);
    double reverse = 4.755e+07 * exp(1.65*vlntemp - 2.08e+04*ortc);
    rr_f[215] = forward * mole_frac[2] * mole_frac[22];
    rr_r[215] = reverse * mole_frac[0] * mole_frac[26];
  }
  // 216)  OH + C2H2 <=> H + CH2CO
  {
    double forward = 3.236e+13 * exp(-1.2e+04*ortc);
    double reverse = 3.061e+17 * exp(-0.802*vlntemp - 3.579e+04*ortc);
    rr_f[216] = forward * mole_frac[4] * mole_frac[22];
    rr_r[216] = reverse * mole_frac[0] * mole_frac[25];
  }
  // 217)  OH + C2H2 <=> CO + CH3
  {
    double forward = 4.83e-04 * exp(4.0*vlntemp + 2.0e+03*ortc);
    double reverse = 3.495e-06 * exp(4.638*vlntemp - 5.212e+04*ortc);
    rr_f[217] = forward * mole_frac[4] * mole_frac[22];
    rr_r[217] = reverse * mole_frac[8] * mole_frac[17];
  }
  // 218)  CH3COCH3 (+M) <=> CH3 + CH3CO (+M)
  {
    double rr_k0 = 7.013e+89 * exp(-20.38*vlntemp - 1.0715e+05*ortc);
    double rr_kinf = 7.108e+21 * exp(-1.57*vlntemp - 8.468e+04*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[0];
    double fcent = log10(MAX(0.137 * exp(-1.0e-10 * temperature) + 0.863 * 
      exp(-2.401536983669549e-03 * temperature) + exp(-3.29e+09 * otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = cgspl[20] + cgspl[29] - cgspl[38];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[218] = forward * mole_frac[30];
    rr_r[218] = reverse * mole_frac[17];
  }
  // 219)  OH + CH3COCH3 <=> H2O + CH3COCH2
  {
    double forward = 1.25e+05 * exp(2.483*vlntemp - 445.0*ortc);
    double reverse = 8.62e+04 * exp(2.322*vlntemp - 2.471e+04*ortc);
    rr_f[219] = forward * mole_frac[4] * mole_frac[30];
    rr_r[219] = reverse * mole_frac[5];
  }
  // 220)  H + CH3COCH3 <=> H2 + CH3COCH2
  {
    double forward = 9.8e+05 * exp(2.43*vlntemp - 5.16e+03*ortc);
    double reverse = 6.374e+04 * exp(2.375*vlntemp - 1.453e+04*ortc);
    rr_f[220] = forward * mole_frac[0] * mole_frac[30];
    rr_r[220] = reverse * mole_frac[1];
  }
  // 221)  O + CH3COCH3 <=> OH + CH3COCH2
  {
    double forward = 5.13e+11 * exp(0.211*vlntemp - 4.89e+03*ortc);
    double reverse = 1.732e+10 * exp(0.137*vlntemp - 1.285e+04*ortc);
    rr_f[221] = forward * mole_frac[2] * mole_frac[30];
    rr_r[221] = reverse * mole_frac[4];
  }
  // 222)  CH3 + CH3COCH3 <=> CH4 + CH3COCH2
  {
    double forward = 3.96e+11 * exp(-9.784e+03*ortc);
    double reverse = 2.35e+13 * exp(-0.501*vlntemp - 2.069e+04*ortc);
    rr_f[222] = forward * mole_frac[17] * mole_frac[30];
    rr_r[222] = reverse * mole_frac[16];
  }
  // 223)  CH3O + CH3COCH3 <=> CH3OH + CH3COCH2
  {
    double forward = 4.34e+11 * exp(-6.46e+03*ortc);
    double reverse = 6.06e+11 * exp(-0.268*vlntemp - 1.606e+04*ortc);
    rr_f[223] = forward * mole_frac[30];
    rr_r[223] = reverse * mole_frac[13];
  }
  // 224)  O2 + CH3COCH3 <=> HO2 + CH3COCH2
  {
    double forward = 6.03e+13 * exp(-4.85e+04*ortc);
    double reverse = 2.057e+13 * exp(-0.403*vlntemp - 3.181e+03*ortc);
    rr_f[224] = forward * mole_frac[3] * mole_frac[30];
    rr_r[224] = reverse * mole_frac[6];
  }
  // 225)  HO2 + CH3COCH3 <=> H2O2 + CH3COCH2
  {
    double forward = 1.7e+13 * exp(-2.046e+04*ortc);
    double reverse = 6.397e+14 * exp(-0.75*vlntemp - 1.383e+04*ortc);
    rr_f[225] = forward * mole_frac[6] * mole_frac[30];
    rr_r[225] = reverse * mole_frac[7];
  }
  // 226)  CH3O2 + CH3COCH3 <=> CH3O2H + CH3COCH2
  {
    double forward = 1.7e+13 * exp(-2.046e+04*ortc);
    double reverse = 1.245e+16 * exp(-1.195*vlntemp - 1.223e+04*ortc);
    rr_f[226] = forward * mole_frac[15] * mole_frac[30];
    rr_r[226] = reverse * mole_frac[14];
  }
  // 227)  CH3COCH2 <=> CH3 + CH2CO
  {
    double forward = 1.0e+14 * exp(-3.1e+04*ortc);
    double reverse = 1.0e+11 * exp(-6.0e+03*ortc);
    rr_f[227] = forward;
    rr_r[227] = reverse * mole_frac[17] * mole_frac[25];
  }
  // 228)  CH3COCH2O2 <=> O2 + CH3COCH2
  {
    double forward = 2.021e+15 * exp(-0.956*vlntemp - 2.446e+04*ortc);
    double reverse = 1.2e+11 * exp(1.1e+03*ortc);
    rr_f[228] = forward * mole_frac[31];
    rr_r[228] = reverse * mole_frac[3];
  }
  // 229)  CH3COCH3 + CH3COCH2O2 <=> CH3COCH2 + CH3COCH2O2H
  {
    double forward = 1.0e+11 * exp(-5.0e+03*ortc);
    double reverse = 1.995e+10 * exp(-1.0e+04*ortc);
    rr_f[229] = forward * mole_frac[30] * mole_frac[31];
    rr_r[229] = reverse * mole_frac[32];
  }
  // 230)  CH2O + CH3COCH2O2 <=> HCO + CH3COCH2O2H
  {
    double forward = 1.288e+11 * exp(-9.0e+03*ortc);
    double reverse = 2.512e+10 * exp(-1.01e+04*ortc);
    rr_f[230] = forward * mole_frac[10] * mole_frac[31];
    rr_r[230] = reverse * mole_frac[11] * mole_frac[32];
  }
  // 231)  HO2 + CH3COCH2O2 <=> O2 + CH3COCH2O2H
  {
    double forward = 1.0e+12;
    double reverse = 0.0;
    rr_f[231] = forward * mole_frac[6] * mole_frac[31];
    rr_r[231] = reverse * mole_frac[3] * mole_frac[32];
  }
  // 232)  CH3COCH2O2H <=> OH + CH3COCH2O
  {
    double forward = 1.0e+16 * exp(-4.3e+04*ortc);
    double reverse = 1.0e+15 * exp(-0.8 * vlntemp);
    rr_f[232] = forward * mole_frac[32];
    rr_r[232] = reverse * mole_frac[4];
  }
  // 233)  CH3COCH2O <=> CH2O + CH3CO
  {
    double forward = 3.732e+20 * exp(-2.176*vlntemp - 1.726e+04*ortc);
    double reverse = 1.0e+11 * exp(-1.19e+04*ortc);
    rr_f[233] = forward;
    rr_r[233] = reverse * mole_frac[10];
  }
  // 234)  C2H3CHO <=> HCO + C2H3
  {
    double forward = 2.003e+24 * exp(-2.135*vlntemp - 1.034e+05*ortc);
    double reverse = 1.81e+13;
    rr_f[234] = forward * mole_frac[33];
    rr_r[234] = reverse * mole_frac[11] * mole_frac[21];
  }
  // 235)  H + C2H3CHO <=> H2 + C2H3CO
  {
    double forward = 1.34e+13 * exp(-3.3e+03*ortc);
    double reverse = 3.311e+10 * exp(0.613*vlntemp - 2.268e+04*ortc);
    rr_f[235] = forward * mole_frac[0] * mole_frac[33];
    rr_r[235] = reverse * mole_frac[1] * mole_frac[34];
  }
  // 236)  O + C2H3CHO <=> OH + C2H3CO
  {
    double forward = 5.94e+12 * exp(-1.868e+03*ortc);
    double reverse = 7.618e+09 * exp(0.594*vlntemp - 1.984e+04*ortc);
    rr_f[236] = forward * mole_frac[2] * mole_frac[33];
    rr_r[236] = reverse * mole_frac[4] * mole_frac[34];
  }
  // 237)  OH + C2H3CHO <=> H2O + C2H3CO
  {
    double forward = 9.24e+06 * exp(1.5*vlntemp + 962.0*ortc);
    double reverse = 2.42e+05 * exp(2.007*vlntemp - 3.331e+04*ortc);
    rr_f[237] = forward * mole_frac[4] * mole_frac[33];
    rr_r[237] = reverse * mole_frac[5] * mole_frac[34];
  }
  // 238)  O2 + C2H3CHO <=> HO2 + C2H3CO
  {
    double forward = 1.005e+13 * exp(-4.07e+04*ortc);
    double reverse = 1.302e+11 * exp(0.265*vlntemp - 5.391e+03*ortc);
    rr_f[238] = forward * mole_frac[3] * mole_frac[33];
    rr_r[238] = reverse * mole_frac[6] * mole_frac[34];
  }
  // 239)  HO2 + C2H3CHO <=> H2O2 + C2H3CO
  {
    double forward = 3.01e+12 * exp(-1.192e+04*ortc);
    double reverse = 4.303e+12 * exp(-0.082*vlntemp - 1.53e+04*ortc);
    rr_f[239] = forward * mole_frac[6] * mole_frac[33];
    rr_r[239] = reverse * mole_frac[7] * mole_frac[34];
  }
  // 240)  CH3 + C2H3CHO <=> CH4 + C2H3CO
  {
    double forward = 2.608e+06 * exp(1.78*vlntemp - 5.911e+03*ortc);
    double reverse = 5.878e+06 * exp(1.947*vlntemp - 2.683e+04*ortc);
    rr_f[240] = forward * mole_frac[17] * mole_frac[33];
    rr_r[240] = reverse * mole_frac[16] * mole_frac[34];
  }
  // 241)  C2H3 + C2H3CHO <=> C2H4 + C2H3CO
  {
    double forward = 1.74e+12 * exp(-8.44e+03*ortc);
    double reverse = 1.0e+13 * exp(-2.8e+04*ortc);
    rr_f[241] = forward * mole_frac[21] * mole_frac[33];
    rr_r[241] = reverse * mole_frac[20] * mole_frac[34];
  }
  // 242)  CH3O + C2H3CHO <=> CH3OH + C2H3CO
  {
    double forward = 1.0e+12 * exp(-3.3e+03*ortc);
    double reverse = 5.304e+10 * exp(0.401*vlntemp - 2.291e+04*ortc);
    rr_f[242] = forward * mole_frac[33];
    rr_r[242] = reverse * mole_frac[13] * mole_frac[34];
  }
  // 243)  CH3O2 + C2H3CHO <=> CH3O2H + C2H3CO
  {
    double forward = 3.01e+12 * exp(-1.192e+04*ortc);
    double reverse = 8.371e+13 * exp(-0.527*vlntemp - 1.371e+04*ortc);
    rr_f[243] = forward * mole_frac[15] * mole_frac[33];
    rr_r[243] = reverse * mole_frac[14] * mole_frac[34];
  }
  // 244)  C2H3CO <=> CO + C2H3
  {
    double forward = 1.37e+21 * exp(-2.179*vlntemp - 3.941e+04*ortc);
    double reverse = 1.51e+11 * exp(-4.81e+03*ortc);
    rr_f[244] = forward * mole_frac[34];
    rr_r[244] = reverse * mole_frac[8] * mole_frac[21];
  }
  // 245)  C2H5CHO <=> HCO + C2H5
  {
    double forward = 1.496e+27 * exp(-3.205*vlntemp - 8.704e+04*ortc);
    double reverse = 1.81e+13;
    rr_f[245] = forward * mole_frac[35];
    rr_r[245] = reverse * mole_frac[11] * mole_frac[19];
  }
  // 246)  H + C2H5CHO <=> H2 + C2H5CO
  {
    double forward = 4.0e+13 * exp(-4.2e+03*ortc);
    double reverse = 2.377e+10 * exp(0.654*vlntemp - 1.813e+04*ortc);
    rr_f[246] = forward * mole_frac[0] * mole_frac[35];
    rr_r[246] = reverse * mole_frac[1];
  }
  // 247)  O + C2H5CHO <=> OH + C2H5CO
  {
    double forward = 5.0e+12 * exp(-1.79e+03*ortc);
    double reverse = 1.542e+09 * exp(0.636*vlntemp - 1.431e+04*ortc);
    rr_f[247] = forward * mole_frac[2] * mole_frac[35];
    rr_r[247] = reverse * mole_frac[4];
  }
  // 248)  OH + C2H5CHO <=> H2O + C2H5CO
  {
    double forward = 2.69e+10 * exp(0.76*vlntemp + 340.0*ortc);
    double reverse = 1.695e+08 * exp(1.308*vlntemp - 2.848e+04*ortc);
    rr_f[248] = forward * mole_frac[4] * mole_frac[35];
    rr_r[248] = reverse * mole_frac[5];
  }
  // 249)  CH3 + C2H5CHO <=> CH4 + C2H5CO
  {
    double forward = 2.608e+06 * exp(1.78*vlntemp - 5.911e+03*ortc);
    double reverse = 1.414e+06 * exp(1.988*vlntemp - 2.138e+04*ortc);
    rr_f[249] = forward * mole_frac[17] * mole_frac[35];
    rr_r[249] = reverse * mole_frac[16];
  }
  // 250)  HO2 + C2H5CHO <=> H2O2 + C2H5CO
  {
    double forward = 2.8e+12 * exp(-1.36e+04*ortc);
    double reverse = 9.626e+11 * exp(-0.041*vlntemp - 1.153e+04*ortc);
    rr_f[250] = forward * mole_frac[6] * mole_frac[35];
    rr_r[250] = reverse * mole_frac[7];
  }
  // 251)  CH3O + C2H5CHO <=> CH3OH + C2H5CO
  {
    double forward = 1.0e+12 * exp(-3.3e+03*ortc);
    double reverse = 1.276e+10 * exp(0.442*vlntemp - 1.746e+04*ortc);
    rr_f[251] = forward * mole_frac[35];
    rr_r[251] = reverse * mole_frac[13];
  }
  // 252)  CH3O2 + C2H5CHO <=> CH3O2H + C2H5CO
  {
    double forward = 3.01e+12 * exp(-1.192e+04*ortc);
    double reverse = 2.013e+13 * exp(-0.485*vlntemp - 8.26e+03*ortc);
    rr_f[252] = forward * mole_frac[15] * mole_frac[35];
    rr_r[252] = reverse * mole_frac[14];
  }
  // 253)  C2H5 + C2H5CHO <=> C2H6 + C2H5CO
  {
    double forward = 1.0e+12 * exp(-8.0e+03*ortc);
    double reverse = 6.432e+12 * exp(-0.028*vlntemp - 1.97e+04*ortc);
    rr_f[253] = forward * mole_frac[19] * mole_frac[35];
    rr_r[253] = reverse * mole_frac[18];
  }
  // 254)  C2H5O2 + C2H5CHO <=> C2H5O2H + C2H5CO
  {
    double forward = 3.01e+12 * exp(-1.192e+04*ortc);
    double reverse = 2.017e+13 * exp(-0.486*vlntemp - 8.264e+03*ortc);
    rr_f[254] = forward * mole_frac[29] * mole_frac[35];
    rr_r[254] = reverse * mole_frac[28];
  }
  // 255)  O2 + C2H5CHO <=> HO2 + C2H5CO
  {
    double forward = 1.005e+13 * exp(-4.07e+04*ortc);
    double reverse = 3.131e+10 * exp(0.306*vlntemp + 58.0*ortc);
    rr_f[255] = forward * mole_frac[3] * mole_frac[35];
    rr_r[255] = reverse * mole_frac[6];
  }
  // 256)  C2H3 + C2H5CHO <=> C2H4 + C2H5CO
  {
    double forward = 1.7e+12 * exp(-8.44e+03*ortc);
    double reverse = 3.198e+12 * exp(0.148*vlntemp - 3.013e+04*ortc);
    rr_f[256] = forward * mole_frac[21] * mole_frac[35];
    rr_r[256] = reverse * mole_frac[20];
  }
  // 257)  C2H5CO <=> CO + C2H5
  {
    double forward = 2.46e+23 * exp(-3.208*vlntemp - 1.755e+04*ortc);
    double reverse = 1.51e+11 * exp(-4.81e+03*ortc);
    rr_f[257] = forward;
    rr_r[257] = reverse * mole_frac[8] * mole_frac[19];
  }
  // 258)  IC3H7 <=> H + C3H6
  {
    double forward = 6.919e+13 * exp(-0.025*vlntemp - 3.769e+04*ortc);
    double reverse = 2.64e+13 * exp(-2.16e+03*ortc);
    rr_f[258] = forward;
    rr_r[258] = reverse * mole_frac[0] * mole_frac[36];
  }
  // 259)  H + IC3H7 <=> CH3 + C2H5
  {
    double forward = 2.0e+13;
    double reverse = 4.344e+07 * exp(1.176*vlntemp - 8.62e+03*ortc);
    rr_f[259] = forward * mole_frac[0];
    rr_r[259] = reverse * mole_frac[17] * mole_frac[19];
  }
  // 260)  O2 + IC3H7 <=> HO2 + C3H6
  {
    double forward = 4.5e-19 * exp(-5.02e+03*ortc);
    double reverse = 2.0e-19 * exp(-1.75e+04*ortc);
    rr_f[260] = forward * mole_frac[3];
    rr_r[260] = reverse * mole_frac[6] * mole_frac[36];
  }
  // 261)  OH + IC3H7 <=> H2O + C3H6
  {
    double forward = 2.41e+13;
    double reverse = 2.985e+12 * exp(0.57*vlntemp - 8.382e+04*ortc);
    rr_f[261] = forward * mole_frac[4];
    rr_r[261] = reverse * mole_frac[5] * mole_frac[36];
  }
  // 262)  O + IC3H7 <=> H + CH3COCH3
  {
    double forward = 4.818e+13;
    double reverse = 1.293e+16 * exp(-0.19*vlntemp - 7.938e+04*ortc);
    rr_f[262] = forward * mole_frac[2];
    rr_r[262] = reverse * mole_frac[0] * mole_frac[30];
  }
  // 263)  O + IC3H7 <=> CH3 + CH3CHO
  {
    double forward = 4.818e+13;
    double reverse = 1.279e+11 * exp(0.8*vlntemp - 8.648e+04*ortc);
    rr_f[263] = forward * mole_frac[2];
    rr_r[263] = reverse * mole_frac[17] * mole_frac[23];
  }
  // 264)  NC3H7 <=> CH3 + C2H4
  {
    double forward = 9.24e+10 * exp(0.87*vlntemp - 3.05e+04*ortc);
    double xik = cgspl[20] + cgspl[25] - cgspl[48];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[264] = forward;
    rr_r[264] = reverse * mole_frac[17] * mole_frac[20];
  }
  // 265)  NC3H7 <=> H + C3H6
  {
    double forward = 4.14e+12 * exp(0.17*vlntemp - 3.56e+04*ortc);
    double xik = cgspl[0] - cgspl[48] + cgspl[49];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[265] = forward;
    rr_r[265] = reverse * mole_frac[0] * mole_frac[36];
  }
  // 266)  O2 + NC3H7 <=> HO2 + C3H6
  {
    double forward = 3.0e-19 * exp(-3.0e+03*ortc);
    double reverse = 2.0e-19 * exp(-1.75e+04*ortc);
    rr_f[266] = forward * mole_frac[3];
    rr_r[266] = reverse * mole_frac[6] * mole_frac[36];
  }
  // 267)  C2H5CHO + C3H5-A <=> C2H5CO + C3H6
  {
    double forward = 1.7e+12 * exp(-8.44e+03*ortc);
    double reverse = 1.0e+13 * exp(-2.8e+04*ortc);
    rr_f[267] = forward * mole_frac[35] * mole_frac[37];
    rr_r[267] = reverse * mole_frac[36];
  }
  // 268)  C3H6 <=> CH3 + C2H3
  {
    double forward = 2.73e+62 * exp(-13.28*vlntemp - 1.232e+05*ortc);
    double reverse = 6.822e+53 * exp(-11.779*vlntemp - 2.055e+04*ortc);
    rr_f[268] = forward * mole_frac[36];
    rr_r[268] = reverse * mole_frac[17] * mole_frac[21];
  }
  // 269)  C3H6 <=> H + C3H5-A
  {
    double forward = 2.01e+61 * exp(-13.26*vlntemp - 1.185e+05*ortc);
    double reverse = 2.041e+61 * exp(-13.52*vlntemp - 3.061e+04*ortc);
    rr_f[269] = forward * mole_frac[36];
    rr_r[269] = reverse * mole_frac[0] * mole_frac[37];
  }
  // 270)  C3H6 <=> H + C3H5-S
  {
    double forward = 7.709999999999999e+69 * exp(-16.09*vlntemp - 1.4e+05*ortc);
    double reverse = 2.551e+67 * exp(-15.867*vlntemp - 2.869e+04*ortc);
    rr_f[270] = forward * mole_frac[36];
    rr_r[270] = reverse * mole_frac[0];
  }
  // 271)  C3H6 <=> H + C3H5-T
  {
    double forward = 5.62e+71 * exp(-16.58*vlntemp - 1.393e+05*ortc);
    double reverse = 4.26e+68 * exp(-16.164*vlntemp - 3.008e+04*ortc);
    rr_f[271] = forward * mole_frac[36];
    rr_r[271] = reverse * mole_frac[0];
  }
  // 272)  O + C3H6 <=> HCO + C2H5
  {
    double forward = 1.58e+07 * exp(1.76*vlntemp + 1.216e+03*ortc);
    double reverse = 91.88 * exp(2.725*vlntemp - 2.311e+04*ortc);
    rr_f[272] = forward * mole_frac[2] * mole_frac[36];
    rr_r[272] = reverse * mole_frac[11] * mole_frac[19];
  }
  // 273)  O + C3H6 <=> H + CH3 + CH2CO
  {
    double forward = 2.5e+07 * exp(1.76*vlntemp - 76.0*ortc);
    double reverse = 0.0;
    rr_f[273] = forward * mole_frac[2] * mole_frac[36];
    rr_r[273] = reverse * mole_frac[0] * mole_frac[17] * mole_frac[25];
  }
  // 274)  O + C3H6 => 2 H + CH3CHCO
  {
    double forward = 2.5e+07 * exp(1.76*vlntemp - 76.0*ortc);
    rr_f[274] = forward * mole_frac[2] * mole_frac[36];
    rr_r[274] = 0.0;
  }
  // 275)  O + C3H6 <=> OH + C3H5-A
  {
    double forward = 5.24e+11 * exp(0.7*vlntemp - 5.884e+03*ortc);
    double reverse = 1.104e+11 * exp(0.697*vlntemp - 2.015e+04*ortc);
    rr_f[275] = forward * mole_frac[2] * mole_frac[36];
    rr_r[275] = reverse * mole_frac[4] * mole_frac[37];
  }
  // 276)  O + C3H6 <=> OH + C3H5-S
  {
    double forward = 1.2e+11 * exp(0.7*vlntemp - 8.959e+03*ortc);
    double reverse = 8.239e+07 * exp(1.18*vlntemp + 207.0*ortc);
    rr_f[276] = forward * mole_frac[2] * mole_frac[36];
    rr_r[276] = reverse * mole_frac[4];
  }
  // 277)  O + C3H6 <=> OH + C3H5-T
  {
    double forward = 6.03e+10 * exp(0.7*vlntemp - 7.632e+03*ortc);
    double reverse = 9.483e+06 * exp(1.373*vlntemp - 576.0*ortc);
    rr_f[277] = forward * mole_frac[2] * mole_frac[36];
    rr_r[277] = reverse * mole_frac[4];
  }
  // 278)  OH + C3H6 <=> H2O + C3H5-A
  {
    double forward = 3.12e+06 * temperature * temperature * exp(298.0*ortc);
    double reverse = 1.343e+07 * exp(1.909*vlntemp - 3.027e+04*ortc);
    rr_f[278] = forward * mole_frac[4] * mole_frac[36];
    rr_r[278] = reverse * mole_frac[5] * mole_frac[37];
  }
  // 279)  OH + C3H6 <=> H2O + C3H5-S
  {
    double forward = 2.11e+06 * temperature * temperature * 
      exp(-2.778e+03*ortc); 
    double reverse = 2.959e+04 * exp(2.393*vlntemp - 9.916e+03*ortc);
    rr_f[279] = forward * mole_frac[4] * mole_frac[36];
    rr_r[279] = reverse * mole_frac[5];
  }
  // 280)  OH + C3H6 <=> H2O + C3H5-T
  {
    double forward = 1.11e+06 * temperature * temperature * 
      exp(-1.451e+03*ortc); 
    double reverse = 3.565e+03 * exp(2.586*vlntemp - 1.07e+04*ortc);
    rr_f[280] = forward * mole_frac[4] * mole_frac[36];
    rr_r[280] = reverse * mole_frac[5];
  }
  // 281)  HO2 + C3H6 <=> H2O2 + C3H5-A
  {
    double forward = 2.7e+04 * exp(2.5*vlntemp - 1.234e+04*ortc);
    double reverse = 6.341e+06 * exp(1.82*vlntemp - 1.201e+04*ortc);
    rr_f[281] = forward * mole_frac[6] * mole_frac[36];
    rr_r[281] = reverse * mole_frac[7] * mole_frac[37];
  }
  // 282)  HO2 + C3H6 <=> H2O2 + C3H5-S
  {
    double forward = 1.8e+04 * exp(2.5*vlntemp - 2.762e+04*ortc);
    double reverse = 1.377e+04 * exp(2.304*vlntemp - 3.864e+03*ortc);
    rr_f[282] = forward * mole_frac[6] * mole_frac[36];
    rr_r[282] = reverse * mole_frac[7];
  }
  // 283)  HO2 + C3H6 <=> H2O2 + C3H5-T
  {
    double forward = 9.0e+03 * exp(2.5*vlntemp - 2.359e+04*ortc);
    double reverse = 1.577e+03 * exp(2.497*vlntemp - 1.941e+03*ortc);
    rr_f[283] = forward * mole_frac[6] * mole_frac[36];
    rr_r[283] = reverse * mole_frac[7];
  }
  // 284)  H + C3H6 <=> H2 + C3H5-A
  {
    double forward = 1.73e+05 * exp(2.5*vlntemp - 2.492e+03*ortc);
    double reverse = 7.023e+04 * exp(2.515*vlntemp - 1.817e+04*ortc);
    rr_f[284] = forward * mole_frac[0] * mole_frac[36];
    rr_r[284] = reverse * mole_frac[1] * mole_frac[37];
  }
  // 285)  H + C3H6 <=> H2 + C3H5-S
  {
    double forward = 8.04e+05 * exp(2.5*vlntemp - 1.228e+04*ortc);
    double reverse = 1.063e+03 * exp(2.999*vlntemp - 4.526e+03*ortc);
    rr_f[285] = forward * mole_frac[0] * mole_frac[36];
    rr_r[285] = reverse * mole_frac[1];
  }
  // 286)  H + C3H6 <=> H2 + C3H5-T
  {
    double forward = 4.05e+05 * exp(2.5*vlntemp - 9.794e+03*ortc);
    double reverse = 122.7 * exp(3.192*vlntemp - 4.15e+03*ortc);
    rr_f[286] = forward * mole_frac[0] * mole_frac[36];
    rr_r[286] = reverse * mole_frac[1];
  }
  // 287)  H + C3H6 <=> CH3 + C2H4
  {
    double forward = 2.3e+13 * exp(-2.547e+03*ortc);
    double reverse = 7.272e+07 * exp(1.271*vlntemp - 1.12e+04*ortc);
    rr_f[287] = forward * mole_frac[0] * mole_frac[36];
    rr_r[287] = reverse * mole_frac[17] * mole_frac[20];
  }
  // 288)  O2 + C3H6 <=> HO2 + C3H5-A
  {
    double forward = 4.0e+12 * exp(-3.99e+04*ortc);
    double reverse = 8.514e+12 * exp(-0.333*vlntemp - 887.0*ortc);
    rr_f[288] = forward * mole_frac[3] * mole_frac[36];
    rr_r[288] = reverse * mole_frac[6] * mole_frac[37];
  }
  // 289)  O2 + C3H6 <=> HO2 + C3H5-S
  {
    double forward = 2.0e+12 * exp(-6.29e+04*ortc);
    double reverse = 1.387e+10 * exp(0.151*vlntemp - 459.0*ortc);
    rr_f[289] = forward * mole_frac[3] * mole_frac[36];
    rr_r[289] = reverse * mole_frac[6];
  }
  // 290)  O2 + C3H6 <=> HO2 + C3H5-T
  {
    double forward = 1.4e+12 * exp(-6.07e+04*ortc);
    double reverse = 2.224e+09 * exp(0.344*vlntemp - 369.0*ortc);
    rr_f[290] = forward * mole_frac[3] * mole_frac[36];
    rr_r[290] = reverse * mole_frac[6];
  }
  // 291)  CH3 + C3H6 <=> CH4 + C3H5-A
  {
    double forward = 2.21 * exp(3.5*vlntemp - 5.675e+03*ortc);
    double reverse = 818.4 * exp(3.07*vlntemp - 2.289e+04*ortc);
    rr_f[291] = forward * mole_frac[17] * mole_frac[36];
    rr_r[291] = reverse * mole_frac[16] * mole_frac[37];
  }
  // 292)  CH3 + C3H6 <=> CH4 + C3H5-S
  {
    double forward = 1.348 * exp(3.5*vlntemp - 1.285e+04*ortc);
    double reverse = 1.626 * exp(3.553*vlntemp - 6.635e+03*ortc);
    rr_f[292] = forward * mole_frac[17] * mole_frac[36];
    rr_r[292] = reverse * mole_frac[16];
  }
  // 293)  CH3 + C3H6 <=> CH4 + C3H5-T
  {
    double forward = 0.84 * exp(3.5*vlntemp - 1.166e+04*ortc);
    double reverse = 0.2322 * exp(3.746*vlntemp - 7.552e+03*ortc);
    rr_f[293] = forward * mole_frac[17] * mole_frac[36];
    rr_r[293] = reverse * mole_frac[16];
  }
  // 294)  C2H5 + C3H6 <=> C2H6 + C3H5-A
  {
    double forward = 1.0e+11 * exp(-9.8e+03*ortc);
    double reverse = 5.369e+05 * exp(1.33*vlntemp - 1.644e+04*ortc);
    rr_f[294] = forward * mole_frac[19] * mole_frac[36];
    rr_r[294] = reverse * mole_frac[18] * mole_frac[37];
  }
  // 295)  CH3O2 + C3H6 <=> CH3O2H + C3H5-A
  {
    double forward = 3.24e+11 * exp(-1.49e+04*ortc);
    double reverse = 2.0e+10 * exp(-1.5e+04*ortc);
    rr_f[295] = forward * mole_frac[15] * mole_frac[36];
    rr_r[295] = reverse * mole_frac[14] * mole_frac[37];
  }
  // 296)  C2H5O2 + C3H6 <=> C2H5O2H + C3H5-A
  {
    double forward = 3.24e+11 * exp(-1.49e+04*ortc);
    double reverse = 2.0e+10 * exp(-1.5e+04*ortc);
    rr_f[296] = forward * mole_frac[29] * mole_frac[36];
    rr_r[296] = reverse * mole_frac[28] * mole_frac[37];
  }
  // 297)  C3H5-A <=> CH3 + C2H2
  {
    double forward = 2.397e+48 * exp(-9.9*vlntemp - 8.208e+04*ortc);
    double reverse = 2.61e+46 * exp(-9.82*vlntemp - 3.695e+04*ortc);
    rr_f[297] = forward * mole_frac[37];
    rr_r[297] = reverse * mole_frac[17] * mole_frac[22];
  }
  // 298)  C3H5-A <=> H + C3H4-A
  {
    double forward = 4.194e+13 * exp(0.216*vlntemp - 6.193e+04*ortc);
    double reverse = 2.4e+11 * exp(0.6899999999999999*vlntemp - 3.007e+03*ortc);
    rr_f[298] = forward * mole_frac[37];
    rr_r[298] = reverse * mole_frac[0] * mole_frac[39];
  }
  // 299)  HO2 + C3H5-A <=> OH + C3H5O
  {
    double forward = 7.0e+12 * exp(1.0e+03*ortc);
    double reverse = 1.605e+12 * exp(0.06*vlntemp - 1.166e+04*ortc);
    rr_f[299] = forward * mole_frac[6] * mole_frac[37];
    rr_r[299] = reverse * mole_frac[4];
  }
  // 300)  CH3O2 + C3H5-A <=> CH3O + C3H5O
  {
    double forward = 7.0e+12 * exp(1.0e+03*ortc);
    double reverse = 1.99e+15 * exp(-0.74*vlntemp - 1.702e+04*ortc);
    rr_f[300] = forward * mole_frac[15] * mole_frac[37];
    rr_r[300] = reverse;
  }
  // 301)  H + C3H5-A <=> H2 + C3H4-A
  {
    double forward = 1.232e+03 * exp(3.035*vlntemp - 2.582e+03*ortc);
    double reverse = 2.818 * exp(3.784*vlntemp - 4.722e+04*ortc);
    rr_f[301] = forward * mole_frac[0] * mole_frac[37];
    rr_r[301] = reverse * mole_frac[1] * mole_frac[39];
  }
  // 302)  CH3 + C3H5-A <=> CH4 + C3H4-A
  {
    double forward = 1.0e+11;
    double reverse = 4.921e+12 * exp(0.05*vlntemp - 4.778e+04*ortc);
    rr_f[302] = forward * mole_frac[17] * mole_frac[37];
    rr_r[302] = reverse * mole_frac[16] * mole_frac[39];
  }
  // 303)  C2H5 + C3H5-A <=> C2H6 + C3H4-A
  {
    double forward = 4.0e+11;
    double reverse = 1.802e+12 * exp(0.05*vlntemp - 4.033e+04*ortc);
    rr_f[303] = forward * mole_frac[19] * mole_frac[37];
    rr_r[303] = reverse * mole_frac[18] * mole_frac[39];
  }
  // 304)  C2H5 + C3H5-A <=> C2H4 + C3H6
  {
    double forward = 4.0e+11;
    double reverse = 6.937e+16 * exp(-1.33*vlntemp - 5.28e+04*ortc);
    rr_f[304] = forward * mole_frac[19] * mole_frac[37];
    rr_r[304] = reverse * mole_frac[20] * mole_frac[36];
  }
  // 305)  C2H3 + C3H5-A <=> C2H4 + C3H4-A
  {
    double forward = 1.0e+12;
    double reverse = 1.624e+13 * exp(0.05*vlntemp - 4.819e+04*ortc);
    rr_f[305] = forward * mole_frac[21] * mole_frac[37];
    rr_r[305] = reverse * mole_frac[20] * mole_frac[39];
  }
  // 306)  C3H6 + C3H4-A <=> 2 C3H5-A
  {
    double forward = 4.749e+08 * exp(0.734*vlntemp - 2.87e+04*ortc);
    double reverse = 8.43e+10 * exp(262.0*ortc);
    rr_f[306] = forward * mole_frac[36] * mole_frac[39];
    rr_r[306] = reverse * mole_frac[37] * mole_frac[37];
  }
  // 307)  O2 + C3H5-A <=> HO2 + C3H4-A
  {
    double forward = 2.18e+21 * exp(-2.85*vlntemp - 3.076e+04*ortc);
    double reverse = 2.614e+19 * exp(-2.449*vlntemp - 2.071e+04*ortc);
    rr_f[307] = forward * mole_frac[3] * mole_frac[37];
    rr_r[307] = reverse * mole_frac[6] * mole_frac[39];
  }
  // 308)  O2 + C3H5-A <=> CH2O + CH2CHO
  {
    double forward = 7.14e+15 * exp(-1.21*vlntemp - 2.105e+04*ortc);
    double reverse = 4.944e+16 * exp(-1.4*vlntemp - 8.862e+04*ortc);
    rr_f[308] = forward * mole_frac[3] * mole_frac[37];
    rr_r[308] = reverse * mole_frac[10] * mole_frac[24];
  }
  // 309)  O2 + C3H5-A <=> OH + C2H3CHO
  {
    double forward = 2.47e+13 * exp(-0.44*vlntemp - 2.302e+04*ortc);
    double reverse = 1.989e+13 * exp(-0.609*vlntemp - 7.514e+04*ortc);
    rr_f[309] = forward * mole_frac[3] * mole_frac[37];
    rr_r[309] = reverse * mole_frac[4] * mole_frac[33];
  }
  // 310)  O2 + C3H5-A <=> OH + CH2O + C2H2
  {
    double forward = 9.72e+29 * exp(-5.71*vlntemp - 2.145e+04*ortc);
    double reverse = 0.0;
    rr_f[310] = forward * mole_frac[3] * mole_frac[37];
    rr_r[310] = reverse * mole_frac[4] * mole_frac[10] * mole_frac[22];
  }
  // 311)  C3H5-S <=> CH3 + C2H2
  {
    double forward = 9.598e+39 * exp(-8.17*vlntemp - 4.203e+04*ortc);
    double reverse = 1.61e+40 * exp(-8.58*vlntemp - 2.033e+04*ortc);
    rr_f[311] = forward;
    rr_r[311] = reverse * mole_frac[17] * mole_frac[22];
  }
  // 312)  C3H5-S <=> H + C3H4-P
  {
    double forward = 4.187e+15 * exp(-0.79*vlntemp - 3.748e+04*ortc);
    double reverse = 5.8e+12 * exp(-3.1e+03*ortc);
    rr_f[312] = forward;
    rr_r[312] = reverse * mole_frac[0] * mole_frac[38];
  }
  // 313)  O2 + C3H5-S <=> HCO + CH3CHO
  {
    double forward = 4.335e+12;
    double reverse = 1.611e+17 * exp(-1.27*vlntemp - 9.653e+04*ortc);
    rr_f[313] = forward * mole_frac[3];
    rr_r[313] = reverse * mole_frac[11] * mole_frac[23];
  }
  // 314)  H + C3H5-S <=> H2 + C3H4-A
  {
    double forward = 3.333e+12;
    double reverse = 7.977e+12 * exp(0.11*vlntemp - 6.886e+04*ortc);
    rr_f[314] = forward * mole_frac[0];
    rr_r[314] = reverse * mole_frac[1] * mole_frac[39];
  }
  // 315)  CH3 + C3H5-S <=> CH4 + C3H4-A
  {
    double forward = 1.0e+11;
    double reverse = 6.253e+12 * exp(0.11*vlntemp - 6.934e+04*ortc);
    rr_f[315] = forward * mole_frac[17];
    rr_r[315] = reverse * mole_frac[16] * mole_frac[39];
  }
  // 316)  C3H5-T <=> CH3 + C2H2
  {
    double forward = 2.163e+40 * exp(-8.31*vlntemp - 4.511e+04*ortc);
    double reverse = 1.61e+40 * exp(-8.58*vlntemp - 2.033e+04*ortc);
    rr_f[316] = forward;
    rr_r[316] = reverse * mole_frac[17] * mole_frac[22];
  }
  // 317)  C3H5-T <=> H + C3H4-A
  {
    double forward = 3.508e+14 * exp(-0.44*vlntemp - 4.089e+04*ortc);
    double reverse = 8.5e+12 * exp(-2.0e+03*ortc);
    rr_f[317] = forward;
    rr_r[317] = reverse * mole_frac[0] * mole_frac[39];
  }
  // 318)  C3H5-T <=> H + C3H4-P
  {
    double forward = 1.075e+15 * exp(-0.6*vlntemp - 3.849e+04*ortc);
    double reverse = 6.5e+12 * exp(-2.0e+03*ortc);
    rr_f[318] = forward;
    rr_r[318] = reverse * mole_frac[0] * mole_frac[38];
  }
  // 319)  O2 + C3H5-T <=> HO2 + C3H4-A
  {
    double forward = 1.89e+30 * exp(-5.59*vlntemp - 1.554e+04*ortc);
    double reverse = 3.037e+31 * exp(-5.865*vlntemp - 2.681e+04*ortc);
    rr_f[319] = forward * mole_frac[3];
    rr_r[319] = reverse * mole_frac[6] * mole_frac[39];
  }
  // 320)  O2 + C3H5-T <=> O + CH3COCH2
  {
    double forward = 3.81e+17 * exp(-1.36*vlntemp - 5.58e+03*ortc);
    double reverse = 2.0e+11 * exp(-1.75e+04*ortc);
    rr_f[320] = forward * mole_frac[3];
    rr_r[320] = reverse * mole_frac[2];
  }
  // 321)  O2 + C3H5-T <=> CH2O + CH3CO
  {
    double forward = 3.71e+25 * exp(-3.96*vlntemp - 7.043e+03*ortc);
    double reverse = 1.872e+27 * exp(-4.43*vlntemp - 1.012e+05*ortc);
    rr_f[321] = forward * mole_frac[3];
    rr_r[321] = reverse * mole_frac[10];
  }
  // 322)  H + C3H5-T <=> H2 + C3H4-P
  {
    double forward = 3.333e+12;
    double reverse = 2.138e+16 * exp(-0.88*vlntemp - 7.105e+04*ortc);
    rr_f[322] = forward * mole_frac[0];
    rr_r[322] = reverse * mole_frac[1] * mole_frac[38];
  }
  // 323)  CH3 + C3H5-T <=> CH4 + C3H4-P
  {
    double forward = 1.0e+11;
    double reverse = 1.676e+16 * exp(-0.88*vlntemp - 7.153e+04*ortc);
    rr_f[323] = forward * mole_frac[17];
    rr_r[323] = reverse * mole_frac[16] * mole_frac[38];
  }
  // 324)  C3H4-A + M <=> H + C3H3 + M
  {
    double forward = 1.143e+17 * exp(-7.0e+04*ortc);
    double reverse = 2.0e+17 * exp(1.0e+03*ortc);
    rr_f[324] = forward * mole_frac[39];
    rr_r[324] = reverse * mole_frac[0] * mole_frac[40];
    rr_f[324] *= thbctemp[0];
    rr_r[324] *= thbctemp[0];
  }
  // 325)  C3H4-A <=> C3H4-P
  {
    double forward = 1.202e+15 * exp(-9.24e+04*ortc);
    double reverse = 3.222e+18 * exp(-0.99*vlntemp - 9.659e+04*ortc);
    rr_f[325] = forward * mole_frac[39];
    rr_r[325] = reverse * mole_frac[38];
  }
  // 326)  O2 + C3H4-A <=> HO2 + C3H3
  {
    double forward = 4.0e+13 * exp(-3.916e+04*ortc);
    double reverse = 3.17e+11 * exp(-0.08599999999999999*vlntemp - 311.0*ortc);
    rr_f[326] = forward * mole_frac[3] * mole_frac[39];
    rr_r[326] = reverse * mole_frac[6] * mole_frac[40];
  }
  // 327)  HO2 + C3H4-A <=> OH + CH2 + CH2CO
  {
    double forward = 4.0e+12 * exp(-1.9e+04*ortc);
    double reverse = 1.0;
    rr_f[327] = forward * mole_frac[6] * mole_frac[39];
    rr_r[327] = reverse * mole_frac[4] * mole_frac[25];
  }
  // 328)  OH + C3H4-A <=> CH3 + CH2CO
  {
    double forward = 3.12e+12 * exp(397.0*ortc);
    double reverse = 1.806e+17 * exp(-1.38*vlntemp - 3.607e+04*ortc);
    rr_f[328] = forward * mole_frac[4] * mole_frac[39];
    rr_r[328] = reverse * mole_frac[17] * mole_frac[25];
  }
  // 329)  OH + C3H4-A <=> H2O + C3H3
  {
    double forward = 1.0e+07 * temperature * temperature * exp(-1.0e+03*ortc);
    double reverse = 1.602e+05 * exp(2.157*vlntemp - 3.173e+04*ortc);
    rr_f[329] = forward * mole_frac[4] * mole_frac[39];
    rr_r[329] = reverse * mole_frac[5] * mole_frac[40];
  }
  // 330)  O + C3H4-A <=> CO + C2H4
  {
    double forward = 7.8e+12 * exp(-1.6e+03*ortc);
    double reverse = 3.269e+08 * exp(1.252*vlntemp - 1.219e+05*ortc);
    rr_f[330] = forward * mole_frac[2] * mole_frac[39];
    rr_r[330] = reverse * mole_frac[8] * mole_frac[20];
  }
  // 331)  O + C3H4-A <=> CH2O + C2H2
  {
    double forward = 3.0e-03 * exp(4.61*vlntemp + 4.243e+03*ortc);
    double reverse = 232.0 * exp(3.23*vlntemp - 8.119e+04*ortc);
    rr_f[331] = forward * mole_frac[2] * mole_frac[39];
    rr_r[331] = reverse * mole_frac[10] * mole_frac[22];
  }
  // 332)  H + C3H4-A <=> H2 + C3H3
  {
    double forward = 2.0e+07 * temperature * temperature * exp(-5.0e+03*ortc);
    double reverse = 3.022e+04 * exp(2.262*vlntemp - 2.084e+04*ortc);
    rr_f[332] = forward * mole_frac[0] * mole_frac[39];
    rr_r[332] = reverse * mole_frac[1] * mole_frac[40];
  }
  // 333)  CH3 + C3H4-A <=> CH4 + C3H3
  {
    double forward = 0.0367 * exp(4.01*vlntemp - 6.83e+03*ortc);
    double reverse = 0.0506 * exp(3.826*vlntemp - 2.421e+04*ortc);
    rr_f[333] = forward * mole_frac[17] * mole_frac[39];
    rr_r[333] = reverse * mole_frac[16] * mole_frac[40];
  }
  // 334)  C3H5-A + C3H4-A <=> C3H6 + C3H3
  {
    double forward = 2.0e+11 * exp(-7.7e+03*ortc);
    double reverse = 2.644e+19 * exp(-2.71*vlntemp - 4.214e+04*ortc);
    rr_f[334] = forward * mole_frac[37] * mole_frac[39];
    rr_r[334] = reverse * mole_frac[36] * mole_frac[40];
  }
  // 335)  C3H4-P + M <=> H + C3H3 + M
  {
    double forward = 1.143e+17 * exp(-7.0e+04*ortc);
    double reverse = 2.0e+17 * exp(1.0e+03*ortc);
    rr_f[335] = forward * mole_frac[38];
    rr_r[335] = reverse * mole_frac[0] * mole_frac[40];
    rr_f[335] *= thbctemp[0];
    rr_r[335] *= thbctemp[0];
  }
  // 336)  O2 + C3H4-P <=> OH + CH2 + HCCO
  {
    double forward = 1.0e+07 * exp(1.5*vlntemp - 3.01e+04*ortc);
    double reverse = 1.0;
    rr_f[336] = forward * mole_frac[3] * mole_frac[38];
    rr_r[336] = reverse * mole_frac[4] * mole_frac[26];
  }
  // 337)  O2 + C3H4-P <=> HO2 + C3H3
  {
    double forward = 2.0e+13 * exp(-4.16e+04*ortc);
    double reverse = 6.371e+11 * exp(-0.208*vlntemp - 1.021e+03*ortc);
    rr_f[337] = forward * mole_frac[3] * mole_frac[38];
    rr_r[337] = reverse * mole_frac[6] * mole_frac[40];
  }
  // 338)  HO2 + C3H4-P <=> OH + CO + C2H4
  {
    double forward = 3.0e+12 * exp(-1.9e+04*ortc);
    double reverse = 1.0;
    rr_f[338] = forward * mole_frac[6] * mole_frac[38];
    rr_r[338] = reverse * mole_frac[4] * mole_frac[8] * mole_frac[20];
  }
  // 339)  OH + C3H4-P <=> H2O + C3H3
  {
    double forward = 1.0e+07 * temperature * temperature * exp(-1.0e+03*ortc);
    double reverse = 6.441e+05 * exp(2.034*vlntemp - 3.0e+04*ortc);
    rr_f[339] = forward * mole_frac[4] * mole_frac[38];
    rr_r[339] = reverse * mole_frac[5] * mole_frac[40];
  }
  // 340)  OH + C3H4-P <=> CH3 + CH2CO
  {
    double forward = 5.0e-04 * exp(4.5*vlntemp + 1.0e+03*ortc);
    double reverse = 0.01079 * exp(4.11*vlntemp - 3.128e+04*ortc);
    rr_f[340] = forward * mole_frac[4] * mole_frac[38];
    rr_r[340] = reverse * mole_frac[17] * mole_frac[25];
  }
  // 341)  O + C3H4-P <=> HCO + C2H3
  {
    double forward = 3.2e+12 * exp(-2.01e+03*ortc);
    double reverse = 2.548e+12 * exp(-0.39*vlntemp - 3.235e+04*ortc);
    rr_f[341] = forward * mole_frac[2] * mole_frac[38];
    rr_r[341] = reverse * mole_frac[11] * mole_frac[21];
  }
  // 342)  O + C3H4-P <=> CH3 + HCCO
  {
    double forward = 9.6e+08 * temperature;
    double reverse = 1.43e+04 * exp(1.793*vlntemp - 2.699e+04*ortc);
    rr_f[342] = forward * mole_frac[2] * mole_frac[38];
    rr_r[342] = reverse * mole_frac[17] * mole_frac[26];
  }
  // 343)  O + C3H4-P => H + CH2 + HCCO
  {
    double forward = 3.2e-19 * exp(-2.01e+03*ortc);
    rr_f[343] = forward * mole_frac[2] * mole_frac[38];
    rr_r[343] = 0.0;
  }
  // 344)  O + C3H4-P <=> OH + C3H3
  {
    double forward = 7.65e+08 * exp(1.5*vlntemp - 8.6e+03*ortc);
    double reverse = 2.177e+08 * exp(1.31*vlntemp - 2.247e+04*ortc);
    rr_f[344] = forward * mole_frac[2] * mole_frac[38];
    rr_r[344] = reverse * mole_frac[4] * mole_frac[40];
  }
  // 345)  H + C3H4-P <=> H2 + C3H3
  {
    double forward = 2.0e+07 * temperature * temperature * exp(-5.0e+03*ortc);
    double reverse = 1.215e+05 * exp(2.14*vlntemp - 1.911e+04*ortc);
    rr_f[345] = forward * mole_frac[0] * mole_frac[38];
    rr_r[345] = reverse * mole_frac[1] * mole_frac[40];
  }
  // 346)  CH3 + C3H4-P <=> CH4 + C3H3
  {
    double forward = 1.5 * exp(3.5*vlntemp - 5.6e+03*ortc);
    double reverse = 8.313000000000001 * exp(3.195*vlntemp - 2.125e+04*ortc);
    rr_f[346] = forward * mole_frac[17] * mole_frac[38];
    rr_r[346] = reverse * mole_frac[16] * mole_frac[40];
  }
  // 347)  C2H3 + C3H4-P <=> C2H4 + C3H3
  {
    double forward = 1.0e+12 * exp(-7.7e+03*ortc);
    double reverse = 9.541e+11 * exp(-0.39*vlntemp - 5.245e+04*ortc);
    rr_f[347] = forward * mole_frac[21] * mole_frac[38];
    rr_r[347] = reverse * mole_frac[20] * mole_frac[40];
  }
  // 348)  C3H5-A + C3H4-P <=> C3H6 + C3H3
  {
    double forward = 1.0e+12 * exp(-7.7e+03*ortc);
    double reverse = 4.931e+16 * exp(-1.73*vlntemp - 3.795e+04*ortc);
    rr_f[348] = forward * mole_frac[37] * mole_frac[38];
    rr_r[348] = reverse * mole_frac[36] * mole_frac[40];
  }
  // 349)  OH + C3H3 <=> H2O + C3H2
  {
    double forward = 1.0e+13;
    double reverse = 1.343e+15 * exp(-1.568e+04*ortc);
    rr_f[349] = forward * mole_frac[4] * mole_frac[40];
    rr_r[349] = reverse * mole_frac[5];
  }
  // 350)  O2 + C3H3 <=> HCO + CH2CO
  {
    double forward = 3.01e+10 * exp(-2.87e+03*ortc);
    double reverse = 4.881e+11 * exp(-5.947e+04*ortc);
    rr_f[350] = forward * mole_frac[3] * mole_frac[40];
    rr_r[350] = reverse * mole_frac[11] * mole_frac[25];
  }
  // 351)  O2 + C3H2 <=> HCO + HCCO
  {
    double forward = 5.0e+13;
    double reverse = 2.326e+14 * exp(-0.214*vlntemp - 7.719e+04*ortc);
    rr_f[351] = forward * mole_frac[3];
    rr_r[351] = reverse * mole_frac[11] * mole_frac[26];
  }
  // 352)  HO2 + C3H4-A <=> OH + CO + C2H4
  {
    double forward = 1.0e+12 * exp(-1.4e+04*ortc);
    double reverse = 1.0;
    rr_f[352] = forward * mole_frac[6] * mole_frac[39];
    rr_r[352] = reverse * mole_frac[4] * mole_frac[8] * mole_frac[20];
  }
  // 353)  HO2 + C3H4-A <=> H2O2 + C3H3
  {
    double forward = 3.0e+13 * exp(-1.4e+04*ortc);
    double reverse = 1.551e+16 * exp(-1.38*vlntemp - 4.4e+04*ortc);
    rr_f[353] = forward * mole_frac[6] * mole_frac[39];
    rr_r[353] = reverse * mole_frac[7] * mole_frac[40];
  }
  // 354)  CH3 + C2H2 <=> H + C3H4-P
  {
    double forward = 4.229e+08 * exp(1.143*vlntemp - 1.209e+04*ortc);
    double reverse = 1.0e+14 * exp(-4.0e+03*ortc);
    rr_f[354] = forward * mole_frac[17] * mole_frac[22];
    rr_r[354] = reverse * mole_frac[0] * mole_frac[38];
  }
  // 355)  CH3 + C2H2 <=> H + C3H4-A
  {
    double forward = 6.74e+19 * exp(-2.08*vlntemp - 3.159e+04*ortc);
    double reverse = 6.407e+25 * exp(-3.345*vlntemp - 2.177e+04*ortc);
    rr_f[355] = forward * mole_frac[17] * mole_frac[22];
    rr_r[355] = reverse * mole_frac[0] * mole_frac[39];
  }
  // 356)  H + C3H3 <=> H2 + C3H2
  {
    double forward = 5.0e+13;
    double reverse = 5.999e+07 * exp(1.365*vlntemp - 4.11e+03*ortc);
    rr_f[356] = forward * mole_frac[0] * mole_frac[40];
    rr_r[356] = reverse * mole_frac[1];
  }
  // 357)  OH + C3H2 <=> HCO + C2H2
  {
    double forward = 5.0e+13;
    double reverse = 2.282e+16 * exp(-0.254*vlntemp - 7.502e+04*ortc);
    rr_f[357] = forward * mole_frac[4];
    rr_r[357] = reverse * mole_frac[11] * mole_frac[22];
  }
  // 358)  O2 + C3H2 <=> H + CO + HCCO
  {
    double forward = 5.0e+13;
    double reverse = 0.0;
    rr_f[358] = forward * mole_frac[3];
    rr_r[358] = reverse * mole_frac[0] * mole_frac[8] * mole_frac[26];
  }
  // 359)  OH + CH3CHCO <=> CO2 + C2H5
  {
    double forward = 1.73e+12 * exp(1.01e+03*ortc);
    double reverse = 0.0;
    rr_f[359] = forward * mole_frac[4] * mole_frac[43];
    rr_r[359] = reverse * mole_frac[9] * mole_frac[19];
  }
  // 360)  H + CH3CHCO <=> CO + C2H5
  {
    double forward = 4.4e+12 * exp(-1.459e+03*ortc);
    double reverse = 0.0;
    rr_f[360] = forward * mole_frac[0] * mole_frac[43];
    rr_r[360] = reverse * mole_frac[8] * mole_frac[19];
  }
  // 361)  O + CH3CHCO <=> CO + CH3CHO
  {
    double forward = 3.2e+12 * exp(437.0*ortc);
    double reverse = 0.0;
    rr_f[361] = forward * mole_frac[2] * mole_frac[43];
    rr_r[361] = reverse * mole_frac[8] * mole_frac[23];
  }
  // 362)  NC3H7O2 <=> O2 + NC3H7
  {
    double forward = 2.4e+20 * exp(-1.616*vlntemp - 3.596e+04*ortc);
    double reverse = 4.52e+12;
    rr_f[362] = forward * mole_frac[41];
    rr_r[362] = reverse * mole_frac[3];
  }
  // 363)  IC3H7O2 <=> O2 + IC3H7
  {
    double forward = 3.132e+22 * exp(-2.167*vlntemp - 3.816e+04*ortc);
    double reverse = 7.54e+12;
    rr_f[363] = forward * mole_frac[42];
    rr_r[363] = reverse * mole_frac[3];
  }
  // 364)  C3H5O <=> H + C2H3CHO
  {
    double forward = 1.0e+14 * exp(-2.91e+04*ortc);
    double reverse = 1.676e+14 * exp(-0.156*vlntemp - 1.969e+04*ortc);
    rr_f[364] = forward;
    rr_r[364] = reverse * mole_frac[0] * mole_frac[33];
  }
  // 365)  C3H5O <=> CH2O + C2H3
  {
    double forward = 1.464e+20 * exp(-1.968*vlntemp - 3.509e+04*ortc);
    double reverse = 1.5e+11 * exp(-1.06e+04*ortc);
    rr_f[365] = forward;
    rr_r[365] = reverse * mole_frac[10] * mole_frac[21];
  }
  // 366)  O2 + C3H5O <=> HO2 + C2H3CHO
  {
    double forward = 1.0e+12 * exp(-6.0e+03*ortc);
    double reverse = 1.288e+11 * exp(-3.2e+04*ortc);
    rr_f[366] = forward * mole_frac[3];
    rr_r[366] = reverse * mole_frac[6] * mole_frac[33];
  }
  // 367)  IC3H7O2 <=> HO2 + C3H6
  {
    double forward = 1.015e+43 * exp(-9.409000000000001*vlntemp - 
      4.149e+04*ortc); 
    double reverse = 1.954e+33 * exp(-7.289*vlntemp - 1.667e+04*ortc);
    rr_f[367] = forward * mole_frac[42];
    rr_r[367] = reverse * mole_frac[6] * mole_frac[36];
  }
  // 368)  NC3H7O2 <=> HO2 + C3H6
  {
    double forward = 5.044e+38 * exp(-8.112*vlntemp - 4.049e+04*ortc);
    double reverse = 1.198e+30 * exp(-6.229*vlntemp - 2.042e+04*ortc);
    rr_f[368] = forward * mole_frac[41];
    rr_r[368] = reverse * mole_frac[6] * mole_frac[36];
  }
  // 369)  PC4H9 <=> C2H5 + C2H4
  {
    double forward = 3.504e+12 * exp(0.463*vlntemp - 2.947e+04*ortc);
    double reverse = 1.32e+04 * exp(2.48*vlntemp - 6.13e+03*ortc);
    rr_f[369] = forward;
    rr_r[369] = reverse * mole_frac[19] * mole_frac[20];
  }
  // 370)  SC4H9 <=> CH3 + C3H6
  {
    double forward = 4.803e+10 * exp(1.044*vlntemp - 3.035e+04*ortc);
    double reverse = 1.76e+04 * exp(2.48*vlntemp - 6.13e+03*ortc);
    rr_f[370] = forward * mole_frac[45];
    rr_r[370] = reverse * mole_frac[17] * mole_frac[36];
  }
  // 371)  PC4H9 <=> H + C4H8-1
  {
    double forward = 2.622e+12 * exp(0.253*vlntemp - 3.57e+04*ortc);
    double reverse = 2.5e+11 * exp(0.51*vlntemp - 2.62e+03*ortc);
    rr_f[371] = forward;
    rr_r[371] = reverse * mole_frac[0] * mole_frac[44];
  }
  // 372)  SC4H9 <=> H + C4H8-1
  {
    double forward = 3.027e+11 * exp(0.591*vlntemp - 3.682e+04*ortc);
    double reverse = 4.24e+11 * exp(0.51*vlntemp - 1.23e+03*ortc);
    rr_f[372] = forward * mole_frac[45];
    rr_r[372] = reverse * mole_frac[0] * mole_frac[44];
  }
  // 373)  O2 + PC4H9 <=> HO2 + C4H8-1
  {
    double forward = 2.0e-18 * exp(-5.0e+03*ortc);
    double reverse = 2.0e-19 * exp(-1.75e+04*ortc);
    rr_f[373] = forward * mole_frac[3];
    rr_r[373] = reverse * mole_frac[6] * mole_frac[44];
  }
  // 374)  O2 + SC4H9 <=> HO2 + C4H8-1
  {
    double forward = 2.0e-18 * exp(-5.0e+03*ortc);
    double reverse = 2.0e-19 * exp(-1.75e+04*ortc);
    rr_f[374] = forward * mole_frac[3] * mole_frac[45];
    rr_r[374] = reverse * mole_frac[6] * mole_frac[44];
  }
  // 375)  C4H8-1 <=> CH3 + C3H5-A
  {
    double forward = 5.081e+19 * exp(-1.256*vlntemp - 7.651e+04*ortc);
    double reverse = 1.35e+13;
    rr_f[375] = forward * mole_frac[44];
    rr_r[375] = reverse * mole_frac[17] * mole_frac[37];
  }
  // 376)  C4H8-1 <=> C2H5 + C2H3
  {
    double forward = 2.882e+23 * exp(-1.99*vlntemp - 1.016e+05*ortc);
    double reverse = 9.0e+12;
    rr_f[376] = forward * mole_frac[44];
    rr_r[376] = reverse * mole_frac[19] * mole_frac[21];
  }
  // 377)  C4H8-1 <=> H + C4H71-3
  {
    double forward = 3.724e+14 * exp(-0.111*vlntemp - 8.52e+04*ortc);
    double reverse = 5.0e+13;
    rr_f[377] = forward * mole_frac[44];
    rr_r[377] = reverse * mole_frac[0] * mole_frac[46];
  }
  // 378)  O2 + C4H8-1 <=> HO2 + C4H71-3
  {
    double forward = 2.0e+13 * exp(-3.719e+04*ortc);
    double reverse = 4.653e+12 * exp(0.07000000000000001*vlntemp + 168.0*ortc);
    rr_f[378] = forward * mole_frac[3] * mole_frac[44];
    rr_r[378] = reverse * mole_frac[6] * mole_frac[46];
  }
  // 379)  H + C4H8-1 <=> H2 + C4H71-3
  {
    double forward = 3.376e+05 * exp(2.36*vlntemp - 207.0*ortc);
    double reverse = 4.323e+06 * exp(2.1*vlntemp - 2.033e+04*ortc);
    rr_f[379] = forward * mole_frac[0] * mole_frac[44];
    rr_r[379] = reverse * mole_frac[1] * mole_frac[46];
  }
  // 380)  OH + C4H8-1 <=> H2O + C4H71-3
  {
    double forward = 2.764e+04 * exp(2.64*vlntemp + 1.919e+03*ortc);
    double reverse = 1.532e+06 * exp(2.38*vlntemp - 3.336e+04*ortc);
    rr_f[380] = forward * mole_frac[4] * mole_frac[44];
    rr_r[380] = reverse * mole_frac[5] * mole_frac[46];
  }
  // 381)  CH3 + C4H8-1 <=> CH4 + C4H71-3
  {
    double forward = 3.69 * exp(3.31*vlntemp - 4.002e+03*ortc);
    double reverse = 1.234e+03 * exp(3.05*vlntemp - 2.461e+04*ortc);
    rr_f[381] = forward * mole_frac[17] * mole_frac[44];
    rr_r[381] = reverse * mole_frac[16] * mole_frac[46];
  }
  // 382)  HO2 + C4H8-1 <=> H2O2 + C4H71-3
  {
    double forward = 4.82e+03 * exp(2.55*vlntemp - 1.053e+04*ortc);
    double reverse = 1.586e+06 * exp(1.96*vlntemp - 1.435e+04*ortc);
    rr_f[382] = forward * mole_frac[6] * mole_frac[44];
    rr_r[382] = reverse * mole_frac[7] * mole_frac[46];
  }
  // 383)  CH3O2 + C4H8-1 <=> CH3O2H + C4H71-3
  {
    double forward = 4.82e+03 * exp(2.55*vlntemp - 1.053e+04*ortc);
    double reverse = 3.303e+06 * exp(1.79*vlntemp - 1.133e+04*ortc);
    rr_f[383] = forward * mole_frac[15] * mole_frac[44];
    rr_r[383] = reverse * mole_frac[14] * mole_frac[46];
  }
  // 384)  CH3O + C4H8-1 <=> CH3OH + C4H71-3
  {
    double forward = 40.0 * exp(2.9*vlntemp - 8.609e+03*ortc);
    double reverse = 247.0 * exp(2.67*vlntemp - 2.7e+04*ortc);
    rr_f[384] = forward * mole_frac[44];
    rr_r[384] = reverse * mole_frac[13] * mole_frac[46];
  }
  // 385)  C3H5-A + C4H8-1 <=> C3H6 + C4H71-3
  {
    double forward = 7.9e+10 * exp(-1.24e+04*ortc);
    double reverse = 1.0e+11 * exp(-1.75e+04*ortc);
    rr_f[385] = forward * mole_frac[37] * mole_frac[44];
    rr_r[385] = reverse * mole_frac[36] * mole_frac[46];
  }
  // 386)  C4H8-1 + C4H6 <=> 2 C4H71-3
  {
    double forward = 2.35e+12 * exp(-4.672e+04*ortc);
    double reverse = 1.6e+12;
    rr_f[386] = forward * mole_frac[44] * mole_frac[47];
    rr_r[386] = reverse * mole_frac[46] * mole_frac[46];
  }
  // 387)  C2H5O2 + C4H8-1 <=> C2H5O2H + C4H71-3
  {
    double forward = 1.4e+12 * exp(-1.49e+04*ortc);
    double reverse = 3.16e+11 * exp(-1.3e+04*ortc);
    rr_f[387] = forward * mole_frac[29] * mole_frac[44];
    rr_r[387] = reverse * mole_frac[28] * mole_frac[46];
  }
  // 388)  C4H71-3 <=> H + C4H6
  {
    double forward = 1.2e+14 * exp(-4.93e+04*ortc);
    double reverse = 4.0e+13 * exp(-1.3e+03*ortc);
    rr_f[388] = forward * mole_frac[46];
    rr_r[388] = reverse * mole_frac[0] * mole_frac[47];
  }
  // 389)  C2H5 + C4H71-3 <=> C2H4 + C4H8-1
  {
    double forward = 2.59e+12 * exp(131.0*ortc);
    double reverse = 1.149e+13 * exp(0.06*vlntemp - 4.944e+04*ortc);
    rr_f[389] = forward * mole_frac[19] * mole_frac[46];
    rr_r[389] = reverse * mole_frac[20] * mole_frac[44];
  }
  // 390)  CH3O + C4H71-3 <=> CH2O + C4H8-1
  {
    double forward = 2.41e+13;
    double reverse = 2.482e+12 * exp(0.28*vlntemp - 6.633e+04*ortc);
    rr_f[390] = forward * mole_frac[46];
    rr_r[390] = reverse * mole_frac[10] * mole_frac[44];
  }
  // 391)  O + C4H71-3 <=> CH3 + C2H3CHO
  {
    double forward = 6.03e+13;
    double reverse = 3.385e+15 * exp(-0.78*vlntemp - 8.163e+04*ortc);
    rr_f[391] = forward * mole_frac[2] * mole_frac[46];
    rr_r[391] = reverse * mole_frac[17] * mole_frac[33];
  }
  // 392)  HO2 + C4H71-3 <=> OH + C4H7O
  {
    double forward = 9.64e+12;
    double reverse = 7.29e+15 * exp(-1.09*vlntemp - 1.553e+04*ortc);
    rr_f[392] = forward * mole_frac[6] * mole_frac[46];
    rr_r[392] = reverse * mole_frac[4];
  }
  // 393)  CH3O2 + C4H71-3 <=> CH3O + C4H7O
  {
    double forward = 9.64e+12;
    double reverse = 7.12e+17 * exp(-1.67*vlntemp - 2.029e+04*ortc);
    rr_f[393] = forward * mole_frac[15] * mole_frac[46];
    rr_r[393] = reverse;
  }
  // 394)  C3H5-A + C4H71-3 <=> C3H6 + C4H6
  {
    double forward = 6.31e+12;
    double reverse = 1.0e+10 * exp(-5.0e+04*ortc);
    rr_f[394] = forward * mole_frac[37] * mole_frac[46];
    rr_r[394] = reverse * mole_frac[36] * mole_frac[47];
  }
  // 395)  O2 + C4H71-3 <=> HO2 + C4H6
  {
    double forward = 1.0e+09;
    double reverse = 1.0e+11 * exp(-1.7e+04*ortc);
    rr_f[395] = forward * mole_frac[3] * mole_frac[46];
    rr_r[395] = reverse * mole_frac[6] * mole_frac[47];
  }
  // 396)  H + C4H71-3 <=> H2 + C4H6
  {
    double forward = 3.16e+13;
    double reverse = 1.066e+13 * exp(-5.681e+04*ortc);
    rr_f[396] = forward * mole_frac[0] * mole_frac[46];
    rr_r[396] = reverse * mole_frac[1] * mole_frac[47];
  }
  // 397)  C2H5 + C4H71-3 <=> C2H6 + C4H6
  {
    double forward = 3.98e+12;
    double reverse = 3.211e+12 * exp(-4.984e+04*ortc);
    rr_f[397] = forward * mole_frac[19] * mole_frac[46];
    rr_r[397] = reverse * mole_frac[18] * mole_frac[47];
  }
  // 398)  C2H3 + C4H71-3 <=> C2H4 + C4H6
  {
    double forward = 3.98e+12;
    double reverse = 1.157e+13 * exp(-5.771e+04*ortc);
    rr_f[398] = forward * mole_frac[21] * mole_frac[46];
    rr_r[398] = reverse * mole_frac[20] * mole_frac[47];
  }
  // 399)  C2H5O2 + C4H71-3 <=> C2H5O + C4H7O
  {
    double forward = 3.8e+12 * exp(1.2e+03*ortc);
    double reverse = 2.0e+10;
    rr_f[399] = forward * mole_frac[29] * mole_frac[46];
    rr_r[399] = reverse;
  }
  // 400)  C4H7O <=> C2H3 + CH3CHO
  {
    double forward = 7.94e+14 * exp(-1.9e+04*ortc);
    double reverse = 1.0e+10 * exp(-2.0e+04*ortc);
    rr_f[400] = forward;
    rr_r[400] = reverse * mole_frac[21] * mole_frac[23];
  }
  // 401)  C4H7O <=> CH3 + C2H3CHO
  {
    double forward = 7.94e+14 * exp(-1.9e+04*ortc);
    double reverse = 1.0e+10 * exp(-2.0e+04*ortc);
    rr_f[401] = forward;
    rr_r[401] = reverse * mole_frac[17] * mole_frac[33];
  }
  // 402)  C4H6 <=> 2 C2H3
  {
    double forward = 4.027e+19 * otc * exp(-9.815e+04*ortc);
    double reverse = 1.26e+13;
    rr_f[402] = forward * mole_frac[47];
    rr_r[402] = reverse * mole_frac[21] * mole_frac[21];
  }
  // 403)  OH + C4H6 <=> C2H5 + CH2CO
  {
    double forward = 1.0e+12;
    double reverse = 3.73e+12 * exp(-3.002e+04*ortc);
    rr_f[403] = forward * mole_frac[4] * mole_frac[47];
    rr_r[403] = reverse * mole_frac[19] * mole_frac[25];
  }
  // 404)  OH + C4H6 <=> CH2O + C3H5-A
  {
    double forward = 1.0e+12;
    double reverse = 3.501e+06 * exp(-7.106e+04*ortc);
    rr_f[404] = forward * mole_frac[4] * mole_frac[47];
    rr_r[404] = reverse * mole_frac[10] * mole_frac[37];
  }
  // 405)  OH + C4H6 <=> C2H3 + CH3CHO
  {
    double forward = 1.0e+12;
    double reverse = 5.437e+11 * exp(-1.855e+04*ortc);
    rr_f[405] = forward * mole_frac[4] * mole_frac[47];
    rr_r[405] = reverse * mole_frac[21] * mole_frac[23];
  }
  // 406)  O + C4H6 <=> C2H4 + CH2CO
  {
    double forward = 1.0e+12;
    double reverse = 6.377e+11 * exp(-9.434e+04*ortc);
    rr_f[406] = forward * mole_frac[2] * mole_frac[47];
    rr_r[406] = reverse * mole_frac[20] * mole_frac[25];
  }
  // 407)  O + C4H6 <=> CH2O + C3H4-A
  {
    double forward = 1.0e+12;
    double reverse = 1.075e+12 * exp(-7.905e+04*ortc);
    rr_f[407] = forward * mole_frac[2] * mole_frac[47];
    rr_r[407] = reverse * mole_frac[10] * mole_frac[39];
  }
  // 408)  C2H4 + C2H3 <=> H + C4H6
  {
    double forward = 5.0e+11 * exp(-7.3e+03*ortc);
    double reverse = 1.0e+13 * exp(-4.7e+03*ortc);
    rr_f[408] = forward * mole_frac[20] * mole_frac[21];
    rr_r[408] = reverse * mole_frac[0] * mole_frac[47];
  }
  // 409)  PC4H9O2 <=> O2 + PC4H9
  {
    double forward = 2.849e+20 * exp(-1.642*vlntemp - 3.593e+04*ortc);
    double reverse = 4.52e+12;
    rr_f[409] = forward * mole_frac[48];
    rr_r[409] = reverse * mole_frac[3];
  }
  // 410)  PC4H9O2 <=> HO2 + C4H8-1
  {
    double forward = 5.044e+38 * exp(-8.109999999999999*vlntemp - 
      4.049e+04*ortc); 
    double reverse = 1.599e+30 * exp(-6.283*vlntemp - 2.035e+04*ortc);
    rr_f[410] = forward * mole_frac[48];
    rr_r[410] = reverse * mole_frac[6] * mole_frac[44];
  }
  // 411)  C2H5COCH2 <=> C2H5 + CH2CO
  {
    double forward = 1.0e+14 * exp(-3.5e+04*ortc);
    double reverse = 1.0e+11;
    rr_f[411] = forward * mole_frac[49];
    rr_r[411] = reverse * mole_frac[19] * mole_frac[25];
  }
  // 412)  O2 + NC3H7CHO <=> HO2 + NC3H7CO
  {
    double forward = 1.2e+05 * exp(2.5*vlntemp - 3.756e+04*ortc);
    double reverse = 1.0e+07 * exp(0.5*vlntemp - 4.0e+03*ortc);
    rr_f[412] = forward * mole_frac[3] * mole_frac[50];
    rr_r[412] = reverse * mole_frac[6];
  }
  // 413)  OH + NC3H7CHO <=> H2O + NC3H7CO
  {
    double forward = 2.0e+06 * exp(1.8*vlntemp + 1.3e+03*ortc);
    double reverse = 1.553e+04 * exp(2.32*vlntemp - 2.805e+04*ortc);
    rr_f[413] = forward * mole_frac[4] * mole_frac[50];
    rr_r[413] = reverse * mole_frac[5];
  }
  // 414)  H + NC3H7CHO <=> H2 + NC3H7CO
  {
    double forward = 4.14e+09 * exp(1.12*vlntemp - 2.32e+03*ortc);
    double reverse = 3.03e+06 * exp(1.746*vlntemp - 1.678e+04*ortc);
    rr_f[414] = forward * mole_frac[0] * mole_frac[50];
    rr_r[414] = reverse * mole_frac[1];
  }
  // 415)  O + NC3H7CHO <=> OH + NC3H7CO
  {
    double forward = 5.94e+12 * exp(-1.868e+03*ortc);
    double reverse = 2.258e+09 * exp(0.607*vlntemp - 1.492e+04*ortc);
    rr_f[415] = forward * mole_frac[2] * mole_frac[50];
    rr_r[415] = reverse * mole_frac[4];
  }
  // 416)  HO2 + NC3H7CHO <=> H2O2 + NC3H7CO
  {
    double forward = 4.09e+04 * exp(2.5*vlntemp - 1.02e+04*ortc);
    double reverse = 1.733e+04 * exp(2.431*vlntemp - 8.662e+03*ortc);
    rr_f[416] = forward * mole_frac[6] * mole_frac[50];
    rr_r[416] = reverse * mole_frac[7];
  }
  // 417)  CH3 + NC3H7CHO <=> CH4 + NC3H7CO
  {
    double forward = 2.89e-03 * exp(4.62*vlntemp - 3.21e+03*ortc);
    double reverse = 1.93e-03 * exp(4.8*vlntemp - 1.921e+04*ortc);
    rr_f[417] = forward * mole_frac[17] * mole_frac[50];
    rr_r[417] = reverse * mole_frac[16];
  }
  // 418)  CH3O + NC3H7CHO <=> CH3OH + NC3H7CO
  {
    double forward = 1.0e+12 * exp(-3.3e+03*ortc);
    double reverse = 1.572e+10 * exp(0.413*vlntemp - 1.799e+04*ortc);
    rr_f[418] = forward * mole_frac[50];
    rr_r[418] = reverse * mole_frac[13];
  }
  // 419)  CH3O2 + NC3H7CHO <=> CH3O2H + NC3H7CO
  {
    double forward = 4.09e+04 * exp(2.5*vlntemp - 1.02e+04*ortc);
    double reverse = 3.371e+05 * exp(1.986*vlntemp - 7.067e+03*ortc);
    rr_f[419] = forward * mole_frac[15] * mole_frac[50];
    rr_r[419] = reverse * mole_frac[14];
  }
  // 420)  NC3H7CO <=> CO + NC3H7
  {
    double forward = 1.0e+11 * exp(-9.6e+03*ortc);
    double reverse = 2.193e+03 * exp(1.763*vlntemp + 1.1e+03*ortc);
    rr_f[420] = forward;
    rr_r[420] = reverse * mole_frac[8];
  }
  // 421)  HO2 + TC4H9 <=> OH + TC4H9O
  {
    double forward = 7.0e+12 * exp(1.0e+03*ortc);
    double reverse = 4.083e+18 * exp(-1.329*vlntemp - 2.865e+04*ortc);
    rr_f[421] = forward * mole_frac[6];
    rr_r[421] = reverse * mole_frac[4];
  }
  // 422)  CH3O2 + TC4H9 <=> CH3O + TC4H9O
  {
    double forward = 7.0e+12 * exp(1.0e+03*ortc);
    double reverse = 2.554e+17 * exp(-1.03*vlntemp - 3.289e+04*ortc);
    rr_f[422] = forward * mole_frac[15];
    rr_r[422] = reverse;
  }
  // 423)  IC4H9 <=> H + IC4H8
  {
    double forward = 3.371e+13 * exp(0.124*vlntemp - 3.366e+04*ortc);
    double reverse = 6.25e+11 * exp(0.51*vlntemp - 2.62e+03*ortc);
    rr_f[423] = forward;
    rr_r[423] = reverse * mole_frac[0] * mole_frac[51];
  }
  // 424)  IC4H9 <=> CH3 + C3H6
  {
    double forward = 9.504e+11 * exp(0.773*vlntemp - 3.07e+04*ortc);
    double reverse = 1.89e+03 * exp(2.67*vlntemp - 6.85e+03*ortc);
    rr_f[424] = forward;
    rr_r[424] = reverse * mole_frac[17] * mole_frac[36];
  }
  // 425)  TC4H9 <=> H + IC4H8
  {
    double forward = 1.128e+12 * exp(0.703*vlntemp - 3.656e+04*ortc);
    double reverse = 1.06e+12 * exp(0.51*vlntemp - 1.23e+03*ortc);
    rr_f[425] = forward;
    rr_r[425] = reverse * mole_frac[0] * mole_frac[51];
  }
  // 426)  O2 + TC4H9 <=> HO2 + IC4H8
  {
    double forward = 0.837 * exp(3.59*vlntemp - 1.196e+04*ortc);
    double reverse = 1.648 * exp(3.325*vlntemp - 2.55e+04*ortc);
    rr_f[426] = forward * mole_frac[3];
    rr_r[426] = reverse * mole_frac[6] * mole_frac[51];
  }
  // 427)  O2 + IC4H9 <=> HO2 + IC4H8
  {
    double forward = 1.07 * exp(3.71*vlntemp - 9.322e+03*ortc);
    double reverse = 0.04158 * exp(4.024*vlntemp - 2.715e+04*ortc);
    rr_f[427] = forward * mole_frac[3];
    rr_r[427] = reverse * mole_frac[6] * mole_frac[51];
  }
  // 428)  IC4H9O2 <=> O2 + IC4H9
  {
    double forward = 6.64e+19 * exp(-1.575*vlntemp - 3.608e+04*ortc);
    double reverse = 2.26e+12;
    rr_f[428] = forward * mole_frac[54];
    rr_r[428] = reverse * mole_frac[3];
  }
  // 429)  TC4H9O2 <=> O2 + TC4H9
  {
    double forward = 3.331e+24 * exp(-2.472*vlntemp - 3.787e+04*ortc);
    double reverse = 1.41e+13;
    rr_f[429] = forward * mole_frac[53];
    rr_r[429] = reverse * mole_frac[3];
  }
  // 430)  CH3O2 + TC4H9O2 <=> O2 + CH3O + TC4H9O
  {
    double forward = 1.4e+16 * exp(-1.61*vlntemp - 1.86e+03*ortc);
    double reverse = 0.0;
    rr_f[430] = forward * mole_frac[15] * mole_frac[53];
    rr_r[430] = reverse * mole_frac[3];
  }
  // 431)  C2H5O2 + TC4H9O2 <=> O2 + C2H5O + TC4H9O
  {
    double forward = 1.4e+16 * exp(-1.61*vlntemp - 1.86e+03*ortc);
    double reverse = 0.0;
    rr_f[431] = forward * mole_frac[29] * mole_frac[53];
    rr_r[431] = reverse * mole_frac[3];
  }
  // 432)  2 TC4H9O2 <=> O2 + 2 TC4H9O
  {
    double forward = 1.4e+16 * exp(-1.61*vlntemp - 1.86e+03*ortc);
    double reverse = 0.0;
    rr_f[432] = forward * mole_frac[53] * mole_frac[53];
    rr_r[432] = reverse * mole_frac[3];
  }
  // 433)  HO2 + TC4H9O2 <=> O2 + OH + TC4H9O
  {
    double forward = 1.4e+16 * exp(-1.61*vlntemp - 1.86e+03*ortc);
    double reverse = 0.0;
    rr_f[433] = forward * mole_frac[6] * mole_frac[53];
    rr_r[433] = reverse * mole_frac[3] * mole_frac[4];
  }
  // 434)  CH3 + TC4H9O2 <=> CH3O + TC4H9O
  {
    double forward = 7.0e+12 * exp(1.0e+03*ortc);
    double reverse = 7.824e+11 * exp(0.229*vlntemp - 2.834e+04*ortc);
    rr_f[434] = forward * mole_frac[17] * mole_frac[53];
    rr_r[434] = reverse;
  }
  // 435)  C2H5 + TC4H9O2 <=> C2H5O + TC4H9O
  {
    double forward = 7.0e+12 * exp(1.0e+03*ortc);
    double reverse = 1.112e+14 * exp(-0.372*vlntemp - 3.075e+04*ortc);
    rr_f[435] = forward * mole_frac[19] * mole_frac[53];
    rr_r[435] = reverse;
  }
  // 436)  TC4H9 + TC4H9O2 <=> 2 TC4H9O
  {
    double forward = 7.0e+12 * exp(1.0e+03*ortc);
    double reverse = 7.373e+16 * exp(-0.978*vlntemp - 3.275e+04*ortc);
    rr_f[436] = forward * mole_frac[53];
    rr_r[436] = reverse;
  }
  // 437)  C3H5-A + TC4H9O2 <=> C3H5O + TC4H9O
  {
    double forward = 7.0e+12 * exp(1.0e+03*ortc);
    double reverse = 2.898e+10 * exp(0.411*vlntemp - 1.576e+04*ortc);
    rr_f[437] = forward * mole_frac[37] * mole_frac[53];
    rr_r[437] = reverse;
  }
  // 438)  C4H71-3 + TC4H9O2 <=> C4H7O + TC4H9O
  {
    double forward = 7.0e+12 * exp(1.0e+03*ortc);
    double reverse = 1.497e+13 * exp(-0.519*vlntemp - 1.781e+04*ortc);
    rr_f[438] = forward * mole_frac[46] * mole_frac[53];
    rr_r[438] = reverse;
  }
  // 439)  IC4H7 + TC4H9O2 <=> TC4H9O + IC4H7O
  {
    double forward = 7.0e+12 * exp(1.0e+03*ortc);
    double reverse = 6.172e+10 * exp(0.401*vlntemp - 1.492e+04*ortc);
    rr_f[439] = forward * mole_frac[52] * mole_frac[53];
    rr_r[439] = reverse;
  }
  // 440)  TC4H9O <=> CH3 + CH3COCH3
  {
    double forward = 9.558e+22 * exp(-2.548*vlntemp - 1.865e+04*ortc);
    double reverse = 1.5e+11 * exp(-1.19e+04*ortc);
    rr_f[440] = forward;
    rr_r[440] = reverse * mole_frac[17] * mole_frac[30];
  }
  // 441)  IC3H7CHO <=> H + TC3H6CHO
  {
    double forward = 2.304e+18 * exp(-0.91*vlntemp - 9.2e+04*ortc);
    double reverse = 2.0e+14;
    rr_f[441] = forward * mole_frac[55];
    rr_r[441] = reverse * mole_frac[0] * mole_frac[56];
  }
  // 442)  IC3H7CHO <=> HCO + IC3H7
  {
    double forward = 1.129e+17 * exp(-0.03*vlntemp - 7.976e+04*ortc);
    double reverse = 1.81e+13;
    rr_f[442] = forward * mole_frac[55];
    rr_r[442] = reverse * mole_frac[11];
  }
  // 443)  HO2 + IC3H7CHO <=> H2O2 + TC3H6CHO
  {
    double forward = 8.0e+10 * exp(-1.192e+04*ortc);
    double reverse = 3.366e+12 * exp(-0.42*vlntemp - 1.105e+04*ortc);
    rr_f[443] = forward * mole_frac[6] * mole_frac[55];
    rr_r[443] = reverse * mole_frac[7] * mole_frac[56];
  }
  // 444)  OH + IC3H7CHO <=> H2O + TC3H6CHO
  {
    double forward = 1.684e+12 * exp(781.0*ortc);
    double reverse = 1.194e+13 * exp(-0.09*vlntemp - 2.981e+04*ortc);
    rr_f[444] = forward * mole_frac[4] * mole_frac[55];
    rr_r[444] = reverse * mole_frac[5] * mole_frac[56];
  }
  // 445)  IC4H9O2 <=> IC4H8O2H-I
  {
    double forward = 7.5e+10 * exp(-2.44e+04*ortc);
    double reverse = 1.815e+11 * exp(-0.507*vlntemp - 8.946e+03*ortc);
    rr_f[445] = forward * mole_frac[54];
    rr_r[445] = reverse;
  }
  // 446)  IC4H9O2 <=> HO2 + IC4H8
  {
    double forward = 2.265e+35 * exp(-7.22*vlntemp - 3.949e+04*ortc);
    double reverse = 2.996e+26 * exp(-5.331*vlntemp - 2.124e+04*ortc);
    rr_f[446] = forward * mole_frac[54];
    rr_r[446] = reverse * mole_frac[6] * mole_frac[51];
  }
  // 447)  TC4H9O2 <=> HO2 + IC4H8
  {
    double forward = 7.612e+42 * exp(-9.41*vlntemp - 4.149e+04*ortc);
    double reverse = 6.344e+31 * exp(-7.203*vlntemp - 1.716e+04*ortc);
    rr_f[447] = forward * mole_frac[53];
    rr_r[447] = reverse * mole_frac[6] * mole_frac[51];
  }
  // 448)  IC4H8OOH-IO2 <=> O2 + IC4H8O2H-I
  {
    double forward = 1.44e+20 * exp(-1.627*vlntemp - 3.569e+04*ortc);
    double reverse = 2.26e+12;
    rr_f[448] = forward * mole_frac[57];
    rr_r[448] = reverse * mole_frac[3];
  }
  // 449)  IC4H8OOH-IO2 <=> OH + IC4KETII
  {
    double forward = 5.0e+10 * exp(-2.14e+04*ortc);
    double reverse = 1.986e+03 * exp(1.455*vlntemp - 4.442e+04*ortc);
    rr_f[449] = forward * mole_frac[57];
    rr_r[449] = reverse * mole_frac[4] * mole_frac[58];
  }
  // 450)  IC4KETII <=> OH + CH2O + C2H5CO
  {
    double forward = 1.5e+16 * exp(-4.2e+04*ortc);
    double reverse = 0.0;
    rr_f[450] = forward * mole_frac[58];
    rr_r[450] = reverse * mole_frac[4] * mole_frac[10];
  }
  // 451)  IC4H8O2H-I <=> OH + CH2O + C3H6
  {
    double forward = 8.451e+15 * exp(-0.68*vlntemp - 2.917e+04*ortc);
    double reverse = 0.0;
    rr_f[451] = forward;
    rr_r[451] = reverse * mole_frac[4] * mole_frac[10] * mole_frac[36];
  }
  // 452)  IC4H8 <=> CH3 + C3H5-T
  {
    double forward = 1.92e+66 * exp(-14.22*vlntemp - 1.281e+05*ortc);
    double reverse = 1.561e+56 * exp(-12.293*vlntemp - 2.61e+04*ortc);
    rr_f[452] = forward * mole_frac[51];
    rr_r[452] = reverse * mole_frac[17];
  }
  // 453)  IC4H8 <=> H + IC4H7
  {
    double forward = 3.07e+55 * exp(-11.49*vlntemp - 1.143e+05*ortc);
    double reverse = 1.428e+55 * exp(-11.738*vlntemp - 2.64e+04*ortc);
    rr_f[453] = forward * mole_frac[51];
    rr_r[453] = reverse * mole_frac[0] * mole_frac[52];
  }
  // 454)  H + IC4H8 <=> CH3 + C3H6
  {
    double forward = 5.68e+33 * exp(-5.72*vlntemp - 2.0e+04*ortc);
    double reverse = 6.093e+26 * exp(-4.209*vlntemp - 2.72e+04*ortc);
    rr_f[454] = forward * mole_frac[0] * mole_frac[51];
    rr_r[454] = reverse * mole_frac[17] * mole_frac[36];
  }
  // 455)  H + IC4H8 <=> H2 + IC4H7
  {
    double forward = 3.4e+05 * exp(2.5*vlntemp - 2.492e+03*ortc);
    double reverse = 6.32e+04 * exp(2.528*vlntemp - 1.816e+04*ortc);
    rr_f[455] = forward * mole_frac[0] * mole_frac[51];
    rr_r[455] = reverse * mole_frac[1] * mole_frac[52];
  }
  // 456)  O + IC4H8 <=> 2 CH3 + CH2CO
  {
    double forward = 3.33e+07 * exp(1.76*vlntemp - 76.0*ortc);
    double reverse = 0.0;
    rr_f[456] = forward * mole_frac[2] * mole_frac[51];
    rr_r[456] = reverse * mole_frac[17] * mole_frac[17] * mole_frac[25];
  }
  // 457)  O + IC4H8 <=> OH + IC4H7
  {
    double forward = 1.206e+11 * exp(0.7*vlntemp - 7.633e+03*ortc);
    double reverse = 1.164e+10 * exp(0.709*vlntemp - 2.189e+04*ortc);
    rr_f[457] = forward * mole_frac[2] * mole_frac[51];
    rr_r[457] = reverse * mole_frac[4] * mole_frac[52];
  }
  // 458)  CH3 + IC4H8 <=> CH4 + IC4H7
  {
    double forward = 4.42 * exp(3.5*vlntemp - 5.675e+03*ortc);
    double reverse = 749.5 * exp(3.082*vlntemp - 2.289e+04*ortc);
    rr_f[458] = forward * mole_frac[17] * mole_frac[51];
    rr_r[458] = reverse * mole_frac[16] * mole_frac[52];
  }
  // 459)  HO2 + IC4H8 <=> H2O2 + IC4H7
  {
    double forward = 1.928e+04 * exp(2.6*vlntemp - 1.391e+04*ortc);
    double reverse = 2.073e+06 * exp(1.933*vlntemp - 1.358e+04*ortc);
    rr_f[459] = forward * mole_frac[6] * mole_frac[51];
    rr_r[459] = reverse * mole_frac[7] * mole_frac[52];
  }
  // 460)  O2 + IC4H8 <=> HO2 + IC4H7
  {
    double forward = 6.0e+12 * exp(-3.99e+04*ortc);
    double reverse = 5.848e+12 * exp(-0.32*vlntemp - 883.0*ortc);
    rr_f[460] = forward * mole_frac[3] * mole_frac[51];
    rr_r[460] = reverse * mole_frac[6] * mole_frac[52];
  }
  // 461)  C3H5-A + IC4H8 <=> C3H6 + IC4H7
  {
    double forward = 7.94e+11 * exp(-2.05e+04*ortc);
    double reverse = 4.4e+20 * exp(-1.33*vlntemp - 6.061e+04*ortc);
    rr_f[461] = forward * mole_frac[37] * mole_frac[51];
    rr_r[461] = reverse * mole_frac[36] * mole_frac[52];
  }
  // 462)  C3H5-S + IC4H8 <=> C3H6 + IC4H7
  {
    double forward = 7.94e+11 * exp(-2.05e+04*ortc);
    double reverse = 5.592e+20 * exp(-1.27*vlntemp - 8.217e+04*ortc);
    rr_f[462] = forward * mole_frac[51];
    rr_r[462] = reverse * mole_frac[36] * mole_frac[52];
  }
  // 463)  C3H5-T + IC4H8 <=> C3H6 + IC4H7
  {
    double forward = 7.94e+11 * exp(-2.05e+04*ortc);
    double reverse = 5.592e+20 * exp(-1.27*vlntemp - 8.017e+04*ortc);
    rr_f[463] = forward * mole_frac[51];
    rr_r[463] = reverse * mole_frac[36] * mole_frac[52];
  }
  // 464)  OH + IC4H8 <=> H2O + IC4H7
  {
    double forward = 5.2e+06 * temperature * temperature * exp(298.0*ortc);
    double reverse = 1.025e+07 * exp(1.922*vlntemp - 3.027e+04*ortc);
    rr_f[464] = forward * mole_frac[4] * mole_frac[51];
    rr_r[464] = reverse * mole_frac[5] * mole_frac[52];
  }
  // 465)  O + IC4H8 <=> HCO + IC3H7
  {
    double forward = 1.58e+07 * exp(1.76*vlntemp + 1.216e+03*ortc);
    double reverse = 4.538 * exp(3.06*vlntemp - 2.169e+04*ortc);
    rr_f[465] = forward * mole_frac[2] * mole_frac[51];
    rr_r[465] = reverse * mole_frac[11];
  }
  // 466)  CH3O2 + IC4H8 <=> CH3O2H + IC4H7
  {
    double forward = 1.928e+04 * exp(2.6*vlntemp - 1.391e+04*ortc);
    double reverse = 4.034e+07 * exp(1.488*vlntemp - 1.199e+04*ortc);
    rr_f[466] = forward * mole_frac[15] * mole_frac[51];
    rr_r[466] = reverse * mole_frac[14] * mole_frac[52];
  }
  // 467)  O2 + IC4H7 <=> OH + IC3H5CHO
  {
    double forward = 2.47e+13 * exp(-0.45*vlntemp - 2.302e+04*ortc);
    double reverse = 3.372e+13 * exp(-0.577*vlntemp - 7.301e+04*ortc);
    rr_f[467] = forward * mole_frac[3] * mole_frac[52];
    rr_r[467] = reverse * mole_frac[4] * mole_frac[60];
  }
  // 468)  O2 + IC4H7 <=> CH2O + CH3COCH2
  {
    double forward = 7.14e+15 * exp(-1.21*vlntemp - 2.105e+04*ortc);
    double reverse = 1.7e+12 * exp(-0.407*vlntemp - 8.825e+04*ortc);
    rr_f[468] = forward * mole_frac[3] * mole_frac[52];
    rr_r[468] = reverse * mole_frac[10];
  }
  // 469)  O2 + IC4H7 <=> OH + CH2O + C3H4-A
  {
    double forward = 7.29e+29 * exp(-5.71*vlntemp - 2.145e+04*ortc);
    double reverse = 0.0;
    rr_f[469] = forward * mole_frac[3] * mole_frac[52];
    rr_r[469] = reverse * mole_frac[4] * mole_frac[10] * mole_frac[39];
  }
  // 470)  O + IC4H7 <=> H + IC3H5CHO
  {
    double forward = 6.03e+13;
    double reverse = 2.844e+16 * exp(-0.519*vlntemp - 6.673e+04*ortc);
    rr_f[470] = forward * mole_frac[2] * mole_frac[52];
    rr_r[470] = reverse * mole_frac[0] * mole_frac[60];
  }
  // 471)  IC4H7 <=> CH3 + C3H4-A
  {
    double forward = 1.23e+47 * exp(-9.74*vlntemp - 7.426e+04*ortc);
    double reverse = 1.649e+38 * exp(-7.768*vlntemp - 2.254e+04*ortc);
    rr_f[471] = forward * mole_frac[52];
    rr_r[471] = reverse * mole_frac[17] * mole_frac[39];
  }
  // 472)  CH3O2 + IC4H7 <=> CH3O + IC4H7O
  {
    double forward = 7.0e+12 * exp(1.0e+03*ortc);
    double reverse = 2.138e+11 * exp(0.349*vlntemp - 1.506e+04*ortc);
    rr_f[472] = forward * mole_frac[15] * mole_frac[52];
    rr_r[472] = reverse;
  }
  // 473)  HO2 + IC4H7 <=> OH + IC4H7O
  {
    double forward = 7.0e+12 * exp(1.0e+03*ortc);
    double reverse = 3.418e+12 * exp(0.05*vlntemp - 1.082e+04*ortc);
    rr_f[473] = forward * mole_frac[6] * mole_frac[52];
    rr_r[473] = reverse * mole_frac[4];
  }
  // 474)  IC4H7O <=> CH2O + C3H5-T
  {
    double forward = 2.925e+21 * exp(-2.391*vlntemp - 3.559e+04*ortc);
    double reverse = 1.0e+11 * exp(-1.26e+04*ortc);
    rr_f[474] = forward;
    rr_r[474] = reverse * mole_frac[10];
  }
  // 475)  IC4H7O <=> IC4H6OH
  {
    double forward = 1.391e+11 * exp(-1.56e+04*ortc);
    double reverse = 4.233e+11 * exp(-0.164*vlntemp - 3.167e+04*ortc);
    rr_f[475] = forward;
    rr_r[475] = reverse * mole_frac[59];
  }
  // 476)  IC4H7O <=> H + IC3H5CHO
  {
    double forward = 5.0e+13 * exp(-2.91e+04*ortc);
    double reverse = 6.67e+13 * exp(-0.105*vlntemp - 1.841e+04*ortc);
    rr_f[476] = forward;
    rr_r[476] = reverse * mole_frac[0] * mole_frac[60];
  }
  // 477)  IC4H6OH <=> CH2OH + C3H4-A
  {
    double forward = 7.244e+19 * exp(-1.859*vlntemp - 5.705e+04*ortc);
    double reverse = 1.0e+11 * exp(-9.2e+03*ortc);
    rr_f[477] = forward * mole_frac[59];
    rr_r[477] = reverse * mole_frac[39];
  }
  // 478)  O2 + IC4H7O <=> HO2 + IC3H5CHO
  {
    double forward = 3.0e+10 * exp(-1.649e+03*ortc);
    double reverse = 6.312e+10 * exp(-0.14*vlntemp - 3.898e+04*ortc);
    rr_f[478] = forward * mole_frac[3];
    rr_r[478] = reverse * mole_frac[6] * mole_frac[60];
  }
  // 479)  HO2 + IC4H7O <=> H2O2 + IC3H5CHO
  {
    double forward = 3.0e+11;
    double reverse = 8.93e+14 * exp(-0.8*vlntemp - 7.85e+04*ortc);
    rr_f[479] = forward * mole_frac[6];
    rr_r[479] = reverse * mole_frac[7] * mole_frac[60];
  }
  // 480)  CH3 + IC4H7O <=> CH4 + IC3H5CHO
  {
    double forward = 2.4e+13;
    double reverse = 7.261e+16 * exp(-0.47*vlntemp - 9.529e+04*ortc);
    rr_f[480] = forward * mole_frac[17];
    rr_r[480] = reverse * mole_frac[16] * mole_frac[60];
  }
  // 481)  O + IC4H7O <=> OH + IC3H5CHO
  {
    double forward = 6.0e+12;
    double reverse = 3.052e+14 * exp(-0.47*vlntemp - 9.272e+04*ortc);
    rr_f[481] = forward * mole_frac[2];
    rr_r[481] = reverse * mole_frac[4] * mole_frac[60];
  }
  // 482)  OH + IC4H7O <=> H2O + IC3H5CHO
  {
    double forward = 1.81e+13;
    double reverse = 9.076e+15 * exp(-0.47*vlntemp - 1.1e+05*ortc);
    rr_f[482] = forward * mole_frac[4];
    rr_r[482] = reverse * mole_frac[5] * mole_frac[60];
  }
  // 483)  H + IC4H7O <=> H2 + IC3H5CHO
  {
    double forward = 1.99e+13;
    double reverse = 2.305e+15 * exp(-0.47*vlntemp - 9.481e+04*ortc);
    rr_f[483] = forward * mole_frac[0];
    rr_r[483] = reverse * mole_frac[1] * mole_frac[60];
  }
  // 484)  OH + IC3H5CHO <=> H2O + IC3H5CO
  {
    double forward = 2.69e+10 * exp(0.76*vlntemp + 340.0*ortc);
    double reverse = 4.4e+10 * exp(0.78*vlntemp - 3.608e+04*ortc);
    rr_f[484] = forward * mole_frac[4] * mole_frac[60];
    rr_r[484] = reverse * mole_frac[5] * mole_frac[61];
  }
  // 485)  HO2 + IC3H5CHO <=> H2O2 + IC3H5CO
  {
    double forward = 1.0e+12 * exp(-1.192e+04*ortc);
    double reverse = 9.709e+12 * exp(-0.31*vlntemp - 1.688e+04*ortc);
    rr_f[485] = forward * mole_frac[6] * mole_frac[60];
    rr_r[485] = reverse * mole_frac[7] * mole_frac[61];
  }
  // 486)  CH3 + IC3H5CHO <=> CH4 + IC3H5CO
  {
    double forward = 3.98e+12 * exp(-8.7e+03*ortc);
    double reverse = 3.928e+13 * exp(0.02*vlntemp - 3.045e+04*ortc);
    rr_f[486] = forward * mole_frac[17] * mole_frac[60];
    rr_r[486] = reverse * mole_frac[16] * mole_frac[61];
  }
  // 487)  O + IC3H5CHO <=> OH + IC3H5CO
  {
    double forward = 7.18e+12 * exp(-1.389e+03*ortc);
    double reverse = 1.191e+12 * exp(0.02*vlntemp - 2.056e+04*ortc);
    rr_f[487] = forward * mole_frac[2] * mole_frac[60];
    rr_r[487] = reverse * mole_frac[4] * mole_frac[61];
  }
  // 488)  O2 + IC3H5CHO <=> HO2 + IC3H5CO
  {
    double forward = 2.0e+13 * exp(-4.07e+04*ortc);
    double reverse = 1.824e+11 * exp(0.311*vlntemp - 5.337e+03*ortc);
    rr_f[488] = forward * mole_frac[3] * mole_frac[60];
    rr_r[488] = reverse * mole_frac[6] * mole_frac[61];
  }
  // 489)  H + IC3H5CHO <=> H2 + IC3H5CO
  {
    double forward = 2.6e+12 * exp(-2.6e+03*ortc);
    double reverse = 9.822e+11 * exp(0.02*vlntemp - 2.387e+04*ortc);
    rr_f[489] = forward * mole_frac[0] * mole_frac[60];
    rr_r[489] = reverse * mole_frac[1] * mole_frac[61];
  }
  // 490)  IC3H5CO <=> CO + C3H5-T
  {
    double forward = 1.278e+20 * exp(-1.89*vlntemp - 3.446e+04*ortc);
    double reverse = 1.51e+11 * exp(-4.809e+03*ortc);
    rr_f[490] = forward * mole_frac[61];
    rr_r[490] = reverse * mole_frac[8];
  }
  // 491)  HO2 + TC3H6CHO <=> OH + TC3H6OCHO
  {
    double forward = 9.64e+12;
    double reverse = 2.018e+17 * exp(-1.2*vlntemp - 2.101e+04*ortc);
    rr_f[491] = forward * mole_frac[6] * mole_frac[56];
    rr_r[491] = reverse * mole_frac[4];
  }
  // 492)  TC3H6OCHO <=> HCO + CH3COCH3
  {
    double forward = 3.98e+13 * exp(-9.7e+03*ortc);
    double reverse = 2.173e+08 * exp(0.8*vlntemp - 1.424e+04*ortc);
    rr_f[492] = forward;
    rr_r[492] = reverse * mole_frac[11] * mole_frac[30];
  }
  // 493)  TC3H6CHO <=> H + IC3H5CHO
  {
    double forward = 1.325e+14 * exp(0.01*vlntemp - 3.934e+04*ortc);
    double reverse = 1.3e+13 * exp(-1.2e+03*ortc);
    rr_f[493] = forward * mole_frac[56];
    rr_r[493] = reverse * mole_frac[0] * mole_frac[60];
  }
  // 494)  H2 + TC3H6CHO <=> H + IC3H7CHO
  {
    double forward = 2.16e+05 * exp(2.38*vlntemp - 1.899e+04*ortc);
    double reverse = 1.319e+05 * exp(2.47*vlntemp - 3.55e+03*ortc);
    rr_f[494] = forward * mole_frac[1] * mole_frac[56];
    rr_r[494] = reverse * mole_frac[0] * mole_frac[55];
  }
  // 495)  IC4H7OOH <=> OH + IC4H7O
  {
    double forward = 6.4e+15 * exp(-4.555e+04*ortc);
    double reverse = 1.0e+11;
    rr_f[495] = forward * mole_frac[62];
    rr_r[495] = reverse * mole_frac[4];
  }
  // 496)  CH2O + TC3H6CHO <=> HCO + IC3H7CHO
  {
    double forward = 2.52e+08 * exp(1.9*vlntemp - 1.819e+04*ortc);
    double reverse = 1.229e+07 * exp(1.99*vlntemp - 1.742e+04*ortc);
    rr_f[496] = forward * mole_frac[10] * mole_frac[56];
    rr_r[496] = reverse * mole_frac[11] * mole_frac[55];
  }
  // 497)  IC4H8 + TC3H6CHO <=> IC4H7 + IC3H7CHO
  {
    double forward = 470.0 * exp(3.3*vlntemp - 1.984e+04*ortc);
    double reverse = 6.613 * exp(3.39*vlntemp - 8.672e+03*ortc);
    rr_f[497] = forward * mole_frac[51] * mole_frac[56];
    rr_r[497] = reverse * mole_frac[52] * mole_frac[55];
  }
  // 498)  TC3H6O2CHO <=> O2 + TC3H6CHO
  {
    double forward = 2.458e+25 * exp(-4.065*vlntemp - 2.708e+04*ortc);
    double reverse = 1.99e+17 * exp(-2.1 * vlntemp);
    rr_f[498] = forward * mole_frac[63];
    rr_r[498] = reverse * mole_frac[3] * mole_frac[56];
  }
  // 499)  O2 + TC3H6CHO <=> HO2 + IC3H5CHO
  {
    double forward = 2.725e-19 * exp(-7.24e+03*ortc);
    double reverse = 1.39e+11 * exp(-0.2*vlntemp - 1.731e+04*ortc);
    rr_f[499] = forward * mole_frac[3] * mole_frac[56];
    rr_r[499] = reverse * mole_frac[6] * mole_frac[60];
  }
  // 500)  O2 + TC3H6CHO <=> OH + CO + CH3COCH3
  {
    double forward = 3.62e-20;
    double reverse = 0.0;
    rr_f[500] = forward * mole_frac[3] * mole_frac[56];
    rr_r[500] = reverse * mole_frac[4] * mole_frac[8] * mole_frac[30];
  }
  // 501)  HO2 + TC3H6CHO <=> O2 + IC3H7CHO
  {
    double forward = 3.675e+12 * exp(-1.31e+03*ortc);
    double reverse = 1.236e+14 * exp(-0.24*vlntemp - 4.335e+04*ortc);
    rr_f[501] = forward * mole_frac[6] * mole_frac[56];
    rr_r[501] = reverse * mole_frac[3] * mole_frac[55];
  }
  // 502)  CH3 + TC3H6CHO <=> CH4 + IC3H5CHO
  {
    double forward = 3.01e+12 * exp(-0.32*vlntemp + 131.0*ortc);
    double reverse = 2.207e+15 * exp(-0.85*vlntemp - 6.79e+04*ortc);
    rr_f[502] = forward * mole_frac[17] * mole_frac[56];
    rr_r[502] = reverse * mole_frac[16] * mole_frac[60];
  }
  // 503)  HO2 + IC4H6OH <=> OH + CH2O + CH2CCH2OH
  {
    double forward = 1.446e+13;
    double reverse = 0.0;
    rr_f[503] = forward * mole_frac[6] * mole_frac[59];
    rr_r[503] = reverse * mole_frac[4] * mole_frac[10];
  }
  // 504)  O2 + CH2CCH2OH <=> CO + CH2O + CH2OH
  {
    double forward = 4.335e+12;
    double reverse = 0.0;
    rr_f[504] = forward * mole_frac[3];
    rr_r[504] = reverse * mole_frac[8] * mole_frac[10];
  }
  // 505)  CH2CCH2OH <=> CH2OH + C2H2
  {
    double forward = 2.163e+40 * exp(-8.31*vlntemp - 4.511e+04*ortc);
    double reverse = 1.61e+40 * exp(-8.58*vlntemp - 2.033e+04*ortc);
    rr_f[505] = forward;
    rr_r[505] = reverse * mole_frac[22];
  }
  // 506)  CH2CCH2OH <=> OH + C3H4-A
  {
    double forward = 6.697e+16 * exp(-1.11*vlntemp - 4.258e+04*ortc);
    double reverse = 8.5e+12 * exp(-2.0e+03*ortc);
    rr_f[506] = forward;
    rr_r[506] = reverse * mole_frac[4] * mole_frac[39];
  }
  // 507)  C5H10-1 <=> C2H5 + C3H5-A
  {
    double forward = 1.0e+16 * exp(-7.1e+04*ortc);
    double xik = cgspl[24] + cgspl[50] - cgspl[91];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[507] = forward * mole_frac[64];
    rr_r[507] = reverse * mole_frac[19] * mole_frac[37];
  }
  // 508)  H + C5H91-3 <=> C5H10-1
  {
    double forward = 1.0e+14;
    double xik = -cgspl[0] + cgspl[91] - cgspl[92];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[508] = forward * mole_frac[0] * mole_frac[65];
    rr_r[508] = reverse * mole_frac[64];
  }
  // 509)  H + C5H10-1 <=> H2 + C5H91-3
  {
    double forward = 3.376e+05 * exp(2.36*vlntemp - 207.0*ortc);
    double xik = -cgspl[0] + cgspl[1] - cgspl[91] + cgspl[92];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[509] = forward * mole_frac[0] * mole_frac[64];
    rr_r[509] = reverse * mole_frac[1] * mole_frac[65];
  }
  // 510)  H + C5H10-1 <=> H2 + C5H91-4
  {
    double forward = 1.3e+06 * exp(2.4*vlntemp - 4.471e+03*ortc);
    double xik = -cgspl[0] + cgspl[1] - cgspl[91] + cgspl[93];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[510] = forward * mole_frac[0] * mole_frac[64];
    rr_r[510] = reverse * mole_frac[1] * mole_frac[66];
  }
  // 511)  O + C5H10-1 <=> OH + C5H91-3
  {
    double forward = 6.6e+05 * exp(2.43*vlntemp - 1.21e+03*ortc);
    double xik = -cgspl[2] + cgspl[4] - cgspl[91] + cgspl[92];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[511] = forward * mole_frac[2] * mole_frac[64];
    rr_r[511] = reverse * mole_frac[4] * mole_frac[65];
  }
  // 512)  O + C5H10-1 <=> OH + C5H91-4
  {
    double forward = 5.51e+05 * exp(2.45*vlntemp - 2.83e+03*ortc);
    double xik = -cgspl[2] + cgspl[4] - cgspl[91] + cgspl[93];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[512] = forward * mole_frac[2] * mole_frac[64];
    rr_r[512] = reverse * mole_frac[4] * mole_frac[66];
  }
  // 513)  OH + C5H10-1 <=> H2O + C5H91-3
  {
    double forward = 2.764e+04 * exp(2.64*vlntemp + 1.919e+03*ortc);
    double xik = -cgspl[4] + cgspl[5] - cgspl[91] + cgspl[92];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[513] = forward * mole_frac[4] * mole_frac[64];
    rr_r[513] = reverse * mole_frac[5] * mole_frac[65];
  }
  // 514)  OH + C5H10-1 <=> H2O + C5H91-4
  {
    double forward = 4.67e+07 * exp(1.61*vlntemp + 35.0*ortc);
    double xik = -cgspl[4] + cgspl[5] - cgspl[91] + cgspl[93];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[514] = forward * mole_frac[4] * mole_frac[64];
    rr_r[514] = reverse * mole_frac[5] * mole_frac[66];
  }
  // 515)  CH3 + C5H10-1 <=> CH4 + C5H91-3
  {
    double forward = 3.69 * exp(3.31*vlntemp - 4.002e+03*ortc);
    double xik = cgspl[19] - cgspl[20] - cgspl[91] + cgspl[92];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[515] = forward * mole_frac[17] * mole_frac[64];
    rr_r[515] = reverse * mole_frac[16] * mole_frac[65];
  }
  // 516)  CH3 + C5H10-1 <=> CH4 + C5H91-4
  {
    double forward = 1.51 * exp(3.46*vlntemp - 5.481e+03*ortc);
    double xik = cgspl[19] - cgspl[20] - cgspl[91] + cgspl[93];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[516] = forward * mole_frac[17] * mole_frac[64];
    rr_r[516] = reverse * mole_frac[16] * mole_frac[66];
  }
  // 517)  O2 + C5H10-1 <=> HO2 + C5H91-3
  {
    double forward = 2.2e+12 * exp(-3.722e+04*ortc);
    double xik = -cgspl[3] + cgspl[6] - cgspl[91] + cgspl[92];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[517] = forward * mole_frac[3] * mole_frac[64];
    rr_r[517] = reverse * mole_frac[6] * mole_frac[65];
  }
  // 518)  O2 + C5H10-1 <=> HO2 + C5H91-4
  {
    double forward = 2.0e+13 * exp(-4.964e+04*ortc);
    double xik = -cgspl[3] + cgspl[6] - cgspl[91] + cgspl[93];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[518] = forward * mole_frac[3] * mole_frac[64];
    rr_r[518] = reverse * mole_frac[6] * mole_frac[66];
  }
  // 519)  HO2 + C5H10-1 <=> H2O2 + C5H91-3
  {
    double forward = 4.82e+03 * exp(2.55*vlntemp - 1.053e+04*ortc);
    double xik = -cgspl[6] + cgspl[7] - cgspl[91] + cgspl[92];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[519] = forward * mole_frac[6] * mole_frac[64];
    rr_r[519] = reverse * mole_frac[7] * mole_frac[65];
  }
  // 520)  HO2 + C5H10-1 <=> H2O2 + C5H91-4
  {
    double forward = 9.64e+03 * exp(2.6*vlntemp - 1.391e+04*ortc);
    double xik = -cgspl[6] + cgspl[7] - cgspl[91] + cgspl[93];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[520] = forward * mole_frac[6] * mole_frac[64];
    rr_r[520] = reverse * mole_frac[7] * mole_frac[66];
  }
  // 521)  CH3O2 + C5H10-1 <=> CH3O2H + C5H91-3
  {
    double forward = 4.82e+03 * exp(2.55*vlntemp - 1.053e+04*ortc);
    double xik = cgspl[17] - cgspl[18] - cgspl[91] + cgspl[92];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[521] = forward * mole_frac[15] * mole_frac[64];
    rr_r[521] = reverse * mole_frac[14] * mole_frac[65];
  }
  // 522)  CH3O2 + C5H10-1 <=> CH3O2H + C5H91-4
  {
    double forward = 9.64e+03 * exp(2.6*vlntemp - 1.391e+04*ortc);
    double xik = cgspl[17] - cgspl[18] - cgspl[91] + cgspl[93];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[522] = forward * mole_frac[15] * mole_frac[64];
    rr_r[522] = reverse * mole_frac[14] * mole_frac[66];
  }
  // 523)  CH3O + C5H10-1 <=> CH3OH + C5H91-3
  {
    double forward = 40.0 * exp(2.9*vlntemp - 8.609e+03*ortc);
    double xik = cgspl[14] - cgspl[16] - cgspl[91] + cgspl[92];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[523] = forward * mole_frac[64];
    rr_r[523] = reverse * mole_frac[13] * mole_frac[65];
  }
  // 524)  CH3O + C5H10-1 <=> CH3OH + C5H91-4
  {
    double forward = 1.45e+11 * exp(-4.571e+03*ortc);
    double xik = cgspl[14] - cgspl[16] - cgspl[91] + cgspl[93];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[524] = forward * mole_frac[64];
    rr_r[524] = reverse * mole_frac[13] * mole_frac[66];
  }
  // 525)  CH3 + C4H6 <=> C5H91-3
  {
    double forward = 1.2e+11 * exp(-7.5e+03*ortc);
    double xik = -cgspl[20] - cgspl[65] + cgspl[92];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[525] = forward * mole_frac[17] * mole_frac[47];
    rr_r[525] = reverse * mole_frac[65];
  }
  // 526)  C2H3 + C3H6 <=> C5H91-4
  {
    double forward = 6.3e+11 * exp(-3.1e+03*ortc);
    double xik = -cgspl[26] - cgspl[49] + cgspl[93];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[526] = forward * mole_frac[21] * mole_frac[36];
    rr_r[526] = reverse * mole_frac[66];
  }
  // 527)  C6H13-1 <=> C2H4 + PC4H9
  {
    double forward = 6.391e+19 * exp(-1.97*vlntemp - 3.064e+04*ortc);
    double reverse = 3.3e+11 * exp(-7.2e+03*ortc);
    rr_f[527] = forward;
    rr_r[527] = reverse * mole_frac[20];
  }
  // 528)  OH + C5H10-1 <=> CH3CHO + NC3H7
  {
    double forward = 1.0e+11 * exp(2.0e+03*ortc);
    double xik = -cgspl[4] + cgspl[28] + cgspl[48] - cgspl[91];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[528] = forward * mole_frac[4] * mole_frac[64];
    rr_r[528] = reverse * mole_frac[23];
  }
  // 529)  C6H13O2-1 <=> O2 + C6H13-1
  {
    double forward = 5.15e+20 * exp(-1.71*vlntemp - 3.579e+04*ortc);
    double reverse = 4.52e+12;
    rr_f[529] = forward * mole_frac[67];
    rr_r[529] = reverse * mole_frac[3];
  }
  // 530)  O2 + NC4H9CHO <=> HO2 + NC4H9CO
  {
    double forward = 2.0e+13 * exp(0.5*vlntemp - 4.22e+04*ortc);
    double reverse = 1.0e+07 * exp(-4.0e+04*ortc);
    rr_f[530] = forward * mole_frac[3] * mole_frac[68];
    rr_r[530] = reverse * mole_frac[6];
  }
  // 531)  OH + NC4H9CHO <=> H2O + NC4H9CO
  {
    double forward = 2.69e+10 * exp(0.76*vlntemp + 340.0*ortc);
    double reverse = 2.143e+10 * exp(0.73*vlntemp - 3.124e+04*ortc);
    rr_f[531] = forward * mole_frac[4] * mole_frac[68];
    rr_r[531] = reverse * mole_frac[5];
  }
  // 532)  H + NC4H9CHO <=> H2 + NC4H9CO
  {
    double forward = 4.0e+13 * exp(-4.2e+03*ortc);
    double reverse = 1.8e+13 * exp(-2.4e+04*ortc);
    rr_f[532] = forward * mole_frac[0] * mole_frac[68];
    rr_r[532] = reverse * mole_frac[1];
  }
  // 533)  O + NC4H9CHO <=> OH + NC4H9CO
  {
    double forward = 5.0e+12 * exp(-1.79e+03*ortc);
    double reverse = 1.0e+12 * exp(-1.9e+04*ortc);
    rr_f[533] = forward * mole_frac[2] * mole_frac[68];
    rr_r[533] = reverse * mole_frac[4];
  }
  // 534)  HO2 + NC4H9CHO <=> H2O2 + NC4H9CO
  {
    double forward = 2.8e+12 * exp(-1.36e+04*ortc);
    double reverse = 1.0e+12 * exp(-1.0e+04*ortc);
    rr_f[534] = forward * mole_frac[6] * mole_frac[68];
    rr_r[534] = reverse * mole_frac[7];
  }
  // 535)  CH3 + NC4H9CHO <=> CH4 + NC4H9CO
  {
    double forward = 1.7e+12 * exp(-8.44e+03*ortc);
    double reverse = 1.5e+13 * exp(-2.8e+04*ortc);
    rr_f[535] = forward * mole_frac[17] * mole_frac[68];
    rr_r[535] = reverse * mole_frac[16];
  }
  // 536)  CH3O + NC4H9CHO <=> CH3OH + NC4H9CO
  {
    double forward = 1.15e+11 * exp(-1.28e+03*ortc);
    double reverse = 3.0e+11 * exp(-1.8e+04*ortc);
    rr_f[536] = forward * mole_frac[68];
    rr_r[536] = reverse * mole_frac[13];
  }
  // 537)  CH3O2 + NC4H9CHO <=> CH3O2H + NC4H9CO
  {
    double forward = 1.0e+12 * exp(-9.5e+03*ortc);
    double reverse = 2.5e+10 * exp(-1.0e+04*ortc);
    rr_f[537] = forward * mole_frac[15] * mole_frac[68];
    rr_r[537] = reverse * mole_frac[14];
  }
  // 538)  NC4H9CO <=> CO + PC4H9
  {
    double forward = 1.0e+11 * exp(-9.6e+03*ortc);
    double reverse = 1.0e+11;
    rr_f[538] = forward;
    rr_r[538] = reverse * mole_frac[8];
  }
  // 539)  NC7H16 <=> H + C7H15-1
  {
    double forward = 2.68e+88 * exp(-21.17*vlntemp - 1.428e+05*ortc);
    double reverse = 7.264e+84 * exp(-20.81*vlntemp - 4.156e+04*ortc);
    rr_f[539] = forward * mole_frac[69];
    rr_r[539] = reverse * mole_frac[0];
  }
  // 540)  NC7H16 <=> H + C7H15-2
  {
    double forward = 1.3e+88 * exp(-21.01*vlntemp - 1.395e+05*ortc);
    double reverse = 2.263e+83 * exp(-20.31*vlntemp - 4.083e+04*ortc);
    rr_f[540] = forward * mole_frac[69];
    rr_r[540] = reverse * mole_frac[0];
  }
  // 541)  NC7H16 <=> H + C7H15-3
  {
    double forward = 1.3e+88 * exp(-21.01*vlntemp - 1.395e+05*ortc);
    double reverse = 2.263e+83 * exp(-20.31*vlntemp - 4.083e+04*ortc);
    rr_f[541] = forward * mole_frac[69];
    rr_r[541] = reverse * mole_frac[0];
  }
  // 542)  NC7H16 <=> H + C7H15-4
  {
    double forward = 6.5e+87 * exp(-21.01*vlntemp - 1.395e+05*ortc);
    double reverse = 2.255e+83 * exp(-20.31*vlntemp - 4.083e+04*ortc);
    rr_f[542] = forward * mole_frac[69];
    rr_r[542] = reverse * mole_frac[0];
  }
  // 543)  NC7H16 (+M) <=> CH3 + C6H13-1 (+M)
  {
    double rr_k0 = 4.963e+42 * exp(-7.78*vlntemp - 4.28e+04*ortc);
    double rr_kinf = 4.325e+24 * exp(-2.12*vlntemp - 8.99e+04*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[0];
    double fcent = log10(MAX(0.108 * exp(-1.0e-10 * temperature) + 0.892 * 
      exp(-0.4488330341113105 * temperature) + exp(-1.798e+09 * otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = cgspl[20] + cgspl[94] - cgspl[98];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[543] = forward * mole_frac[69];
    rr_r[543] = reverse * mole_frac[17];
  }
  // 544)  NC7H16 (+M) <=> NC3H7 + PC4H9 (+M)
  {
    double rr_k0 = 6.509e+48 * exp(-9.57*vlntemp - 4.129e+04*ortc);
    double rr_kinf = 1.362e+26 * exp(-2.53*vlntemp - 8.876e+04*ortc);
    double pr = rr_k0 / rr_kinf * thbctemp[0];
    double fcent = log10(MAX(0.08899999999999997 * exp(-1.0e-10 * temperature) + 
      0.911 * exp(-0.04467875971763024 * temperature) + exp(-5.0e+09 * 
      otc),1e-200)); 
    double flogpr = log10(MAX(pr,1e-200)) - 0.4 - 0.67 * fcent;
    double fdenom = 0.75 - 1.27 * fcent - 0.14 * flogpr;
    double fquan = flogpr / fdenom;
    fquan = fcent / (1.0 + fquan * fquan);
    double forward = rr_kinf * pr/(1.0 + pr) * exp(fquan*DLn10);
    double xik = cgspl[48] + cgspl[62] - cgspl[98];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[544] = forward * mole_frac[69];
    rr_r[544] = reverse;
  }
  // 545)  H + NC7H16 <=> H2 + C7H15-1
  {
    double forward = 1.88e+05 * exp(2.75*vlntemp - 6.28e+03*ortc);
    double reverse = 20.37 * exp(3.38*vlntemp - 8.618e+03*ortc);
    rr_f[545] = forward * mole_frac[0] * mole_frac[69];
    rr_r[545] = reverse * mole_frac[1];
  }
  // 546)  H + NC7H16 <=> H2 + C7H15-2
  {
    double forward = 2.6e+06 * exp(2.4*vlntemp - 4.471e+03*ortc);
    double reverse = 18.07 * exp(3.38*vlntemp - 9.318e+03*ortc);
    rr_f[546] = forward * mole_frac[0] * mole_frac[69];
    rr_r[546] = reverse * mole_frac[1];
  }
  // 547)  H + NC7H16 <=> H2 + C7H15-3
  {
    double forward = 2.6e+06 * exp(2.4*vlntemp - 4.471e+03*ortc);
    double reverse = 18.07 * exp(3.38*vlntemp - 9.318e+03*ortc);
    rr_f[547] = forward * mole_frac[0] * mole_frac[69];
    rr_r[547] = reverse * mole_frac[1];
  }
  // 548)  H + NC7H16 <=> H2 + C7H15-4
  {
    double forward = 1.3e+06 * exp(2.4*vlntemp - 4.471e+03*ortc);
    double reverse = 18.0 * exp(3.38*vlntemp - 9.318e+03*ortc);
    rr_f[548] = forward * mole_frac[0] * mole_frac[69];
    rr_r[548] = reverse * mole_frac[1];
  }
  // 549)  O + NC7H16 <=> OH + C7H15-1
  {
    double forward = 1.93e+05 * exp(2.68*vlntemp - 3.716e+03*ortc);
    double reverse = 10.85 * exp(3.29*vlntemp - 4.642e+03*ortc);
    rr_f[549] = forward * mole_frac[2] * mole_frac[69];
    rr_r[549] = reverse * mole_frac[4];
  }
  // 550)  O + NC7H16 <=> OH + C7H15-2
  {
    double forward = 9.54e+04 * exp(2.71*vlntemp - 2.106e+03*ortc);
    double reverse = 0.3481 * exp(3.67*vlntemp - 5.541e+03*ortc);
    rr_f[550] = forward * mole_frac[2] * mole_frac[69];
    rr_r[550] = reverse * mole_frac[4];
  }
  // 551)  O + NC7H16 <=> OH + C7H15-3
  {
    double forward = 9.54e+04 * exp(2.71*vlntemp - 2.106e+03*ortc);
    double reverse = 0.3481 * exp(3.67*vlntemp - 5.541e+03*ortc);
    rr_f[551] = forward * mole_frac[2] * mole_frac[69];
    rr_r[551] = reverse * mole_frac[4];
  }
  // 552)  O + NC7H16 <=> OH + C7H15-4
  {
    double forward = 4.77e+04 * exp(2.71*vlntemp - 2.106e+03*ortc);
    double reverse = 0.3468 * exp(3.67*vlntemp - 5.541e+03*ortc);
    rr_f[552] = forward * mole_frac[2] * mole_frac[69];
    rr_r[552] = reverse * mole_frac[4];
  }
  // 553)  OH + NC7H16 <=> H2O + C7H15-1
  {
    double forward = 2.57e+07 * exp(1.8*vlntemp - 954.0*ortc);
    double reverse = 2.952e+04 * exp(2.33*vlntemp - 1.818e+04*ortc);
    rr_f[553] = forward * mole_frac[4] * mole_frac[69];
    rr_r[553] = reverse * mole_frac[5];
  }
  // 554)  OH + NC7H16 <=> H2O + C7H15-2
  {
    double forward = 4.9e+06 * temperature * temperature * exp(596.0*ortc);
    double reverse = 362.4 * exp(2.87*vlntemp - 1.914e+04*ortc);
    rr_f[554] = forward * mole_frac[4] * mole_frac[69];
    rr_r[554] = reverse * mole_frac[5];
  }
  // 555)  OH + NC7H16 <=> H2O + C7H15-3
  {
    double forward = 4.9e+06 * temperature * temperature * exp(596.0*ortc);
    double reverse = 362.4 * exp(2.87*vlntemp - 1.914e+04*ortc);
    rr_f[555] = forward * mole_frac[4] * mole_frac[69];
    rr_r[555] = reverse * mole_frac[5];
  }
  // 556)  OH + NC7H16 <=> H2O + C7H15-4
  {
    double forward = 2.45e+06 * temperature * temperature * exp(596.0*ortc);
    double reverse = 361.0 * exp(2.87*vlntemp - 1.914e+04*ortc);
    rr_f[556] = forward * mole_frac[4] * mole_frac[69];
    rr_r[556] = reverse * mole_frac[5];
  }
  // 557)  HO2 + NC7H16 <=> H2O2 + C7H15-1
  {
    double forward = 40.8 * exp(3.59*vlntemp - 1.716e+04*ortc);
    double reverse = 2.557 * exp(3.53*vlntemp - 3.498e+03*ortc);
    rr_f[557] = forward * mole_frac[6] * mole_frac[69];
    rr_r[557] = reverse * mole_frac[7];
  }
  // 558)  HO2 + NC7H16 <=> H2O2 + C7H15-2
  {
    double forward = 126.4 * exp(3.37*vlntemp - 1.372e+04*ortc);
    double reverse = 0.4982 * exp(3.66*vlntemp - 2.562e+03*ortc);
    rr_f[558] = forward * mole_frac[6] * mole_frac[69];
    rr_r[558] = reverse * mole_frac[7];
  }
  // 559)  HO2 + NC7H16 <=> H2O2 + C7H15-3
  {
    double forward = 126.4 * exp(3.37*vlntemp - 1.372e+04*ortc);
    double reverse = 0.4982 * exp(3.66*vlntemp - 2.562e+03*ortc);
    rr_f[559] = forward * mole_frac[6] * mole_frac[69];
    rr_r[559] = reverse * mole_frac[7];
  }
  // 560)  HO2 + NC7H16 <=> H2O2 + C7H15-4
  {
    double forward = 63.2 * exp(3.37*vlntemp - 1.372e+04*ortc);
    double reverse = 0.4952 * exp(3.66*vlntemp - 2.562e+03*ortc);
    rr_f[560] = forward * mole_frac[6] * mole_frac[69];
    rr_r[560] = reverse * mole_frac[7];
  }
  // 561)  CH3 + NC7H16 <=> CH4 + C7H15-1
  {
    double forward = 0.904 * exp(3.65*vlntemp - 7.154e+03*ortc);
    double reverse = 0.08935 * exp(3.84*vlntemp - 1.103e+04*ortc);
    rr_f[561] = forward * mole_frac[17] * mole_frac[69];
    rr_r[561] = reverse * mole_frac[16];
  }
  // 562)  CH3 + NC7H16 <=> CH4 + C7H15-2
  {
    double forward = 5.41e+04 * exp(2.26*vlntemp - 7.287e+03*ortc);
    double reverse = 343.2 * exp(2.79*vlntemp - 1.367e+04*ortc);
    rr_f[562] = forward * mole_frac[17] * mole_frac[69];
    rr_r[562] = reverse * mole_frac[16];
  }
  // 563)  CH3 + NC7H16 <=> CH4 + C7H15-3
  {
    double forward = 5.41e+04 * exp(2.26*vlntemp - 7.287e+03*ortc);
    double reverse = 343.2 * exp(2.79*vlntemp - 1.367e+04*ortc);
    rr_f[563] = forward * mole_frac[17] * mole_frac[69];
    rr_r[563] = reverse * mole_frac[16];
  }
  // 564)  CH3 + NC7H16 <=> CH4 + C7H15-4
  {
    double forward = 2.705e+04 * exp(2.26*vlntemp - 7.287e+03*ortc);
    double reverse = 341.9 * exp(2.79*vlntemp - 1.367e+04*ortc);
    rr_f[564] = forward * mole_frac[17] * mole_frac[69];
    rr_r[564] = reverse * mole_frac[16];
  }
  // 565)  O2 + NC7H16 <=> HO2 + C7H15-1
  {
    double forward = 4.2e+13 * exp(-5.28e+04*ortc);
    double reverse = 2.386e+10 * exp(0.28*vlntemp - 453.0*ortc);
    rr_f[565] = forward * mole_frac[3] * mole_frac[69];
    rr_r[565] = reverse * mole_frac[6];
  }
  // 566)  O2 + NC7H16 <=> HO2 + C7H15-2
  {
    double forward = 2.8e+13 * exp(-5.015e+04*ortc);
    double reverse = 1.0e+09 * exp(0.63*vlntemp - 309.0*ortc);
    rr_f[566] = forward * mole_frac[3] * mole_frac[69];
    rr_r[566] = reverse * mole_frac[6];
  }
  // 567)  O2 + NC7H16 <=> HO2 + C7H15-3
  {
    double forward = 2.8e+13 * exp(-5.015e+04*ortc);
    double reverse = 1.0e+09 * exp(0.63*vlntemp - 309.0*ortc);
    rr_f[567] = forward * mole_frac[3] * mole_frac[69];
    rr_r[567] = reverse * mole_frac[6];
  }
  // 568)  O2 + NC7H16 <=> HO2 + C7H15-4
  {
    double forward = 1.4e+13 * exp(-5.015e+04*ortc);
    double reverse = 9.941e+08 * exp(0.63*vlntemp - 309.0*ortc);
    rr_f[568] = forward * mole_frac[3] * mole_frac[69];
    rr_r[568] = reverse * mole_frac[6];
  }
  // 569)  C2H5 + NC7H16 <=> C2H6 + C7H15-1
  {
    double forward = 1.0e+11 * exp(-1.34e+04*ortc);
    double reverse = 3.2e+11 * exp(-1.23e+04*ortc);
    rr_f[569] = forward * mole_frac[19] * mole_frac[69];
    rr_r[569] = reverse * mole_frac[18];
  }
  // 570)  C2H5 + NC7H16 <=> C2H6 + C7H15-2
  {
    double forward = 1.0e+11 * exp(-1.04e+04*ortc);
    double reverse = 1.0e+11 * exp(-1.29e+04*ortc);
    rr_f[570] = forward * mole_frac[19] * mole_frac[69];
    rr_r[570] = reverse * mole_frac[18];
  }
  // 571)  C2H5 + NC7H16 <=> C2H6 + C7H15-3
  {
    double forward = 1.0e+11 * exp(-1.04e+04*ortc);
    double reverse = 1.0e+11 * exp(-1.29e+04*ortc);
    rr_f[571] = forward * mole_frac[19] * mole_frac[69];
    rr_r[571] = reverse * mole_frac[18];
  }
  // 572)  C2H5 + NC7H16 <=> C2H6 + C7H15-4
  {
    double forward = 5.0e+10 * exp(-1.04e+04*ortc);
    double reverse = 1.0e+11 * exp(-1.29e+04*ortc);
    rr_f[572] = forward * mole_frac[19] * mole_frac[69];
    rr_r[572] = reverse * mole_frac[18];
  }
  // 573)  CH3O + NC7H16 <=> CH3OH + C7H15-1
  {
    double forward = 3.16e+11 * exp(-7.0e+03*ortc);
    double reverse = 1.2e+10 * exp(-9.2e+03*ortc);
    rr_f[573] = forward * mole_frac[69];
    rr_r[573] = reverse * mole_frac[13];
  }
  // 574)  CH3O + NC7H16 <=> CH3OH + C7H15-2
  {
    double forward = 2.19e+11 * exp(-5.0e+03*ortc);
    double reverse = 8.9e+09 * exp(-7.2e+03*ortc);
    rr_f[574] = forward * mole_frac[69];
    rr_r[574] = reverse * mole_frac[13];
  }
  // 575)  CH3O + NC7H16 <=> CH3OH + C7H15-3
  {
    double forward = 2.19e+11 * exp(-5.0e+03*ortc);
    double reverse = 8.9e+09 * exp(-7.2e+03*ortc);
    rr_f[575] = forward * mole_frac[69];
    rr_r[575] = reverse * mole_frac[13];
  }
  // 576)  CH3O + NC7H16 <=> CH3OH + C7H15-4
  {
    double forward = 1.095e+11 * exp(-5.0e+03*ortc);
    double reverse = 8.9e+09 * exp(-7.2e+03*ortc);
    rr_f[576] = forward * mole_frac[69];
    rr_r[576] = reverse * mole_frac[13];
  }
  // 577)  C2H3 + NC7H16 <=> C2H4 + C7H15-1
  {
    double forward = 1.0e+12 * exp(-1.8e+04*ortc);
    double reverse = 2.57e+12 * exp(-2.54e+04*ortc);
    rr_f[577] = forward * mole_frac[21] * mole_frac[69];
    rr_r[577] = reverse * mole_frac[20];
  }
  // 578)  C2H3 + NC7H16 <=> C2H4 + C7H15-2
  {
    double forward = 8.0e+11 * exp(-1.68e+04*ortc);
    double reverse = 2.0e+12 * exp(-2.42e+04*ortc);
    rr_f[578] = forward * mole_frac[21] * mole_frac[69];
    rr_r[578] = reverse * mole_frac[20];
  }
  // 579)  C2H3 + NC7H16 <=> C2H4 + C7H15-3
  {
    double forward = 8.0e+11 * exp(-1.68e+04*ortc);
    double reverse = 2.0e+12 * exp(-2.42e+04*ortc);
    rr_f[579] = forward * mole_frac[21] * mole_frac[69];
    rr_r[579] = reverse * mole_frac[20];
  }
  // 580)  C2H3 + NC7H16 <=> C2H4 + C7H15-4
  {
    double forward = 4.0e+11 * exp(-1.68e+04*ortc);
    double reverse = 2.0e+12 * exp(-2.42e+04*ortc);
    rr_f[580] = forward * mole_frac[21] * mole_frac[69];
    rr_r[580] = reverse * mole_frac[20];
  }
  // 581)  CH3O2 + NC7H16 <=> CH3O2H + C7H15-1
  {
    double forward = 1.386 * exp(3.97*vlntemp - 1.828e+04*ortc);
    double reverse = 1.69 * exp(3.46*vlntemp - 3.024e+03*ortc);
    rr_f[581] = forward * mole_frac[15] * mole_frac[69];
    rr_r[581] = reverse * mole_frac[14];
  }
  // 582)  CH3O2 + NC7H16 <=> CH3O2H + C7H15-2
  {
    double forward = 20.37 * exp(3.58*vlntemp - 1.481e+04*ortc);
    double reverse = 1.562 * exp(3.42*vlntemp - 2.054e+03*ortc);
    rr_f[582] = forward * mole_frac[15] * mole_frac[69];
    rr_r[582] = reverse * mole_frac[14];
  }
  // 583)  CH3O2 + NC7H16 <=> CH3O2H + C7H15-3
  {
    double forward = 20.37 * exp(3.58*vlntemp - 1.481e+04*ortc);
    double reverse = 1.562 * exp(3.42*vlntemp - 2.054e+03*ortc);
    rr_f[583] = forward * mole_frac[15] * mole_frac[69];
    rr_r[583] = reverse * mole_frac[14];
  }
  // 584)  CH3O2 + NC7H16 <=> CH3O2H + C7H15-4
  {
    double forward = 10.19 * exp(3.58*vlntemp - 1.481e+04*ortc);
    double reverse = 1.553 * exp(3.42*vlntemp - 2.054e+03*ortc);
    rr_f[584] = forward * mole_frac[15] * mole_frac[69];
    rr_r[584] = reverse * mole_frac[14];
  }
  // 585)  NC7H16 + C7H15-1 <=> NC7H16 + C7H15-2
  {
    double forward = 1.0e+11 * exp(-1.04e+04*ortc);
    double reverse = 1.5e+11 * exp(-1.23e+04*ortc);
    rr_f[585] = forward * mole_frac[69];
    rr_r[585] = reverse * mole_frac[69];
  }
  // 586)  NC7H16 + C7H15-1 <=> NC7H16 + C7H15-3
  {
    double forward = 1.0e+11 * exp(-1.04e+04*ortc);
    double reverse = 1.5e+11 * exp(-1.23e+04*ortc);
    rr_f[586] = forward * mole_frac[69];
    rr_r[586] = reverse * mole_frac[69];
  }
  // 587)  NC7H16 + C7H15-1 <=> NC7H16 + C7H15-4
  {
    double forward = 5.0e+10 * exp(-1.04e+04*ortc);
    double reverse = 1.5e+11 * exp(-1.23e+04*ortc);
    rr_f[587] = forward * mole_frac[69];
    rr_r[587] = reverse * mole_frac[69];
  }
  // 588)  NC7H16 + C7H15-2 <=> NC7H16 + C7H15-3
  {
    double forward = 1.0e+11 * exp(-1.04e+04*ortc);
    double reverse = 1.0e+11 * exp(-1.04e+04*ortc);
    rr_f[588] = forward * mole_frac[69];
    rr_r[588] = reverse * mole_frac[69];
  }
  // 589)  NC7H16 + C7H15-2 <=> NC7H16 + C7H15-4
  {
    double forward = 5.0e+10 * exp(-1.04e+04*ortc);
    double reverse = 1.0e+11 * exp(-1.04e+04*ortc);
    rr_f[589] = forward * mole_frac[69];
    rr_r[589] = reverse * mole_frac[69];
  }
  // 590)  NC7H16 + C7H15-3 <=> NC7H16 + C7H15-4
  {
    double forward = 5.0e+10 * exp(-1.04e+04*ortc);
    double reverse = 1.0e+11 * exp(-1.04e+04*ortc);
    rr_f[590] = forward * mole_frac[69];
    rr_r[590] = reverse * mole_frac[69];
  }
  // 591)  C7H15-1 <=> H + C7H14-1
  {
    double forward = 9.648e+13 * exp(-0.26*vlntemp - 3.601e+04*ortc);
    double reverse = 1.0e+13 * exp(-2.9e+03*ortc);
    rr_f[591] = forward;
    rr_r[591] = reverse * mole_frac[0] * mole_frac[70];
  }
  // 592)  C7H15-2 <=> C3H6 + PC4H9
  {
    double forward = 9.764e+18 * exp(-1.79*vlntemp - 3.136e+04*ortc);
    double reverse = 1.0e+11 * exp(-8.2e+03*ortc);
    rr_f[592] = forward;
    rr_r[592] = reverse * mole_frac[36];
  }
  // 593)  C7H15-2 <=> H + C7H14-1
  {
    double forward = 6.067e+12 * exp(0.09*vlntemp - 3.681e+04*ortc);
    double reverse = 1.0e+13 * exp(-1.2e+03*ortc);
    rr_f[593] = forward;
    rr_r[593] = reverse * mole_frac[0] * mole_frac[70];
  }
  // 594)  C7H15-2 <=> H + C7H14-2
  {
    double forward = 1.229e+13 * exp(-0.08*vlntemp - 3.564e+04*ortc);
    double reverse = 1.0e+13 * exp(-2.9e+03*ortc);
    rr_f[594] = forward;
    rr_r[594] = reverse * mole_frac[0] * mole_frac[71];
  }
  // 595)  C7H15-3 <=> NC3H7 + C4H8-1
  {
    double forward = 6.165e+18 * exp(-1.71*vlntemp - 3.096e+04*ortc);
    double reverse = 1.0e+11 * exp(-7.7e+03*ortc);
    rr_f[595] = forward;
    rr_r[595] = reverse * mole_frac[44];
  }
  // 596)  C7H15-3 <=> H + C7H14-2
  {
    double forward = 1.229e+13 * exp(-0.08*vlntemp - 3.564e+04*ortc);
    double reverse = 1.0e+13 * exp(-2.9e+03*ortc);
    rr_f[596] = forward;
    rr_r[596] = reverse * mole_frac[0] * mole_frac[71];
  }
  // 597)  C7H15-3 <=> H + C7H14-3
  {
    double forward = 9.152e+12 * exp(-0.02*vlntemp - 3.573e+04*ortc);
    double reverse = 1.0e+13 * exp(-2.9e+03*ortc);
    rr_f[597] = forward;
    rr_r[597] = reverse * mole_frac[0] * mole_frac[72];
  }
  // 598)  C7H15-4 <=> C2H5 + C5H10-1
  {
    double forward = 1.143e+18 * exp(-1.34*vlntemp - 3.143e+04*ortc);
    double reverse = 1.0e+11 * exp(-8.2e+03*ortc);
    rr_f[598] = forward;
    rr_r[598] = reverse * mole_frac[19] * mole_frac[64];
  }
  // 599)  C7H15-4 <=> H + C7H14-3
  {
    double forward = 1.819e+13 * exp(-0.02*vlntemp - 3.573e+04*ortc);
    double reverse = 1.0e+13 * exp(-2.9e+03*ortc);
    rr_f[599] = forward;
    rr_r[599] = reverse * mole_frac[0] * mole_frac[72];
  }
  // 600)  O2 + C7H15-1 <=> HO2 + C7H14-1
  {
    double forward = 3.0e-09 * exp(-3.0e+03*ortc);
    double reverse = 6.517e-10 * exp(0.18*vlntemp - 1.876e+04*ortc);
    rr_f[600] = forward * mole_frac[3];
    rr_r[600] = reverse * mole_frac[6] * mole_frac[70];
  }
  // 601)  O2 + C7H15-2 <=> HO2 + C7H14-1
  {
    double forward = 4.5e-09 * exp(-5.02e+03*ortc);
    double reverse = 1.532e-08 * exp(-0.16*vlntemp - 1.827e+04*ortc);
    rr_f[601] = forward * mole_frac[3];
    rr_r[601] = reverse * mole_frac[6] * mole_frac[70];
  }
  // 602)  O2 + C7H15-2 <=> HO2 + C7H14-2
  {
    double forward = 3.0e-09 * exp(-3.0e+03*ortc);
    double reverse = 5.04e-09 * exp(0.01*vlntemp - 1.912e+04*ortc);
    rr_f[602] = forward * mole_frac[3];
    rr_r[602] = reverse * mole_frac[6] * mole_frac[71];
  }
  // 603)  O2 + C7H15-3 <=> HO2 + C7H14-2
  {
    double forward = 3.0e-09 * exp(-3.0e+03*ortc);
    double reverse = 5.04e-09 * exp(0.01*vlntemp - 1.912e+04*ortc);
    rr_f[603] = forward * mole_frac[3];
    rr_r[603] = reverse * mole_frac[6] * mole_frac[71];
  }
  // 604)  O2 + C7H15-3 <=> HO2 + C7H14-3
  {
    double forward = 3.0e-09 * exp(-3.0e+03*ortc);
    double reverse = 6.769e-09 * exp(-0.05*vlntemp - 1.903e+04*ortc);
    rr_f[604] = forward * mole_frac[3];
    rr_r[604] = reverse * mole_frac[6] * mole_frac[72];
  }
  // 605)  O2 + C7H15-4 <=> HO2 + C7H14-3
  {
    double forward = 6.0e-09 * exp(-3.0e+03*ortc);
    double reverse = 6.795e-09 * exp(-0.05*vlntemp - 1.903e+04*ortc);
    rr_f[605] = forward * mole_frac[3];
    rr_r[605] = reverse * mole_frac[6] * mole_frac[72];
  }
  // 606)  C7H15-1 <=> C7H15-3
  {
    double forward = 1.386e+09 * exp(0.98*vlntemp - 3.376e+04*ortc);
    double reverse = 8.716e+07 * exp(1.33*vlntemp - 3.627e+04*ortc);
    rr_f[606] = forward;
    rr_r[606] = reverse;
  }
  // 607)  C7H15-1 <=> C7H15-4
  {
    double forward = 2.541e+09 * exp(0.35*vlntemp - 1.976e+04*ortc);
    double reverse = 3.176e+08 * exp(0.7*vlntemp - 2.227e+04*ortc);
    rr_f[607] = forward;
    rr_r[607] = reverse;
  }
  // 608)  C7H15-2 <=> C7H15-3
  {
    double forward = 9.587e+08 * exp(1.39*vlntemp - 3.97e+04*ortc);
    double reverse = 9.587e+08 * exp(1.39*vlntemp - 3.97e+04*ortc);
    rr_f[608] = forward;
    rr_r[608] = reverse;
  }
  // 609)  C7H15-1 <=> C7H15-2
  {
    double forward = 5.478e+08 * exp(1.62*vlntemp - 3.876e+04*ortc);
    double reverse = 3.445e+07 * exp(1.97*vlntemp - 4.127e+04*ortc);
    rr_f[609] = forward;
    rr_r[609] = reverse;
  }
  // 610)  H + C7H14-2 <=> H2 + C7H132-4
  {
    double forward = 3.376e+05 * exp(2.36*vlntemp - 207.0*ortc);
    double reverse = 4.4e+06 * exp(2.1*vlntemp - 2.059e+04*ortc);
    rr_f[610] = forward * mole_frac[0] * mole_frac[71];
    rr_r[610] = reverse * mole_frac[1] * mole_frac[73];
  }
  // 611)  OH + C7H14-2 <=> H2O + C7H132-4
  {
    double forward = 2.764e+04 * exp(2.64*vlntemp + 1.919e+03*ortc);
    double reverse = 1.56e+06 * exp(2.38*vlntemp - 3.362e+04*ortc);
    rr_f[611] = forward * mole_frac[4] * mole_frac[71];
    rr_r[611] = reverse * mole_frac[5] * mole_frac[73];
  }
  // 612)  CH3 + C7H14-2 <=> CH4 + C7H132-4
  {
    double forward = 3.69 * exp(3.31*vlntemp - 4.002e+03*ortc);
    double reverse = 1.256e+03 * exp(3.05*vlntemp - 2.486e+04*ortc);
    rr_f[612] = forward * mole_frac[17] * mole_frac[71];
    rr_r[612] = reverse * mole_frac[16] * mole_frac[73];
  }
  // 613)  HO2 + C7H14-2 <=> H2O2 + C7H132-4
  {
    double forward = 4.82e+03 * exp(2.55*vlntemp - 1.053e+04*ortc);
    double reverse = 1.615e+06 * exp(1.96*vlntemp - 1.46e+04*ortc);
    rr_f[613] = forward * mole_frac[6] * mole_frac[71];
    rr_r[613] = reverse * mole_frac[7] * mole_frac[73];
  }
  // 614)  CH3O2 + C7H14-2 <=> CH3O2H + C7H132-4
  {
    double forward = 4.82e+03 * exp(2.55*vlntemp - 1.053e+04*ortc);
    double reverse = 3.362e+06 * exp(1.79*vlntemp - 1.158e+04*ortc);
    rr_f[614] = forward * mole_frac[15] * mole_frac[71];
    rr_r[614] = reverse * mole_frac[14] * mole_frac[73];
  }
  // 615)  CH3O + C7H14-2 <=> CH3OH + C7H132-4
  {
    double forward = 40.0 * exp(2.9*vlntemp - 8.609e+03*ortc);
    double reverse = 251.3 * exp(2.66*vlntemp - 2.726e+04*ortc);
    rr_f[615] = forward * mole_frac[71];
    rr_r[615] = reverse * mole_frac[13] * mole_frac[73];
  }
  // 616)  H + C7H14-3 <=> H2 + C7H132-4
  {
    double forward = 3.376e+05 * exp(2.36*vlntemp - 207.0*ortc);
    double reverse = 3.276e+06 * exp(2.16*vlntemp - 2.068e+04*ortc);
    rr_f[616] = forward * mole_frac[0] * mole_frac[72];
    rr_r[616] = reverse * mole_frac[1] * mole_frac[73];
  }
  // 617)  OH + C7H14-3 <=> H2O + C7H132-4
  {
    double forward = 2.764e+04 * exp(2.64*vlntemp + 1.919e+03*ortc);
    double reverse = 1.161e+06 * exp(2.44*vlntemp - 3.371e+04*ortc);
    rr_f[617] = forward * mole_frac[4] * mole_frac[72];
    rr_r[617] = reverse * mole_frac[5] * mole_frac[73];
  }
  // 618)  CH3 + C7H14-3 <=> CH4 + C7H132-4
  {
    double forward = 3.69 * exp(3.31*vlntemp - 4.002e+03*ortc);
    double reverse = 935.5 * exp(3.11*vlntemp - 2.495e+04*ortc);
    rr_f[618] = forward * mole_frac[17] * mole_frac[72];
    rr_r[618] = reverse * mole_frac[16] * mole_frac[73];
  }
  // 619)  HO2 + C7H14-3 <=> H2O2 + C7H132-4
  {
    double forward = 4.82e+03 * exp(2.55*vlntemp - 1.053e+04*ortc);
    double reverse = 1.202e+06 * exp(2.03*vlntemp - 1.469e+04*ortc);
    rr_f[619] = forward * mole_frac[6] * mole_frac[72];
    rr_r[619] = reverse * mole_frac[7] * mole_frac[73];
  }
  // 620)  CH3O2 + C7H14-3 <=> CH3O2H + C7H132-4
  {
    double forward = 4.82e+03 * exp(2.55*vlntemp - 1.053e+04*ortc);
    double reverse = 2.503e+06 * exp(1.85*vlntemp - 1.167e+04*ortc);
    rr_f[620] = forward * mole_frac[15] * mole_frac[72];
    rr_r[620] = reverse * mole_frac[14] * mole_frac[73];
  }
  // 621)  CH3O + C7H14-3 <=> CH3OH + C7H132-4
  {
    double forward = 40.0 * exp(2.9*vlntemp - 8.609e+03*ortc);
    double reverse = 187.1 * exp(2.73*vlntemp - 2.735e+04*ortc);
    rr_f[621] = forward * mole_frac[72];
    rr_r[621] = reverse * mole_frac[13] * mole_frac[73];
  }
  // 622)  HO2 + C7H132-4 <=> OH + C7H13O2-4
  {
    double forward = 9.64e+12;
    double reverse = 4.273e+15 * exp(-1.02*vlntemp - 1.543e+04*ortc);
    rr_f[622] = forward * mole_frac[6] * mole_frac[73];
    rr_r[622] = reverse * mole_frac[4];
  }
  // 623)  CH3O2 + C7H132-4 <=> CH3O + C7H13O2-4
  {
    double forward = 9.64e+12;
    double reverse = 4.173e+17 * exp(-1.59*vlntemp - 2.019e+04*ortc);
    rr_f[623] = forward * mole_frac[15] * mole_frac[73];
    rr_r[623] = reverse;
  }
  // 624)  C2H5O2 + C7H132-4 <=> C2H5O + C7H13O2-4
  {
    double forward = 9.64e+12;
    double reverse = 2.732e+14 * exp(-0.67*vlntemp - 1.803e+04*ortc);
    rr_f[624] = forward * mole_frac[29] * mole_frac[73];
    rr_r[624] = reverse;
  }
  // 625)  C7H13O2-4 <=> C3H5-S + NC3H7CHO
  {
    double forward = 1.012e+22 * exp(-2.46*vlntemp - 2.919e+04*ortc);
    double reverse = 1.0e+11 * exp(-9.6e+03*ortc);
    rr_f[625] = forward;
    rr_r[625] = reverse * mole_frac[50];
  }
  // 626)  OH + C7H14-1 <=> CH2O + C6H13-1
  {
    double forward = 1.0e+11 * exp(4.0e+03*ortc);
    double reverse = 0.0;
    rr_f[626] = forward * mole_frac[4] * mole_frac[70];
    rr_r[626] = reverse * mole_frac[10];
  }
  // 627)  OH + C7H14-2 <=> C2H5CHO + PC4H9
  {
    double forward = 1.0e+11 * exp(4.0e+03*ortc);
    double reverse = 0.0;
    rr_f[627] = forward * mole_frac[4] * mole_frac[71];
    rr_r[627] = reverse * mole_frac[35];
  }
  // 628)  OH + C7H14-3 <=> C2H5CHO + PC4H9
  {
    double forward = 1.0e+11 * exp(4.0e+03*ortc);
    double reverse = 0.0;
    rr_f[628] = forward * mole_frac[4] * mole_frac[72];
    rr_r[628] = reverse * mole_frac[35];
  }
  // 629)  O + C7H14-2 <=> CH3CHO + C5H10-1
  {
    double forward = 1.0e+11 * exp(1.05e+03*ortc);
    double reverse = 0.0;
    rr_f[629] = forward * mole_frac[2] * mole_frac[71];
    rr_r[629] = reverse * mole_frac[23] * mole_frac[64];
  }
  // 630)  O + C7H14-3 <=> CH3CHO + C5H10-1
  {
    double forward = 1.0e+11 * exp(1.05e+03*ortc);
    double reverse = 0.0;
    rr_f[630] = forward * mole_frac[2] * mole_frac[72];
    rr_r[630] = reverse * mole_frac[23] * mole_frac[64];
  }
  // 631)  C7H14-1 <=> C3H5-A + PC4H9
  {
    double forward = 3.17e+21 * exp(-1.62*vlntemp - 7.533e+04*ortc);
    double reverse = 1.0e+13;
    rr_f[631] = forward * mole_frac[70];
    rr_r[631] = reverse * mole_frac[37];
  }
  // 632)  C7H14-2 <=> NC3H7 + C4H71-3
  {
    double forward = 3.735e+21 * exp(-1.74*vlntemp - 7.571e+04*ortc);
    double reverse = 1.0e+13;
    rr_f[632] = forward * mole_frac[71];
    rr_r[632] = reverse * mole_frac[46];
  }
  // 633)  C7H14-3 <=> C2H5 + C5H91-3
  {
    double forward = 5.947e+21 * exp(-1.85*vlntemp - 7.579e+04*ortc);
    double reverse = 1.0e+13;
    rr_f[633] = forward * mole_frac[72];
    rr_r[633] = reverse * mole_frac[19] * mole_frac[65];
  }
  // 634)  C7H15O2-1 <=> O2 + C7H15-1
  {
    double forward = 2.657e+20 * exp(-1.67*vlntemp - 3.54e+04*ortc);
    double reverse = 4.52e+12;
    rr_f[634] = forward * mole_frac[74];
    rr_r[634] = reverse * mole_frac[3];
  }
  // 635)  C7H15O2-2 <=> O2 + C7H15-2
  {
    double forward = 1.357e+23 * exp(-2.36*vlntemp - 3.767e+04*ortc);
    double reverse = 7.54e+12;
    rr_f[635] = forward * mole_frac[75];
    rr_r[635] = reverse * mole_frac[3];
  }
  // 636)  C7H15O2-3 <=> O2 + C7H15-3
  {
    double forward = 1.357e+23 * exp(-2.36*vlntemp - 3.767e+04*ortc);
    double reverse = 7.54e+12;
    rr_f[636] = forward * mole_frac[76];
    rr_r[636] = reverse * mole_frac[3];
  }
  // 637)  C7H15O2-4 <=> O2 + C7H15-4
  {
    double forward = 1.357e+23 * exp(-2.36*vlntemp - 3.767e+04*ortc);
    double reverse = 7.54e+12;
    rr_f[637] = forward * mole_frac[77];
    rr_r[637] = reverse * mole_frac[3];
  }
  // 638)  C7H15O2-1 <=> HO2 + C7H14-1
  {
    double forward = 5.044e+38 * exp(-8.109999999999999*vlntemp - 
      4.149e+04*ortc); 
    double reverse = 9.291e+29 * exp(-6.22*vlntemp - 2.086e+04*ortc);
    rr_f[638] = forward * mole_frac[74];
    rr_r[638] = reverse * mole_frac[6] * mole_frac[70];
  }
  // 639)  C7H15O2-2 <=> HO2 + C7H14-1
  {
    double forward = 5.075e+42 * exp(-9.41*vlntemp - 4.249e+04*ortc);
    double reverse = 9.595999999999999e+32 * exp(-7.21*vlntemp - 
      1.707e+04*ortc); 
    rr_f[639] = forward * mole_frac[75];
    rr_r[639] = reverse * mole_frac[6] * mole_frac[70];
  }
  // 640)  C7H15O2-2 <=> HO2 + C7H14-2
  {
    double forward = 5.044e+38 * exp(-8.109999999999999*vlntemp - 
      4.149e+04*ortc); 
    double reverse = 4.708e+28 * exp(-5.74*vlntemp - 1.894e+04*ortc);
    rr_f[640] = forward * mole_frac[75];
    rr_r[640] = reverse * mole_frac[6] * mole_frac[71];
  }
  // 641)  C7H15O2-3 <=> HO2 + C7H14-2
  {
    double forward = 5.044e+38 * exp(-8.109999999999999*vlntemp - 
      4.149e+04*ortc); 
    double reverse = 4.708e+28 * exp(-5.74*vlntemp - 1.894e+04*ortc);
    rr_f[641] = forward * mole_frac[76];
    rr_r[641] = reverse * mole_frac[6] * mole_frac[71];
  }
  // 642)  C7H15O2-3 <=> HO2 + C7H14-3
  {
    double forward = 5.044e+38 * exp(-8.109999999999999*vlntemp - 
      4.149e+04*ortc); 
    double reverse = 6.323e+28 * exp(-5.8*vlntemp - 1.885e+04*ortc);
    rr_f[642] = forward * mole_frac[76];
    rr_r[642] = reverse * mole_frac[6] * mole_frac[72];
  }
  // 643)  C7H15O2-4 <=> HO2 + C7H14-3
  {
    double forward = 1.009e+39 * exp(-8.109999999999999*vlntemp - 
      4.149e+04*ortc); 
    double reverse = 6.348e+28 * exp(-5.8*vlntemp - 1.885e+04*ortc);
    rr_f[643] = forward * mole_frac[77];
    rr_r[643] = reverse * mole_frac[6] * mole_frac[72];
  }
  // 644)  C7H15O2-1 <=> C7H14OOH1-2
  {
    double forward = 2.0e+11 * exp(-2.645e+04*ortc);
    double xik = -cgspl[108] + cgspl[112];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[644] = forward * mole_frac[74];
    rr_r[644] = reverse;
  }
  // 645)  C7H15O2-1 <=> C7H14OOH1-3
  {
    double forward = 2.5e+10 * exp(-2.045e+04*ortc);
    double xik = -cgspl[108] + cgspl[113];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[645] = forward * mole_frac[74];
    rr_r[645] = reverse;
  }
  // 646)  C7H15O2-2 <=> C7H14OOH2-3
  {
    double forward = 2.0e+11 * exp(-2.645e+04*ortc);
    double xik = -cgspl[109] + cgspl[114];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[646] = forward * mole_frac[75];
    rr_r[646] = reverse;
  }
  // 647)  C7H15O2-2 <=> C7H14OOH2-4
  {
    double forward = 2.5e+10 * exp(-2.045e+04*ortc);
    double xik = -cgspl[109] + cgspl[115];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[647] = forward * mole_frac[75];
    rr_r[647] = reverse;
  }
  // 648)  C7H15O2-3 <=> C7H14OOH3-5
  {
    double forward = 2.5e+10 * exp(-2.045e+04*ortc);
    double xik = -cgspl[110] + cgspl[116];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[648] = forward * mole_frac[76];
    rr_r[648] = reverse;
  }
  // 649)  C7H15O2-4 <=> C7H14OOH4-2
  {
    double forward = 5.0e+10 * exp(-2.045e+04*ortc);
    double xik = -cgspl[111] + cgspl[117];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[649] = forward * mole_frac[77];
    rr_r[649] = reverse;
  }
  // 650)  C7H15O2-4 <=> C7H14OOH4-3
  {
    double forward = 4.0e+11 * exp(-2.645e+04*ortc);
    double xik = -cgspl[111] + cgspl[118];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[650] = forward * mole_frac[77];
    rr_r[650] = reverse;
  }
  // 651)  HO2 + C7H14-1 <=> C7H14OOH1-2
  {
    double forward = 2.5e+03 * exp(2.5*vlntemp - 1.12e+04*ortc);
    double xik = -cgspl[6] - cgspl[103] + cgspl[112];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[651] = forward * mole_frac[6] * mole_frac[70];
    rr_r[651] = reverse;
  }
  // 652)  HO2 + C7H14-2 <=> C7H14OOH2-3
  {
    double forward = 2.7e+03 * exp(2.5*vlntemp - 1.05e+04*ortc);
    double xik = -cgspl[6] - cgspl[104] + cgspl[114];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[652] = forward * mole_frac[6] * mole_frac[71];
    rr_r[652] = reverse;
  }
  // 653)  HO2 + C7H14-3 <=> C7H14OOH4-3
  {
    double forward = 2.7e+03 * exp(2.5*vlntemp - 1.05e+04*ortc);
    double xik = -cgspl[6] - cgspl[105] + cgspl[118];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[653] = forward * mole_frac[6] * mole_frac[72];
    rr_r[653] = reverse;
  }
  // 654)  C7H14OOH1-3 <=> OH + C7H14O1-3
  {
    double forward = 7.5e+10 * exp(-1.525e+04*ortc);
    double reverse = 0.0;
    rr_f[654] = forward;
    rr_r[654] = reverse * mole_frac[4] * mole_frac[83];
  }
  // 655)  C7H14OOH2-4 <=> OH + C7H14O2-4
  {
    double forward = 7.5e+10 * exp(-1.525e+04*ortc);
    double reverse = 0.0;
    rr_f[655] = forward;
    rr_r[655] = reverse * mole_frac[4] * mole_frac[84];
  }
  // 656)  C7H14OOH3-5 <=> OH + C7H14O3-5
  {
    double forward = 7.5e+10 * exp(-1.525e+04*ortc);
    double reverse = 0.0;
    rr_f[656] = forward;
    rr_r[656] = reverse * mole_frac[4] * mole_frac[85];
  }
  // 657)  C7H14OOH4-2 <=> OH + C7H14O2-4
  {
    double forward = 7.5e+10 * exp(-1.525e+04*ortc);
    double reverse = 0.0;
    rr_f[657] = forward;
    rr_r[657] = reverse * mole_frac[4] * mole_frac[84];
  }
  // 658)  C7H14OOH2-4 <=> OH + CH3CHO + C5H10-1
  {
    double forward = 1.548e+12 * exp(0.59*vlntemp - 3.009e+04*ortc);
    double reverse = 0.0;
    rr_f[658] = forward;
    rr_r[658] = reverse * mole_frac[4] * mole_frac[23] * mole_frac[64];
  }
  // 659)  C7H14OOH3-5 <=> OH + C2H5CHO + C4H8-1
  {
    double forward = 2.661e+13 * exp(0.13*vlntemp - 3.043e+04*ortc);
    double reverse = 0.0;
    rr_f[659] = forward;
    rr_r[659] = reverse * mole_frac[4] * mole_frac[35] * mole_frac[44];
  }
  // 660)  C7H14OOH4-2 <=> OH + C3H6 + NC3H7CHO
  {
    double forward = 6.186e+13 * exp(0.09*vlntemp - 3.084e+04*ortc);
    double reverse = 0.0;
    rr_f[660] = forward;
    rr_r[660] = reverse * mole_frac[4] * mole_frac[36] * mole_frac[50];
  }
  // 661)  C7H14OOH1-3 <=> NC3H7 + C4H7OOH1-4
  {
    double forward = 1.609e+12 * exp(0.54*vlntemp - 2.774e+04*ortc);
    double reverse = 2.2e+03 * exp(2.48*vlntemp - 6.13e+03*ortc);
    rr_f[661] = forward;
    rr_r[661] = reverse * mole_frac[87];
  }
  // 662)  C7H14OOH1-3O2 <=> O2 + C7H14OOH1-3
  {
    double forward = 1.367e+23 * exp(-2.37*vlntemp - 3.764e+04*ortc);
    double reverse = 7.54e+12;
    rr_f[662] = forward * mole_frac[78];
    rr_r[662] = reverse * mole_frac[3];
  }
  // 663)  C7H14OOH2-3O2 <=> O2 + C7H14OOH2-3
  {
    double forward = 1.389e+23 * exp(-2.38*vlntemp - 3.76e+04*ortc);
    double reverse = 7.54e+12;
    rr_f[663] = forward * mole_frac[79];
    rr_r[663] = reverse * mole_frac[3];
  }
  // 664)  C7H14OOH2-4O2 <=> O2 + C7H14OOH2-4
  {
    double forward = 1.389e+23 * exp(-2.38*vlntemp - 3.76e+04*ortc);
    double reverse = 7.54e+12;
    rr_f[664] = forward * mole_frac[80];
    rr_r[664] = reverse * mole_frac[3];
  }
  // 665)  C7H14OOH4-2O2 <=> O2 + C7H14OOH4-2
  {
    double forward = 6.969e+22 * exp(-2.38*vlntemp - 3.76e+04*ortc);
    double reverse = 7.54e+12;
    rr_f[665] = forward * mole_frac[81];
    rr_r[665] = reverse * mole_frac[3];
  }
  // 666)  C7H14OOH4-3O2 <=> O2 + C7H14OOH4-3
  {
    double forward = 6.969e+22 * exp(-2.38*vlntemp - 3.76e+04*ortc);
    double reverse = 7.54e+12;
    rr_f[666] = forward * mole_frac[82];
    rr_r[666] = reverse * mole_frac[3];
  }
  // 667)  C7H14OOH1-3O2 <=> OH + NC7KET13
  {
    double forward = 2.5e+10 * exp(-2.1e+04*ortc);
    double xik = cgspl[4] - cgspl[119] + cgspl[127];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[667] = forward * mole_frac[78];
    rr_r[667] = reverse * mole_frac[4] * mole_frac[86];
  }
  // 668)  NC7KET13 <=> OH + CH2CHO + NC4H9CHO
  {
    double forward = 1.0e+16 * exp(-3.9e+04*ortc);
    double reverse = 0.0;
    rr_f[668] = forward * mole_frac[86];
    rr_r[668] = reverse * mole_frac[4] * mole_frac[24] * mole_frac[68];
  }
  // 669)  OH + C7H14O2-4 <=> H2O + CH3CO + C5H10-1
  {
    double forward = 2.5e+12;
    double reverse = 0.0;
    rr_f[669] = forward * mole_frac[4] * mole_frac[84];
    rr_r[669] = reverse * mole_frac[5] * mole_frac[64];
  }
  // 670)  OH + C7H14O3-5 <=> H2O + C2H5CO + C4H8-1
  {
    double forward = 2.5e+12;
    double reverse = 0.0;
    rr_f[670] = forward * mole_frac[4] * mole_frac[85];
    rr_r[670] = reverse * mole_frac[5] * mole_frac[44];
  }
  // 671)  OH + C7H14O1-3 <=> H2O + C2H4 + NC4H9CO
  {
    double forward = 2.5e+12;
    double reverse = 0.0;
    rr_f[671] = forward * mole_frac[4] * mole_frac[83];
    rr_r[671] = reverse * mole_frac[5] * mole_frac[20];
  }
  // 672)  OH + C7H14O2-4 <=> H2O + C3H6 + NC3H7CO
  {
    double forward = 2.5e+12;
    double reverse = 0.0;
    rr_f[672] = forward * mole_frac[4] * mole_frac[84];
    rr_r[672] = reverse * mole_frac[5] * mole_frac[36];
  }
  // 673)  HO2 + C7H14O2-4 <=> H2O2 + CH3CO + C5H10-1
  {
    double forward = 5.0e+12 * exp(-1.77e+04*ortc);
    double reverse = 0.0;
    rr_f[673] = forward * mole_frac[6] * mole_frac[84];
    rr_r[673] = reverse * mole_frac[7] * mole_frac[64];
  }
  // 674)  HO2 + C7H14O3-5 <=> H2O2 + C2H5CO + C4H8-1
  {
    double forward = 5.0e+12 * exp(-1.77e+04*ortc);
    double reverse = 0.0;
    rr_f[674] = forward * mole_frac[6] * mole_frac[85];
    rr_r[674] = reverse * mole_frac[7] * mole_frac[44];
  }
  // 675)  HO2 + C7H14O1-3 <=> H2O2 + C2H4 + NC4H9CO
  {
    double forward = 5.0e+12 * exp(-1.77e+04*ortc);
    double reverse = 0.0;
    rr_f[675] = forward * mole_frac[6] * mole_frac[83];
    rr_r[675] = reverse * mole_frac[7] * mole_frac[20];
  }
  // 676)  HO2 + C7H14O2-4 <=> H2O2 + C3H6 + NC3H7CO
  {
    double forward = 5.0e+12 * exp(-1.77e+04*ortc);
    double reverse = 0.0;
    rr_f[676] = forward * mole_frac[6] * mole_frac[84];
    rr_r[676] = reverse * mole_frac[7] * mole_frac[36];
  }
  // 677)  C4H7OOH1-4 <=> OH + C4H7O1-4
  {
    double forward = 2.021e+20 * exp(-1.53*vlntemp - 4.704e+04*ortc);
    double reverse = 2.0e+13;
    rr_f[677] = forward * mole_frac[87];
    rr_r[677] = reverse * mole_frac[4];
  }
  // 678)  C4H7O1-4 <=> CH2O + C3H5-A
  {
    double forward = 2.412e+16 * exp(-1.14*vlntemp - 7.55e+03*ortc);
    double reverse = 1.0e+11 * exp(-1.19e+04*ortc);
    rr_f[678] = forward;
    rr_r[678] = reverse * mole_frac[10] * mole_frac[37];
  }
  // 679)  CH3CHCHO <=> H + C2H3CHO
  {
    double forward = 3.515e+15 * exp(-0.51*vlntemp - 4.106e+04*ortc);
    double reverse = 6.5e+12 * exp(-2.9e+03*ortc);
    rr_f[679] = forward * mole_frac[88];
    rr_r[679] = reverse * mole_frac[0] * mole_frac[33];
  }
  // 680)  CH3CHCHO <=> H + CH3CHCO
  {
    double forward = 1.135e+16 * exp(-0.66*vlntemp - 4.031e+04*ortc);
    double reverse = 5.0e+12 * exp(-1.2e+03*ortc);
    rr_f[680] = forward * mole_frac[88];
    rr_r[680] = reverse * mole_frac[0] * mole_frac[43];
  }
  // 681)  H2 + CH3CHCHO <=> H + C2H5CHO
  {
    double forward = 2.16e+05 * exp(2.38*vlntemp - 1.899e+04*ortc);
    double reverse = 4.309e+04 * exp(2.63*vlntemp - 5.265e+03*ortc);
    rr_f[681] = forward * mole_frac[1] * mole_frac[88];
    rr_r[681] = reverse * mole_frac[0] * mole_frac[35];
  }
  // 682)  OH + C5H10-1 <=> CH2O + PC4H9
  {
    double forward = 2.0e+10 * exp(4.0e+03*ortc);
    double reverse = 3.0e+11 * exp(-2.04e+04*ortc);
    rr_f[682] = forward * mole_frac[4] * mole_frac[64];
    rr_r[682] = reverse * mole_frac[10];
  }
  // 683)  O + C5H10-1 <=> HCO + PC4H9
  {
    double forward = 1.0e+11;
    double reverse = 0.0;
    rr_f[683] = forward * mole_frac[2] * mole_frac[64];
    rr_r[683] = reverse * mole_frac[11];
  }
  // 684)  O + C5H10-1 <=> CH3CO + NC3H7
  {
    double forward = 1.0e+11;
    double reverse = 0.0;
    rr_f[684] = forward * mole_frac[2] * mole_frac[64];
    rr_r[684] = reverse;
  }
  // 685)  IC4H7-I1 <=> CH3 + C3H4-P
  {
    double forward = 2.103e+12 * exp(0.08*vlntemp - 2.995e+04*ortc);
    double reverse = 1.0e+11 * exp(-9.2e+03*ortc);
    rr_f[685] = forward * mole_frac[89];
    rr_r[685] = reverse * mole_frac[17] * mole_frac[38];
  }
  // 686)  YC7H15 <=> IC3H7 + IC4H8
  {
    double forward = 2.22e+20 * exp(-2.06*vlntemp - 3.247e+04*ortc);
    double reverse = 5.0e+10 * exp(-9.2e+03*ortc);
    rr_f[686] = forward;
    rr_r[686] = reverse * mole_frac[51];
  }
  // 687)  YC7H15 <=> H + XC7H14
  {
    double forward = 1.437e+13 * exp(0.23*vlntemp - 3.769e+04*ortc);
    double reverse = 2.6e+13 * exp(-1.2e+03*ortc);
    rr_f[687] = forward;
    rr_r[687] = reverse * mole_frac[0] * mole_frac[90];
  }
  // 688)  YC7H15 <=> H + YC7H14
  {
    double forward = 3.093e+13 * exp(0.049*vlntemp - 3.639e+04*ortc);
    double reverse = 2.6e+13 * exp(-2.5e+03*ortc);
    rr_f[688] = forward;
    rr_r[688] = reverse * mole_frac[0] * mole_frac[91];
  }
  // 689)  O2 + YC7H15 <=> HO2 + XC7H14
  {
    double forward = 6.000000000000001e-29 * exp(-5.02e+03*ortc);
    double reverse = 2.0e-29 * exp(-1.75e+04*ortc);
    rr_f[689] = forward * mole_frac[3];
    rr_r[689] = reverse * mole_frac[6] * mole_frac[90];
  }
  // 690)  O2 + YC7H15 <=> HO2 + YC7H14
  {
    double forward = 3.0e-29 * exp(-3.0e+03*ortc);
    double reverse = 2.0e-29 * exp(-1.75e+04*ortc);
    rr_f[690] = forward * mole_frac[3];
    rr_r[690] = reverse * mole_frac[6] * mole_frac[91];
  }
  // 691)  XC7H14 <=> IC3H7 + IC4H7
  {
    double forward = 2.211e+24 * exp(-2.392*vlntemp - 7.467e+04*ortc);
    double reverse = 1.28e+14 * exp(-0.35 * vlntemp);
    rr_f[691] = forward * mole_frac[90];
    rr_r[691] = reverse * mole_frac[52];
  }
  // 692)  OH + XC7H14 <=> CH3COCH3 + IC4H9
  {
    double forward = 2.0e+10 * exp(4.0e+03*ortc);
    double reverse = 0.0;
    rr_f[692] = forward * mole_frac[4] * mole_frac[90];
    rr_r[692] = reverse * mole_frac[30];
  }
  // 693)  OH + YC7H14 <=> CH3COCH3 + IC4H9
  {
    double forward = 2.0e+10 * exp(4.0e+03*ortc);
    double reverse = 0.0;
    rr_f[693] = forward * mole_frac[4] * mole_frac[91];
    rr_r[693] = reverse * mole_frac[30];
  }
  // 694)  O + YC7H14 <=> CH3COCH3 + IC4H8
  {
    double forward = 2.0e+10 * exp(1.05e+03*ortc);
    double reverse = 0.0;
    rr_f[694] = forward * mole_frac[2] * mole_frac[91];
    rr_r[694] = reverse * mole_frac[30] * mole_frac[51];
  }
  // 695)  H + XC7H14 <=> H2 + XC7H13-Z
  {
    double forward = 3.376e+05 * exp(2.36*vlntemp - 207.0*ortc);
    double reverse = 4.418e+06 * exp(2.1*vlntemp - 2.037e+04*ortc);
    rr_f[695] = forward * mole_frac[0] * mole_frac[90];
    rr_r[695] = reverse * mole_frac[1] * mole_frac[92];
  }
  // 696)  OH + XC7H14 <=> H2O + XC7H13-Z
  {
    double forward = 2.764e+04 * exp(2.64*vlntemp + 1.919e+03*ortc);
    double reverse = 1.566e+06 * exp(2.38*vlntemp - 3.34e+04*ortc);
    rr_f[696] = forward * mole_frac[4] * mole_frac[90];
    rr_r[696] = reverse * mole_frac[5] * mole_frac[92];
  }
  // 697)  CH3 + XC7H14 <=> CH4 + XC7H13-Z
  {
    double forward = 3.69 * exp(3.31*vlntemp - 4.002e+03*ortc);
    double reverse = 1.262e+03 * exp(3.05*vlntemp - 2.464e+04*ortc);
    rr_f[697] = forward * mole_frac[17] * mole_frac[90];
    rr_r[697] = reverse * mole_frac[16] * mole_frac[92];
  }
  // 698)  HO2 + XC7H14 <=> H2O2 + XC7H13-Z
  {
    double forward = 4.82e+03 * exp(2.55*vlntemp - 1.053e+04*ortc);
    double reverse = 1.621e+06 * exp(1.96*vlntemp - 1.438e+04*ortc);
    rr_f[698] = forward * mole_frac[6] * mole_frac[90];
    rr_r[698] = reverse * mole_frac[7] * mole_frac[92];
  }
  // 699)  CH3O2 + XC7H14 <=> CH3O2H + XC7H13-Z
  {
    double forward = 4.82e+03 * exp(2.55*vlntemp - 1.053e+04*ortc);
    double reverse = 3.376e+06 * exp(1.79*vlntemp - 1.136e+04*ortc);
    rr_f[699] = forward * mole_frac[15] * mole_frac[90];
    rr_r[699] = reverse * mole_frac[14] * mole_frac[92];
  }
  // 700)  CH3O + XC7H14 <=> CH3OH + XC7H13-Z
  {
    double forward = 40.0 * exp(2.9*vlntemp - 8.609e+03*ortc);
    double reverse = 252.4 * exp(2.66*vlntemp - 2.704e+04*ortc);
    rr_f[700] = forward * mole_frac[90];
    rr_r[700] = reverse * mole_frac[13] * mole_frac[92];
  }
  // 701)  H + YC7H14 <=> H2 + XC7H13-Z
  {
    double forward = 3.46e+05 * exp(2.5*vlntemp - 2.492e+03*ortc);
    double reverse = 9.749e+06 * exp(2.06*vlntemp - 2.005e+04*ortc);
    rr_f[701] = forward * mole_frac[0] * mole_frac[91];
    rr_r[701] = reverse * mole_frac[1] * mole_frac[92];
  }
  // 702)  H + YC7H14 <=> H2 + YC7H13-Y2
  {
    double forward = 2.65e+06 * exp(2.2 * vlntemp);
    double reverse = 1.836e+07 * exp(2.14*vlntemp - 2.229e+04*ortc);
    rr_f[702] = forward * mole_frac[0] * mole_frac[91];
    rr_r[702] = reverse * mole_frac[1] * mole_frac[93];
  }
  // 703)  OH + YC7H14 <=> H2O + XC7H13-Z
  {
    double forward = 6.24e+06 * temperature * temperature * exp(298.0*ortc);
    double reverse = 7.613e+08 * exp(1.56*vlntemp - 3.242e+04*ortc);
    rr_f[703] = forward * mole_frac[4] * mole_frac[91];
    rr_r[703] = reverse * mole_frac[5] * mole_frac[92];
  }
  // 704)  OH + YC7H14 <=> H2O + YC7H13-Y2
  {
    double forward = 614.0 * exp(3.2*vlntemp + 3.5e+03*ortc);
    double reverse = 1.842e+04 * exp(3.14*vlntemp - 3.394e+04*ortc);
    rr_f[704] = forward * mole_frac[4] * mole_frac[91];
    rr_r[704] = reverse * mole_frac[5] * mole_frac[93];
  }
  // 705)  XC7H13-Z <=> CH3 + ACC6H10
  {
    double forward = 2.837e+16 * exp(-0.83*vlntemp - 4.123e+04*ortc);
    double reverse = 1.0e+11 * exp(-1.06e+04*ortc);
    rr_f[705] = forward * mole_frac[92];
    rr_r[705] = reverse * mole_frac[17] * mole_frac[95];
  }
  // 706)  HO2 + XC7H13-Z <=> OH + XC7H13O-Z
  {
    double forward = 9.64e+12;
    double reverse = 5.406e+14 * exp(-0.73*vlntemp - 1.538e+04*ortc);
    rr_f[706] = forward * mole_frac[6] * mole_frac[92];
    rr_r[706] = reverse * mole_frac[4];
  }
  // 707)  CH3O2 + XC7H13-Z <=> CH3O + XC7H13O-Z
  {
    double forward = 9.64e+12;
    double reverse = 5.28e+16 * exp(-1.31*vlntemp - 2.014e+04*ortc);
    rr_f[707] = forward * mole_frac[15] * mole_frac[92];
    rr_r[707] = reverse;
  }
  // 708)  C2H5O2 + XC7H13-Z <=> C2H5O + XC7H13O-Z
  {
    double forward = 9.64e+12;
    double reverse = 3.456e+13 * exp(-0.38*vlntemp - 1.798e+04*ortc);
    rr_f[708] = forward * mole_frac[29] * mole_frac[92];
    rr_r[708] = reverse;
  }
  // 709)  HO2 + YC7H13-Y2 <=> OH + YC7H13O-Y2
  {
    double forward = 9.64e+12;
    double reverse = 2.111e+15 * exp(-0.97*vlntemp - 1.7e+04*ortc);
    rr_f[709] = forward * mole_frac[6] * mole_frac[93];
    rr_r[709] = reverse * mole_frac[4];
  }
  // 710)  CH3O2 + YC7H13-Y2 <=> CH3O + YC7H13O-Y2
  {
    double forward = 9.64e+12;
    double reverse = 2.062e+17 * exp(-1.55*vlntemp - 2.176e+04*ortc);
    rr_f[710] = forward * mole_frac[15] * mole_frac[93];
    rr_r[710] = reverse;
  }
  // 711)  C2H5O2 + YC7H13-Y2 <=> C2H5O + YC7H13O-Y2
  {
    double forward = 9.64e+12;
    double reverse = 1.35e+14 * exp(-0.62*vlntemp - 1.961e+04*ortc);
    rr_f[711] = forward * mole_frac[29] * mole_frac[93];
    rr_r[711] = reverse;
  }
  // 712)  XC7H13O-Z <=> C3H5-T + IC3H7CHO
  {
    double forward = 7.521e+21 * exp(-2.4*vlntemp - 3.019e+04*ortc);
    double reverse = 1.0e+11 * exp(-1.19e+04*ortc);
    rr_f[712] = forward;
    rr_r[712] = reverse * mole_frac[55];
  }
  // 713)  YC7H13O-Y2 <=> CH3COCH3 + IC4H7-I1
  {
    double forward = 1.31e+18 * exp(-1.3*vlntemp - 2.942e+04*ortc);
    double reverse = 1.0e+11 * exp(-1.19e+04*ortc);
    rr_f[713] = forward;
    rr_r[713] = reverse * mole_frac[30] * mole_frac[89];
  }
  // 714)  YC7H15O2 <=> O2 + YC7H15
  {
    double forward = 3.408e+23 * exp(-2.448*vlntemp - 3.721e+04*ortc);
    double reverse = 3.0e+12;
    rr_f[714] = forward * mole_frac[94];
    rr_r[714] = reverse * mole_frac[3];
  }
  // 715)  YC7H15O2 <=> HO2 + XC7H14
  {
    double forward = 1.015e+43 * exp(-9.41*vlntemp - 4.149e+04*ortc);
    double reverse = 3.387e+32 * exp(-7.264*vlntemp - 1.666e+04*ortc);
    rr_f[715] = forward * mole_frac[94];
    rr_r[715] = reverse * mole_frac[6] * mole_frac[90];
  }
  // 716)  YC7H15O2 <=> HO2 + YC7H14
  {
    double forward = 5.044e+38 * exp(-8.109999999999999*vlntemp - 
      4.049e+04*ortc); 
    double reverse = 7.817e+27 * exp(-5.783*vlntemp - 1.826e+04*ortc);
    rr_f[716] = forward * mole_frac[94];
    rr_r[716] = reverse * mole_frac[6] * mole_frac[91];
  }
  // 717)  OH + ACC6H10 <=> H2O + ACC6H9-A
  {
    double forward = 3.12e+06 * temperature * temperature * exp(298.0*ortc);
    double reverse = 5.491e+08 * exp(1.39*vlntemp - 3.246e+04*ortc);
    rr_f[717] = forward * mole_frac[4] * mole_frac[95];
    rr_r[717] = reverse * mole_frac[5] * mole_frac[96];
  }
  // 718)  OH + ACC6H10 <=> H2O + ACC6H9-D
  {
    double forward = 3.12e+06 * temperature * temperature * exp(298.0*ortc);
    double reverse = 5.491e+08 * exp(1.39*vlntemp - 3.246e+04*ortc);
    rr_f[718] = forward * mole_frac[4] * mole_frac[95];
    rr_r[718] = reverse * mole_frac[5] * mole_frac[97];
  }
  // 719)  HO2 + ACC6H10 <=> H2O2 + ACC6H9-A
  {
    double forward = 9.64e+03 * exp(2.6*vlntemp - 1.391e+04*ortc);
    double reverse = 1.007e+07 * exp(1.66*vlntemp - 1.521e+04*ortc);
    rr_f[719] = forward * mole_frac[6] * mole_frac[95];
    rr_r[719] = reverse * mole_frac[7] * mole_frac[96];
  }
  // 720)  HO2 + ACC6H10 <=> H2O2 + ACC6H9-D
  {
    double forward = 9.64e+03 * exp(2.6*vlntemp - 1.391e+04*ortc);
    double reverse = 1.007e+07 * exp(1.66*vlntemp - 1.521e+04*ortc);
    rr_f[720] = forward * mole_frac[6] * mole_frac[95];
    rr_r[720] = reverse * mole_frac[7] * mole_frac[97];
  }
  // 721)  CH3O2 + ACC6H10 <=> CH3O2H + ACC6H9-A
  {
    double forward = 9.64e+03 * exp(2.6*vlntemp - 1.391e+04*ortc);
    double reverse = 4.139e+07 * exp(1.49*vlntemp - 1.449e+04*ortc);
    rr_f[721] = forward * mole_frac[15] * mole_frac[95];
    rr_r[721] = reverse * mole_frac[14] * mole_frac[96];
  }
  // 722)  CH3O2 + ACC6H10 <=> CH3O2H + ACC6H9-D
  {
    double forward = 9.64e+03 * exp(2.6*vlntemp - 1.391e+04*ortc);
    double reverse = 4.139e+07 * exp(1.49*vlntemp - 1.449e+04*ortc);
    rr_f[722] = forward * mole_frac[15] * mole_frac[95];
    rr_r[722] = reverse * mole_frac[14] * mole_frac[97];
  }
  // 723)  ACC6H9-A <=> C3H5-S + C3H4-A
  {
    double forward = 1.194e+24 * exp(-2.85*vlntemp - 7.431e+04*ortc);
    double reverse = 1.0e+11 * exp(-9.2e+03*ortc);
    rr_f[723] = forward * mole_frac[96];
    rr_r[723] = reverse * mole_frac[39];
  }
  // 724)  HO2 + ACC6H9-D <=> OH + C2H3 + IC3H5CHO
  {
    double forward = 8.91e+12;
    double reverse = 0.0;
    rr_f[724] = forward * mole_frac[6] * mole_frac[97];
    rr_r[724] = reverse * mole_frac[4] * mole_frac[21] * mole_frac[60];
  }
  // 725)  NEOC5H11 <=> CH3 + IC4H8
  {
    double forward = 8.466e+17 * exp(-1.111*vlntemp - 3.293e+04*ortc);
    double reverse = 1.3e+03 * exp(2.48*vlntemp - 8.52e+03*ortc);
    rr_f[725] = forward * mole_frac[98];
    rr_r[725] = reverse * mole_frac[17] * mole_frac[51];
  }
  // 726)  HO2 + TC4H9CHO <=> H2O2 + TC4H9CO
  {
    double forward = 1.0e+12 * exp(-1.192e+04*ortc);
    double reverse = 3.852e+12 * exp(-0.33*vlntemp - 1.2e+04*ortc);
    rr_f[726] = forward * mole_frac[6] * mole_frac[99];
    rr_r[726] = reverse * mole_frac[7];
  }
  // 727)  CH3 + TC4H9CHO <=> CH4 + TC4H9CO
  {
    double forward = 3.98e+12 * exp(-8.7e+03*ortc);
    double reverse = 1.558e+13 * exp(-2.557e+04*ortc);
    rr_f[727] = forward * mole_frac[17] * mole_frac[99];
    rr_r[727] = reverse * mole_frac[16];
  }
  // 728)  O + TC4H9CHO <=> OH + TC4H9CO
  {
    double forward = 7.18e+12 * exp(-1.389e+03*ortc);
    double reverse = 4.726e+11 * exp(-1.568e+04*ortc);
    rr_f[728] = forward * mole_frac[2] * mole_frac[99];
    rr_r[728] = reverse * mole_frac[4];
  }
  // 729)  O2 + TC4H9CHO <=> HO2 + TC4H9CO
  {
    double forward = 4.0e+13 * exp(-3.76e+04*ortc);
    double reverse = 1.089e+11 * exp(0.32*vlntemp + 3.492e+03*ortc);
    rr_f[729] = forward * mole_frac[3] * mole_frac[99];
    rr_r[729] = reverse * mole_frac[6];
  }
  // 730)  OH + TC4H9CHO <=> H2O + TC4H9CO
  {
    double forward = 2.69e+10 * exp(0.76*vlntemp + 340.0*ortc);
    double reverse = 1.746e+10 * exp(0.76*vlntemp - 3.12e+04*ortc);
    rr_f[730] = forward * mole_frac[4] * mole_frac[99];
    rr_r[730] = reverse * mole_frac[5];
  }
  // 731)  TC4H9CO <=> CO + TC4H9
  {
    double forward = 2.517e+23 * exp(-2.881*vlntemp - 1.349e+04*ortc);
    double reverse = 1.5e+11 * exp(-4.81e+03*ortc);
    rr_f[731] = forward;
    rr_r[731] = reverse * mole_frac[8];
  }
  // 732)  PC7H15 <=> C3H6 + TC4H9
  {
    double forward = 2.308e+23 * exp(-2.801*vlntemp - 2.934e+04*ortc);
    double reverse = 1.5e+11 * exp(-7.2e+03*ortc);
    rr_f[732] = forward;
    rr_r[732] = reverse * mole_frac[36];
  }
  // 733)  IC8H18 <=> H + CC8H17
  {
    double forward = 1.146e+19 * exp(-0.9409999999999999*vlntemp - 
      9.543e+04*ortc); 
    double reverse = 1.0e+14;
    rr_f[733] = forward * mole_frac[100];
    rr_r[733] = reverse * mole_frac[0];
  }
  // 734)  IC8H18 <=> CH3 + YC7H15
  {
    double forward = 1.635e+27 * exp(-2.794*vlntemp - 8.393e+04*ortc);
    double reverse = 1.63e+13 * exp(596.0*ortc);
    rr_f[734] = forward * mole_frac[100];
    rr_r[734] = reverse * mole_frac[17];
  }
  // 735)  IC8H18 <=> CH3 + PC7H15
  {
    double forward = 1.376e+26 * exp(-2.591*vlntemp - 8.586e+04*ortc);
    double reverse = 1.93e+14 * exp(-0.32 * vlntemp);
    rr_f[735] = forward * mole_frac[100];
    rr_r[735] = reverse * mole_frac[17];
  }
  // 736)  IC8H18 <=> IC4H9 + TC4H9
  {
    double forward = 7.828e+29 * exp(-3.925*vlntemp - 8.415e+04*ortc);
    double reverse = 3.59e+14 * exp(-0.75 * vlntemp);
    rr_f[736] = forward * mole_frac[100];
    rr_r[736] = reverse;
  }
  // 737)  IC8H18 <=> IC3H7 + NEOC5H11
  {
    double forward = 2.455e+23 * exp(-2.013*vlntemp - 8.34e+04*ortc);
    double reverse = 3.59e+14 * exp(-0.75 * vlntemp);
    rr_f[737] = forward * mole_frac[100];
    rr_r[737] = reverse * mole_frac[98];
  }
  // 738)  H + IC8H18 <=> H2 + CC8H17
  {
    double forward = 6.02e+05 * exp(2.4*vlntemp - 2.583e+03*ortc);
    double reverse = 2.097 * exp(3.617*vlntemp - 1.071e+04*ortc);
    rr_f[738] = forward * mole_frac[0] * mole_frac[100];
    rr_r[738] = reverse * mole_frac[1];
  }
  // 739)  O + IC8H18 <=> OH + CC8H17
  {
    double forward = 3.83e+05 * exp(2.41*vlntemp - 1.14e+03*ortc);
    double reverse = 0.7003 * exp(3.607*vlntemp - 7.858e+03*ortc);
    rr_f[739] = forward * mole_frac[2] * mole_frac[100];
    rr_r[739] = reverse * mole_frac[4];
  }
  // 740)  OH + IC8H18 <=> H2O + CC8H17
  {
    double forward = 1.7e+06 * exp(1.9*vlntemp + 1.45e+03*ortc);
    double reverse = 63.01 * exp(3.011*vlntemp - 2.157e+04*ortc);
    rr_f[740] = forward * mole_frac[4] * mole_frac[100];
    rr_r[740] = reverse * mole_frac[5];
  }
  // 741)  CH3 + IC8H18 <=> CH4 + CC8H17
  {
    double forward = 6.01e-10 * exp(6.36*vlntemp - 893.0*ortc);
    double reverse = 1.911e-12 * exp(7.131*vlntemp - 1.056e+04*ortc);
    rr_f[741] = forward * mole_frac[17] * mole_frac[100];
    rr_r[741] = reverse * mole_frac[16];
  }
  // 742)  HO2 + IC8H18 <=> H2O2 + CC8H17
  {
    double forward = 433.2 * exp(3.01*vlntemp - 1.209e+04*ortc);
    double xik = -cgspl[6] + cgspl[7] - cgspl[147] + cgspl[148];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[742] = forward * mole_frac[6] * mole_frac[100];
    rr_r[742] = reverse * mole_frac[7];
  }
  // 743)  CH3O + IC8H18 <=> CH3OH + CC8H17
  {
    double forward = 1.9e+10 * exp(-2.8e+03*ortc);
    double reverse = 1.0e+10 * exp(-5.2e+03*ortc);
    rr_f[743] = forward * mole_frac[100];
    rr_r[743] = reverse * mole_frac[13];
  }
  // 744)  O2 + IC8H18 <=> HO2 + CC8H17
  {
    double forward = 7.0e+12 * exp(-4.606e+04*ortc);
    double reverse = 1.279e+08 * exp(0.869*vlntemp + 499.0*ortc);
    rr_f[744] = forward * mole_frac[3] * mole_frac[100];
    rr_r[744] = reverse * mole_frac[6];
  }
  // 745)  C2H5 + IC8H18 <=> C2H6 + CC8H17
  {
    double forward = 1.0e+11 * exp(-7.9e+03*ortc);
    double reverse = 3.0e+11 * exp(-2.1e+04*ortc);
    rr_f[745] = forward * mole_frac[19] * mole_frac[100];
    rr_r[745] = reverse * mole_frac[18];
  }
  // 746)  C2H3 + IC8H18 <=> C2H4 + CC8H17
  {
    double forward = 2.0e+11 * exp(-1.43e+04*ortc);
    double reverse = 2.5e+12 * exp(-2.3e+04*ortc);
    rr_f[746] = forward * mole_frac[21] * mole_frac[100];
    rr_r[746] = reverse * mole_frac[20];
  }
  // 747)  TC4H9 + IC4H8 <=> CC8H17
  {
    double forward = 609.0 * exp(2.48*vlntemp - 6.13e+03*ortc);
    double reverse = 6.245e+14 * exp(-0.14*vlntemp - 2.589e+04*ortc);
    rr_f[747] = forward * mole_frac[51];
    rr_r[747] = reverse;
  }
  // 748)  CC8H17 <=> H + IC8H16
  {
    double forward = 8.995e+11 * exp(0.596*vlntemp - 3.715e+04*ortc);
    double reverse = 1.06e+12 * exp(0.5*vlntemp - 1.23e+03*ortc);
    rr_f[748] = forward;
    rr_r[748] = reverse * mole_frac[0] * mole_frac[101];
  }
  // 749)  CC8H17 <=> H + JC8H16
  {
    double forward = 4.213e+11 * exp(0.777*vlntemp - 3.669e+04*ortc);
    double reverse = 1.06e+12 * exp(0.5*vlntemp - 1.23e+03*ortc);
    rr_f[749] = forward;
    rr_r[749] = reverse * mole_frac[0] * mole_frac[102];
  }
  // 750)  O2 + CC8H17 <=> HO2 + IC8H16
  {
    double forward = 3.0e-19 * exp(-5.0e+03*ortc);
    double reverse = 2.0e-19 * exp(-1.75e+04*ortc);
    rr_f[750] = forward * mole_frac[3];
    rr_r[750] = reverse * mole_frac[6] * mole_frac[101];
  }
  // 751)  O2 + CC8H17 <=> HO2 + JC8H16
  {
    double forward = 1.5e-19 * exp(-4.0e+03*ortc);
    double reverse = 2.0e-19 * exp(-1.75e+04*ortc);
    rr_f[751] = forward * mole_frac[3];
    rr_r[751] = reverse * mole_frac[6] * mole_frac[102];
  }
  // 752)  OH + IC8H16 <=> CH3COCH3 + NEOC5H11
  {
    double forward = 1.0e+11 * exp(4.0e+03*ortc);
    double reverse = 0.0;
    rr_f[752] = forward * mole_frac[4] * mole_frac[101];
    rr_r[752] = reverse * mole_frac[30] * mole_frac[98];
  }
  // 753)  OH + JC8H16 <=> CH2O + PC7H15
  {
    double forward = 1.0e+11 * exp(4.0e+03*ortc);
    double reverse = 0.0;
    rr_f[753] = forward * mole_frac[4] * mole_frac[102];
    rr_r[753] = reverse * mole_frac[10];
  }
  // 754)  CH3O2 + IC8H18 <=> CH3O2H + CC8H17
  {
    double forward = 136.6 * exp(3.12*vlntemp - 1.319e+04*ortc);
    double xik = cgspl[17] - cgspl[18] - cgspl[147] + cgspl[148];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[754] = forward * mole_frac[15] * mole_frac[100];
    rr_r[754] = reverse * mole_frac[14];
  }
  // 755)  CC8H17O2 <=> O2 + CC8H17
  {
    double forward = 3.62e+24 * exp(-2.56*vlntemp - 3.601e+04*ortc);
    double reverse = 1.41e+13;
    rr_f[755] = forward * mole_frac[104];
    rr_r[755] = reverse * mole_frac[3];
  }
  // 756)  BC8H17O2 <=> BC8H16OOH-C
  {
    double forward = 1.0e+11 * exp(-2.37e+04*ortc);
    double xik = -cgspl[151] + cgspl[154];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[756] = forward * mole_frac[103];
    rr_r[756] = reverse;
  }
  // 757)  BC8H17O2 <=> HO2 + IC8H16
  {
    double forward = 8.530000000000001e+35 * exp(-7.22*vlntemp - 
      4.149e+04*ortc); 
    double xik = cgspl[6] + cgspl[149] - cgspl[151];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[757] = forward * mole_frac[103];
    rr_r[757] = reverse * mole_frac[6] * mole_frac[101];
  }
  // 758)  CC8H17O2 <=> HO2 + IC8H16
  {
    double forward = 1.0044e+39 * exp(-8.109999999999999*vlntemp - 
      4.249e+04*ortc); 
    double xik = cgspl[6] + cgspl[149] - cgspl[152];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[758] = forward * mole_frac[104];
    rr_r[758] = reverse * mole_frac[6] * mole_frac[101];
  }
  // 759)  CC8H17O2 <=> HO2 + JC8H16
  {
    double forward = 2.015e+43 * exp(-9.41*vlntemp - 4.349e+04*ortc);
    double xik = cgspl[6] + cgspl[150] - cgspl[152];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[759] = forward * mole_frac[104];
    rr_r[759] = reverse * mole_frac[6] * mole_frac[102];
  }
  // 760)  BC8H16OOH-C <=> HO2 + IC8H16
  {
    double forward = 1.526e+20 * exp(-2.395*vlntemp - 2.44e+04*ortc);
    double reverse = 1.0e+11 * exp(-1.29e+04*ortc);
    rr_f[760] = forward;
    rr_r[760] = reverse * mole_frac[6] * mole_frac[101];
  }
  // 761)  DC8H16OOH-C <=> HO2 + JC8H16
  {
    double forward = 1.883e+18 * exp(-1.821*vlntemp - 1.496e+04*ortc);
    double reverse = 1.0e+11 * exp(-9.6e+03*ortc);
    rr_f[761] = forward;
    rr_r[761] = reverse * mole_frac[6] * mole_frac[102];
  }
  // 762)  AC8H16OOH-A <=> OH + CH2O + XC7H14
  {
    double forward = 9.087e+17 * exp(-1.26*vlntemp - 2.858e+04*ortc);
    double reverse = 0.0;
    rr_f[762] = forward;
    rr_r[762] = reverse * mole_frac[4] * mole_frac[10] * mole_frac[90];
  }
  // 763)  AC8H16OOH-A <=> TC4H9 + IC4H7OOH
  {
    double forward = 1.513e+24 * exp(-3.08*vlntemp - 2.684e+04*ortc);
    double reverse = 1.25e+11 * exp(-1.23e+04*ortc);
    rr_f[763] = forward;
    rr_r[763] = reverse * mole_frac[62];
  }
  // 764)  OH + IC8ETERAB <=> H2O + HCO + YC7H14
  {
    double forward = 1.25e+12;
    double reverse = 0.0;
    rr_f[764] = forward * mole_frac[4] * mole_frac[105];
    rr_r[764] = reverse * mole_frac[5] * mole_frac[11] * mole_frac[91];
  }
  // 765)  OH + IC8ETERBD <=> H2O + C3H6 + TC4H9CO
  {
    double forward = 1.25e+12;
    double reverse = 0.0;
    rr_f[765] = forward * mole_frac[4] * mole_frac[106];
    rr_r[765] = reverse * mole_frac[5] * mole_frac[36];
  }
  // 766)  HO2 + IC8ETERAB <=> H2O2 + HCO + YC7H14
  {
    double forward = 2.5e+12 * exp(-1.77e+04*ortc);
    double reverse = 0.0;
    rr_f[766] = forward * mole_frac[6] * mole_frac[105];
    rr_r[766] = reverse * mole_frac[7] * mole_frac[11] * mole_frac[91];
  }
  // 767)  HO2 + IC8ETERBD <=> H2O2 + C3H6 + TC4H9CO
  {
    double forward = 2.5e+12 * exp(-1.77e+04*ortc);
    double reverse = 0.0;
    rr_f[767] = forward * mole_frac[6] * mole_frac[106];
    rr_r[767] = reverse * mole_frac[7] * mole_frac[36];
  }
  // 768)  IC8KETDB <=> OH + CH3CHCHO + TC4H9CHO
  {
    double forward = 1.0e+16 * exp(-3.9e+04*ortc);
    double reverse = 0.0;
    rr_f[768] = forward * mole_frac[107];
    rr_r[768] = reverse * mole_frac[4] * mole_frac[88] * mole_frac[99];
  }
  // 769)  CH2O + C3H5-A <=> HCO + C3H6
  {
    double forward = 6.3e+08 * exp(1.9*vlntemp - 1.819e+04*ortc);
    double reverse = 9.165e+06 * exp(2.171*vlntemp - 1.77e+04*ortc);
    rr_f[769] = forward * mole_frac[10] * mole_frac[37];
    rr_r[769] = reverse * mole_frac[11] * mole_frac[36];
  }
  // 770)  C7H15O2-3 <=> ISO012
  {
    double forward = 2.0e+11 * exp(-2.645e+04*ortc);
    double xik = -cgspl[110] + cgspl[168];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[770] = forward * mole_frac[76];
    rr_r[770] = reverse;
  }
  // 771)  C7H15O2-3 <=> ISO012
  {
    double forward = 2.0e+11 * exp(-2.645e+04*ortc);
    double xik = -cgspl[110] + cgspl[168];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[771] = forward * mole_frac[76];
    rr_r[771] = reverse;
  }
  // 772)  HO2 + C7H14-2 <=> ISO012
  {
    double forward = 2.7e+03 * exp(2.5*vlntemp - 1.05e+04*ortc);
    double xik = -cgspl[6] - cgspl[104] + cgspl[168];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[772] = forward * mole_frac[6] * mole_frac[71];
    rr_r[772] = reverse;
  }
  // 773)  HO2 + C7H14-3 <=> ISO012
  {
    double forward = 2.7e+03 * exp(2.5*vlntemp - 1.05e+04*ortc);
    double xik = -cgspl[6] - cgspl[105] + cgspl[168];
    double reverse = forward * MIN(exp(xik*otc) * prt,1e200);
    rr_f[773] = forward * mole_frac[6] * mole_frac[72];
    rr_r[773] = reverse;
  }
  // 774)  ISO010 <=> O2 + ISO012
  {
    double forward = 5.5232e+22 * exp(-2.38*vlntemp - 3.76e+04*ortc);
    double reverse = 3.9533e+12;
    rr_f[774] = forward * mole_frac[112];
    rr_r[774] = reverse * mole_frac[3];
  }
  // 775)  ISO010 <=> O2 + ISO012
  {
    double forward = 5.2429e+22 * exp(-2.38*vlntemp - 3.76e+04*ortc);
    double reverse = 3.5867e+12;
    rr_f[775] = forward * mole_frac[112];
    rr_r[775] = reverse * mole_frac[3];
  }
  // 776)  ISO011 <=> O2 + C7H14OOH3-5
  {
    double forward = 1.2212e+23 * exp(-2.38*vlntemp - 3.76e+04*ortc);
    double reverse = 7.54e+12;
    rr_f[776] = forward * mole_frac[113];
    rr_r[776] = reverse * mole_frac[3];
  }
  // 777)  C7H14OOH2-3O2 <=> OH + ISO014
  {
    double forward = 1.0e+11 * exp(-2.345e+04*ortc);
    double xik = cgspl[4] - cgspl[120] + cgspl[169];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[777] = forward * mole_frac[79];
    rr_r[777] = reverse * mole_frac[4] * mole_frac[114];
  }
  // 778)  C7H14OOH2-4O2 <=> OH + ISO014
  {
    double forward = 1.25e+10 * exp(-1.745e+04*ortc);
    double xik = cgspl[4] - cgspl[121] + cgspl[169];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[778] = forward * mole_frac[80];
    rr_r[778] = reverse * mole_frac[4] * mole_frac[114];
  }
  // 779)  ISO011 <=> OH + ISO014
  {
    double forward = 1.5101e+09 * exp(-1.745e+04*ortc);
    double xik = cgspl[4] - cgspl[167] + cgspl[169];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[779] = forward * mole_frac[113];
    rr_r[779] = reverse * mole_frac[4] * mole_frac[114];
  }
  // 780)  ISO010 <=> OH + ISO014
  {
    double forward = 3.9764e+10 * exp(-2.345e+04*ortc);
    double xik = cgspl[4] - cgspl[166] + cgspl[169];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[780] = forward * mole_frac[112];
    rr_r[780] = reverse * mole_frac[4] * mole_frac[114];
  }
  // 781)  ISO010 <=> OH + ISO014
  {
    double forward = 3.7746e+10 * exp(-2.345e+04*ortc);
    double xik = cgspl[4] - cgspl[166] + cgspl[169];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[781] = forward * mole_frac[112];
    rr_r[781] = reverse * mole_frac[4] * mole_frac[114];
  }
  // 782)  ISO011 <=> OH + ISO014
  {
    double forward = 1.099e+10 * exp(-1.745e+04*ortc);
    double xik = cgspl[4] - cgspl[167] + cgspl[169];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[782] = forward * mole_frac[113];
    rr_r[782] = reverse * mole_frac[4] * mole_frac[114];
  }
  // 783)  C7H14OOH4-2O2 <=> OH + ISO014
  {
    double forward = 1.25e+10 * exp(-1.745e+04*ortc);
    double xik = cgspl[4] - cgspl[122] + cgspl[169];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[783] = forward * mole_frac[81];
    rr_r[783] = reverse * mole_frac[4] * mole_frac[114];
  }
  // 784)  C7H14OOH4-3O2 <=> OH + ISO014
  {
    double forward = 1.0e+11 * exp(-2.345e+04*ortc);
    double xik = cgspl[4] - cgspl[123] + cgspl[169];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[784] = forward * mole_frac[82];
    rr_r[784] = reverse * mole_frac[4] * mole_frac[114];
  }
  // 785)  ISO014 <=> OH + CH3CO + NC4H9CHO
  {
    double forward = 2.7036e+14 * exp(-3.9e+04*ortc);
    double reverse = 0.0;
    rr_f[785] = forward * mole_frac[114];
    rr_r[785] = reverse * mole_frac[4] * mole_frac[68];
  }
  // 786)  ISO014 <=> OH + CH3COCH2 + NC3H7CHO
  {
    double forward = 3.1419e+15 * exp(-3.9e+04*ortc);
    double reverse = 0.0;
    rr_f[786] = forward * mole_frac[114];
    rr_r[786] = reverse * mole_frac[4] * mole_frac[50];
  }
  // 787)  ISO014 <=> OH + CH3CHO + NC4H9CO
  {
    double forward = 2.3661e+14 * exp(-3.9e+04*ortc);
    double reverse = 0.0;
    rr_f[787] = forward * mole_frac[114];
    rr_r[787] = reverse * mole_frac[4] * mole_frac[23];
  }
  // 788)  ISO014 <=> OH + C2H5CO + NC3H7CHO
  {
    double forward = 2.2664e+14 * exp(-3.9e+04*ortc);
    double reverse = 0.0;
    rr_f[788] = forward * mole_frac[114];
    rr_r[788] = reverse * mole_frac[4] * mole_frac[50];
  }
  // 789)  ISO014 <=> OH + C2H5CHO + C2H5COCH2
  {
    double forward = 2.8731e+15 * exp(-3.9e+04*ortc);
    double reverse = 0.0;
    rr_f[789] = forward * mole_frac[114];
    rr_r[789] = reverse * mole_frac[4] * mole_frac[35] * mole_frac[49];
  }
  // 790)  ISO014 <=> OH + C2H5CHO + NC3H7CO
  {
    double forward = 1.9312e+14 * exp(-3.9e+04*ortc);
    double reverse = 0.0;
    rr_f[790] = forward * mole_frac[114];
    rr_r[790] = reverse * mole_frac[4] * mole_frac[35];
  }
  // 791)  IC8H18 <=> H + ISO007
  {
    double forward = 5.748e+17 * exp(-0.36*vlntemp - 1.012e+05*ortc);
    double reverse = 4.0984e+13;
    rr_f[791] = forward * mole_frac[100];
    rr_r[791] = reverse * mole_frac[0];
  }
  // 792)  IC8H18 <=> H + ISO007
  {
    double forward = 3.299e+18 * exp(-0.721*vlntemp - 9.873e+04*ortc);
    double reverse = 2.3649e+13;
    rr_f[792] = forward * mole_frac[100];
    rr_r[792] = reverse * mole_frac[0];
  }
  // 793)  IC8H18 <=> H + ISO007
  {
    double forward = 1.919e+17 * exp(-0.36*vlntemp - 1.004e+05*ortc);
    double reverse = 3.5366e+13;
    rr_f[793] = forward * mole_frac[100];
    rr_r[793] = reverse * mole_frac[0];
  }
  // 794)  H + IC8H18 <=> H2 + ISO007
  {
    double forward = 7.341e+05 * exp(2.768*vlntemp - 8.147e+03*ortc);
    double reverse = 20.902 * exp(3.404*vlntemp - 1.048e+04*ortc);
    rr_f[794] = forward * mole_frac[0] * mole_frac[100];
    rr_r[794] = reverse * mole_frac[1];
  }
  // 795)  H + IC8H18 <=> H2 + ISO007
  {
    double forward = 5.736e+05 * exp(2.491*vlntemp - 4.124e+03*ortc);
    double reverse = 1.6417 * exp(3.488*vlntemp - 8.954e+03*ortc);
    rr_f[795] = forward * mole_frac[0] * mole_frac[100];
    rr_r[795] = reverse * mole_frac[1];
  }
  // 796)  H + IC8H18 <=> H2 + ISO007
  {
    double forward = 1.88e+05 * exp(2.75*vlntemp - 6.28e+03*ortc);
    double reverse = 13.832 * exp(3.386*vlntemp - 9.417e+03*ortc);
    rr_f[796] = forward * mole_frac[0] * mole_frac[100];
    rr_r[796] = reverse * mole_frac[1];
  }
  // 797)  O + IC8H18 <=> OH + ISO007
  {
    double forward = 8.55e+03 * exp(3.05*vlntemp - 3.123e+03*ortc);
    double reverse = 0.12779 * exp(3.666*vlntemp - 4.048e+03*ortc);
    rr_f[797] = forward * mole_frac[2] * mole_frac[100];
    rr_r[797] = reverse * mole_frac[4];
  }
  // 798)  O + IC8H18 <=> OH + ISO007
  {
    double forward = 4.77e+04 * exp(2.71*vlntemp - 2.106e+03*ortc);
    double reverse = 0.071658 * exp(3.687*vlntemp - 5.524e+03*ortc);
    rr_f[798] = forward * mole_frac[2] * mole_frac[100];
    rr_r[798] = reverse * mole_frac[4];
  }
  // 799)  O + IC8H18 <=> OH + ISO007
  {
    double forward = 2.853e+05 * exp(2.5*vlntemp - 3.645e+03*ortc);
    double reverse = 11.02 * exp(3.116*vlntemp - 5.37e+03*ortc);
    rr_f[799] = forward * mole_frac[2] * mole_frac[100];
    rr_r[799] = reverse * mole_frac[4];
  }
  // 800)  OH + IC8H18 <=> H2O + ISO007
  {
    double forward = 2.63e+07 * exp(1.8*vlntemp - 1.431e+03*ortc);
    double xik = -cgspl[4] + cgspl[5] - cgspl[147] + cgspl[164];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[800] = forward * mole_frac[4] * mole_frac[100];
    rr_r[800] = reverse * mole_frac[5];
  }
  // 801)  OH + IC8H18 <=> H2O + ISO007
  {
    double forward = 9.0e+05 * temperature * temperature * exp(1.133e+03*ortc);
    double reverse = 27.41 * exp(2.891*vlntemp - 1.859e+04*ortc);
    rr_f[801] = forward * mole_frac[4] * mole_frac[100];
    rr_r[801] = reverse * mole_frac[5];
  }
  // 802)  OH + IC8H18 <=> H2O + ISO007
  {
    double forward = 1.78e+07 * exp(1.8*vlntemp - 1.431e+03*ortc);
    double reverse = 1.3934e+04 * exp(2.33*vlntemp - 1.946e+04*ortc);
    rr_f[802] = forward * mole_frac[4] * mole_frac[100];
    rr_r[802] = reverse * mole_frac[5];
  }
  // 803)  CH3 + IC8H18 <=> CH4 + ISO007
  {
    double forward = 4.257e-14 * exp(8.06*vlntemp - 4.154e+03*ortc);
    double reverse = 1.1062e-15 * exp(8.25*vlntemp - 8.031e+03*ortc);
    rr_f[803] = forward * mole_frac[17] * mole_frac[100];
    rr_r[803] = reverse * mole_frac[16];
  }
  // 804)  CH3 + IC8H18 <=> CH4 + ISO007
  {
    double forward = 2.705e+04 * exp(2.26*vlntemp - 7.287e+03*ortc);
    double reverse = 70.66500000000001 * exp(2.811*vlntemp - 1.366e+04*ortc);
    rr_f[804] = forward * mole_frac[17] * mole_frac[100];
    rr_r[804] = reverse * mole_frac[16];
  }
  // 805)  CH3 + IC8H18 <=> CH4 + ISO007
  {
    double forward = 0.147 * exp(3.87*vlntemp - 6.808e+03*ortc);
    double reverse = 9.870800000000001e-03 * exp(4.06*vlntemp - 1.148e+04*ortc);
    rr_f[805] = forward * mole_frac[17] * mole_frac[100];
    rr_r[805] = reverse * mole_frac[16];
  }
  // 806)  HO2 + IC8H18 <=> H2O2 + ISO007
  {
    double forward = 61.2 * exp(3.59*vlntemp - 1.716e+04*ortc);
    double xik = -cgspl[6] + cgspl[7] - cgspl[147] + cgspl[164];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[806] = forward * mole_frac[6] * mole_frac[100];
    rr_r[806] = reverse * mole_frac[7];
  }
  // 807)  HO2 + IC8H18 <=> H2O2 + ISO007
  {
    double forward = 63.2 * exp(3.37*vlntemp - 1.372e+04*ortc);
    double xik = -cgspl[6] + cgspl[7] - cgspl[147] + cgspl[164];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[807] = forward * mole_frac[6] * mole_frac[100];
    rr_r[807] = reverse * mole_frac[7];
  }
  // 808)  HO2 + IC8H18 <=> H2O2 + ISO007
  {
    double forward = 40.8 * exp(3.59*vlntemp - 1.716e+04*ortc);
    double xik = -cgspl[6] + cgspl[7] - cgspl[147] + cgspl[164];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[808] = forward * mole_frac[6] * mole_frac[100];
    rr_r[808] = reverse * mole_frac[7];
  }
  // 809)  CH3O + IC8H18 <=> CH3OH + ISO007
  {
    double forward = 4.74e+11 * exp(-7.0e+03*ortc);
    double reverse = 4.9181e+09 * exp(-9.2e+03*ortc);
    rr_f[809] = forward * mole_frac[100];
    rr_r[809] = reverse * mole_frac[13];
  }
  // 810)  CH3O + IC8H18 <=> CH3OH + ISO007
  {
    double forward = 1.095e+11 * exp(-5.0e+03*ortc);
    double reverse = 2.1048e+09 * exp(-7.2e+03*ortc);
    rr_f[810] = forward * mole_frac[100];
    rr_r[810] = reverse * mole_frac[13];
  }
  // 811)  CH3O + IC8H18 <=> CH3OH + ISO007
  {
    double forward = 3.2e+11 * exp(-7.0e+03*ortc);
    double reverse = 4.244e+09 * exp(-9.2e+03*ortc);
    rr_f[811] = forward * mole_frac[100];
    rr_r[811] = reverse * mole_frac[13];
  }
  // 812)  O2 + IC8H18 <=> HO2 + ISO007
  {
    double forward = 6.3e+13 * exp(-5.076e+04*ortc);
    double reverse = 9.4099e+09 * exp(0.288*vlntemp + 1.592e+03*ortc);
    rr_f[812] = forward * mole_frac[3] * mole_frac[100];
    rr_r[812] = reverse * mole_frac[6];
  }
  // 813)  O2 + IC8H18 <=> HO2 + ISO007
  {
    double forward = 1.4e+13 * exp(-4.821e+04*ortc);
    double reverse = 2.1022e+08 * exp(0.649*vlntemp + 1.649e+03*ortc);
    rr_f[813] = forward * mole_frac[3] * mole_frac[100];
    rr_r[813] = reverse * mole_frac[6];
  }
  // 814)  O2 + IC8H18 <=> HO2 + ISO007
  {
    double forward = 4.2e+13 * exp(-5.076e+04*ortc);
    double reverse = 1.6212e+10 * exp(0.288*vlntemp + 792.0*ortc);
    rr_f[814] = forward * mole_frac[3] * mole_frac[100];
    rr_r[814] = reverse * mole_frac[6];
  }
  // 815)  C2H5 + IC8H18 <=> C2H6 + ISO007
  {
    double forward = 1.5e+11 * exp(-1.34e+04*ortc);
    double reverse = 1.3115e+11 * exp(-1.23e+04*ortc);
    rr_f[815] = forward * mole_frac[19] * mole_frac[100];
    rr_r[815] = reverse * mole_frac[18];
  }
  // 816)  C2H5 + IC8H18 <=> C2H6 + ISO007
  {
    double forward = 5.0e+10 * exp(-1.04e+04*ortc);
    double reverse = 2.3649e+10 * exp(-1.29e+04*ortc);
    rr_f[816] = forward * mole_frac[19] * mole_frac[100];
    rr_r[816] = reverse * mole_frac[18];
  }
  // 817)  C2H5 + IC8H18 <=> C2H6 + ISO007
  {
    double forward = 1.0e+11 * exp(-1.34e+04*ortc);
    double reverse = 1.1317e+11 * exp(-1.23e+04*ortc);
    rr_f[817] = forward * mole_frac[19] * mole_frac[100];
    rr_r[817] = reverse * mole_frac[18];
  }
  // 818)  C2H3 + IC8H18 <=> C2H4 + ISO007
  {
    double forward = 1.5e+12 * exp(-1.8e+04*ortc);
    double reverse = 1.0533e+12 * exp(-2.54e+04*ortc);
    rr_f[818] = forward * mole_frac[21] * mole_frac[100];
    rr_r[818] = reverse * mole_frac[20];
  }
  // 819)  C2H3 + IC8H18 <=> C2H4 + ISO007
  {
    double forward = 4.0e+11 * exp(-1.68e+04*ortc);
    double reverse = 4.7299e+11 * exp(-2.42e+04*ortc);
    rr_f[819] = forward * mole_frac[21] * mole_frac[100];
    rr_r[819] = reverse * mole_frac[20];
  }
  // 820)  C2H3 + IC8H18 <=> C2H4 + ISO007
  {
    double forward = 1.0e+12 * exp(-1.8e+04*ortc);
    double reverse = 9.0892e+11 * exp(-2.54e+04*ortc);
    rr_f[820] = forward * mole_frac[21] * mole_frac[100];
    rr_r[820] = reverse * mole_frac[20];
  }
  // 821)  CH3 + XC7H14 <=> ISO007
  {
    double forward = 1.3e+03 * exp(2.5*vlntemp - 8.52e+03*ortc);
    double reverse = 5.1394e+12 * exp(0.3*vlntemp - 2.832e+04*ortc);
    rr_f[821] = forward * mole_frac[17] * mole_frac[90];
    rr_r[821] = reverse;
  }
  // 822)  IC4H9 + IC4H8 <=> ISO007
  {
    double forward = 609.0 * exp(2.48*vlntemp - 8.52e+03*ortc);
    double reverse = 1.0074e+14 * exp(-0.14*vlntemp - 2.678e+04*ortc);
    rr_f[822] = forward * mole_frac[51];
    rr_r[822] = reverse;
  }
  // 823)  CH3 + YC7H14 <=> ISO007
  {
    double forward = 1.3e+03 * exp(2.5*vlntemp - 8.52e+03*ortc);
    double reverse = 1.112e+12 * exp(0.48*vlntemp - 2.821e+04*ortc);
    rr_f[823] = forward * mole_frac[17] * mole_frac[91];
    rr_r[823] = reverse;
  }
  // 824)  C3H6 + NEOC5H11 <=> ISO007
  {
    double forward = 400.0 * exp(2.5*vlntemp - 8.52e+03*ortc);
    double reverse = 1.3206e+08 * exp(1.57*vlntemp - 2.702e+04*ortc);
    rr_f[824] = forward * mole_frac[36] * mole_frac[98];
    rr_r[824] = reverse;
  }
  // 825)  ISO007 <=> H + IC8H16
  {
    double forward = 4.3586e+11 * exp(0.376*vlntemp - 3.524e+04*ortc);
    double reverse = 6.25e+11 * exp(0.5*vlntemp - 2.62e+03*ortc);
    rr_f[825] = forward;
    rr_r[825] = reverse * mole_frac[0] * mole_frac[101];
  }
  // 826)  ISO007 <=> H + JC8H16
  {
    double forward = 5.2484e+12 * exp(0.196*vlntemp - 3.309e+04*ortc);
    double reverse = 6.25e+11 * exp(0.5*vlntemp - 2.62e+03*ortc);
    rr_f[826] = forward;
    rr_r[826] = reverse * mole_frac[0] * mole_frac[102];
  }
  // 827)  ISO007 <=> CC8H17
  {
    double forward = 1.5197e+11 * exp(-2.04e+04*ortc);
    double reverse = 1.859e+10 * exp(0.581*vlntemp - 2.619e+04*ortc);
    rr_f[827] = forward;
    rr_r[827] = reverse;
  }
  // 828)  O2 + ISO007 <=> HO2 + JC8H16
  {
    double forward = 7.0733e-19 * exp(-5.0e+03*ortc);
    double reverse = 2.0e-19 * exp(-1.75e+04*ortc);
    rr_f[828] = forward * mole_frac[3];
    rr_r[828] = reverse * mole_frac[6] * mole_frac[102];
  }
  // 829)  CH3O2 + IC8H18 <=> CH3O2H + ISO007
  {
    double forward = 2.079 * exp(3.97*vlntemp - 1.828e+04*ortc);
    double xik = cgspl[17] - cgspl[18] - cgspl[147] + cgspl[164];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[829] = forward * mole_frac[15] * mole_frac[100];
    rr_r[829] = reverse * mole_frac[14];
  }
  // 830)  CH3O2 + IC8H18 <=> CH3O2H + ISO007
  {
    double forward = 10.165 * exp(3.58*vlntemp - 1.481e+04*ortc);
    double xik = cgspl[17] - cgspl[18] - cgspl[147] + cgspl[164];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[830] = forward * mole_frac[15] * mole_frac[100];
    rr_r[830] = reverse * mole_frac[14];
  }
  // 831)  CH3O2 + IC8H18 <=> CH3O2H + ISO007
  {
    double forward = 1.386 * exp(3.97*vlntemp - 1.828e+04*ortc);
    double xik = cgspl[17] - cgspl[18] - cgspl[147] + cgspl[164];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[831] = forward * mole_frac[15] * mole_frac[100];
    rr_r[831] = reverse * mole_frac[14];
  }
  // 832)  ISO004 <=> O2 + ISO007
  {
    double forward = 2.1233e+20 * exp(-1.653*vlntemp - 3.572e+04*ortc);
    double reverse = 1.8525e+12;
    rr_f[832] = forward * mole_frac[110];
    rr_r[832] = reverse * mole_frac[3];
  }
  // 833)  BC8H17O2 <=> O2 + ISO007
  {
    double forward = 1.046e+23 * exp(-2.323*vlntemp - 3.884e+04*ortc);
    double reverse = 1.7832e+12;
    rr_f[833] = forward * mole_frac[103];
    rr_r[833] = reverse * mole_frac[3];
  }
  // 834)  ISO004 <=> O2 + ISO007
  {
    double forward = 1.3417e+20 * exp(-1.653*vlntemp - 3.492e+04*ortc);
    double reverse = 1.5986e+12;
    rr_f[834] = forward * mole_frac[110];
    rr_r[834] = reverse * mole_frac[3];
  }
  // 835)  ISO004 <=> AC8H16OOH-A
  {
    double forward = 4.5958e+10 * exp(-2.4e+04*ortc);
    double xik = cgspl[153] - cgspl[161];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[835] = forward * mole_frac[110];
    rr_r[835] = reverse;
  }
  // 836)  ISO004 <=> ISO005
  {
    double forward = 1.5319e+10 * exp(-2.045e+04*ortc);
    double xik = -cgspl[161] + cgspl[162];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[836] = forward * mole_frac[110];
    rr_r[836] = reverse;
  }
  // 837)  BC8H17O2 <=> ISO006
  {
    double forward = 1.125e+11 * exp(-2.4e+04*ortc);
    double xik = -cgspl[151] + cgspl[163];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[837] = forward * mole_frac[103];
    rr_r[837] = reverse;
  }
  // 838)  BC8H17O2 <=> ISO006
  {
    double forward = 7.5e+10 * exp(-2.4e+04*ortc);
    double xik = -cgspl[151] + cgspl[163];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[838] = forward * mole_frac[103];
    rr_r[838] = reverse;
  }
  // 839)  ISO004 <=> ISO005
  {
    double forward = 9.6805e+09 * exp(-2.045e+04*ortc);
    double xik = -cgspl[161] + cgspl[162];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[839] = forward * mole_frac[110];
    rr_r[839] = reverse;
  }
  // 840)  ISO004 <=> DC8H16OOH-C
  {
    double forward = 3.8722e+10 * exp(-2.37e+04*ortc);
    double xik = cgspl[155] - cgspl[161];
    double reverse = forward * MIN(exp(xik*otc),1e200);
    rr_f[840] = forward * mole_frac[110];
    rr_r[840] = reverse;
  }
  // 841)  ISO004 <=> HO2 + JC8H16
  {
    double forward = 3.303e+35 * exp(-7.22*vlntemp - 4.149e+04*ortc);
    double xik = cgspl[6] + cgspl[150] - cgspl[161];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[841] = forward * mole_frac[110];
    rr_r[841] = reverse * mole_frac[6] * mole_frac[102];
  }
  // 842)  ISO005 <=> OH + IC8ETERAB
  {
    double forward = 2.0763e+11 * exp(-1.425e+04*ortc);
    double reverse = 0.0;
    rr_f[842] = forward;
    rr_r[842] = reverse * mole_frac[4] * mole_frac[105];
  }
  // 843)  ISO006 <=> OH + IC8ETERAB
  {
    double forward = 1.5385e+11 * exp(-1.425e+04*ortc);
    double reverse = 0.0;
    rr_f[843] = forward;
    rr_r[843] = reverse * mole_frac[4] * mole_frac[105];
  }
  // 844)  ISO006 <=> OH + IC8ETERBD
  {
    double forward = 1.4615e+11 * exp(-1.425e+04*ortc);
    double reverse = 0.0;
    rr_f[844] = forward;
    rr_r[844] = reverse * mole_frac[4] * mole_frac[106];
  }
  // 845)  ISO005 <=> OH + IC8ETERBD
  {
    double forward = 9.2373e+10 * exp(-1.425e+04*ortc);
    double reverse = 0.0;
    rr_f[845] = forward;
    rr_r[845] = reverse * mole_frac[4] * mole_frac[106];
  }
  // 846)  ISO005 <=> OH + CH2O + YC7H14
  {
    double forward = 8.665e+16 * exp(-1.08*vlntemp - 2.821e+04*ortc);
    double reverse = 0.0;
    rr_f[846] = forward;
    rr_r[846] = reverse * mole_frac[4] * mole_frac[10] * mole_frac[91];
  }
  // 847)  ISO006 <=> OH + IC4H8 + IC3H7CHO
  {
    double forward = 1.599e+21 * exp(-2.43*vlntemp - 2.633e+04*ortc);
    double reverse = 0.0;
    rr_f[847] = forward;
    rr_r[847] = reverse * mole_frac[4] * mole_frac[51] * mole_frac[55];
  }
  // 848)  ISO006 <=> OH + C3H6 + TC4H9CHO
  {
    double forward = 6.2992e+20 * exp(-2.2*vlntemp - 3.297e+04*ortc);
    double reverse = 0.0;
    rr_f[848] = forward;
    rr_r[848] = reverse * mole_frac[4] * mole_frac[36] * mole_frac[99];
  }
  // 849)  ISO003 <=> O2 + ISO005
  {
    double forward = 7.6395e+22 * exp(-2.357*vlntemp - 3.728e+04*ortc);
    double reverse = 5.2183e+12;
    rr_f[849] = forward * mole_frac[109];
    rr_r[849] = reverse * mole_frac[3];
  }
  // 850)  ISO002 <=> O2 + BC8H16OOH-C
  {
    double forward = 1.1251e+24 * exp(-2.437*vlntemp - 3.662e+04*ortc);
    double reverse = 1.41e+13;
    rr_f[850] = forward * mole_frac[108];
    rr_r[850] = reverse * mole_frac[3];
  }
  // 851)  ISO002 <=> O2 + ISO006
  {
    double forward = 4.4415e+19 * exp(-1.632*vlntemp - 3.49e+04*ortc);
    double reverse = 2.318e+12;
    rr_f[851] = forward * mole_frac[108];
    rr_r[851] = reverse * mole_frac[3];
  }
  // 852)  ISO002 <=> O2 + ISO006
  {
    double forward = 3.5007e+19 * exp(-1.632*vlntemp - 3.49e+04*ortc);
    double reverse = 2.202e+12;
    rr_f[852] = forward * mole_frac[108];
    rr_r[852] = reverse * mole_frac[3];
  }
  // 853)  ISO003 <=> O2 + ISO005
  {
    double forward = 5.9749e+22 * exp(-2.357*vlntemp - 3.808e+04*ortc);
    double reverse = 2.3217e+12;
    rr_f[853] = forward * mole_frac[109];
    rr_r[853] = reverse * mole_frac[3];
  }
  // 854)  ISO003 <=> OH + ISO008
  {
    double forward = 1.4033e+10 * exp(-2.1e+04*ortc);
    double xik = cgspl[4] - cgspl[160] + cgspl[165];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[854] = forward * mole_frac[109];
    rr_r[854] = reverse * mole_frac[4] * mole_frac[111];
  }
  // 855)  ISO002 <=> OH + ISO008
  {
    double forward = 7.3343e+10 * exp(-2.345e+04*ortc);
    double xik = cgspl[4] - cgspl[159] + cgspl[165];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[855] = forward * mole_frac[108];
    rr_r[855] = reverse * mole_frac[4] * mole_frac[111];
  }
  // 856)  ISO002 <=> OH + ISO008
  {
    double forward = 1.8637e+09 * exp(-1.745e+04*ortc);
    double xik = cgspl[4] - cgspl[159] + cgspl[165];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[856] = forward * mole_frac[108];
    rr_r[856] = reverse * mole_frac[4] * mole_frac[111];
  }
  // 857)  ISO002 <=> OH + ISO008
  {
    double forward = 1.4684e+09 * exp(-1.745e+04*ortc);
    double xik = cgspl[4] - cgspl[159] + cgspl[165];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[857] = forward * mole_frac[108];
    rr_r[857] = reverse * mole_frac[4] * mole_frac[111];
  }
  // 858)  ISO003 <=> OH + IC8KETDB
  {
    double forward = 1.0967e+10 * exp(-2.1e+04*ortc);
    double xik = cgspl[4] + cgspl[158] - cgspl[160];
    double reverse = forward * MIN(exp(xik*otc) * oprt,1e200);
    rr_f[858] = forward * mole_frac[109];
    rr_r[858] = reverse * mole_frac[4] * mole_frac[107];
  }
  // 859)  ISO008 <=> OH + IC3H7CHO + TC3H6CHO
  {
    double forward = 3.4459e+15 * exp(-3.9e+04*ortc);
    double reverse = 0.0;
    rr_f[859] = forward * mole_frac[111];
    rr_r[859] = reverse * mole_frac[4] * mole_frac[55] * mole_frac[56];
  }
  // 860)  ISO008 <=> OH + CH3COCH3 + TC4H9CO
  {
    double forward = 2.4516e+15 * exp(-3.9e+04*ortc);
    double reverse = 0.0;
    rr_f[860] = forward * mole_frac[111];
    rr_r[860] = reverse * mole_frac[4] * mole_frac[30];
  }
  // Unimportant reaction rates
  rr_f[68] = 0.0;
  rr_r[72] = 0.0;
  rr_r[85] = 0.0;
  rr_r[106] = 0.0;
  rr_r[109] = 0.0;
  rr_r[147] = 0.0;
  rr_r[300] = 0.0;
  rr_r[393] = 0.0;
  rr_r[399] = 0.0;
  rr_r[422] = 0.0;
  rr_r[430] = 0.0;
  rr_r[431] = 0.0;
  rr_r[432] = 0.0;
  rr_r[434] = 0.0;
  rr_r[435] = 0.0;
  rr_r[436] = 0.0;
  rr_r[437] = 0.0;
  rr_r[438] = 0.0;
  rr_r[439] = 0.0;
  rr_r[472] = 0.0;
  rr_r[544] = 0.0;
  rr_r[623] = 0.0;
  rr_r[624] = 0.0;
  rr_r[684] = 0.0;
  rr_r[707] = 0.0;
  rr_r[708] = 0.0;
  rr_r[710] = 0.0;
  rr_r[711] = 0.0;
  rr_r[736] = 0.0;
  // QSSA connected component
  {
    double a1_0, a1_2;
    {
      double den = rr_f[43] + rr_f[44] + rr_r[73];
      a1_0 = (rr_r[43] + rr_r[44])/den;
      a1_2 = (rr_f[73])/den;
    }
    double a2_0, a2_1, a2_3, a2_27;
    {
      double den = rr_f[63] + rr_f[64] + rr_f[65] + rr_f[66] + rr_f[67] + 
        rr_f[69] + rr_f[70] + rr_f[71] + rr_f[73] + rr_r[62] + rr_r[75] + 
        rr_r[77] + rr_r[78] + rr_r[80] + rr_r[81] + rr_r[82] + rr_r[83] + 
        rr_r[84] + rr_r[95] + rr_r[105] + rr_r[154] + rr_r[188] + rr_r[477] + 
        rr_r[504] + rr_r[505]; 
      a2_0 = (rr_f[62] + rr_f[72] + rr_f[72] + rr_f[75] + rr_f[77] + rr_f[78] + 
        rr_f[80] + rr_f[81] + rr_f[82] + rr_f[83] + rr_f[95] + rr_f[105] + 
        rr_f[154] + rr_f[188] + rr_f[477] + rr_r[63] + rr_r[64] + rr_r[65] + 
        rr_r[66] + rr_r[67] + rr_r[68] + rr_r[69] + rr_r[70] + rr_r[71])/den; 
      a2_1 = (rr_r[73])/den;
      a2_3 = (rr_f[84])/den;
      a2_27 = (rr_f[504] + rr_f[505])/den;
    }
    double a3_0, a3_2, a3_9, a3_11, a3_20, a3_22, a3_29, a3_30, a3_31, a3_32, 
      a3_33, a3_48, a3_54; 
    {
      double den = rr_f[55] + rr_f[56] + rr_f[57] + rr_f[58] + rr_f[59] + 
        rr_f[60] + rr_f[61] + rr_f[84] + rr_f[136] + rr_f[202] + rr_f[223] + 
        rr_f[242] + rr_f[251] + rr_f[536] + rr_f[384] + rr_f[390] + rr_f[418] + 
        rr_f[523] + rr_f[524] + rr_f[573] + rr_f[574] + rr_f[575] + rr_f[576] + 
        rr_f[615] + rr_f[621] + rr_f[700] + rr_f[743] + rr_f[809] + rr_f[810] + 
        rr_f[811] + rr_r[76] + rr_r[79] + rr_r[94] + rr_r[97] + rr_r[100] + 
        rr_r[110] + rr_r[111] + rr_r[113]; 
      a3_0 = (rr_f[76] + rr_f[79] + rr_f[85] + rr_f[85] + rr_f[94] + rr_f[97] + 
        rr_f[100] + rr_f[106] + rr_f[106] + rr_f[109] + rr_f[109] + rr_f[110] + 
        rr_f[111] + rr_f[113] + rr_f[147] + rr_f[300] + rr_f[393] + rr_f[430] + 
        rr_f[434] + rr_f[472] + rr_f[623] + rr_f[707] + rr_f[710] + rr_r[55] + 
        rr_r[56] + rr_r[57] + rr_r[58] + rr_r[59] + rr_r[60] + rr_r[61] + 
        rr_r[68] + rr_r[136] + rr_r[202] + rr_r[242] + rr_r[384] + rr_r[390] + 
        rr_r[523] + rr_r[524] + rr_r[615] + rr_r[621] + rr_r[700])/den; 
      a3_2 = (rr_r[84])/den;
      a3_9 = (rr_r[223])/den;
      a3_11 = (rr_r[251])/den;
      a3_20 = (rr_r[418])/den;
      a3_22 = (rr_f[422])/den;
      a3_29 = (rr_r[536])/den;
      a3_30 = (rr_r[573])/den;
      a3_31 = (rr_r[574])/den;
      a3_32 = (rr_r[575])/den;
      a3_33 = (rr_r[576])/den;
      a3_48 = (rr_r[743])/den;
      a3_54 = (rr_r[809] + rr_r[810] + rr_r[811])/den;
    }
    double a6_0, a6_8, a6_10, a6_15;
    {
      double den = rr_f[175] + rr_f[176] + rr_f[177] + rr_f[178] + rr_r[163] + 
        rr_r[166] + rr_r[167] + rr_r[168] + rr_r[169] + rr_r[170] + rr_r[171] + 
        rr_r[172] + rr_r[179] + rr_r[218] + rr_r[233] + rr_r[321] + rr_r[669] + 
        rr_r[673] + rr_r[785]; 
      a6_0 = (rr_f[166] + rr_f[167] + rr_f[168] + rr_f[169] + rr_f[170] + 
        rr_f[171] + rr_f[172] + rr_f[179] + rr_f[218] + rr_f[669] + rr_f[673] + 
        rr_f[684] + rr_f[785] + rr_r[175] + rr_r[176] + rr_r[177] + 
        rr_r[178])/den; 
      a6_8 = (rr_f[163])/den;
      a6_10 = (rr_f[233])/den;
      a6_15 = (rr_f[321])/den;
    }
    double a8_0, a8_6;
    {
      double den = rr_f[163] + rr_f[164];
      a8_0 = (rr_r[164])/den;
      a8_6 = (rr_r[163])/den;
    }
    double a9_0, a9_3, a9_15;
    {
      double den = rr_f[227] + rr_r[219] + rr_r[220] + rr_r[221] + rr_r[222] + 
        rr_r[223] + rr_r[224] + rr_r[225] + rr_r[226] + rr_r[228] + rr_r[229] + 
        rr_r[320] + rr_r[468] + rr_r[786]; 
      a9_0 = (rr_f[219] + rr_f[220] + rr_f[221] + rr_f[222] + rr_f[224] + 
        rr_f[225] + rr_f[226] + rr_f[228] + rr_f[229] + rr_f[468] + rr_f[786] + 
        rr_r[227])/den; 
      a9_3 = (rr_f[223])/den;
      a9_15 = (rr_f[320])/den;
    }
    double a10_0, a10_6;
    {
      double den = rr_f[233] + rr_r[232];
      a10_0 = (rr_f[232])/den;
      a10_6 = (rr_r[233])/den;
    }
    double a11_0, a11_3;
    {
      double den = rr_f[257] + rr_r[246] + rr_r[247] + rr_r[248] + rr_r[249] + 
        rr_r[250] + rr_r[251] + rr_r[252] + rr_r[253] + rr_r[254] + rr_r[255] + 
        rr_r[256] + rr_r[267] + rr_r[450] + rr_r[670] + rr_r[674] + rr_r[788]; 
      a11_0 = (rr_f[246] + rr_f[247] + rr_f[248] + rr_f[249] + rr_f[250] + 
        rr_f[252] + rr_f[253] + rr_f[254] + rr_f[255] + rr_f[256] + rr_f[267] + 
        rr_f[450] + rr_f[670] + rr_f[674] + rr_f[788] + rr_r[257])/den; 
      a11_3 = (rr_f[251])/den;
    }
    double a13_0, a13_20, a13_32, a13_36;
    {
      double den = rr_f[264] + rr_f[265] + rr_f[266] + rr_r[362] + rr_r[420] + 
        rr_r[528] + rr_r[595] + rr_r[632] + rr_r[661]; 
      a13_0 = (rr_f[362] + rr_f[528] + rr_f[544] + rr_f[632] + rr_f[684] + 
        rr_r[264] + rr_r[265] + rr_r[266])/den; 
      a13_20 = (rr_f[420])/den;
      a13_32 = (rr_f[595])/den;
      a13_36 = (rr_f[661])/den;
    }
    double a15_0, a15_6, a15_9, a15_25, a15_44;
    {
      double den = rr_f[316] + rr_f[317] + rr_f[318] + rr_f[319] + rr_f[320] + 
        rr_f[321] + rr_f[322] + rr_f[323] + rr_f[463] + rr_r[271] + rr_r[277] + 
        rr_r[280] + rr_r[283] + rr_r[286] + rr_r[290] + rr_r[293] + rr_r[452] + 
        rr_r[474] + rr_r[490] + rr_r[712]; 
      a15_0 = (rr_f[271] + rr_f[277] + rr_f[280] + rr_f[283] + rr_f[286] + 
        rr_f[290] + rr_f[293] + rr_f[452] + rr_f[490] + rr_r[316] + rr_r[317] + 
        rr_r[318] + rr_r[319] + rr_r[322] + rr_r[323] + rr_r[463])/den; 
      a15_6 = (rr_r[321])/den;
      a15_9 = (rr_r[320])/den;
      a15_25 = (rr_f[474])/den;
      a15_44 = (rr_f[712])/den;
    }
    double a18_0, a18_28, a18_29, a18_31;
    {
      double den = rr_f[369] + rr_f[371] + rr_f[373] + rr_r[538] + rr_r[409] + 
        rr_r[527] + rr_r[592] + rr_r[627] + rr_r[628] + rr_r[631] + rr_r[682] + 
        rr_r[683]; 
      a18_0 = (rr_f[409] + rr_f[544] + rr_f[627] + rr_f[628] + rr_f[631] + 
        rr_f[682] + rr_f[683] + rr_r[369] + rr_r[371] + rr_r[373])/den; 
      a18_28 = (rr_f[527])/den;
      a18_29 = (rr_f[538])/den;
      a18_31 = (rr_f[592])/den;
    }
    double a20_0, a20_3, a20_13;
    {
      double den = rr_f[420] + rr_r[412] + rr_r[413] + rr_r[414] + rr_r[415] + 
        rr_r[416] + rr_r[417] + rr_r[418] + rr_r[419] + rr_r[672] + rr_r[676] + 
        rr_r[790]; 
      a20_0 = (rr_f[412] + rr_f[413] + rr_f[414] + rr_f[415] + rr_f[416] + 
        rr_f[417] + rr_f[419] + rr_f[672] + rr_f[676] + rr_f[790])/den; 
      a20_3 = (rr_f[418])/den;
      a20_13 = (rr_r[420])/den;
    }
    double a21_0, a21_54;
    {
      double den = rr_f[423] + rr_f[424] + rr_f[427] + rr_f[822] + rr_r[428] + 
        rr_r[692] + rr_r[693]; 
      a21_0 = (rr_f[428] + rr_f[692] + rr_f[693] + rr_f[736] + rr_r[423] + 
        rr_r[424] + rr_r[427])/den; 
      a21_54 = (rr_r[822])/den;
    }
    double a22_0, a22_24, a22_46, a22_47, a22_48, a22_49;
    {
      double den = rr_f[421] + rr_f[422] + rr_f[425] + rr_f[426] + rr_f[436] + 
        rr_f[747] + rr_r[429] + rr_r[731] + rr_r[732] + rr_r[763]; 
      a22_0 = (rr_f[429] + rr_f[736] + rr_r[425] + rr_r[426])/den;
      a22_24 = (rr_r[421])/den;
      a22_46 = (rr_f[731])/den;
      a22_47 = (rr_f[732])/den;
      a22_48 = (rr_r[747])/den;
      a22_49 = (rr_f[763])/den;
    }
    double a24_0, a24_22;
    {
      double den = rr_f[440] + rr_r[421] + rr_r[433];
      a24_0 = (rr_f[430] + rr_f[431] + rr_f[432] + rr_f[432] + rr_f[433] + 
        rr_f[434] + rr_f[435] + rr_f[437] + rr_f[438] + rr_f[439] + 
        rr_r[440])/den; 
      a24_22 = (rr_f[421] + rr_f[422] + rr_f[436] + rr_f[436])/den;
    }
    double a25_0, a25_15;
    {
      double den = rr_f[474] + rr_f[475] + rr_f[476] + rr_f[478] + rr_f[479] + 
        rr_f[480] + rr_f[481] + rr_f[482] + rr_f[483] + rr_r[473] + rr_r[495]; 
      a25_0 = (rr_f[439] + rr_f[472] + rr_f[473] + rr_f[495] + rr_r[475] + 
        rr_r[476] + rr_r[478] + rr_r[479] + rr_r[480] + rr_r[481] + rr_r[482] + 
        rr_r[483])/den; 
      a25_15 = (rr_r[474])/den;
    }
    double a27_0, a27_2;
    {
      double den = rr_f[504] + rr_f[505] + rr_f[506] + rr_r[503];
      a27_0 = (rr_f[503] + rr_r[506])/den;
      a27_2 = (rr_r[504] + rr_r[505])/den;
    }
    double a28_0, a28_18;
    {
      double den = rr_f[527] + rr_r[529] + rr_r[543] + rr_r[626];
      a28_0 = (rr_f[529] + rr_f[543] + rr_f[626])/den;
      a28_18 = (rr_r[527])/den;
    }
    double a29_0, a29_3, a29_18;
    {
      double den = rr_f[538] + rr_r[531] + rr_r[532] + rr_r[533] + rr_r[534] + 
        rr_r[535] + rr_r[536] + rr_r[537] + rr_r[530] + rr_r[671] + rr_r[675] + 
        rr_r[787]; 
      a29_0 = (rr_f[531] + rr_f[532] + rr_f[533] + rr_f[534] + rr_f[535] + 
        rr_f[537] + rr_f[530] + rr_f[671] + rr_f[675] + rr_f[787])/den; 
      a29_3 = (rr_f[536])/den;
      a29_18 = (rr_r[538])/den;
    }
    double a30_0, a30_3, a30_31, a30_32, a30_33;
    {
      double den = rr_f[585] + rr_f[586] + rr_f[587] + rr_f[591] + rr_f[600] + 
        rr_f[606] + rr_f[607] + rr_f[609] + rr_r[539] + rr_r[545] + rr_r[549] + 
        rr_r[553] + rr_r[557] + rr_r[561] + rr_r[565] + rr_r[569] + rr_r[573] + 
        rr_r[577] + rr_r[581] + rr_r[634]; 
      a30_0 = (rr_f[539] + rr_f[545] + rr_f[549] + rr_f[553] + rr_f[557] + 
        rr_f[561] + rr_f[565] + rr_f[569] + rr_f[577] + rr_f[581] + rr_f[634] + 
        rr_r[591] + rr_r[600])/den; 
      a30_3 = (rr_f[573])/den;
      a30_31 = (rr_r[585] + rr_r[609])/den;
      a30_32 = (rr_r[586] + rr_r[606])/den;
      a30_33 = (rr_r[587] + rr_r[607])/den;
    }
    double a31_0, a31_3, a31_18, a31_30, a31_32, a31_33;
    {
      double den = rr_f[588] + rr_f[589] + rr_f[592] + rr_f[593] + rr_f[594] + 
        rr_f[601] + rr_f[602] + rr_f[608] + rr_r[540] + rr_r[546] + rr_r[550] + 
        rr_r[554] + rr_r[558] + rr_r[562] + rr_r[566] + rr_r[570] + rr_r[574] + 
        rr_r[578] + rr_r[582] + rr_r[585] + rr_r[609] + rr_r[635]; 
      a31_0 = (rr_f[540] + rr_f[546] + rr_f[550] + rr_f[554] + rr_f[558] + 
        rr_f[562] + rr_f[566] + rr_f[570] + rr_f[578] + rr_f[582] + rr_f[635] + 
        rr_r[593] + rr_r[594] + rr_r[601] + rr_r[602])/den; 
      a31_3 = (rr_f[574])/den;
      a31_18 = (rr_r[592])/den;
      a31_30 = (rr_f[585] + rr_f[609])/den;
      a31_32 = (rr_r[588] + rr_r[608])/den;
      a31_33 = (rr_r[589])/den;
    }
    double a32_0, a32_3, a32_13, a32_30, a32_31, a32_33;
    {
      double den = rr_f[590] + rr_f[595] + rr_f[596] + rr_f[597] + rr_f[603] + 
        rr_f[604] + rr_r[541] + rr_r[547] + rr_r[551] + rr_r[555] + rr_r[559] + 
        rr_r[563] + rr_r[567] + rr_r[571] + rr_r[575] + rr_r[579] + rr_r[583] + 
        rr_r[586] + rr_r[588] + rr_r[606] + rr_r[608] + rr_r[636]; 
      a32_0 = (rr_f[541] + rr_f[547] + rr_f[551] + rr_f[555] + rr_f[559] + 
        rr_f[563] + rr_f[567] + rr_f[571] + rr_f[579] + rr_f[583] + rr_f[636] + 
        rr_r[596] + rr_r[597] + rr_r[603] + rr_r[604])/den; 
      a32_3 = (rr_f[575])/den;
      a32_13 = (rr_r[595])/den;
      a32_30 = (rr_f[586] + rr_f[606])/den;
      a32_31 = (rr_f[588] + rr_f[608])/den;
      a32_33 = (rr_r[590])/den;
    }
    double a33_0, a33_3, a33_30, a33_31, a33_32;
    {
      double den = rr_f[598] + rr_f[599] + rr_f[605] + rr_r[542] + rr_r[548] + 
        rr_r[552] + rr_r[556] + rr_r[560] + rr_r[564] + rr_r[568] + rr_r[572] + 
        rr_r[576] + rr_r[580] + rr_r[584] + rr_r[587] + rr_r[589] + rr_r[590] + 
        rr_r[607] + rr_r[637]; 
      a33_0 = (rr_f[542] + rr_f[548] + rr_f[552] + rr_f[556] + rr_f[560] + 
        rr_f[564] + rr_f[568] + rr_f[572] + rr_f[580] + rr_f[584] + rr_f[637] + 
        rr_r[598] + rr_r[599] + rr_r[605])/den; 
      a33_3 = (rr_f[576])/den;
      a33_30 = (rr_f[587] + rr_f[607])/den;
      a33_31 = (rr_f[589])/den;
      a33_32 = (rr_f[590])/den;
    }
    double a36_0, a36_13;
    {
      double den = rr_f[654] + rr_f[661] + rr_r[645] + rr_r[662];
      a36_0 = (rr_f[645] + rr_f[662] + rr_r[654])/den;
      a36_13 = (rr_r[661])/den;
    }
    double a44_0, a44_15;
    {
      double den = rr_f[712] + rr_r[706];
      a44_0 = (rr_f[706] + rr_f[707] + rr_f[708])/den;
      a44_15 = (rr_r[712])/den;
    }
    double a46_0, a46_22;
    {
      double den = rr_f[731] + rr_r[726] + rr_r[727] + rr_r[728] + rr_r[729] + 
        rr_r[730] + rr_r[765] + rr_r[767] + rr_r[860]; 
      a46_0 = (rr_f[726] + rr_f[727] + rr_f[728] + rr_f[729] + rr_f[730] + 
        rr_f[765] + rr_f[767] + rr_f[860])/den; 
      a46_22 = (rr_r[731])/den;
    }
    double a47_0, a47_22;
    {
      double den = rr_f[732] + rr_r[735] + rr_r[753];
      a47_0 = (rr_f[735] + rr_f[753])/den;
      a47_22 = (rr_r[732])/den;
    }
    double a48_0, a48_3, a48_22, a48_54;
    {
      double den = rr_f[748] + rr_f[749] + rr_f[750] + rr_f[751] + rr_r[733] + 
        rr_r[738] + rr_r[739] + rr_r[740] + rr_r[741] + rr_r[742] + rr_r[743] + 
        rr_r[744] + rr_r[745] + rr_r[746] + rr_r[747] + rr_r[754] + rr_r[755] + 
        rr_r[827]; 
      a48_0 = (rr_f[733] + rr_f[738] + rr_f[739] + rr_f[740] + rr_f[741] + 
        rr_f[742] + rr_f[744] + rr_f[745] + rr_f[746] + rr_f[754] + rr_f[755] + 
        rr_r[748] + rr_r[749] + rr_r[750] + rr_r[751])/den; 
      a48_3 = (rr_f[743])/den;
      a48_22 = (rr_f[747])/den;
      a48_54 = (rr_f[827])/den;
    }
    double a49_0, a49_22;
    {
      double den = rr_f[762] + rr_f[763] + rr_r[835];
      a49_0 = (rr_f[835] + rr_r[762])/den;
      a49_22 = (rr_r[763])/den;
    }
    double a54_0, a54_3, a54_21, a54_48;
    {
      double den = rr_f[825] + rr_f[826] + rr_f[827] + rr_f[828] + rr_r[791] + 
        rr_r[792] + rr_r[793] + rr_r[794] + rr_r[795] + rr_r[796] + rr_r[797] + 
        rr_r[798] + rr_r[799] + rr_r[800] + rr_r[801] + rr_r[802] + rr_r[803] + 
        rr_r[804] + rr_r[805] + rr_r[806] + rr_r[807] + rr_r[808] + rr_r[809] + 
        rr_r[810] + rr_r[811] + rr_r[812] + rr_r[813] + rr_r[814] + rr_r[815] + 
        rr_r[816] + rr_r[817] + rr_r[818] + rr_r[819] + rr_r[820] + rr_r[821] + 
        rr_r[822] + rr_r[823] + rr_r[824] + rr_r[829] + rr_r[830] + rr_r[831] + 
        rr_r[832] + rr_r[833] + rr_r[834]; 
      a54_0 = (rr_f[791] + rr_f[792] + rr_f[793] + rr_f[794] + rr_f[795] + 
        rr_f[796] + rr_f[797] + rr_f[798] + rr_f[799] + rr_f[800] + rr_f[801] + 
        rr_f[802] + rr_f[803] + rr_f[804] + rr_f[805] + rr_f[806] + rr_f[807] + 
        rr_f[808] + rr_f[812] + rr_f[813] + rr_f[814] + rr_f[815] + rr_f[816] + 
        rr_f[817] + rr_f[818] + rr_f[819] + rr_f[820] + rr_f[821] + rr_f[823] + 
        rr_f[824] + rr_f[829] + rr_f[830] + rr_f[831] + rr_f[832] + rr_f[833] + 
        rr_f[834] + rr_r[825] + rr_r[826] + rr_r[828])/den; 
      a54_3 = (rr_f[809] + rr_f[810] + rr_f[811])/den;
      a54_21 = (rr_f[822])/den;
      a54_48 = (rr_r[827])/den;
    }
    double den, xq_1, xq_2, xq_3, xq_6, xq_8, xq_9, xq_10, xq_11, xq_13, xq_15, 
      xq_18, xq_20, xq_21, xq_22, xq_24, xq_25, xq_27, xq_28, xq_29, xq_30, 
      xq_31, xq_32, xq_33, xq_36, xq_44, xq_46, xq_47, xq_48, xq_49, xq_54; 
    a22_0 = a22_0 + a22_49 * a49_0;
    den = 1.0/(1.0 - a49_22*a22_49);
    a22_0 = a22_0*den;
    a22_48 = a22_48*den;
    a22_24 = a22_24*den;
    a22_46 = a22_46*den;
    a22_47 = a22_47*den;
    a22_0 = a22_0 + a22_47 * a47_0;
    den = 1.0/(1.0 - a47_22*a22_47);
    a22_0 = a22_0*den;
    a22_48 = a22_48*den;
    a22_24 = a22_24*den;
    a22_46 = a22_46*den;
    a22_0 = a22_0 + a22_46 * a46_0;
    den = 1.0/(1.0 - a46_22*a22_46);
    a22_0 = a22_0*den;
    a22_48 = a22_48*den;
    a22_24 = a22_24*den;
    a22_0 = a22_0 + a22_24 * a24_0;
    den = 1.0/(1.0 - a24_22*a22_24);
    a22_0 = a22_0*den;
    a22_48 = a22_48*den;
    a54_0 = a54_0 + a54_21 * a21_0;
    den = 1.0/(1.0 - a21_54*a54_21);
    a54_0 = a54_0*den;
    a54_3 = a54_3*den;
    a54_48 = a54_48*den;
    a2_0 = a2_0 + a2_1 * a1_0;
    den = 1.0/(1.0 - a1_2*a2_1);
    a2_0 = a2_0*den;
    a2_3 = a2_3*den;
    a2_27 = a2_27*den;
    a2_0 = a2_0 + a2_27 * a27_0;
    den = 1.0/(1.0 - a27_2*a2_27);
    a2_0 = a2_0*den;
    a2_3 = a2_3*den;
    a6_0 = a6_0 + a6_8 * a8_0;
    den = 1.0/(1.0 - a8_6*a6_8);
    a6_0 = a6_0*den;
    a6_15 = a6_15*den;
    a6_10 = a6_10*den;
    a6_0 = a6_0 + a6_10 * a10_0;
    den = 1.0/(1.0 - a10_6*a6_10);
    a6_0 = a6_0*den;
    a6_15 = a6_15*den;
    a13_0 = a13_0 + a13_36 * a36_0;
    den = 1.0/(1.0 - a36_13*a13_36);
    a13_0 = a13_0*den;
    a13_32 = a13_32*den;
    a13_20 = a13_20*den;
    a18_0 = a18_0 + a18_28 * a28_0;
    den = 1.0/(1.0 - a28_18*a18_28);
    a18_0 = a18_0*den;
    a18_31 = a18_31*den;
    a18_29 = a18_29*den;
    a15_0 = a15_0 + a15_44 * a44_0;
    den = 1.0/(1.0 - a44_15*a15_44);
    a15_0 = a15_0*den;
    a15_6 = a15_6*den;
    a15_9 = a15_9*den;
    a15_25 = a15_25*den;
    a15_0 = a15_0 + a15_25 * a25_0;
    den = 1.0/(1.0 - a25_15*a15_25);
    a15_0 = a15_0*den;
    a15_6 = a15_6*den;
    a15_9 = a15_9*den;
    a3_0 = a3_0 + a3_11 * a11_0;
    den = 1.0/(1.0 - a11_3*a3_11);
    a3_0 = a3_0*den;
    a3_32 = a3_32*den;
    a3_31 = a3_31*den;
    a3_30 = a3_30*den;
    a3_33 = a3_33*den;
    a3_2 = a3_2*den;
    a3_54 = a3_54*den;
    a3_48 = a3_48*den;
    a3_22 = a3_22*den;
    a3_20 = a3_20*den;
    a3_29 = a3_29*den;
    a3_9 = a3_9*den;
    a3_0 = a3_0 + a3_9 * a9_0;
    den = 1.0/(1.0 - a9_3*a3_9);
    double a3_15 = a3_9 * a9_15;
    a3_0 = a3_0*den;
    a3_32 = a3_32*den;
    a3_31 = a3_31*den;
    a3_30 = a3_30*den;
    a3_33 = a3_33*den;
    a3_15 = a3_15*den;
    a3_2 = a3_2*den;
    a3_54 = a3_54*den;
    a3_48 = a3_48*den;
    a3_22 = a3_22*den;
    a3_20 = a3_20*den;
    a3_29 = a3_29*den;
    a15_0 = a15_0 + a15_9 * a9_0;
    den = 1.0/(1.0 - a9_15*a15_9);
    double a15_3 = a15_9 * a9_3;
    a15_0 = a15_0*den;
    a15_3 = a15_3*den;
    a15_6 = a15_6*den;
    a3_0 = a3_0 + a3_29 * a29_0;
    den = 1.0/(1.0 - a29_3*a3_29);
    double a3_18 = a3_29 * a29_18;
    a3_0 = a3_0*den;
    a3_32 = a3_32*den;
    a3_31 = a3_31*den;
    a3_30 = a3_30*den;
    a3_33 = a3_33*den;
    a3_15 = a3_15*den;
    a3_18 = a3_18*den;
    a3_2 = a3_2*den;
    a3_54 = a3_54*den;
    a3_48 = a3_48*den;
    a3_22 = a3_22*den;
    a3_20 = a3_20*den;
    a18_0 = a18_0 + a18_29 * a29_0;
    den = 1.0/(1.0 - a29_18*a18_29);
    double a18_3 = a18_29 * a29_3;
    a18_0 = a18_0*den;
    a18_3 = a18_3*den;
    a18_31 = a18_31*den;
    a3_0 = a3_0 + a3_20 * a20_0;
    den = 1.0/(1.0 - a20_3*a3_20);
    double a3_13 = a3_20 * a20_13;
    a3_0 = a3_0*den;
    a3_32 = a3_32*den;
    a3_31 = a3_31*den;
    a3_30 = a3_30*den;
    a3_33 = a3_33*den;
    a3_15 = a3_15*den;
    a3_18 = a3_18*den;
    a3_13 = a3_13*den;
    a3_2 = a3_2*den;
    a3_54 = a3_54*den;
    a3_48 = a3_48*den;
    a3_22 = a3_22*den;
    a13_0 = a13_0 + a13_20 * a20_0;
    den = 1.0/(1.0 - a20_13*a13_20);
    double a13_3 = a13_20 * a20_3;
    a13_0 = a13_0*den;
    a13_3 = a13_3*den;
    a13_32 = a13_32*den;
    a3_0 = a3_0 + a3_22 * a22_0;
    den = 1.0/(1.0);
    a3_48 = a3_48 + a3_22 * a22_48;
    a48_0 = a48_0 + a48_22 * a22_0;
    den = 1.0/(1.0 - a22_48*a48_22);
    a48_0 = a48_0*den;
    a48_3 = a48_3*den;
    a48_54 = a48_54*den;
    a3_0 = a3_0 + a3_48 * a48_0;
    den = 1.0/(1.0 - a48_3*a3_48);
    a3_54 = a3_54 + a3_48 * a48_54;
    a3_0 = a3_0*den;
    a3_32 = a3_32*den;
    a3_31 = a3_31*den;
    a3_30 = a3_30*den;
    a3_33 = a3_33*den;
    a3_15 = a3_15*den;
    a3_18 = a3_18*den;
    a3_13 = a3_13*den;
    a3_2 = a3_2*den;
    a3_54 = a3_54*den;
    a54_0 = a54_0 + a54_48 * a48_0;
    den = 1.0/(1.0 - a48_54*a54_48);
    a54_3 = a54_3 + a54_48 * a48_3;
    a54_0 = a54_0*den;
    a54_3 = a54_3*den;
    a3_0 = a3_0 + a3_54 * a54_0;
    den = 1.0/(1.0 - a54_3*a3_54);
    a3_0 = a3_0*den;
    a3_32 = a3_32*den;
    a3_31 = a3_31*den;
    a3_30 = a3_30*den;
    a3_33 = a3_33*den;
    a3_15 = a3_15*den;
    a3_18 = a3_18*den;
    a3_13 = a3_13*den;
    a3_2 = a3_2*den;
    a3_0 = a3_0 + a3_2 * a2_0;
    den = 1.0/(1.0 - a2_3*a3_2);
    a3_0 = a3_0*den;
    a3_32 = a3_32*den;
    a3_31 = a3_31*den;
    a3_30 = a3_30*den;
    a3_33 = a3_33*den;
    a3_15 = a3_15*den;
    a3_18 = a3_18*den;
    a3_13 = a3_13*den;
    a15_0 = a15_0 + a15_6 * a6_0;
    den = 1.0/(1.0 - a6_15*a15_6);
    a15_0 = a15_0*den;
    a15_3 = a15_3*den;
    a3_0 = a3_0 + a3_13 * a13_0;
    den = 1.0/(1.0 - a13_3*a3_13);
    a3_32 = a3_32 + a3_13 * a13_32;
    a3_0 = a3_0*den;
    a3_32 = a3_32*den;
    a3_31 = a3_31*den;
    a3_30 = a3_30*den;
    a3_33 = a3_33*den;
    a3_15 = a3_15*den;
    a3_18 = a3_18*den;
    a32_0 = a32_0 + a32_13 * a13_0;
    den = 1.0/(1.0 - a13_32*a32_13);
    a32_3 = a32_3 + a32_13 * a13_3;
    a32_0 = a32_0*den;
    a32_3 = a32_3*den;
    a32_31 = a32_31*den;
    a32_30 = a32_30*den;
    a32_33 = a32_33*den;
    a3_0 = a3_0 + a3_18 * a18_0;
    den = 1.0/(1.0 - a18_3*a3_18);
    a3_31 = a3_31 + a3_18 * a18_31;
    a3_0 = a3_0*den;
    a3_32 = a3_32*den;
    a3_31 = a3_31*den;
    a3_30 = a3_30*den;
    a3_33 = a3_33*den;
    a3_15 = a3_15*den;
    a31_0 = a31_0 + a31_18 * a18_0;
    den = 1.0/(1.0 - a18_31*a31_18);
    a31_3 = a31_3 + a31_18 * a18_3;
    a31_0 = a31_0*den;
    a31_3 = a31_3*den;
    a31_32 = a31_32*den;
    a31_30 = a31_30*den;
    a31_33 = a31_33*den;
    a3_0 = a3_0 + a3_15 * a15_0;
    den = 1.0/(1.0 - a15_3*a3_15);
    a3_0 = a3_0*den;
    a3_32 = a3_32*den;
    a3_31 = a3_31*den;
    a3_30 = a3_30*den;
    a3_33 = a3_33*den;
    a3_0 = a3_0 + a3_33 * a33_0;
    den = 1.0/(1.0 - a33_3*a3_33);
    a3_32 = a3_32 + a3_33 * a33_32;
    a3_31 = a3_31 + a3_33 * a33_31;
    a3_30 = a3_30 + a3_33 * a33_30;
    a3_0 = a3_0*den;
    a3_32 = a3_32*den;
    a3_31 = a3_31*den;
    a3_30 = a3_30*den;
    a32_0 = a32_0 + a32_33 * a33_0;
    den = 1.0/(1.0 - a33_32*a32_33);
    a32_3 = a32_3 + a32_33 * a33_3;
    a32_31 = a32_31 + a32_33 * a33_31;
    a32_30 = a32_30 + a32_33 * a33_30;
    a32_0 = a32_0*den;
    a32_3 = a32_3*den;
    a32_31 = a32_31*den;
    a32_30 = a32_30*den;
    a31_0 = a31_0 + a31_33 * a33_0;
    den = 1.0/(1.0 - a33_31*a31_33);
    a31_3 = a31_3 + a31_33 * a33_3;
    a31_32 = a31_32 + a31_33 * a33_32;
    a31_30 = a31_30 + a31_33 * a33_30;
    a31_0 = a31_0*den;
    a31_3 = a31_3*den;
    a31_32 = a31_32*den;
    a31_30 = a31_30*den;
    a30_0 = a30_0 + a30_33 * a33_0;
    den = 1.0/(1.0 - a33_30*a30_33);
    a30_3 = a30_3 + a30_33 * a33_3;
    a30_32 = a30_32 + a30_33 * a33_32;
    a30_31 = a30_31 + a30_33 * a33_31;
    a30_0 = a30_0*den;
    a30_3 = a30_3*den;
    a30_32 = a30_32*den;
    a30_31 = a30_31*den;
    a3_0 = a3_0 + a3_30 * a30_0;
    den = 1.0/(1.0 - a30_3*a3_30);
    a3_32 = a3_32 + a3_30 * a30_32;
    a3_31 = a3_31 + a3_30 * a30_31;
    a3_0 = a3_0*den;
    a3_32 = a3_32*den;
    a3_31 = a3_31*den;
    a32_0 = a32_0 + a32_30 * a30_0;
    den = 1.0/(1.0 - a30_32*a32_30);
    a32_3 = a32_3 + a32_30 * a30_3;
    a32_31 = a32_31 + a32_30 * a30_31;
    a32_0 = a32_0*den;
    a32_3 = a32_3*den;
    a32_31 = a32_31*den;
    a31_0 = a31_0 + a31_30 * a30_0;
    den = 1.0/(1.0 - a30_31*a31_30);
    a31_3 = a31_3 + a31_30 * a30_3;
    a31_32 = a31_32 + a31_30 * a30_32;
    a31_0 = a31_0*den;
    a31_3 = a31_3*den;
    a31_32 = a31_32*den;
    a3_0 = a3_0 + a3_31 * a31_0;
    den = 1.0/(1.0 - a31_3*a3_31);
    a3_32 = a3_32 + a3_31 * a31_32;
    a3_0 = a3_0*den;
    a3_32 = a3_32*den;
    a32_0 = a32_0 + a32_31 * a31_0;
    den = 1.0/(1.0 - a31_32*a32_31);
    a32_3 = a32_3 + a32_31 * a31_3;
    a32_0 = a32_0*den;
    a32_3 = a32_3*den;
    a3_0 = a3_0 + a3_32 * a32_0;
    den = 1.0/(1.0 - a32_3*a3_32);
    a3_0 = a3_0*den;
    xq_3 = a3_0;
    xq_32 = a32_0 + a32_3*xq_3;
    xq_31 = a31_0 + a31_3*xq_3 + a31_32*xq_32;
    xq_30 = a30_0 + a30_3*xq_3 + a30_31*xq_31 + a30_32*xq_32;
    xq_33 = a33_0 + a33_3*xq_3 + a33_30*xq_30 + a33_31*xq_31 + a33_32*xq_32;
    xq_15 = a15_0 + a15_3*xq_3;
    xq_18 = a18_0 + a18_3*xq_3 + a18_31*xq_31;
    xq_13 = a13_0 + a13_3*xq_3 + a13_32*xq_32;
    xq_6 = a6_0 + a6_15*xq_15;
    xq_2 = a2_0 + a2_3*xq_3;
    xq_54 = a54_0 + a54_3*xq_3;
    xq_48 = a48_0 + a48_3*xq_3 + a48_54*xq_54;
    xq_22 = a22_0 + a22_48*xq_48;
    xq_20 = a20_0 + a20_3*xq_3 + a20_13*xq_13;
    xq_29 = a29_0 + a29_3*xq_3 + a29_18*xq_18;
    xq_9 = a9_0 + a9_3*xq_3 + a9_15*xq_15;
    xq_11 = a11_0 + a11_3*xq_3;
    xq_25 = a25_0 + a25_15*xq_15;
    xq_44 = a44_0 + a44_15*xq_15;
    xq_28 = a28_0 + a28_18*xq_18;
    xq_36 = a36_0 + a36_13*xq_13;
    xq_10 = a10_0 + a10_6*xq_6;
    xq_8 = a8_0 + a8_6*xq_6;
    xq_27 = a27_0 + a27_2*xq_2;
    xq_1 = a1_0 + a1_2*xq_2;
    xq_21 = a21_0 + a21_54*xq_54;
    xq_24 = a24_0 + a24_22*xq_22;
    xq_46 = a46_0 + a46_22*xq_22;
    xq_47 = a47_0 + a47_22*xq_22;
    xq_49 = a49_0 + a49_22*xq_22;
    rr_f[43] *= xq_1;
    rr_f[44] *= xq_1;
    rr_r[73] *= xq_1;
    rr_f[63] *= xq_2;
    rr_f[64] *= xq_2;
    rr_f[65] *= xq_2;
    rr_f[66] *= xq_2;
    rr_f[67] *= xq_2;
    rr_f[69] *= xq_2;
    rr_f[70] *= xq_2;
    rr_f[71] *= xq_2;
    rr_f[73] *= xq_2;
    rr_r[62] *= xq_2;
    rr_r[75] *= xq_2;
    rr_r[77] *= xq_2;
    rr_r[78] *= xq_2;
    rr_r[80] *= xq_2;
    rr_r[81] *= xq_2;
    rr_r[82] *= xq_2;
    rr_r[83] *= xq_2;
    rr_r[84] *= xq_2;
    rr_r[95] *= xq_2;
    rr_r[105] *= xq_2;
    rr_r[154] *= xq_2;
    rr_r[188] *= xq_2;
    rr_r[477] *= xq_2;
    rr_r[504] *= xq_2;
    rr_r[505] *= xq_2;
    rr_f[55] *= xq_3;
    rr_f[56] *= xq_3;
    rr_f[57] *= xq_3;
    rr_f[58] *= xq_3;
    rr_f[59] *= xq_3;
    rr_f[60] *= xq_3;
    rr_f[61] *= xq_3;
    rr_f[84] *= xq_3;
    rr_f[136] *= xq_3;
    rr_f[202] *= xq_3;
    rr_f[223] *= xq_3;
    rr_f[242] *= xq_3;
    rr_f[251] *= xq_3;
    rr_f[536] *= xq_3;
    rr_f[384] *= xq_3;
    rr_f[390] *= xq_3;
    rr_f[418] *= xq_3;
    rr_f[523] *= xq_3;
    rr_f[524] *= xq_3;
    rr_f[573] *= xq_3;
    rr_f[574] *= xq_3;
    rr_f[575] *= xq_3;
    rr_f[576] *= xq_3;
    rr_f[615] *= xq_3;
    rr_f[621] *= xq_3;
    rr_f[700] *= xq_3;
    rr_f[743] *= xq_3;
    rr_f[809] *= xq_3;
    rr_f[810] *= xq_3;
    rr_f[811] *= xq_3;
    rr_r[76] *= xq_3;
    rr_r[79] *= xq_3;
    rr_r[94] *= xq_3;
    rr_r[97] *= xq_3;
    rr_r[100] *= xq_3;
    rr_r[110] *= xq_3;
    rr_r[111] *= xq_3;
    rr_r[113] *= xq_3;
    rr_f[175] *= xq_6;
    rr_f[176] *= xq_6;
    rr_f[177] *= xq_6;
    rr_f[178] *= xq_6;
    rr_r[163] *= xq_6;
    rr_r[166] *= xq_6;
    rr_r[167] *= xq_6;
    rr_r[168] *= xq_6;
    rr_r[169] *= xq_6;
    rr_r[170] *= xq_6;
    rr_r[171] *= xq_6;
    rr_r[172] *= xq_6;
    rr_r[179] *= xq_6;
    rr_r[218] *= xq_6;
    rr_r[233] *= xq_6;
    rr_r[321] *= xq_6;
    rr_r[669] *= xq_6;
    rr_r[673] *= xq_6;
    rr_r[785] *= xq_6;
    rr_f[163] *= xq_8;
    rr_f[164] *= xq_8;
    rr_f[227] *= xq_9;
    rr_r[219] *= xq_9;
    rr_r[220] *= xq_9;
    rr_r[221] *= xq_9;
    rr_r[222] *= xq_9;
    rr_r[223] *= xq_9;
    rr_r[224] *= xq_9;
    rr_r[225] *= xq_9;
    rr_r[226] *= xq_9;
    rr_r[228] *= xq_9;
    rr_r[229] *= xq_9;
    rr_r[320] *= xq_9;
    rr_r[468] *= xq_9;
    rr_r[786] *= xq_9;
    rr_f[233] *= xq_10;
    rr_r[232] *= xq_10;
    rr_f[257] *= xq_11;
    rr_r[246] *= xq_11;
    rr_r[247] *= xq_11;
    rr_r[248] *= xq_11;
    rr_r[249] *= xq_11;
    rr_r[250] *= xq_11;
    rr_r[251] *= xq_11;
    rr_r[252] *= xq_11;
    rr_r[253] *= xq_11;
    rr_r[254] *= xq_11;
    rr_r[255] *= xq_11;
    rr_r[256] *= xq_11;
    rr_r[267] *= xq_11;
    rr_r[450] *= xq_11;
    rr_r[670] *= xq_11;
    rr_r[674] *= xq_11;
    rr_r[788] *= xq_11;
    rr_f[264] *= xq_13;
    rr_f[265] *= xq_13;
    rr_f[266] *= xq_13;
    rr_r[362] *= xq_13;
    rr_r[420] *= xq_13;
    rr_r[528] *= xq_13;
    rr_r[595] *= xq_13;
    rr_r[632] *= xq_13;
    rr_r[661] *= xq_13;
    rr_f[316] *= xq_15;
    rr_f[317] *= xq_15;
    rr_f[318] *= xq_15;
    rr_f[319] *= xq_15;
    rr_f[320] *= xq_15;
    rr_f[321] *= xq_15;
    rr_f[322] *= xq_15;
    rr_f[323] *= xq_15;
    rr_f[463] *= xq_15;
    rr_r[271] *= xq_15;
    rr_r[277] *= xq_15;
    rr_r[280] *= xq_15;
    rr_r[283] *= xq_15;
    rr_r[286] *= xq_15;
    rr_r[290] *= xq_15;
    rr_r[293] *= xq_15;
    rr_r[452] *= xq_15;
    rr_r[474] *= xq_15;
    rr_r[490] *= xq_15;
    rr_r[712] *= xq_15;
    rr_f[369] *= xq_18;
    rr_f[371] *= xq_18;
    rr_f[373] *= xq_18;
    rr_r[538] *= xq_18;
    rr_r[409] *= xq_18;
    rr_r[527] *= xq_18;
    rr_r[592] *= xq_18;
    rr_r[627] *= xq_18;
    rr_r[628] *= xq_18;
    rr_r[631] *= xq_18;
    rr_r[682] *= xq_18;
    rr_r[683] *= xq_18;
    rr_f[420] *= xq_20;
    rr_r[412] *= xq_20;
    rr_r[413] *= xq_20;
    rr_r[414] *= xq_20;
    rr_r[415] *= xq_20;
    rr_r[416] *= xq_20;
    rr_r[417] *= xq_20;
    rr_r[418] *= xq_20;
    rr_r[419] *= xq_20;
    rr_r[672] *= xq_20;
    rr_r[676] *= xq_20;
    rr_r[790] *= xq_20;
    rr_f[423] *= xq_21;
    rr_f[424] *= xq_21;
    rr_f[427] *= xq_21;
    rr_f[822] *= xq_21;
    rr_r[428] *= xq_21;
    rr_r[692] *= xq_21;
    rr_r[693] *= xq_21;
    rr_f[421] *= xq_22;
    rr_f[422] *= xq_22;
    rr_f[425] *= xq_22;
    rr_f[426] *= xq_22;
    rr_f[436] *= xq_22;
    rr_f[747] *= xq_22;
    rr_r[429] *= xq_22;
    rr_r[731] *= xq_22;
    rr_r[732] *= xq_22;
    rr_r[763] *= xq_22;
    rr_f[440] *= xq_24;
    rr_r[421] *= xq_24;
    rr_r[433] *= xq_24;
    rr_f[474] *= xq_25;
    rr_f[475] *= xq_25;
    rr_f[476] *= xq_25;
    rr_f[478] *= xq_25;
    rr_f[479] *= xq_25;
    rr_f[480] *= xq_25;
    rr_f[481] *= xq_25;
    rr_f[482] *= xq_25;
    rr_f[483] *= xq_25;
    rr_r[473] *= xq_25;
    rr_r[495] *= xq_25;
    rr_f[504] *= xq_27;
    rr_f[505] *= xq_27;
    rr_f[506] *= xq_27;
    rr_r[503] *= xq_27;
    rr_f[527] *= xq_28;
    rr_r[529] *= xq_28;
    rr_r[543] *= xq_28;
    rr_r[626] *= xq_28;
    rr_f[538] *= xq_29;
    rr_r[531] *= xq_29;
    rr_r[532] *= xq_29;
    rr_r[533] *= xq_29;
    rr_r[534] *= xq_29;
    rr_r[535] *= xq_29;
    rr_r[536] *= xq_29;
    rr_r[537] *= xq_29;
    rr_r[530] *= xq_29;
    rr_r[671] *= xq_29;
    rr_r[675] *= xq_29;
    rr_r[787] *= xq_29;
    rr_f[585] *= xq_30;
    rr_f[586] *= xq_30;
    rr_f[587] *= xq_30;
    rr_f[591] *= xq_30;
    rr_f[600] *= xq_30;
    rr_f[606] *= xq_30;
    rr_f[607] *= xq_30;
    rr_f[609] *= xq_30;
    rr_r[539] *= xq_30;
    rr_r[545] *= xq_30;
    rr_r[549] *= xq_30;
    rr_r[553] *= xq_30;
    rr_r[557] *= xq_30;
    rr_r[561] *= xq_30;
    rr_r[565] *= xq_30;
    rr_r[569] *= xq_30;
    rr_r[573] *= xq_30;
    rr_r[577] *= xq_30;
    rr_r[581] *= xq_30;
    rr_r[634] *= xq_30;
    rr_f[588] *= xq_31;
    rr_f[589] *= xq_31;
    rr_f[592] *= xq_31;
    rr_f[593] *= xq_31;
    rr_f[594] *= xq_31;
    rr_f[601] *= xq_31;
    rr_f[602] *= xq_31;
    rr_f[608] *= xq_31;
    rr_r[540] *= xq_31;
    rr_r[546] *= xq_31;
    rr_r[550] *= xq_31;
    rr_r[554] *= xq_31;
    rr_r[558] *= xq_31;
    rr_r[562] *= xq_31;
    rr_r[566] *= xq_31;
    rr_r[570] *= xq_31;
    rr_r[574] *= xq_31;
    rr_r[578] *= xq_31;
    rr_r[582] *= xq_31;
    rr_r[585] *= xq_31;
    rr_r[609] *= xq_31;
    rr_r[635] *= xq_31;
    rr_f[590] *= xq_32;
    rr_f[595] *= xq_32;
    rr_f[596] *= xq_32;
    rr_f[597] *= xq_32;
    rr_f[603] *= xq_32;
    rr_f[604] *= xq_32;
    rr_r[541] *= xq_32;
    rr_r[547] *= xq_32;
    rr_r[551] *= xq_32;
    rr_r[555] *= xq_32;
    rr_r[559] *= xq_32;
    rr_r[563] *= xq_32;
    rr_r[567] *= xq_32;
    rr_r[571] *= xq_32;
    rr_r[575] *= xq_32;
    rr_r[579] *= xq_32;
    rr_r[583] *= xq_32;
    rr_r[586] *= xq_32;
    rr_r[588] *= xq_32;
    rr_r[606] *= xq_32;
    rr_r[608] *= xq_32;
    rr_r[636] *= xq_32;
    rr_f[598] *= xq_33;
    rr_f[599] *= xq_33;
    rr_f[605] *= xq_33;
    rr_r[542] *= xq_33;
    rr_r[548] *= xq_33;
    rr_r[552] *= xq_33;
    rr_r[556] *= xq_33;
    rr_r[560] *= xq_33;
    rr_r[564] *= xq_33;
    rr_r[568] *= xq_33;
    rr_r[572] *= xq_33;
    rr_r[576] *= xq_33;
    rr_r[580] *= xq_33;
    rr_r[584] *= xq_33;
    rr_r[587] *= xq_33;
    rr_r[589] *= xq_33;
    rr_r[590] *= xq_33;
    rr_r[607] *= xq_33;
    rr_r[637] *= xq_33;
    rr_f[654] *= xq_36;
    rr_f[661] *= xq_36;
    rr_r[645] *= xq_36;
    rr_r[662] *= xq_36;
    rr_f[712] *= xq_44;
    rr_r[706] *= xq_44;
    rr_f[731] *= xq_46;
    rr_r[726] *= xq_46;
    rr_r[727] *= xq_46;
    rr_r[728] *= xq_46;
    rr_r[729] *= xq_46;
    rr_r[730] *= xq_46;
    rr_r[765] *= xq_46;
    rr_r[767] *= xq_46;
    rr_r[860] *= xq_46;
    rr_f[732] *= xq_47;
    rr_r[735] *= xq_47;
    rr_r[753] *= xq_47;
    rr_f[748] *= xq_48;
    rr_f[749] *= xq_48;
    rr_f[750] *= xq_48;
    rr_f[751] *= xq_48;
    rr_r[733] *= xq_48;
    rr_r[738] *= xq_48;
    rr_r[739] *= xq_48;
    rr_r[740] *= xq_48;
    rr_r[741] *= xq_48;
    rr_r[742] *= xq_48;
    rr_r[743] *= xq_48;
    rr_r[744] *= xq_48;
    rr_r[745] *= xq_48;
    rr_r[746] *= xq_48;
    rr_r[747] *= xq_48;
    rr_r[754] *= xq_48;
    rr_r[755] *= xq_48;
    rr_r[827] *= xq_48;
    rr_f[762] *= xq_49;
    rr_f[763] *= xq_49;
    rr_r[835] *= xq_49;
    rr_f[825] *= xq_54;
    rr_f[826] *= xq_54;
    rr_f[827] *= xq_54;
    rr_f[828] *= xq_54;
    rr_r[791] *= xq_54;
    rr_r[792] *= xq_54;
    rr_r[793] *= xq_54;
    rr_r[794] *= xq_54;
    rr_r[795] *= xq_54;
    rr_r[796] *= xq_54;
    rr_r[797] *= xq_54;
    rr_r[798] *= xq_54;
    rr_r[799] *= xq_54;
    rr_r[800] *= xq_54;
    rr_r[801] *= xq_54;
    rr_r[802] *= xq_54;
    rr_r[803] *= xq_54;
    rr_r[804] *= xq_54;
    rr_r[805] *= xq_54;
    rr_r[806] *= xq_54;
    rr_r[807] *= xq_54;
    rr_r[808] *= xq_54;
    rr_r[809] *= xq_54;
    rr_r[810] *= xq_54;
    rr_r[811] *= xq_54;
    rr_r[812] *= xq_54;
    rr_r[813] *= xq_54;
    rr_r[814] *= xq_54;
    rr_r[815] *= xq_54;
    rr_r[816] *= xq_54;
    rr_r[817] *= xq_54;
    rr_r[818] *= xq_54;
    rr_r[819] *= xq_54;
    rr_r[820] *= xq_54;
    rr_r[821] *= xq_54;
    rr_r[822] *= xq_54;
    rr_r[823] *= xq_54;
    rr_r[824] *= xq_54;
    rr_r[829] *= xq_54;
    rr_r[830] *= xq_54;
    rr_r[831] *= xq_54;
    rr_r[832] *= xq_54;
    rr_r[833] *= xq_54;
    rr_r[834] *= xq_54;
  }
  // QSSA connected component
  {
    double a4_0, a4_5;
    {
      double den = rr_f[91] + rr_f[122] + rr_f[123] + rr_f[124] + rr_f[125] + 
        rr_f[126] + rr_f[182] + rr_r[96] + rr_r[114] + rr_r[118] + rr_r[185] + 
        rr_r[214] + rr_r[327] + rr_r[336] + rr_r[343]; 
      a4_0 = (rr_f[96] + rr_f[185] + rr_f[214] + rr_f[327] + rr_f[336] + 
        rr_f[343] + rr_r[91] + rr_r[122] + rr_r[123] + rr_r[124] + rr_r[125] + 
        rr_r[126] + rr_r[182])/den; 
      a4_5 = (rr_f[114] + rr_f[118])/den;
    }
    double a5_0, a5_4;
    {
      double den = rr_f[114] + rr_f[115] + rr_f[116] + rr_f[117] + rr_f[118] + 
        rr_f[119] + rr_f[120] + rr_f[121] + rr_f[137] + rr_f[189] + rr_f[205] + 
        rr_r[93] + rr_r[191]; 
      a5_0 = (rr_f[93] + rr_f[191] + rr_r[115] + rr_r[116] + rr_r[117] + 
        rr_r[119] + rr_r[120] + rr_r[121] + rr_r[137] + rr_r[189] + 
        rr_r[205])/den; 
      a5_4 = (rr_r[114] + rr_r[118])/den;
    }
    double den, xq_4, xq_5;
    a4_0 = a4_0 + a4_5 * a5_0;
    den = 1.0/(1.0 - a5_4*a4_5);
    a4_0 = a4_0*den;
    xq_4 = a4_0;
    xq_5 = a5_0 + a5_4*xq_4;
    rr_f[91] *= xq_4;
    rr_f[122] *= xq_4;
    rr_f[123] *= xq_4;
    rr_f[124] *= xq_4;
    rr_f[125] *= xq_4;
    rr_f[126] *= xq_4;
    rr_f[182] *= xq_4;
    rr_r[96] *= xq_4;
    rr_r[114] *= xq_4;
    rr_r[118] *= xq_4;
    rr_r[185] *= xq_4;
    rr_r[214] *= xq_4;
    rr_r[327] *= xq_4;
    rr_r[336] *= xq_4;
    rr_r[343] *= xq_4;
    rr_f[114] *= xq_5;
    rr_f[115] *= xq_5;
    rr_f[116] *= xq_5;
    rr_f[117] *= xq_5;
    rr_f[118] *= xq_5;
    rr_f[119] *= xq_5;
    rr_f[120] *= xq_5;
    rr_f[121] *= xq_5;
    rr_f[137] *= xq_5;
    rr_f[189] *= xq_5;
    rr_f[205] *= xq_5;
    rr_r[93] *= xq_5;
    rr_r[191] *= xq_5;
  }
  // QSSA connected component
  {
    double a7_0;
    {
      double den = rr_f[148] + rr_f[149] + rr_f[150] + rr_r[146] + rr_r[157];
      a7_0 = (rr_f[146] + rr_f[147] + rr_f[157] + rr_f[399] + rr_f[431] + 
        rr_f[435] + rr_f[624] + rr_f[708] + rr_f[711] + rr_r[148] + rr_r[149] + 
        rr_r[150])/den; 
    }
    double den, xq_7;
    xq_7 = a7_0;
    rr_f[148] *= xq_7;
    rr_f[149] *= xq_7;
    rr_f[150] *= xq_7;
    rr_r[146] *= xq_7;
    rr_r[157] *= xq_7;
  }
  // QSSA connected component
  {
    double a12_0, a12_43;
    {
      double den = rr_f[262] + rr_f[263] + rr_f[258] + rr_f[259] + rr_f[260] + 
        rr_f[261] + rr_r[363] + rr_r[442] + rr_r[465] + rr_r[686] + rr_r[691] + 
        rr_r[737]; 
      a12_0 = (rr_f[363] + rr_f[442] + rr_f[465] + rr_f[691] + rr_f[737] + 
        rr_r[262] + rr_r[263] + rr_r[258] + rr_r[259] + rr_r[260] + 
        rr_r[261])/den; 
      a12_43 = (rr_f[686])/den;
    }
    double a43_0, a43_12;
    {
      double den = rr_f[686] + rr_f[687] + rr_f[688] + rr_f[689] + rr_f[690] + 
        rr_r[714] + rr_r[734]; 
      a43_0 = (rr_f[714] + rr_f[734] + rr_r[687] + rr_r[688] + rr_r[689] + 
        rr_r[690])/den; 
      a43_12 = (rr_r[686])/den;
    }
    double den, xq_12, xq_43;
    a12_0 = a12_0 + a12_43 * a43_0;
    den = 1.0/(1.0 - a43_12*a12_43);
    a12_0 = a12_0*den;
    xq_12 = a12_0;
    xq_43 = a43_0 + a43_12*xq_12;
    rr_f[262] *= xq_12;
    rr_f[263] *= xq_12;
    rr_f[258] *= xq_12;
    rr_f[259] *= xq_12;
    rr_f[260] *= xq_12;
    rr_f[261] *= xq_12;
    rr_r[363] *= xq_12;
    rr_r[442] *= xq_12;
    rr_r[465] *= xq_12;
    rr_r[686] *= xq_12;
    rr_r[691] *= xq_12;
    rr_r[737] *= xq_12;
    rr_f[686] *= xq_43;
    rr_f[687] *= xq_43;
    rr_f[688] *= xq_43;
    rr_f[689] *= xq_43;
    rr_f[690] *= xq_43;
    rr_r[714] *= xq_43;
    rr_r[734] *= xq_43;
  }
  // QSSA connected component
  {
    double a14_0, a14_34;
    {
      double den = rr_f[311] + rr_f[312] + rr_f[313] + rr_f[314] + rr_f[315] + 
        rr_f[462] + rr_r[270] + rr_r[276] + rr_r[279] + rr_r[282] + rr_r[285] + 
        rr_r[289] + rr_r[292] + rr_r[625] + rr_r[723]; 
      a14_0 = (rr_f[270] + rr_f[276] + rr_f[279] + rr_f[282] + rr_f[285] + 
        rr_f[289] + rr_f[292] + rr_f[723] + rr_r[311] + rr_r[312] + rr_r[313] + 
        rr_r[314] + rr_r[315] + rr_r[462])/den; 
      a14_34 = (rr_f[625])/den;
    }
    double a34_0, a34_14;
    {
      double den = rr_f[625] + rr_r[622];
      a34_0 = (rr_f[622] + rr_f[623] + rr_f[624])/den;
      a34_14 = (rr_r[625])/den;
    }
    double den, xq_14, xq_34;
    a14_0 = a14_0 + a14_34 * a34_0;
    den = 1.0/(1.0 - a34_14*a14_34);
    a14_0 = a14_0*den;
    xq_14 = a14_0;
    xq_34 = a34_0 + a34_14*xq_14;
    rr_f[311] *= xq_14;
    rr_f[312] *= xq_14;
    rr_f[313] *= xq_14;
    rr_f[314] *= xq_14;
    rr_f[315] *= xq_14;
    rr_f[462] *= xq_14;
    rr_r[270] *= xq_14;
    rr_r[276] *= xq_14;
    rr_r[279] *= xq_14;
    rr_r[282] *= xq_14;
    rr_r[285] *= xq_14;
    rr_r[289] *= xq_14;
    rr_r[292] *= xq_14;
    rr_r[625] *= xq_14;
    rr_r[723] *= xq_14;
    rr_f[625] *= xq_34;
    rr_r[622] *= xq_34;
  }
  // QSSA connected component
  {
    double a16_0;
    {
      double den = rr_f[351] + rr_f[357] + rr_f[358] + rr_r[349] + rr_r[356];
      a16_0 = (rr_f[349] + rr_f[356] + rr_r[351] + rr_r[357] + rr_r[358])/den;
    }
    double den, xq_16;
    xq_16 = a16_0;
    rr_f[351] *= xq_16;
    rr_f[357] *= xq_16;
    rr_f[358] *= xq_16;
    rr_r[349] *= xq_16;
    rr_r[356] *= xq_16;
  }
  // QSSA connected component
  {
    double a17_0;
    {
      double den = rr_f[364] + rr_f[365] + rr_f[366] + rr_r[299];
      a17_0 = (rr_f[299] + rr_f[300] + rr_f[437] + rr_r[364] + rr_r[365] + 
        rr_r[366])/den; 
    }
    double den, xq_17;
    xq_17 = a17_0;
    rr_f[364] *= xq_17;
    rr_f[365] *= xq_17;
    rr_f[366] *= xq_17;
    rr_r[299] *= xq_17;
  }
  // QSSA connected component
  {
    double a19_0;
    {
      double den = rr_f[400] + rr_f[401] + rr_r[392];
      a19_0 = (rr_f[392] + rr_f[393] + rr_f[399] + rr_f[438] + rr_r[400] + 
        rr_r[401])/den; 
    }
    double den, xq_19;
    xq_19 = a19_0;
    rr_f[400] *= xq_19;
    rr_f[401] *= xq_19;
    rr_r[392] *= xq_19;
  }
  // QSSA connected component
  {
    double a23_0;
    {
      double den = rr_f[451] + rr_r[445] + rr_r[448];
      a23_0 = (rr_f[445] + rr_f[448] + rr_r[451])/den;
    }
    double den, xq_23;
    xq_23 = a23_0;
    rr_f[451] *= xq_23;
    rr_r[445] *= xq_23;
    rr_r[448] *= xq_23;
  }
  // QSSA connected component
  {
    double a26_0;
    {
      double den = rr_f[492] + rr_r[491];
      a26_0 = (rr_f[491] + rr_r[492])/den;
    }
    double den, xq_26;
    xq_26 = a26_0;
    rr_f[492] *= xq_26;
    rr_r[491] *= xq_26;
  }
  // QSSA connected component
  {
    double a35_0;
    {
      double den = rr_r[644] + rr_r[651];
      a35_0 = (rr_f[644] + rr_f[651])/den;
    }
    double den, xq_35;
    xq_35 = a35_0;
    rr_r[644] *= xq_35;
    rr_r[651] *= xq_35;
  }
  // QSSA connected component
  {
    double a37_0;
    {
      double den = rr_r[646] + rr_r[652] + rr_r[663];
      a37_0 = (rr_f[646] + rr_f[652] + rr_f[663])/den;
    }
    double den, xq_37;
    xq_37 = a37_0;
    rr_r[646] *= xq_37;
    rr_r[652] *= xq_37;
    rr_r[663] *= xq_37;
  }
  // QSSA connected component
  {
    double a38_0;
    {
      double den = rr_f[655] + rr_f[658] + rr_r[647] + rr_r[664];
      a38_0 = (rr_f[647] + rr_f[664] + rr_r[655] + rr_r[658])/den;
    }
    double den, xq_38;
    xq_38 = a38_0;
    rr_f[655] *= xq_38;
    rr_f[658] *= xq_38;
    rr_r[647] *= xq_38;
    rr_r[664] *= xq_38;
  }
  // QSSA connected component
  {
    double a39_0;
    {
      double den = rr_f[656] + rr_f[659] + rr_r[648] + rr_r[776];
      a39_0 = (rr_f[648] + rr_f[776] + rr_r[656] + rr_r[659])/den;
    }
    double den, xq_39;
    xq_39 = a39_0;
    rr_f[656] *= xq_39;
    rr_f[659] *= xq_39;
    rr_r[648] *= xq_39;
    rr_r[776] *= xq_39;
  }
  // QSSA connected component
  {
    double a40_0;
    {
      double den = rr_f[657] + rr_f[660] + rr_r[649] + rr_r[665];
      a40_0 = (rr_f[649] + rr_f[665] + rr_r[657] + rr_r[660])/den;
    }
    double den, xq_40;
    xq_40 = a40_0;
    rr_f[657] *= xq_40;
    rr_f[660] *= xq_40;
    rr_r[649] *= xq_40;
    rr_r[665] *= xq_40;
  }
  // QSSA connected component
  {
    double a41_0;
    {
      double den = rr_r[650] + rr_r[653] + rr_r[666];
      a41_0 = (rr_f[650] + rr_f[653] + rr_f[666])/den;
    }
    double den, xq_41;
    xq_41 = a41_0;
    rr_r[650] *= xq_41;
    rr_r[653] *= xq_41;
    rr_r[666] *= xq_41;
  }
  // QSSA connected component
  {
    double a42_0;
    {
      double den = rr_f[678] + rr_r[677];
      a42_0 = (rr_f[677] + rr_r[678])/den;
    }
    double den, xq_42;
    xq_42 = a42_0;
    rr_f[678] *= xq_42;
    rr_r[677] *= xq_42;
  }
  // QSSA connected component
  {
    double a45_0;
    {
      double den = rr_f[713] + rr_r[709];
      a45_0 = (rr_f[709] + rr_f[710] + rr_f[711] + rr_r[713])/den;
    }
    double den, xq_45;
    xq_45 = a45_0;
    rr_f[713] *= xq_45;
    rr_r[709] *= xq_45;
  }
  // QSSA connected component
  {
    double a50_0;
    {
      double den = rr_f[760] + rr_r[756] + rr_r[850];
      a50_0 = (rr_f[756] + rr_f[850] + rr_r[760])/den;
    }
    double den, xq_50;
    xq_50 = a50_0;
    rr_f[760] *= xq_50;
    rr_r[756] *= xq_50;
    rr_r[850] *= xq_50;
  }
  // QSSA connected component
  {
    double a51_0;
    {
      double den = rr_f[761] + rr_r[840];
      a51_0 = (rr_f[840] + rr_r[761])/den;
    }
    double den, xq_51;
    xq_51 = a51_0;
    rr_f[761] *= xq_51;
    rr_r[840] *= xq_51;
  }
  // QSSA connected component
  {
    double a52_0;
    {
      double den = rr_f[842] + rr_f[845] + rr_f[846] + rr_r[836] + rr_r[839] + 
        rr_r[849] + rr_r[853]; 
      a52_0 = (rr_f[836] + rr_f[839] + rr_f[849] + rr_f[853] + rr_r[842] + 
        rr_r[845] + rr_r[846])/den; 
    }
    double den, xq_52;
    xq_52 = a52_0;
    rr_f[842] *= xq_52;
    rr_f[845] *= xq_52;
    rr_f[846] *= xq_52;
    rr_r[836] *= xq_52;
    rr_r[839] *= xq_52;
    rr_r[849] *= xq_52;
    rr_r[853] *= xq_52;
  }
  // QSSA connected component
  {
    double a53_0;
    {
      double den = rr_f[843] + rr_f[844] + rr_f[847] + rr_f[848] + rr_r[837] + 
        rr_r[838] + rr_r[851] + rr_r[852]; 
      a53_0 = (rr_f[837] + rr_f[838] + rr_f[851] + rr_f[852] + rr_r[843] + 
        rr_r[844] + rr_r[847] + rr_r[848])/den; 
    }
    double den, xq_53;
    xq_53 = a53_0;
    rr_f[843] *= xq_53;
    rr_f[844] *= xq_53;
    rr_f[847] *= xq_53;
    rr_f[848] *= xq_53;
    rr_r[837] *= xq_53;
    rr_r[838] *= xq_53;
    rr_r[851] *= xq_53;
    rr_r[852] *= xq_53;
  }
  // QSSA connected component
  {
    double a55_0;
    {
      double den = rr_r[770] + rr_r[771] + rr_r[772] + rr_r[773] + rr_r[774] + 
        rr_r[775]; 
      a55_0 = (rr_f[770] + rr_f[771] + rr_f[772] + rr_f[773] + rr_f[774] + 
        rr_f[775])/den; 
    }
    double den, xq_55;
    xq_55 = a55_0;
    rr_r[770] *= xq_55;
    rr_r[771] *= xq_55;
    rr_r[772] *= xq_55;
    rr_r[773] *= xq_55;
    rr_r[774] *= xq_55;
    rr_r[775] *= xq_55;
  }
  double diffusion[116];
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[0]) : 
    "l"(diffusion_array+0*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[1]) : 
    "l"(diffusion_array+1*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[2]) : 
    "l"(diffusion_array+2*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[3]) : 
    "l"(diffusion_array+3*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[4]) : 
    "l"(diffusion_array+4*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[5]) : 
    "l"(diffusion_array+5*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[6]) : 
    "l"(diffusion_array+6*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[7]) : 
    "l"(diffusion_array+7*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[8]) : 
    "l"(diffusion_array+8*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[9]) : 
    "l"(diffusion_array+9*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[10]) : 
    "l"(diffusion_array+10*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[11]) : 
    "l"(diffusion_array+11*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[12]) : 
    "l"(diffusion_array+12*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[13]) : 
    "l"(diffusion_array+13*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[14]) : 
    "l"(diffusion_array+14*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[15]) : 
    "l"(diffusion_array+15*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[16]) : 
    "l"(diffusion_array+16*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[17]) : 
    "l"(diffusion_array+17*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[18]) : 
    "l"(diffusion_array+18*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[19]) : 
    "l"(diffusion_array+19*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[20]) : 
    "l"(diffusion_array+20*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[21]) : 
    "l"(diffusion_array+21*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[22]) : 
    "l"(diffusion_array+22*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[23]) : 
    "l"(diffusion_array+23*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[24]) : 
    "l"(diffusion_array+24*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[25]) : 
    "l"(diffusion_array+25*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[26]) : 
    "l"(diffusion_array+26*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[27]) : 
    "l"(diffusion_array+27*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[28]) : 
    "l"(diffusion_array+28*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[29]) : 
    "l"(diffusion_array+29*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[30]) : 
    "l"(diffusion_array+30*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[31]) : 
    "l"(diffusion_array+31*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[32]) : 
    "l"(diffusion_array+32*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[33]) : 
    "l"(diffusion_array+33*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[34]) : 
    "l"(diffusion_array+34*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[35]) : 
    "l"(diffusion_array+35*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[36]) : 
    "l"(diffusion_array+36*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[37]) : 
    "l"(diffusion_array+37*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[38]) : 
    "l"(diffusion_array+38*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[39]) : 
    "l"(diffusion_array+39*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[40]) : 
    "l"(diffusion_array+40*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[41]) : 
    "l"(diffusion_array+41*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[42]) : 
    "l"(diffusion_array+42*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[43]) : 
    "l"(diffusion_array+43*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[44]) : 
    "l"(diffusion_array+44*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[45]) : 
    "l"(diffusion_array+45*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[46]) : 
    "l"(diffusion_array+46*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[47]) : 
    "l"(diffusion_array+47*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[48]) : 
    "l"(diffusion_array+48*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[49]) : 
    "l"(diffusion_array+49*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[50]) : 
    "l"(diffusion_array+50*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[51]) : 
    "l"(diffusion_array+51*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[52]) : 
    "l"(diffusion_array+52*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[53]) : 
    "l"(diffusion_array+53*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[54]) : 
    "l"(diffusion_array+54*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[55]) : 
    "l"(diffusion_array+55*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[56]) : 
    "l"(diffusion_array+56*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[57]) : 
    "l"(diffusion_array+57*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[58]) : 
    "l"(diffusion_array+58*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[59]) : 
    "l"(diffusion_array+59*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[60]) : 
    "l"(diffusion_array+60*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[61]) : 
    "l"(diffusion_array+61*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[62]) : 
    "l"(diffusion_array+62*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[63]) : 
    "l"(diffusion_array+63*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[64]) : 
    "l"(diffusion_array+64*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[65]) : 
    "l"(diffusion_array+65*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[66]) : 
    "l"(diffusion_array+66*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[67]) : 
    "l"(diffusion_array+67*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[68]) : 
    "l"(diffusion_array+68*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[69]) : 
    "l"(diffusion_array+69*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[70]) : 
    "l"(diffusion_array+70*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[71]) : 
    "l"(diffusion_array+71*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[72]) : 
    "l"(diffusion_array+72*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[73]) : 
    "l"(diffusion_array+73*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[74]) : 
    "l"(diffusion_array+74*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[75]) : 
    "l"(diffusion_array+75*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[76]) : 
    "l"(diffusion_array+76*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[77]) : 
    "l"(diffusion_array+77*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[78]) : 
    "l"(diffusion_array+78*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[79]) : 
    "l"(diffusion_array+79*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[80]) : 
    "l"(diffusion_array+80*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[81]) : 
    "l"(diffusion_array+81*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[82]) : 
    "l"(diffusion_array+82*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[83]) : 
    "l"(diffusion_array+83*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[84]) : 
    "l"(diffusion_array+84*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[85]) : 
    "l"(diffusion_array+85*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[86]) : 
    "l"(diffusion_array+86*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[87]) : 
    "l"(diffusion_array+87*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[88]) : 
    "l"(diffusion_array+88*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[89]) : 
    "l"(diffusion_array+89*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[90]) : 
    "l"(diffusion_array+90*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[91]) : 
    "l"(diffusion_array+91*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[92]) : 
    "l"(diffusion_array+92*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[93]) : 
    "l"(diffusion_array+93*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[94]) : 
    "l"(diffusion_array+94*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[95]) : 
    "l"(diffusion_array+95*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[96]) : 
    "l"(diffusion_array+96*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[97]) : 
    "l"(diffusion_array+97*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[98]) : 
    "l"(diffusion_array+98*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[99]) : 
    "l"(diffusion_array+99*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[100]) : 
    "l"(diffusion_array+100*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[101]) : 
    "l"(diffusion_array+101*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[102]) : 
    "l"(diffusion_array+102*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[103]) : 
    "l"(diffusion_array+103*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[104]) : 
    "l"(diffusion_array+104*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[105]) : 
    "l"(diffusion_array+105*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[106]) : 
    "l"(diffusion_array+106*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[107]) : 
    "l"(diffusion_array+107*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[108]) : 
    "l"(diffusion_array+108*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[109]) : 
    "l"(diffusion_array+109*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[110]) : 
    "l"(diffusion_array+110*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[111]) : 
    "l"(diffusion_array+111*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[112]) : 
    "l"(diffusion_array+112*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[113]) : 
    "l"(diffusion_array+113*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[114]) : 
    "l"(diffusion_array+114*spec_stride) : "memory"); 
  asm volatile("ld.global.cg.f64 %0, [%1];" : "=d"(diffusion[115]) : 
    "l"(diffusion_array+115*spec_stride) : "memory"); 
  // Stiff species H
  {
    double ddot = rr_f[0] + rr_f[8] + rr_f[9] + rr_f[10] + rr_f[16] + rr_f[17] + 
      rr_f[27] + rr_f[36] + rr_f[39] + rr_f[50] + rr_f[51] + rr_f[60] + rr_f[62] 
      + rr_f[128] + rr_f[65] + rr_f[76] + rr_f[77] + rr_f[86] + rr_f[87] + 
      rr_f[110] + rr_f[118] + rr_f[122] + rr_f[129] + rr_f[138] + rr_f[143] + 
      rr_f[144] + rr_f[166] + rr_f[176] + rr_f[183] + rr_f[184] + rr_f[191] + 
      rr_f[194] + rr_f[196] + rr_f[206] + rr_f[211] + rr_f[220] + rr_f[235] + 
      rr_f[246] + rr_f[259] + rr_f[532] + rr_f[284] + rr_f[285] + rr_f[286] + 
      rr_f[287] + rr_f[301] + rr_f[314] + rr_f[322] + rr_f[332] + rr_f[345] + 
      rr_f[356] + rr_f[360] + rr_f[379] + rr_f[396] + rr_f[414] + rr_f[454] + 
      rr_f[455] + rr_f[483] + rr_f[489] + rr_f[508] + rr_f[509] + rr_f[510] + 
      rr_f[545] + rr_f[546] + rr_f[547] + rr_f[548] + rr_f[610] + rr_f[616] + 
      rr_f[695] + rr_f[701] + rr_f[702] + rr_f[738] + rr_f[794] + rr_f[795] + 
      rr_f[796] + rr_r[1] + rr_r[2] + rr_r[4] + rr_r[4] + rr_r[6] + rr_r[7] + 
      rr_r[23] + rr_r[25] + rr_r[29] + rr_r[44] + rr_r[55] + rr_r[75] + rr_r[94] 
      + rr_r[95] + rr_r[99] + rr_r[117] + rr_r[118] + rr_r[120] + rr_r[262] + 
      rr_r[265] + rr_r[139] + rr_r[140] + rr_r[145] + rr_r[150] + rr_r[180] + 
      rr_r[198] + rr_r[205] + rr_r[215] + rr_r[216] + rr_r[258] + rr_r[539] + 
      rr_r[540] + rr_r[269] + rr_r[270] + rr_r[271] + rr_r[298] + rr_r[312] + 
      rr_r[317] + rr_r[318] + rr_r[324] + rr_r[335] + rr_r[354] + rr_r[355] + 
      rr_r[364] + rr_r[371] + rr_r[372] + rr_r[377] + rr_r[388] + rr_r[408] + 
      rr_r[423] + rr_r[425] + rr_r[441] + rr_r[453] + rr_r[470] + rr_r[476] + 
      rr_r[493] + rr_r[494] + rr_r[541] + rr_r[542] + rr_r[591] + rr_r[593] + 
      rr_r[594] + rr_r[596] + rr_r[597] + rr_r[599] + rr_r[679] + rr_r[680] + 
      rr_r[681] + rr_r[687] + rr_r[688] + rr_r[733] + rr_r[748] + rr_r[749] + 
      rr_r[791] + rr_r[792] + rr_r[793] + rr_r[825] + rr_r[826]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[0])
    {
      double cdot = rr_f[1] + rr_f[2] + rr_f[4] + rr_f[4] + rr_f[6] + rr_f[7] + 
        rr_f[23] + rr_f[25] + rr_f[29] + rr_f[33] + rr_f[44] + rr_f[48] + 
        rr_f[50] + rr_f[55] + rr_f[75] + rr_f[94] + rr_f[95] + rr_f[99] + 
        rr_f[116] + rr_f[117] + rr_f[118] + rr_f[119] + rr_f[119] + rr_f[120] + 
        rr_f[124] + rr_f[124] + rr_f[125] + rr_f[126] + rr_f[126] + rr_f[262] + 
        rr_f[265] + rr_f[139] + rr_f[140] + rr_f[145] + rr_f[150] + rr_f[180] + 
        rr_f[192] + rr_f[198] + rr_f[205] + rr_f[215] + rr_f[216] + rr_f[258] + 
        rr_f[539] + rr_f[540] + rr_f[269] + rr_f[270] + rr_f[271] + rr_f[273] + 
        rr_f[274] + rr_f[274] + rr_f[298] + rr_f[312] + rr_f[317] + rr_f[318] + 
        rr_f[324] + rr_f[335] + rr_f[343] + rr_f[354] + rr_f[355] + rr_f[358] + 
        rr_f[364] + rr_f[371] + rr_f[372] + rr_f[377] + rr_f[388] + rr_f[408] + 
        rr_f[423] + rr_f[425] + rr_f[441] + rr_f[453] + rr_f[470] + rr_f[476] + 
        rr_f[493] + rr_f[494] + rr_f[541] + rr_f[542] + rr_f[591] + rr_f[593] + 
        rr_f[594] + rr_f[596] + rr_f[597] + rr_f[599] + rr_f[679] + rr_f[680] + 
        rr_f[681] + rr_f[687] + rr_f[688] + rr_f[733] + rr_f[748] + rr_f[749] + 
        rr_f[791] + rr_f[792] + rr_f[793] + rr_f[825] + rr_f[826] + rr_r[0] + 
        rr_r[8] + rr_r[9] + rr_r[10] + rr_r[16] + rr_r[17] + rr_r[27] + rr_r[36] 
        + rr_r[39] + rr_r[60] + rr_r[62] + rr_r[128] + rr_r[65] + rr_r[76] + 
        rr_r[77] + rr_r[86] + rr_r[87] + rr_r[110] + rr_r[118] + rr_r[122] + 
        rr_r[129] + rr_r[138] + rr_r[143] + rr_r[144] + rr_r[166] + rr_r[176] + 
        rr_r[183] + rr_r[184] + rr_r[191] + rr_r[194] + rr_r[196] + rr_r[206] + 
        rr_r[211] + rr_r[220] + rr_r[235] + rr_r[246] + rr_r[259] + rr_r[532] + 
        rr_r[284] + rr_r[285] + rr_r[286] + rr_r[287] + rr_r[301] + rr_r[314] + 
        rr_r[322] + rr_r[332] + rr_r[345] + rr_r[356] + rr_r[379] + rr_r[396] + 
        rr_r[414] + rr_r[454] + rr_r[455] + rr_r[483] + rr_r[489] + rr_r[508] + 
        rr_r[509] + rr_r[510] + rr_r[545] + rr_r[546] + rr_r[547] + rr_r[548] + 
        rr_r[610] + rr_r[616] + rr_r[695] + rr_r[701] + rr_r[702] + rr_r[738] + 
        rr_r[794] + rr_r[795] + rr_r[796]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[0] * 0.9920930186414277;
      double c0 = mole_frac[0] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[0]- c0) / dt) 
        * recip_ddot; 
      rr_f[0] *= scale_r;
      rr_f[8] *= scale_r;
      rr_f[9] *= scale_r;
      rr_f[10] *= scale_r;
      rr_f[16] *= scale_r;
      rr_f[17] *= scale_r;
      rr_f[27] *= scale_r;
      rr_f[36] *= scale_r;
      rr_f[39] *= scale_r;
      rr_f[50] *= scale_r;
      rr_f[51] *= scale_r;
      rr_f[60] *= scale_r;
      rr_f[62] *= scale_r;
      rr_f[128] *= scale_r;
      rr_f[65] *= scale_r;
      rr_f[76] *= scale_r;
      rr_f[77] *= scale_r;
      rr_f[86] *= scale_r;
      rr_f[87] *= scale_r;
      rr_f[110] *= scale_r;
      rr_f[118] *= scale_r;
      rr_f[122] *= scale_r;
      rr_f[129] *= scale_r;
      rr_f[138] *= scale_r;
      rr_f[143] *= scale_r;
      rr_f[144] *= scale_r;
      rr_f[166] *= scale_r;
      rr_f[176] *= scale_r;
      rr_f[183] *= scale_r;
      rr_f[184] *= scale_r;
      rr_f[191] *= scale_r;
      rr_f[194] *= scale_r;
      rr_f[196] *= scale_r;
      rr_f[206] *= scale_r;
      rr_f[211] *= scale_r;
      rr_f[220] *= scale_r;
      rr_f[235] *= scale_r;
      rr_f[246] *= scale_r;
      rr_f[259] *= scale_r;
      rr_f[532] *= scale_r;
      rr_f[284] *= scale_r;
      rr_f[285] *= scale_r;
      rr_f[286] *= scale_r;
      rr_f[287] *= scale_r;
      rr_f[301] *= scale_r;
      rr_f[314] *= scale_r;
      rr_f[322] *= scale_r;
      rr_f[332] *= scale_r;
      rr_f[345] *= scale_r;
      rr_f[356] *= scale_r;
      rr_f[360] *= scale_r;
      rr_f[379] *= scale_r;
      rr_f[396] *= scale_r;
      rr_f[414] *= scale_r;
      rr_f[454] *= scale_r;
      rr_f[455] *= scale_r;
      rr_f[483] *= scale_r;
      rr_f[489] *= scale_r;
      rr_f[508] *= scale_r;
      rr_f[509] *= scale_r;
      rr_f[510] *= scale_r;
      rr_f[545] *= scale_r;
      rr_f[546] *= scale_r;
      rr_f[547] *= scale_r;
      rr_f[548] *= scale_r;
      rr_f[610] *= scale_r;
      rr_f[616] *= scale_r;
      rr_f[695] *= scale_r;
      rr_f[701] *= scale_r;
      rr_f[702] *= scale_r;
      rr_f[738] *= scale_r;
      rr_f[794] *= scale_r;
      rr_f[795] *= scale_r;
      rr_f[796] *= scale_r;
      rr_r[1] *= scale_r;
      rr_r[2] *= scale_r;
      rr_r[4] *= scale_r;
      rr_r[4] *= scale_r;
      rr_r[6] *= scale_r;
      rr_r[7] *= scale_r;
      rr_r[23] *= scale_r;
      rr_r[25] *= scale_r;
      rr_r[29] *= scale_r;
      rr_r[44] *= scale_r;
      rr_r[55] *= scale_r;
      rr_r[75] *= scale_r;
      rr_r[94] *= scale_r;
      rr_r[95] *= scale_r;
      rr_r[99] *= scale_r;
      rr_r[117] *= scale_r;
      rr_r[118] *= scale_r;
      rr_r[120] *= scale_r;
      rr_r[262] *= scale_r;
      rr_r[265] *= scale_r;
      rr_r[139] *= scale_r;
      rr_r[140] *= scale_r;
      rr_r[145] *= scale_r;
      rr_r[150] *= scale_r;
      rr_r[180] *= scale_r;
      rr_r[198] *= scale_r;
      rr_r[205] *= scale_r;
      rr_r[215] *= scale_r;
      rr_r[216] *= scale_r;
      rr_r[258] *= scale_r;
      rr_r[539] *= scale_r;
      rr_r[540] *= scale_r;
      rr_r[269] *= scale_r;
      rr_r[270] *= scale_r;
      rr_r[271] *= scale_r;
      rr_r[298] *= scale_r;
      rr_r[312] *= scale_r;
      rr_r[317] *= scale_r;
      rr_r[318] *= scale_r;
      rr_r[324] *= scale_r;
      rr_r[335] *= scale_r;
      rr_r[354] *= scale_r;
      rr_r[355] *= scale_r;
      rr_r[364] *= scale_r;
      rr_r[371] *= scale_r;
      rr_r[372] *= scale_r;
      rr_r[377] *= scale_r;
      rr_r[388] *= scale_r;
      rr_r[408] *= scale_r;
      rr_r[423] *= scale_r;
      rr_r[425] *= scale_r;
      rr_r[441] *= scale_r;
      rr_r[453] *= scale_r;
      rr_r[470] *= scale_r;
      rr_r[476] *= scale_r;
      rr_r[493] *= scale_r;
      rr_r[494] *= scale_r;
      rr_r[541] *= scale_r;
      rr_r[542] *= scale_r;
      rr_r[591] *= scale_r;
      rr_r[593] *= scale_r;
      rr_r[594] *= scale_r;
      rr_r[596] *= scale_r;
      rr_r[597] *= scale_r;
      rr_r[599] *= scale_r;
      rr_r[679] *= scale_r;
      rr_r[680] *= scale_r;
      rr_r[681] *= scale_r;
      rr_r[687] *= scale_r;
      rr_r[688] *= scale_r;
      rr_r[733] *= scale_r;
      rr_r[748] *= scale_r;
      rr_r[749] *= scale_r;
      rr_r[791] *= scale_r;
      rr_r[792] *= scale_r;
      rr_r[793] *= scale_r;
      rr_r[825] *= scale_r;
      rr_r[826] *= scale_r;
    }
  }
  // Stiff species O
  {
    double ddot = rr_f[1] + rr_f[3] + rr_f[11] + rr_f[18] + rr_f[21] + rr_f[28] 
      + rr_f[29] + rr_f[40] + rr_f[54] + rr_f[71] + rr_f[78] + rr_f[89] + 
      rr_f[99] + rr_f[111] + rr_f[119] + rr_f[126] + rr_f[262] + rr_f[263] + 
      rr_f[130] + rr_f[145] + rr_f[167] + rr_f[177] + rr_f[185] + rr_f[186] + 
      rr_f[192] + rr_f[197] + rr_f[198] + rr_f[214] + rr_f[215] + rr_f[221] + 
      rr_f[236] + rr_f[247] + rr_f[533] + rr_f[272] + rr_f[273] + rr_f[274] + 
      rr_f[275] + rr_f[276] + rr_f[277] + rr_f[330] + rr_f[331] + rr_f[341] + 
      rr_f[342] + rr_f[343] + rr_f[344] + rr_f[361] + rr_f[391] + rr_f[406] + 
      rr_f[407] + rr_f[415] + rr_f[456] + rr_f[457] + rr_f[465] + rr_f[470] + 
      rr_f[481] + rr_f[487] + rr_f[511] + rr_f[512] + rr_f[549] + rr_f[550] + 
      rr_f[551] + rr_f[552] + rr_f[629] + rr_f[630] + rr_f[683] + rr_f[684] + 
      rr_f[694] + rr_f[728] + rr_f[739] + rr_f[797] + rr_f[798] + rr_f[799] + 
      rr_r[0] + rr_r[5] + rr_r[5] + rr_r[6] + rr_r[22] + rr_r[100] + rr_r[123] + 
      rr_r[209] + rr_r[320]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[2])
    {
      double cdot = rr_f[0] + rr_f[5] + rr_f[5] + rr_f[6] + rr_f[22] + rr_f[100] 
        + rr_f[123] + rr_f[209] + rr_f[320] + rr_r[1] + rr_r[3] + rr_r[11] + 
        rr_r[18] + rr_r[21] + rr_r[28] + rr_r[29] + rr_r[40] + rr_r[71] + 
        rr_r[78] + rr_r[89] + rr_r[99] + rr_r[111] + rr_r[262] + rr_r[263] + 
        rr_r[130] + rr_r[145] + rr_r[167] + rr_r[177] + rr_r[185] + rr_r[186] + 
        rr_r[197] + rr_r[198] + rr_r[214] + rr_r[215] + rr_r[221] + rr_r[236] + 
        rr_r[247] + rr_r[533] + rr_r[272] + rr_r[275] + rr_r[276] + rr_r[277] + 
        rr_r[330] + rr_r[331] + rr_r[341] + rr_r[342] + rr_r[344] + rr_r[391] + 
        rr_r[406] + rr_r[407] + rr_r[415] + rr_r[457] + rr_r[465] + rr_r[470] + 
        rr_r[481] + rr_r[487] + rr_r[511] + rr_r[512] + rr_r[549] + rr_r[550] + 
        rr_r[551] + rr_r[552] + rr_r[728] + rr_r[739] + rr_r[797] + rr_r[798] + 
        rr_r[799]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[2] * 0.06250234383789392;
      double c0 = mole_frac[2] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[2]- c0) / dt) 
        * recip_ddot; 
      rr_f[1] *= scale_r;
      rr_f[3] *= scale_r;
      rr_f[11] *= scale_r;
      rr_f[18] *= scale_r;
      rr_f[21] *= scale_r;
      rr_f[28] *= scale_r;
      rr_f[29] *= scale_r;
      rr_f[40] *= scale_r;
      rr_f[54] *= scale_r;
      rr_f[71] *= scale_r;
      rr_f[78] *= scale_r;
      rr_f[89] *= scale_r;
      rr_f[99] *= scale_r;
      rr_f[111] *= scale_r;
      rr_f[119] *= scale_r;
      rr_f[126] *= scale_r;
      rr_f[262] *= scale_r;
      rr_f[263] *= scale_r;
      rr_f[130] *= scale_r;
      rr_f[145] *= scale_r;
      rr_f[167] *= scale_r;
      rr_f[177] *= scale_r;
      rr_f[185] *= scale_r;
      rr_f[186] *= scale_r;
      rr_f[192] *= scale_r;
      rr_f[197] *= scale_r;
      rr_f[198] *= scale_r;
      rr_f[214] *= scale_r;
      rr_f[215] *= scale_r;
      rr_f[221] *= scale_r;
      rr_f[236] *= scale_r;
      rr_f[247] *= scale_r;
      rr_f[533] *= scale_r;
      rr_f[272] *= scale_r;
      rr_f[273] *= scale_r;
      rr_f[274] *= scale_r;
      rr_f[275] *= scale_r;
      rr_f[276] *= scale_r;
      rr_f[277] *= scale_r;
      rr_f[330] *= scale_r;
      rr_f[331] *= scale_r;
      rr_f[341] *= scale_r;
      rr_f[342] *= scale_r;
      rr_f[343] *= scale_r;
      rr_f[344] *= scale_r;
      rr_f[361] *= scale_r;
      rr_f[391] *= scale_r;
      rr_f[406] *= scale_r;
      rr_f[407] *= scale_r;
      rr_f[415] *= scale_r;
      rr_f[456] *= scale_r;
      rr_f[457] *= scale_r;
      rr_f[465] *= scale_r;
      rr_f[470] *= scale_r;
      rr_f[481] *= scale_r;
      rr_f[487] *= scale_r;
      rr_f[511] *= scale_r;
      rr_f[512] *= scale_r;
      rr_f[549] *= scale_r;
      rr_f[550] *= scale_r;
      rr_f[551] *= scale_r;
      rr_f[552] *= scale_r;
      rr_f[629] *= scale_r;
      rr_f[630] *= scale_r;
      rr_f[683] *= scale_r;
      rr_f[684] *= scale_r;
      rr_f[694] *= scale_r;
      rr_f[728] *= scale_r;
      rr_f[739] *= scale_r;
      rr_f[797] *= scale_r;
      rr_f[798] *= scale_r;
      rr_f[799] *= scale_r;
      rr_r[0] *= scale_r;
      rr_r[5] *= scale_r;
      rr_r[5] *= scale_r;
      rr_r[6] *= scale_r;
      rr_r[22] *= scale_r;
      rr_r[100] *= scale_r;
      rr_r[123] *= scale_r;
      rr_r[209] *= scale_r;
      rr_r[320] *= scale_r;
    }
  }
  // Stiff species OH
  {
    double ddot = rr_f[2] + rr_f[6] + rr_f[12] + rr_f[19] + rr_f[20] + rr_f[23] 
      + rr_f[30] + rr_f[38] + rr_f[48] + rr_f[49] + rr_f[70] + rr_f[79] + 
      rr_f[80] + rr_f[88] + rr_f[92] + rr_f[93] + rr_f[94] + rr_f[95] + rr_f[96] 
      + rr_f[112] + rr_f[120] + rr_f[131] + rr_f[168] + rr_f[173] + rr_f[174] + 
      rr_f[187] + rr_f[188] + rr_f[190] + rr_f[199] + rr_f[212] + rr_f[216] + 
      rr_f[217] + rr_f[219] + rr_f[237] + rr_f[248] + rr_f[513] + rr_f[514] + 
      rr_f[261] + rr_f[531] + rr_f[278] + rr_f[279] + rr_f[280] + rr_f[328] + 
      rr_f[329] + rr_f[339] + rr_f[340] + rr_f[349] + rr_f[357] + rr_f[359] + 
      rr_f[380] + rr_f[403] + rr_f[404] + rr_f[405] + rr_f[413] + rr_f[444] + 
      rr_f[464] + rr_f[482] + rr_f[484] + rr_f[528] + rr_f[553] + rr_f[554] + 
      rr_f[555] + rr_f[556] + rr_f[611] + rr_f[617] + rr_f[626] + rr_f[627] + 
      rr_f[628] + rr_f[669] + rr_f[670] + rr_f[671] + rr_f[672] + rr_f[682] + 
      rr_f[692] + rr_f[693] + rr_f[696] + rr_f[703] + rr_f[704] + rr_f[717] + 
      rr_f[718] + rr_f[730] + rr_f[740] + rr_f[752] + rr_f[753] + rr_f[764] + 
      rr_f[765] + rr_f[800] + rr_f[801] + rr_f[802] + rr_r[0] + rr_r[1] + 
      rr_r[3] + rr_r[3] + rr_r[7] + rr_r[10] + rr_r[10] + rr_r[11] + rr_r[15] + 
      rr_r[15] + rr_r[16] + rr_r[18] + rr_r[24] + rr_r[28] + rr_r[40] + rr_r[43] 
      + rr_r[47] + rr_r[71] + rr_r[73] + rr_r[74] + rr_r[78] + rr_r[89] + 
      rr_r[97] + rr_r[101] + rr_r[110] + rr_r[113] + rr_r[130] + rr_r[146] + 
      rr_r[157] + rr_r[160] + rr_r[161] + rr_r[167] + rr_r[177] + rr_r[186] + 
      rr_r[213] + rr_r[221] + rr_r[232] + rr_r[236] + rr_r[247] + rr_r[533] + 
      rr_r[275] + rr_r[276] + rr_r[277] + rr_r[299] + rr_r[309] + rr_r[327] + 
      rr_r[336] + rr_r[338] + rr_r[344] + rr_r[352] + rr_r[392] + rr_r[415] + 
      rr_r[421] + rr_r[449] + rr_r[457] + rr_r[467] + rr_r[473] + rr_r[481] + 
      rr_r[487] + rr_r[491] + rr_r[495] + rr_r[506] + rr_r[511] + rr_r[512] + 
      rr_r[549] + rr_r[550] + rr_r[551] + rr_r[552] + rr_r[622] + rr_r[667] + 
      rr_r[677] + rr_r[706] + rr_r[709] + rr_r[728] + rr_r[739] + rr_r[777] + 
      rr_r[778] + rr_r[779] + rr_r[780] + rr_r[781] + rr_r[782] + rr_r[783] + 
      rr_r[784] + rr_r[797] + rr_r[798] + rr_r[799] + rr_r[854] + rr_r[855] + 
      rr_r[856] + rr_r[857] + rr_r[858]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[4])
    {
      double cdot = rr_f[0] + rr_f[1] + rr_f[3] + rr_f[3] + rr_f[7] + rr_f[10] + 
        rr_f[10] + rr_f[11] + rr_f[15] + rr_f[15] + rr_f[16] + rr_f[18] + 
        rr_f[24] + rr_f[28] + rr_f[33] + rr_f[40] + rr_f[43] + rr_f[47] + 
        rr_f[49] + rr_f[51] + rr_f[52] + rr_f[53] + rr_f[54] + rr_f[54] + 
        rr_f[71] + rr_f[73] + rr_f[74] + rr_f[78] + rr_f[89] + rr_f[97] + 
        rr_f[101] + rr_f[110] + rr_f[113] + rr_f[116] + rr_f[125] + rr_f[130] + 
        rr_f[146] + rr_f[157] + rr_f[160] + rr_f[161] + rr_f[167] + rr_f[177] + 
        rr_f[181] + rr_f[186] + rr_f[193] + rr_f[213] + rr_f[221] + rr_f[232] + 
        rr_f[236] + rr_f[247] + rr_f[533] + rr_f[275] + rr_f[276] + rr_f[277] + 
        rr_f[299] + rr_f[309] + rr_f[310] + rr_f[327] + rr_f[336] + rr_f[338] + 
        rr_f[344] + rr_f[352] + rr_f[392] + rr_f[415] + rr_f[421] + rr_f[433] + 
        rr_f[449] + rr_f[450] + rr_f[451] + rr_f[457] + rr_f[467] + rr_f[469] + 
        rr_f[473] + rr_f[481] + rr_f[487] + rr_f[491] + rr_f[495] + rr_f[500] + 
        rr_f[503] + rr_f[506] + rr_f[511] + rr_f[512] + rr_f[549] + rr_f[550] + 
        rr_f[551] + rr_f[552] + rr_f[622] + rr_f[654] + rr_f[655] + rr_f[656] + 
        rr_f[657] + rr_f[658] + rr_f[659] + rr_f[660] + rr_f[667] + rr_f[668] + 
        rr_f[677] + rr_f[706] + rr_f[709] + rr_f[724] + rr_f[728] + rr_f[739] + 
        rr_f[762] + rr_f[768] + rr_f[777] + rr_f[778] + rr_f[779] + rr_f[780] + 
        rr_f[781] + rr_f[782] + rr_f[783] + rr_f[784] + rr_f[785] + rr_f[786] + 
        rr_f[787] + rr_f[788] + rr_f[789] + rr_f[790] + rr_f[797] + rr_f[798] + 
        rr_f[799] + rr_f[842] + rr_f[843] + rr_f[844] + rr_f[845] + rr_f[846] + 
        rr_f[847] + rr_f[848] + rr_f[854] + rr_f[855] + rr_f[856] + rr_f[857] + 
        rr_f[858] + rr_f[859] + rr_f[860] + rr_r[2] + rr_r[6] + rr_r[12] + 
        rr_r[19] + rr_r[20] + rr_r[23] + rr_r[30] + rr_r[38] + rr_r[70] + 
        rr_r[79] + rr_r[80] + rr_r[88] + rr_r[92] + rr_r[93] + rr_r[94] + 
        rr_r[95] + rr_r[96] + rr_r[112] + rr_r[120] + rr_r[131] + rr_r[168] + 
        rr_r[173] + rr_r[174] + rr_r[187] + rr_r[188] + rr_r[199] + rr_r[212] + 
        rr_r[216] + rr_r[217] + rr_r[219] + rr_r[237] + rr_r[248] + rr_r[513] + 
        rr_r[514] + rr_r[261] + rr_r[531] + rr_r[278] + rr_r[279] + rr_r[280] + 
        rr_r[328] + rr_r[329] + rr_r[339] + rr_r[340] + rr_r[349] + rr_r[357] + 
        rr_r[380] + rr_r[403] + rr_r[404] + rr_r[405] + rr_r[413] + rr_r[444] + 
        rr_r[464] + rr_r[482] + rr_r[484] + rr_r[528] + rr_r[553] + rr_r[554] + 
        rr_r[555] + rr_r[556] + rr_r[611] + rr_r[617] + rr_r[682] + rr_r[696] + 
        rr_r[703] + rr_r[704] + rr_r[717] + rr_r[718] + rr_r[730] + rr_r[740] + 
        rr_r[800] + rr_r[801] + rr_r[802]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[4] * 0.05879803873262004;
      double c0 = mole_frac[4] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[4]- c0) / dt) 
        * recip_ddot; 
      rr_f[2] *= scale_r;
      rr_f[6] *= scale_r;
      rr_f[12] *= scale_r;
      rr_f[19] *= scale_r;
      rr_f[20] *= scale_r;
      rr_f[23] *= scale_r;
      rr_f[30] *= scale_r;
      rr_f[38] *= scale_r;
      rr_f[48] *= scale_r;
      rr_f[49] *= scale_r;
      rr_f[70] *= scale_r;
      rr_f[79] *= scale_r;
      rr_f[80] *= scale_r;
      rr_f[88] *= scale_r;
      rr_f[92] *= scale_r;
      rr_f[93] *= scale_r;
      rr_f[94] *= scale_r;
      rr_f[95] *= scale_r;
      rr_f[96] *= scale_r;
      rr_f[112] *= scale_r;
      rr_f[120] *= scale_r;
      rr_f[131] *= scale_r;
      rr_f[168] *= scale_r;
      rr_f[173] *= scale_r;
      rr_f[174] *= scale_r;
      rr_f[187] *= scale_r;
      rr_f[188] *= scale_r;
      rr_f[190] *= scale_r;
      rr_f[199] *= scale_r;
      rr_f[212] *= scale_r;
      rr_f[216] *= scale_r;
      rr_f[217] *= scale_r;
      rr_f[219] *= scale_r;
      rr_f[237] *= scale_r;
      rr_f[248] *= scale_r;
      rr_f[513] *= scale_r;
      rr_f[514] *= scale_r;
      rr_f[261] *= scale_r;
      rr_f[531] *= scale_r;
      rr_f[278] *= scale_r;
      rr_f[279] *= scale_r;
      rr_f[280] *= scale_r;
      rr_f[328] *= scale_r;
      rr_f[329] *= scale_r;
      rr_f[339] *= scale_r;
      rr_f[340] *= scale_r;
      rr_f[349] *= scale_r;
      rr_f[357] *= scale_r;
      rr_f[359] *= scale_r;
      rr_f[380] *= scale_r;
      rr_f[403] *= scale_r;
      rr_f[404] *= scale_r;
      rr_f[405] *= scale_r;
      rr_f[413] *= scale_r;
      rr_f[444] *= scale_r;
      rr_f[464] *= scale_r;
      rr_f[482] *= scale_r;
      rr_f[484] *= scale_r;
      rr_f[528] *= scale_r;
      rr_f[553] *= scale_r;
      rr_f[554] *= scale_r;
      rr_f[555] *= scale_r;
      rr_f[556] *= scale_r;
      rr_f[611] *= scale_r;
      rr_f[617] *= scale_r;
      rr_f[626] *= scale_r;
      rr_f[627] *= scale_r;
      rr_f[628] *= scale_r;
      rr_f[669] *= scale_r;
      rr_f[670] *= scale_r;
      rr_f[671] *= scale_r;
      rr_f[672] *= scale_r;
      rr_f[682] *= scale_r;
      rr_f[692] *= scale_r;
      rr_f[693] *= scale_r;
      rr_f[696] *= scale_r;
      rr_f[703] *= scale_r;
      rr_f[704] *= scale_r;
      rr_f[717] *= scale_r;
      rr_f[718] *= scale_r;
      rr_f[730] *= scale_r;
      rr_f[740] *= scale_r;
      rr_f[752] *= scale_r;
      rr_f[753] *= scale_r;
      rr_f[764] *= scale_r;
      rr_f[765] *= scale_r;
      rr_f[800] *= scale_r;
      rr_f[801] *= scale_r;
      rr_f[802] *= scale_r;
      rr_r[0] *= scale_r;
      rr_r[1] *= scale_r;
      rr_r[3] *= scale_r;
      rr_r[3] *= scale_r;
      rr_r[7] *= scale_r;
      rr_r[10] *= scale_r;
      rr_r[10] *= scale_r;
      rr_r[11] *= scale_r;
      rr_r[15] *= scale_r;
      rr_r[15] *= scale_r;
      rr_r[16] *= scale_r;
      rr_r[18] *= scale_r;
      rr_r[24] *= scale_r;
      rr_r[28] *= scale_r;
      rr_r[40] *= scale_r;
      rr_r[43] *= scale_r;
      rr_r[47] *= scale_r;
      rr_r[71] *= scale_r;
      rr_r[73] *= scale_r;
      rr_r[74] *= scale_r;
      rr_r[78] *= scale_r;
      rr_r[89] *= scale_r;
      rr_r[97] *= scale_r;
      rr_r[101] *= scale_r;
      rr_r[110] *= scale_r;
      rr_r[113] *= scale_r;
      rr_r[130] *= scale_r;
      rr_r[146] *= scale_r;
      rr_r[157] *= scale_r;
      rr_r[160] *= scale_r;
      rr_r[161] *= scale_r;
      rr_r[167] *= scale_r;
      rr_r[177] *= scale_r;
      rr_r[186] *= scale_r;
      rr_r[213] *= scale_r;
      rr_r[221] *= scale_r;
      rr_r[232] *= scale_r;
      rr_r[236] *= scale_r;
      rr_r[247] *= scale_r;
      rr_r[533] *= scale_r;
      rr_r[275] *= scale_r;
      rr_r[276] *= scale_r;
      rr_r[277] *= scale_r;
      rr_r[299] *= scale_r;
      rr_r[309] *= scale_r;
      rr_r[327] *= scale_r;
      rr_r[336] *= scale_r;
      rr_r[338] *= scale_r;
      rr_r[344] *= scale_r;
      rr_r[352] *= scale_r;
      rr_r[392] *= scale_r;
      rr_r[415] *= scale_r;
      rr_r[421] *= scale_r;
      rr_r[449] *= scale_r;
      rr_r[457] *= scale_r;
      rr_r[467] *= scale_r;
      rr_r[473] *= scale_r;
      rr_r[481] *= scale_r;
      rr_r[487] *= scale_r;
      rr_r[491] *= scale_r;
      rr_r[495] *= scale_r;
      rr_r[506] *= scale_r;
      rr_r[511] *= scale_r;
      rr_r[512] *= scale_r;
      rr_r[549] *= scale_r;
      rr_r[550] *= scale_r;
      rr_r[551] *= scale_r;
      rr_r[552] *= scale_r;
      rr_r[622] *= scale_r;
      rr_r[667] *= scale_r;
      rr_r[677] *= scale_r;
      rr_r[706] *= scale_r;
      rr_r[709] *= scale_r;
      rr_r[728] *= scale_r;
      rr_r[739] *= scale_r;
      rr_r[777] *= scale_r;
      rr_r[778] *= scale_r;
      rr_r[779] *= scale_r;
      rr_r[780] *= scale_r;
      rr_r[781] *= scale_r;
      rr_r[782] *= scale_r;
      rr_r[783] *= scale_r;
      rr_r[784] *= scale_r;
      rr_r[797] *= scale_r;
      rr_r[798] *= scale_r;
      rr_r[799] *= scale_r;
      rr_r[854] *= scale_r;
      rr_r[855] *= scale_r;
      rr_r[856] *= scale_r;
      rr_r[857] *= scale_r;
      rr_r[858] *= scale_r;
    }
  }
  // Stiff species HO2
  {
    double ddot = rr_f[9] + rr_f[10] + rr_f[11] + rr_f[12] + rr_f[24] + rr_f[32] 
      + rr_f[33] + rr_f[42] + rr_f[53] + rr_f[61] + rr_f[66] + rr_f[73] + 
      rr_f[82] + rr_f[90] + rr_f[97] + rr_f[98] + rr_f[107] + rr_f[134] + 
      rr_f[146] + rr_f[155] + rr_f[171] + rr_f[225] + rr_f[231] + rr_f[239] + 
      rr_f[250] + rr_f[534] + rr_f[281] + rr_f[282] + rr_f[283] + rr_f[299] + 
      rr_f[327] + rr_f[338] + rr_f[352] + rr_f[353] + rr_f[382] + rr_f[392] + 
      rr_f[416] + rr_f[421] + rr_f[433] + rr_f[443] + rr_f[459] + rr_f[473] + 
      rr_f[479] + rr_f[485] + rr_f[491] + rr_f[501] + rr_f[503] + rr_f[519] + 
      rr_f[520] + rr_f[557] + rr_f[558] + rr_f[559] + rr_f[560] + rr_f[613] + 
      rr_f[619] + rr_f[622] + rr_f[651] + rr_f[652] + rr_f[653] + rr_f[673] + 
      rr_f[674] + rr_f[675] + rr_f[676] + rr_f[698] + rr_f[706] + rr_f[709] + 
      rr_f[719] + rr_f[720] + rr_f[724] + rr_f[726] + rr_f[742] + rr_f[766] + 
      rr_f[767] + rr_f[772] + rr_f[773] + rr_f[806] + rr_f[807] + rr_f[808] + 
      rr_r[8] + rr_r[13] + rr_r[13] + rr_r[14] + rr_r[14] + rr_r[17] + rr_r[18] 
      + rr_r[19] + rr_r[20] + rr_r[26] + rr_r[63] + rr_r[56] + rr_r[64] + 
      rr_r[81] + rr_r[266] + rr_r[132] + rr_r[148] + rr_r[158] + rr_r[159] + 
      rr_r[162] + rr_r[169] + rr_r[201] + rr_r[207] + rr_r[224] + rr_r[238] + 
      rr_r[255] + rr_r[260] + rr_r[288] + rr_r[289] + rr_r[290] + rr_r[307] + 
      rr_r[319] + rr_r[326] + rr_r[337] + rr_r[366] + rr_r[367] + rr_r[368] + 
      rr_r[373] + rr_r[374] + rr_r[378] + rr_r[395] + rr_r[410] + rr_r[412] + 
      rr_r[426] + rr_r[427] + rr_r[446] + rr_r[447] + rr_r[460] + rr_r[478] + 
      rr_r[488] + rr_r[499] + rr_r[517] + rr_r[518] + rr_r[530] + rr_r[565] + 
      rr_r[566] + rr_r[567] + rr_r[568] + rr_r[600] + rr_r[601] + rr_r[602] + 
      rr_r[603] + rr_r[604] + rr_r[605] + rr_r[638] + rr_r[639] + rr_r[640] + 
      rr_r[641] + rr_r[642] + rr_r[643] + rr_r[689] + rr_r[690] + rr_r[715] + 
      rr_r[716] + rr_r[729] + rr_r[744] + rr_r[750] + rr_r[751] + rr_r[757] + 
      rr_r[758] + rr_r[759] + rr_r[760] + rr_r[761] + rr_r[812] + rr_r[813] + 
      rr_r[814] + rr_r[828] + rr_r[841]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[6])
    {
      double cdot = rr_f[8] + rr_f[13] + rr_f[13] + rr_f[14] + rr_f[14] + 
        rr_f[17] + rr_f[18] + rr_f[19] + rr_f[20] + rr_f[26] + rr_f[63] + 
        rr_f[56] + rr_f[64] + rr_f[81] + rr_f[266] + rr_f[132] + rr_f[148] + 
        rr_f[158] + rr_f[159] + rr_f[162] + rr_f[169] + rr_f[201] + rr_f[207] + 
        rr_f[224] + rr_f[238] + rr_f[255] + rr_f[260] + rr_f[288] + rr_f[289] + 
        rr_f[290] + rr_f[307] + rr_f[319] + rr_f[326] + rr_f[337] + rr_f[366] + 
        rr_f[367] + rr_f[368] + rr_f[373] + rr_f[374] + rr_f[378] + rr_f[395] + 
        rr_f[410] + rr_f[412] + rr_f[426] + rr_f[427] + rr_f[446] + rr_f[447] + 
        rr_f[460] + rr_f[478] + rr_f[488] + rr_f[499] + rr_f[517] + rr_f[518] + 
        rr_f[530] + rr_f[565] + rr_f[566] + rr_f[567] + rr_f[568] + rr_f[600] + 
        rr_f[601] + rr_f[602] + rr_f[603] + rr_f[604] + rr_f[605] + rr_f[638] + 
        rr_f[639] + rr_f[640] + rr_f[641] + rr_f[642] + rr_f[643] + rr_f[689] + 
        rr_f[690] + rr_f[715] + rr_f[716] + rr_f[729] + rr_f[744] + rr_f[750] + 
        rr_f[751] + rr_f[757] + rr_f[758] + rr_f[759] + rr_f[760] + rr_f[761] + 
        rr_f[812] + rr_f[813] + rr_f[814] + rr_f[828] + rr_f[841] + rr_r[9] + 
        rr_r[10] + rr_r[11] + rr_r[12] + rr_r[24] + rr_r[32] + rr_r[42] + 
        rr_r[61] + rr_r[66] + rr_r[73] + rr_r[82] + rr_r[90] + rr_r[97] + 
        rr_r[98] + rr_r[107] + rr_r[134] + rr_r[146] + rr_r[155] + rr_r[171] + 
        rr_r[225] + rr_r[239] + rr_r[250] + rr_r[534] + rr_r[281] + rr_r[282] + 
        rr_r[283] + rr_r[299] + rr_r[327] + rr_r[338] + rr_r[352] + rr_r[353] + 
        rr_r[382] + rr_r[392] + rr_r[416] + rr_r[421] + rr_r[443] + rr_r[459] + 
        rr_r[473] + rr_r[479] + rr_r[485] + rr_r[491] + rr_r[501] + rr_r[519] + 
        rr_r[520] + rr_r[557] + rr_r[558] + rr_r[559] + rr_r[560] + rr_r[613] + 
        rr_r[619] + rr_r[622] + rr_r[651] + rr_r[652] + rr_r[653] + rr_r[698] + 
        rr_r[706] + rr_r[709] + rr_r[719] + rr_r[720] + rr_r[726] + rr_r[742] + 
        rr_r[772] + rr_r[773] + rr_r[806] + rr_r[807] + rr_r[808]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[6] * 0.03029681486555637;
      double c0 = mole_frac[6] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[6]- c0) / dt) 
        * recip_ddot; 
      rr_f[9] *= scale_r;
      rr_f[10] *= scale_r;
      rr_f[11] *= scale_r;
      rr_f[12] *= scale_r;
      rr_f[24] *= scale_r;
      rr_f[32] *= scale_r;
      rr_f[33] *= scale_r;
      rr_f[42] *= scale_r;
      rr_f[53] *= scale_r;
      rr_f[61] *= scale_r;
      rr_f[66] *= scale_r;
      rr_f[73] *= scale_r;
      rr_f[82] *= scale_r;
      rr_f[90] *= scale_r;
      rr_f[97] *= scale_r;
      rr_f[98] *= scale_r;
      rr_f[107] *= scale_r;
      rr_f[134] *= scale_r;
      rr_f[146] *= scale_r;
      rr_f[155] *= scale_r;
      rr_f[171] *= scale_r;
      rr_f[225] *= scale_r;
      rr_f[231] *= scale_r;
      rr_f[239] *= scale_r;
      rr_f[250] *= scale_r;
      rr_f[534] *= scale_r;
      rr_f[281] *= scale_r;
      rr_f[282] *= scale_r;
      rr_f[283] *= scale_r;
      rr_f[299] *= scale_r;
      rr_f[327] *= scale_r;
      rr_f[338] *= scale_r;
      rr_f[352] *= scale_r;
      rr_f[353] *= scale_r;
      rr_f[382] *= scale_r;
      rr_f[392] *= scale_r;
      rr_f[416] *= scale_r;
      rr_f[421] *= scale_r;
      rr_f[433] *= scale_r;
      rr_f[443] *= scale_r;
      rr_f[459] *= scale_r;
      rr_f[473] *= scale_r;
      rr_f[479] *= scale_r;
      rr_f[485] *= scale_r;
      rr_f[491] *= scale_r;
      rr_f[501] *= scale_r;
      rr_f[503] *= scale_r;
      rr_f[519] *= scale_r;
      rr_f[520] *= scale_r;
      rr_f[557] *= scale_r;
      rr_f[558] *= scale_r;
      rr_f[559] *= scale_r;
      rr_f[560] *= scale_r;
      rr_f[613] *= scale_r;
      rr_f[619] *= scale_r;
      rr_f[622] *= scale_r;
      rr_f[651] *= scale_r;
      rr_f[652] *= scale_r;
      rr_f[653] *= scale_r;
      rr_f[673] *= scale_r;
      rr_f[674] *= scale_r;
      rr_f[675] *= scale_r;
      rr_f[676] *= scale_r;
      rr_f[698] *= scale_r;
      rr_f[706] *= scale_r;
      rr_f[709] *= scale_r;
      rr_f[719] *= scale_r;
      rr_f[720] *= scale_r;
      rr_f[724] *= scale_r;
      rr_f[726] *= scale_r;
      rr_f[742] *= scale_r;
      rr_f[766] *= scale_r;
      rr_f[767] *= scale_r;
      rr_f[772] *= scale_r;
      rr_f[773] *= scale_r;
      rr_f[806] *= scale_r;
      rr_f[807] *= scale_r;
      rr_f[808] *= scale_r;
      rr_r[8] *= scale_r;
      rr_r[13] *= scale_r;
      rr_r[13] *= scale_r;
      rr_r[14] *= scale_r;
      rr_r[14] *= scale_r;
      rr_r[17] *= scale_r;
      rr_r[18] *= scale_r;
      rr_r[19] *= scale_r;
      rr_r[20] *= scale_r;
      rr_r[26] *= scale_r;
      rr_r[63] *= scale_r;
      rr_r[56] *= scale_r;
      rr_r[64] *= scale_r;
      rr_r[81] *= scale_r;
      rr_r[266] *= scale_r;
      rr_r[132] *= scale_r;
      rr_r[148] *= scale_r;
      rr_r[158] *= scale_r;
      rr_r[159] *= scale_r;
      rr_r[162] *= scale_r;
      rr_r[169] *= scale_r;
      rr_r[201] *= scale_r;
      rr_r[207] *= scale_r;
      rr_r[224] *= scale_r;
      rr_r[238] *= scale_r;
      rr_r[255] *= scale_r;
      rr_r[260] *= scale_r;
      rr_r[288] *= scale_r;
      rr_r[289] *= scale_r;
      rr_r[290] *= scale_r;
      rr_r[307] *= scale_r;
      rr_r[319] *= scale_r;
      rr_r[326] *= scale_r;
      rr_r[337] *= scale_r;
      rr_r[366] *= scale_r;
      rr_r[367] *= scale_r;
      rr_r[368] *= scale_r;
      rr_r[373] *= scale_r;
      rr_r[374] *= scale_r;
      rr_r[378] *= scale_r;
      rr_r[395] *= scale_r;
      rr_r[410] *= scale_r;
      rr_r[412] *= scale_r;
      rr_r[426] *= scale_r;
      rr_r[427] *= scale_r;
      rr_r[446] *= scale_r;
      rr_r[447] *= scale_r;
      rr_r[460] *= scale_r;
      rr_r[478] *= scale_r;
      rr_r[488] *= scale_r;
      rr_r[499] *= scale_r;
      rr_r[517] *= scale_r;
      rr_r[518] *= scale_r;
      rr_r[530] *= scale_r;
      rr_r[565] *= scale_r;
      rr_r[566] *= scale_r;
      rr_r[567] *= scale_r;
      rr_r[568] *= scale_r;
      rr_r[600] *= scale_r;
      rr_r[601] *= scale_r;
      rr_r[602] *= scale_r;
      rr_r[603] *= scale_r;
      rr_r[604] *= scale_r;
      rr_r[605] *= scale_r;
      rr_r[638] *= scale_r;
      rr_r[639] *= scale_r;
      rr_r[640] *= scale_r;
      rr_r[641] *= scale_r;
      rr_r[642] *= scale_r;
      rr_r[643] *= scale_r;
      rr_r[689] *= scale_r;
      rr_r[690] *= scale_r;
      rr_r[715] *= scale_r;
      rr_r[716] *= scale_r;
      rr_r[729] *= scale_r;
      rr_r[744] *= scale_r;
      rr_r[750] *= scale_r;
      rr_r[751] *= scale_r;
      rr_r[757] *= scale_r;
      rr_r[758] *= scale_r;
      rr_r[759] *= scale_r;
      rr_r[760] *= scale_r;
      rr_r[761] *= scale_r;
      rr_r[812] *= scale_r;
      rr_r[813] *= scale_r;
      rr_r[814] *= scale_r;
      rr_r[828] *= scale_r;
      rr_r[841] *= scale_r;
    }
  }
  // Stiff species H2O2
  {
    double ddot = rr_f[13] + rr_f[14] + rr_f[15] + rr_f[16] + rr_f[17] + 
      rr_f[18] + rr_f[19] + rr_f[20] + rr_r[42] + rr_r[61] + rr_r[66] + rr_r[82] 
      + rr_r[90] + rr_r[134] + rr_r[171] + rr_r[225] + rr_r[239] + rr_r[250] + 
      rr_r[534] + rr_r[281] + rr_r[282] + rr_r[283] + rr_r[353] + rr_r[382] + 
      rr_r[416] + rr_r[443] + rr_r[459] + rr_r[479] + rr_r[485] + rr_r[519] + 
      rr_r[520] + rr_r[557] + rr_r[558] + rr_r[559] + rr_r[560] + rr_r[613] + 
      rr_r[619] + rr_r[698] + rr_r[719] + rr_r[720] + rr_r[726] + rr_r[742] + 
      rr_r[806] + rr_r[807] + rr_r[808]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[7])
    {
      double cdot = rr_f[42] + rr_f[53] + rr_f[61] + rr_f[66] + rr_f[82] + 
        rr_f[90] + rr_f[134] + rr_f[171] + rr_f[225] + rr_f[239] + rr_f[250] + 
        rr_f[534] + rr_f[281] + rr_f[282] + rr_f[283] + rr_f[353] + rr_f[382] + 
        rr_f[416] + rr_f[443] + rr_f[459] + rr_f[479] + rr_f[485] + rr_f[519] + 
        rr_f[520] + rr_f[557] + rr_f[558] + rr_f[559] + rr_f[560] + rr_f[613] + 
        rr_f[619] + rr_f[673] + rr_f[674] + rr_f[675] + rr_f[676] + rr_f[698] + 
        rr_f[719] + rr_f[720] + rr_f[726] + rr_f[742] + rr_f[766] + rr_f[767] + 
        rr_f[806] + rr_f[807] + rr_f[808] + rr_r[13] + rr_r[14] + rr_r[15] + 
        rr_r[16] + rr_r[17] + rr_r[18] + rr_r[19] + rr_r[20]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[7] * 0.02939901936631002;
      double c0 = mole_frac[7] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[7]- c0) / dt) 
        * recip_ddot; 
      rr_f[13] *= scale_r;
      rr_f[14] *= scale_r;
      rr_f[15] *= scale_r;
      rr_f[16] *= scale_r;
      rr_f[17] *= scale_r;
      rr_f[18] *= scale_r;
      rr_f[19] *= scale_r;
      rr_f[20] *= scale_r;
      rr_r[42] *= scale_r;
      rr_r[61] *= scale_r;
      rr_r[66] *= scale_r;
      rr_r[82] *= scale_r;
      rr_r[90] *= scale_r;
      rr_r[134] *= scale_r;
      rr_r[171] *= scale_r;
      rr_r[225] *= scale_r;
      rr_r[239] *= scale_r;
      rr_r[250] *= scale_r;
      rr_r[534] *= scale_r;
      rr_r[281] *= scale_r;
      rr_r[282] *= scale_r;
      rr_r[283] *= scale_r;
      rr_r[353] *= scale_r;
      rr_r[382] *= scale_r;
      rr_r[416] *= scale_r;
      rr_r[443] *= scale_r;
      rr_r[459] *= scale_r;
      rr_r[479] *= scale_r;
      rr_r[485] *= scale_r;
      rr_r[519] *= scale_r;
      rr_r[520] *= scale_r;
      rr_r[557] *= scale_r;
      rr_r[558] *= scale_r;
      rr_r[559] *= scale_r;
      rr_r[560] *= scale_r;
      rr_r[613] *= scale_r;
      rr_r[619] *= scale_r;
      rr_r[698] *= scale_r;
      rr_r[719] *= scale_r;
      rr_r[720] *= scale_r;
      rr_r[726] *= scale_r;
      rr_r[742] *= scale_r;
      rr_r[806] *= scale_r;
      rr_r[807] *= scale_r;
      rr_r[808] *= scale_r;
    }
  }
  // Stiff species CH2O
  {
    double ddot = rr_f[34] + rr_f[38] + rr_f[39] + rr_f[40] + rr_f[41] + 
      rr_f[42] + rr_f[57] + rr_f[62] + rr_f[69] + rr_f[72] + rr_f[85] + 
      rr_f[103] + rr_f[152] + rr_f[230] + rr_f[496] + rr_f[769] + rr_r[63] + 
      rr_r[32] + rr_r[36] + rr_r[37] + rr_r[43] + rr_r[55] + rr_r[56] + rr_r[59] 
      + rr_r[60] + rr_r[61] + rr_r[64] + rr_r[65] + rr_r[66] + rr_r[67] + 
      rr_r[67] + rr_r[68] + rr_r[70] + rr_r[71] + rr_r[92] + rr_r[99] + 
      rr_r[101] + rr_r[120] + rr_r[121] + rr_r[123] + rr_r[149] + rr_r[208] + 
      rr_r[233] + rr_r[308] + rr_r[321] + rr_r[331] + rr_r[365] + rr_r[390] + 
      rr_r[404] + rr_r[407] + rr_r[468] + rr_r[474] + rr_r[678] + rr_r[682]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[10])
    {
      double cdot = rr_f[63] + rr_f[32] + rr_f[36] + rr_f[37] + rr_f[43] + 
        rr_f[55] + rr_f[56] + rr_f[59] + rr_f[60] + rr_f[61] + rr_f[64] + 
        rr_f[65] + rr_f[66] + rr_f[67] + rr_f[67] + rr_f[68] + rr_f[70] + 
        rr_f[71] + rr_f[92] + rr_f[99] + rr_f[101] + rr_f[108] + rr_f[120] + 
        rr_f[121] + rr_f[123] + rr_f[149] + rr_f[181] + rr_f[208] + rr_f[233] + 
        rr_f[308] + rr_f[310] + rr_f[321] + rr_f[331] + rr_f[365] + rr_f[390] + 
        rr_f[404] + rr_f[407] + rr_f[450] + rr_f[451] + rr_f[468] + rr_f[469] + 
        rr_f[474] + rr_f[503] + rr_f[504] + rr_f[626] + rr_f[678] + rr_f[682] + 
        rr_f[753] + rr_f[762] + rr_f[846] + rr_r[34] + rr_r[38] + rr_r[39] + 
        rr_r[40] + rr_r[41] + rr_r[42] + rr_r[57] + rr_r[62] + rr_r[69] + 
        rr_r[72] + rr_r[85] + rr_r[103] + rr_r[152] + rr_r[230] + rr_r[496] + 
        rr_r[769]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[10] * 0.03330392596670473;
      double c0 = mole_frac[10] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[10]- c0) / dt) 
        * recip_ddot; 
      rr_f[34] *= scale_r;
      rr_f[38] *= scale_r;
      rr_f[39] *= scale_r;
      rr_f[40] *= scale_r;
      rr_f[41] *= scale_r;
      rr_f[42] *= scale_r;
      rr_f[57] *= scale_r;
      rr_f[62] *= scale_r;
      rr_f[69] *= scale_r;
      rr_f[72] *= scale_r;
      rr_f[85] *= scale_r;
      rr_f[103] *= scale_r;
      rr_f[152] *= scale_r;
      rr_f[230] *= scale_r;
      rr_f[496] *= scale_r;
      rr_f[769] *= scale_r;
      rr_r[63] *= scale_r;
      rr_r[32] *= scale_r;
      rr_r[36] *= scale_r;
      rr_r[37] *= scale_r;
      rr_r[43] *= scale_r;
      rr_r[55] *= scale_r;
      rr_r[56] *= scale_r;
      rr_r[59] *= scale_r;
      rr_r[60] *= scale_r;
      rr_r[61] *= scale_r;
      rr_r[64] *= scale_r;
      rr_r[65] *= scale_r;
      rr_r[66] *= scale_r;
      rr_r[67] *= scale_r;
      rr_r[67] *= scale_r;
      rr_r[68] *= scale_r;
      rr_r[70] *= scale_r;
      rr_r[71] *= scale_r;
      rr_r[92] *= scale_r;
      rr_r[99] *= scale_r;
      rr_r[101] *= scale_r;
      rr_r[120] *= scale_r;
      rr_r[121] *= scale_r;
      rr_r[123] *= scale_r;
      rr_r[149] *= scale_r;
      rr_r[208] *= scale_r;
      rr_r[233] *= scale_r;
      rr_r[308] *= scale_r;
      rr_r[321] *= scale_r;
      rr_r[331] *= scale_r;
      rr_r[365] *= scale_r;
      rr_r[390] *= scale_r;
      rr_r[404] *= scale_r;
      rr_r[407] *= scale_r;
      rr_r[468] *= scale_r;
      rr_r[474] *= scale_r;
      rr_r[678] *= scale_r;
      rr_r[682] *= scale_r;
    }
  }
  // Stiff species HCO
  {
    double ddot = rr_f[25] + rr_f[26] + rr_f[27] + rr_f[28] + rr_f[29] + 
      rr_f[30] + rr_f[31] + rr_f[32] + rr_f[33] + rr_f[35] + rr_f[35] + rr_f[36] 
      + rr_f[67] + rr_r[34] + rr_r[34] + rr_r[38] + rr_r[39] + rr_r[40] + 
      rr_r[41] + rr_r[42] + rr_r[47] + rr_r[57] + rr_r[69] + rr_r[103] + 
      rr_r[152] + rr_r[165] + rr_r[197] + rr_r[208] + rr_r[230] + rr_r[234] + 
      rr_r[245] + rr_r[272] + rr_r[313] + rr_r[341] + rr_r[350] + rr_r[351] + 
      rr_r[357] + rr_r[442] + rr_r[465] + rr_r[492] + rr_r[496] + rr_r[769]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[11])
    {
      double cdot = rr_f[34] + rr_f[34] + rr_f[38] + rr_f[39] + rr_f[40] + 
        rr_f[41] + rr_f[42] + rr_f[47] + rr_f[57] + rr_f[69] + rr_f[103] + 
        rr_f[152] + rr_f[165] + rr_f[197] + rr_f[208] + rr_f[230] + rr_f[234] + 
        rr_f[245] + rr_f[272] + rr_f[313] + rr_f[341] + rr_f[350] + rr_f[351] + 
        rr_f[357] + rr_f[442] + rr_f[465] + rr_f[492] + rr_f[496] + rr_f[683] + 
        rr_f[764] + rr_f[766] + rr_f[769] + rr_r[25] + rr_r[26] + rr_r[27] + 
        rr_r[28] + rr_r[29] + rr_r[30] + rr_r[31] + rr_r[32] + rr_r[36] + 
        rr_r[67]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[11] * 0.03446075127194632;
      double c0 = mole_frac[11] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[11]- c0) / dt) 
        * recip_ddot; 
      rr_f[25] *= scale_r;
      rr_f[26] *= scale_r;
      rr_f[27] *= scale_r;
      rr_f[28] *= scale_r;
      rr_f[29] *= scale_r;
      rr_f[30] *= scale_r;
      rr_f[31] *= scale_r;
      rr_f[32] *= scale_r;
      rr_f[33] *= scale_r;
      rr_f[35] *= scale_r;
      rr_f[35] *= scale_r;
      rr_f[36] *= scale_r;
      rr_f[67] *= scale_r;
      rr_r[34] *= scale_r;
      rr_r[34] *= scale_r;
      rr_r[38] *= scale_r;
      rr_r[39] *= scale_r;
      rr_r[40] *= scale_r;
      rr_r[41] *= scale_r;
      rr_r[42] *= scale_r;
      rr_r[47] *= scale_r;
      rr_r[57] *= scale_r;
      rr_r[69] *= scale_r;
      rr_r[103] *= scale_r;
      rr_r[152] *= scale_r;
      rr_r[165] *= scale_r;
      rr_r[197] *= scale_r;
      rr_r[208] *= scale_r;
      rr_r[230] *= scale_r;
      rr_r[234] *= scale_r;
      rr_r[245] *= scale_r;
      rr_r[272] *= scale_r;
      rr_r[313] *= scale_r;
      rr_r[341] *= scale_r;
      rr_r[350] *= scale_r;
      rr_r[351] *= scale_r;
      rr_r[357] *= scale_r;
      rr_r[442] *= scale_r;
      rr_r[465] *= scale_r;
      rr_r[492] *= scale_r;
      rr_r[496] *= scale_r;
      rr_r[769] *= scale_r;
    }
  }
  // Stiff species HOCHO
  {
    double ddot = rr_f[45] + rr_f[46] + rr_f[47] + rr_f[48] + rr_f[49] + 
      rr_f[50] + rr_f[51] + rr_f[52] + rr_f[53] + rr_f[54] + rr_r[44] + 
      rr_r[173]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[12])
    {
      double cdot = rr_f[44] + rr_f[173] + rr_r[45] + rr_r[46] + rr_r[47];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[12] * 0.02172690196756651;
      double c0 = mole_frac[12] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[12]- c0) / dt) 
        * recip_ddot; 
      rr_f[45] *= scale_r;
      rr_f[46] *= scale_r;
      rr_f[47] *= scale_r;
      rr_f[48] *= scale_r;
      rr_f[49] *= scale_r;
      rr_f[50] *= scale_r;
      rr_f[51] *= scale_r;
      rr_f[52] *= scale_r;
      rr_f[53] *= scale_r;
      rr_f[54] *= scale_r;
      rr_r[44] *= scale_r;
      rr_r[173] *= scale_r;
    }
  }
  // Stiff species CH3OH
  {
    double ddot = rr_f[72] + rr_f[74] + rr_f[75] + rr_f[76] + rr_f[77] + 
      rr_f[78] + rr_f[79] + rr_f[80] + rr_f[81] + rr_f[82] + rr_f[83] + rr_f[84] 
      + rr_f[85] + rr_f[105] + rr_f[154] + rr_r[57] + rr_r[58] + rr_r[68] + 
      rr_r[69] + rr_r[84] + rr_r[112] + rr_r[136] + rr_r[202] + rr_r[223] + 
      rr_r[242] + rr_r[251] + rr_r[536] + rr_r[384] + rr_r[418] + rr_r[523] + 
      rr_r[524] + rr_r[573] + rr_r[574] + rr_r[575] + rr_r[576] + rr_r[615] + 
      rr_r[621] + rr_r[700] + rr_r[743] + rr_r[809] + rr_r[810] + rr_r[811]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[13])
    {
      double cdot = rr_f[57] + rr_f[58] + rr_f[68] + rr_f[69] + rr_f[84] + 
        rr_f[108] + rr_f[112] + rr_f[136] + rr_f[202] + rr_f[223] + rr_f[242] + 
        rr_f[251] + rr_f[536] + rr_f[384] + rr_f[418] + rr_f[523] + rr_f[524] + 
        rr_f[573] + rr_f[574] + rr_f[575] + rr_f[576] + rr_f[615] + rr_f[621] + 
        rr_f[700] + rr_f[743] + rr_f[809] + rr_f[810] + rr_f[811] + rr_r[72] + 
        rr_r[74] + rr_r[75] + rr_r[76] + rr_r[77] + rr_r[78] + rr_r[79] + 
        rr_r[80] + rr_r[81] + rr_r[82] + rr_r[83] + rr_r[84] + rr_r[85] + 
        rr_r[105] + rr_r[154]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[13] * 0.03120861932131863;
      double c0 = mole_frac[13] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[13]- c0) / dt) 
        * recip_ddot; 
      rr_f[72] *= scale_r;
      rr_f[74] *= scale_r;
      rr_f[75] *= scale_r;
      rr_f[76] *= scale_r;
      rr_f[77] *= scale_r;
      rr_f[78] *= scale_r;
      rr_f[79] *= scale_r;
      rr_f[80] *= scale_r;
      rr_f[81] *= scale_r;
      rr_f[82] *= scale_r;
      rr_f[83] *= scale_r;
      rr_f[84] *= scale_r;
      rr_f[85] *= scale_r;
      rr_f[105] *= scale_r;
      rr_f[154] *= scale_r;
      rr_r[57] *= scale_r;
      rr_r[58] *= scale_r;
      rr_r[68] *= scale_r;
      rr_r[69] *= scale_r;
      rr_r[84] *= scale_r;
      rr_r[112] *= scale_r;
      rr_r[136] *= scale_r;
      rr_r[202] *= scale_r;
      rr_r[223] *= scale_r;
      rr_r[242] *= scale_r;
      rr_r[251] *= scale_r;
      rr_r[536] *= scale_r;
      rr_r[384] *= scale_r;
      rr_r[418] *= scale_r;
      rr_r[523] *= scale_r;
      rr_r[524] *= scale_r;
      rr_r[573] *= scale_r;
      rr_r[574] *= scale_r;
      rr_r[575] *= scale_r;
      rr_r[576] *= scale_r;
      rr_r[615] *= scale_r;
      rr_r[621] *= scale_r;
      rr_r[700] *= scale_r;
      rr_r[743] *= scale_r;
      rr_r[809] *= scale_r;
      rr_r[810] *= scale_r;
      rr_r[811] *= scale_r;
    }
  }
  // Stiff species CH3O2H
  {
    double ddot = rr_f[113] + rr_r[103] + rr_r[104] + rr_r[105] + rr_r[107] + 
      rr_r[135] + rr_r[139] + rr_r[172] + rr_r[203] + rr_r[226] + rr_r[243] + 
      rr_r[252] + rr_r[537] + rr_r[295] + rr_r[383] + rr_r[419] + rr_r[466] + 
      rr_r[521] + rr_r[522] + rr_r[581] + rr_r[582] + rr_r[583] + rr_r[584] + 
      rr_r[614] + rr_r[620] + rr_r[699] + rr_r[721] + rr_r[722] + rr_r[754] + 
      rr_r[829] + rr_r[830] + rr_r[831]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[14])
    {
      double cdot = rr_f[103] + rr_f[104] + rr_f[105] + rr_f[107] + rr_f[135] + 
        rr_f[139] + rr_f[172] + rr_f[203] + rr_f[226] + rr_f[243] + rr_f[252] + 
        rr_f[537] + rr_f[295] + rr_f[383] + rr_f[419] + rr_f[466] + rr_f[521] + 
        rr_f[522] + rr_f[581] + rr_f[582] + rr_f[583] + rr_f[584] + rr_f[614] + 
        rr_f[620] + rr_f[699] + rr_f[721] + rr_f[722] + rr_f[754] + rr_f[829] + 
        rr_f[830] + rr_f[831] + rr_r[113]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[14] * 0.02081519375927187;
      double c0 = mole_frac[14] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[14]- c0) / dt) 
        * recip_ddot; 
      rr_f[113] *= scale_r;
      rr_r[103] *= scale_r;
      rr_r[104] *= scale_r;
      rr_r[105] *= scale_r;
      rr_r[107] *= scale_r;
      rr_r[135] *= scale_r;
      rr_r[139] *= scale_r;
      rr_r[172] *= scale_r;
      rr_r[203] *= scale_r;
      rr_r[226] *= scale_r;
      rr_r[243] *= scale_r;
      rr_r[252] *= scale_r;
      rr_r[537] *= scale_r;
      rr_r[295] *= scale_r;
      rr_r[383] *= scale_r;
      rr_r[419] *= scale_r;
      rr_r[466] *= scale_r;
      rr_r[521] *= scale_r;
      rr_r[522] *= scale_r;
      rr_r[581] *= scale_r;
      rr_r[582] *= scale_r;
      rr_r[583] *= scale_r;
      rr_r[584] *= scale_r;
      rr_r[614] *= scale_r;
      rr_r[620] *= scale_r;
      rr_r[699] *= scale_r;
      rr_r[721] *= scale_r;
      rr_r[722] *= scale_r;
      rr_r[754] *= scale_r;
      rr_r[829] *= scale_r;
      rr_r[830] *= scale_r;
      rr_r[831] *= scale_r;
    }
  }
  // Stiff species CH3O2
  {
    double ddot = rr_f[103] + rr_f[104] + rr_f[105] + rr_f[106] + rr_f[107] + 
      rr_f[108] + rr_f[108] + rr_f[109] + rr_f[109] + rr_f[110] + rr_f[111] + 
      rr_f[112] + rr_f[135] + rr_f[139] + rr_f[147] + rr_f[172] + rr_f[203] + 
      rr_f[226] + rr_f[243] + rr_f[252] + rr_f[537] + rr_f[295] + rr_f[300] + 
      rr_f[383] + rr_f[393] + rr_f[419] + rr_f[422] + rr_f[430] + rr_f[466] + 
      rr_f[472] + rr_f[521] + rr_f[522] + rr_f[581] + rr_f[582] + rr_f[583] + 
      rr_f[584] + rr_f[614] + rr_f[620] + rr_f[623] + rr_f[699] + rr_f[707] + 
      rr_f[710] + rr_f[721] + rr_f[722] + rr_f[754] + rr_f[829] + rr_f[830] + 
      rr_f[831] + rr_r[102]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[15])
    {
      double cdot = rr_f[102] + rr_r[103] + rr_r[104] + rr_r[105] + rr_r[106] + 
        rr_r[107] + rr_r[110] + rr_r[111] + rr_r[112] + rr_r[135] + rr_r[139] + 
        rr_r[147] + rr_r[172] + rr_r[203] + rr_r[226] + rr_r[243] + rr_r[252] + 
        rr_r[537] + rr_r[295] + rr_r[300] + rr_r[383] + rr_r[393] + rr_r[419] + 
        rr_r[422] + rr_r[466] + rr_r[472] + rr_r[521] + rr_r[522] + rr_r[581] + 
        rr_r[582] + rr_r[583] + rr_r[584] + rr_r[614] + rr_r[620] + rr_r[623] + 
        rr_r[699] + rr_r[707] + rr_r[710] + rr_r[721] + rr_r[722] + rr_r[754] + 
        rr_r[829] + rr_r[830] + rr_r[831]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[15] * 0.021261278576753;
      double c0 = mole_frac[15] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[15]- c0) / dt) 
        * recip_ddot; 
      rr_f[103] *= scale_r;
      rr_f[104] *= scale_r;
      rr_f[105] *= scale_r;
      rr_f[106] *= scale_r;
      rr_f[107] *= scale_r;
      rr_f[108] *= scale_r;
      rr_f[108] *= scale_r;
      rr_f[109] *= scale_r;
      rr_f[109] *= scale_r;
      rr_f[110] *= scale_r;
      rr_f[111] *= scale_r;
      rr_f[112] *= scale_r;
      rr_f[135] *= scale_r;
      rr_f[139] *= scale_r;
      rr_f[147] *= scale_r;
      rr_f[172] *= scale_r;
      rr_f[203] *= scale_r;
      rr_f[226] *= scale_r;
      rr_f[243] *= scale_r;
      rr_f[252] *= scale_r;
      rr_f[537] *= scale_r;
      rr_f[295] *= scale_r;
      rr_f[300] *= scale_r;
      rr_f[383] *= scale_r;
      rr_f[393] *= scale_r;
      rr_f[419] *= scale_r;
      rr_f[422] *= scale_r;
      rr_f[430] *= scale_r;
      rr_f[466] *= scale_r;
      rr_f[472] *= scale_r;
      rr_f[521] *= scale_r;
      rr_f[522] *= scale_r;
      rr_f[581] *= scale_r;
      rr_f[582] *= scale_r;
      rr_f[583] *= scale_r;
      rr_f[584] *= scale_r;
      rr_f[614] *= scale_r;
      rr_f[620] *= scale_r;
      rr_f[623] *= scale_r;
      rr_f[699] *= scale_r;
      rr_f[707] *= scale_r;
      rr_f[710] *= scale_r;
      rr_f[721] *= scale_r;
      rr_f[722] *= scale_r;
      rr_f[754] *= scale_r;
      rr_f[829] *= scale_r;
      rr_f[830] *= scale_r;
      rr_f[831] *= scale_r;
      rr_r[102] *= scale_r;
    }
  }
  // Stiff species CH4
  {
    double ddot = rr_f[58] + rr_f[87] + rr_f[88] + rr_f[89] + rr_f[90] + 
      rr_f[91] + rr_f[104] + rr_f[115] + rr_f[153] + rr_r[31] + rr_r[41] + 
      rr_r[59] + rr_r[83] + rr_r[86] + rr_r[98] + rr_r[133] + rr_r[142] + 
      rr_r[170] + rr_r[178] + rr_r[200] + rr_r[210] + rr_r[222] + rr_r[240] + 
      rr_r[249] + rr_r[535] + rr_r[291] + rr_r[292] + rr_r[293] + rr_r[302] + 
      rr_r[315] + rr_r[323] + rr_r[333] + rr_r[346] + rr_r[381] + rr_r[417] + 
      rr_r[458] + rr_r[480] + rr_r[486] + rr_r[502] + rr_r[515] + rr_r[516] + 
      rr_r[561] + rr_r[562] + rr_r[563] + rr_r[564] + rr_r[612] + rr_r[618] + 
      rr_r[697] + rr_r[727] + rr_r[741] + rr_r[803] + rr_r[804] + rr_r[805]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[16])
    {
      double cdot = rr_f[31] + rr_f[41] + rr_f[52] + rr_f[59] + rr_f[83] + 
        rr_f[86] + rr_f[98] + rr_f[133] + rr_f[142] + rr_f[170] + rr_f[178] + 
        rr_f[200] + rr_f[210] + rr_f[222] + rr_f[240] + rr_f[249] + rr_f[535] + 
        rr_f[291] + rr_f[292] + rr_f[293] + rr_f[302] + rr_f[315] + rr_f[323] + 
        rr_f[333] + rr_f[346] + rr_f[381] + rr_f[417] + rr_f[458] + rr_f[480] + 
        rr_f[486] + rr_f[502] + rr_f[515] + rr_f[516] + rr_f[561] + rr_f[562] + 
        rr_f[563] + rr_f[564] + rr_f[612] + rr_f[618] + rr_f[697] + rr_f[727] + 
        rr_f[741] + rr_f[803] + rr_f[804] + rr_f[805] + rr_r[58] + rr_r[87] + 
        rr_r[88] + rr_r[89] + rr_r[90] + rr_r[91] + rr_r[104] + rr_r[115] + 
        rr_r[153]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[16] * 0.06233236489615739;
      double c0 = mole_frac[16] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[16]- c0) / dt) 
        * recip_ddot; 
      rr_f[58] *= scale_r;
      rr_f[87] *= scale_r;
      rr_f[88] *= scale_r;
      rr_f[89] *= scale_r;
      rr_f[90] *= scale_r;
      rr_f[91] *= scale_r;
      rr_f[104] *= scale_r;
      rr_f[115] *= scale_r;
      rr_f[153] *= scale_r;
      rr_r[31] *= scale_r;
      rr_r[41] *= scale_r;
      rr_r[59] *= scale_r;
      rr_r[83] *= scale_r;
      rr_r[86] *= scale_r;
      rr_r[98] *= scale_r;
      rr_r[133] *= scale_r;
      rr_r[142] *= scale_r;
      rr_r[170] *= scale_r;
      rr_r[178] *= scale_r;
      rr_r[200] *= scale_r;
      rr_r[210] *= scale_r;
      rr_r[222] *= scale_r;
      rr_r[240] *= scale_r;
      rr_r[249] *= scale_r;
      rr_r[535] *= scale_r;
      rr_r[291] *= scale_r;
      rr_r[292] *= scale_r;
      rr_r[293] *= scale_r;
      rr_r[302] *= scale_r;
      rr_r[315] *= scale_r;
      rr_r[323] *= scale_r;
      rr_r[333] *= scale_r;
      rr_r[346] *= scale_r;
      rr_r[381] *= scale_r;
      rr_r[417] *= scale_r;
      rr_r[458] *= scale_r;
      rr_r[480] *= scale_r;
      rr_r[486] *= scale_r;
      rr_r[502] *= scale_r;
      rr_r[515] *= scale_r;
      rr_r[516] *= scale_r;
      rr_r[561] *= scale_r;
      rr_r[562] *= scale_r;
      rr_r[563] *= scale_r;
      rr_r[564] *= scale_r;
      rr_r[612] *= scale_r;
      rr_r[618] *= scale_r;
      rr_r[697] *= scale_r;
      rr_r[727] *= scale_r;
      rr_r[741] *= scale_r;
      rr_r[803] *= scale_r;
      rr_r[804] *= scale_r;
      rr_r[805] *= scale_r;
    }
  }
  // Stiff species CH3
  {
    double ddot = rr_f[31] + rr_f[41] + rr_f[52] + rr_f[59] + rr_f[83] + 
      rr_f[86] + rr_f[92] + rr_f[93] + rr_f[94] + rr_f[95] + rr_f[96] + rr_f[97] 
      + rr_f[98] + rr_f[99] + rr_f[100] + rr_f[101] + rr_f[102] + rr_f[106] + 
      rr_f[127] + rr_f[127] + rr_f[133] + rr_f[142] + rr_f[170] + rr_f[178] + 
      rr_f[200] + rr_f[205] + rr_f[210] + rr_f[222] + rr_f[240] + rr_f[249] + 
      rr_f[535] + rr_f[291] + rr_f[292] + rr_f[293] + rr_f[302] + rr_f[315] + 
      rr_f[323] + rr_f[333] + rr_f[346] + rr_f[354] + rr_f[355] + rr_f[381] + 
      rr_f[417] + rr_f[434] + rr_f[458] + rr_f[480] + rr_f[486] + rr_f[502] + 
      rr_f[515] + rr_f[516] + rr_f[525] + rr_f[561] + rr_f[562] + rr_f[563] + 
      rr_f[564] + rr_f[612] + rr_f[618] + rr_f[697] + rr_f[727] + rr_f[741] + 
      rr_f[803] + rr_f[804] + rr_f[805] + rr_f[821] + rr_f[823] + rr_r[58] + 
      rr_r[74] + rr_r[87] + rr_r[88] + rr_r[89] + rr_r[90] + rr_r[91] + rr_r[91] 
      + rr_r[104] + rr_r[115] + rr_r[115] + rr_r[117] + rr_r[122] + rr_r[263] + 
      rr_r[264] + rr_r[137] + rr_r[143] + rr_r[143] + rr_r[149] + rr_r[153] + 
      rr_r[165] + rr_r[173] + rr_r[175] + rr_r[183] + rr_r[197] + rr_r[217] + 
      rr_r[218] + rr_r[227] + rr_r[259] + rr_r[268] + rr_r[287] + rr_r[297] + 
      rr_r[311] + rr_r[316] + rr_r[328] + rr_r[340] + rr_r[342] + rr_r[370] + 
      rr_r[375] + rr_r[391] + rr_r[401] + rr_r[424] + rr_r[440] + rr_r[452] + 
      rr_r[454] + rr_r[471] + rr_r[543] + rr_r[685] + rr_r[705] + rr_r[725] + 
      rr_r[734] + rr_r[735]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[17])
    {
      double cdot = rr_f[58] + rr_f[74] + rr_f[87] + rr_f[88] + rr_f[89] + 
        rr_f[90] + rr_f[91] + rr_f[91] + rr_f[104] + rr_f[115] + rr_f[115] + 
        rr_f[117] + rr_f[122] + rr_f[263] + rr_f[264] + rr_f[137] + rr_f[143] + 
        rr_f[143] + rr_f[149] + rr_f[153] + rr_f[165] + rr_f[173] + rr_f[175] + 
        rr_f[183] + rr_f[197] + rr_f[217] + rr_f[218] + rr_f[227] + rr_f[259] + 
        rr_f[268] + rr_f[273] + rr_f[287] + rr_f[297] + rr_f[311] + rr_f[316] + 
        rr_f[328] + rr_f[340] + rr_f[342] + rr_f[370] + rr_f[375] + rr_f[391] + 
        rr_f[401] + rr_f[424] + rr_f[440] + rr_f[452] + rr_f[454] + rr_f[456] + 
        rr_f[456] + rr_f[471] + rr_f[543] + rr_f[685] + rr_f[705] + rr_f[725] + 
        rr_f[734] + rr_f[735] + rr_r[31] + rr_r[41] + rr_r[59] + rr_r[83] + 
        rr_r[86] + rr_r[92] + rr_r[93] + rr_r[94] + rr_r[95] + rr_r[96] + 
        rr_r[97] + rr_r[98] + rr_r[99] + rr_r[100] + rr_r[101] + rr_r[102] + 
        rr_r[106] + rr_r[127] + rr_r[127] + rr_r[133] + rr_r[142] + rr_r[170] + 
        rr_r[178] + rr_r[200] + rr_r[205] + rr_r[210] + rr_r[222] + rr_r[240] + 
        rr_r[249] + rr_r[535] + rr_r[291] + rr_r[292] + rr_r[293] + rr_r[302] + 
        rr_r[315] + rr_r[323] + rr_r[333] + rr_r[346] + rr_r[354] + rr_r[355] + 
        rr_r[381] + rr_r[417] + rr_r[434] + rr_r[458] + rr_r[480] + rr_r[486] + 
        rr_r[502] + rr_r[515] + rr_r[516] + rr_r[525] + rr_r[561] + rr_r[562] + 
        rr_r[563] + rr_r[564] + rr_r[612] + rr_r[618] + rr_r[697] + rr_r[727] + 
        rr_r[741] + rr_r[803] + rr_r[804] + rr_r[805] + rr_r[821] + rr_r[823]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[17] * 0.06651120780362699;
      double c0 = mole_frac[17] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[17]- c0) / dt) 
        * recip_ddot; 
      rr_f[31] *= scale_r;
      rr_f[41] *= scale_r;
      rr_f[52] *= scale_r;
      rr_f[59] *= scale_r;
      rr_f[83] *= scale_r;
      rr_f[86] *= scale_r;
      rr_f[92] *= scale_r;
      rr_f[93] *= scale_r;
      rr_f[94] *= scale_r;
      rr_f[95] *= scale_r;
      rr_f[96] *= scale_r;
      rr_f[97] *= scale_r;
      rr_f[98] *= scale_r;
      rr_f[99] *= scale_r;
      rr_f[100] *= scale_r;
      rr_f[101] *= scale_r;
      rr_f[102] *= scale_r;
      rr_f[106] *= scale_r;
      rr_f[127] *= scale_r;
      rr_f[127] *= scale_r;
      rr_f[133] *= scale_r;
      rr_f[142] *= scale_r;
      rr_f[170] *= scale_r;
      rr_f[178] *= scale_r;
      rr_f[200] *= scale_r;
      rr_f[205] *= scale_r;
      rr_f[210] *= scale_r;
      rr_f[222] *= scale_r;
      rr_f[240] *= scale_r;
      rr_f[249] *= scale_r;
      rr_f[535] *= scale_r;
      rr_f[291] *= scale_r;
      rr_f[292] *= scale_r;
      rr_f[293] *= scale_r;
      rr_f[302] *= scale_r;
      rr_f[315] *= scale_r;
      rr_f[323] *= scale_r;
      rr_f[333] *= scale_r;
      rr_f[346] *= scale_r;
      rr_f[354] *= scale_r;
      rr_f[355] *= scale_r;
      rr_f[381] *= scale_r;
      rr_f[417] *= scale_r;
      rr_f[434] *= scale_r;
      rr_f[458] *= scale_r;
      rr_f[480] *= scale_r;
      rr_f[486] *= scale_r;
      rr_f[502] *= scale_r;
      rr_f[515] *= scale_r;
      rr_f[516] *= scale_r;
      rr_f[525] *= scale_r;
      rr_f[561] *= scale_r;
      rr_f[562] *= scale_r;
      rr_f[563] *= scale_r;
      rr_f[564] *= scale_r;
      rr_f[612] *= scale_r;
      rr_f[618] *= scale_r;
      rr_f[697] *= scale_r;
      rr_f[727] *= scale_r;
      rr_f[741] *= scale_r;
      rr_f[803] *= scale_r;
      rr_f[804] *= scale_r;
      rr_f[805] *= scale_r;
      rr_f[821] *= scale_r;
      rr_f[823] *= scale_r;
      rr_r[58] *= scale_r;
      rr_r[74] *= scale_r;
      rr_r[87] *= scale_r;
      rr_r[88] *= scale_r;
      rr_r[89] *= scale_r;
      rr_r[90] *= scale_r;
      rr_r[91] *= scale_r;
      rr_r[91] *= scale_r;
      rr_r[104] *= scale_r;
      rr_r[115] *= scale_r;
      rr_r[115] *= scale_r;
      rr_r[117] *= scale_r;
      rr_r[122] *= scale_r;
      rr_r[263] *= scale_r;
      rr_r[264] *= scale_r;
      rr_r[137] *= scale_r;
      rr_r[143] *= scale_r;
      rr_r[143] *= scale_r;
      rr_r[149] *= scale_r;
      rr_r[153] *= scale_r;
      rr_r[165] *= scale_r;
      rr_r[173] *= scale_r;
      rr_r[175] *= scale_r;
      rr_r[183] *= scale_r;
      rr_r[197] *= scale_r;
      rr_r[217] *= scale_r;
      rr_r[218] *= scale_r;
      rr_r[227] *= scale_r;
      rr_r[259] *= scale_r;
      rr_r[268] *= scale_r;
      rr_r[287] *= scale_r;
      rr_r[297] *= scale_r;
      rr_r[311] *= scale_r;
      rr_r[316] *= scale_r;
      rr_r[328] *= scale_r;
      rr_r[340] *= scale_r;
      rr_r[342] *= scale_r;
      rr_r[370] *= scale_r;
      rr_r[375] *= scale_r;
      rr_r[391] *= scale_r;
      rr_r[401] *= scale_r;
      rr_r[424] *= scale_r;
      rr_r[440] *= scale_r;
      rr_r[452] *= scale_r;
      rr_r[454] *= scale_r;
      rr_r[471] *= scale_r;
      rr_r[543] *= scale_r;
      rr_r[685] *= scale_r;
      rr_r[705] *= scale_r;
      rr_r[725] *= scale_r;
      rr_r[734] *= scale_r;
      rr_r[735] *= scale_r;
    }
  }
  // Stiff species C2H6
  {
    double ddot = rr_f[129] + rr_f[130] + rr_f[131] + rr_f[132] + rr_f[133] + 
      rr_f[134] + rr_f[135] + rr_f[136] + rr_f[137] + rr_f[156] + rr_r[128] + 
      rr_r[127] + rr_r[253] + rr_r[294] + rr_r[303] + rr_r[397] + rr_r[569] + 
      rr_r[570] + rr_r[571] + rr_r[572] + rr_r[745] + rr_r[815] + rr_r[816] + 
      rr_r[817]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[18])
    {
      double cdot = rr_f[128] + rr_f[127] + rr_f[253] + rr_f[294] + rr_f[303] + 
        rr_f[397] + rr_f[569] + rr_f[570] + rr_f[571] + rr_f[572] + rr_f[745] + 
        rr_f[815] + rr_f[816] + rr_f[817] + rr_r[129] + rr_r[130] + rr_r[131] + 
        rr_r[132] + rr_r[133] + rr_r[134] + rr_r[135] + rr_r[136] + rr_r[137] + 
        rr_r[156]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[18] * 0.03325560390181349;
      double c0 = mole_frac[18] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[18]- c0) / dt) 
        * recip_ddot; 
      rr_f[129] *= scale_r;
      rr_f[130] *= scale_r;
      rr_f[131] *= scale_r;
      rr_f[132] *= scale_r;
      rr_f[133] *= scale_r;
      rr_f[134] *= scale_r;
      rr_f[135] *= scale_r;
      rr_f[136] *= scale_r;
      rr_f[137] *= scale_r;
      rr_f[156] *= scale_r;
      rr_r[128] *= scale_r;
      rr_r[127] *= scale_r;
      rr_r[253] *= scale_r;
      rr_r[294] *= scale_r;
      rr_r[303] *= scale_r;
      rr_r[397] *= scale_r;
      rr_r[569] *= scale_r;
      rr_r[570] *= scale_r;
      rr_r[571] *= scale_r;
      rr_r[572] *= scale_r;
      rr_r[745] *= scale_r;
      rr_r[815] *= scale_r;
      rr_r[816] *= scale_r;
      rr_r[817] *= scale_r;
    }
  }
  // Stiff species C2H5
  {
    double ddot = rr_f[128] + rr_f[141] + rr_f[142] + rr_f[143] + rr_f[144] + 
      rr_f[145] + rr_f[146] + rr_f[147] + rr_f[158] + rr_f[159] + rr_f[160] + 
      rr_f[253] + rr_f[294] + rr_f[303] + rr_f[304] + rr_f[389] + rr_f[397] + 
      rr_f[435] + rr_f[569] + rr_f[570] + rr_f[571] + rr_f[572] + rr_f[745] + 
      rr_f[815] + rr_f[816] + rr_f[817] + rr_r[257] + rr_r[129] + rr_r[130] + 
      rr_r[131] + rr_r[132] + rr_r[133] + rr_r[134] + rr_r[135] + rr_r[136] + 
      rr_r[137] + rr_r[138] + rr_r[151] + rr_r[156] + rr_r[245] + rr_r[259] + 
      rr_r[272] + rr_r[369] + rr_r[376] + rr_r[403] + rr_r[411] + rr_r[507] + 
      rr_r[598] + rr_r[633]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[19])
    {
      double cdot = rr_f[257] + rr_f[129] + rr_f[130] + rr_f[131] + rr_f[132] + 
        rr_f[133] + rr_f[134] + rr_f[135] + rr_f[136] + rr_f[137] + rr_f[138] + 
        rr_f[151] + rr_f[156] + rr_f[245] + rr_f[259] + rr_f[272] + rr_f[359] + 
        rr_f[360] + rr_f[369] + rr_f[376] + rr_f[403] + rr_f[411] + rr_f[507] + 
        rr_f[598] + rr_f[633] + rr_r[128] + rr_r[141] + rr_r[142] + rr_r[143] + 
        rr_r[144] + rr_r[145] + rr_r[146] + rr_r[147] + rr_r[158] + rr_r[159] + 
        rr_r[160] + rr_r[253] + rr_r[294] + rr_r[303] + rr_r[304] + rr_r[389] + 
        rr_r[397] + rr_r[435] + rr_r[569] + rr_r[570] + rr_r[571] + rr_r[572] + 
        rr_r[745] + rr_r[815] + rr_r[816] + rr_r[817]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[19] * 0.0344090165386938;
      double c0 = mole_frac[19] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[19]- c0) / dt) 
        * recip_ddot; 
      rr_f[128] *= scale_r;
      rr_f[141] *= scale_r;
      rr_f[142] *= scale_r;
      rr_f[143] *= scale_r;
      rr_f[144] *= scale_r;
      rr_f[145] *= scale_r;
      rr_f[146] *= scale_r;
      rr_f[147] *= scale_r;
      rr_f[158] *= scale_r;
      rr_f[159] *= scale_r;
      rr_f[160] *= scale_r;
      rr_f[253] *= scale_r;
      rr_f[294] *= scale_r;
      rr_f[303] *= scale_r;
      rr_f[304] *= scale_r;
      rr_f[389] *= scale_r;
      rr_f[397] *= scale_r;
      rr_f[435] *= scale_r;
      rr_f[569] *= scale_r;
      rr_f[570] *= scale_r;
      rr_f[571] *= scale_r;
      rr_f[572] *= scale_r;
      rr_f[745] *= scale_r;
      rr_f[815] *= scale_r;
      rr_f[816] *= scale_r;
      rr_f[817] *= scale_r;
      rr_r[257] *= scale_r;
      rr_r[129] *= scale_r;
      rr_r[130] *= scale_r;
      rr_r[131] *= scale_r;
      rr_r[132] *= scale_r;
      rr_r[133] *= scale_r;
      rr_r[134] *= scale_r;
      rr_r[135] *= scale_r;
      rr_r[136] *= scale_r;
      rr_r[137] *= scale_r;
      rr_r[138] *= scale_r;
      rr_r[151] *= scale_r;
      rr_r[156] *= scale_r;
      rr_r[245] *= scale_r;
      rr_r[259] *= scale_r;
      rr_r[272] *= scale_r;
      rr_r[369] *= scale_r;
      rr_r[376] *= scale_r;
      rr_r[403] *= scale_r;
      rr_r[411] *= scale_r;
      rr_r[507] *= scale_r;
      rr_r[598] *= scale_r;
      rr_r[633] *= scale_r;
    }
  }
  // Stiff species C2H3
  {
    double ddot = rr_f[141] + rr_f[194] + rr_f[207] + rr_f[208] + rr_f[209] + 
      rr_f[210] + rr_f[211] + rr_f[212] + rr_f[241] + rr_f[256] + rr_f[305] + 
      rr_f[347] + rr_f[398] + rr_f[408] + rr_f[526] + rr_f[577] + rr_f[578] + 
      rr_f[579] + rr_f[580] + rr_f[746] + rr_f[818] + rr_f[819] + rr_f[820] + 
      rr_r[196] + rr_r[199] + rr_r[200] + rr_r[201] + rr_r[202] + rr_r[203] + 
      rr_r[204] + rr_r[206] + rr_r[234] + rr_r[244] + rr_r[268] + rr_r[341] + 
      rr_r[365] + rr_r[376] + rr_r[400] + rr_r[402] + rr_r[402] + rr_r[405]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[21])
    {
      double cdot = rr_f[196] + rr_f[199] + rr_f[200] + rr_f[201] + rr_f[202] + 
        rr_f[203] + rr_f[204] + rr_f[206] + rr_f[234] + rr_f[244] + rr_f[268] + 
        rr_f[341] + rr_f[365] + rr_f[376] + rr_f[400] + rr_f[402] + rr_f[402] + 
        rr_f[405] + rr_f[724] + rr_r[141] + rr_r[194] + rr_r[207] + rr_r[208] + 
        rr_r[209] + rr_r[210] + rr_r[211] + rr_r[212] + rr_r[241] + rr_r[256] + 
        rr_r[305] + rr_r[347] + rr_r[398] + rr_r[408] + rr_r[526] + rr_r[577] + 
        rr_r[578] + rr_r[579] + rr_r[580] + rr_r[746] + rr_r[818] + rr_r[819] + 
        rr_r[820]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[21] * 0.0369737571363973;
      double c0 = mole_frac[21] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[21]- c0) / dt) 
        * recip_ddot; 
      rr_f[141] *= scale_r;
      rr_f[194] *= scale_r;
      rr_f[207] *= scale_r;
      rr_f[208] *= scale_r;
      rr_f[209] *= scale_r;
      rr_f[210] *= scale_r;
      rr_f[211] *= scale_r;
      rr_f[212] *= scale_r;
      rr_f[241] *= scale_r;
      rr_f[256] *= scale_r;
      rr_f[305] *= scale_r;
      rr_f[347] *= scale_r;
      rr_f[398] *= scale_r;
      rr_f[408] *= scale_r;
      rr_f[526] *= scale_r;
      rr_f[577] *= scale_r;
      rr_f[578] *= scale_r;
      rr_f[579] *= scale_r;
      rr_f[580] *= scale_r;
      rr_f[746] *= scale_r;
      rr_f[818] *= scale_r;
      rr_f[819] *= scale_r;
      rr_f[820] *= scale_r;
      rr_r[196] *= scale_r;
      rr_r[199] *= scale_r;
      rr_r[200] *= scale_r;
      rr_r[201] *= scale_r;
      rr_r[202] *= scale_r;
      rr_r[203] *= scale_r;
      rr_r[204] *= scale_r;
      rr_r[206] *= scale_r;
      rr_r[234] *= scale_r;
      rr_r[244] *= scale_r;
      rr_r[268] *= scale_r;
      rr_r[341] *= scale_r;
      rr_r[365] *= scale_r;
      rr_r[376] *= scale_r;
      rr_r[400] *= scale_r;
      rr_r[402] *= scale_r;
      rr_r[402] *= scale_r;
      rr_r[405] *= scale_r;
    }
  }
  // Stiff species C2H2
  {
    double ddot = rr_f[206] + rr_f[213] + rr_f[214] + rr_f[215] + rr_f[216] + 
      rr_f[217] + rr_f[354] + rr_f[355] + rr_r[195] + rr_r[207] + rr_r[210] + 
      rr_r[211] + rr_r[212] + rr_r[297] + rr_r[311] + rr_r[316] + rr_r[331] + 
      rr_r[357] + rr_r[505]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[22])
    {
      double cdot = rr_f[195] + rr_f[207] + rr_f[210] + rr_f[211] + rr_f[212] + 
        rr_f[297] + rr_f[310] + rr_f[311] + rr_f[316] + rr_f[331] + rr_f[357] + 
        rr_f[505] + rr_r[206] + rr_r[213] + rr_r[214] + rr_r[215] + rr_r[216] + 
        rr_r[217] + rr_r[354] + rr_r[355]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[22] * 0.0384050534905585;
      double c0 = mole_frac[22] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[22]- c0) / dt) 
        * recip_ddot; 
      rr_f[206] *= scale_r;
      rr_f[213] *= scale_r;
      rr_f[214] *= scale_r;
      rr_f[215] *= scale_r;
      rr_f[216] *= scale_r;
      rr_f[217] *= scale_r;
      rr_f[354] *= scale_r;
      rr_f[355] *= scale_r;
      rr_r[195] *= scale_r;
      rr_r[207] *= scale_r;
      rr_r[210] *= scale_r;
      rr_r[211] *= scale_r;
      rr_r[212] *= scale_r;
      rr_r[297] *= scale_r;
      rr_r[311] *= scale_r;
      rr_r[316] *= scale_r;
      rr_r[331] *= scale_r;
      rr_r[357] *= scale_r;
      rr_r[505] *= scale_r;
    }
  }
  // Stiff species CH3CHO
  {
    double ddot = rr_f[165] + rr_f[166] + rr_f[167] + rr_f[168] + rr_f[169] + 
      rr_f[170] + rr_f[171] + rr_f[172] + rr_f[173] + rr_f[174] + rr_r[263] + 
      rr_r[145] + rr_r[148] + rr_r[150] + rr_r[160] + rr_r[161] + rr_r[313] + 
      rr_r[400] + rr_r[405] + rr_r[528]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[23])
    {
      double cdot = rr_f[263] + rr_f[145] + rr_f[148] + rr_f[150] + rr_f[160] + 
        rr_f[161] + rr_f[313] + rr_f[361] + rr_f[400] + rr_f[405] + rr_f[528] + 
        rr_f[629] + rr_f[630] + rr_f[658] + rr_f[787] + rr_r[165] + rr_r[166] + 
        rr_r[167] + rr_r[168] + rr_r[169] + rr_r[170] + rr_r[171] + rr_r[172] + 
        rr_r[173] + rr_r[174]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[23] * 0.02269963076780593;
      double c0 = mole_frac[23] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[23]- c0) / dt) 
        * recip_ddot; 
      rr_f[165] *= scale_r;
      rr_f[166] *= scale_r;
      rr_f[167] *= scale_r;
      rr_f[168] *= scale_r;
      rr_f[169] *= scale_r;
      rr_f[170] *= scale_r;
      rr_f[171] *= scale_r;
      rr_f[172] *= scale_r;
      rr_f[173] *= scale_r;
      rr_f[174] *= scale_r;
      rr_r[263] *= scale_r;
      rr_r[145] *= scale_r;
      rr_r[148] *= scale_r;
      rr_r[150] *= scale_r;
      rr_r[160] *= scale_r;
      rr_r[161] *= scale_r;
      rr_r[313] *= scale_r;
      rr_r[400] *= scale_r;
      rr_r[405] *= scale_r;
      rr_r[528] *= scale_r;
    }
  }
  // Stiff species CH2CHO
  {
    double ddot = rr_f[180] + rr_f[181] + rr_r[164] + rr_r[174] + rr_r[198] + 
      rr_r[209] + rr_r[308]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[24])
    {
      double cdot = rr_f[164] + rr_f[174] + rr_f[198] + rr_f[209] + rr_f[308] + 
        rr_f[668] + rr_r[180]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[24] * 0.02323117270262868;
      double c0 = mole_frac[24] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[24]- c0) / dt) 
        * recip_ddot; 
      rr_f[180] *= scale_r;
      rr_f[181] *= scale_r;
      rr_r[164] *= scale_r;
      rr_r[174] *= scale_r;
      rr_r[198] *= scale_r;
      rr_r[209] *= scale_r;
      rr_r[308] *= scale_r;
    }
  }
  // Stiff species HCCO
  {
    double ddot = rr_f[190] + rr_f[191] + rr_f[192] + rr_f[193] + rr_r[184] + 
      rr_r[186] + rr_r[187] + rr_r[213] + rr_r[215] + rr_r[336] + rr_r[342] + 
      rr_r[351]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[26])
    {
      double cdot = rr_f[184] + rr_f[186] + rr_f[187] + rr_f[213] + rr_f[215] + 
        rr_f[336] + rr_f[342] + rr_f[343] + rr_f[351] + rr_f[358] + rr_r[191]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[26] * 0.02437260645771706;
      double c0 = mole_frac[26] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[26]- c0) / dt) 
        * recip_ddot; 
      rr_f[190] *= scale_r;
      rr_f[191] *= scale_r;
      rr_f[192] *= scale_r;
      rr_f[193] *= scale_r;
      rr_r[184] *= scale_r;
      rr_r[186] *= scale_r;
      rr_r[187] *= scale_r;
      rr_r[213] *= scale_r;
      rr_r[215] *= scale_r;
      rr_r[336] *= scale_r;
      rr_r[342] *= scale_r;
      rr_r[351] *= scale_r;
    }
  }
  // Stiff species CH3CO3
  {
    double ddot = rr_f[179];
    if ((ddot * dt * 9.750641978812205) > mole_frac[27])
    {
      double cdot = rr_r[179];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[27] * 0.01332544289441412;
      double c0 = mole_frac[27] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[27]- c0) / dt) 
        * recip_ddot; 
      rr_f[179] *= scale_r;
    }
  }
  // Stiff species C2H5O2H
  {
    double ddot = rr_f[157] + rr_r[140] + rr_r[152] + rr_r[153] + rr_r[154] + 
      rr_r[155] + rr_r[156] + rr_r[204] + rr_r[254] + rr_r[296] + rr_r[387]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[28])
    {
      double cdot = rr_f[140] + rr_f[152] + rr_f[153] + rr_f[154] + rr_f[155] + 
        rr_f[156] + rr_f[204] + rr_f[254] + rr_f[296] + rr_f[387] + rr_r[157]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[28] * 0.01611112292593459;
      double c0 = mole_frac[28] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[28]- c0) / dt) 
        * recip_ddot; 
      rr_f[157] *= scale_r;
      rr_r[140] *= scale_r;
      rr_r[152] *= scale_r;
      rr_r[153] *= scale_r;
      rr_r[154] *= scale_r;
      rr_r[155] *= scale_r;
      rr_r[156] *= scale_r;
      rr_r[204] *= scale_r;
      rr_r[254] *= scale_r;
      rr_r[296] *= scale_r;
      rr_r[387] *= scale_r;
    }
  }
  // Stiff species C2H5O2
  {
    double ddot = rr_f[140] + rr_f[151] + rr_f[152] + rr_f[153] + rr_f[154] + 
      rr_f[155] + rr_f[156] + rr_f[161] + rr_f[162] + rr_f[204] + rr_f[254] + 
      rr_f[296] + rr_f[387] + rr_f[399] + rr_f[431] + rr_f[624] + rr_f[708] + 
      rr_f[711]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[29])
    {
      double cdot = rr_r[140] + rr_r[151] + rr_r[152] + rr_r[153] + rr_r[154] + 
        rr_r[155] + rr_r[156] + rr_r[161] + rr_r[162] + rr_r[204] + rr_r[254] + 
        rr_r[296] + rr_r[387] + rr_r[399] + rr_r[624] + rr_r[708] + rr_r[711]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[29] * 0.01637707896781822;
      double c0 = mole_frac[29] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[29]- c0) / dt) 
        * recip_ddot; 
      rr_f[140] *= scale_r;
      rr_f[151] *= scale_r;
      rr_f[152] *= scale_r;
      rr_f[153] *= scale_r;
      rr_f[154] *= scale_r;
      rr_f[155] *= scale_r;
      rr_f[156] *= scale_r;
      rr_f[161] *= scale_r;
      rr_f[162] *= scale_r;
      rr_f[204] *= scale_r;
      rr_f[254] *= scale_r;
      rr_f[296] *= scale_r;
      rr_f[387] *= scale_r;
      rr_f[399] *= scale_r;
      rr_f[431] *= scale_r;
      rr_f[624] *= scale_r;
      rr_f[708] *= scale_r;
      rr_f[711] *= scale_r;
    }
  }
  // Stiff species CH3COCH3
  {
    double ddot = rr_f[218] + rr_f[219] + rr_f[220] + rr_f[221] + rr_f[222] + 
      rr_f[223] + rr_f[224] + rr_f[225] + rr_f[226] + rr_f[229] + rr_r[262] + 
      rr_r[440] + rr_r[492] + rr_r[713]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[30])
    {
      double cdot = rr_f[262] + rr_f[440] + rr_f[492] + rr_f[500] + rr_f[692] + 
        rr_f[693] + rr_f[694] + rr_f[713] + rr_f[752] + rr_f[860] + rr_r[218] + 
        rr_r[219] + rr_r[220] + rr_r[221] + rr_r[222] + rr_r[223] + rr_r[224] + 
        rr_r[225] + rr_r[226] + rr_r[229]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[30] * 0.01721743223692151;
      double c0 = mole_frac[30] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[30]- c0) / dt) 
        * recip_ddot; 
      rr_f[218] *= scale_r;
      rr_f[219] *= scale_r;
      rr_f[220] *= scale_r;
      rr_f[221] *= scale_r;
      rr_f[222] *= scale_r;
      rr_f[223] *= scale_r;
      rr_f[224] *= scale_r;
      rr_f[225] *= scale_r;
      rr_f[226] *= scale_r;
      rr_f[229] *= scale_r;
      rr_r[262] *= scale_r;
      rr_r[440] *= scale_r;
      rr_r[492] *= scale_r;
      rr_r[713] *= scale_r;
    }
  }
  // Stiff species CH3COCH2O2
  {
    double ddot = rr_f[228] + rr_f[229] + rr_f[230] + rr_f[231];
    if ((ddot * dt * 9.750641978812205) > mole_frac[31])
    {
      double cdot = rr_r[228] + rr_r[229] + rr_r[230];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[31] * 0.01122693566404518;
      double c0 = mole_frac[31] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[31]- c0) / dt) 
        * recip_ddot; 
      rr_f[228] *= scale_r;
      rr_f[229] *= scale_r;
      rr_f[230] *= scale_r;
      rr_f[231] *= scale_r;
    }
  }
  // Stiff species CH3COCH2O2H
  {
    double ddot = rr_f[232] + rr_r[229] + rr_r[230];
    if ((ddot * dt * 9.750641978812205) > mole_frac[32])
    {
      double cdot = rr_f[229] + rr_f[230] + rr_f[231] + rr_r[232];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[32] * 0.01110130865556824;
      double c0 = mole_frac[32] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[32]- c0) / dt) 
        * recip_ddot; 
      rr_f[232] *= scale_r;
      rr_r[229] *= scale_r;
      rr_r[230] *= scale_r;
    }
  }
  // Stiff species C2H3CHO
  {
    double ddot = rr_f[234] + rr_f[235] + rr_f[236] + rr_f[237] + rr_f[238] + 
      rr_f[239] + rr_f[240] + rr_f[241] + rr_f[242] + rr_f[243] + rr_r[309] + 
      rr_r[364] + rr_r[366] + rr_r[391] + rr_r[401] + rr_r[679]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[33])
    {
      double cdot = rr_f[309] + rr_f[364] + rr_f[366] + rr_f[391] + rr_f[401] + 
        rr_f[679] + rr_r[234] + rr_r[235] + rr_r[236] + rr_r[237] + rr_r[238] + 
        rr_r[239] + rr_r[240] + rr_r[241] + rr_r[242] + rr_r[243]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[33] * 0.01783652574443861;
      double c0 = mole_frac[33] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[33]- c0) / dt) 
        * recip_ddot; 
      rr_f[234] *= scale_r;
      rr_f[235] *= scale_r;
      rr_f[236] *= scale_r;
      rr_f[237] *= scale_r;
      rr_f[238] *= scale_r;
      rr_f[239] *= scale_r;
      rr_f[240] *= scale_r;
      rr_f[241] *= scale_r;
      rr_f[242] *= scale_r;
      rr_f[243] *= scale_r;
      rr_r[309] *= scale_r;
      rr_r[364] *= scale_r;
      rr_r[366] *= scale_r;
      rr_r[391] *= scale_r;
      rr_r[401] *= scale_r;
      rr_r[679] *= scale_r;
    }
  }
  // Stiff species C2H3CO
  {
    double ddot = rr_f[244] + rr_r[235] + rr_r[236] + rr_r[237] + rr_r[238] + 
      rr_r[239] + rr_r[240] + rr_r[241] + rr_r[242] + rr_r[243]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[34])
    {
      double cdot = rr_f[235] + rr_f[236] + rr_f[237] + rr_f[238] + rr_f[239] + 
        rr_f[240] + rr_f[241] + rr_f[242] + rr_f[243] + rr_r[244]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[34] * 0.01816307388956415;
      double c0 = mole_frac[34] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[34]- c0) / dt) 
        * recip_ddot; 
      rr_f[244] *= scale_r;
      rr_r[235] *= scale_r;
      rr_r[236] *= scale_r;
      rr_r[237] *= scale_r;
      rr_r[238] *= scale_r;
      rr_r[239] *= scale_r;
      rr_r[240] *= scale_r;
      rr_r[241] *= scale_r;
      rr_r[242] *= scale_r;
      rr_r[243] *= scale_r;
    }
  }
  // Stiff species C2H5CHO
  {
    double ddot = rr_f[245] + rr_f[246] + rr_f[247] + rr_f[248] + rr_f[249] + 
      rr_f[250] + rr_f[251] + rr_f[252] + rr_f[253] + rr_f[254] + rr_f[255] + 
      rr_f[256] + rr_f[267] + rr_r[681]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[35])
    {
      double cdot = rr_f[627] + rr_f[628] + rr_f[659] + rr_f[681] + rr_f[789] + 
        rr_f[790] + rr_r[245] + rr_r[246] + rr_r[247] + rr_r[248] + rr_r[249] + 
        rr_r[250] + rr_r[251] + rr_r[252] + rr_r[253] + rr_r[254] + rr_r[255] + 
        rr_r[256] + rr_r[267]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[35] * 0.01721743223692151;
      double c0 = mole_frac[35] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[35]- c0) / dt) 
        * recip_ddot; 
      rr_f[245] *= scale_r;
      rr_f[246] *= scale_r;
      rr_f[247] *= scale_r;
      rr_f[248] *= scale_r;
      rr_f[249] *= scale_r;
      rr_f[250] *= scale_r;
      rr_f[251] *= scale_r;
      rr_f[252] *= scale_r;
      rr_f[253] *= scale_r;
      rr_f[254] *= scale_r;
      rr_f[255] *= scale_r;
      rr_f[256] *= scale_r;
      rr_f[267] *= scale_r;
      rr_r[681] *= scale_r;
    }
  }
  // Stiff species C3H6
  {
    double ddot = rr_f[268] + rr_f[269] + rr_f[270] + rr_f[271] + rr_f[272] + 
      rr_f[273] + rr_f[274] + rr_f[275] + rr_f[276] + rr_f[277] + rr_f[278] + 
      rr_f[279] + rr_f[280] + rr_f[281] + rr_f[282] + rr_f[283] + rr_f[284] + 
      rr_f[285] + rr_f[286] + rr_f[287] + rr_f[288] + rr_f[289] + rr_f[290] + 
      rr_f[291] + rr_f[292] + rr_f[293] + rr_f[294] + rr_f[295] + rr_f[296] + 
      rr_f[306] + rr_f[526] + rr_f[824] + rr_r[265] + rr_r[266] + rr_r[258] + 
      rr_r[260] + rr_r[261] + rr_r[267] + rr_r[304] + rr_r[334] + rr_r[348] + 
      rr_r[367] + rr_r[368] + rr_r[370] + rr_r[385] + rr_r[394] + rr_r[424] + 
      rr_r[454] + rr_r[461] + rr_r[462] + rr_r[463] + rr_r[592] + rr_r[732] + 
      rr_r[769]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[36])
    {
      double cdot = rr_f[265] + rr_f[266] + rr_f[258] + rr_f[260] + rr_f[261] + 
        rr_f[267] + rr_f[304] + rr_f[334] + rr_f[348] + rr_f[367] + rr_f[368] + 
        rr_f[370] + rr_f[385] + rr_f[394] + rr_f[424] + rr_f[451] + rr_f[454] + 
        rr_f[461] + rr_f[462] + rr_f[463] + rr_f[592] + rr_f[660] + rr_f[672] + 
        rr_f[676] + rr_f[732] + rr_f[765] + rr_f[767] + rr_f[769] + rr_f[848] + 
        rr_r[268] + rr_r[269] + rr_r[270] + rr_r[271] + rr_r[272] + rr_r[275] + 
        rr_r[276] + rr_r[277] + rr_r[278] + rr_r[279] + rr_r[280] + rr_r[281] + 
        rr_r[282] + rr_r[283] + rr_r[284] + rr_r[285] + rr_r[286] + rr_r[287] + 
        rr_r[288] + rr_r[289] + rr_r[290] + rr_r[291] + rr_r[292] + rr_r[293] + 
        rr_r[294] + rr_r[295] + rr_r[296] + rr_r[306] + rr_r[526] + rr_r[824]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[36] * 0.02376354135699802;
      double c0 = mole_frac[36] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[36]- c0) / dt) 
        * recip_ddot; 
      rr_f[268] *= scale_r;
      rr_f[269] *= scale_r;
      rr_f[270] *= scale_r;
      rr_f[271] *= scale_r;
      rr_f[272] *= scale_r;
      rr_f[273] *= scale_r;
      rr_f[274] *= scale_r;
      rr_f[275] *= scale_r;
      rr_f[276] *= scale_r;
      rr_f[277] *= scale_r;
      rr_f[278] *= scale_r;
      rr_f[279] *= scale_r;
      rr_f[280] *= scale_r;
      rr_f[281] *= scale_r;
      rr_f[282] *= scale_r;
      rr_f[283] *= scale_r;
      rr_f[284] *= scale_r;
      rr_f[285] *= scale_r;
      rr_f[286] *= scale_r;
      rr_f[287] *= scale_r;
      rr_f[288] *= scale_r;
      rr_f[289] *= scale_r;
      rr_f[290] *= scale_r;
      rr_f[291] *= scale_r;
      rr_f[292] *= scale_r;
      rr_f[293] *= scale_r;
      rr_f[294] *= scale_r;
      rr_f[295] *= scale_r;
      rr_f[296] *= scale_r;
      rr_f[306] *= scale_r;
      rr_f[526] *= scale_r;
      rr_f[824] *= scale_r;
      rr_r[265] *= scale_r;
      rr_r[266] *= scale_r;
      rr_r[258] *= scale_r;
      rr_r[260] *= scale_r;
      rr_r[261] *= scale_r;
      rr_r[267] *= scale_r;
      rr_r[304] *= scale_r;
      rr_r[334] *= scale_r;
      rr_r[348] *= scale_r;
      rr_r[367] *= scale_r;
      rr_r[368] *= scale_r;
      rr_r[370] *= scale_r;
      rr_r[385] *= scale_r;
      rr_r[394] *= scale_r;
      rr_r[424] *= scale_r;
      rr_r[454] *= scale_r;
      rr_r[461] *= scale_r;
      rr_r[462] *= scale_r;
      rr_r[463] *= scale_r;
      rr_r[592] *= scale_r;
      rr_r[732] *= scale_r;
      rr_r[769] *= scale_r;
    }
  }
  // Stiff species C3H5-A
  {
    double ddot = rr_f[267] + rr_f[297] + rr_f[298] + rr_f[299] + rr_f[300] + 
      rr_f[301] + rr_f[302] + rr_f[303] + rr_f[304] + rr_f[305] + rr_f[307] + 
      rr_f[308] + rr_f[309] + rr_f[310] + rr_f[334] + rr_f[348] + rr_f[385] + 
      rr_f[394] + rr_f[437] + rr_f[461] + rr_f[769] + rr_r[269] + rr_r[275] + 
      rr_r[278] + rr_r[281] + rr_r[284] + rr_r[288] + rr_r[291] + rr_r[294] + 
      rr_r[295] + rr_r[296] + rr_r[306] + rr_r[306] + rr_r[375] + rr_r[404] + 
      rr_r[507] + rr_r[631] + rr_r[678]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[37])
    {
      double cdot = rr_f[269] + rr_f[275] + rr_f[278] + rr_f[281] + rr_f[284] + 
        rr_f[288] + rr_f[291] + rr_f[294] + rr_f[295] + rr_f[296] + rr_f[306] + 
        rr_f[306] + rr_f[375] + rr_f[404] + rr_f[507] + rr_f[631] + rr_f[678] + 
        rr_r[267] + rr_r[297] + rr_r[298] + rr_r[299] + rr_r[300] + rr_r[301] + 
        rr_r[302] + rr_r[303] + rr_r[304] + rr_r[305] + rr_r[307] + rr_r[308] + 
        rr_r[309] + rr_r[334] + rr_r[348] + rr_r[385] + rr_r[394] + rr_r[437] + 
        rr_r[461] + rr_r[769]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[37] * 0.02434671672351625;
      double c0 = mole_frac[37] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[37]- c0) / dt) 
        * recip_ddot; 
      rr_f[267] *= scale_r;
      rr_f[297] *= scale_r;
      rr_f[298] *= scale_r;
      rr_f[299] *= scale_r;
      rr_f[300] *= scale_r;
      rr_f[301] *= scale_r;
      rr_f[302] *= scale_r;
      rr_f[303] *= scale_r;
      rr_f[304] *= scale_r;
      rr_f[305] *= scale_r;
      rr_f[307] *= scale_r;
      rr_f[308] *= scale_r;
      rr_f[309] *= scale_r;
      rr_f[310] *= scale_r;
      rr_f[334] *= scale_r;
      rr_f[348] *= scale_r;
      rr_f[385] *= scale_r;
      rr_f[394] *= scale_r;
      rr_f[437] *= scale_r;
      rr_f[461] *= scale_r;
      rr_f[769] *= scale_r;
      rr_r[269] *= scale_r;
      rr_r[275] *= scale_r;
      rr_r[278] *= scale_r;
      rr_r[281] *= scale_r;
      rr_r[284] *= scale_r;
      rr_r[288] *= scale_r;
      rr_r[291] *= scale_r;
      rr_r[294] *= scale_r;
      rr_r[295] *= scale_r;
      rr_r[296] *= scale_r;
      rr_r[306] *= scale_r;
      rr_r[306] *= scale_r;
      rr_r[375] *= scale_r;
      rr_r[404] *= scale_r;
      rr_r[507] *= scale_r;
      rr_r[631] *= scale_r;
      rr_r[678] *= scale_r;
    }
  }
  // Stiff species C3H4-P
  {
    double ddot = rr_f[335] + rr_f[336] + rr_f[337] + rr_f[338] + rr_f[339] + 
      rr_f[340] + rr_f[341] + rr_f[342] + rr_f[343] + rr_f[344] + rr_f[345] + 
      rr_f[346] + rr_f[347] + rr_f[348] + rr_r[312] + rr_r[318] + rr_r[322] + 
      rr_r[323] + rr_r[325] + rr_r[354] + rr_r[685]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[38])
    {
      double cdot = rr_f[312] + rr_f[318] + rr_f[322] + rr_f[323] + rr_f[325] + 
        rr_f[354] + rr_f[685] + rr_r[335] + rr_r[336] + rr_r[337] + rr_r[338] + 
        rr_r[339] + rr_r[340] + rr_r[341] + rr_r[342] + rr_r[344] + rr_r[345] + 
        rr_r[346] + rr_r[347] + rr_r[348]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[38] * 0.02495923532889907;
      double c0 = mole_frac[38] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[38]- c0) / dt) 
        * recip_ddot; 
      rr_f[335] *= scale_r;
      rr_f[336] *= scale_r;
      rr_f[337] *= scale_r;
      rr_f[338] *= scale_r;
      rr_f[339] *= scale_r;
      rr_f[340] *= scale_r;
      rr_f[341] *= scale_r;
      rr_f[342] *= scale_r;
      rr_f[343] *= scale_r;
      rr_f[344] *= scale_r;
      rr_f[345] *= scale_r;
      rr_f[346] *= scale_r;
      rr_f[347] *= scale_r;
      rr_f[348] *= scale_r;
      rr_r[312] *= scale_r;
      rr_r[318] *= scale_r;
      rr_r[322] *= scale_r;
      rr_r[323] *= scale_r;
      rr_r[325] *= scale_r;
      rr_r[354] *= scale_r;
      rr_r[685] *= scale_r;
    }
  }
  // Stiff species C3H4-A
  {
    double ddot = rr_f[306] + rr_f[324] + rr_f[325] + rr_f[326] + rr_f[327] + 
      rr_f[328] + rr_f[329] + rr_f[330] + rr_f[331] + rr_f[332] + rr_f[333] + 
      rr_f[334] + rr_f[352] + rr_f[353] + rr_r[298] + rr_r[301] + rr_r[302] + 
      rr_r[303] + rr_r[305] + rr_r[307] + rr_r[314] + rr_r[315] + rr_r[317] + 
      rr_r[319] + rr_r[355] + rr_r[407] + rr_r[471] + rr_r[477] + rr_r[506] + 
      rr_r[723]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[39])
    {
      double cdot = rr_f[298] + rr_f[301] + rr_f[302] + rr_f[303] + rr_f[305] + 
        rr_f[307] + rr_f[314] + rr_f[315] + rr_f[317] + rr_f[319] + rr_f[355] + 
        rr_f[407] + rr_f[469] + rr_f[471] + rr_f[477] + rr_f[506] + rr_f[723] + 
        rr_r[306] + rr_r[324] + rr_r[325] + rr_r[326] + rr_r[327] + rr_r[328] + 
        rr_r[329] + rr_r[330] + rr_r[331] + rr_r[332] + rr_r[333] + rr_r[334] + 
        rr_r[352] + rr_r[353]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[39] * 0.02495923532889907;
      double c0 = mole_frac[39] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[39]- c0) / dt) 
        * recip_ddot; 
      rr_f[306] *= scale_r;
      rr_f[324] *= scale_r;
      rr_f[325] *= scale_r;
      rr_f[326] *= scale_r;
      rr_f[327] *= scale_r;
      rr_f[328] *= scale_r;
      rr_f[329] *= scale_r;
      rr_f[330] *= scale_r;
      rr_f[331] *= scale_r;
      rr_f[332] *= scale_r;
      rr_f[333] *= scale_r;
      rr_f[334] *= scale_r;
      rr_f[352] *= scale_r;
      rr_f[353] *= scale_r;
      rr_r[298] *= scale_r;
      rr_r[301] *= scale_r;
      rr_r[302] *= scale_r;
      rr_r[303] *= scale_r;
      rr_r[305] *= scale_r;
      rr_r[307] *= scale_r;
      rr_r[314] *= scale_r;
      rr_r[315] *= scale_r;
      rr_r[317] *= scale_r;
      rr_r[319] *= scale_r;
      rr_r[355] *= scale_r;
      rr_r[407] *= scale_r;
      rr_r[471] *= scale_r;
      rr_r[477] *= scale_r;
      rr_r[506] *= scale_r;
      rr_r[723] *= scale_r;
    }
  }
  // Stiff species C3H3
  {
    double ddot = rr_f[349] + rr_f[350] + rr_f[356] + rr_r[324] + rr_r[326] + 
      rr_r[329] + rr_r[332] + rr_r[333] + rr_r[334] + rr_r[335] + rr_r[337] + 
      rr_r[339] + rr_r[344] + rr_r[345] + rr_r[346] + rr_r[347] + rr_r[348] + 
      rr_r[353]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[40])
    {
      double cdot = rr_f[324] + rr_f[326] + rr_f[329] + rr_f[332] + rr_f[333] + 
        rr_f[334] + rr_f[335] + rr_f[337] + rr_f[339] + rr_f[344] + rr_f[345] + 
        rr_f[346] + rr_f[347] + rr_f[348] + rr_f[353] + rr_r[349] + rr_r[350] + 
        rr_r[356]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[40] * 0.02560336899370566;
      double c0 = mole_frac[40] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[40]- c0) / dt) 
        * recip_ddot; 
      rr_f[349] *= scale_r;
      rr_f[350] *= scale_r;
      rr_f[356] *= scale_r;
      rr_r[324] *= scale_r;
      rr_r[326] *= scale_r;
      rr_r[329] *= scale_r;
      rr_r[332] *= scale_r;
      rr_r[333] *= scale_r;
      rr_r[334] *= scale_r;
      rr_r[335] *= scale_r;
      rr_r[337] *= scale_r;
      rr_r[339] *= scale_r;
      rr_r[344] *= scale_r;
      rr_r[345] *= scale_r;
      rr_r[346] *= scale_r;
      rr_r[347] *= scale_r;
      rr_r[348] *= scale_r;
      rr_r[353] *= scale_r;
    }
  }
  // Stiff species NC3H7O2
  {
    double ddot = rr_f[362] + rr_f[368];
    if ((ddot * dt * 9.750641978812205) > mole_frac[41])
    {
      double cdot = rr_r[362] + rr_r[368];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[41] * 0.01331770012907515;
      double c0 = mole_frac[41] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[41]- c0) / dt) 
        * recip_ddot; 
      rr_f[362] *= scale_r;
      rr_f[368] *= scale_r;
    }
  }
  // Stiff species IC3H7O2
  {
    double ddot = rr_f[363] + rr_f[367];
    if ((ddot * dt * 9.750641978812205) > mole_frac[42])
    {
      double cdot = rr_r[363] + rr_r[367];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[42] * 0.01331770012907515;
      double c0 = mole_frac[42] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[42]- c0) / dt) 
        * recip_ddot; 
      rr_f[363] *= scale_r;
      rr_f[367] *= scale_r;
    }
  }
  // Stiff species C4H8-1
  {
    double ddot = rr_f[375] + rr_f[376] + rr_f[377] + rr_f[378] + rr_f[379] + 
      rr_f[380] + rr_f[381] + rr_f[382] + rr_f[383] + rr_f[384] + rr_f[385] + 
      rr_f[386] + rr_f[387] + rr_r[371] + rr_r[372] + rr_r[373] + rr_r[374] + 
      rr_r[389] + rr_r[390] + rr_r[410] + rr_r[595]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[44])
    {
      double cdot = rr_f[371] + rr_f[372] + rr_f[373] + rr_f[374] + rr_f[389] + 
        rr_f[390] + rr_f[410] + rr_f[595] + rr_f[659] + rr_f[670] + rr_f[674] + 
        rr_r[375] + rr_r[376] + rr_r[377] + rr_r[378] + rr_r[379] + rr_r[380] + 
        rr_r[381] + rr_r[382] + rr_r[383] + rr_r[384] + rr_r[385] + rr_r[386] + 
        rr_r[387]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[44] * 0.01782265601774851;
      double c0 = mole_frac[44] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[44]- c0) / dt) 
        * recip_ddot; 
      rr_f[375] *= scale_r;
      rr_f[376] *= scale_r;
      rr_f[377] *= scale_r;
      rr_f[378] *= scale_r;
      rr_f[379] *= scale_r;
      rr_f[380] *= scale_r;
      rr_f[381] *= scale_r;
      rr_f[382] *= scale_r;
      rr_f[383] *= scale_r;
      rr_f[384] *= scale_r;
      rr_f[385] *= scale_r;
      rr_f[386] *= scale_r;
      rr_f[387] *= scale_r;
      rr_r[371] *= scale_r;
      rr_r[372] *= scale_r;
      rr_r[373] *= scale_r;
      rr_r[374] *= scale_r;
      rr_r[389] *= scale_r;
      rr_r[390] *= scale_r;
      rr_r[410] *= scale_r;
      rr_r[595] *= scale_r;
    }
  }
  // Stiff species SC4H9
  {
    double ddot = rr_f[370] + rr_f[372] + rr_f[374];
    if ((ddot * dt * 9.750641978812205) > mole_frac[45])
    {
      double cdot = rr_r[370] + rr_r[372] + rr_r[374];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[45] * 0.01750812771058645;
      double c0 = mole_frac[45] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[45]- c0) / dt) 
        * recip_ddot; 
      rr_f[370] *= scale_r;
      rr_f[372] *= scale_r;
      rr_f[374] *= scale_r;
    }
  }
  // Stiff species C4H71-3
  {
    double ddot = rr_f[388] + rr_f[389] + rr_f[390] + rr_f[391] + rr_f[392] + 
      rr_f[393] + rr_f[394] + rr_f[395] + rr_f[396] + rr_f[397] + rr_f[398] + 
      rr_f[399] + rr_f[438] + rr_r[377] + rr_r[378] + rr_r[379] + rr_r[380] + 
      rr_r[381] + rr_r[382] + rr_r[383] + rr_r[384] + rr_r[385] + rr_r[386] + 
      rr_r[386] + rr_r[387] + rr_r[632]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[46])
    {
      double cdot = rr_f[377] + rr_f[378] + rr_f[379] + rr_f[380] + rr_f[381] + 
        rr_f[382] + rr_f[383] + rr_f[384] + rr_f[385] + rr_f[386] + rr_f[386] + 
        rr_f[387] + rr_f[632] + rr_r[388] + rr_r[389] + rr_r[390] + rr_r[391] + 
        rr_r[392] + rr_r[393] + rr_r[394] + rr_r[395] + rr_r[396] + rr_r[397] + 
        rr_r[398] + rr_r[399] + rr_r[438]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[46] * 0.01814869186951308;
      double c0 = mole_frac[46] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[46]- c0) / dt) 
        * recip_ddot; 
      rr_f[388] *= scale_r;
      rr_f[389] *= scale_r;
      rr_f[390] *= scale_r;
      rr_f[391] *= scale_r;
      rr_f[392] *= scale_r;
      rr_f[393] *= scale_r;
      rr_f[394] *= scale_r;
      rr_f[395] *= scale_r;
      rr_f[396] *= scale_r;
      rr_f[397] *= scale_r;
      rr_f[398] *= scale_r;
      rr_f[399] *= scale_r;
      rr_f[438] *= scale_r;
      rr_r[377] *= scale_r;
      rr_r[378] *= scale_r;
      rr_r[379] *= scale_r;
      rr_r[380] *= scale_r;
      rr_r[381] *= scale_r;
      rr_r[382] *= scale_r;
      rr_r[383] *= scale_r;
      rr_r[384] *= scale_r;
      rr_r[385] *= scale_r;
      rr_r[386] *= scale_r;
      rr_r[386] *= scale_r;
      rr_r[387] *= scale_r;
      rr_r[632] *= scale_r;
    }
  }
  // Stiff species C4H6
  {
    double ddot = rr_f[386] + rr_f[402] + rr_f[403] + rr_f[404] + rr_f[405] + 
      rr_f[406] + rr_f[407] + rr_f[525] + rr_r[388] + rr_r[394] + rr_r[395] + 
      rr_r[396] + rr_r[397] + rr_r[398] + rr_r[408]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[47])
    {
      double cdot = rr_f[388] + rr_f[394] + rr_f[395] + rr_f[396] + rr_f[397] + 
        rr_f[398] + rr_f[408] + rr_r[386] + rr_r[402] + rr_r[403] + rr_r[404] + 
        rr_r[405] + rr_r[406] + rr_r[407] + rr_r[525]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[47] * 0.01848687856819865;
      double c0 = mole_frac[47] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[47]- c0) / dt) 
        * recip_ddot; 
      rr_f[386] *= scale_r;
      rr_f[402] *= scale_r;
      rr_f[403] *= scale_r;
      rr_f[404] *= scale_r;
      rr_f[405] *= scale_r;
      rr_f[406] *= scale_r;
      rr_f[407] *= scale_r;
      rr_f[525] *= scale_r;
      rr_r[388] *= scale_r;
      rr_r[394] *= scale_r;
      rr_r[395] *= scale_r;
      rr_r[396] *= scale_r;
      rr_r[397] *= scale_r;
      rr_r[398] *= scale_r;
      rr_r[408] *= scale_r;
    }
  }
  // Stiff species PC4H9O2
  {
    double ddot = rr_f[409] + rr_f[410];
    if ((ddot * dt * 9.750641978812205) > mole_frac[48])
    {
      double cdot = rr_r[409] + rr_r[410];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[48] * 0.01122143905305418;
      double c0 = mole_frac[48] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[48]- c0) / dt) 
        * recip_ddot; 
      rr_f[409] *= scale_r;
      rr_f[410] *= scale_r;
    }
  }
  // Stiff species C2H5COCH2
  {
    double ddot = rr_f[411];
    if ((ddot * dt * 9.750641978812205) > mole_frac[49])
    {
      double cdot = rr_f[789] + rr_r[411];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[49] * 0.01406473915042506;
      double c0 = mole_frac[49] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[49]- c0) / dt) 
        * recip_ddot; 
      rr_f[411] *= scale_r;
    }
  }
  // Stiff species IC4H8
  {
    double ddot = rr_f[452] + rr_f[453] + rr_f[454] + rr_f[455] + rr_f[456] + 
      rr_f[457] + rr_f[458] + rr_f[459] + rr_f[460] + rr_f[461] + rr_f[462] + 
      rr_f[463] + rr_f[464] + rr_f[465] + rr_f[466] + rr_f[497] + rr_f[747] + 
      rr_f[822] + rr_r[423] + rr_r[425] + rr_r[426] + rr_r[427] + rr_r[446] + 
      rr_r[447] + rr_r[686] + rr_r[725]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[51])
    {
      double cdot = rr_f[423] + rr_f[425] + rr_f[426] + rr_f[427] + rr_f[446] + 
        rr_f[447] + rr_f[686] + rr_f[694] + rr_f[725] + rr_f[847] + rr_r[452] + 
        rr_r[453] + rr_r[454] + rr_r[455] + rr_r[457] + rr_r[458] + rr_r[459] + 
        rr_r[460] + rr_r[461] + rr_r[462] + rr_r[463] + rr_r[464] + rr_r[465] + 
        rr_r[466] + rr_r[497] + rr_r[747] + rr_r[822]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[51] * 0.01782265601774851;
      double c0 = mole_frac[51] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[51]- c0) / dt) 
        * recip_ddot; 
      rr_f[452] *= scale_r;
      rr_f[453] *= scale_r;
      rr_f[454] *= scale_r;
      rr_f[455] *= scale_r;
      rr_f[456] *= scale_r;
      rr_f[457] *= scale_r;
      rr_f[458] *= scale_r;
      rr_f[459] *= scale_r;
      rr_f[460] *= scale_r;
      rr_f[461] *= scale_r;
      rr_f[462] *= scale_r;
      rr_f[463] *= scale_r;
      rr_f[464] *= scale_r;
      rr_f[465] *= scale_r;
      rr_f[466] *= scale_r;
      rr_f[497] *= scale_r;
      rr_f[747] *= scale_r;
      rr_f[822] *= scale_r;
      rr_r[423] *= scale_r;
      rr_r[425] *= scale_r;
      rr_r[426] *= scale_r;
      rr_r[427] *= scale_r;
      rr_r[446] *= scale_r;
      rr_r[447] *= scale_r;
      rr_r[686] *= scale_r;
      rr_r[725] *= scale_r;
    }
  }
  // Stiff species IC4H7
  {
    double ddot = rr_f[439] + rr_f[467] + rr_f[468] + rr_f[469] + rr_f[470] + 
      rr_f[471] + rr_f[472] + rr_f[473] + rr_r[453] + rr_r[455] + rr_r[457] + 
      rr_r[458] + rr_r[459] + rr_r[460] + rr_r[461] + rr_r[462] + rr_r[463] + 
      rr_r[464] + rr_r[466] + rr_r[497] + rr_r[691]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[52])
    {
      double cdot = rr_f[453] + rr_f[455] + rr_f[457] + rr_f[458] + rr_f[459] + 
        rr_f[460] + rr_f[461] + rr_f[462] + rr_f[463] + rr_f[464] + rr_f[466] + 
        rr_f[497] + rr_f[691] + rr_r[439] + rr_r[467] + rr_r[468] + rr_r[470] + 
        rr_r[471] + rr_r[472] + rr_r[473]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[52] * 0.01814869186951308;
      double c0 = mole_frac[52] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[52]- c0) / dt) 
        * recip_ddot; 
      rr_f[439] *= scale_r;
      rr_f[467] *= scale_r;
      rr_f[468] *= scale_r;
      rr_f[469] *= scale_r;
      rr_f[470] *= scale_r;
      rr_f[471] *= scale_r;
      rr_f[472] *= scale_r;
      rr_f[473] *= scale_r;
      rr_r[453] *= scale_r;
      rr_r[455] *= scale_r;
      rr_r[457] *= scale_r;
      rr_r[458] *= scale_r;
      rr_r[459] *= scale_r;
      rr_r[460] *= scale_r;
      rr_r[461] *= scale_r;
      rr_r[462] *= scale_r;
      rr_r[463] *= scale_r;
      rr_r[464] *= scale_r;
      rr_r[466] *= scale_r;
      rr_r[497] *= scale_r;
      rr_r[691] *= scale_r;
    }
  }
  // Stiff species TC4H9O2
  {
    double ddot = rr_f[429] + rr_f[430] + rr_f[431] + rr_f[432] + rr_f[432] + 
      rr_f[433] + rr_f[434] + rr_f[435] + rr_f[436] + rr_f[437] + rr_f[438] + 
      rr_f[439] + rr_f[447]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[53])
    {
      double cdot = rr_r[429] + rr_r[434] + rr_r[435] + rr_r[436] + rr_r[437] + 
        rr_r[438] + rr_r[439] + rr_r[447]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[53] * 0.01122143905305418;
      double c0 = mole_frac[53] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[53]- c0) / dt) 
        * recip_ddot; 
      rr_f[429] *= scale_r;
      rr_f[430] *= scale_r;
      rr_f[431] *= scale_r;
      rr_f[432] *= scale_r;
      rr_f[432] *= scale_r;
      rr_f[433] *= scale_r;
      rr_f[434] *= scale_r;
      rr_f[435] *= scale_r;
      rr_f[436] *= scale_r;
      rr_f[437] *= scale_r;
      rr_f[438] *= scale_r;
      rr_f[439] *= scale_r;
      rr_f[447] *= scale_r;
    }
  }
  // Stiff species IC4H9O2
  {
    double ddot = rr_f[428] + rr_f[445] + rr_f[446];
    if ((ddot * dt * 9.750641978812205) > mole_frac[54])
    {
      double cdot = rr_r[428] + rr_r[445] + rr_r[446];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[54] * 0.01122143905305418;
      double c0 = mole_frac[54] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[54]- c0) / dt) 
        * recip_ddot; 
      rr_f[428] *= scale_r;
      rr_f[445] *= scale_r;
      rr_f[446] *= scale_r;
    }
  }
  // Stiff species IC3H7CHO
  {
    double ddot = rr_f[441] + rr_f[442] + rr_f[443] + rr_f[444] + rr_r[494] + 
      rr_r[496] + rr_r[497] + rr_r[501] + rr_r[712]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[55])
    {
      double cdot = rr_f[494] + rr_f[496] + rr_f[497] + rr_f[501] + rr_f[712] + 
        rr_f[847] + rr_f[859] + rr_r[441] + rr_r[442] + rr_r[443] + rr_r[444]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[55] * 0.01386813291662368;
      double c0 = mole_frac[55] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[55]- c0) / dt) 
        * recip_ddot; 
      rr_f[441] *= scale_r;
      rr_f[442] *= scale_r;
      rr_f[443] *= scale_r;
      rr_f[444] *= scale_r;
      rr_r[494] *= scale_r;
      rr_r[496] *= scale_r;
      rr_r[497] *= scale_r;
      rr_r[501] *= scale_r;
      rr_r[712] *= scale_r;
    }
  }
  // Stiff species TC3H6CHO
  {
    double ddot = rr_f[491] + rr_f[493] + rr_f[494] + rr_f[496] + rr_f[497] + 
      rr_f[499] + rr_f[500] + rr_f[501] + rr_f[502] + rr_r[441] + rr_r[443] + 
      rr_r[444] + rr_r[498]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[56])
    {
      double cdot = rr_f[441] + rr_f[443] + rr_f[444] + rr_f[498] + rr_f[859] + 
        rr_r[491] + rr_r[493] + rr_r[494] + rr_r[496] + rr_r[497] + rr_r[499] + 
        rr_r[501] + rr_r[502]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[56] * 0.01406473915042506;
      double c0 = mole_frac[56] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[56]- c0) / dt) 
        * recip_ddot; 
      rr_f[491] *= scale_r;
      rr_f[493] *= scale_r;
      rr_f[494] *= scale_r;
      rr_f[496] *= scale_r;
      rr_f[497] *= scale_r;
      rr_f[499] *= scale_r;
      rr_f[500] *= scale_r;
      rr_f[501] *= scale_r;
      rr_f[502] *= scale_r;
      rr_r[441] *= scale_r;
      rr_r[443] *= scale_r;
      rr_r[444] *= scale_r;
      rr_r[498] *= scale_r;
    }
  }
  // Stiff species IC4H8OOH-IO2
  {
    double ddot = rr_f[448] + rr_f[449];
    if ((ddot * dt * 9.750641978812205) > mole_frac[57])
    {
      double cdot = rr_r[448] + rr_r[449];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[57] * 8.256688557625038e-03;
      double c0 = mole_frac[57] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[57]- c0) / dt) 
        * recip_ddot; 
      rr_f[448] *= scale_r;
      rr_f[449] *= scale_r;
    }
  }
  // Stiff species IC4KETII
  {
    double ddot = rr_f[450] + rr_r[449];
    if ((ddot * dt * 9.750641978812205) > mole_frac[58])
    {
      double cdot = rr_f[449];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[58] * 9.605542628629742e-03;
      double c0 = mole_frac[58] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[58]- c0) / dt) 
        * recip_ddot; 
      rr_f[450] *= scale_r;
      rr_r[449] *= scale_r;
    }
  }
  // Stiff species IC4H6OH
  {
    double ddot = rr_f[477] + rr_f[503] + rr_r[475];
    if ((ddot * dt * 9.750641978812205) > mole_frac[59])
    {
      double cdot = rr_f[475] + rr_r[477];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[59] * 0.01406473915042506;
      double c0 = mole_frac[59] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[59]- c0) / dt) 
        * recip_ddot; 
      rr_f[477] *= scale_r;
      rr_f[503] *= scale_r;
      rr_r[475] *= scale_r;
    }
  }
  // Stiff species IC3H5CO
  {
    double ddot = rr_f[490] + rr_r[484] + rr_r[485] + rr_r[486] + rr_r[487] + 
      rr_r[488] + rr_r[489]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[61])
    {
      double cdot = rr_f[484] + rr_f[485] + rr_f[486] + rr_f[487] + rr_f[488] + 
        rr_f[489] + rr_r[490]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[61] * 0.01447516315318269;
      double c0 = mole_frac[61] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[61]- c0) / dt) 
        * recip_ddot; 
      rr_f[490] *= scale_r;
      rr_r[484] *= scale_r;
      rr_r[485] *= scale_r;
      rr_r[486] *= scale_r;
      rr_r[487] *= scale_r;
      rr_r[488] *= scale_r;
      rr_r[489] *= scale_r;
    }
  }
  // Stiff species IC4H7OOH
  {
    double ddot = rr_f[495] + rr_r[763];
    if ((ddot * dt * 9.750641978812205) > mole_frac[62])
    {
      double cdot = rr_f[763] + rr_r[495];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[62] * 0.01134981538390296;
      double c0 = mole_frac[62] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[62]- c0) / dt) 
        * recip_ddot; 
      rr_f[495] *= scale_r;
      rr_r[763] *= scale_r;
    }
  }
  // Stiff species TC3H6O2CHO
  {
    double ddot = rr_f[498];
    if ((ddot * dt * 9.750641978812205) > mole_frac[63])
    {
      double cdot = rr_r[498];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[63] * 9.699453697669386e-03;
      double c0 = mole_frac[63] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[63]- c0) / dt) 
        * recip_ddot; 
      rr_f[498] *= scale_r;
    }
  }
  // Stiff species C5H10-1
  {
    double ddot = rr_f[513] + rr_f[514] + rr_f[507] + rr_f[509] + rr_f[510] + 
      rr_f[511] + rr_f[512] + rr_f[515] + rr_f[516] + rr_f[517] + rr_f[518] + 
      rr_f[519] + rr_f[520] + rr_f[521] + rr_f[522] + rr_f[523] + rr_f[524] + 
      rr_f[528] + rr_f[682] + rr_f[683] + rr_f[684] + rr_r[508] + rr_r[598]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[64])
    {
      double cdot = rr_f[508] + rr_f[598] + rr_f[629] + rr_f[630] + rr_f[658] + 
        rr_f[669] + rr_f[673] + rr_r[513] + rr_r[514] + rr_r[507] + rr_r[509] + 
        rr_r[510] + rr_r[511] + rr_r[512] + rr_r[515] + rr_r[516] + rr_r[517] + 
        rr_r[518] + rr_r[519] + rr_r[520] + rr_r[521] + rr_r[522] + rr_r[523] + 
        rr_r[524] + rr_r[528] + rr_r[682]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[64] * 0.01425812481419881;
      double c0 = mole_frac[64] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[64]- c0) / dt) 
        * recip_ddot; 
      rr_f[513] *= scale_r;
      rr_f[514] *= scale_r;
      rr_f[507] *= scale_r;
      rr_f[509] *= scale_r;
      rr_f[510] *= scale_r;
      rr_f[511] *= scale_r;
      rr_f[512] *= scale_r;
      rr_f[515] *= scale_r;
      rr_f[516] *= scale_r;
      rr_f[517] *= scale_r;
      rr_f[518] *= scale_r;
      rr_f[519] *= scale_r;
      rr_f[520] *= scale_r;
      rr_f[521] *= scale_r;
      rr_f[522] *= scale_r;
      rr_f[523] *= scale_r;
      rr_f[524] *= scale_r;
      rr_f[528] *= scale_r;
      rr_f[682] *= scale_r;
      rr_f[683] *= scale_r;
      rr_f[684] *= scale_r;
      rr_r[508] *= scale_r;
      rr_r[598] *= scale_r;
    }
  }
  // Stiff species C5H91-3
  {
    double ddot = rr_f[508] + rr_r[513] + rr_r[509] + rr_r[511] + rr_r[515] + 
      rr_r[517] + rr_r[519] + rr_r[521] + rr_r[523] + rr_r[525] + rr_r[633]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[65])
    {
      double cdot = rr_f[513] + rr_f[509] + rr_f[511] + rr_f[515] + rr_f[517] + 
        rr_f[519] + rr_f[521] + rr_f[523] + rr_f[525] + rr_f[633] + rr_r[508]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[65] * 0.01446602711396394;
      double c0 = mole_frac[65] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[65]- c0) / dt) 
        * recip_ddot; 
      rr_f[508] *= scale_r;
      rr_r[513] *= scale_r;
      rr_r[509] *= scale_r;
      rr_r[511] *= scale_r;
      rr_r[515] *= scale_r;
      rr_r[517] *= scale_r;
      rr_r[519] *= scale_r;
      rr_r[521] *= scale_r;
      rr_r[523] *= scale_r;
      rr_r[525] *= scale_r;
      rr_r[633] *= scale_r;
    }
  }
  // Stiff species C5H91-4
  {
    double ddot = rr_r[514] + rr_r[510] + rr_r[512] + rr_r[516] + rr_r[518] + 
      rr_r[520] + rr_r[522] + rr_r[524] + rr_r[526]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[66])
    {
      double cdot = rr_f[514] + rr_f[510] + rr_f[512] + rr_f[516] + rr_f[518] + 
        rr_f[520] + rr_f[522] + rr_f[524] + rr_f[526]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[66] * 0.01446602711396394;
      double c0 = mole_frac[66] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[66]- c0) / dt) 
        * recip_ddot; 
      rr_r[514] *= scale_r;
      rr_r[510] *= scale_r;
      rr_r[512] *= scale_r;
      rr_r[516] *= scale_r;
      rr_r[518] *= scale_r;
      rr_r[520] *= scale_r;
      rr_r[522] *= scale_r;
      rr_r[524] *= scale_r;
      rr_r[526] *= scale_r;
    }
  }
  // Stiff species C6H13O2-1
  {
    double ddot = rr_f[529];
    if ((ddot * dt * 9.750641978812205) > mole_frac[67])
    {
      double cdot = rr_r[529];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[67] * 8.534658094342281e-03;
      double c0 = mole_frac[67] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[67]- c0) / dt) 
        * recip_ddot; 
      rr_f[529] *= scale_r;
    }
  }
  // Stiff species NC7H16
  {
    double ddot = rr_f[539] + rr_f[540] + rr_f[541] + rr_f[542] + rr_f[543] + 
      rr_f[544] + rr_f[545] + rr_f[546] + rr_f[547] + rr_f[548] + rr_f[549] + 
      rr_f[550] + rr_f[551] + rr_f[552] + rr_f[553] + rr_f[554] + rr_f[555] + 
      rr_f[556] + rr_f[557] + rr_f[558] + rr_f[559] + rr_f[560] + rr_f[561] + 
      rr_f[562] + rr_f[563] + rr_f[564] + rr_f[565] + rr_f[566] + rr_f[567] + 
      rr_f[568] + rr_f[569] + rr_f[570] + rr_f[571] + rr_f[572] + rr_f[573] + 
      rr_f[574] + rr_f[575] + rr_f[576] + rr_f[577] + rr_f[578] + rr_f[579] + 
      rr_f[580] + rr_f[581] + rr_f[582] + rr_f[583] + rr_f[584] + rr_f[585] + 
      rr_f[586] + rr_f[587] + rr_f[588] + rr_f[589] + rr_f[590] + rr_r[585] + 
      rr_r[586] + rr_r[587] + rr_r[588] + rr_r[589] + rr_r[590]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[69])
    {
      double cdot = rr_f[585] + rr_f[586] + rr_f[587] + rr_f[588] + rr_f[589] + 
        rr_f[590] + rr_r[539] + rr_r[540] + rr_r[541] + rr_r[542] + rr_r[543] + 
        rr_r[544] + rr_r[545] + rr_r[546] + rr_r[547] + rr_r[548] + rr_r[549] + 
        rr_r[550] + rr_r[551] + rr_r[552] + rr_r[553] + rr_r[554] + rr_r[555] + 
        rr_r[556] + rr_r[557] + rr_r[558] + rr_r[559] + rr_r[560] + rr_r[561] + 
        rr_r[562] + rr_r[563] + rr_r[564] + rr_r[565] + rr_r[566] + rr_r[567] + 
        rr_r[568] + rr_r[569] + rr_r[570] + rr_r[571] + rr_r[572] + rr_r[573] + 
        rr_r[574] + rr_r[575] + rr_r[576] + rr_r[577] + rr_r[578] + rr_r[579] + 
        rr_r[580] + rr_r[581] + rr_r[582] + rr_r[583] + rr_r[584] + rr_r[585] + 
        rr_r[586] + rr_r[587] + rr_r[588] + rr_r[589] + rr_r[590]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[69] * 9.979485172331238e-03;
      double c0 = mole_frac[69] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[69]- c0) / dt) 
        * recip_ddot; 
      rr_f[539] *= scale_r;
      rr_f[540] *= scale_r;
      rr_f[541] *= scale_r;
      rr_f[542] *= scale_r;
      rr_f[543] *= scale_r;
      rr_f[544] *= scale_r;
      rr_f[545] *= scale_r;
      rr_f[546] *= scale_r;
      rr_f[547] *= scale_r;
      rr_f[548] *= scale_r;
      rr_f[549] *= scale_r;
      rr_f[550] *= scale_r;
      rr_f[551] *= scale_r;
      rr_f[552] *= scale_r;
      rr_f[553] *= scale_r;
      rr_f[554] *= scale_r;
      rr_f[555] *= scale_r;
      rr_f[556] *= scale_r;
      rr_f[557] *= scale_r;
      rr_f[558] *= scale_r;
      rr_f[559] *= scale_r;
      rr_f[560] *= scale_r;
      rr_f[561] *= scale_r;
      rr_f[562] *= scale_r;
      rr_f[563] *= scale_r;
      rr_f[564] *= scale_r;
      rr_f[565] *= scale_r;
      rr_f[566] *= scale_r;
      rr_f[567] *= scale_r;
      rr_f[568] *= scale_r;
      rr_f[569] *= scale_r;
      rr_f[570] *= scale_r;
      rr_f[571] *= scale_r;
      rr_f[572] *= scale_r;
      rr_f[573] *= scale_r;
      rr_f[574] *= scale_r;
      rr_f[575] *= scale_r;
      rr_f[576] *= scale_r;
      rr_f[577] *= scale_r;
      rr_f[578] *= scale_r;
      rr_f[579] *= scale_r;
      rr_f[580] *= scale_r;
      rr_f[581] *= scale_r;
      rr_f[582] *= scale_r;
      rr_f[583] *= scale_r;
      rr_f[584] *= scale_r;
      rr_f[585] *= scale_r;
      rr_f[586] *= scale_r;
      rr_f[587] *= scale_r;
      rr_f[588] *= scale_r;
      rr_f[589] *= scale_r;
      rr_f[590] *= scale_r;
      rr_r[585] *= scale_r;
      rr_r[586] *= scale_r;
      rr_r[587] *= scale_r;
      rr_r[588] *= scale_r;
      rr_r[589] *= scale_r;
      rr_r[590] *= scale_r;
    }
  }
  // Stiff species C7H14-1
  {
    double ddot = rr_f[626] + rr_f[631] + rr_f[651] + rr_r[591] + rr_r[593] + 
      rr_r[600] + rr_r[601] + rr_r[638] + rr_r[639]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[70])
    {
      double cdot = rr_f[591] + rr_f[593] + rr_f[600] + rr_f[601] + rr_f[638] + 
        rr_f[639] + rr_r[631] + rr_r[651]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[70] * 0.01018437486728486;
      double c0 = mole_frac[70] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[70]- c0) / dt) 
        * recip_ddot; 
      rr_f[626] *= scale_r;
      rr_f[631] *= scale_r;
      rr_f[651] *= scale_r;
      rr_r[591] *= scale_r;
      rr_r[593] *= scale_r;
      rr_r[600] *= scale_r;
      rr_r[601] *= scale_r;
      rr_r[638] *= scale_r;
      rr_r[639] *= scale_r;
    }
  }
  // Stiff species C7H14-2
  {
    double ddot = rr_f[610] + rr_f[611] + rr_f[612] + rr_f[613] + rr_f[614] + 
      rr_f[615] + rr_f[627] + rr_f[629] + rr_f[632] + rr_f[652] + rr_f[772] + 
      rr_r[594] + rr_r[596] + rr_r[602] + rr_r[603] + rr_r[640] + rr_r[641]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[71])
    {
      double cdot = rr_f[594] + rr_f[596] + rr_f[602] + rr_f[603] + rr_f[640] + 
        rr_f[641] + rr_r[610] + rr_r[611] + rr_r[612] + rr_r[613] + rr_r[614] + 
        rr_r[615] + rr_r[632] + rr_r[652] + rr_r[772]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[71] * 0.01018437486728486;
      double c0 = mole_frac[71] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[71]- c0) / dt) 
        * recip_ddot; 
      rr_f[610] *= scale_r;
      rr_f[611] *= scale_r;
      rr_f[612] *= scale_r;
      rr_f[613] *= scale_r;
      rr_f[614] *= scale_r;
      rr_f[615] *= scale_r;
      rr_f[627] *= scale_r;
      rr_f[629] *= scale_r;
      rr_f[632] *= scale_r;
      rr_f[652] *= scale_r;
      rr_f[772] *= scale_r;
      rr_r[594] *= scale_r;
      rr_r[596] *= scale_r;
      rr_r[602] *= scale_r;
      rr_r[603] *= scale_r;
      rr_r[640] *= scale_r;
      rr_r[641] *= scale_r;
    }
  }
  // Stiff species C7H14-3
  {
    double ddot = rr_f[616] + rr_f[617] + rr_f[618] + rr_f[619] + rr_f[620] + 
      rr_f[621] + rr_f[628] + rr_f[630] + rr_f[633] + rr_f[653] + rr_f[773] + 
      rr_r[597] + rr_r[599] + rr_r[604] + rr_r[605] + rr_r[642] + rr_r[643]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[72])
    {
      double cdot = rr_f[597] + rr_f[599] + rr_f[604] + rr_f[605] + rr_f[642] + 
        rr_f[643] + rr_r[616] + rr_r[617] + rr_r[618] + rr_r[619] + rr_r[620] + 
        rr_r[621] + rr_r[633] + rr_r[653] + rr_r[773]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[72] * 0.01018437486728486;
      double c0 = mole_frac[72] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[72]- c0) / dt) 
        * recip_ddot; 
      rr_f[616] *= scale_r;
      rr_f[617] *= scale_r;
      rr_f[618] *= scale_r;
      rr_f[619] *= scale_r;
      rr_f[620] *= scale_r;
      rr_f[621] *= scale_r;
      rr_f[628] *= scale_r;
      rr_f[630] *= scale_r;
      rr_f[633] *= scale_r;
      rr_f[653] *= scale_r;
      rr_f[773] *= scale_r;
      rr_r[597] *= scale_r;
      rr_r[599] *= scale_r;
      rr_r[604] *= scale_r;
      rr_r[605] *= scale_r;
      rr_r[642] *= scale_r;
      rr_r[643] *= scale_r;
    }
  }
  // Stiff species C7H15O2-1
  {
    double ddot = rr_f[634] + rr_f[638] + rr_f[644] + rr_f[645];
    if ((ddot * dt * 9.750641978812205) > mole_frac[74])
    {
      double cdot = rr_r[634] + rr_r[638] + rr_r[644] + rr_r[645];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[74] * 7.622160364156333e-03;
      double c0 = mole_frac[74] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[74]- c0) / dt) 
        * recip_ddot; 
      rr_f[634] *= scale_r;
      rr_f[638] *= scale_r;
      rr_f[644] *= scale_r;
      rr_f[645] *= scale_r;
    }
  }
  // Stiff species C7H15O2-2
  {
    double ddot = rr_f[635] + rr_f[639] + rr_f[640] + rr_f[646] + rr_f[647];
    if ((ddot * dt * 9.750641978812205) > mole_frac[75])
    {
      double cdot = rr_r[635] + rr_r[639] + rr_r[640] + rr_r[646] + rr_r[647];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[75] * 7.622160364156333e-03;
      double c0 = mole_frac[75] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[75]- c0) / dt) 
        * recip_ddot; 
      rr_f[635] *= scale_r;
      rr_f[639] *= scale_r;
      rr_f[640] *= scale_r;
      rr_f[646] *= scale_r;
      rr_f[647] *= scale_r;
    }
  }
  // Stiff species C7H15O2-3
  {
    double ddot = rr_f[636] + rr_f[641] + rr_f[642] + rr_f[648] + rr_f[770] + 
      rr_f[771]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[76])
    {
      double cdot = rr_r[636] + rr_r[641] + rr_r[642] + rr_r[648] + rr_r[770] + 
        rr_r[771]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[76] * 7.622160364156333e-03;
      double c0 = mole_frac[76] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[76]- c0) / dt) 
        * recip_ddot; 
      rr_f[636] *= scale_r;
      rr_f[641] *= scale_r;
      rr_f[642] *= scale_r;
      rr_f[648] *= scale_r;
      rr_f[770] *= scale_r;
      rr_f[771] *= scale_r;
    }
  }
  // Stiff species C7H15O2-4
  {
    double ddot = rr_f[637] + rr_f[643] + rr_f[649] + rr_f[650];
    if ((ddot * dt * 9.750641978812205) > mole_frac[77])
    {
      double cdot = rr_r[637] + rr_r[643] + rr_r[649] + rr_r[650];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[77] * 7.622160364156333e-03;
      double c0 = mole_frac[77] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[77]- c0) / dt) 
        * recip_ddot; 
      rr_f[637] *= scale_r;
      rr_f[643] *= scale_r;
      rr_f[649] *= scale_r;
      rr_f[650] *= scale_r;
    }
  }
  // Stiff species C7H14OOH1-3O2
  {
    double ddot = rr_f[662] + rr_f[667];
    if ((ddot * dt * 9.750641978812205) > mole_frac[78])
    {
      double cdot = rr_r[662] + rr_r[667];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[78] * 6.127631204839358e-03;
      double c0 = mole_frac[78] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[78]- c0) / dt) 
        * recip_ddot; 
      rr_f[662] *= scale_r;
      rr_f[667] *= scale_r;
    }
  }
  // Stiff species C7H14OOH2-3O2
  {
    double ddot = rr_f[663] + rr_f[777];
    if ((ddot * dt * 9.750641978812205) > mole_frac[79])
    {
      double cdot = rr_r[663] + rr_r[777];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[79] * 6.127631204839358e-03;
      double c0 = mole_frac[79] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[79]- c0) / dt) 
        * recip_ddot; 
      rr_f[663] *= scale_r;
      rr_f[777] *= scale_r;
    }
  }
  // Stiff species C7H14OOH2-4O2
  {
    double ddot = rr_f[664] + rr_f[778];
    if ((ddot * dt * 9.750641978812205) > mole_frac[80])
    {
      double cdot = rr_r[664] + rr_r[778];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[80] * 6.127631204839358e-03;
      double c0 = mole_frac[80] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[80]- c0) / dt) 
        * recip_ddot; 
      rr_f[664] *= scale_r;
      rr_f[778] *= scale_r;
    }
  }
  // Stiff species C7H14OOH4-2O2
  {
    double ddot = rr_f[665] + rr_f[783];
    if ((ddot * dt * 9.750641978812205) > mole_frac[81])
    {
      double cdot = rr_r[665] + rr_r[783];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[81] * 6.127631204839358e-03;
      double c0 = mole_frac[81] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[81]- c0) / dt) 
        * recip_ddot; 
      rr_f[665] *= scale_r;
      rr_f[783] *= scale_r;
    }
  }
  // Stiff species C7H14OOH4-3O2
  {
    double ddot = rr_f[666] + rr_f[784];
    if ((ddot * dt * 9.750641978812205) > mole_frac[82])
    {
      double cdot = rr_r[666] + rr_r[784];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[82] * 6.127631204839358e-03;
      double c0 = mole_frac[82] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[82]- c0) / dt) 
        * recip_ddot; 
      rr_f[666] *= scale_r;
      rr_f[784] *= scale_r;
    }
  }
  // Stiff species NC7KET13
  {
    double ddot = rr_f[668] + rr_r[667];
    if ((ddot * dt * 9.750641978812205) > mole_frac[86])
    {
      double cdot = rr_f[667];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[86] * 6.840514699479429e-03;
      double c0 = mole_frac[86] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[86]- c0) / dt) 
        * recip_ddot; 
      rr_f[668] *= scale_r;
      rr_r[667] *= scale_r;
    }
  }
  // Stiff species C4H7OOH1-4
  {
    double ddot = rr_f[677] + rr_r[661];
    if ((ddot * dt * 9.750641978812205) > mole_frac[87])
    {
      double cdot = rr_f[661] + rr_r[677];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[87] * 0.01134981538390296;
      double c0 = mole_frac[87] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[87]- c0) / dt) 
        * recip_ddot; 
      rr_f[677] *= scale_r;
      rr_r[661] *= scale_r;
    }
  }
  // Stiff species CH3CHCHO
  {
    double ddot = rr_f[679] + rr_f[680] + rr_f[681];
    if ((ddot * dt * 9.750641978812205) > mole_frac[88])
    {
      double cdot = rr_f[768] + rr_r[679] + rr_r[680] + rr_r[681];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[88] * 0.01752151203640269;
      double c0 = mole_frac[88] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[88]- c0) / dt) 
        * recip_ddot; 
      rr_f[679] *= scale_r;
      rr_f[680] *= scale_r;
      rr_f[681] *= scale_r;
    }
  }
  // Stiff species IC4H7-I1
  {
    double ddot = rr_f[685] + rr_r[713];
    if ((ddot * dt * 9.750641978812205) > mole_frac[89])
    {
      double cdot = rr_f[713] + rr_r[685];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[89] * 0.01814869186951308;
      double c0 = mole_frac[89] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[89]- c0) / dt) 
        * recip_ddot; 
      rr_f[685] *= scale_r;
      rr_r[713] *= scale_r;
    }
  }
  // Stiff species XC7H14
  {
    double ddot = rr_f[691] + rr_f[692] + rr_f[695] + rr_f[696] + rr_f[697] + 
      rr_f[698] + rr_f[699] + rr_f[700] + rr_f[821] + rr_r[687] + rr_r[689] + 
      rr_r[715]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[90])
    {
      double cdot = rr_f[687] + rr_f[689] + rr_f[715] + rr_f[762] + rr_r[691] + 
        rr_r[695] + rr_r[696] + rr_r[697] + rr_r[698] + rr_r[699] + rr_r[700] + 
        rr_r[821]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[90] * 0.01018437486728486;
      double c0 = mole_frac[90] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[90]- c0) / dt) 
        * recip_ddot; 
      rr_f[691] *= scale_r;
      rr_f[692] *= scale_r;
      rr_f[695] *= scale_r;
      rr_f[696] *= scale_r;
      rr_f[697] *= scale_r;
      rr_f[698] *= scale_r;
      rr_f[699] *= scale_r;
      rr_f[700] *= scale_r;
      rr_f[821] *= scale_r;
      rr_r[687] *= scale_r;
      rr_r[689] *= scale_r;
      rr_r[715] *= scale_r;
    }
  }
  // Stiff species YC7H14
  {
    double ddot = rr_f[693] + rr_f[694] + rr_f[701] + rr_f[702] + rr_f[703] + 
      rr_f[704] + rr_f[823] + rr_r[688] + rr_r[690] + rr_r[716]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[91])
    {
      double cdot = rr_f[688] + rr_f[690] + rr_f[716] + rr_f[764] + rr_f[766] + 
        rr_f[846] + rr_r[701] + rr_r[702] + rr_r[703] + rr_r[704] + rr_r[823]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[91] * 0.01018437486728486;
      double c0 = mole_frac[91] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[91]- c0) / dt) 
        * recip_ddot; 
      rr_f[693] *= scale_r;
      rr_f[694] *= scale_r;
      rr_f[701] *= scale_r;
      rr_f[702] *= scale_r;
      rr_f[703] *= scale_r;
      rr_f[704] *= scale_r;
      rr_f[823] *= scale_r;
      rr_r[688] *= scale_r;
      rr_r[690] *= scale_r;
      rr_r[716] *= scale_r;
    }
  }
  // Stiff species XC7H13-Z
  {
    double ddot = rr_f[705] + rr_f[706] + rr_f[707] + rr_f[708] + rr_r[695] + 
      rr_r[696] + rr_r[697] + rr_r[698] + rr_r[699] + rr_r[700] + rr_r[701] + 
      rr_r[703]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[92])
    {
      double cdot = rr_f[695] + rr_f[696] + rr_f[697] + rr_f[698] + rr_f[699] + 
        rr_f[700] + rr_f[701] + rr_f[703] + rr_r[705] + rr_r[706] + rr_r[707] + 
        rr_r[708]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[92] * 0.01029000739439931;
      double c0 = mole_frac[92] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[92]- c0) / dt) 
        * recip_ddot; 
      rr_f[705] *= scale_r;
      rr_f[706] *= scale_r;
      rr_f[707] *= scale_r;
      rr_f[708] *= scale_r;
      rr_r[695] *= scale_r;
      rr_r[696] *= scale_r;
      rr_r[697] *= scale_r;
      rr_r[698] *= scale_r;
      rr_r[699] *= scale_r;
      rr_r[700] *= scale_r;
      rr_r[701] *= scale_r;
      rr_r[703] *= scale_r;
    }
  }
  // Stiff species YC7H15O2
  {
    double ddot = rr_f[714] + rr_f[715] + rr_f[716];
    if ((ddot * dt * 9.750641978812205) > mole_frac[94])
    {
      double cdot = rr_r[714] + rr_r[715] + rr_r[716];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[94] * 7.622160364156333e-03;
      double c0 = mole_frac[94] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[94]- c0) / dt) 
        * recip_ddot; 
      rr_f[714] *= scale_r;
      rr_f[715] *= scale_r;
      rr_f[716] *= scale_r;
    }
  }
  // Stiff species ACC6H10
  {
    double ddot = rr_f[717] + rr_f[718] + rr_f[719] + rr_f[720] + rr_f[721] + 
      rr_f[722] + rr_r[705]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[95])
    {
      double cdot = rr_f[705] + rr_r[717] + rr_r[718] + rr_r[719] + rr_r[720] + 
        rr_r[721] + rr_r[722]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[95] * 0.01217335836175812;
      double c0 = mole_frac[95] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[95]- c0) / dt) 
        * recip_ddot; 
      rr_f[717] *= scale_r;
      rr_f[718] *= scale_r;
      rr_f[719] *= scale_r;
      rr_f[720] *= scale_r;
      rr_f[721] *= scale_r;
      rr_f[722] *= scale_r;
      rr_r[705] *= scale_r;
    }
  }
  // Stiff species ACC6H9-A
  {
    double ddot = rr_f[723] + rr_r[717] + rr_r[719] + rr_r[721];
    if ((ddot * dt * 9.750641978812205) > mole_frac[96])
    {
      double cdot = rr_f[717] + rr_f[719] + rr_f[721] + rr_r[723];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[96] * 0.01232458571213243;
      double c0 = mole_frac[96] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[96]- c0) / dt) 
        * recip_ddot; 
      rr_f[723] *= scale_r;
      rr_r[717] *= scale_r;
      rr_r[719] *= scale_r;
      rr_r[721] *= scale_r;
    }
  }
  // Stiff species NEOC5H11
  {
    double ddot = rr_f[725] + rr_f[824] + rr_r[737];
    if ((ddot * dt * 9.750641978812205) > mole_frac[98])
    {
      double cdot = rr_f[737] + rr_f[752] + rr_r[725] + rr_r[824];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[98] * 0.01405611369259448;
      double c0 = mole_frac[98] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[98]- c0) / dt) 
        * recip_ddot; 
      rr_f[725] *= scale_r;
      rr_f[824] *= scale_r;
      rr_r[737] *= scale_r;
    }
  }
  // Stiff species IC8H18
  {
    double ddot = rr_f[733] + rr_f[734] + rr_f[735] + rr_f[736] + rr_f[737] + 
      rr_f[738] + rr_f[739] + rr_f[740] + rr_f[741] + rr_f[742] + rr_f[743] + 
      rr_f[744] + rr_f[745] + rr_f[746] + rr_f[754] + rr_f[791] + rr_f[792] + 
      rr_f[793] + rr_f[794] + rr_f[795] + rr_f[796] + rr_f[797] + rr_f[798] + 
      rr_f[799] + rr_f[800] + rr_f[801] + rr_f[802] + rr_f[803] + rr_f[804] + 
      rr_f[805] + rr_f[806] + rr_f[807] + rr_f[808] + rr_f[809] + rr_f[810] + 
      rr_f[811] + rr_f[812] + rr_f[813] + rr_f[814] + rr_f[815] + rr_f[816] + 
      rr_f[817] + rr_f[818] + rr_f[819] + rr_f[820] + rr_f[829] + rr_f[830] + 
      rr_f[831]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[100])
    {
      double cdot = rr_r[733] + rr_r[734] + rr_r[735] + rr_r[736] + rr_r[737] + 
        rr_r[738] + rr_r[739] + rr_r[740] + rr_r[741] + rr_r[742] + rr_r[743] + 
        rr_r[744] + rr_r[745] + rr_r[746] + rr_r[754] + rr_r[791] + rr_r[792] + 
        rr_r[793] + rr_r[794] + rr_r[795] + rr_r[796] + rr_r[797] + rr_r[798] + 
        rr_r[799] + rr_r[800] + rr_r[801] + rr_r[802] + rr_r[803] + rr_r[804] + 
        rr_r[805] + rr_r[806] + rr_r[807] + rr_r[808] + rr_r[809] + rr_r[810] + 
        rr_r[811] + rr_r[812] + rr_r[813] + rr_r[814] + rr_r[815] + rr_r[816] + 
        rr_r[817] + rr_r[818] + rr_r[819] + rr_r[820] + rr_r[829] + rr_r[830] + 
        rr_r[831]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[100] * 8.754063855293223e-03;
      double c0 = mole_frac[100] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[100]- c0) / 
        dt) * recip_ddot; 
      rr_f[733] *= scale_r;
      rr_f[734] *= scale_r;
      rr_f[735] *= scale_r;
      rr_f[736] *= scale_r;
      rr_f[737] *= scale_r;
      rr_f[738] *= scale_r;
      rr_f[739] *= scale_r;
      rr_f[740] *= scale_r;
      rr_f[741] *= scale_r;
      rr_f[742] *= scale_r;
      rr_f[743] *= scale_r;
      rr_f[744] *= scale_r;
      rr_f[745] *= scale_r;
      rr_f[746] *= scale_r;
      rr_f[754] *= scale_r;
      rr_f[791] *= scale_r;
      rr_f[792] *= scale_r;
      rr_f[793] *= scale_r;
      rr_f[794] *= scale_r;
      rr_f[795] *= scale_r;
      rr_f[796] *= scale_r;
      rr_f[797] *= scale_r;
      rr_f[798] *= scale_r;
      rr_f[799] *= scale_r;
      rr_f[800] *= scale_r;
      rr_f[801] *= scale_r;
      rr_f[802] *= scale_r;
      rr_f[803] *= scale_r;
      rr_f[804] *= scale_r;
      rr_f[805] *= scale_r;
      rr_f[806] *= scale_r;
      rr_f[807] *= scale_r;
      rr_f[808] *= scale_r;
      rr_f[809] *= scale_r;
      rr_f[810] *= scale_r;
      rr_f[811] *= scale_r;
      rr_f[812] *= scale_r;
      rr_f[813] *= scale_r;
      rr_f[814] *= scale_r;
      rr_f[815] *= scale_r;
      rr_f[816] *= scale_r;
      rr_f[817] *= scale_r;
      rr_f[818] *= scale_r;
      rr_f[819] *= scale_r;
      rr_f[820] *= scale_r;
      rr_f[829] *= scale_r;
      rr_f[830] *= scale_r;
      rr_f[831] *= scale_r;
    }
  }
  // Stiff species BC8H17O2
  {
    double ddot = rr_f[756] + rr_f[757] + rr_f[833] + rr_f[837] + rr_f[838];
    if ((ddot * dt * 9.750641978812205) > mole_frac[103])
    {
      double cdot = rr_r[756] + rr_r[757] + rr_r[833] + rr_r[837] + rr_r[838];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[103] * 6.885938356115805e-03;
      double c0 = mole_frac[103] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[103]- c0) / 
        dt) * recip_ddot; 
      rr_f[756] *= scale_r;
      rr_f[757] *= scale_r;
      rr_f[833] *= scale_r;
      rr_f[837] *= scale_r;
      rr_f[838] *= scale_r;
    }
  }
  // Stiff species CC8H17O2
  {
    double ddot = rr_f[755] + rr_f[758] + rr_f[759];
    if ((ddot * dt * 9.750641978812205) > mole_frac[104])
    {
      double cdot = rr_r[755] + rr_r[758] + rr_r[759];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[104] * 6.885938356115805e-03;
      double c0 = mole_frac[104] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[104]- c0) / 
        dt) * recip_ddot; 
      rr_f[755] *= scale_r;
      rr_f[758] *= scale_r;
      rr_f[759] *= scale_r;
    }
  }
  // Stiff species IC8KETDB
  {
    double ddot = rr_f[768] + rr_r[858];
    if ((ddot * dt * 9.750641978812205) > mole_frac[107])
    {
      double cdot = rr_f[858];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[107] * 6.241615949376e-03;
      double c0 = mole_frac[107] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[107]- c0) / 
        dt) * recip_ddot; 
      rr_f[768] *= scale_r;
      rr_r[858] *= scale_r;
    }
  }
  // Stiff species iso002
  {
    double ddot = rr_f[850] + rr_f[851] + rr_f[852] + rr_f[855] + rr_f[856] + 
      rr_f[857]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[108])
    {
      double cdot = rr_r[850] + rr_r[851] + rr_r[852] + rr_r[855] + rr_r[856] + 
        rr_r[857]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[108] * 5.642631070843289e-03;
      double c0 = mole_frac[108] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[108]- c0) / 
        dt) * recip_ddot; 
      rr_f[850] *= scale_r;
      rr_f[851] *= scale_r;
      rr_f[852] *= scale_r;
      rr_f[855] *= scale_r;
      rr_f[856] *= scale_r;
      rr_f[857] *= scale_r;
    }
  }
  // Stiff species iso003
  {
    double ddot = rr_f[849] + rr_f[853] + rr_f[854] + rr_f[858];
    if ((ddot * dt * 9.750641978812205) > mole_frac[109])
    {
      double cdot = rr_r[849] + rr_r[853] + rr_r[854] + rr_r[858];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[109] * 5.642631070843289e-03;
      double c0 = mole_frac[109] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[109]- c0) / 
        dt) * recip_ddot; 
      rr_f[849] *= scale_r;
      rr_f[853] *= scale_r;
      rr_f[854] *= scale_r;
      rr_f[858] *= scale_r;
    }
  }
  // Stiff species iso004
  {
    double ddot = rr_f[832] + rr_f[834] + rr_f[835] + rr_f[836] + rr_f[839] + 
      rr_f[840] + rr_f[841]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[110])
    {
      double cdot = rr_r[832] + rr_r[834] + rr_r[835] + rr_r[836] + rr_r[839] + 
        rr_r[840] + rr_r[841]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[110] * 6.885938356115805e-03;
      double c0 = mole_frac[110] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[110]- c0) / 
        dt) * recip_ddot; 
      rr_f[832] *= scale_r;
      rr_f[834] *= scale_r;
      rr_f[835] *= scale_r;
      rr_f[836] *= scale_r;
      rr_f[839] *= scale_r;
      rr_f[840] *= scale_r;
      rr_f[841] *= scale_r;
    }
  }
  // Stiff species iso008
  {
    double ddot = rr_f[859] + rr_f[860] + rr_r[854] + rr_r[855] + rr_r[856] + 
      rr_r[857]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[111])
    {
      double cdot = rr_f[854] + rr_f[855] + rr_f[856] + rr_f[857];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[111] * 6.241615949376e-03;
      double c0 = mole_frac[111] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[111]- c0) / 
        dt) * recip_ddot; 
      rr_f[859] *= scale_r;
      rr_f[860] *= scale_r;
      rr_r[854] *= scale_r;
      rr_r[855] *= scale_r;
      rr_r[856] *= scale_r;
      rr_r[857] *= scale_r;
    }
  }
  // Stiff species iso010
  {
    double ddot = rr_f[774] + rr_f[775] + rr_f[780] + rr_f[781];
    if ((ddot * dt * 9.750641978812205) > mole_frac[112])
    {
      double cdot = rr_r[774] + rr_r[775] + rr_r[780] + rr_r[781];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[112] * 6.127631204839358e-03;
      double c0 = mole_frac[112] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[112]- c0) / 
        dt) * recip_ddot; 
      rr_f[774] *= scale_r;
      rr_f[775] *= scale_r;
      rr_f[780] *= scale_r;
      rr_f[781] *= scale_r;
    }
  }
  // Stiff species iso011
  {
    double ddot = rr_f[776] + rr_f[779] + rr_f[782];
    if ((ddot * dt * 9.750641978812205) > mole_frac[113])
    {
      double cdot = rr_r[776] + rr_r[779] + rr_r[782];
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[113] * 6.127631204839358e-03;
      double c0 = mole_frac[113] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[113]- c0) / 
        dt) * recip_ddot; 
      rr_f[776] *= scale_r;
      rr_f[779] *= scale_r;
      rr_f[782] *= scale_r;
    }
  }
  // Stiff species iso014
  {
    double ddot = rr_f[785] + rr_f[786] + rr_f[787] + rr_f[788] + rr_f[789] + 
      rr_f[790] + rr_r[777] + rr_r[778] + rr_r[779] + rr_r[780] + rr_r[781] + 
      rr_r[782] + rr_r[783] + rr_r[784]; 
    if ((ddot * dt * 9.750641978812205) > mole_frac[114])
    {
      double cdot = rr_f[777] + rr_f[778] + rr_f[779] + rr_f[780] + rr_f[781] + 
        rr_f[782] + rr_f[783] + rr_f[784]; 
      double recip_ddot = 1.0/ddot;
      double part_sum = cdot + diffusion[114] * 6.840514699479429e-03;
      double c0 = mole_frac[114] * part_sum * recip_ddot;
      double scale_r = (part_sum + 0.102557349779939 * (mole_frac[114]- c0) / 
        dt) * recip_ddot; 
      rr_f[785] *= scale_r;
      rr_f[786] *= scale_r;
      rr_f[787] *= scale_r;
      rr_f[788] *= scale_r;
      rr_f[789] *= scale_r;
      rr_f[790] *= scale_r;
      rr_r[777] *= scale_r;
      rr_r[778] *= scale_r;
      rr_r[779] *= scale_r;
      rr_r[780] *= scale_r;
      rr_r[781] *= scale_r;
      rr_r[782] *= scale_r;
      rr_r[783] *= scale_r;
      rr_r[784] *= scale_r;
    }
  }
  double wdot[116];
  double ropl[861];
  for (int i = 0; i < 861; i++)
  {
    ropl[i] = rr_f[i] - rr_r[i];
  }
  // 0. H
  wdot[0] = -ropl[0] + ropl[1] + ropl[2] + 2.0*ropl[4] + ropl[6] + ropl[7] - 
    ropl[8] - ropl[9] - ropl[10] - ropl[16] - ropl[17] + ropl[23] + ropl[25] - 
    ropl[27] + ropl[29] + ropl[33] - ropl[36] - ropl[39] + ropl[44] + ropl[48] - 
    ropl[51] + ropl[55] - ropl[60] - ropl[62] - ropl[128] - ropl[65] + ropl[75] 
    - ropl[76] - ropl[77] - ropl[86] - ropl[87] + ropl[94] + ropl[95] + ropl[99] 
    - ropl[110] + ropl[116] + ropl[117] + 2.0*ropl[119] + ropl[120] - ropl[122] 
    + 2.0*ropl[124] + ropl[125] + 2.0*ropl[126] + ropl[262] + ropl[265] - 
    ropl[129] - ropl[138] + ropl[139] + ropl[140] - ropl[143] - ropl[144] + 
    ropl[145] + ropl[150] - ropl[166] - ropl[176] + ropl[180] - ropl[183] - 
    ropl[184] - ropl[191] + ropl[192] - ropl[194] - ropl[196] + ropl[198] + 
    ropl[205] - ropl[206] - ropl[211] + ropl[215] + ropl[216] - ropl[220] - 
    ropl[235] - ropl[246] + ropl[258] - ropl[259] - ropl[532] + ropl[539] + 
    ropl[540] + ropl[269] + ropl[270] + ropl[271] + ropl[273] + 2.0*ropl[274] - 
    ropl[284] - ropl[285] - ropl[286] - ropl[287] + ropl[298] - ropl[301] + 
    ropl[312] - ropl[314] + ropl[317] + ropl[318] - ropl[322] + ropl[324] - 
    ropl[332] + ropl[335] + ropl[343] - ropl[345] + ropl[354] + ropl[355] - 
    ropl[356] + ropl[358] - ropl[360] + ropl[364] + ropl[371] + ropl[372] + 
    ropl[377] - ropl[379] + ropl[388] - ropl[396] + ropl[408] - ropl[414] + 
    ropl[423] + ropl[425] + ropl[441] + ropl[453] - ropl[454] - ropl[455] + 
    ropl[470] + ropl[476] - ropl[483] - ropl[489] + ropl[493] + ropl[494] - 
    ropl[508] - ropl[509] - ropl[510] + ropl[541] + ropl[542] - ropl[545] - 
    ropl[546] - ropl[547] - ropl[548] + ropl[591] + ropl[593] + ropl[594] + 
    ropl[596] + ropl[597] + ropl[599] - ropl[610] - ropl[616] + ropl[679] + 
    ropl[680] + ropl[681] + ropl[687] + ropl[688] - ropl[695] - ropl[701] - 
    ropl[702] + ropl[733] - ropl[738] + ropl[748] + ropl[749] + ropl[791] + 
    ropl[792] + ropl[793] - ropl[794] - ropl[795] - ropl[796] + ropl[825] + 
    ropl[826]; 
  // 1. H2
  wdot[1] = -ropl[1] - ropl[2] - ropl[4] + ropl[9] + ropl[17] + ropl[27] + 
    ropl[35] - ropl[37] + ropl[39] + ropl[46] + ropl[50] + ropl[51] + ropl[60] + 
    ropl[65] + ropl[76] + ropl[77] + ropl[87] + ropl[92] - ropl[117] + ropl[129] 
    - ropl[139] - ropl[140] + ropl[144] + ropl[166] + ropl[176] + ropl[184] + 
    ropl[190] + ropl[195] + ropl[196] + ropl[211] + ropl[220] + ropl[235] + 
    ropl[246] + ropl[532] + ropl[284] + ropl[285] + ropl[286] + ropl[301] + 
    ropl[314] + ropl[322] + ropl[332] + ropl[345] + ropl[356] + ropl[379] + 
    ropl[396] + ropl[414] + ropl[455] + ropl[483] + ropl[489] - ropl[494] + 
    ropl[509] + ropl[510] + ropl[545] + ropl[546] + ropl[547] + ropl[548] + 
    ropl[610] + ropl[616] - ropl[681] + ropl[695] + ropl[701] + ropl[702] + 
    ropl[738] + ropl[794] + ropl[795] + ropl[796]; 
  // 2. O
  wdot[2] = ropl[0] - ropl[1] - ropl[3] + 2.0*ropl[5] + ropl[6] - ropl[11] - 
    ropl[18] - ropl[21] + ropl[22] - ropl[28] - ropl[29] - ropl[40] - ropl[54] - 
    ropl[71] - ropl[78] - ropl[89] - ropl[99] + ropl[100] - ropl[111] - 
    ropl[119] + ropl[123] - ropl[126] - ropl[262] - ropl[263] - ropl[130] - 
    ropl[145] - ropl[167] - ropl[177] - ropl[185] - ropl[186] - ropl[192] - 
    ropl[197] - ropl[198] + ropl[209] - ropl[214] - ropl[215] - ropl[221] - 
    ropl[236] - ropl[247] - ropl[533] - ropl[272] - ropl[273] - ropl[274] - 
    ropl[275] - ropl[276] - ropl[277] + ropl[320] - ropl[330] - ropl[331] - 
    ropl[341] - ropl[342] - ropl[343] - ropl[344] - ropl[361] - ropl[391] - 
    ropl[406] - ropl[407] - ropl[415] - ropl[456] - ropl[457] - ropl[465] - 
    ropl[470] - ropl[481] - ropl[487] - ropl[511] - ropl[512] - ropl[549] - 
    ropl[550] - ropl[551] - ropl[552] - ropl[629] - ropl[630] - ropl[683] - 
    ropl[684] - ropl[694] - ropl[728] - ropl[739] - ropl[797] - ropl[798] - 
    ropl[799]; 
  // 3. O2
  wdot[3] = -ropl[0] - ropl[5] - ropl[8] + ropl[9] + ropl[11] + ropl[12] - 
    ropl[13] - ropl[14] - ropl[22] - ropl[26] - ropl[63] + ropl[32] - ropl[56] - 
    ropl[64] - ropl[81] + ropl[98] - ropl[100] - ropl[101] - ropl[102] + 
    ropl[107] + ropl[108] + ropl[109] + ropl[111] + ropl[112] - ropl[116] - 
    ropl[123] - ropl[124] - ropl[125] - ropl[266] - ropl[132] - ropl[148] + 
    ropl[151] + ropl[155] - ropl[158] - ropl[159] - ropl[160] - ropl[169] + 
    ropl[179] - ropl[181] - ropl[193] - ropl[201] - ropl[207] - ropl[208] - 
    ropl[209] - ropl[213] - ropl[224] + ropl[228] + ropl[231] - ropl[238] - 
    ropl[255] - ropl[260] - ropl[288] - ropl[289] - ropl[290] - ropl[307] - 
    ropl[308] - ropl[309] - ropl[310] - ropl[313] - ropl[319] - ropl[320] - 
    ropl[321] - ropl[326] - ropl[336] - ropl[337] - ropl[350] - ropl[351] - 
    ropl[358] + ropl[362] + ropl[363] - ropl[366] - ropl[373] - ropl[374] - 
    ropl[378] - ropl[395] + ropl[409] - ropl[412] - ropl[426] - ropl[427] + 
    ropl[428] + ropl[429] + ropl[430] + ropl[431] + ropl[432] + ropl[433] + 
    ropl[448] - ropl[460] - ropl[467] - ropl[468] - ropl[469] - ropl[478] - 
    ropl[488] + ropl[498] - ropl[499] - ropl[500] + ropl[501] - ropl[504] - 
    ropl[517] - ropl[518] + ropl[529] - ropl[530] - ropl[565] - ropl[566] - 
    ropl[567] - ropl[568] - ropl[600] - ropl[601] - ropl[602] - ropl[603] - 
    ropl[604] - ropl[605] + ropl[634] + ropl[635] + ropl[636] + ropl[637] + 
    ropl[662] + ropl[663] + ropl[664] + ropl[665] + ropl[666] - ropl[689] - 
    ropl[690] + ropl[714] - ropl[729] - ropl[744] - ropl[750] - ropl[751] + 
    ropl[755] + ropl[774] + ropl[775] + ropl[776] - ropl[812] - ropl[813] - 
    ropl[814] - ropl[828] + ropl[832] + ropl[833] + ropl[834] + ropl[849] + 
    ropl[850] + ropl[851] + ropl[852] + ropl[853]; 
  // 4. OH
  wdot[4] = ropl[0] + ropl[1] - ropl[2] + 2.0*ropl[3] - ropl[6] + ropl[7] + 
    2.0*ropl[10] + ropl[11] - ropl[12] + 2.0*ropl[15] + ropl[16] + ropl[18] - 
    ropl[19] - ropl[20] - ropl[23] + ropl[24] + ropl[28] - ropl[30] + ropl[33] - 
    ropl[38] + ropl[40] + ropl[43] + ropl[47] - ropl[48] + ropl[51] + ropl[52] + 
    ropl[53] + 2.0*ropl[54] - ropl[70] + ropl[71] + ropl[73] + ropl[74] + 
    ropl[78] - ropl[79] - ropl[80] - ropl[88] + ropl[89] - ropl[92] - ropl[93] - 
    ropl[94] - ropl[95] - ropl[96] + ropl[97] + ropl[101] + ropl[110] - 
    ropl[112] + ropl[113] + ropl[116] - ropl[120] + ropl[125] + ropl[130] - 
    ropl[131] + ropl[146] + ropl[157] + ropl[160] + ropl[161] + ropl[167] - 
    ropl[168] - ropl[173] - ropl[174] + ropl[177] + ropl[181] + ropl[186] - 
    ropl[187] - ropl[188] - ropl[190] + ropl[193] - ropl[199] - ropl[212] + 
    ropl[213] - ropl[216] - ropl[217] - ropl[219] + ropl[221] + ropl[232] + 
    ropl[236] - ropl[237] + ropl[247] - ropl[248] - ropl[513] - ropl[514] - 
    ropl[261] - ropl[531] + ropl[533] + ropl[275] + ropl[276] + ropl[277] - 
    ropl[278] - ropl[279] - ropl[280] + ropl[299] + ropl[309] + ropl[310] + 
    ropl[327] - ropl[328] - ropl[329] + ropl[336] + ropl[338] - ropl[339] - 
    ropl[340] + ropl[344] - ropl[349] + ropl[352] - ropl[357] - ropl[359] - 
    ropl[380] + ropl[392] - ropl[403] - ropl[404] - ropl[405] - ropl[413] + 
    ropl[415] + ropl[421] + ropl[433] - ropl[444] + ropl[449] + ropl[450] + 
    ropl[451] + ropl[457] - ropl[464] + ropl[467] + ropl[469] + ropl[473] + 
    ropl[481] - ropl[482] - ropl[484] + ropl[487] + ropl[491] + ropl[495] + 
    ropl[500] + ropl[503] + ropl[506] + ropl[511] + ropl[512] - ropl[528] + 
    ropl[549] + ropl[550] + ropl[551] + ropl[552] - ropl[553] - ropl[554] - 
    ropl[555] - ropl[556] - ropl[611] - ropl[617] + ropl[622] - ropl[626] - 
    ropl[627] - ropl[628] + ropl[654] + ropl[655] + ropl[656] + ropl[657] + 
    ropl[658] + ropl[659] + ropl[660] + ropl[667] + ropl[668] - ropl[669] - 
    ropl[670] - ropl[671] - ropl[672] + ropl[677] - ropl[682] - ropl[692] - 
    ropl[693] - ropl[696] - ropl[703] - ropl[704] + ropl[706] + ropl[709] - 
    ropl[717] - ropl[718] + ropl[724] + ropl[728] - ropl[730] + ropl[739] - 
    ropl[740] - ropl[752] - ropl[753] + ropl[762] - ropl[764] - ropl[765] + 
    ropl[768] + ropl[777] + ropl[778] + ropl[779] + ropl[780] + ropl[781] + 
    ropl[782] + ropl[783] + ropl[784] + ropl[785] + ropl[786] + ropl[787] + 
    ropl[788] + ropl[789] + ropl[790] + ropl[797] + ropl[798] + ropl[799] - 
    ropl[800] - ropl[801] - ropl[802] + ropl[842] + ropl[843] + ropl[844] + 
    ropl[845] + ropl[846] + ropl[847] + ropl[848] + ropl[854] + ropl[855] + 
    ropl[856] + ropl[857] + ropl[858] + ropl[859] + ropl[860]; 
  // 5. H2O
  wdot[5] = ropl[2] - ropl[3] - ropl[7] + ropl[12] + ropl[16] + ropl[19] + 
    ropl[20] + ropl[30] + ropl[38] + ropl[45] + ropl[48] + ropl[49] + ropl[70] + 
    ropl[79] + ropl[80] + ropl[88] + ropl[93] + ropl[96] + ropl[131] + ropl[168] 
    + ropl[174] + ropl[187] + ropl[199] + ropl[212] + ropl[219] + ropl[237] + 
    ropl[248] + ropl[513] + ropl[514] + ropl[261] + ropl[531] + ropl[278] + 
    ropl[279] + ropl[280] + ropl[329] + ropl[339] + ropl[349] + ropl[380] + 
    ropl[413] + ropl[444] + ropl[464] + ropl[482] + ropl[484] + ropl[553] + 
    ropl[554] + ropl[555] + ropl[556] + ropl[611] + ropl[617] + ropl[669] + 
    ropl[670] + ropl[671] + ropl[672] + ropl[696] + ropl[703] + ropl[704] + 
    ropl[717] + ropl[718] + ropl[730] + ropl[740] + ropl[764] + ropl[765] + 
    ropl[800] + ropl[801] + ropl[802]; 
  // 6. HO2
  wdot[6] = ropl[8] - ropl[9] - ropl[10] - ropl[11] - ropl[12] + 2.0*ropl[13] + 
    2.0*ropl[14] + ropl[17] + ropl[18] + ropl[19] + ropl[20] - ropl[24] + 
    ropl[26] + ropl[63] - ropl[32] - ropl[33] - ropl[42] - ropl[53] + ropl[56] - 
    ropl[61] + ropl[64] - ropl[66] - ropl[73] + ropl[81] - ropl[82] - ropl[90] - 
    ropl[97] - ropl[98] - ropl[107] + ropl[266] + ropl[132] - ropl[134] - 
    ropl[146] + ropl[148] - ropl[155] + ropl[158] + ropl[159] + ropl[162] + 
    ropl[169] - ropl[171] + ropl[201] + ropl[207] + ropl[224] - ropl[225] - 
    ropl[231] + ropl[238] - ropl[239] - ropl[250] + ropl[255] + ropl[260] - 
    ropl[534] - ropl[281] - ropl[282] - ropl[283] + ropl[288] + ropl[289] + 
    ropl[290] - ropl[299] + ropl[307] + ropl[319] + ropl[326] - ropl[327] + 
    ropl[337] - ropl[338] - ropl[352] - ropl[353] + ropl[366] + ropl[367] + 
    ropl[368] + ropl[373] + ropl[374] + ropl[378] - ropl[382] - ropl[392] + 
    ropl[395] + ropl[410] + ropl[412] - ropl[416] - ropl[421] + ropl[426] + 
    ropl[427] - ropl[433] - ropl[443] + ropl[446] + ropl[447] - ropl[459] + 
    ropl[460] - ropl[473] + ropl[478] - ropl[479] - ropl[485] + ropl[488] - 
    ropl[491] + ropl[499] - ropl[501] - ropl[503] + ropl[517] + ropl[518] - 
    ropl[519] - ropl[520] + ropl[530] - ropl[557] - ropl[558] - ropl[559] - 
    ropl[560] + ropl[565] + ropl[566] + ropl[567] + ropl[568] + ropl[600] + 
    ropl[601] + ropl[602] + ropl[603] + ropl[604] + ropl[605] - ropl[613] - 
    ropl[619] - ropl[622] + ropl[638] + ropl[639] + ropl[640] + ropl[641] + 
    ropl[642] + ropl[643] - ropl[651] - ropl[652] - ropl[653] - ropl[673] - 
    ropl[674] - ropl[675] - ropl[676] + ropl[689] + ropl[690] - ropl[698] - 
    ropl[706] - ropl[709] + ropl[715] + ropl[716] - ropl[719] - ropl[720] - 
    ropl[724] - ropl[726] + ropl[729] - ropl[742] + ropl[744] + ropl[750] + 
    ropl[751] + ropl[757] + ropl[758] + ropl[759] + ropl[760] + ropl[761] - 
    ropl[766] - ropl[767] - ropl[772] - ropl[773] - ropl[806] - ropl[807] - 
    ropl[808] + ropl[812] + ropl[813] + ropl[814] + ropl[828] + ropl[841]; 
  // 7. H2O2
  wdot[7] = -ropl[13] - ropl[14] - ropl[15] - ropl[16] - ropl[17] - ropl[18] - 
    ropl[19] - ropl[20] + ropl[42] + ropl[53] + ropl[61] + ropl[66] + ropl[82] + 
    ropl[90] + ropl[134] + ropl[171] + ropl[225] + ropl[239] + ropl[250] + 
    ropl[534] + ropl[281] + ropl[282] + ropl[283] + ropl[353] + ropl[382] + 
    ropl[416] + ropl[443] + ropl[459] + ropl[479] + ropl[485] + ropl[519] + 
    ropl[520] + ropl[557] + ropl[558] + ropl[559] + ropl[560] + ropl[613] + 
    ropl[619] + ropl[673] + ropl[674] + ropl[675] + ropl[676] + ropl[698] + 
    ropl[719] + ropl[720] + ropl[726] + ropl[742] + ropl[766] + ropl[767] + 
    ropl[806] + ropl[807] + ropl[808]; 
  // 8. CO
  wdot[8] = -ropl[21] - ropl[22] - ropl[23] - ropl[24] + ropl[25] + ropl[26] + 
    ropl[27] + ropl[28] + ropl[30] + ropl[31] - ropl[34] + 2.0*ropl[35] - 
    ropl[37] + ropl[45] + ropl[49] + ropl[51] + ropl[52] + ropl[53] + ropl[54] + 
    ropl[257] + ropl[116] + ropl[119] + ropl[121] + ropl[125] + ropl[126] + 
    ropl[175] + ropl[181] - ropl[182] + ropl[183] + ropl[188] + ropl[189] + 
    2.0*ropl[190] + ropl[191] + 2.0*ropl[192] + 2.0*ropl[193] + ropl[214] + 
    ropl[217] + ropl[244] + ropl[538] + ropl[330] + ropl[338] + ropl[352] + 
    ropl[358] + ropl[360] + ropl[361] + ropl[420] + ropl[490] + ropl[500] + 
    ropl[504] + ropl[731]; 
  // 9. CO2
  wdot[9] = ropl[21] + ropl[22] + ropl[23] + ropl[24] + ropl[29] + ropl[33] + 
    ropl[46] + ropl[48] + ropl[50] - ropl[121] + ropl[124] + ropl[185] + 
    ropl[359]; 
  // 10. CH2O
  wdot[10] = ropl[63] + ropl[32] - ropl[34] + ropl[36] + ropl[37] - ropl[38] - 
    ropl[39] - ropl[40] - ropl[41] - ropl[42] + ropl[43] + ropl[55] + ropl[56] - 
    ropl[57] + ropl[59] + ropl[60] + ropl[61] - ropl[62] + ropl[64] + ropl[65] + 
    ropl[66] + 2.0*ropl[67] + ropl[68] - ropl[69] + ropl[70] + ropl[71] - 
    ropl[72] - ropl[85] + ropl[92] + ropl[99] + ropl[101] - ropl[103] + 
    ropl[108] + ropl[120] + ropl[121] + ropl[123] + ropl[149] - ropl[152] + 
    ropl[181] + ropl[208] - ropl[230] + ropl[233] + ropl[308] + ropl[310] + 
    ropl[321] + ropl[331] + ropl[365] + ropl[390] + ropl[404] + ropl[407] + 
    ropl[450] + ropl[451] + ropl[468] + ropl[469] + ropl[474] - ropl[496] + 
    ropl[503] + ropl[504] + ropl[626] + ropl[678] + ropl[682] + ropl[753] + 
    ropl[762] - ropl[769] + ropl[846]; 
  // 11. HCO
  wdot[11] = -ropl[25] - ropl[26] - ropl[27] - ropl[28] - ropl[29] - ropl[30] - 
    ropl[31] - ropl[32] - ropl[33] + 2.0*ropl[34] - 2.0*ropl[35] - ropl[36] + 
    ropl[38] + ropl[39] + ropl[40] + ropl[41] + ropl[42] + ropl[47] + ropl[57] - 
    ropl[67] + ropl[69] + ropl[103] + ropl[152] + ropl[165] + ropl[197] + 
    ropl[208] + ropl[230] + ropl[234] + ropl[245] + ropl[272] + ropl[313] + 
    ropl[341] + ropl[350] + ropl[351] + ropl[357] + ropl[442] + ropl[465] + 
    ropl[492] + ropl[496] + ropl[683] + ropl[764] + ropl[766] + ropl[769]; 
  // 12. HOCHO
  wdot[12] = ropl[44] - ropl[45] - ropl[46] - ropl[47] - ropl[48] - ropl[49] - 
    ropl[50] - ropl[51] - ropl[52] - ropl[53] - ropl[54] + ropl[173]; 
  // 13. CH3OH
  wdot[13] = ropl[57] + ropl[58] + ropl[68] + ropl[69] - ropl[72] - ropl[74] - 
    ropl[75] - ropl[76] - ropl[77] - ropl[78] - ropl[79] - ropl[80] - ropl[81] - 
    ropl[82] - ropl[83] - ropl[85] - ropl[105] + ropl[108] + ropl[112] + 
    ropl[136] - ropl[154] + ropl[202] + ropl[223] + ropl[242] + ropl[251] + 
    ropl[536] + ropl[384] + ropl[418] + ropl[523] + ropl[524] + ropl[573] + 
    ropl[574] + ropl[575] + ropl[576] + ropl[615] + ropl[621] + ropl[700] + 
    ropl[743] + ropl[809] + ropl[810] + ropl[811]; 
  // 14. CH3O2H
  wdot[14] = ropl[103] + ropl[104] + ropl[105] + ropl[107] - ropl[113] + 
    ropl[135] + ropl[139] + ropl[172] + ropl[203] + ropl[226] + ropl[243] + 
    ropl[252] + ropl[537] + ropl[295] + ropl[383] + ropl[419] + ropl[466] + 
    ropl[521] + ropl[522] + ropl[581] + ropl[582] + ropl[583] + ropl[584] + 
    ropl[614] + ropl[620] + ropl[699] + ropl[721] + ropl[722] + ropl[754] + 
    ropl[829] + ropl[830] + ropl[831]; 
  // 15. CH3O2
  wdot[15] = ropl[102] - ropl[103] - ropl[104] - ropl[105] - ropl[106] - 
    ropl[107] - 2.0*ropl[108] - 2.0*ropl[109] - ropl[110] - ropl[111] - 
    ropl[112] - ropl[135] - ropl[139] - ropl[147] - ropl[172] - ropl[203] - 
    ropl[226] - ropl[243] - ropl[252] - ropl[537] - ropl[295] - ropl[300] - 
    ropl[383] - ropl[393] - ropl[419] - ropl[422] - ropl[430] - ropl[466] - 
    ropl[472] - ropl[521] - ropl[522] - ropl[581] - ropl[582] - ropl[583] - 
    ropl[584] - ropl[614] - ropl[620] - ropl[623] - ropl[699] - ropl[707] - 
    ropl[710] - ropl[721] - ropl[722] - ropl[754] - ropl[829] - ropl[830] - 
    ropl[831]; 
  // 16. CH4
  wdot[16] = ropl[31] + ropl[41] + ropl[52] - ropl[58] + ropl[59] + ropl[83] + 
    ropl[86] - ropl[87] - ropl[88] - ropl[89] - ropl[90] - ropl[91] + ropl[98] - 
    ropl[104] - ropl[115] + ropl[133] + ropl[142] - ropl[153] + ropl[170] + 
    ropl[178] + ropl[200] + ropl[210] + ropl[222] + ropl[240] + ropl[249] + 
    ropl[535] + ropl[291] + ropl[292] + ropl[293] + ropl[302] + ropl[315] + 
    ropl[323] + ropl[333] + ropl[346] + ropl[381] + ropl[417] + ropl[458] + 
    ropl[480] + ropl[486] + ropl[502] + ropl[515] + ropl[516] + ropl[561] + 
    ropl[562] + ropl[563] + ropl[564] + ropl[612] + ropl[618] + ropl[697] + 
    ropl[727] + ropl[741] + ropl[803] + ropl[804] + ropl[805]; 
  // 17. CH3
  wdot[17] = -ropl[31] - ropl[41] - ropl[52] + ropl[58] - ropl[59] + ropl[74] - 
    ropl[83] - ropl[86] + ropl[87] + ropl[88] + ropl[89] + ropl[90] + 
    2.0*ropl[91] - ropl[92] - ropl[93] - ropl[94] - ropl[95] - ropl[96] - 
    ropl[97] - ropl[98] - ropl[99] - ropl[100] - ropl[101] - ropl[102] + 
    ropl[104] - ropl[106] + 2.0*ropl[115] + ropl[117] + ropl[122] - 
    2.0*ropl[127] + ropl[263] + ropl[264] - ropl[133] + ropl[137] - ropl[142] + 
    2.0*ropl[143] + ropl[149] + ropl[153] + ropl[165] - ropl[170] + ropl[173] + 
    ropl[175] - ropl[178] + ropl[183] + ropl[197] - ropl[200] - ropl[205] - 
    ropl[210] + ropl[217] + ropl[218] - ropl[222] + ropl[227] - ropl[240] - 
    ropl[249] + ropl[259] - ropl[535] + ropl[268] + ropl[273] + ropl[287] - 
    ropl[291] - ropl[292] - ropl[293] + ropl[297] - ropl[302] + ropl[311] - 
    ropl[315] + ropl[316] - ropl[323] + ropl[328] - ropl[333] + ropl[340] + 
    ropl[342] - ropl[346] - ropl[354] - ropl[355] + ropl[370] + ropl[375] - 
    ropl[381] + ropl[391] + ropl[401] - ropl[417] + ropl[424] - ropl[434] + 
    ropl[440] + ropl[452] + ropl[454] + 2.0*ropl[456] - ropl[458] + ropl[471] - 
    ropl[480] - ropl[486] - ropl[502] - ropl[515] - ropl[516] - ropl[525] + 
    ropl[543] - ropl[561] - ropl[562] - ropl[563] - ropl[564] - ropl[612] - 
    ropl[618] + ropl[685] - ropl[697] + ropl[705] + ropl[725] - ropl[727] + 
    ropl[734] + ropl[735] - ropl[741] - ropl[803] - ropl[804] - ropl[805] - 
    ropl[821] - ropl[823]; 
  // 18. C2H6
  wdot[18] = ropl[128] + ropl[127] - ropl[129] - ropl[130] - ropl[131] - 
    ropl[132] - ropl[133] - ropl[134] - ropl[135] - ropl[136] - ropl[137] - 
    ropl[156] + ropl[253] + ropl[294] + ropl[303] + ropl[397] + ropl[569] + 
    ropl[570] + ropl[571] + ropl[572] + ropl[745] + ropl[815] + ropl[816] + 
    ropl[817]; 
  // 19. C2H5
  wdot[19] = -ropl[128] + ropl[257] + ropl[129] + ropl[130] + ropl[131] + 
    ropl[132] + ropl[133] + ropl[134] + ropl[135] + ropl[136] + ropl[137] + 
    ropl[138] - ropl[141] - ropl[142] - ropl[143] - ropl[144] - ropl[145] - 
    ropl[146] - ropl[147] + ropl[151] + ropl[156] - ropl[158] - ropl[159] - 
    ropl[160] + ropl[245] - ropl[253] + ropl[259] + ropl[272] - ropl[294] - 
    ropl[303] - ropl[304] + ropl[359] + ropl[360] + ropl[369] + ropl[376] - 
    ropl[389] - ropl[397] + ropl[403] + ropl[411] - ropl[435] + ropl[507] - 
    ropl[569] - ropl[570] - ropl[571] - ropl[572] + ropl[598] + ropl[633] - 
    ropl[745] - ropl[815] - ropl[816] - ropl[817]; 
  // 20. C2H4
  wdot[20] = ropl[264] - ropl[138] + 2.0*ropl[141] + ropl[142] + ropl[144] + 
    ropl[158] + ropl[159] + ropl[162] + ropl[189] + ropl[194] - ropl[195] - 
    ropl[196] - ropl[197] - ropl[198] - ropl[199] - ropl[200] - ropl[201] - 
    ropl[202] - ropl[203] - ropl[204] + ropl[205] + ropl[241] + ropl[256] + 
    ropl[287] + ropl[304] + ropl[305] + ropl[330] + ropl[338] + ropl[347] + 
    ropl[352] + ropl[369] + ropl[389] + ropl[398] + ropl[406] - ropl[408] + 
    ropl[527] + ropl[577] + ropl[578] + ropl[579] + ropl[580] + ropl[671] + 
    ropl[675] + ropl[746] + ropl[818] + ropl[819] + ropl[820]; 
  // 21. C2H3
  wdot[21] = -ropl[141] - ropl[194] + ropl[196] + ropl[199] + ropl[200] + 
    ropl[201] + ropl[202] + ropl[203] + ropl[204] + ropl[206] - ropl[207] - 
    ropl[208] - ropl[209] - ropl[210] - ropl[211] - ropl[212] + ropl[234] - 
    ropl[241] + ropl[244] - ropl[256] + ropl[268] - ropl[305] + ropl[341] - 
    ropl[347] + ropl[365] + ropl[376] - ropl[398] + ropl[400] + 2.0*ropl[402] + 
    ropl[405] - ropl[408] - ropl[526] - ropl[577] - ropl[578] - ropl[579] - 
    ropl[580] + ropl[724] - ropl[746] - ropl[818] - ropl[819] - ropl[820]; 
  // 22. C2H2
  wdot[22] = ropl[195] - ropl[206] + ropl[207] + ropl[210] + ropl[211] + 
    ropl[212] - ropl[213] - ropl[214] - ropl[215] - ropl[216] - ropl[217] + 
    ropl[297] + ropl[310] + ropl[311] + ropl[316] + ropl[331] - ropl[354] - 
    ropl[355] + ropl[357] + ropl[505]; 
  // 23. CH3CHO
  wdot[23] = ropl[263] + ropl[145] + ropl[148] + ropl[150] + ropl[160] + 
    ropl[161] - ropl[165] - ropl[166] - ropl[167] - ropl[168] - ropl[169] - 
    ropl[170] - ropl[171] - ropl[172] - ropl[173] - ropl[174] + ropl[313] + 
    ropl[361] + ropl[400] + ropl[405] + ropl[528] + ropl[629] + ropl[630] + 
    ropl[658] + ropl[787]; 
  // 24. CH2CHO
  wdot[24] = ropl[164] + ropl[174] - ropl[180] - ropl[181] + ropl[198] + 
    ropl[209] + ropl[308] + ropl[668]; 
  // 25. CH2CO
  wdot[25] = ropl[176] + ropl[177] + ropl[178] + ropl[180] + ropl[182] - 
    ropl[183] - ropl[184] - ropl[185] - ropl[186] - ropl[187] - ropl[188] - 
    ropl[189] + ropl[216] + ropl[227] + ropl[273] + ropl[327] + ropl[328] + 
    ropl[340] + ropl[350] + ropl[403] + ropl[406] + ropl[411] + ropl[456]; 
  // 26. HCCO
  wdot[26] = ropl[184] + ropl[186] + ropl[187] - ropl[190] - ropl[191] - 
    ropl[192] - ropl[193] + ropl[213] + ropl[215] + ropl[336] + ropl[342] + 
    ropl[343] + ropl[351] + ropl[358]; 
  // 27. CH3CO3
  wdot[27] = -ropl[179];
  // 28. C2H5O2H
  wdot[28] = ropl[140] + ropl[152] + ropl[153] + ropl[154] + ropl[155] + 
    ropl[156] - ropl[157] + ropl[204] + ropl[254] + ropl[296] + ropl[387]; 
  // 29. C2H5O2
  wdot[29] = -ropl[140] - ropl[151] - ropl[152] - ropl[153] - ropl[154] - 
    ropl[155] - ropl[156] - ropl[161] - ropl[162] - ropl[204] - ropl[254] - 
    ropl[296] - ropl[387] - ropl[399] - ropl[431] - ropl[624] - ropl[708] - 
    ropl[711]; 
  // 30. CH3COCH3
  wdot[30] = ropl[262] - ropl[218] - ropl[219] - ropl[220] - ropl[221] - 
    ropl[222] - ropl[223] - ropl[224] - ropl[225] - ropl[226] - ropl[229] + 
    ropl[440] + ropl[492] + ropl[500] + ropl[692] + ropl[693] + ropl[694] + 
    ropl[713] + ropl[752] + ropl[860]; 
  // 31. CH3COCH2O2
  wdot[31] = -ropl[228] - ropl[229] - ropl[230] - ropl[231];
  // 32. CH3COCH2O2H
  wdot[32] = ropl[229] + ropl[230] + ropl[231] - ropl[232];
  // 33. C2H3CHO
  wdot[33] = -ropl[234] - ropl[235] - ropl[236] - ropl[237] - ropl[238] - 
    ropl[239] - ropl[240] - ropl[241] - ropl[242] - ropl[243] + ropl[309] + 
    ropl[364] + ropl[366] + ropl[391] + ropl[401] + ropl[679]; 
  // 34. C2H3CO
  wdot[34] = ropl[235] + ropl[236] + ropl[237] + ropl[238] + ropl[239] + 
    ropl[240] + ropl[241] + ropl[242] + ropl[243] - ropl[244]; 
  // 35. C2H5CHO
  wdot[35] = -ropl[245] - ropl[246] - ropl[247] - ropl[248] - ropl[249] - 
    ropl[250] - ropl[251] - ropl[252] - ropl[253] - ropl[254] - ropl[255] - 
    ropl[256] - ropl[267] + ropl[627] + ropl[628] + ropl[659] + ropl[681] + 
    ropl[789] + ropl[790]; 
  // 36. C3H6
  wdot[36] = ropl[265] + ropl[266] + ropl[258] + ropl[260] + ropl[261] + 
    ropl[267] - ropl[268] - ropl[269] - ropl[270] - ropl[271] - ropl[272] - 
    ropl[273] - ropl[274] - ropl[275] - ropl[276] - ropl[277] - ropl[278] - 
    ropl[279] - ropl[280] - ropl[281] - ropl[282] - ropl[283] - ropl[284] - 
    ropl[285] - ropl[286] - ropl[287] - ropl[288] - ropl[289] - ropl[290] - 
    ropl[291] - ropl[292] - ropl[293] - ropl[294] - ropl[295] - ropl[296] + 
    ropl[304] - ropl[306] + ropl[334] + ropl[348] + ropl[367] + ropl[368] + 
    ropl[370] + ropl[385] + ropl[394] + ropl[424] + ropl[451] + ropl[454] + 
    ropl[461] + ropl[462] + ropl[463] - ropl[526] + ropl[592] + ropl[660] + 
    ropl[672] + ropl[676] + ropl[732] + ropl[765] + ropl[767] + ropl[769] - 
    ropl[824] + ropl[848]; 
  // 37. C3H5-A
  wdot[37] = -ropl[267] + ropl[269] + ropl[275] + ropl[278] + ropl[281] + 
    ropl[284] + ropl[288] + ropl[291] + ropl[294] + ropl[295] + ropl[296] - 
    ropl[297] - ropl[298] - ropl[299] - ropl[300] - ropl[301] - ropl[302] - 
    ropl[303] - ropl[304] - ropl[305] + 2.0*ropl[306] - ropl[307] - ropl[308] - 
    ropl[309] - ropl[310] - ropl[334] - ropl[348] + ropl[375] - ropl[385] - 
    ropl[394] + ropl[404] - ropl[437] - ropl[461] + ropl[507] + ropl[631] + 
    ropl[678] - ropl[769]; 
  // 38. C3H4-P
  wdot[38] = ropl[312] + ropl[318] + ropl[322] + ropl[323] + ropl[325] - 
    ropl[335] - ropl[336] - ropl[337] - ropl[338] - ropl[339] - ropl[340] - 
    ropl[341] - ropl[342] - ropl[343] - ropl[344] - ropl[345] - ropl[346] - 
    ropl[347] - ropl[348] + ropl[354] + ropl[685]; 
  // 39. C3H4-A
  wdot[39] = ropl[298] + ropl[301] + ropl[302] + ropl[303] + ropl[305] - 
    ropl[306] + ropl[307] + ropl[314] + ropl[315] + ropl[317] + ropl[319] - 
    ropl[324] - ropl[325] - ropl[326] - ropl[327] - ropl[328] - ropl[329] - 
    ropl[330] - ropl[331] - ropl[332] - ropl[333] - ropl[334] - ropl[352] - 
    ropl[353] + ropl[355] + ropl[407] + ropl[469] + ropl[471] + ropl[477] + 
    ropl[506] + ropl[723]; 
  // 40. C3H3
  wdot[40] = ropl[324] + ropl[326] + ropl[329] + ropl[332] + ropl[333] + 
    ropl[334] + ropl[335] + ropl[337] + ropl[339] + ropl[344] + ropl[345] + 
    ropl[346] + ropl[347] + ropl[348] - ropl[349] - ropl[350] + ropl[353] - 
    ropl[356]; 
  // 41. NC3H7O2
  wdot[41] = -ropl[362] - ropl[368];
  // 42. IC3H7O2
  wdot[42] = -ropl[363] - ropl[367];
  // 43. CH3CHCO
  wdot[43] = ropl[274] - ropl[359] - ropl[360] - ropl[361] + ropl[680];
  // 44. C4H8-1
  wdot[44] = ropl[371] + ropl[372] + ropl[373] + ropl[374] - ropl[375] - 
    ropl[376] - ropl[377] - ropl[378] - ropl[379] - ropl[380] - ropl[381] - 
    ropl[382] - ropl[383] - ropl[384] - ropl[385] - ropl[386] - ropl[387] + 
    ropl[389] + ropl[390] + ropl[410] + ropl[595] + ropl[659] + ropl[670] + 
    ropl[674]; 
  // 45. SC4H9
  wdot[45] = -ropl[370] - ropl[372] - ropl[374];
  // 46. C4H71-3
  wdot[46] = ropl[377] + ropl[378] + ropl[379] + ropl[380] + ropl[381] + 
    ropl[382] + ropl[383] + ropl[384] + ropl[385] + 2.0*ropl[386] + ropl[387] - 
    ropl[388] - ropl[389] - ropl[390] - ropl[391] - ropl[392] - ropl[393] - 
    ropl[394] - ropl[395] - ropl[396] - ropl[397] - ropl[398] - ropl[399] - 
    ropl[438] + ropl[632]; 
  // 47. C4H6
  wdot[47] = -ropl[386] + ropl[388] + ropl[394] + ropl[395] + ropl[396] + 
    ropl[397] + ropl[398] - ropl[402] - ropl[403] - ropl[404] - ropl[405] - 
    ropl[406] - ropl[407] + ropl[408] - ropl[525]; 
  // 48. PC4H9O2
  wdot[48] = -ropl[409] - ropl[410];
  // 49. C2H5COCH2
  wdot[49] = -ropl[411] + ropl[789];
  // 50. NC3H7CHO
  wdot[50] = -ropl[412] - ropl[413] - ropl[414] - ropl[415] - ropl[416] - 
    ropl[417] - ropl[418] - ropl[419] + ropl[625] + ropl[660] + ropl[786] + 
    ropl[788]; 
  // 51. IC4H8
  wdot[51] = ropl[423] + ropl[425] + ropl[426] + ropl[427] + ropl[446] + 
    ropl[447] - ropl[452] - ropl[453] - ropl[454] - ropl[455] - ropl[456] - 
    ropl[457] - ropl[458] - ropl[459] - ropl[460] - ropl[461] - ropl[462] - 
    ropl[463] - ropl[464] - ropl[465] - ropl[466] - ropl[497] + ropl[686] + 
    ropl[694] + ropl[725] - ropl[747] - ropl[822] + ropl[847]; 
  // 52. IC4H7
  wdot[52] = -ropl[439] + ropl[453] + ropl[455] + ropl[457] + ropl[458] + 
    ropl[459] + ropl[460] + ropl[461] + ropl[462] + ropl[463] + ropl[464] + 
    ropl[466] - ropl[467] - ropl[468] - ropl[469] - ropl[470] - ropl[471] - 
    ropl[472] - ropl[473] + ropl[497] + ropl[691]; 
  // 53. TC4H9O2
  wdot[53] = -ropl[429] - ropl[430] - ropl[431] - 2.0*ropl[432] - ropl[433] - 
    ropl[434] - ropl[435] - ropl[436] - ropl[437] - ropl[438] - ropl[439] - 
    ropl[447]; 
  // 54. IC4H9O2
  wdot[54] = -ropl[428] - ropl[445] - ropl[446];
  // 55. IC3H7CHO
  wdot[55] = -ropl[441] - ropl[442] - ropl[443] - ropl[444] + ropl[494] + 
    ropl[496] + ropl[497] + ropl[501] + ropl[712] + ropl[847] + ropl[859]; 
  // 56. TC3H6CHO
  wdot[56] = ropl[441] + ropl[443] + ropl[444] - ropl[491] - ropl[493] - 
    ropl[494] - ropl[496] - ropl[497] + ropl[498] - ropl[499] - ropl[500] - 
    ropl[501] - ropl[502] + ropl[859]; 
  // 57. IC4H8OOH-IO2
  wdot[57] = -ropl[448] - ropl[449];
  // 58. IC4KETII
  wdot[58] = ropl[449] - ropl[450];
  // 59. IC4H6OH
  wdot[59] = ropl[475] - ropl[477] - ropl[503];
  // 60. IC3H5CHO
  wdot[60] = ropl[467] + ropl[470] + ropl[476] + ropl[478] + ropl[479] + 
    ropl[480] + ropl[481] + ropl[482] + ropl[483] - ropl[484] - ropl[485] - 
    ropl[486] - ropl[487] - ropl[488] - ropl[489] + ropl[493] + ropl[499] + 
    ropl[502] + ropl[724]; 
  // 61. IC3H5CO
  wdot[61] = ropl[484] + ropl[485] + ropl[486] + ropl[487] + ropl[488] + 
    ropl[489] - ropl[490]; 
  // 62. IC4H7OOH
  wdot[62] = -ropl[495] + ropl[763];
  // 63. TC3H6O2CHO
  wdot[63] = -ropl[498];
  // 64. C5H10-1
  wdot[64] = -ropl[513] - ropl[514] - ropl[507] + ropl[508] - ropl[509] - 
    ropl[510] - ropl[511] - ropl[512] - ropl[515] - ropl[516] - ropl[517] - 
    ropl[518] - ropl[519] - ropl[520] - ropl[521] - ropl[522] - ropl[523] - 
    ropl[524] - ropl[528] + ropl[598] + ropl[629] + ropl[630] + ropl[658] + 
    ropl[669] + ropl[673] - ropl[682] - ropl[683] - ropl[684]; 
  // 65. C5H91-3
  wdot[65] = ropl[513] - ropl[508] + ropl[509] + ropl[511] + ropl[515] + 
    ropl[517] + ropl[519] + ropl[521] + ropl[523] + ropl[525] + ropl[633]; 
  // 66. C5H91-4
  wdot[66] = ropl[514] + ropl[510] + ropl[512] + ropl[516] + ropl[518] + 
    ropl[520] + ropl[522] + ropl[524] + ropl[526]; 
  // 67. C6H13O2-1
  wdot[67] = -ropl[529];
  // 68. NC4H9CHO
  wdot[68] = -ropl[531] - ropl[532] - ropl[533] - ropl[534] - ropl[535] - 
    ropl[536] - ropl[537] - ropl[530] + ropl[668] + ropl[785]; 
  // 69. NC7H16
  wdot[69] = -ropl[539] - ropl[540] - ropl[541] - ropl[542] - ropl[543] - 
    ropl[544] - ropl[545] - ropl[546] - ropl[547] - ropl[548] - ropl[549] - 
    ropl[550] - ropl[551] - ropl[552] - ropl[553] - ropl[554] - ropl[555] - 
    ropl[556] - ropl[557] - ropl[558] - ropl[559] - ropl[560] - ropl[561] - 
    ropl[562] - ropl[563] - ropl[564] - ropl[565] - ropl[566] - ropl[567] - 
    ropl[568] - ropl[569] - ropl[570] - ropl[571] - ropl[572] - ropl[573] - 
    ropl[574] - ropl[575] - ropl[576] - ropl[577] - ropl[578] - ropl[579] - 
    ropl[580] - ropl[581] - ropl[582] - ropl[583] - ropl[584]; 
  // 70. C7H14-1
  wdot[70] = ropl[591] + ropl[593] + ropl[600] + ropl[601] - ropl[626] - 
    ropl[631] + ropl[638] + ropl[639] - ropl[651]; 
  // 71. C7H14-2
  wdot[71] = ropl[594] + ropl[596] + ropl[602] + ropl[603] - ropl[610] - 
    ropl[611] - ropl[612] - ropl[613] - ropl[614] - ropl[615] - ropl[627] - 
    ropl[629] - ropl[632] + ropl[640] + ropl[641] - ropl[652] - ropl[772]; 
  // 72. C7H14-3
  wdot[72] = ropl[597] + ropl[599] + ropl[604] + ropl[605] - ropl[616] - 
    ropl[617] - ropl[618] - ropl[619] - ropl[620] - ropl[621] - ropl[628] - 
    ropl[630] - ropl[633] + ropl[642] + ropl[643] - ropl[653] - ropl[773]; 
  // 73. C7H132-4
  wdot[73] = ropl[610] + ropl[611] + ropl[612] + ropl[613] + ropl[614] + 
    ropl[615] + ropl[616] + ropl[617] + ropl[618] + ropl[619] + ropl[620] + 
    ropl[621] - ropl[622] - ropl[623] - ropl[624]; 
  // 74. C7H15O2-1
  wdot[74] = -ropl[634] - ropl[638] - ropl[644] - ropl[645];
  // 75. C7H15O2-2
  wdot[75] = -ropl[635] - ropl[639] - ropl[640] - ropl[646] - ropl[647];
  // 76. C7H15O2-3
  wdot[76] = -ropl[636] - ropl[641] - ropl[642] - ropl[648] - ropl[770] - 
    ropl[771]; 
  // 77. C7H15O2-4
  wdot[77] = -ropl[637] - ropl[643] - ropl[649] - ropl[650];
  // 78. C7H14OOH1-3O2
  wdot[78] = -ropl[662] - ropl[667];
  // 79. C7H14OOH2-3O2
  wdot[79] = -ropl[663] - ropl[777];
  // 80. C7H14OOH2-4O2
  wdot[80] = -ropl[664] - ropl[778];
  // 81. C7H14OOH4-2O2
  wdot[81] = -ropl[665] - ropl[783];
  // 82. C7H14OOH4-3O2
  wdot[82] = -ropl[666] - ropl[784];
  // 83. C7H14O1-3
  wdot[83] = ropl[654] - ropl[671] - ropl[675];
  // 84. C7H14O2-4
  wdot[84] = ropl[655] + ropl[657] - ropl[669] - ropl[672] - ropl[673] - 
    ropl[676]; 
  // 85. C7H14O3-5
  wdot[85] = ropl[656] - ropl[670] - ropl[674];
  // 86. NC7KET13
  wdot[86] = ropl[667] - ropl[668];
  // 87. C4H7OOH1-4
  wdot[87] = ropl[661] - ropl[677];
  // 88. CH3CHCHO
  wdot[88] = -ropl[679] - ropl[680] - ropl[681] + ropl[768];
  // 89. IC4H7-I1
  wdot[89] = -ropl[685] + ropl[713];
  // 90. XC7H14
  wdot[90] = ropl[687] + ropl[689] - ropl[691] - ropl[692] - ropl[695] - 
    ropl[696] - ropl[697] - ropl[698] - ropl[699] - ropl[700] + ropl[715] + 
    ropl[762] - ropl[821]; 
  // 91. YC7H14
  wdot[91] = ropl[688] + ropl[690] - ropl[693] - ropl[694] - ropl[701] - 
    ropl[702] - ropl[703] - ropl[704] + ropl[716] + ropl[764] + ropl[766] - 
    ropl[823] + ropl[846]; 
  // 92. XC7H13-Z
  wdot[92] = ropl[695] + ropl[696] + ropl[697] + ropl[698] + ropl[699] + 
    ropl[700] + ropl[701] + ropl[703] - ropl[705] - ropl[706] - ropl[707] - 
    ropl[708]; 
  // 93. YC7H13-Y2
  wdot[93] = ropl[702] + ropl[704] - ropl[709] - ropl[710] - ropl[711];
  // 94. YC7H15O2
  wdot[94] = -ropl[714] - ropl[715] - ropl[716];
  // 95. ACC6H10
  wdot[95] = ropl[705] - ropl[717] - ropl[718] - ropl[719] - ropl[720] - 
    ropl[721] - ropl[722]; 
  // 96. ACC6H9-A
  wdot[96] = ropl[717] + ropl[719] + ropl[721] - ropl[723];
  // 97. ACC6H9-D
  wdot[97] = ropl[718] + ropl[720] + ropl[722] - ropl[724];
  // 98. NEOC5H11
  wdot[98] = -ropl[725] + ropl[737] + ropl[752] - ropl[824];
  // 99. TC4H9CHO
  wdot[99] = -ropl[726] - ropl[727] - ropl[728] - ropl[729] - ropl[730] + 
    ropl[768] + ropl[848]; 
  // 100. IC8H18
  wdot[100] = -ropl[733] - ropl[734] - ropl[735] - ropl[736] - ropl[737] - 
    ropl[738] - ropl[739] - ropl[740] - ropl[741] - ropl[742] - ropl[743] - 
    ropl[744] - ropl[745] - ropl[746] - ropl[754] - ropl[791] - ropl[792] - 
    ropl[793] - ropl[794] - ropl[795] - ropl[796] - ropl[797] - ropl[798] - 
    ropl[799] - ropl[800] - ropl[801] - ropl[802] - ropl[803] - ropl[804] - 
    ropl[805] - ropl[806] - ropl[807] - ropl[808] - ropl[809] - ropl[810] - 
    ropl[811] - ropl[812] - ropl[813] - ropl[814] - ropl[815] - ropl[816] - 
    ropl[817] - ropl[818] - ropl[819] - ropl[820] - ropl[829] - ropl[830] - 
    ropl[831]; 
  // 101. IC8H16
  wdot[101] = ropl[748] + ropl[750] - ropl[752] + ropl[757] + ropl[758] + 
    ropl[760] + ropl[825]; 
  // 102. JC8H16
  wdot[102] = ropl[749] + ropl[751] - ropl[753] + ropl[759] + ropl[761] + 
    ropl[826] + ropl[828] + ropl[841]; 
  // 103. BC8H17O2
  wdot[103] = -ropl[756] - ropl[757] - ropl[833] - ropl[837] - ropl[838];
  // 104. CC8H17O2
  wdot[104] = -ropl[755] - ropl[758] - ropl[759];
  // 105. IC8ETERAB
  wdot[105] = -ropl[764] - ropl[766] + ropl[842] + ropl[843];
  // 106. IC8ETERBD
  wdot[106] = -ropl[765] - ropl[767] + ropl[844] + ropl[845];
  // 107. IC8KETDB
  wdot[107] = -ropl[768] + ropl[858];
  // 108. ISO002
  wdot[108] = -ropl[850] - ropl[851] - ropl[852] - ropl[855] - ropl[856] - 
    ropl[857]; 
  // 109. ISO003
  wdot[109] = -ropl[849] - ropl[853] - ropl[854] - ropl[858];
  // 110. ISO004
  wdot[110] = -ropl[832] - ropl[834] - ropl[835] - ropl[836] - ropl[839] - 
    ropl[840] - ropl[841]; 
  // 111. ISO008
  wdot[111] = ropl[854] + ropl[855] + ropl[856] + ropl[857] - ropl[859] - 
    ropl[860]; 
  // 112. ISO010
  wdot[112] = -ropl[774] - ropl[775] - ropl[780] - ropl[781];
  // 113. ISO011
  wdot[113] = -ropl[776] - ropl[779] - ropl[782];
  // 114. ISO014
  wdot[114] = ropl[777] + ropl[778] + ropl[779] + ropl[780] + ropl[781] + 
    ropl[782] + ropl[783] + ropl[784] - ropl[785] - ropl[786] - ropl[787] - 
    ropl[788] - ropl[789] - ropl[790]; 
  // 115. N2
  wdot[115] = 0.0;
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+0*spec_stride) , 
    "d"(wdot[0]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+1*spec_stride) , 
    "d"(wdot[1]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+2*spec_stride) , 
    "d"(wdot[2]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+3*spec_stride) , 
    "d"(wdot[3]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+4*spec_stride) , 
    "d"(wdot[4]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+5*spec_stride) , 
    "d"(wdot[5]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+6*spec_stride) , 
    "d"(wdot[6]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+7*spec_stride) , 
    "d"(wdot[7]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+8*spec_stride) , 
    "d"(wdot[8]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+9*spec_stride) , 
    "d"(wdot[9]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+10*spec_stride) , 
    "d"(wdot[10]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+11*spec_stride) , 
    "d"(wdot[11]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+12*spec_stride) , 
    "d"(wdot[12]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+13*spec_stride) , 
    "d"(wdot[13]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+14*spec_stride) , 
    "d"(wdot[14]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+15*spec_stride) , 
    "d"(wdot[15]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+16*spec_stride) , 
    "d"(wdot[16]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+17*spec_stride) , 
    "d"(wdot[17]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+18*spec_stride) , 
    "d"(wdot[18]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+19*spec_stride) , 
    "d"(wdot[19]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+20*spec_stride) , 
    "d"(wdot[20]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+21*spec_stride) , 
    "d"(wdot[21]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+22*spec_stride) , 
    "d"(wdot[22]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+23*spec_stride) , 
    "d"(wdot[23]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+24*spec_stride) , 
    "d"(wdot[24]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+25*spec_stride) , 
    "d"(wdot[25]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+26*spec_stride) , 
    "d"(wdot[26]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+27*spec_stride) , 
    "d"(wdot[27]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+28*spec_stride) , 
    "d"(wdot[28]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+29*spec_stride) , 
    "d"(wdot[29]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+30*spec_stride) , 
    "d"(wdot[30]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+31*spec_stride) , 
    "d"(wdot[31]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+32*spec_stride) , 
    "d"(wdot[32]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+33*spec_stride) , 
    "d"(wdot[33]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+34*spec_stride) , 
    "d"(wdot[34]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+35*spec_stride) , 
    "d"(wdot[35]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+36*spec_stride) , 
    "d"(wdot[36]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+37*spec_stride) , 
    "d"(wdot[37]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+38*spec_stride) , 
    "d"(wdot[38]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+39*spec_stride) , 
    "d"(wdot[39]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+40*spec_stride) , 
    "d"(wdot[40]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+41*spec_stride) , 
    "d"(wdot[41]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+42*spec_stride) , 
    "d"(wdot[42]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+43*spec_stride) , 
    "d"(wdot[43]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+44*spec_stride) , 
    "d"(wdot[44]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+45*spec_stride) , 
    "d"(wdot[45]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+46*spec_stride) , 
    "d"(wdot[46]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+47*spec_stride) , 
    "d"(wdot[47]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+48*spec_stride) , 
    "d"(wdot[48]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+49*spec_stride) , 
    "d"(wdot[49]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+50*spec_stride) , 
    "d"(wdot[50]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+51*spec_stride) , 
    "d"(wdot[51]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+52*spec_stride) , 
    "d"(wdot[52]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+53*spec_stride) , 
    "d"(wdot[53]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+54*spec_stride) , 
    "d"(wdot[54]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+55*spec_stride) , 
    "d"(wdot[55]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+56*spec_stride) , 
    "d"(wdot[56]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+57*spec_stride) , 
    "d"(wdot[57]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+58*spec_stride) , 
    "d"(wdot[58]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+59*spec_stride) , 
    "d"(wdot[59]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+60*spec_stride) , 
    "d"(wdot[60]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+61*spec_stride) , 
    "d"(wdot[61]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+62*spec_stride) , 
    "d"(wdot[62]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+63*spec_stride) , 
    "d"(wdot[63]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+64*spec_stride) , 
    "d"(wdot[64]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+65*spec_stride) , 
    "d"(wdot[65]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+66*spec_stride) , 
    "d"(wdot[66]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+67*spec_stride) , 
    "d"(wdot[67]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+68*spec_stride) , 
    "d"(wdot[68]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+69*spec_stride) , 
    "d"(wdot[69]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+70*spec_stride) , 
    "d"(wdot[70]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+71*spec_stride) , 
    "d"(wdot[71]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+72*spec_stride) , 
    "d"(wdot[72]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+73*spec_stride) , 
    "d"(wdot[73]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+74*spec_stride) , 
    "d"(wdot[74]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+75*spec_stride) , 
    "d"(wdot[75]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+76*spec_stride) , 
    "d"(wdot[76]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+77*spec_stride) , 
    "d"(wdot[77]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+78*spec_stride) , 
    "d"(wdot[78]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+79*spec_stride) , 
    "d"(wdot[79]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+80*spec_stride) , 
    "d"(wdot[80]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+81*spec_stride) , 
    "d"(wdot[81]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+82*spec_stride) , 
    "d"(wdot[82]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+83*spec_stride) , 
    "d"(wdot[83]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+84*spec_stride) , 
    "d"(wdot[84]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+85*spec_stride) , 
    "d"(wdot[85]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+86*spec_stride) , 
    "d"(wdot[86]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+87*spec_stride) , 
    "d"(wdot[87]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+88*spec_stride) , 
    "d"(wdot[88]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+89*spec_stride) , 
    "d"(wdot[89]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+90*spec_stride) , 
    "d"(wdot[90]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+91*spec_stride) , 
    "d"(wdot[91]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+92*spec_stride) , 
    "d"(wdot[92]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+93*spec_stride) , 
    "d"(wdot[93]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+94*spec_stride) , 
    "d"(wdot[94]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+95*spec_stride) , 
    "d"(wdot[95]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+96*spec_stride) , 
    "d"(wdot[96]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+97*spec_stride) , 
    "d"(wdot[97]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+98*spec_stride) , 
    "d"(wdot[98]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+99*spec_stride) , 
    "d"(wdot[99]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+100*spec_stride) 
    , "d"(wdot[100]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+101*spec_stride) 
    , "d"(wdot[101]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+102*spec_stride) 
    , "d"(wdot[102]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+103*spec_stride) 
    , "d"(wdot[103]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+104*spec_stride) 
    , "d"(wdot[104]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+105*spec_stride) 
    , "d"(wdot[105]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+106*spec_stride) 
    , "d"(wdot[106]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+107*spec_stride) 
    , "d"(wdot[107]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+108*spec_stride) 
    , "d"(wdot[108]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+109*spec_stride) 
    , "d"(wdot[109]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+110*spec_stride) 
    , "d"(wdot[110]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+111*spec_stride) 
    , "d"(wdot[111]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+112*spec_stride) 
    , "d"(wdot[112]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+113*spec_stride) 
    , "d"(wdot[113]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+114*spec_stride) 
    , "d"(wdot[114]) : "memory"); 
  asm volatile("st.global.cs.f64 [%0], %1;" : : "l"(wdot_array+115*spec_stride) 
    , "d"(wdot[115]) : "memory"); 
}

