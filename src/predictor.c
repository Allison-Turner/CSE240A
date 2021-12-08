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
const char *studentName = "NAME";
const char *studentID   = "PID";
const char *email       = "EMAIL";

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
uint32_t history_btb [32];


//Gshare: global with index sharing
//Similar to gselect predictor, except the branch address and global history are combined by an XOR into a 2-bit prediction


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
      ghistoryBits = 0;
      lhistoryBits = 0;
      pcIndexBits = 0;

    case TOURNAMENT:
      ghistoryBits = 0;
      lhistoryBits = 0;
      pcIndexBits = 0;

    //custom TBD
    case CUSTOM:
      ghistoryBits = 0;
      lhistoryBits = 0;
      pcIndexBits = 0;

    default:
    break;
  }
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

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

uint8_t
gshare_make_prediction(uint32_t pc)
{
  return NOTTAKEN;
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
gshare_train_predictor(uint32_t pc, uint8_t outcome){

}

void
tournament_train_predictor(uint32_t pc, uint8_t outcome){

}

void
custom_train_predictor(uint32_t pc, uint8_t outcome){

}