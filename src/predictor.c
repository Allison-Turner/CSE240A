//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <stdlib.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Allison Turner && James Yuan";
const char *studentID   = "A59008979 && A15707224";
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

//size of unsigned int in bytes
unsigned unsigned_size = sizeof(unsigned);
//and in bits
unsigned unsigned_bits = sizeof(unsigned) * 8;

//Gshare: global with index sharing
//Similar to gselect predictor, except the branch address and global history are combined by an XOR into a 2-bit prediction
unsigned global_history;
unsigned* branch_history_table;

unsigned ghistory_mask;
unsigned bht_entry_mask = 0x11;

//Tournament uses simple BHT for history-independent branches and correlated predictor for branches that need history
//chooser starts branches in simple BHT, move to correlated predictor once a mis-prediction threshold is crossed
const unsigned SIMPLE_BHT = 0x0;
const unsigned CORRELATED_PREDICTOR = 0x1;

unsigned* chooser;
unsigned* total_predictions;
unsigned* local_mispredictions;
unsigned* global_mispredictions;
unsigned chooser_entry_mask;

unsigned* local_pattern_hist;
unsigned* pattern_hist_predictor_state;
unsigned local_pattern_history_mask;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

uint8_t
predictor_state_to_binary_prediction(unsigned predictor_state){
  if((predictor_state == strong_not_taken) || (predictor_state == weak_not_taken)){
    return NOTTAKEN;
  }
  else if ((predictor_state == strong_taken) || (predictor_state == weak_taken)){
    return TAKEN;
  }
}

unsigned
update_predictor_state(uint8_t outcome, unsigned predictor_state){
  if(predictor_state == strong_not_taken){
    if(outcome == TAKEN){
      return weak_not_taken;
    }
    else{
      return strong_not_taken;
    }
  }
  else if(predictor_state == weak_not_taken){
    if(outcome == TAKEN){
      return weak_taken;
    }
    else{
      return strong_not_taken;
    }
  }
  else if(predictor_state == weak_taken){
    if(outcome == TAKEN){
      return strong_taken;
    }
    else{
      return weak_taken;
    }
  }
  else if(predictor_state == strong_taken){
    if(outcome == TAKEN){
      return strong_taken;
    }
    else{
      return weak_taken;
    }
  }
}

void
init_gshare()
{
  global_history = (0x0 << ghistoryBits);
  ghistory_mask = ((0x0 << (unsigned_bits - 0x1)) | ((0x1 << ghistoryBits) - 0x1));

  unsigned num_bht_entries = (0x1 << ghistoryBits);
  unsigned bht_size = (num_bht_entries * unsigned_size);

  branch_history_table = (unsigned*)malloc(bht_size);

  int iterator;
  for (iterator = 0; iterator < num_bht_entries; iterator++){
    branch_history_table[iterator] = weak_not_taken;
  }
}

void
init_tournament()
{
  //init chooser
  unsigned num_entries = (0x1 << pcIndexBits);
  unsigned chooser_size = (num_entries * unsigned_size);
  
  chooser = (unsigned*)malloc(chooser_size);
  total_predictions = (unsigned*)malloc(chooser_size);
  local_mispredictions = (unsigned*)malloc(chooser_size);
  global_mispredictions = (unsigned*)malloc(chooser_size);
      

  int iterator;
  for(iterator = 0; iterator < num_entries; iterator++){
    chooser[iterator] = SIMPLE_BHT;
    total_predictions[iterator] = 0;
    local_mispredictions[iterator] = 0;
    global_mispredictions[iterator] = 0;
  } 

  chooser_entry_mask = ((0x0 << (unsigned_bits - 0x1)) | ((0x1 << pcIndexBits) - 0x1));

  //init local predictor
  local_pattern_history_mask = ((0x1 << lhistoryBits) - 0x1);

  local_pattern_hist = (unsigned*)malloc(chooser_size);
  for(iterator = 0; iterator < num_entries; iterator++){
    local_pattern_hist[iterator] = (0x0 << lhistoryBits);
  }  

  unsigned pattern_hist_predictors = (0x1 << lhistoryBits);
  unsigned pht_size = (pattern_hist_predictors * unsigned_size);
  
  pattern_hist_predictor_state = (unsigned*)malloc(pht_size);

  for(iterator = 0; iterator < pattern_hist_predictors; iterator++){
    pattern_hist_predictor_state[iterator] = weak_not_taken;
  }  

  //init global predictor
  init_gshare();
}

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
      init_gshare();
      break;

    case TOURNAMENT:
      init_tournament();
      break;

    //custom TBD
    case CUSTOM:
      FILE *fp = fopen("his.txt", "w");
      fclose(fp);
      break;
      
    default:
      break;
  }
}

