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


#include "io_scheme_internal.hpp"
#include <map>
using namespace std;

namespace casadi {

    IOSchemeCustomInternal::IOSchemeCustomInternal(const std::vector<std::string> &entries,
                                                   const std::vector<std::string> &descriptions) :
        entries_(entries), descriptions_(descriptions)  {
      if (descriptions_.empty())
        descriptions_.resize(entries.size());
      casadi_assert(descriptions_.size()==entries.size());
    }

    std::string IOSchemeCustomInternal::name() const {
      return "customIO";
    }

    std::string IOSchemeCustomInternal::entryNames() const {
      std::stringstream ss;
      for (int i=0;i<entries_.size();++i) {
         if (i!=0) ss << ", ";
         ss << entries_[i];
      }
      return ss.str();
    }

    std::string IOSchemeCustomInternal::entry(int i) const {
      casadi_assert_message(i>=0 && i<entries_.size(),
                            "customIO::entry(): requesting entry for index " << i
                            << ", but IOScheme is only length " << entries_.size());
      return entries_[i];
    }

    std::string IOSchemeCustomInternal::entryEnum(int i) const {
      return "";
    }

    std::string IOSchemeCustomInternal::describe(int i) const {
      if (descriptions_[i].empty()) {
        return entry(i);
      }  else {
        std::stringstream ss;
        ss << entry(i) <<  " '" << descriptions_[i] << "'";
        return ss.str();
      }
    }

    int IOSchemeCustomInternal::index(const std::string &name) const {
      for (vector<string>::const_iterator i=entries_.begin(); i!=entries_.end(); ++i) {
        if (name==*i) return i-entries_.begin();
      }
      casadi_error("customIO::index(): entry '" << name
                   << "' not available. Available entries are " << entryNames());
      return -1;
    }

    int IOSchemeCustomInternal::size() const {
      return entries_.size();
    }

    void IOSchemeCustomInternal::print(std::ostream &stream) const {
      stream << "customIO(" << entryNames() << ")";
    }

    void IOSchemeCustomInternal::repr(std::ostream &stream) const {
      stream << "customIO(" << entryNames() << ")";
    }

} // namespace casadi
