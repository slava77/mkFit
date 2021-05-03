#ifndef MkStdSeqs_h
#define MkStdSeqs_h

#include "Hit.h"
#include "Track.h"

namespace mkfit {

class Event;
class EventOfHits;

class IterationConfig;

namespace StdSeq
{
    void LoadHits(Event &ev, EventOfHits &eoh);

    void Cmssw_LoadHits_Begin(EventOfHits &eoh, const std::vector<const HitVec*> &orig_hitvectors);
    void Cmssw_LoadHits_End(EventOfHits &eoh);

    // Not used anymore. Left here if we want to experiment again with
    // COPY_SORTED_HITS in class LayerOfHits.
    void Cmssw_Map_TrackHitIndices(const EventOfHits &eoh, TrackVec &seeds);
    void Cmssw_ReMap_TrackHitIndices(const EventOfHits &eoh, TrackVec &out_tracks);

    int clean_cms_seedtracks_iter(TrackVec *seed_ptr, const IterationConfig& itrcfg);
    
    void find_duplicates(TrackVec &tracks);
    void remove_duplicates(TrackVec &tracks);
    void handle_duplicates(Event *m_event);
      
    void quality_filter(TrackVec &tracks, TrackVec & seeds, const int nMinHits=4, const int algo=9);
    void find_duplicates_sharedhits(TrackVec &tracks,  TrackVec & seeds, const float fraction=0.11, const int algo=9);

} // namespace StdSeq

}

#endif