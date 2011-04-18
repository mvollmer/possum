/*  possum - synchronous data flow simulations
*/

#ifndef POSSUM_SIM_H
#define POSSUM_SIM_H

#include <Python.h>

#include <complex>
#include <stdarg.h>
#include <assert.h>
#include <stddef.h>
#include <limits.h>

typedef unsigned char sim_data;

typedef std::complex<double> sim_complex;
typedef int sim_int;

#define SIM_GENERIC(n) { (n) }

struct sim_generic {
  const char *name;
};

#define SIM_RESULT(n) { (n) }

struct sim_result {
  const char *name;
};

#define SIM_PORT(n,t,s)            { (n), #t, false, sizeof(t), (s) }
#define SIM_MULTI_PORT(n,t,s)      { (n), #t, true, sizeof(t), (s) }

#define SIM_COMPLEX_PORT(n,s)       SIM_PORT((n), sim_complex, (s))
#define SIM_MULTI_COMPLEX_PORT(n,s) SIM_MULTI_PORT((n), sim_complex, (s))
#define SIM_INT_PORT(n,s)           SIM_PORT((n), sim_int, (s))
#define SIM_MULTI_INT_PORT(n,s)     SIM_MULTI_PORT((n), sim_int, (s))

struct sim_port {
  const char *name;
  const char *type;
  bool multi;
  size_t type_size;
  size_t initial_chunk;
};

/* A sim_block_info is meant to be a statically initialized structure
   (with block == NULL) and sim_block is a more elaborate structure
   that is derived from it during sim_comp::set_block_info.
   SIM_BLOCK_INFO_VERSION applies to the layout of sim_block_info,
   sim_generic, sim_result and sim_port. */

#define SIM_BLOCK_INFO_VERSION 0

struct sim_block {
  const char *name;

  int n_generics;
  sim_generic *generics;

  int n_results;
  sim_result *results;

  int n_inputs;
  sim_port *inputs;

  int n_outputs;
  sim_port *outputs;
};

struct sim_block_info {
  int version;                // must be SIM_BLOCK_INFO_VERSION
  sim_block *block;           // must be NULL
  const char *name;
  sim_generic *generics;
  sim_result *results;
  sim_port *inputs;
  sim_port *outputs;
};

struct sim_comp {

  sim_comp ();
  virtual ~sim_comp ();

  // To be used by possum

  void set_name (char *name);
  void set_block_info (sim_block_info *info);
  void set_gvals (PyObject *vec);
  void set_rvec (PyObject *vec);

  void set_n_in (int n);
  void set_n_out (int n);

  PyObject *call_init ();

  // General information functions

  const char *get_name () { return name; }
  int get_n_in ()         { return n_in; }
  int get_n_out ()        { return n_out; }

  // This is called after the interface of the block has been
  // established in BLOCK.  Ignore for ordinary blocks.

  virtual void check ();

  // To be used by CHECK

  void check_error (const char *fmt, ...);
  void check_verror (const char *fmt, va_list ap);
  int check_errors;

  void check_homogenous_types (const char *in_type, const char *out_type);

  // This gets called at block instantiation time.  You should
  // overwrite it.

  virtual void init ();

  // To be used by INIT.

  bool get (bool &var, const char *name, bool def);
  void get (bool &var, const char *name);
  bool get (int &var, const char *name, int def);
  void get (int &var, const char *name);
  bool get (unsigned int &var, const char *name, unsigned int def);
  void get (unsigned int &var, const char *name);
  bool get (long &var, const char *name, long def);
  void get (long &var, const char *name);
  bool get (unsigned long &var, const char *name, unsigned long def);
  void get (unsigned long &var, const char *name);
  bool get (long long &var, const char *name, long long def);
  void get (long long &var, const char *name);
  bool get (unsigned long long &var, const char *name, unsigned long long def);
  void get (unsigned long long &var, const char *name);
  bool get (double &var, const char *name, double def);
  void get (double &var, const char *name);
  bool get (sim_complex &var, const char *name, sim_complex def);
  void get (sim_complex &var, const char *name);
  bool get (const char *&var, const char *name, const char *def);
  void get (const char *&var, const char *name);
  bool get (int *&var, int &len, const char *name, int *def,
	    int def_len);
  void get (int *&var, int &len, const char *name);
  bool get (double *&var, int &len, const char *name, double *def,
	    int def_len);
  void get (double *&var, int &len, const char *name);
  bool get (sim_complex *&var, int &len, const char *name, sim_complex *def,
	    int def_len);
  void get (sim_complex *&var, int &len, const char *name);