uint8_t
gshare_make_prediction(uint32_t pc)
{
  unsigned xor = ghistory_mask & (pc ^ global_history);
  return predictor_state_to_binary_prediction(branch_history_table[xor] & bht_entry_mask);
}

uint8_t
tournament_make_prediction(uint32_t pc)
{
  unsigned chooser_entry = pc & chooser_entry_mask;

  if(chooser[chooser_entry] == SIMPLE_BHT){
    unsigned local_history_pattern = local_pattern_hist[chooser_entry];
    return predictor_state_to_binary_prediction(pattern_hist_predictor_state[local_history_pattern]);
  }
  else{
    return gshare_make_prediction(pc);
  }
}

uint8_t
custom_make_prediction(uint32_t pc)
{
  //int myPrediction = make_prediction(pc, ghistoryBits, lhistoryBits);
  FILE *fp;
  char *commandLine;
  sprintf(commandLine, "./NN.py predict %d %d %d", pc, ghistoryBits, lhistoryBits);
  fp = popen(commandLine, "r");
  char *predict;
  fgets(predict, 60, fp);
  pclose(fp);
  return atoi(predict);
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
  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
      break;

    case GSHARE:
      return gshare_make_prediction(pc);
      break;

    case TOURNAMENT:
      return tournament_make_prediction(pc);
      break;

    case CUSTOM:
      return custom_make_prediction(pc);
      break;

    default:
      break;
  }

  // If there is not a compatible bpType then return NOTTAKEN
  return NOTTAKEN;
}



void
gshare_train_predictor(uint32_t pc, uint8_t outcome){
  unsigned bht_entry_index = ghistory_mask & (pc ^ global_history);
  uint16_t predictor_state = branch_history_table[bht_entry_index] & bht_entry_mask;

  branch_history_table[bht_entry_index] = update_predictor_state(outcome, predictor_state);

  //cut off first bit of global hist
  unsigned temp = ghistory_mask & (global_history << 1);

  //NOTTAKEN
  if(outcome == 0){
    //make sure last bit is 0
    temp = temp & ((0x1 << ghistoryBits) - 0x10);
  }
  //TAKEN
  else{
    //make sure last bit is 1
    temp = temp + 0x1;
  }

  //set global hist to new val
  global_history = temp;
}

void
tournament_train_predictor(uint32_t pc, uint8_t outcome){
  unsigned chooser_entry = pc & chooser_entry_mask;

  unsigned local_history_pattern = local_pattern_hist[chooser_entry];
  unsigned local_predictor_state = pattern_hist_predictor_state[local_history_pattern];
  unsigned local_prediction = predictor_state_to_binary_prediction(local_predictor_state);

  unsigned xor = ghistory_mask & (pc ^ global_history);
  unsigned global_predictor_state = branch_history_table[xor] & bht_entry_mask;
  unsigned global_prediction = predictor_state_to_binary_prediction(global_predictor_state);

  total_predictions[chooser_entry] += 1;

  //Track mispredictions
  if(local_prediction != outcome){
    local_mispredictions[chooser_entry] += 1;
  }

  if(global_prediction != outcome){
    global_mispredictions[chooser_entry] += 1;
  }

  double local_error  = ((double)local_mispredictions[chooser_entry])  / ((double)total_predictions[chooser_entry]);
  double global_error = ((double)global_mispredictions[chooser_entry]) / ((double)total_predictions[chooser_entry]);

  if(local_error < global_error){
    chooser[chooser_entry] = SIMPLE_BHT;
  }
  else if(global_error < local_error){
    chooser[chooser_entry] = CORRELATED_PREDICTOR;
  }

  //update local predictor
  pattern_hist_predictor_state[local_history_pattern] = update_predictor_state(outcome, local_predictor_state);
  local_pattern_hist[chooser_entry] = ((local_history_pattern << 1) | (outcome >> lhistoryBits)) & local_pattern_history_mask;

  //update global predictor
  gshare_train_predictor(pc, outcome);
}

void
custom_train_predictor(uint32_t pc, uint8_t outcome){
  FILE *fp;
  fp = fopen("his.txt", "a");
  fprintf(fp, "%d %d %d %d\n", pc, ghistoryBits, lhistoryBits, outcome);
  fclose(fp);
  // char *commandLine;
  // sprintf(commandLine, "./NN.py train %d %d %d %d", pc, ghistoryBits, lhistoryBits, outcome);
  fp = popen("./NN.py train", "r");
  pclose(fp);
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
      break;

    case TOURNAMENT:
      tournament_train_predictor(pc, outcome);
      break;

    case CUSTOM:
      custom_train_predictor(pc, outcome);
      break;

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
      free(chooser);
      free(total_predictions);
      free(local_mispredictions);
      free(global_mispredictions);
      free(local_pattern_hist);
      free(pattern_hist_predictor_state);
      free(branch_history_table);
      break;

    case CUSTOM:
      break;

    default:
      break;
  }
}