#ifndef GAMMALAW_H
#define GAMMALAW_H

#include <AMReX.H>
#include <AMReX_REAL.H>
#include <AMReX_Utility.H>
#include <AMReX_Arena.H>
#include <AMReX_ParmParse.H>

#include "mechanism.H"
#include "PhysicsConstants.H"

namespace pele {
namespace physics {
namespace eos {

struct GammaLaw
{
  using eos_type = GammaLaw;

  static std::string identifier() { return "GammaLaw"; }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void molecular_weight(amrex::Real mw[])
  {
    for (int n = 0; n < NUM_SPECIES; n++) {
      mw[n] = Constants::AIRMW;
    }
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void inv_molecular_weight(amrex::Real imw[])
  {
    for (int n = 0; n < NUM_SPECIES; n++) {
      imw[n] = 1. / Constants::AIRMW;
    }
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void Y2WBAR(amrex::Real* /*Y*/, amrex::Real& WBAR)
  {
    WBAR = Constants::AIRMW;
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void T2Ei(amrex::Real T, amrex::Real Ei[])
  {
    amrex::Real wbar = Constants::AIRMW;
    const amrex::Real Cv = Constants::RU / (wbar * (Constants::gamma - 1.0));
    for (int n = 0; n < NUM_SPECIES; n++) {
      Ei[n] = Cv * T;
    }
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void RTY2Ei(
    amrex::Real /*R*/, amrex::Real T, amrex::Real* /*Y[]*/, amrex::Real Ei[])
  {
    T2Ei(T, Ei);
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void TY2Cv(amrex::Real /*T*/, amrex::Real Y[], amrex::Real& Cv)
  {
    amrex::Real wbar;
    Y2WBAR(Y, wbar);
    Cv = Constants::RU / (wbar * (Constants::gamma - 1.0));
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void
  RTY2Cv(amrex::Real /*R*/, amrex::Real T, amrex::Real Y[], amrex::Real& Cv)
  {
    TY2Cv(T, Y, Cv);
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void TY2Cp(amrex::Real T, amrex::Real Y[], amrex::Real& Cp)
  {
    amrex::Real cv;
    TY2Cv(T, Y, cv);
    Cp = Constants::gamma * cv;
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void
  RTY2Cp(amrex::Real /*R*/, amrex::Real T, amrex::Real Y[], amrex::Real& Cp)
  {
    TY2Cp(T, Y, Cp);
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void T2Cpi(amrex::Real /*T*/, amrex::Real Cpi[])
  {
    for (int n = 0; n < NUM_SPECIES; n++) {
      Cpi[n] = Constants::gamma * Constants::RU /
               (Constants::AIRMW * (Constants::gamma - 1.0));
    }
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void RYET2P(
    amrex::Real R,
    amrex::Real* /*Y*/,
    const amrex::Real E,
    amrex::Real /*T*/,
    amrex::Real& P)
  {
    P = (Constants::gamma - 1.0) * R * E;
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void
  RPY2Cs(amrex::Real R, amrex::Real P, amrex::Real* /*Y*/, amrex::Real& Cs)
  {
    Cs = std::sqrt(Constants::gamma * P / R);
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void
  RTY2Cs(amrex::Real R, amrex::Real T, amrex::Real Y[], amrex::Real& Cs)
  {
    amrex::Real wbar, P;
    Y2WBAR(Y, wbar);
    amrex::Real Cv = Constants::RU / (wbar * (Constants::gamma - 1.0));
    amrex::Real E = Cv * T;
    RYET2P(R, Y, E, T, P);
    Cs = std::sqrt(Constants::gamma * P / R);
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void EY2T(amrex::Real E, amrex::Real Y[], amrex::Real& T)
  {
    amrex::Real poverrho, wbar;
    poverrho = (Constants::gamma - 1.0) * E;
    Y2WBAR(Y, wbar);
    T = poverrho * wbar / Constants::RU;
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void
  REY2T(amrex::Real /*R*/, amrex::Real E, amrex::Real Y[], amrex::Real& T)
  {
    EY2T(E, Y, T);
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void HY2T(amrex::Real H, amrex::Real* /*Y*/, amrex::Real& T)
  {
    amrex::Real wbar = Constants::AIRMW;
    const amrex::Real Cv = Constants::RU / (wbar * (Constants::gamma - 1.0));
    T = H / (Cv * Constants::gamma);
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void
  RHY2T(amrex::Real /*R*/, amrex::Real H, amrex::Real Y[], amrex::Real& T)
  {
    HY2T(H, Y, T);
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void
  RTY2P(amrex::Real R, amrex::Real T, amrex::Real Y[], amrex::Real& P)
  {
    amrex::Real Cv, E;
    TY2Cv(T, Y, Cv);
    E = Cv * T;
    P = (Constants::gamma - 1.0) * R * E;
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void
  RYP2T(amrex::Real R, amrex::Real Y[], amrex::Real P, amrex::Real& T)
  {
    amrex::Real wbar;
    Y2WBAR(Y, wbar);
    T = P * wbar / (R * Constants::RU);
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void RTY2WDOT(
    amrex::Real /*R*/,
    amrex::Real /*T*/,
    amrex::Real* /*Y[]*/,
    amrex::Real* /*WDOT[]*/)
  {
    amrex::Error("RTY2WDOT not physically possible for GammaLaw EoS");
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void RTY2JAC(
    amrex::Real /*R*/,
    amrex::Real /*T*/,
    amrex::Real* /*Y[]*/,
    amrex::Real* /*JAC[]*/,
    const int /*HP*/)
  {
    amrex::Error("RTY2JAC not supported with GammaLaw EoS");
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void RTY2C(
    amrex::Real /*R*/,
    amrex::Real /*T*/,
    amrex::Real* /*Y[]*/,
    amrex::Real* /*acti[]*/)
  {
    amrex::Error("RTY2C not supported with GammaLaw EoS");
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void Y2X(const amrex::Real Y[], amrex::Real X[])
  {
    for (int n = 0; n < NUM_SPECIES; n++) {
      X[n] = Y[n];
    }
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void X2Y(const amrex::Real X[], amrex::Real Y[])
  {
    for (int n = 0; n < NUM_SPECIES; n++) {
      Y[n] = X[n];
    }
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void PYT2RE(
    amrex::Real P,
    amrex::Real Y[],
    amrex::Real T,
    amrex::Real& R,
    amrex::Real& E)
  {
    amrex::Real wbar;
    Y2WBAR(Y, wbar);
    R = P * wbar / (Constants::RU * T);
    E = P / (R * (Constants::gamma - 1.0));
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void
  PYT2R(amrex::Real P, amrex::Real Y[], amrex::Real T, amrex::Real& R)
  {
    amrex::Real wbar;
    Y2WBAR(Y, wbar);
    R = P * wbar / (Constants::RU * T);
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void
  RYP2E(amrex::Real R, amrex::Real Y[], amrex::Real P, amrex::Real& E)
  {
    amrex::Real wbar;
    Y2WBAR(Y, wbar);
    E = P / (R * (Constants::gamma - 1.0));
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void TY2E(amrex::Real T, const amrex::Real Y[], amrex::Real& E)
  {
    amrex::Real ei[NUM_SPECIES];
    T2Ei(T, ei);
    E = 0.0;
    for (int n = 0; n < NUM_SPECIES; n++) {
      E += Y[n] * ei[n];
    }
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void
  RTY2E(amrex::Real /*R*/, amrex::Real T, amrex::Real Y[], amrex::Real& E)
  {
    TY2E(T, Y, E);
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void T2Hi(amrex::Real T, amrex::Real Hi[])
  {
    amrex::Real wbar = Constants::AIRMW;
    const amrex::Real Cv = Constants::RU / (wbar * (Constants::gamma - 1.0));
    for (int n = 0; n < NUM_SPECIES; n++) {
      Hi[n] = Cv * T * Constants::gamma;
    }
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void RTY2Hi(
    amrex::Real /*R*/, amrex::Real T, amrex::Real* /*Y[]*/, amrex::Real Hi[])
  {
    T2Hi(T, Hi);
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void S(amrex::Real& s) { s = 1.0; }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void TY2G(amrex::Real /*T*/, amrex::Real* /*Y*/, amrex::Real& G)
  {
    G = Constants::gamma;
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void
  RTY2G(amrex::Real /*R*/, amrex::Real T, amrex::Real* Y, amrex::Real& G)
  {
    TY2G(T, Y, G);
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void
  TY2H(amrex::Real T, const amrex::Real Y[NUM_SPECIES], amrex::Real& H)
  {
    amrex::Real Hi[NUM_SPECIES];
    amrex::Real wbar = Constants::AIRMW;
    const amrex::Real Cv = Constants::RU / (wbar * (Constants::gamma - 1.0));
    H = 0.0;
    for (int n = 0; n < NUM_SPECIES; n++) {
      Hi[n] = Cv * T * Constants::gamma;
      H = H + Y[n] * Hi[n];
    }
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void RPE2dpdr_e(
    amrex::Real /*R*/, amrex::Real /*P*/, amrex::Real E, amrex::Real& dpdr_e)
  {
    dpdr_e = (Constants::gamma - 1.0) * E;
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void RG2dpde(amrex::Real R, amrex::Real /*G*/, amrex::Real& dpde)
  {
    dpde = (Constants::gamma - 1.0) * R;
  }

  AMREX_GPU_HOST_DEVICE
  AMREX_FORCE_INLINE
  static void RTY2dpde_dpdre(
    amrex::Real R,
    amrex::Real T,
    amrex::Real Y[],
    amrex::Real& dpde,
    amrex::Real& dpdr_e)
  {
    amrex::Real G, P;
    TY2G(T, Y, G);
    RTY2P(R, T, Y, P);
    dpde = (G - 1.0) * R;
    dpdr_e = P / R;
  }

  template <class... Args>
  AMREX_GPU_HOST_DEVICE explicit GammaLaw(Args... /*unused*/)
  {
  }

  amrex::Real gamma{Constants::gamma};
};

} // namespace eos
} // namespace physics
} // namespace pele
#endif
