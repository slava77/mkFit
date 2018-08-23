#ifndef _PlotMEIFBenchmarks_
#define _PlotMEIFBenchmarks_

#include "Common.hh"

#include "TGraph.h"

struct EventOpts
{
  EventOpts() {}
  EventOpts(const Int_t nev, const Color_t color)
    : nev(nev), color(color) {}

  Int_t nev;
  Color_t color;
};
typedef std::vector<EventOpts> EOVec;

namespace
{
  EOVec events;
  void setupEvents(const ArchEnum ARCH)
  {
    // N.B.: Consult ./xeon_scripts/benchmark-cmssw-ttbar-fulldet-build.sh for matching MEIF to arch

    events.emplace_back(1,kBlack);
    events.emplace_back(2,kBlue);
    events.emplace_back(4,kGreen+1);
    events.emplace_back(8,kRed);
    events.emplace_back((ARCH==SNB?12:16),kMagenta);
    
    if (ARCH == KNL || ARCH == SKL)
    {
      events.emplace_back(32,kAzure+10);
      events.emplace_back(64,kOrange+3);
    }
    if (ARCH == KNL)
    {
      events.emplace_back(128,kViolet-1);
    }
  }
};

typedef std::vector<TGraph*> TGVec;

class PlotMEIFBenchmarks
{
public:
  PlotMEIFBenchmarks(const TString & arch, const TString & sample, const TString & build);
  ~PlotMEIFBenchmarks();
  void RunMEIFBenchmarkPlots();
  void MakeOverlay(const TString & text, const TString & title, const TString & xtitle, const TString & ytitle, 
		   const Double_t xmin, const Double_t xmax, const Double_t ymin, const Double_t ymax);

private:
  const TString arch;
  const TString sample;
  const TString build;

  ArchEnum ARCH;
  TFile * file;
};

#endif
