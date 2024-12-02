#include "cpu/lvp/load_value_prediction_table.hh"

#include "base/intmath.hh"
#include "base/trace.hh"
#include "debug/LVPT.hh"

namespace gem5
{

LoadValuePredictionTable::LoadValuePredictionTable(const LoadValuePredictionTableParams &params)
    : SimObject(params),
      numEntries(params.entries),
      historyDepth(params.historyDepth),
      idxMask(numEntries - 1),
      instShiftAmt(0) 

{
    DPRINTF(LVPT, "LVPT: Creating LVPT object.\n");

    if (!isPowerOf2(numEntries)) {
        fatal("LVPT entries is not a power of 2!");
    }

    LVPT.resize(numEntries);

    DPRINTF(LVPT, "LVPT: Doing an initial reset \n");
    for (unsigned i = 0; i < numEntries; ++i) {
        LVPT[i] = LVPTEntry(historyDepth);
    }

    idxMask = numEntries - 1;
    tagMask = (1 << tagBits) - 1;
    tagShiftAmt = instShiftAmt + floorLog2(numEntries);
}

/* Reset API */
void
LoadValuePredictionTable::reset()
{
    DPRINTF(LVPT, "LVPT : Calling the Reset API \n");
    for (unsigned i = 0; i < numEntries; ++i) {
        LVPT[i].valid = false;
    }
}

/* APIs to get index and tag*/
unsigned
LoadValuePredictionTable::getIndex(Addr instPC, ThreadID tid)
{
    // Need to shift PC over by the word offset.
    // Math: ((instPC >> instShiftAmt)^(tid<<(tagShiftAmt-instShiftAmt-log2NumThreads)))&idxMask;
    DPRINTF(LVPT, "LVPT : Getting Index \n");
    return ((instPC >> instShiftAmt)
            ^ (tid << (tagShiftAmt - instShiftAmt - log2NumThreads)))
            & idxMask;
}

inline
Addr
LoadValuePredictionTable::getTag(Addr instPC)
{
    DPRINTF(LVPT, "LVPT : Getting Tag \n");
    return (instPC >> tagShiftAmt) & tagMask;
}

/** Checks if the load entry is in the LVPT.i **/
bool
LoadValuePredictionTable::valid(Addr instPC, ThreadID tid)
{
    unsigned LVPT_idx = getIndex(instPC, tid);


    // Making sure index doesn't go out of bounds
    assert(LVPT_idx < numEntries);

    // Check if: (a) LVPT entry is valid
    // (b) index matches
    // (c) tag matches
    if (LVPT[LVPT_idx].valid
        && LVPT[LVPT_idx].tid == tid) {
        return true;
    } else {
        DPRINTF(LVPT, "LVPT : Checking if LVPT entry is valid \n");
        return false;
    }
}

// data = 0 represent invalid entry.
RegVal 
LoadValuePredictionTable::lookup(ThreadID tid, Addr instPC, bool *lvptResultValid)
{
    unsigned LVPT_idx = getIndex(instPC, tid);

    assert(LVPT_idx < numEntries);

    if (valid(instPC, tid)) {
        DPRINTF(LVPT, "Found valid entry for tid: %d at pc %#x : %#x \n", 
            tid, instPC, LVPT[LVPT_idx].history.back());
        *lvptResultValid = true;
        return LVPT[LVPT_idx].history.back();
    } else {
        DPRINTF(LVPT, "Did not find valid entry for tid: %d at address %#x \n", 
            tid, instPC);
        return 0;
    }
}

RegVal 
LoadValuePredictionTable::strideLookup(ThreadID tid, Addr instPC, bool *lvptResultValid){
    unsigned LVPT_idx = getIndex(instPC, tid);

    assert(LVPT_idx < numEntries);
    assert(historyDepth > 1);

    if (valid(instPC, tid)) {
        DPRINTF(LVPT, "Found valid entry for tid: %d at pc %#x : %d \n", 
            tid, instPC, LVPT[LVPT_idx].history.back());
        *lvptResultValid = true;
        if (LVPT[LVPT_idx].history[LVPT[LVPT_idx].history.size()-2] == 0){
            return LVPT[LVPT_idx].history.back();
        }
        else{
            RegVal stride = LVPT[LVPT_idx].history.back() - LVPT[LVPT_idx].history[LVPT[LVPT_idx].history.size()-2];
            return LVPT[LVPT_idx].history.back() + stride;
        }
        
    } else {
        DPRINTF(LVPT, "Did not find valid entry for tid: %d at address %#x \n", 
            tid, instPC);
        return 0;
    }
}

RegVal
LoadValuePredictionTable::getStride(ThreadID tid, Addr instPC){
    unsigned LVPT_idx = getIndex(instPC, tid);

    assert(LVPT_idx < numEntries);
    assert(historyDepth > 1);

    if (valid(instPC, tid)) {
        if (LVPT[LVPT_idx].history.size() < 2) {
            return 0;
        }
        return LVPT[LVPT_idx].history.back() - LVPT[LVPT_idx].history[LVPT[LVPT_idx].history.size()-2];
    } else {
        return 0;
    }
}

void
//prajyotg :: updated :: LoadValuePredictionTable::update(Addr instPC, const TheISA::PCState &target, ThreadID tid)
LoadValuePredictionTable::update(Addr instPC, const RegVal target, ThreadID tid)
{
    unsigned LVPT_idx = getIndex(instPC, tid);
    DPRINTF(LVPT, "LVPT : Updating the value in the LVPT at index %ld \n", LVPT_idx);

    assert(LVPT_idx < numEntries);

    LVPT[LVPT_idx].tid = tid;
    LVPT[LVPT_idx].valid = true;
    LVPT[LVPT_idx].history.push_back(target);
    LVPT[LVPT_idx].tag = getTag(instPC);
}


} // namespace gem5
