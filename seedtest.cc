#include "seedtest.h"
#include "Hit.h"
#include "Event.h"
#include "ConformalUtils.h"
#include "KalmanUtils.h"
#include "Propagation.h"
//#define DEBUG
#include "Debug.h"

void buildSeedsByMC(const TrackVec& evt_sim_tracks, TrackVec& evt_seed_tracks, Event& ev){
  bool debug(true);
  for (unsigned int itrack=0;itrack<evt_sim_tracks.size();++itrack) {
    const Track& trk = evt_sim_tracks[itrack];
    int   seedhits[Config::nLayers];
    float chi2 = 0;
    TrackState updatedState = trk.state();
    dprint("processing sim track # " << trk.seedID() << " par=" << trk.parameters());

    TSLayerPairVec updatedStates; // validation for position pulls

    for (auto ilayer=0U;ilayer<Config::nlayers_per_seed;++ilayer) {//seeds have first three layers as seeds
      auto hitidx = trk.getHitIdx(ilayer);
      const Hit& seed_hit = ev.layerHits_[ilayer][hitidx];
      TrackState propState = propagateHelixToR(updatedState,seed_hit.r());
      MeasurementState measState = seed_hit.measurementState();
      updatedState = updateParameters(propState, measState);
      seedhits[ilayer] = hitidx;
      //      chi2 += computeChi2(updatedState,measState); --> could use this to make the chi2

      updatedStates.push_back(std::make_pair(ilayer,updatedState)); // validation
    }
    ev.validation_.collectSeedTkTSLayerPairVecMapInfo(itrack,updatedStates); // use to collect position pull info

    Track seed(updatedState,0.,itrack,Config::nlayers_per_seed,seedhits);//fixme chi2
    dprint("created seed track # " << seed.seedID() << " par=" << seed.parameters());
    evt_seed_tracks.push_back(seed);
  }
}

#ifdef BUILDSEEDS
void buildSeedsByRoadTriplets(const std::vector<HitVec>& evt_lay_hits, const BinInfoMap& segmentMap, TrackVec& evt_seed_tracks, Event& ev){
  bool debug(false);

  std::vector<HitVec> hitPairs;   // first will be pairs, then triplets, then filtered chi2 triplets, then Conf fit, then KF fit
  buildHitPairs(evt_lay_hits,segmentMap[0],hitPairs); // pass only first layer map ... no need to pass the whole thing!

#ifdef DEBUG  
  if (debug){
    std::cout << "Hit Pairs" << std::endl;
    for(auto&& hitPair : hitTriplets){
      printf("ilay0: %1u ilay1: %1u  \n",
	     hitPair[0].mcTrackID(),
	     hitPair[1].mcTrackID()
	     );
    }
    std::cout << std::endl;
  }
#endif

  std::vector<HitVec> hitTriplets;
  buildHitTriplets(evt_lay_hits,segmentMap[2],hitPairs,hitTriplets);

#ifdef DEBUG
  if (debug) {
    std::cout << "Hit Triplets" << std::endl;
    for(auto&& hitTriplet : hitTriplets){
      printf("ilay0: %1u ilay1: %1u ilay2: %1u \n",
	     hitTriplet[0].mcTrackID(),
	     hitTriplet[1].mcTrackID(),
	     hitTriplet[2].mcTrackID()
	     );
    }
  }
#endif
  
  std::vector<HitVec> filteredTriplets;
  filterHitTripletsByRZChi2(hitTriplets, filteredTriplets); // filter based on RZ chi2 cut
  buildSeedsFromTriplets(filteredTriplets,evt_seed_tracks,ev);
}

void buildHitPairs(const std::vector<HitVec>& evt_lay_hits, const BinInfoLayerMap& segLayMap, std::vector<HitVec>& hit_pairs){
  const float dZ = 1.0; // 3.25 - 3.3
  const float innerrad = 4.0; // average radius of inner radius
  const float alphaBeta = 0.0520195; // 0.0458378 --> for d0 = .0025 cm --> analytically derived

  for (unsigned int ihit=0;ihit<evt_lay_hits[1].size();++ihit) { // 1 = second layer
    const float outerhitz = evt_lay_hits[1][ihit].z(); // remember, layer[0] is first layer! --> second layer = [1]
    const float outerphi  = evt_lay_hits[1][ihit].phi();

#ifdef ETASEG
    const auto etaBinMinus = getEtaPartition(normalizedEta(getEta(innerrad,(outerhitz-dZ)/2.)));
    const auto etaBinPlus  = getEtaPartition(normalizedEta(getEta(innerrad,(outerhitz+dZ)/2.)));
#else
    const auto etaBinMinus = 0U;
    const auto etaBinPlus  = 0U;
#endif
    const auto phiBinMinus = getPhiPartition(normalizedPhi(outerphi - alphaBeta));
    const auto phiBinPlus  = getPhiPartition(normalizedPhi(outerphi + alphaBeta));

    std::vector<unsigned int> cand_hit_indices = getCandHitIndices(etaBinMinus,etaBinPlus,phiBinMinus,phiBinPlus,segLayMap);
    for (auto&& cand_hit_idx : cand_hit_indices){
      HitVec hit_pair;
      hit_pair.push_back(evt_lay_hits[0][cand_hit_idx]);
      hit_pair.push_back(evt_lay_hits[1][ihit]);
      hit_pairs.push_back(hit_pair);
    }
  }
}  

