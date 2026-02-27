#ifndef _CALLBACK_H
#define _CALLBACK_H

class Mono_Solver;
class Solver_Callback
{
public:
   virtual void process_result(const Mono_Solver& solver) = 0;
   virtual ~Solver_Callback() {}
};

class Ngraph_File_Creator;
class Ngraph_File_Creator_Callback
{
public:
   virtual void process_progress(const Ngraph_File_Creator& creator) = 0;
   virtual ~Ngraph_File_Creator_Callback() {}
};
#endif