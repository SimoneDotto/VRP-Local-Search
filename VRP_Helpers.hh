// File VRP_Helpers.hh
#ifndef VRP_HELPERS_HH
#define VRP_HELPERS_HH

#include "VRP_Basics.hh"
#include <easylocal.hh>

using namespace EasyLocal::Core;

/***************************************************************************
 * State Manager 
 ***************************************************************************/

class VRP_StateManager : public StateManager<VRP_Input,VRP_State> 
{
public:
  VRP_StateManager(const VRP_Input & pin); 
  void RandomState(VRP_State&);   
  bool CheckConsistency(const VRP_State& st) const;
  void GreedyState(VRP_State&);
protected:
  //void ResetState(VRP_State& st);
}; 

class VRP_DistanceCost : public CostComponent<VRP_Input,VRP_State> 
{
public:
  VRP_DistanceCost(const VRP_Input & in, double w, bool hard) 
    : CostComponent<VRP_Input,VRP_State>(in, w, hard, "VRP_DistanceCost") 
  {}
  double ComputeCost(const VRP_State& st) const;
  void PrintViolations(const VRP_State& st, ostream& os = cout) const;
};


class  VRP_Premi: public CostComponent<VRP_Input,VRP_State> 
{
public:
  VRP_Premi(const VRP_Input & in, double w, bool hard) 
    : CostComponent<VRP_Input,VRP_State>(in, w, hard, "VRP_Premi") 
  {}
  double ComputeCost(const VRP_State& st) const;
  void PrintViolations(const VRP_State& st, ostream& os = cout) const;
};


/***************************************************************************
 * VRP_Move Neighborhood Explorer:
 ***************************************************************************/

class VRP_MoveDeltaDistanceCost
  : public DeltaCostComponent<VRP_Input,VRP_State,VRP_Move1>
{
public:
  VRP_MoveDeltaDistanceCost(const VRP_Input & in, VRP_DistanceCost& cc) 
    : DeltaCostComponent<VRP_Input,VRP_State,VRP_Move1>(in, cc, "VRP_MoveDeltaDistanceCost") 
  {}
  double ComputeDeltaCost(const VRP_State& st, const VRP_Move1& mv) const;
};

class VRP_MoveDeltaPremi
  : public DeltaCostComponent<VRP_Input,VRP_State,VRP_Move1>
{
public:
  VRP_MoveDeltaPremi(const VRP_Input & in, VRP_Premi& cc) 
    : DeltaCostComponent<VRP_Input,VRP_State,VRP_Move1>(in, cc, "VRP_MoveDeltaPremi") 
  {}
  double ComputeDeltaCost(const VRP_State& st, const VRP_Move1& mv) const;
};


class VRP_Move1NeighborhoodExplorer
  : public NeighborhoodExplorer<VRP_Input,VRP_State,VRP_Move1> 
{
public:
  VRP_Move1NeighborhoodExplorer(const VRP_Input & pin, StateManager<VRP_Input,VRP_State>& psm)  
    : NeighborhoodExplorer<VRP_Input,VRP_State,VRP_Move1>(pin, psm, "VRP_Move1NeighborhoodExplorer") {} 
  void RandomMove(const VRP_State& st, VRP_Move1& mv) const;          
  bool FeasibleMove(const VRP_State& st, const VRP_Move1& mv) const;  
  void MakeMove(VRP_State& st, const VRP_Move1& mv) const;             
  void FirstMove(const VRP_State& st, VRP_Move1& mv) const;  
  bool NextMove(const VRP_State& st, VRP_Move1& mv) const;   
};


class VRP_Move2NeighborhoodExplorer
  : public NeighborhoodExplorer<VRP_Input,VRP_State,VRP_Move2> 
{
public:
  VRP_Move2NeighborhoodExplorer(const VRP_Input & pin, StateManager<VRP_Input,VRP_State>& psm)  
    : NeighborhoodExplorer<VRP_Input,VRP_State,VRP_Move2>(pin, psm, "VRP_Move2NeighborhoodExplorer") {} 
  void RandomMove(const VRP_State& st, VRP_Move2& mv) const;          
  bool FeasibleMove(const VRP_State& st, const VRP_Move2& mv) const;  
  void MakeMove(VRP_State& st, const VRP_Move2& mv) const;             
  void FirstMove(const VRP_State& st, VRP_Move2& mv) const;  
  bool NextMove(const VRP_State& st, VRP_Move2& mv) const;   
protected:
  bool AnyNextMove(const VRP_State& st, VRP_Move2& mv) const;
};


class VRP_MoveDeltaDistanceCost2
  : public DeltaCostComponent<VRP_Input,VRP_State,VRP_Move2>
{
public:
  VRP_MoveDeltaDistanceCost2(const VRP_Input & in, VRP_DistanceCost& cc) 
    : DeltaCostComponent<VRP_Input,VRP_State,VRP_Move2>(in,cc,"VRP_MoveDeltaDistanceCost2") 
  {}
  double ComputeDeltaCost(const VRP_State& st, const VRP_Move2& mv) const;
};


class VRP_MoveDeltaPremi2
  : public DeltaCostComponent<VRP_Input,VRP_State,VRP_Move2>
{
public:
  VRP_MoveDeltaPremi2(const VRP_Input & in, VRP_Premi& cc) 
    : DeltaCostComponent<VRP_Input,VRP_State,VRP_Move2>(in,cc,"VRP_MoveDeltaPremi2") 
  {}
  double ComputeDeltaCost(const VRP_State& st, const VRP_Move2& mv) const;
};

/***************************************************************************
 * Output Manager:
 ***************************************************************************/
class VRP_OutputManager
  : public OutputManager<VRP_Input,VRP_Output,VRP_State> 
{
public:
  VRP_OutputManager(const VRP_Input & pin)
    : OutputManager<VRP_Input,VRP_Output,VRP_State>(pin,"VRP_OutputManager") {}
  void InputState(VRP_State&, const VRP_Output&) const;  
  void OutputState(const VRP_State&, VRP_Output&) const; 
}; 
#endif


