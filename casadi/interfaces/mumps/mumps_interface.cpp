
/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010-2014 Joel Andersson, Joris Gillis, Moritz Diehl,
 *                            K.U. Leuven. All rights reserved.
 *    Copyright (C) 2011-2014 Greg Horn
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include "mumps_interface.hpp"
#include "casadi/core/global_options.hpp"

using namespace std;
namespace casadi {

  extern "C"
  int CASADI_LINSOL_MUMPS_EXPORT
  casadi_register_linsol_mumps(LinsolInternal::Plugin* plugin) {
    plugin->creator = MumpsInterface::creator;
    plugin->name = "mumps";
    plugin->doc = MumpsInterface::meta_doc.c_str();
    plugin->version = CASADI_VERSION;
    plugin->options = &MumpsInterface::options_;
    plugin->deserialize = &MumpsInterface::deserialize;
    return 0;
  }

  extern "C"
  void CASADI_LINSOL_MUMPS_EXPORT casadi_load_linsol_mumps() {
    LinsolInternal::registerPlugin(casadi_register_linsol_mumps);
  }

  MumpsInterface::MumpsInterface(const std::string& name, const Sparsity& sp)
    : LinsolInternal(name, sp) {
  }

  MumpsInterface::~MumpsInterface() {
    clear_mem();
  }

  void MumpsInterface::init(const Dict& opts) {
    // Call the init method of the base class
    LinsolInternal::init(opts);

  }

  int MumpsInterface::init_mem(void* mem) const {
    if (LinsolInternal::init_mem(mem)) return 1;
    auto m = static_cast<MumpsMemory*>(mem);
    // Free existing MUMPS instance, if any
    if (m->id) {
      // Terminate the instance of the package
      m->id->job = -2;
      dmumps_c(m->id);
      // Delete the memory structure
      delete m->id;
    }
    m->id = new DMUMPS_STRUC_C();

    // Initialize MUMPS
    m->id->job = -1;  // initializes an instance of the package
    m->id->par = 1;
    m->id->sym = 0;
    m->id->comm_fortran = -987654;
    dmumps_c(m->id);

    // Sparsity pattern in MUMPS format
    casadi_int n = this->nrow();
    casadi_int nnz = this->nnz();
    m->irn.clear();
    m->jcn.clear();
    m->irn.reserve(nnz);
    m->jcn.reserve(nnz);
    const casadi_int* colind = this->colind();
    const casadi_int* row = this->row();
    for (casadi_int c = 0; c < n; ++c) {
      for (casadi_int k = colind[c]; k < colind[c + 1]; ++k) {
        m->irn.push_back(row[k] + 1);
        m->jcn.push_back(c + 1);
      }
    }

    return 0;
  }

  int MumpsInterface::nfact(void* mem, const double* A) const {
    auto m = static_cast<MumpsMemory*>(mem);
    casadi_assert_dev(A!=nullptr);

    return 0;
  }

  int MumpsInterface::solve(void* mem, const double* A, double* x, casadi_int nrhs, bool tr) const {
    auto m = static_cast<MumpsMemory*>(mem);

    if (tr) casadi_error("not implemented");
    if (nrhs != 1) casadi_error("not implemented");

    // Define problem
    m->id->n = this->nrow();
    m->id->nnz = m->irn.size();
    m->id->irn = get_ptr(m->irn);
    m->id->jcn = get_ptr(m->jcn);
    m->id->a = const_cast<double*>(A);
    m->id->rhs = x;

    // Macro such that indices match documentation
    #define ICNTL(I) icntl[(I) - 1]

    // No outputs
    m->id->ICNTL(1) = -1;
    m->id->ICNTL(2) = -1;
    m->id->ICNTL(3) = -1;
    m->id->ICNTL(4) = 0;

    // Call mumps
    m->id->job = 6;
    dmumps_c(m->id);

    return 0;
  }

  MumpsMemory::MumpsMemory() {
    this->id = 0;
  }

  MumpsMemory::~MumpsMemory() {
    if (this->id) {
      // Terminate the instance of the package
      this->id->job = -2;
      dmumps_c(this->id);
      // Delete the structure
      delete this->id;
    }
  }

} // namespace casadi
