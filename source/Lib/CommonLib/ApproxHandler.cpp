#include "ApproxHandler.h"

namespace vvenc {

Pel* ApproxHandler::approxIntraOrigBufferY;
Pel* ApproxHandler::approxIntraOrigBufferCb;
Pel* ApproxHandler::approxIntraOrigBufferCr;
const Pel* ApproxHandler::bkpIntraOrigBufferY;
const Pel* ApproxHandler::bkpIntraOrigBufferCb;
const Pel* ApproxHandler::bkpIntraOrigBufferCr;

/*************************************
 * BASELINE APPROXIMATION FUNCTIONS
 *************************************/

void ApproxHandler::allocIntraOrigSB() {
    approxIntraOrigBufferY = xMalloc(Pel, 128 * 128);
    approxIntraOrigBufferCb = xMalloc(Pel, 64 * 64);
    approxIntraOrigBufferCr = xMalloc(Pel, 64 * 64);
}

void ApproxHandler::addApproxIntraOrigSB(ComponentID comp, int approxLevel) {  
  if(comp == COMP_Y) {
    Pel* beginBufferY = approxIntraOrigBufferY;
    Pel* endBufferY = beginBufferY + (128 * 128);
    ApproxSS::add_approx((void *) beginBufferY, (void *) endBufferY, ORIG_SB_BUFFER_Y, approxLevel, sizeof(const Pel)); //Luma (1); Cb (2); Cr (3)
  }
  else if(comp == COMP_Cb) {
    Pel* beginBufferCb = approxIntraOrigBufferCb;
    Pel* endBufferCb = beginBufferCb + (64 * 64);
    ApproxSS::add_approx((void *) beginBufferCb, (void *) endBufferCb, ORIG_SB_BUFFER_CB, approxLevel, sizeof(const Pel)); //Luma (1); Cb (2); Cr (3)

  }
  else { // comp == COMP_Cr
    Pel* beginBufferCr = approxIntraOrigBufferCr;
    Pel* endBufferCr = beginBufferCr + (64 * 64);
    ApproxSS::add_approx((void *) beginBufferCr, (void *) endBufferCr, ORIG_SB_BUFFER_CR, approxLevel, sizeof(const Pel)); //Luma (1); Cb (2); Cr (3)
  } 
}

void ApproxHandler::addApproxIntraNeighSB(Pel* refBuffer, ComponentID comp, int filt, int approxLevel) {
    // size: (MAX_CU_SIZE * 2 + 1 + MAX_REF_LINE_IDX) * 2
   int bufferStride = (MAX_CU_SIZE * 2 + 1 + MAX_REF_LINE_IDX) * 2 - 1;

    Pel* beginNeighBuffer = refBuffer;
    Pel* endNeighBuffer = beginNeighBuffer + bufferStride;

    if(comp == COMP_Y) {
      if(filt == 0) {
          ApproxSS::add_approx((void *) beginNeighBuffer, (void *) endNeighBuffer, NEIGH_SB_BUFFER_Y, approxLevel, sizeof(Pel));
      }
      else {
          ApproxSS::add_approx((void *) beginNeighBuffer, (void *) endNeighBuffer, NEIGH_SB_BUFFER_Y_FILT, approxLevel, sizeof(Pel));
      }
    }
    else if(comp == COMP_Cb) {
        ApproxSS::add_approx((void *) beginNeighBuffer, (void *) endNeighBuffer, NEIGH_SB_BUFFER_CB, approxLevel, sizeof(Pel));
    }
    else {
        ApproxSS::add_approx((void *) beginNeighBuffer, (void *) endNeighBuffer, NEIGH_SB_BUFFER_CR, approxLevel, sizeof(Pel));
    }
}

void ApproxHandler::removeApproxIntraOrigSB(ComponentID comp) {
  if(comp == COMP_Y) {
    Pel* beginBufferY = approxIntraOrigBufferY;
    Pel* endBufferY = beginBufferY + (128 * 128);
    ApproxSS::remove_approx((void *) beginBufferY, (void *) endBufferY);
  }
  else if(comp == COMP_Cb) {
    Pel* beginBufferCb = approxIntraOrigBufferCb;
    Pel* endBufferCb = beginBufferCb + (64 * 64);
    ApproxSS::remove_approx((void *) beginBufferCb, (void *) endBufferCb);
  }
  else { // comp == COMP_Cr
    Pel* beginBufferCr = approxIntraOrigBufferCr;
    Pel* endBufferCr = beginBufferCr + (64 * 64);
    ApproxSS::remove_approx((void *) beginBufferCr, (void *) endBufferCr);
  } 
}

void ApproxHandler::removeApproxIntraNeighSB(Pel* refBuffer) {
  int bufferStride = (MAX_CU_SIZE * 2 + 1 + MAX_REF_LINE_IDX) * 2 - 1;

  Pel* beginNeighBuffer = refBuffer;
  Pel* endNeighBuffer = beginNeighBuffer + bufferStride;

  ApproxSS::remove_approx((void *) beginNeighBuffer, (void *) endNeighBuffer);
}

Pel* ApproxHandler::initIntraOrigSB(CPelBuf origBuffer, ComponentID comp) {  
  int bufferStride = origBuffer.stride * origBuffer.height;

  if(comp == COMP_Y) {
    bkpIntraOrigBufferY = origBuffer.buf;
    for (size_t i = 0; i < bufferStride; i++) {
      approxIntraOrigBufferY[i]  = origBuffer.buf[i];
    }
    return approxIntraOrigBufferY;
  }

  else if(comp == COMP_Cb) {
    bkpIntraOrigBufferCb = origBuffer.buf;
    for (size_t i = 0; i < bufferStride; i++) {
      approxIntraOrigBufferCb[i] = origBuffer.buf[i];
    }
    return approxIntraOrigBufferCb;
  }

  else { //comp == COMP_Cr
    bkpIntraOrigBufferCr = origBuffer.buf;
    for (size_t i = 0; i < bufferStride; i++) {
      approxIntraOrigBufferCr[i] = origBuffer.buf[i];
    }
    return approxIntraOrigBufferCr;
  }
}

const Pel* ApproxHandler::restoreIntraOrigSB(ComponentID comp) {
    if(comp == COMP_Y) {
      return bkpIntraOrigBufferY;
    }
    else if(comp == COMP_Cb) {
      return bkpIntraOrigBufferCb;
    }
    else { //comp == COMP_Cr
      return bkpIntraOrigBufferCr;
    }
}

void ApproxHandler::startGlobalLevel() {
  ApproxSS::start_level();
}

void ApproxHandler::endGlobalLevel() {
  ApproxSS::end_level();
}

}