  bool get (PyObject *&var, const char *name, PyObject *def);
  void get (PyObject *&var, const char *name);

  void set_in_chunk (int input_id, size_t chunk);
  void set_out_chunk (int output_id, size_t chunk);

  void error (const char *fmt, ...);
  void verror (const char *fmt, va_list ap);

  // This is the process.  Overwrite it with your signal processing
  // code.

  virtual void step (int count, const sim_data **in, sim_data **out);
  virtual void step (const sim_data **in, sim_data **out);

  // Raise the finished flag and cause the exit predicate of the
  // simulation to be evaluated.

  void finish ();

  // This is called as the epilog.  Overwrite it to set results, for
  // example.

  virtual void epilog ();

  // To be used by EPILOG

  void set (const char *name, int val);
  void set (const char *name, double val);
  void set (const char *name, sim_complex val);

  // Internals

  void tick (int count);

  PyObject *get_gval (int index);
  int genindex (const char *name);

  bool get_rval (const char *name, PyObject *&var);
  void set_rval (const char *name, PyObject *val);
  virtual void mark_scms ();

  sim_block *block;
  char *name;

  bool finished, finished_trigger;

  PyObject *gvec;
  PyObject *rvec;

  struct deleteme {
    void *ptr;
    deleteme *next;
  } *deletables;

  void register_for_delete (void *ptr);
  
  struct connection {
    size_t size;          // in sizeof(sim_data)
    size_t type_size;     // in sizeof(sim_data)
    sim_data *buf, *buf_end, *ptr;

    connection ()
      : size(0), buf(NULL)
    {
    }

    void advance_ptr (int count)
    {
      ptr += count*size;
      if (ptr >= buf_end)
	ptr = buf;
    }

    void make_writer (size_t bufsize);
    void make_reader (sim_comp *src, int src_id, size_t bufsize, size_t delay);
  };

  int n_in;
  connection *in_connects;
  sim_data **in_data;

  int n_out;
  connection *out_connects;
  sim_data **out_data;

  void step_not_redefined ();
};

struct sim_complex_comp : sim_comp {

  virtual void check ();

  virtual void step (int count, const sim_complex **in, sim_complex **out);
  virtual void step (const sim_complex **in, sim_complex **out);

private:
  void step (int count, const sim_data **in, sim_data **out);
  void step (const sim_data **in, sim_data **out);
};

struct sim_complex_int_comp : sim_comp {

  virtual void check ();
  
  virtual void step (int count, const sim_complex **in, sim_int **out);
  virtual void step (const sim_complex **in, sim_int **out);
  
private:
  void step (int count, const sim_data **in, sim_data **out);
  void step (const sim_data **in, sim_data **out);
};

struct sim_int_complex_comp : sim_comp {

  virtual void check ();

  virtual void step (int count, const sim_int **in, sim_complex **out);
  virtual void step (const sim_int **in, sim_complex **out);

private:
  void step (int count, const sim_data **in, sim_data **out);
  void step (const sim_data **in, sim_data **out);
};

struct sim_int_comp : sim_comp {

  virtual void check ();

  virtual void step (int count, const sim_int **in, sim_int **out);
  virtual void step (const sim_int **in, sim_int **out);

private:
  void step (int count, const sim_data **in, sim_data **out);
  void step (const sim_data **in, sim_data **out);
};

enum sim_block_op {
  simbo_prototype,     // void -> char *
  simbo_create,        // void -> sim_comp *

  sim_n_block_ops
};

char *sim_block_prototype (sim_block_info *info);
typedef void *sim_block_dispatcher_t (sim_block_op op, ...);
extern "C" sim_block_dispatcher_t sim_plugin_block_dispatcher;

#define SIM_DECLARE_BLOCK(structname,generics,results,inputs,outputs) \
void *                                                               \
sim_plugin_block_dispatcher (sim_block_op op, ...)                  \
{                                                                  \
  static sim_block_info info = {                                  \
    SIM_BLOCK_INFO_VERSION,                                      \
    NULL,                                                       \
    #structname,                                               \
    generics,                                                 \
    results,                                                  \
    inputs,                                                   \
    outputs                                                   \
  };                                                          \
  switch (op)                                                 \
   {                                                          \
   case simbo_prototype:                                      \
     return sim_block_prototype (&info);                      \
   case simbo_create:                                         \
     {                                                        \
       sim_comp *sc = new structname; 		              \
       sc->set_block_info (&info);			      \
       return sc;                                             \
     }                                                        \
   default:                                                   \
     return NULL;                                             \
   }                                                          \
}

#endif /* POSSUM_SIM_H */