void buildHitTriplets(const std::vector<HitVec>& evt_lay_hits, const BinInfoLayerMap& segLayMap, const std::vector<HitVec>& hit_pairs, std::vector<HitVec>& hit_triplets){
  const float dEta     = 0.6; // for almost max efficiency --> empirically derived
  const float dPhi     = 0.0458712; // numerically+semianalytically derived
  const float thirdRad = 12.0; // average third radius

  for (auto&& hit_pair : hit_pairs){
#ifdef ETASEG
    const float thirdZline = 2*hit_pair[1].z()-hit_pair[0].z(); // for dz displacements -- straight line window
    const auto etaBinMinus = getEtaPartition(normalizedEta(getEta(thirdRad,thirdZline)-dEta));
    const auto etaBinPlus  = getEtaPartition(normalizedEta(getEta(thirdRad,thirdZline)+dEta));
#else
    const auto etaBinMinus = 0U;
    const auto etaBinPlus  = 0U;
#endif    
    const float linePhi = getPhi(hit_pair[1].position()[0] - hit_pair[0].position()[0], hit_pair[1].position()[1] - hit_pair[0].position()[1]);
    float thirdPhiMinus = 0.0;
    float thirdPhiPlus  = 0.0;  
    if (hit_pair[0].phi() < hit_pair[1].phi() ){
      thirdPhiMinus = normalizedPhi(linePhi - dPhi);
      thirdPhiPlus  = normalizedPhi(linePhi);
    }
    else{
      thirdPhiMinus = normalizedPhi(linePhi);
      thirdPhiPlus  = normalizedPhi(linePhi + dPhi);
    }
    const auto phiBinMinus = getPhiPartition(thirdPhiMinus);
    const auto phiBinPlus  = getPhiPartition(thirdPhiPlus);

    std::vector<unsigned int> cand_hit_indices = getCandHitIndices(etaBinMinus,etaBinPlus,phiBinMinus,phiBinPlus,segLayMap);
    for (auto&& cand_hit_idx : cand_hit_indices){
      HitVec hit_triplet;
      hit_triplet.push_back(hit_pair[0]);
      hit_triplet.push_back(hit_pair[1]);
      hit_triplet.push_back(evt_lay_hits[2][cand_hit_idx]);      
      hit_triplets.push_back(hit_triplet);
    }
  }
}

void filterHitTripletsByRZChi2(const std::vector<HitVec>& hit_triplets, std::vector<HitVec>& filtered_triplets){
  // Seed cleaning --> do some chi2 fit for RZ line then throw away with high chi2
  // choose ind = r, dep = z... could do total chi2 but, z errors 10*r
  // A = y-int, B = slope

  // res on z is set to be hitposerrZ*hitposerrZ
  const float varZ    = 0.1*0.1; // from simulation ... will need to change if that changes!
  const float invsig2 = 3.*(1./varZ);

  for (auto&& hit_triplet : hit_triplets){
    // first do fit for three hits
    float sumx2sig2 = 0;
    float sumxsig2  = 0;
    float sumysig2  = 0;
    float sumxysig2 = 0;
    for (auto&& seedhit : hit_triplet) { 
      sumx2sig2 += ((seedhit.r())*(seedhit.r()) / varZ);
      sumxsig2  += (seedhit.r() / varZ);
      sumysig2  += (seedhit.z() / varZ);
      sumxysig2 += ((seedhit.r())*(seedhit.z()) / varZ);
    }
    const float norm   = 1./ ((invsig2*sumx2sig2) - (sumxsig2*sumxsig2));
    const float aParam = norm * ((sumx2sig2 * sumysig2) - (sumxsig2*sumxysig2));
    const float bParam = norm * ((invsig2 * sumxysig2)  - (sumxsig2*sumysig2));
 
    float chi2fit = 0;     
    for (auto&& seedhit : hit_triplet) { //now perform chi2 on fit!
      chi2fit += pow((seedhit.z() - aParam - (bParam * seedhit.r())),2) / varZ;
    }

    if (chi2fit<Config::chi2seedcut){ // cut empirically derived
      filtered_triplets.push_back(hit_triplet);
    }
  }
}

void buildSeedsFromTriplets(const std::vector<HitVec> & filtered_triplets, TrackVec & evt_seed_tracks, Event& ev){
  // now perform kalman fit on seeds --> first need initial parameters --> get from Conformal fitter!
  const bool backward = false; // use for forward fit of conformal utils
  const bool fiterrs  = false; // use errors derived for seeding

  unsigned int seedID = 0;
  for(auto&& hit_triplet : filtered_triplets){
    int charge = 0;
    if (hit_triplet[1].phi() > hit_triplet[2].phi()){charge = 1;}
    else {charge = -1;}

    TrackState updatedState;
    conformalFit(hit_triplet[0],hit_triplet[1],hit_triplet[2],charge,updatedState,backward,fiterrs); 
    ev.validation_.collectSeedTkCFMapInfo(seedID,updatedState);

    TSLayerPairVec updatedStates; // validation for position pulls
    
    for (auto ilayer=0U;ilayer<Config::nlayers_per_seed;++ilayer) {
      Hit seed_hit = hit_triplet[ilayer];
      TrackState propState = propagateHelixToR(updatedState,seed_hit.r());
#ifdef CHECKSTATEVALID
      if (!propState.valid) {
        break;
      }
#endif
      MeasurementState measState = seed_hit.measurementState();
      updatedState = updateParameters(propState, measState);
      //      chi2 += computeChi2(updatedState,measState); --> could use this to make the chi2

      updatedStates.push_back(std::make_pair(ilayer,updatedState)); // validation
    }
    ev.validation_.collectSeedTkTSLayerPairVecMapInfo(seedID,updatedStates); // use to collect position pull info

    Track seed(updatedState,hit_triplet,0.,seedID);//fixme chi2 
    evt_seed_tracks.push_back(seed);
    seedID++; // increment dummy counter for seedID
  }
}
#endif