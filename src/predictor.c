//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Allison Turner && James Yuan";
const char *studentID   = "A59008979 &&";
const char *email       = "aturner@ucsd.edu && z2yuan@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//

const unsigned strong_taken = 0x11;
const unsigned weak_taken = 0x10;
const unsigned weak_not_taken = 0x01;
const unsigned strong_not_taken = 0x00;


//Gshare: global with index sharing
//Similar to gselect predictor, except the branch address and global history are combined by an XOR into a 2-bit prediction
unsigned global_history;
unsigned* branch_history_table;

//size of unsigned int in bytes
unsigned unsigned_size = sizeof(unsigned);
//and in bits
unsigned unsigned_bits = sizeof(unsigned) * 8;

unsigned ghistory_mask;
unsigned bht_entry_mask = 0x11;

//Tournament uses simple BHT for history-independent branches and correlated predictor for branches that need history
//chooser starts branches in simple BHT, move to correlated predictor once a mis-prediction threshold is crossed


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  switch (bpType){

    case GSHARE:
      global_history = 0x0 << ghistoryBits;
      ghistory_mask = (0x1 << ghistoryBits) - 0x1;

      //printf("Global History Mask: %x\n", ghistory_mask);

      unsigned num_bht_entries = 0x1 << ghistoryBits;

      //printf("Num BHT Entries: %x\n", num_bht_entries);

      unsigned bht_size = num_bht_entries * unsigned_size;

      //printf("BHT Size: %x\n", bht_size);

      branch_history_table = (unsigned*)malloc(bht_size);

      int iterator;
      for (iterator = 0; iterator < num_bht_entries; iterator++){
        //printf("Iterator: %i\n", iterator);
        branch_history_table[iterator] = weak_not_taken;
      }
      break;

    case TOURNAMENT:
      break;

    //custom TBD
    case CUSTOM:
      break;
      
    default:
      break;
  }
}



uint8_t
gshare_make_prediction(uint32_t pc)
{
  unsigned xor = ghistory_mask & (pc ^ global_history);
  unsigned prediction = branch_history_table[xor] & bht_entry_mask;

  if ((prediction & strong_taken) || (prediction & weak_taken)){
    return TAKEN;
  }
  else{
    return NOTTAKEN;
  }
}

uint8_t
tournament_make_prediction(uint32_t pc)
{
  return NOTTAKEN;
}

uint8_t
custom_make_prediction(uint32_t pc)
{
  return NOTTAKEN;
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
//args
//pc: unsigned 32-bit integer, represents program counter value
//
//returns
//unsigned 8-bit integer macro TAKEN or NOTTAKEN
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;

    case GSHARE:
      return gshare_make_prediction(pc);

    case TOURNAMENT:
      return tournament_make_prediction(pc);

    case CUSTOM:
      return custom_make_prediction(pc);

    default:
      break;
  }

  // If there is not a compatible bpType then return NOTTAKEN
  return NOTTAKEN;
}



void
gshare_train_predictor(uint32_t pc, uint8_t outcome){
  unsigned bht_entry_index = ghistory_mask & (pc ^ global_history);
  uint16_t prediction = branch_history_table[bht_entry_index] & bht_entry_mask;

  //NOTTAKEN
  if(outcome == 0){
    
    if(prediction == weak_not_taken){
      branch_history_table[bht_entry_index] = strong_not_taken;
    }
    else if (prediction == weak_taken)
    {
      branch_history_table[bht_entry_index] = weak_not_taken;
    }
    else if (prediction == strong_taken){
      branch_history_table[bht_entry_index] = weak_taken;
    }

    //cut off first bit of global hist
    unsigned temp = ghistory_mask & (global_history << 1);

    //make sure last bit is 0
    temp = temp & 0x0;
    //printf("global history: %x\n", temp);

    //set global hist to new val
    global_history = temp;

  }
  //TAKEN
  else{
    
    if(prediction == strong_not_taken){
      branch_history_table[bht_entry_index] = weak_not_taken;
    }
    else if (prediction == weak_not_taken)
    {
      branch_history_table[bht_entry_index] = weak_taken;
    }
    else if (prediction == weak_taken){
      branch_history_table[bht_entry_index] = strong_taken;
    }

    //cut off first bit of global hist
    unsigned temp = ghistory_mask & (global_history << 1);
    
    //make sure last bit is 1
    temp = temp | 0x1;
    //printf("global history: %x\n", temp);

    //set global hist to new val
    global_history = temp;
  }
}

void
tournament_train_predictor(uint32_t pc, uint8_t outcome){

}

void
custom_train_predictor(uint32_t pc, uint8_t outcome){

}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  switch (bpType) {
    case GSHARE:
      gshare_train_predictor(pc, outcome);

    case TOURNAMENT:
      tournament_train_predictor(pc, outcome);

    case CUSTOM:
      custom_train_predictor(pc, outcome);

    default:
      break;
  }
}

void
cleanup(){
  switch (bpType) {
    case GSHARE:
      free(branch_history_table);
      break;

    case TOURNAMENT:
      
      break;

    case CUSTOM:
      break;

    default:
      break;
  }
}